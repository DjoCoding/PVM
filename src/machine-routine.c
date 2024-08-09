#include "machine-routine.h"

void machine_start_routine(Machine *self, size_t ip) {
    if (self->subroutines.count >= SUBROUTINE_STACK_CAP) { 
        THROW_ERROR("could not push a new subroutine to the stack due to an overflow");
    }

    self->subroutines.items[self->subroutines.count++] = ip;
}

size_t machine_end_routine(Machine *self) {
    return self->subroutines.items[--self->subroutines.count];
}

bool machine_end_of_routines(Machine *self) {
    return (self->subroutines.count == 0);
}
