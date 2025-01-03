// src/memory/memory.c
#include "memory.h"
#include "../thread/thread.h"
#include "../utils/logger.h"

#include <stdlib.h>
#include <string.h>

LocalScope* create_local_scope() {
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

Variable* get_variable(ThreadContext* thread, const char* name) {
    LocalScope* local_scope = thread->local_scope;
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
    pthread_mutex_lock(&thread->vm->heap_mgmt_lock);
    if (thread->vm->heap_size < thread->vm->heap_capacity) {
        Variable* var = &thread->vm->heap[thread->vm->heap_size++];
        var->name = strdup(name);
        var->value = 0;  // Initialize as int by default
        pthread_mutex_unlock(&thread->vm->heap_mgmt_lock);
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