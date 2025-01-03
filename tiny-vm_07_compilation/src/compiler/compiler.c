// src/compiler/compiler.c
#include "compiler.h"
#include "../instruction/instruction.h"
#include "../utils/logger.h"

#include <stdlib.h>
#include <string.h>

static void compile_instruction(const char* line, BytecodeInstruction* bytecode, int* constant_index, char** constant_pool) {
    const Instruction instr = parse_instruction(line);

    switch (instr.type) {
        case PRINT:
            bytecode->opcode = OP_PRINT;
            bytecode->name = strdup(instr.args[0]);
            // Store variable name in constant pool
            constant_pool[*constant_index] = strdup(instr.args[0]);
            bytecode->var_index = (*constant_index)++;
            break;

        case SET:
            bytecode->opcode = OP_LOAD_CONST;
            bytecode->name = strdup(instr.args[0]);
            // Store variable name in constant pool
            constant_pool[*constant_index] = strdup(instr.args[0]);
            bytecode->var_index = (*constant_index)++;
            bytecode->constant = atoi(instr.args[1]);
            break;

        case ADD:
            bytecode->opcode = OP_ADD;
            // Store all variable names in constant pool
            constant_pool[*constant_index] = strdup(instr.args[0]);
            bytecode->var_index = (*constant_index)++;
            constant_pool[*constant_index] = strdup(instr.args[1]);
            bytecode->var_index2 = (*constant_index)++;
            constant_pool[*constant_index] = strdup(instr.args[2]);
            bytecode->var_index3 = (*constant_index)++;
            break;

        case SLEEP:
            bytecode->opcode = OP_SLEEP;
            bytecode->constant = atoi(instr.args[0]);
            break;

        case SETSHARED:
            bytecode->opcode = OP_SETSHARED;
            bytecode->name = strdup(instr.args[0]);
            bytecode->constant = atoi(instr.args[1]);
            break;

        case LOCK:
            bytecode->opcode = OP_MONITOR_ENTER;
            bytecode->name = strdup(instr.args[0]);
            break;

        case UNLOCK:
            bytecode->opcode = OP_MONITOR_EXIT;
            bytecode->name = strdup(instr.args[0]);
            break;

        case SYNC:
            bytecode->opcode = OP_INVOKE_SYNC;
            bytecode->name = strdup(instr.args[0]);
            break;

        case ASYNC:
            bytecode->opcode = OP_INVOKE_ASYNC;
            bytecode->name = strdup(instr.args[0]);
            break;

        case EXIT:
            bytecode->opcode = OP_RETURN;
            break;

        default:
            bytecode->opcode = OP_NOP;
    }
}

static Function* compile_function(const char** source) {
    // Create function structure
    Function* function = malloc(sizeof(Function));
    if (!function) return NULL;

    // Get function name (first line must be function declaration)
    function->name = get_function_name(source);
    if (!function->name) {
        free(function);
        return NULL;
    }

    // Count instructions (excluding NULL terminator and function declaration)
    function->code_length = 0;
    for (int i = 1; source[i] != NULL; i++) {
        function->code_length++;
    }

    // Allocate bytecode array
    function->byte_code = malloc(sizeof(BytecodeInstruction) * function->code_length);
    if (!function->byte_code) {
        free(function->name);
        free(function);
        return NULL;
    }

    // Allocate constant pool (worst case: 3 constants per instruction)
    function->constant_pool = malloc(sizeof(char*) * function->code_length * 3);
    if (!function->constant_pool) {
        free(function->byte_code);
        free(function->name);
        free(function);
        return NULL;
    }

    // Initialize constant pool size
    function->constant_pool_size = 0;

    // Compile each instruction
    for (int i = 0; i < function->code_length; i++) {
        compile_instruction(
            source[i + 1],
            &function->byte_code[i],
            &function->constant_pool_size,
            function->constant_pool
        );
    }

    return function;
}

CompilationResult* compile_program(const char*** source_functions) {
    CompilationResult* result = malloc(sizeof(CompilationResult));
    if (!result) return NULL;

    // Count functions
    result->function_count = 0;
    while (source_functions[result->function_count] != NULL) {
        result->function_count++;
    }

    result->functions = malloc(sizeof(Function*) * result->function_count);
    if (!result->functions) {
        free(result);
        return NULL;
    }

    // Compile each function
    for (int i = 0; i < result->function_count; i++) {
        result->functions[i] = compile_function(source_functions[i]);
        if (result->functions[i]) {
            print_bytecode(result->functions[i]);
        }
    }

    return result;
}

void print_compilation_result(const CompilationResult* result) {
    print("[Compiler] Compiled %d functions:", result->function_count);
    for (int i = 0; i < result->function_count; i++) {
        print_bytecode(result->functions[i]);
    }
}

void print_bytecode(const Function* function) {
    print("[Compiler] Bytecode for function '%s':", function->name);
    for (int i = 0; i < function->code_length; i++) {
        BytecodeInstruction* instr = &function->byte_code[i];
        switch (instr->opcode) {
            case OP_PRINT:
                print("  %d: PRINT %s", i, instr->name);
                break;
            case OP_LOAD_CONST:
                print("  %d: LOAD_CONST %s = %d", i, instr->name, instr->constant);
                break;
            case OP_ADD:
                print("  %d: ADD %s = %s + %s", i,
                      function->constant_pool[instr->var_index],
                      function->constant_pool[instr->var_index2],
                      function->constant_pool[instr->var_index3]);
                break;
            case OP_SLEEP:
                print("  %d: SLEEP %d", i, instr->constant);
                break;
            case OP_SETSHARED:
                print("  %d: SETSHARED %s = %d", i, instr->name, instr->constant);
                break;
            case OP_MONITOR_ENTER:
                print("  %d: MONITOR_ENTER %s", i, instr->name);
                break;
            case OP_MONITOR_EXIT:
                print("  %d: MONITOR_EXIT %s", i, instr->name);
                break;
            case OP_INVOKE_SYNC:
                print("  %d: INVOKE_SYNC %s", i, instr->name);
                break;
            case OP_INVOKE_ASYNC:
                print("  %d: INVOKE_ASYNC %s", i, instr->name);
                break;
            case OP_RETURN:
                print("  %d: RETURN", i);
                break;
            default:
                print("  %d: NOP", i);
        }
    }
}

void free_compilation_result(CompilationResult* result) {
    if (!result) return;

    for (int i = 0; i < result->function_count; i++) {
        Function* func = result->functions[i];
        free((void*)func->name);
        free((void*)func->byte_code);
        for (int j = 0; j < func->constant_pool_size; j++) {
            free((void*)func->constant_pool[j]);
        }
        free(func->constant_pool);
        free(func);
    }
    free(result->functions);
    free(result);

    print("[Compiler] Cleaned up compilation results");
}
