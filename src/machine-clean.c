#include "machine-clean.h"

void machine_clean_prog(Program prog);

void machine_clean_operand(Inst_Op op) {
    if (op.kind == OP_KIND_STRING) {
        free((void *) op.value);
    }
}

void machine_clean_operands(Inst_Ops ops) {
    for (size_t i = 0; i < ops.count; ++i) {
        machine_clean_operand(ops.items[i]);
    }
    free(ops.items);
}

void machine_clean_prog_inst(Program_Inst prog_inst) {
    if (prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        machine_clean_operands(prog_inst.as.inst.ops);
    } else if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        machine_clean_prog(*prog_inst.as.prog);
        free(prog_inst.as.prog);
    } else { ASSERT(false, "unreachable"); }
}

void machine_clean_prog(Program prog) {
    for (size_t i = 0; i < prog.count; ++i) {
        machine_clean_prog_inst(prog.items[i]);
    }
    free(prog.items);
}

void machine_clean(Machine *self) {
    (void)self;
    
    if (self->memory.size) { free(self->memory.items); }
    if (self->stack.size)  { free(self->stack.items); }
    
    for (size_t i = 0; i < self->progs.count; ++i) {
        machine_clean_prog(self->progs.items[i]);
    }
    free(self->progs.items);
}