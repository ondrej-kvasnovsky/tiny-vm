// src/function/function.h

#ifndef TINY_VM_FUNCTION_H
#define TINY_VM_FUNCTION_H

#include "../types.h"

// Function management
void load_function(VM* vm, const char** code);
Function* find_function(const VM* vm, const char* name);

#endif
