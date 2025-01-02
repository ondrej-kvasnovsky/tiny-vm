// src/main.c
#include "utils/logger.h"
#include "core/vm.h"

#include <stdio.h>

int main(void) {
    const char* program[] = {
        "set a 10",
        "print a",
        NULL
     };

    print("Starting TinyVM...");

    VM* vm = create_vm();
    start_vm(vm, program);
    destroy_vm(vm);

    print("TinyVM finished.");
    return 0;
}