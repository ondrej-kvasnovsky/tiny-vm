// src/memory/memory.h
#ifndef TINY_VM_MEMORY_H
#define TINY_VM_MEMORY_H

#include "../types.h"

LocalScope* create_local_scope(void);
void destroy_local_scope(LocalScope* local_scope);

jint get_value(const ThreadContext* thread, const char* name);

Variable* get_variable(const ThreadContext* thread, const char* name);
Variable* get_shared_variable(const ThreadContext* thread, const char* name);

#endif
