// src/synchronization/synchronization.c
#include "synchronization.h"

#include <stdlib.h>
#include <string.h>

// Get or create mutex
SynchronizationLock* get_sync_lock(VM* vm, const char* name) {

    // Look for existing mutex
    for (int i = 0; i < vm->lock_count; i++) {
        if (strcmp(vm->locks[i].name, name) == 0) {
            return &vm->locks[i];
        }
    }

    // Create new mutex if not found
    if (vm->lock_count < vm->lock_capacity) {
        pthread_mutex_lock(&vm->lock_mgmt_lock);

        SynchronizationLock* mutex = &vm->locks[vm->lock_count++];
        mutex->name = strdup(name);
        pthread_mutex_init(&mutex->mutex, NULL);
        mutex->locked = 0;

        pthread_mutex_unlock(&vm->lock_mgmt_lock);
        return mutex;
    }
    return NULL;
}