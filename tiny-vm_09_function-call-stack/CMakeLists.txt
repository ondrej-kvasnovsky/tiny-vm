# CMakeLists.txt
cmake_minimum_required(VERSION 3.30)
project(tiny_vm C)

set(CMAKE_C_STANDARD 11)

set(COMMON_SOURCES
        src/utils/logger.c
        src/utils/logger.h
        src/core/vm.c
        src/core/vm.h
        src/thread/thread.c
        src/thread/thread.h
        src/synchronization/synchronization.c
        src/synchronization/synchronization.h
        src/memory/memory.c
        src/memory/memory.h
        src/instruction/instruction.c
        src/instruction/instruction.h
        src/types.h
        src/function/function.c
        src/function/function.h
        src/execution/execution.c
        src/execution/execution.h
        src/compiler/compiler.h
        src/compiler/compiler.c
        src/compiler/bytecode.c
        src/compiler/source_loader.h
        src/compiler/source_loader.c
        src/execution/stack.h
        src/execution/stack.c
)

# Compiler executable
add_executable(tiny_vm_compile
        src/compiler_main.c
        ${COMMON_SOURCES}
)

# VM executable
add_executable(tiny_vm_run
        src/vm_main.c
        ${COMMON_SOURCES}
)

# Add include directories if needed
target_include_directories(tiny_vm_compile PRIVATE src)
target_include_directories(tiny_vm_run PRIVATE src)

add_custom_target(run_counter
        COMMAND tiny_vm_compile ${CMAKE_SOURCE_DIR}/examples/counter.tvm
        COMMAND tiny_vm_run ${CMAKE_SOURCE_DIR}/examples/counter.tvmc
)

add_custom_target(run_hello
        COMMAND tiny_vm_compile ${CMAKE_SOURCE_DIR}/examples/hello.tvm
        COMMAND tiny_vm_run ${CMAKE_SOURCE_DIR}/examples/hello.tvmc
)
