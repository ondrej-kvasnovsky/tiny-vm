// src/instruction/instruction.c
#include "instruction.h"

#include "../thread/thread.h"
#include "../memory/memory.h"
#include "../synchronization/synchronization.h"
#include "../utils/logger.h"
#include "../function/function.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Instruction parse_instruction(const char* line) {
    Instruction instr;
    memset(&instr, 0, sizeof(Instruction));

    char cmd[32];
    sscanf(line, "%s", cmd);

    if (strcmp(cmd, "print") == 0) {
        instr.type = PRINT;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "set") == 0) {
        instr.type = SET;
        sscanf(line, "%s %s %s", cmd, instr.args[0], instr.args[1]);
    }
    else if (strcmp(cmd, "add") == 0) {
        instr.type = ADD;
        sscanf(line, "%s %s %s %s", cmd, instr.args[0], instr.args[1], instr.args[2]);
    }
    else if (strcmp(cmd, "sleep") == 0) {
        instr.type = SLEEP;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "thread") == 0) {
        instr.type = THREAD;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "exit") == 0) {
        instr.type = EXIT;
    }
    else if (strcmp(cmd, "setshared") == 0) {
        instr.type = SETSHARED;
        sscanf(line, "%s %s %s", cmd, instr.args[0], instr.args[1]);
    }
    else if (strcmp(cmd, "lock") == 0) {
        instr.type = LOCK;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "unlock") == 0) {
        instr.type = UNLOCK;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "sync") == 0) {
        instr.type = SYNC;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "async") == 0) {
        instr.type = ASYNC;
        sscanf(line, "%s %s", cmd, instr.args[0]);
    }
    else if (strcmp(cmd, "function") == 0) {
        instr.type = FUNCTION;
        sscanf(line, "%s %s", cmd, instr.args[0]);  // args[0] will contain function name
    }
    // print("[VM] Instruction parsed: %s", line);
    return instr;
}

char* get_function_name(const char** code) {
    if (!code || !code[0]) return NULL;

    char cmd[32], name[32];
    if (sscanf(code[0], "%s %s", cmd, name) == 2 && strcmp(cmd, "function") == 0) {
        return strdup(name);
    }
    print("[VM] Error: Invalid function declaration: %s", code[0]);
    return NULL;
}
