// main.c
#include <stdio.h>
#include "tiny_vm.h"

int main() {
    // Our "Java" program
    const char* program[] = {
        "setshared counter 0", // Line 0
        "thread 6",            // Line 1 - Start Thread-1
        "thread 13",           // Line 2 - Start Thread-2
        "sleep 500",           // Line 3 - Thread-0 thread waits
        "print counter",       // Line 4 - Thread-0 prints final value
        "exit",                // Line 5

        // Thread-1: setting counter to 1
        "sleep 2",               // Line 6
        "lock counter_mutex",    // Line 7
        "setshared counter 1",   // Line 8
        "print counter",         // Line 9
        "setshared counter 1",   // Line 10
        "unlock counter_mutex",  // Line 11
        "exit",                  // Line 12

        // Thread-2: setting counter to 2
        "sleep 1",              // Line 13
        "lock counter_mutex",   // Line 14
        "setshared counter 2",  // Line 15
        "print counter",        // Line 16
        "setshared counter 2",  // Line 17
        "print counter",        // Line 18
        "unlock counter_mutex", // Line 19
        "exit",                 // Line 20
        NULL
     };
//    const char* program[] = {
//        // Thread-0: Starts two threads
//        "thread 3",       // Line 0 - Start Thread-1
//        "thread 9",       // Line 1 - Start Thread-2
//        "exit",           // Line 2
//
//        // Thread-1: Tries to acquire mutexA then mutexB
//        "lock mutexA",    // Line 3
//        "sleep 500",      // Line 4 - Wait for Thread-2 to acquire mutexB
//        "lock mutexB",    // Line 5 - Will deadlock here
//        "unlock mutexB",  // Line 6
//        "unlock mutexA",  // Line 7
//        "exit",           // Line 8
//
//        // Thread-2: Tries to acquire mutexB then mutexA
//        "sleep 250",      // Line 9 - Wait for Thread-1 to acquire mutexA
//        "lock mutexB",    // Line 10
//        "lock mutexA",    // Line 11 - Will deadlock here
//        "unlock mutexA",  // Line 12
//        "unlock mutexB",  // Line 13
//        "exit",           // Line 14
//        NULL
//    };

    printf("Starting TinyVM...\n");

    VM* vm = create_vm();
    start_vm(vm, program);
    destroy_vm(vm);

    printf("TinyVM finished.\n");
    return 0;
}
