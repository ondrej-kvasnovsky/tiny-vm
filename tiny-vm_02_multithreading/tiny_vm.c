// tiny_jvm.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tiny_vm.h"

VM* create_vm() {
    VM* vm = malloc(sizeof(VM));
    vm->thread_capacity = 10;
    vm->thread_count = 0;
    vm->threads = malloc(sizeof(ThreadContext) * vm->thread_capacity);
    vm->next_thread_id = 0;  // Initialize thread ID counter
    pthread_mutex_init(&vm->thread_lock, NULL);
    return vm;
}

LocalScope* create_local_scope(VM* vm) {
    LocalScope* local_scope = malloc(sizeof(LocalScope));
    local_scope->var_capacity = 10;
    local_scope->var_count = 0;
    local_scope->variables = malloc(sizeof(Variable) * local_scope->var_capacity);
    local_scope->vm = vm;  // Store reference to VM
    return local_scope;
}

void destroy_local_scope(LocalScope* local_scope) {
    for (int i = 0; i < local_scope->var_count; i++) {
        free(local_scope->variables[i].name);
    }
    free(local_scope->variables);
    free(local_scope);
}


void destroy_vm(VM* vm) {
    for (int i = 0; i < vm->thread_count; i++) {
        if (vm->threads[i].local_scope) {
            destroy_local_scope(vm->threads[i].local_scope);
        }
    }
    free(vm->threads);
    pthread_mutex_destroy(&vm->thread_lock);
    free(vm);
}

Variable* get_variable(LocalScope* local_scope, const char* name) {
    for (int i = 0; i < local_scope->var_count; i++) {
        if (strcmp(local_scope->variables[i].name, name) == 0) {
            return &local_scope->variables[i];
        }
    }

    if (local_scope->var_count < local_scope->var_capacity) {
        Variable* var = &local_scope->variables[local_scope->var_count++];
        var->name = strdup(name);
        var->value = 0;
        return var;
    }
    return NULL;
}

jint get_value(LocalScope* local_scope, const char* name) {
    for (int i = 0; i < local_scope->var_count; i++) {
        if (strcmp(local_scope->variables[i].name, name) == 0) {
            return local_scope->variables[i].value;
        }
    }
    return 0;
}

Instruction parse_instruction(const char* line) {
    Instruction instr;
    memset(&instr, 0, sizeof(Instruction));

    char cmd[32];
    sscanf(line, "%s", cmd);

    if (strcmp(cmd, "print") == 0) {
        instr.type = PRINT;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "set") == 0) {
        instr.type = SET;
        sscanf(line, "%s %s %s", cmd, instr.args[0], instr.args[1]);
    }
    else if (strcmp(cmd, "add") == 0) {
        instr.type = ADD;
        sscanf(line, "%s %s %s %s", cmd, instr.args[0], instr.args[1], instr.args[2]);
    }
    else if (strcmp(cmd, "sleep") == 0) {
        instr.type = SLEEP;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "thread") == 0) {
        instr.type = THREAD;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "exit") == 0) {
        instr.type = EXIT;
    }

    return instr;
}

void execute_instruction(VM* vm, ThreadContext* thread, Instruction* instr) {
    LocalScope* local_scope = thread->local_scope;

    switch (instr->type) {
        case PRINT: {
            jint value = get_value(local_scope, instr->args[0]);
            printf("[Thread %d] %d\n", thread->thread_id, value);
            break;
        }
        case SET: {
            Variable* var = get_variable(local_scope, instr->args[0]);
            if (var) {
                var->value = atoi(instr->args[1]);
            }
            break;
        }
        case ADD: {
            jint val1 = get_value(local_scope, instr->args[1]);
            jint val2 = get_value(local_scope, instr->args[2]);
            Variable* target = get_variable(local_scope, instr->args[0]);
            if (target) {
                target->value = val1 + val2;
            }
            break;
        }
        case SLEEP: {
            usleep(atoi(instr->args[0]) * 1000);
            break;
        }
        case THREAD: {
            int start_line = atoi(instr->args[0]);
            create_thread(vm, thread->program, start_line);
            break;
        }
        case EXIT: {
            thread->is_running = 0;
            printf("[Thread %d] Thread exiting\n", thread->thread_id);
            break;
        }
    }
}

void* execute_thread_instructions(void* arg) {
    ThreadContext* thread = (ThreadContext*)arg;
    printf("[Thread %d] Thread started\n", thread->thread_id);

    while (thread->is_running) {
        const char* line = thread->program[thread->pc];
        if (line == NULL) break;

        Instruction instr = parse_instruction(line);
        execute_instruction(thread->local_scope->vm, thread, &instr);

        thread->pc++;
    }

    printf("[Thread %d] Thread finished\n", thread->thread_id);
    return NULL;
}

ThreadContext* create_thread(VM* vm, const char** program, int start_line) {
    pthread_mutex_lock(&vm->thread_lock);

    if (vm->thread_count >= vm->thread_capacity) {
        pthread_mutex_unlock(&vm->thread_lock);
        return NULL;
    }

    ThreadContext* thread = &vm->threads[vm->thread_count++];
    thread->local_scope = create_local_scope(vm);
    thread->program = program;
    thread->pc = start_line;
    thread->is_running = 1;
    thread->thread_id = vm->next_thread_id++;  // Assign and increment thread ID

    pthread_create(&thread->thread, NULL, execute_thread_instructions, thread);

    pthread_mutex_unlock(&vm->thread_lock);
    return thread;
}

void start_vm(VM* vm, const char** program) {
    // Create main thread starting at line 0
    create_thread(vm, program, 0);

    // Wait for all threads to finish
    for (int i = 0; i < vm->thread_count; i++) {
        pthread_join(vm->threads[i].thread, NULL);
    }
}
