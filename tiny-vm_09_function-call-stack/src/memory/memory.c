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

Variable* get_variable(const ThreadContext* thread, const char* name) {
    LocalScope* scope = thread->current_frame->local_scope;

    // Look for existing variable in current scope only
    for (int i = 0; i < scope->var_count; i++) {
        if (strcmp(scope->variables[i].name, name) == 0) {
            return &scope->variables[i];
        }
    }

    // Create new variable in current scope if not found
    if (scope->var_count < scope->var_capacity) {
        Variable* var = &scope->variables[scope->var_count++];
        var->name = strdup(name);
        var->value = 0;
        return var;
    }
    return NULL;
}

// Get a shared variable from heap
Variable* get_shared_variable(const ThreadContext* thread, const char* name) {

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

t_int get_value(const ThreadContext* thread, const char* name) {
    // First check current frame's local scope
    Variable* local = get_variable(thread, name);
    if (local) {
        return local->value;
    }

    // If not found in current scope, check heap for shared variables
    Variable* shared = get_shared_variable(thread, name);
    if (shared) {
        return shared->value;
    }
    return 0;
}