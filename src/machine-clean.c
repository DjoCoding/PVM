#include "machine-clean.h"

void machine_clean(Machine *self) {
    if (self->memory.items) { free(self->memory.items); }
    if (self->stack.items)  { free(self->stack.items); }
}