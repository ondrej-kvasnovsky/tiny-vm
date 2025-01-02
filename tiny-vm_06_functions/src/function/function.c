// src/function/function.c

#include "function.h"

#include <stdio.h>

#include "../instruction/instruction.h"
#include "../utils/logger.h"
#include <string.h>
#include <stdlib.h>

void load_function(VM* vm, const char** code) {
    char* name = get_function_name(code);
    if (!name) {
        return;
    }

    pthread_mutex_lock(&vm->function_mgmt_lock);
    if (vm->function_count < vm->function_capacity) {
        Function* function = &vm->functions[vm->function_count++];
        function->name = name;
        function->code = code;
        print("[VM] Defined function: %s", name);
    } else {
        free(name);
        print("[VM] Error: Function capacity exceeded");
    }
    pthread_mutex_unlock(&vm->function_mgmt_lock);
}

Function* find_function(const VM* vm, const char* name) {
    Function* found = NULL;
    for (int i = 0; i < vm->function_count; i++) {
        if (strcmp(vm->functions[i].name, name) == 0) {
            found = &vm->functions[i];
            break;
        }
    }
    return found;
}
