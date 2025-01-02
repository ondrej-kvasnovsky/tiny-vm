// src/instruction/instruction.h
#ifndef TINY_VM_INSTRUCTION_H
#define TINY_VM_INSTRUCTION_H

#include "../types.h"

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

Instruction parse_instruction(const char* line);

void execute_instruction(ThreadContext* thread, Instruction* instr);

#endif
