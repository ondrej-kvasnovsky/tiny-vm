// tiny_jvm.h
#ifndef TINY_JVM_H
#define TINY_JVM_H

#include <stdint.h>
#include <pthread.h>

#include <stdio.h>
#include <stdarg.h> // provides functionality for functions with variable numbers of arguments (varargs)
#include <time.h>

typedef int32_t jint;

// Forward declaration for VM
typedef struct VM VM;

// Variable storage location
typedef enum {
    LOCAL,
    SHARED
} VariableType;

// Variable storage
typedef struct {
    char* name;
    jint value;
} Variable;

// Execution frame (stack frame)
typedef struct {
    Variable* variables;
    int var_count;
    int var_capacity;
} LocalScope;

// Thread context
typedef struct {
    LocalScope* local_scope; // Each thread has its own frame
    const char** program;  // Program this thread is executing
    int pc;               // Program counter
    pthread_t thread;     // Native thread handle
    int thread_id;        // Logical thread ID
    int is_running;       // Thread status
    VM* vm;  // Reference to VM
} ThreadContext;

// VM state
struct VM {
    ThreadContext* threads;
    int thread_count;
    int thread_capacity;
    pthread_mutex_t thread_lock;
    int next_thread_id;   // For generating thread IDs

    // Heap memory for shared variables
    Variable* heap;
    int heap_size;
    int heap_capacity;
    pthread_mutex_t heap_lock;  // Protect shared memory access
};

// Instruction types
typedef enum {
    PRINT,    // print <variable>
    SET,      // set <variable> <value>
    ADD,      // add <target> <var1> <var2>
    SLEEP,    // sleep <milliseconds>
    THREAD,   // thread <start_line>
    EXIT,      // exit
    SETSHARED,  // setshared <variable> <value>
} InstructionType;

typedef struct {
    InstructionType type;
    char args[3][32];
} Instruction;

// VM functions
VM* create_vm(void);
void start_vm(VM* vm, const char** program);
void destroy_vm(VM* vm);

ThreadContext* create_thread(VM* vm, const char** program, int start_line);

void print(const char *format, ...);

#endif
