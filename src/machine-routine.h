#ifndef MACHINE_ROUTINE_H
#define MACHINE_ROUTINE_H

#include "machine-defs.h"

void machine_start_routine(Machine *self, size_t ip);
size_t machine_end_routine(Machine *self);
bool machine_end_of_routines(Machine *self);



#endif // MACHINE_ROUTINE_H