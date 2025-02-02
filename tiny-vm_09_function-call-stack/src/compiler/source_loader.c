// src/utils/source_loader.c
#include "source_loader.h"
#include "../utils/logger.h"
#include <stdlib.h>
#include <string.h>

char*** read_tvm_source(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        print("[File] Error: Could not open source file: %s", filename);
        return NULL;
    }

    char*** functions = malloc(sizeof(char**) * MAX_FUNCTIONS);
    int current_function = -1;
    char line[MAX_LINE_LENGTH];
    int line_count = 0;

    // Initialize first function
    functions[0] = malloc(sizeof(char*) * MAX_LINES_PER_FUNCTION);
    int current_line = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '/') {
            continue;
        }

        // Check if this is a function declaration
        if (strncmp(line, "function ", 9) == 0) {
            current_function++;
            if (current_function > 0) {
                // Null terminate previous function
                functions[current_function-1][current_line] = NULL;
            }
            functions[current_function] = malloc(sizeof(char*) * MAX_LINES_PER_FUNCTION);
            current_line = 0;
        }

        // Store the line
        functions[current_function][current_line++] = strdup(line);
        line_count++;
    }

    // Null terminate last function
    functions[current_function][current_line] = NULL;
    // Null terminate function array
    functions[current_function + 1] = NULL;

    fclose(file);
    print("[File] Successfully read %d lines from %s", line_count, filename);
    return functions;
}

void free_source_code(char*** source_code) {
    if (!source_code) return;

    for (int i = 0; source_code[i] != NULL; i++) {
        for (int j = 0; source_code[i][j] != NULL; j++) {
            free(source_code[i][j]);
        }
        free(source_code[i]);
    }
    free(source_code);
}

char* get_output_filename(const char* input_filename, const char* new_extension) {
    char* output = malloc(strlen(input_filename) + strlen(new_extension) + 1);
    strcpy(output, input_filename);

    // Find last dot
    char* dot = strrchr(output, '.');
    if (dot) {
        *dot = '\0';  // Remove old extension
    }

    // Add new extension
    strcat(output, new_extension);
    return output;
}