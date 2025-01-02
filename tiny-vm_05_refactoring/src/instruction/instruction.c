// src/instruction/instruction.c
#include "instruction.h"

#include "../thread/thread.h"
#include "../memory/memory.h"
#include "../synchronization/synchronization.h"
#include "../utils/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    else if (strcmp(cmd, "lock") == 0) {
        instr.type = LOCK;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "unlock") == 0) {
        instr.type = UNLOCK;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }

    return instr;
}

void execute_instruction(ThreadContext* thread, Instruction* instr) {
    switch (instr->type) {
        case PRINT: {
            const jint value = get_value(thread, instr->args[0]);
            print("[Thread %d] Variable %s = %d", thread->thread_id, instr->args[0], value);
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
            jint val1 = get_value(thread, instr->args[1]);
            jint val2 = get_value(thread, instr->args[2]);
            Variable* target = get_variable(thread, instr->args[0]);
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
    }
}

