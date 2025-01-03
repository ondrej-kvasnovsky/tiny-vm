// src/compiler_main.c
#include "utils/logger.h"
#include "compiler/compiler.h"
#include "compiler/source_loader.h"
#include <stdlib.h>

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        print("Usage: %s <source.tvm>", argv[0]);
        return 1;
    }

    print("[Compiler] Starting TinyVM Compiler...");

    // Read source file
    char*** source = read_tvm_source(argv[1]);
    if (!source) {
        print("[Compiler] Failed to read source file");
        return 1;
    }

    // Compile the program
    CompilationResult* compiled = compile_program((const char***)source);
    if (!compiled) {
        print("[Compiler] Compilation failed");
        free_source_code(source);
        return 1;
    }
    // print_compilation_result(compiled);

    // Get output filename
    char* output_file = get_output_filename(argv[1], ".tvmc");

    // Save the compiled bytecode
    save_compiled_bytecode(output_file, compiled);

    // Cleanup
    free(output_file);
    free_source_code(source);
    free_compilation_result(compiled);

    print("[Compiler] TinyVM Compiler finished.");
    return 0;
}
