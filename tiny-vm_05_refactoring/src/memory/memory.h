// src/memory/memory.h
#ifndef TINY_VM_MEMORY_H
#define TINY_VM_MEMORY_H

#include "../types.h"

LocalScope* create_local_scope(void);
void destroy_local_scope(LocalScope* local_scope);

t_int get_value(ThreadContext* thread, const char* name);

Variable* get_variable(ThreadContext* thread, const char* name);
Variable* get_shared_variable(ThreadContext* thread, const char* name);

#endif
