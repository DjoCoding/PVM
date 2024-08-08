#include "machine-stack.h"

void machine_stack_push(Machine *self, int64_t value) {
    DA_APPEND(&self->stack, value);
    machine_dump_stack(self);
}

int64_t machine_stack_pop(Machine *self) {
    if (self->stack.count == 0) {
        machine_clean(self);
        THROW_ERROR("could not pop from the stack due to stack underflow");
    }
    self->stack.count -= 1;
    machine_dump_stack(self);
    return self->stack.items[self->stack.count];
}

int64_t machine_stack_peek(Machine *self) {
    if (self->stack.count == 0) {
        machine_clean(self);
        THROW_ERROR("could not peek the top of the stack due to empty stack");
    }

    return self->stack.items[self->stack.count - 1];
}

void machine_dump_stack(Machine *self) {
    printf("Stack---\n");   
    if (self->stack.count == 0) { return; }
    for (size_t i = self->stack.count - 1; i != 0; --i) {
        printf("\t%ld\n", self->stack.items[i]);
    }
    printf("\t%ld\n", self->stack.items[0]);
    printf("Stack-End---\n");
}
