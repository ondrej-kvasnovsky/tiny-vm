// src/thread/thread.c
#include "thread.h"
#include "../core/vm.h"
#include "../execution/execution.h"
#include "../utils/logger.h"
#include "../memory/memory.h"

ThreadContext* create_thread(VM* vm, const Function* function) {
    pthread_mutex_lock(&vm->thread_mgmt_lock);

    if (vm->thread_count >= vm->thread_capacity) {
        pthread_mutex_unlock(&vm->thread_mgmt_lock);
        return NULL;
    }

    ThreadContext* thread = &vm->threads[vm->thread_count++];
    thread->local_scope = create_local_scope();
    thread->current_function = function;

    thread->pc = 0;
    thread->is_running = 1;
    thread->thread_id = vm->next_thread_id++;  // Assign and increment thread ID
    thread->vm = vm;

    pthread_create(&thread->thread, NULL, execute_thread_instructions, thread);

    pthread_mutex_unlock(&vm->thread_mgmt_lock);
    return thread;
}

void* execute_thread_instructions(void* arg) {
    ThreadContext* thread = (ThreadContext*) arg;
    print("[Thread %d] Thread instructions started", thread->thread_id);

    while (thread->is_running && thread->pc < thread->current_function->code_length) {
        execute_bytecode(thread);
        thread->pc++;
    }

    print("[Thread %d] Thread instructions finished", thread->thread_id);
    return NULL;
}
