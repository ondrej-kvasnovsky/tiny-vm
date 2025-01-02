// src/execution/execution.h

#ifndef TINY_VM_EXECUTION_H
#define TINY_VM_EXECUTION_H

#include "../types.h"
#include "../instruction/instruction.h"

void execute_instruction(ThreadContext* thread, Instruction* instr);

// synchronous and asynchronous function execution
void sync_function(ThreadContext* caller, const Function* function);
ThreadContext* async_function(VM* vm, const Function* function);

#endif
