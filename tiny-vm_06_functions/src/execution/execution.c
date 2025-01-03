// src/execution/execution.c

#include "execution.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../types.h"
#include "../instruction/instruction.h"
#include "../thread/thread.h"
#include "../function/function.h"
#include "../utils/logger.h"
#include "../memory/memory.h"
#include "../synchronization/synchronization.h"

void execute_instruction(ThreadContext* thread, Instruction* instr) {
    switch (instr->type) {
        case PRINT: {
            const t_int value = get_value(thread, instr->args[0]);
            print("[Thread %d] PRINT Variable %s = %d", thread->thread_id, instr->args[0], value);
            break;
        }
        case SET: {
            Variable* var = get_variable(thread, instr->args[0]);
            if (var) {
                var->value = atoi(instr->args[1]);
            }
            break;
        }
        case ADD: {
            const t_int val1 = get_value(thread, instr->args[1]);
            const t_int val2 = get_value(thread, instr->args[2]);
            Variable* target = get_variable(thread, instr->args[0]);
            if (target) {
                target->value = val1 + val2;
            }
            print("[Thread %d] ADD %d + %d = %d", thread->thread_id, val1, val2, target->value);
            break;
        }
        case SLEEP: {
            usleep(atoi(instr->args[0]) * 1000);
            break;
        }
        case THREAD: {
            const int start_line = atoi(instr->args[0]);
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
        case LOCK: {
            SynchronizationLock* mutex = get_sync_lock(thread->vm, instr->args[0]);
            if (mutex) {
                print("[Thread %d] Waiting for lock '%s' at address %p",
                    thread->thread_id,
                    mutex->name,
                    (void*)&mutex->mutex
                );
                pthread_mutex_lock(&mutex->mutex);
                mutex->locked = 1;
                print("[Thread %d] Acquired lock '%s'", thread->thread_id, mutex->name);
            }
            break;
        }
        case UNLOCK: {
            SynchronizationLock* mutex = get_sync_lock(thread->vm, instr->args[0]);
            if (mutex && mutex->locked) {
                pthread_mutex_unlock(&mutex->mutex);
                mutex->locked = 0;
                print("[Thread %d] Released lock '%s' at address %p",
                    thread->thread_id,
                    mutex->name,
                    (void*)&mutex->mutex
                );
            }
            break;
        }
        case SYNC: {
            const Function* function = find_function(thread->vm, instr->args[0]);
            if (function) {
                sync_function(thread, function);
            } else {
                print("[Thread %d] Error: Function '%s' not found", thread->thread_id, instr->args[0]);
            }
            break;
        }
        case ASYNC: {
            const Function* function = find_function(thread->vm, instr->args[0]);
            if (function) {
                async_function(thread->vm, function);
            } else {
                print("[Thread %d] Error: Function '%s' not found", thread->thread_id, instr->args[0]);
            }
            break;
        }
        case FUNCTION: {
            // Function declaration is handled during loading
            // No runtime execution needed
            break;
        }
        default: print("[Thread %d] Error: Unknown instruction type: %d", thread->thread_id, instr->type);
    }
}

void sync_function(ThreadContext* caller, const Function* function) {
    // Save caller's context
    const int original_pc = caller->pc;
    const char** original_program = caller->program;
    char* original_function_name = caller->function_name;

    // Set up function context
    caller->program = function->code;
    caller->pc = 0;
    caller->function_name = strdup(function->name);

    print("[Thread %d] Sync function '%s' started", caller->thread_id, function->name);

    // Execute function instructions
    while (caller->is_running && caller->program[caller->pc] != NULL) {
        Instruction instr = parse_instruction(caller->program[caller->pc]);
        execute_instruction(caller, &instr);
        caller->pc++;
    }

    // Restore caller's context
    caller->program = original_program;
    caller->pc = original_pc;
    free(caller->function_name);
    caller->function_name = original_function_name;
}

ThreadContext* async_function(VM* vm, const Function* function) {
    pthread_mutex_lock(&vm->thread_mgmt_lock);

    if (vm->thread_count >= vm->thread_capacity) {
        pthread_mutex_unlock(&vm->thread_mgmt_lock);
        return NULL;
    }

    ThreadContext* thread = &vm->threads[vm->thread_count++];
    thread->local_scope = create_local_scope();
    thread->program = function->code;
    thread->pc = 0;
    thread->is_running = 1;
    thread->thread_id = vm->next_thread_id++;
    thread->vm = vm;
    thread->function_name = strdup(function->name);

    print("[Thread %d] Async function '%s' started ", thread->thread_id, function->name);
    pthread_create(&thread->thread, NULL, execute_thread_instructions, thread);

    pthread_mutex_unlock(&vm->thread_mgmt_lock);
    return thread;
}