// src/execution/stack.h
#ifndef TINY_VM_STACK_H
#define TINY_VM_STACK_H

#include "../types.h"

// Stack operations
StackFrame* push_stack_frame(ThreadContext* thread, const Function* function);
void pop_stack_frame(ThreadContext* thread);
Variable* get_variable_from_stack(ThreadContext* thread, const char* name);

void print_stack_trace(ThreadContext* thread);
void print_stack_frame(ThreadContext* thread, StackFrame* frame);

#endif