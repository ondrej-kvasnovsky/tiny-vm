// src/synchronization/synchronization.h
#ifndef TINY_VM_SYNCHRONIZATION_H
#define TINY_VM_SYNCHRONIZATION_H

#include "../types.h"

SynchronizationLock* get_sync_lock(VM* vm, const char* name);

#endif
