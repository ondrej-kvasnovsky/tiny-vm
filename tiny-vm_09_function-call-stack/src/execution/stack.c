// src/execution/stack.c
#include "stack.h"
#include "../memory/memory.h"
#include <stdlib.h>
#include <utils/logger.h>

StackFrame *push_stack_frame(ThreadContext *thread, const Function *function) {
    StackFrame *new_frame = malloc(sizeof(StackFrame));
    new_frame->function = function;
    new_frame->return_pc = thread->pc;
    new_frame->local_scope = create_local_scope();
    new_frame->previous = thread->current_frame;

    thread->current_frame = new_frame;
    return new_frame;
}

void pop_stack_frame(ThreadContext *thread) {
    if (!thread->current_frame) return;

    StackFrame *frame_to_pop = thread->current_frame;
    thread->current_frame = frame_to_pop->previous;

    // Restore program counter to return address
    if (thread->current_frame) {
        // Move to next instruction after call
        thread->pc = frame_to_pop->return_pc + 1;
    }

    // Cleanup frame resources
    if (frame_to_pop->local_scope) {
        destroy_local_scope(frame_to_pop->local_scope);
    }
    free(frame_to_pop);
}

Variable *get_variable_from_stack(ThreadContext *thread, const char *name) {
    StackFrame *current = thread->current_frame;
    while (current) {
        Variable *var = get_variable(thread, name);
        if (var) return var;
        current = current->previous;
    }
    return NULL;
}

void print_stack_frame(ThreadContext *thread, StackFrame *frame) {
    print("[Thread %d] > at %s (pc=%d)", thread->thread_id, frame->function->name, frame->return_pc);

    // Print local variables if any
    if (frame->local_scope && frame->local_scope->var_count > 0) {
        print("[Thread %d]   > Local variables:", thread->thread_id);
        for (int i = 0; i < frame->local_scope->var_count; i++) {
            Variable *var = &frame->local_scope->variables[i];
            print("[Thread %d]     >  %s = %d", thread->thread_id, var->name, var->value);
        }
    }
}

void print_stack_trace(ThreadContext *thread) {
    print("[Thread %d] Stack trace:", thread->thread_id);

    // Print current execution point
    if (thread->current_frame) {
        print("[Thread %d] > Currently executing: %s (pc=%d)",
              thread->thread_id,
              thread->current_frame->function->name,
              thread->pc);
    }

    // Print stack frames from top to bottom
    StackFrame *current = thread->current_frame;

    while (current) {
        print_stack_frame(thread, current);
        current = current->previous;
    }
}
