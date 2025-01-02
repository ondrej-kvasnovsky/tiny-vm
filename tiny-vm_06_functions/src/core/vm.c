// src/core/vm.c
#include "vm.h"
#include "../execution/execution.h"
#include "../memory/memory.h"
#include "../function/function.h"
#include "../utils/logger.h"

#include <stdlib.h>

VM *create_vm(char** functions[]) {
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
    vm->function_capacity = 10;
    vm->function_count = 0;
    vm->functions = malloc(sizeof(Function) * vm->function_capacity);
    pthread_mutex_init(&vm->function_mgmt_lock, NULL);

    // Load functions
    for (int i = 0; functions[i] != NULL; i++) {
        load_function(vm, functions[i]);
    }

    return vm;
}

void run_vm(VM* vm) {
    // Find main function
    const Function* main_function = find_function(vm, "main");
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

    // Cleanup functions
    for (int i = 0; i < vm->function_count; i++) {
        free(vm->functions[i].name);
        // Note that we don't free function->code because it points to string literals in our case - they are stored in the program's static memory. If we later change to dynamically allocate the code arrays, we would need to free those as well.
        // free(vm->functions[i].code);
    }
    free(vm->functions);
    pthread_mutex_destroy(&vm->function_mgmt_lock);

    // Cleanup threads
    free(vm->threads);
    pthread_mutex_destroy(&vm->thread_mgmt_lock);

    free(vm);
}
