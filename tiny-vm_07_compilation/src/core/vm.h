// src/core/vm.h
#ifndef TINY_VM_CORE_VM_H
#define TINY_VM_CORE_VM_H

#include "../types.h"

// Core VM functions
VM *create_vm(char** functions[]);

void run_vm(VM* vm);

void destroy_vm(VM *vm);

#endif
