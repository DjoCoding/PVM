#ifndef MACHINE_MEMORY_H
#define MACHINE_MEMORY_H

#include "machine-defs.h"
#include "machine-error.h"
#include "machine-clean.h"

#define alloc machine_memory_allocate

size_t machine_memory_allocate(Machine *self, size_t size);


#endif // MACHINE_MEMORY_H