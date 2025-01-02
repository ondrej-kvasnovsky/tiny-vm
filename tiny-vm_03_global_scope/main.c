// main.c
#include <stdio.h>
#include "tiny_vm.h"

int main() {
    // Our "Java" program with threads
    const char* program[] = {
        "setshared counter 0", // Line 0
        "thread 6",            // Line 1 - Start increment thread
        "thread 12",           // Line 2 - Start decrement thread
        "sleep 500",           // Line 3 - Main thread waits for threads to finish
        "print counter",       // Line 4 - Print final value
        "exit",                // Line 5

        // Attempt to change counter directly from Thread-0
        "sleep 1",              // Line 6
        "setshared counter 1",  // Line 7
        "print counter",        // Line 8
        "setshared counter 1",  // Line 9
        "print counter",        // Line 10
        "exit",                 // Line 11

        // Attempt to change counter directly from Thread-1
        "sleep 1",             // Line 12
        "setshared counter 2", // Line 13
        "print counter",       // Line 14
        "setshared counter 2", // Line 15
        "print counter",       // Line 16
        "exit",                // Line 17
        NULL
     };

    print("Starting TinyVM with threads...");

    VM* vm = create_vm();
    start_vm(vm, program);
    destroy_vm(vm);

    print("TinyVM finished.");
    return 0;
}
