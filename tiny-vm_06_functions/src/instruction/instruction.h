// src/instruction/instruction.h
#ifndef TINY_VM_INSTRUCTION_H
#define TINY_VM_INSTRUCTION_H

typedef enum {
    PRINT,    // print <variable>
    SET,      // set <variable> <value>
    ADD,      // add <target> <var1> <var2>
    SLEEP,    // sleep <milliseconds>
    THREAD,   // thread <start_line>
    EXIT,      // exit
    SETSHARED, // setshared <variable> <value>
    LOCK,      // lock <mutex_name>
    UNLOCK,     // unlock <mutex_name>
    SYNC,     // sync <function_name> - Execute function in current thread
    ASYNC,    // async <function_name> - Execute function in new thread
    FUNCTION,  // function <name> - Defines start of a function
} InstructionType;

typedef struct {
    InstructionType type;
    char args[3][32];
} Instruction;

Instruction parse_instruction(const char* line);
char* get_function_name(const char** code);

#endif
