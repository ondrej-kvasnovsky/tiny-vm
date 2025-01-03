// src/execution/execution.c

#include "execution.h"

#include <unistd.h>
#include <pthread.h>

#include "../types.h"
#include "../thread/thread.h"
#include "../function/function.h"
#include "../utils/logger.h"
#include "../memory/memory.h"
#include "../synchronization/synchronization.h"

void execute_bytecode(ThreadContext* thread) {
    const BytecodeInstruction* instr = &thread->current_function->byte_code[thread->pc];

    switch(instr->opcode) {
        case OP_NOP:
            break;

        case OP_PRINT: {
            const char* var_name = instr->name;
            const t_int value = get_value(thread, var_name);
            print("[Thread %d] PRINT %s = %d", thread->thread_id, var_name, value);
            break;
        }

        case OP_LOAD_CONST: {
            Variable* var = get_variable(thread, instr->name);
            if (var) {
                var->value = instr->constant;
                print("[Thread %d] SET %s = %d", thread->thread_id, var->name, var->value);
            }
            break;
        }

        case OP_ADD: {
            const char* target = thread->current_function->constant_pool[instr->var_index];
            const char* op1 = thread->current_function->constant_pool[instr->var_index2];
            const char* op2 = thread->current_function->constant_pool[instr->var_index3];

            const t_int val1 = get_value(thread, op1);
            const t_int val2 = get_value(thread, op2);
            Variable* target_var = get_variable(thread, target);

            if (target_var) {
                target_var->value = val1 + val2;
                print("[Thread %d] ADD %s = %s(%d) + %s(%d) = %d",
                      thread->thread_id, target, op1, val1, op2, val2, target_var->value);
            }
            break;
        }

        case OP_SLEEP: {
            usleep(instr->constant * 1000);
            break;
        }

        case OP_SETSHARED: {
            Variable* var = get_shared_variable(thread, instr->name);
            if (var) {
                var->value = instr->constant;
                print("[Thread %d] SETSHARED %s = %d", thread->thread_id, var->name, var->value);
            }
            break;
        }

        case OP_MONITOR_ENTER: {
            SynchronizationLock* mutex = get_sync_lock(thread->vm, instr->name);
            if (mutex) {
                print("[Thread %d] Waiting for lock '%s'", thread->thread_id, mutex->name);
                pthread_mutex_lock(&mutex->mutex);
                mutex->locked = 1;
                print("[Thread %d] Acquired lock '%s'", thread->thread_id, mutex->name);
            }
            break;
        }

        case OP_MONITOR_EXIT: {
            SynchronizationLock* mutex = get_sync_lock(thread->vm, instr->name);
            if (mutex && mutex->locked) {
                pthread_mutex_unlock(&mutex->mutex);
                mutex->locked = 0;
                print("[Thread %d] Released lock '%s'", thread->thread_id, mutex->name);
            }
            break;
        }

        case OP_INVOKE_SYNC: {
            const Function* function = find_function(thread->vm, instr->name);
            if (function) {
                sync_function(thread, function);
            } else {
                print("[Thread %d] Error: Function '%s' not found", thread->thread_id, instr->name);
            }
            break;
        }

        case OP_INVOKE_ASYNC: {
            const Function* function = find_function(thread->vm, instr->name);
            if (function) {
                async_function(thread->vm, function);
            } else {
                print("[Thread %d] Error: Function '%s' not found", thread->thread_id, instr->name);
            }
            break;
        }

        case OP_RETURN: {
            // this should only end the current function, on stack, but not kill a thread
            thread->is_running = 0;
            break;
        }

        default:
            print("[Thread %d] Error: Unknown opcode: %d", thread->thread_id, instr->opcode);
    }
}

void sync_function(ThreadContext* caller, const Function* function) {
    // Save caller's context
    const Function* original_function = caller->current_function;
    const int original_pc = caller->pc;

    // Set up function context
    caller->current_function = function;
    caller->pc = 0;
    // caller->is_running = 1;  // Ensure we're running

    print("[Thread %d] Sync function '%s' started", caller->thread_id, function->name);

    // Execute function instructions
    while (caller->is_running && caller->pc < function->code_length) {
        execute_bytecode(caller);
        caller->pc++;
    }

    caller->is_running = 1;  // Add this line

    // Restore caller's context
    caller->current_function = original_function;
    caller->pc = original_pc;
}

ThreadContext* async_function(VM* vm, const Function* function) {
    return create_thread(vm, function);
}
