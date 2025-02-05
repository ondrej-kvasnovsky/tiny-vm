// src/thread/thread.h
#ifndef TINY_VM_THREAD_H
#define TINY_VM_THREAD_H

#include "../types.h"

ThreadContext* create_thread(VM* vm, const Function* function);

void* execute_thread_instructions(void* arg);

#endif