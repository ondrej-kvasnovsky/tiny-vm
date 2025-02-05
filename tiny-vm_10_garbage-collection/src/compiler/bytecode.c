// src/compiler/bytecode.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/logger.h"
#include "compiler.h"

// File operations for saving compiled bytecode
void save_compiled_bytecode(const char* filename, CompilationResult* compiled) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        print("[Compiler] Error: Could not open file for writing: %s", filename);
        return;
    }

    // Write number of functions
    fwrite(&compiled->function_count, sizeof(int), 1, file);

    // Write each function
    for (int i = 0; i < compiled->function_count; i++) {
        Function* func = compiled->functions[i];

        // Write function name length and name
        int name_len = strlen(func->name) + 1;
        fwrite(&name_len, sizeof(int), 1, file);
        fwrite(func->name, 1, name_len, file);

        // Write code length
        fwrite(&func->code_length, sizeof(int), 1, file);

        // Write each instruction
        for (int j = 0; j < func->code_length; j++) {
            const BytecodeInstruction* instr = &func->byte_code[j];

            // Write opcode and indexes
            fwrite(&instr->opcode, sizeof(OpCode), 1, file);
            fwrite(&instr->var_index, sizeof(uint16_t), 1, file);
            fwrite(&instr->var_index2, sizeof(uint16_t), 1, file);
            fwrite(&instr->var_index3, sizeof(uint16_t), 1, file);
            fwrite(&instr->constant, sizeof(int32_t), 1, file);

            // Write name if present
            int has_name = (instr->name != NULL);
            fwrite(&has_name, sizeof(int), 1, file);
            if (has_name) {
                int instr_name_len = strlen(instr->name) + 1;
                fwrite(&instr_name_len, sizeof(int), 1, file);
                fwrite(instr->name, 1, instr_name_len, file);
            }
        }

        // Write constant pool
        fwrite(&func->constant_pool_size, sizeof(int), 1, file);
        for (int j = 0; j < func->constant_pool_size; j++) {
            int const_len = strlen(func->constant_pool[j]) + 1;
            fwrite(&const_len, sizeof(int), 1, file);
            fwrite(func->constant_pool[j], 1, const_len, file);
        }
    }

    fclose(file);
    print("[Compiler] Successfully saved compiled bytecode to: %s", filename);
}


// File operations for loading compiled bytecode
CompilationResult* load_compiled_bytecode(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        print("[VM] Error: Could not open file: %s", filename);
        return NULL;
    }

    CompilationResult* result = malloc(sizeof(CompilationResult));

    // Read number of functions
    fread(&result->function_count, sizeof(int), 1, file);
    result->functions = malloc(sizeof(Function*) * result->function_count);

    // Read each function
    for (int i = 0; i < result->function_count; i++) {
        Function* func = malloc(sizeof(Function));
        result->functions[i] = func;

        // Read function name
        int name_len;
        fread(&name_len, sizeof(int), 1, file);
        func->name = malloc(name_len);
        fread(func->name, 1, name_len, file);

        // Read code length
        fread(&func->code_length, sizeof(int), 1, file);

        // Allocate and read bytecode
        func->byte_code = malloc(sizeof(BytecodeInstruction) * func->code_length);

        // Read each instruction
        for (int j = 0; j < func->code_length; j++) {
            BytecodeInstruction* instr = &func->byte_code[j];

            // Read opcode and indexes
            fread(&instr->opcode, sizeof(OpCode), 1, file);
            fread(&instr->var_index, sizeof(uint16_t), 1, file);
            fread(&instr->var_index2, sizeof(uint16_t), 1, file);
            fread(&instr->var_index3, sizeof(uint16_t), 1, file);
            fread(&instr->constant, sizeof(int32_t), 1, file);

            // Read name if present
            int has_name;
            fread(&has_name, sizeof(int), 1, file);
            if (has_name) {
                int instr_name_len;
                fread(&instr_name_len, sizeof(int), 1, file);
                instr->name = malloc(instr_name_len);
                fread(instr->name, 1, instr_name_len, file);
            } else {
                instr->name = NULL;
            }
        }

        // Read constant pool
        fread(&func->constant_pool_size, sizeof(int), 1, file);
        func->constant_pool = malloc(sizeof(char*) * func->constant_pool_size);

        for (int j = 0; j < func->constant_pool_size; j++) {
            int const_len;
            fread(&const_len, sizeof(int), 1, file);
            func->constant_pool[j] = malloc(const_len);
            fread(func->constant_pool[j], 1, const_len, file);
        }

        print("[VM] Loaded function '%s' with %d instructions", func->name, func->code_length);
    }

    fclose(file);
    print("[VM] Successfully loaded compiled bytecode from: %s", filename);
    return result;
}
