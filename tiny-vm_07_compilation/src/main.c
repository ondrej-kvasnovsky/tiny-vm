// src/main.c
#include "utils/logger.h"
#include "core/vm.h"

#include <stdio.h>

int main(void) {
    // const char *function1[] = {
    //     "function function1",
    //     "set a 2",
    //     "print a",
    //     "exit",
    //     NULL
    // };
    //
    // const char *function2[] = {
    //     "function function2",
    //     "set b 10",
    //     "print b",
    //     NULL
    // };
    //
    // const char *main_code[] = {
    //     "function main",
    //     // Runs in new thread, asynchronously
    //     "async function1",
    //     // Executes synchronously in current thread
    //     "sync function2",
    //     "exit",
    //     NULL
    // };
    //
    // const char** functions[] = {
    //     function1,
    //     function2,
    //     main_code,
    //     NULL
    // };
    const char *createCounter[] = {
        "function createCounter",
        "setshared counter 1000",
        "print counter",
        NULL
    };

    const char *incrementCounter[] = {
        "function incrementCounter",
        "lock counter_lock",
        "set increment 10",
        "add counter increment counter",
        "print counter",
        "unlock counter_lock",
        "exit",
        NULL
    };

    const char *decrementCounter[] = {
        "function decrementCounter",
        "lock counter_lock",
        "set decrement -10",
        "add counter decrement counter",
        "print counter",
        "unlock counter_lock",
        "exit",
        NULL
    };

    const char *main_code[] = {
        "function main",
        "sync createCounter",
        "async incrementCounter",
        "async decrementCounter",
        "exit",
        NULL
    };

    const char** functions[] = {
        createCounter,
        incrementCounter,
        decrementCounter,
        main_code,
        NULL
    };

    print("[VM] Starting TinyVM...");
    VM *vm = create_vm(functions);
    run_vm(vm);

    destroy_vm(vm);

    print("[VM] TinyVM finished.");
    return 0;
}
