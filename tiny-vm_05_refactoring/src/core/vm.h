// src/core/vm.h
#ifndef TINY_VM_CORE_VM_H
#define TINY_VM_CORE_VM_H

#include "../types.h"

// Core VM functions
VM* create_vm(void);
void start_vm(VM* vm, const char** program);
void destroy_vm(VM* vm);

#endif
