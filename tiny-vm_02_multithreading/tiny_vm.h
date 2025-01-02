// tiny_vm.h
#ifndef TINY_JVM_H
#define TINY_JVM_H

#include <stdint.h>
#include <pthread.h>

typedef int32_t jint;

// Declaration for VM
typedef struct VM VM;

// Variable storage
typedef struct {
    char* name;
    jint value;
} Variable;

// Thread execution scope (stack frame)
typedef struct {
    Variable* variables;
    int var_count;
    int var_capacity;
    VM* vm;  // Reference to VM
} LocalScope;

// Thread context
typedef struct {
    LocalScope* local_scope; // Each thread has its own local_scope
    const char** program;  // Program this thread is executing
    int pc;               // Program counter
    pthread_t thread;     // Native thread handle
    int thread_id;        // Logical thread ID
    int is_running;       // Thread status
} ThreadContext;

// VM state
struct VM {
    ThreadContext* threads;
    int thread_count;
    int thread_capacity;
    pthread_mutex_t thread_lock;
    int next_thread_id;   // For generating thread IDs
};

// Instruction types
typedef enum {
    PRINT,    // print <variable>
    SET,      // set <variable> <value>
    ADD,      // add <target> <var1> <var2>
    SLEEP,    // sleep <milliseconds>
    THREAD,   // thread <start_line>
    EXIT      // exit
} InstructionType;

typedef struct {
    InstructionType type;
    char args[3][32];
} Instruction;

// VM functions
VM* create_vm(void);
void destroy_vm(VM* vm);
void start_vm(VM* vm, const char** program);

ThreadContext* create_thread(VM* vm, const char** program, int start_line);

Instruction parse_instruction(const char* line);
void execute_instruction(VM* vm, ThreadContext* thread, Instruction* instr);

#endif
