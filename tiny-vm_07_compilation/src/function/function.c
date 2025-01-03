// src/function/function.c

#include <stdio.h>
#include <string.h>

#include "function.h"

Function* find_function(const VM* vm, const char* name) {
    Function* found = NULL;
    for (int i = 0; i < vm->function_count; i++) {
        if (strcmp(vm->functions[i]->name, name) == 0) {
            found = vm->functions[i];
            break;
        }
    }
    return found;
}
