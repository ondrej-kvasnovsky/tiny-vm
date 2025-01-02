# CMakeLists.txt
cmake_minimum_required(VERSION 3.30)
project(tiny_vm_05_refactoring C)

set(CMAKE_C_STANDARD 11)

add_executable(
        tiny_vm_05_refactoring
        src/main.c
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
)
