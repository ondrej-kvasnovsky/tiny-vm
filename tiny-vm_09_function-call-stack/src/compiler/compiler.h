// src/compiler/compiler.h
#ifndef TINY_VM_COMPILER_H
#define TINY_VM_COMPILER_H

#include <stdint.h>

// Bytecode operation codes
typedef enum {
    OP_NOP          = 0x00,    // No operation
    OP_PRINT        = 0x01,    // print variable
    OP_LOAD_CONST   = 0x02,    // Load constant into variable
    OP_ADD          = 0x03,    // Add two variables
    OP_SLEEP        = 0x04,    // Sleep for N milliseconds
    OP_SETSHARED    = 0x05,    // Set shared variable
    OP_MONITOR_ENTER = 0x06,   // Enter monitor (lock)
    OP_MONITOR_EXIT = 0x07,    // Exit monitor (unlock)
    OP_INVOKE_SYNC  = 0x08,    // Call function synchronously
    OP_INVOKE_ASYNC = 0x09,    // Call function asynchronously
    OP_RETURN       = 0x0A,    // Return from function
    OP_PRINT_STACK_TRACE = 0x0B, // Print current stack trace
} OpCode;

// Bytecode instruction format
typedef struct {
    OpCode opcode;         // Operation code
    uint16_t var_index;    // Variable index (if needed)
    uint16_t var_index2;   // Second variable index (if needed)
    uint16_t var_index3;   // Third variable index (if needed)
    int32_t constant;      // Constant value (if needed)
    char* name;            // Name (for variables/functions)
} BytecodeInstruction;

// Compiled function
typedef struct {
    char* name;                     // Function name
    BytecodeInstruction* byte_code; // Bytecode instructions
    int code_length;                // Number of instructions
    int max_locals;                 // Maximum number of local variables
    char** constant_pool;           // Pool of constant values/names
    int constant_pool_size;         // Size of constant pool
} Function;

// Compilation result
typedef struct {
    Function** functions;  // Array of compiled functions
    int function_count;    // Number of functions
} CompilationResult;

// Compiler functions
CompilationResult* compile_program(const char*** source_functions);
void free_compilation_result(CompilationResult* result);
void print_compilation_result(const CompilationResult* result);

// Bytecode file operations
CompilationResult* load_compiled_bytecode(const char* filename);
void save_compiled_bytecode(const char* filename, CompilationResult* compiled);

// Debug functions
void print_bytecode(const Function* function);

#endif