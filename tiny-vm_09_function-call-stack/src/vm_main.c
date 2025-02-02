// src/vm_main.c
#include "utils/logger.h"
#include "core/vm.h"
#include "compiler/compiler.h"

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        print("Usage: %s <bytecode_file>", argv[0]);
        return 1;
    }

    print("[VM] Starting TinyVM...");

    CompilationResult* compiled = load_compiled_bytecode(argv[1]);
    if (!compiled) {
        print("[VM] Compilation failed");
        return 1;
    }
    print_compilation_result(compiled);

    // Create and run VM
    VM* vm = create_vm(compiled);
    run_vm(vm);

    // Cleanup
    destroy_vm(vm);

    print("[VM] TinyVM finished.");
    return 0;
}
