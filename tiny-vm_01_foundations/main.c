// main.c
#include <stdio.h>
#include "tiny_vm.h"

int main() {
    // Initialize local scope to store the variables in our program
    LocalScope* local_scope = create_local_scope();

    // Our "Java" program (each line will be executed in sequence)
    const char* program[] = {
        "set x 5",
        "set y 3",
        "add z x y",
        "print z",
        "sleep 1000",  // sleep 1 second
        "set z 42",
        "print z",
        NULL
    };

    // Execute the program
    printf("Starting TinyVM...\n-----------------\n");

    for (int i = 0; program[i] != NULL; i++) {
        printf("Executing: %s\n", program[i]);
        Instruction instr = parse_instruction(program[i]);
        execute_instruction(local_scope, &instr);
    }

    // Cleanup
    destroy_local_scope(local_scope);
    printf("_________________\nTinyVM finished.\n");

    return 0;
}
