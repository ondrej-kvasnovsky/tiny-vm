// tiny_vm.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tiny_vm.h"

LocalScope* create_local_scope() {
    LocalScope* local_scope = malloc(sizeof(LocalScope));
    local_scope->var_capacity = 10;
    local_scope->var_count = 0;
    local_scope->variables = malloc(sizeof(Variable) * local_scope->var_capacity);
    return local_scope;
}

void destroy_local_scope(LocalScope* local_scope) {
    for (int i = 0; i < local_scope->var_count; i++) {
        free(local_scope->variables[i].name);
    }
    free(local_scope->variables);
    free(local_scope);
}

// Find or create variable in local_scope
Variable* get_variable(LocalScope* local_scope, const char* name) {
    for (int i = 0; i < local_scope->var_count; i++) {
        if (strcmp(local_scope->variables[i].name, name) == 0) {
            return &local_scope->variables[i];
        }
    }

    // Create new variable
    if (local_scope->var_count < local_scope->var_capacity) {
        Variable* var = &local_scope->variables[local_scope->var_count++];
        var->name = strdup(name);
        var->value = 0;
        return var;
    }
    return NULL;
}

// Get variable value, return 0 if not found
t_int get_value(LocalScope* local_scope, const char* name) {
    for (int i = 0; i < local_scope->var_count; i++) {
        if (strcmp(local_scope->variables[i].name, name) == 0) {
            return local_scope->variables[i].value;
        }
    }
    return 0;
}

Instruction parse_instruction(const char* line) {
    Instruction instr;
    memset(&instr, 0, sizeof(Instruction));

    char cmd[32];
    sscanf(line, "%s", cmd); // Read first word from line (command)

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

    return instr;
}

void execute_instruction(LocalScope* local_scope, Instruction* instr) {
    switch (instr->type) {
        case PRINT: {
            t_int value = get_value(local_scope, instr->args[0]);
            printf("Printing: %s %d\n", instr->args[0], value);
            break;
        }
        case SET: {
            Variable* var = get_variable(local_scope, instr->args[0]);
            if (var) {
                var->value = atoi(instr->args[1]);
            }
            break;
        }
        case ADD: {
            t_int val1 = get_value(local_scope, instr->args[1]);
            t_int val2 = get_value(local_scope, instr->args[2]);
            Variable* target = get_variable(local_scope, instr->args[0]);
            if (target) {
                target->value = val1 + val2;
            }
            break;
        }
        case SLEEP: {
            usleep(atoi(instr->args[0]) * 1000);
            break;
        }
    }
}
