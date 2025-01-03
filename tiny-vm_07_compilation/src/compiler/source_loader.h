// src/utils/source_loader.h
#ifndef TINY_VM_SOURCE_FILE_H
#define TINY_VM_SOURCE_FILE_H

#include <stdio.h>
#include "../compiler/compiler.h"

// Maximum line length for source files
#define MAX_LINE_LENGTH 1024
// Maximum number of functions per file
#define MAX_FUNCTIONS 100
// Maximum lines per function
#define MAX_LINES_PER_FUNCTION 100

// File related functions
char*** read_tvm_source(const char* filename);
void free_source_code(char*** source_code);

// Helper to get output filename
char* get_output_filename(const char* input_filename, const char* new_extension);

#endif