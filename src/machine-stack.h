#ifndef MACHINE_STACK_H
#define MACHINE_STACK_H

#include "machine-defs.h"
#include "machine-clean.h"
#include "machine-error.h"

#define push machine_stack_push
#define pop  machine_stack_pop
#define peek machine_stack_peek


void machine_stack_push(Machine *self, int64_t value);
int64_t machine_stack_pop(Machine *self);
int64_t machine_stack_peek(Machine *self);
void machine_dump_stack(Machine *self);


#endif // MACHINE_STACK_H