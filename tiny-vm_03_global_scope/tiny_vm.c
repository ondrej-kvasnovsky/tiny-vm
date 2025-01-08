// tiny_jvm.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tiny_vm.h"

void print(const char *format, ...) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    time_t t = ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);

    printf("[%04d-%02d-%02d %02d:%02d:%02d.%06ld] ",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

LocalScope* create_local_scope(VM* vm) {
    LocalScope* local_scope = malloc(sizeof(LocalScope));
    local_scope->var_capacity = 10;
    local_scope->var_count = 0;
    local_scope->variables = malloc(sizeof(Variable) * local_scope->var_capacity);
    return local_scope;
}

void destroy_local_scope(LocalScope* local_scope) {
    for (int i = 0; i < local_scope->var_count; i++) {
        free(local_scope->variables[i].name);
    }
    free(local_scope->variables);
    free(local_scope);
}

VM* create_vm() {
    VM* vm = malloc(sizeof(VM));
    vm->thread_capacity = 10;
    vm->thread_count = 0;
    vm->threads = malloc(sizeof(ThreadContext) * vm->thread_capacity);
    vm->next_thread_id = 0;  // Initialize thread ID counter
    pthread_mutex_init(&vm->thread_lock, NULL);

    // Initialize heap
    vm->heap_capacity = 10;
    vm->heap_size = 0;
    vm->heap = malloc(sizeof(Variable) * vm->heap_capacity);
    pthread_mutex_init(&vm->heap_lock, NULL);

    return vm;
}

void destroy_vm(VM* vm) {
    // Cleanup local scopes
    for (int i = 0; i < vm->thread_count; i++) {
        if (vm->threads[i].local_scope) {
            destroy_local_scope(vm->threads[i].local_scope);
        }
    }

    // Cleanup heap
    for (int i = 0; i < vm->heap_size; i++) {
        free(vm->heap[i].name);
    }
    free(vm->heap);
    pthread_mutex_destroy(&vm->heap_lock);

    // Cleanup threads
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

// Get a shared variable from heap
Variable* get_shared_variable(ThreadContext* thread, const char* name) {

    // Look for existing variable
    for (int i = 0; i < thread->vm->heap_size; i++) {
        if (strcmp(thread->vm->heap[i].name, name) == 0) {
            print("[Thread %d] Found shared variable %s", thread->thread_id, name);
            return &thread->vm->heap[i];
        }
    }

    // Create a new variable if not found
    pthread_mutex_lock(&thread->vm->heap_lock);
    if (thread->vm->heap_size < thread->vm->heap_capacity) {
        Variable* var = &thread->vm->heap[thread->vm->heap_size++];
        var->name = strdup(name);
        var->value = 0;  // Initialize as int by default
        pthread_mutex_unlock(&thread->vm->heap_lock);
        print("[Thread %d] Created shared variable %s", thread->thread_id, name);
        return var;
    }

    return NULL;
}

t_int get_value(ThreadContext* thread, const char* name) {
    for (int i = 0; i < thread->local_scope->var_count; i++) {
        if (strcmp(thread->local_scope->variables[i].name, name) == 0) {
            return thread->local_scope->variables[i].value;
        }
    }
    Variable* shared = get_shared_variable(thread, name);
    if (shared) {
        return shared->value;
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
    else if (strcmp(cmd, "setshared") == 0) {
        instr.type = SETSHARED;
        sscanf(line, "%s %s %s", cmd, instr.args[0], instr.args[1]);
    }

    return instr;
}

void execute_instruction(ThreadContext* thread, Instruction* instr) {
    LocalScope* local_scope = thread->local_scope;

    switch (instr->type) {
        case PRINT: {
            t_int value = get_value(thread, instr->args[0]);
            print("[Thread %d] PRINT %s = %d", thread->thread_id, instr->args[0], value);
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
            t_int val1 = get_value(thread, instr->args[1]);
            t_int val2 = get_value(thread, instr->args[2]);
            Variable* target = get_variable(local_scope, instr->args[0]);
            if (target) {
                target->value = val1 + val2;
            }
            break;
        }
        case SLEEP: {
            usleep(atoi(instr->args[0]) );
            break;
        }
        case THREAD: {
            int start_line = atoi(instr->args[0]);
            create_thread(thread->vm, thread->program, start_line);
            break;
        }
        case EXIT: {
            thread->is_running = 0;
            break;
        }
        case SETSHARED: {
            Variable* var = get_shared_variable(thread, instr->args[0]);
            if (var) {
                var->value = atoi(instr->args[1]);
                print("[Thread %d] Set-shared %s = %d", thread->thread_id, var->name, var->value);
            }
            break;
        }
    }
}

void* execute_thread_instructions(void* arg) {
    ThreadContext* thread = (ThreadContext*)arg;
    print("[Thread %d] Thread started", thread->thread_id);

    while (thread->is_running) {
        const char* line = thread->program[thread->pc];
        if (line == NULL) break;

        Instruction instr = parse_instruction(line);
        execute_instruction(thread, &instr);

        thread->pc++;
    }

    print("[Thread %d] Thread finished", thread->thread_id);
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
    thread->vm = vm;  // Store reference to VM

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
