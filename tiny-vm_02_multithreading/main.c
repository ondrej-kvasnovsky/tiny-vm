// main.c
#include <stdio.h>
#include "tiny_vm.h"

int main() {
    // Our "Java" program with threads
    const char* program[] = {
        "set x 5",               // Line 0
        "thread 5",              // Line 1 - Start thread at line 5
        "print x",               // Line 2
        "sleep 5000",            // Line 3 - Sleep till the new thread finishes
        "exit",                  // Line 4
        "set y 10",              // Line 5 - Second thread starts here
        "print y",               // Line 6
        "sleep 1000",            // Line 7
        "exit",                  // Line 8
        NULL
    };

    printf("Starting TinyVM with threads...\n");

    VM* vm = create_vm();
    start_vm(vm, program);
    destroy_vm(vm);

    printf("TinyVM finished.\n");
    return 0;
}
