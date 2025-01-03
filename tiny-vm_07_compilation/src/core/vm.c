// src/core/vm.c
#include "vm.h"
#include "../execution/execution.h"
#include "../memory/memory.h"
#include "../function/function.h"
#include "../utils/logger.h"

#include <stdlib.h>

VM* create_vm(const CompilationResult* compiled) {
    VM *vm = malloc(sizeof(VM));

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

    // Initialize function management
    vm->function_capacity = 100;
    vm->function_count = 0;
    vm->functions = malloc(sizeof(Function) * vm->function_capacity);
    pthread_mutex_init(&vm->function_mgmt_lock, NULL);

    for (int i = 0; i < compiled->function_count; i++) {
        vm->functions[i] = compiled->functions[i];  // Shallow copy
        vm->function_count++;
    }

    return vm;
}

void run_vm(VM* vm) {
    // Find main function
    Function* main_function = find_function(vm, "main");
    if (!main_function) {
        print("[VM] Error: No main function defined");
        return;
    }

    // Start main in a new thread
    async_function(vm, main_function);

    // Wait for all threads to complete
    for (int i = 0; i < vm->thread_count; i++) {
        pthread_join(vm->threads[i].thread, NULL);
    }
}

void destroy_vm(VM *vm) {
    // Cleanup local scopes
    for (int i = 0; i < vm->thread_count; i++) {
        destroy_local_scope(vm->threads[i].local_scope);
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

    // Cleanup functions (pointers only)
    free(vm->functions);
    pthread_mutex_destroy(&vm->function_mgmt_lock);

    // Cleanup threads
    free(vm->threads);
    pthread_mutex_destroy(&vm->thread_mgmt_lock);

    free(vm);
}
