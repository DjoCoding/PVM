#include "machine-clean.h"

void machine_clean(Machine *self) {
    if (self->memory.items) { free(self->memory.items); }
    if (self->stack.items)  { free(self->stack.items); }

    if (self->str_stack.items) {
        for (size_t i = 0; i < self->str_stack.count; ++i) {
            if (self->str_stack.items[i].count != 0) {
                free(self->str_stack.items[i].content);
            }
        }
    }
}