// src/execution/execution.h

#ifndef TINY_VM_EXECUTION_H
#define TINY_VM_EXECUTION_H

#include "../types.h"

void execute_bytecode(ThreadContext* thread);

// synchronous and asynchronous function execution
void sync_function(ThreadContext* caller, const Function* function);
ThreadContext* async_function(VM* vm, const Function* function);

#endif
