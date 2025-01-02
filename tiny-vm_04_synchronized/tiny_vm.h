// tiny_vm.h
#ifndef TINY_VM_H
#define TINY_VM_H

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h> // provides functionality for functions with variable numbers of arguments (varargs)

typedef int32_t jint;

// Forward declaration for VM
typedef struct VM VM;

// Mutex object for synchronization
typedef struct {
    char* name;
    pthread_mutex_t mutex;
    int locked;  // For debugging
} SynchronizationLock;

// Variable storage
typedef struct {
    char* name;
    jint value;
} Variable;



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
    pthread_mutex_t thread_mgmt_lock; // Protect thread management
    int next_thread_id;   // For generating thread IDs

    // Heap memory for shared variables
    Variable* heap;
    int heap_size;
    int heap_capacity;
    pthread_mutex_t heap_mngt_lock;  // Protect heap management

    // Mutex management
    SynchronizationLock* mutexes;
    int mutex_count;
    int mutex_capacity;
    pthread_mutex_t mutex_mgmt_lock; // Protect mutex management
};

// Instruction types
typedef enum {
    PRINT,    // print <variable>
    SET,      // set <variable> <value>
    ADD,      // add <target> <var1> <var2>
    SLEEP,    // sleep <milliseconds>
    THREAD,   // thread <start_line>
    EXIT,      // exit
    SETSHARED, // setshared <variable> <value>
    LOCK,      // lock <mutex_name>
    UNLOCK     // unlock <mutex_name>
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
void execute_instruction(ThreadContext* thread, Instruction* instr);

void print(const char *format, ...);

#endif
