// src/types.h
#ifndef TINY_VM_TYPES_H
#define TINY_VM_TYPES_H

#include <pthread.h>

typedef int32_t jint;

// Variable storage
typedef struct Variable {
    char* name;
    jint value;
} Variable;

// Execution frame (stack frame)
typedef struct LocalScope {
    Variable* variables;
    int var_count;
    int var_capacity;
} LocalScope;

typedef struct Function {
    char* name;
    const char** code;
} Function;

// Thread context
typedef struct ThreadContext {
    LocalScope* local_scope;
    const char** program;
    int pc;
    pthread_t thread;
    int thread_id;
    int is_running;
    struct VM* vm;
    char* function_name;
} ThreadContext;

// Synchronization
typedef struct SynchronizationLock {
    char* name;
    pthread_mutex_t mutex;
    int locked;  // For debugging
} SynchronizationLock;

// VM state
typedef struct VM {
    // Thread management
    ThreadContext* threads;
    int thread_count;
    int thread_capacity;
    pthread_mutex_t thread_mgmt_lock;
    int next_thread_id;

    // Heap memory for shared variables
    Variable* heap;
    int heap_size;
    int heap_capacity;
    pthread_mutex_t heap_mgmt_lock;

    // Mutex management
    SynchronizationLock* locks;
    int lock_count;
    int lock_capacity;
    pthread_mutex_t lock_mgmt_lock;

    // Function management
    Function* functions;
    int function_count;
    int function_capacity;
    pthread_mutex_t function_mgmt_lock;
} VM;

#endif
