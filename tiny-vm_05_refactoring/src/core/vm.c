// src/core/vm.c
#include "vm.h"
#include "../thread/thread.h"
#include "../memory/memory.h"

#include <stdlib.h>

VM* create_vm() {
    VM* vm = malloc(sizeof(VM));

    // Thread management
    vm->thread_capacity = 10;
    vm->thread_count = 0;
    vm->threads = malloc(sizeof(ThreadContext) * vm->thread_capacity);
    vm->next_thread_id = 0;
    pthread_mutex_init(&vm->thread_mgmt_lock, NULL);

    // Initialize heap
    vm->heap_capacity = 10;
    vm->heap_size = 0;
    vm->heap = malloc(sizeof(Variable) * vm->heap_capacity);
    pthread_mutex_init(&vm->heap_mgmt_lock, NULL);

    // Initialize synchronization/mutex management
    vm->lock_capacity = 10;
    vm->lock_count = 0;
    vm->locks = malloc(sizeof(SynchronizationLock) * vm->lock_capacity);
    pthread_mutex_init(&vm->lock_mgmt_lock, NULL);

    return vm;
}

void start_vm(VM* vm, const char** program) {
    // Create main thread starting at line 0
    create_thread(vm, program, 0);

    // Wait for all threads to finish
    for (int i = 0; i < vm->thread_count; i++) {
        pthread_join(vm->threads[i].thread, NULL);
    }
}


void destroy_vm(VM* vm) {
    for (int i = 0; i < vm->thread_count; i++) {
        //if (vm->threads[i].local_scope) {
            destroy_local_scope(vm->threads[i].local_scope);
        //}
    }

    // Cleanup heap
    for (int i = 0; i < vm->heap_size; i++) {
        free(vm->heap[i].name);
    }
    free(vm->heap);
    pthread_mutex_destroy(&vm->heap_mgmt_lock);

    // Cleanup mutexes
    for (int i = 0; i < vm->lock_count; i++) {
        pthread_mutex_destroy(&vm->locks[i].mutex);
        free(vm->locks[i].name);
    }
    pthread_mutex_destroy(&vm->lock_mgmt_lock);
    free(vm->locks);

    free(vm->threads);
    pthread_mutex_destroy(&vm->thread_mgmt_lock);
    free(vm);
}
