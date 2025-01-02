// tiny_vm.h
#ifndef TINY_VM_H
#define TINY_VM_H

#include <stdint.h>

// Our VM's data types
typedef int32_t jint;

typedef struct {
    char* name;
    jint value;
} Variable;

typedef struct {
    Variable* variables;
    int var_count;
    int var_capacity;
} LocalScope;

typedef enum {
    PRINT,    // print <variable>
    SET,      // set <variable> <value>
    ADD,      // add <target> <var1> <var2>
    SLEEP,    // sleep <milliseconds>
} InstructionType;

typedef struct {
    InstructionType type;
    char args[3][32];  // Max 3 arguments per instruction, each max 31 chars
} Instruction;

// Thread functions
LocalScope* create_local_scope(void);
void destroy_local_scope(LocalScope* local_scope);

// Instruction functions
Instruction parse_instruction(const char* line);
void execute_instruction(LocalScope* local_scope, Instruction* instr);

#endif

