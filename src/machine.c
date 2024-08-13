#include "machine.h"

Machine machine_init() {
    Machine m;
    
    DA_INIT(&m.memory, sizeof(Memory_Cell));
    DA_INIT(&m.progs, sizeof(Program));
    DA_INIT(&m.stack, sizeof(int64_t));
    DA_INIT(&m.str_stack, sizeof(String_View));

    m.subroutines.count = 0;
    m.halted = 1;
    m.ip = 0;

    return m;
}

void machine_load_program_parts(FILE *f, Program *prog);

void machine_read_operand(FILE *f, Inst_Op *op) {
    fread(&op->kind, sizeof(op->kind), 1, f);

    if (op->kind == OP_KIND_STRING) {
        size_t size = 0;

        fread(&size, sizeof(size_t), 1, f);
        char *s = calloc(sizeof(char), size + 1);
        fread(s, sizeof(char), size, f);
        
        op->value = (int64_t)s;
    } else if (op->kind == OP_KIND_NUMBER) {
        fread(&op->value, sizeof(int64_t), 1, f);
    } else { ASSERT(false, "unreachable"); }
}

void machine_read_operands(FILE *f, Inst *inst) {
    fread(&inst->ops.size, sizeof(size_t), 1, f);
    
    inst->ops.items = malloc(sizeof(*inst->ops.items) * inst->ops.size);
    inst->ops.count = inst->ops.size;

    for (size_t i = 0; i < inst->ops.size; ++i) {
        machine_read_operand(f, &inst->ops.items[i]);
    }
}

void machine_read_instruction_from_file(FILE *f, Program_Inst *prog_inst) {
    Inst inst = {0};
    fread(&inst.kind, sizeof(inst.kind), 1, f);
    machine_read_operands(f, &inst);
    prog_inst->as.inst = inst;
}

void machine_read_prog_inst_from_file(FILE *f, Program *prog) {
    Program_Inst prog_inst = {0};
    fread(&prog_inst.kind, sizeof(prog_inst.kind), 1, f);
    
    if (prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        machine_read_instruction_from_file(f, &prog_inst);
    } else if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        prog_inst.as.prog = malloc(sizeof(Program));
        machine_load_program_parts(f, prog_inst.as.prog);
    }

    DA_APPEND(prog, prog_inst);
}

void machine_load_prog_inst_from_file(FILE *f, Program *prog) {
    fread(&prog->size, sizeof(size_t), 1, f);
    prog->items = malloc(sizeof(*prog->items) * prog->size);
    prog->count = 0;
    for (size_t i = 0; i < prog->size; ++i) {
        machine_read_prog_inst_from_file(f, prog);
    }
}

void machine_load_entry_from_file(FILE *f, Program *prog) {
    fread(&prog->entry, sizeof(prog->entry), 1, f);
}

void machine_load_program_parts(FILE *f, Program *prog) {
    machine_load_entry_from_file(f, prog);
    machine_load_prog_inst_from_file(f, prog);
}

Program machine_load_prog_from_file(char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) { THROW_ERROR("could not open the file %s", filepath); }

    Program prog = {0};
    
    machine_load_program_parts(f, &prog);
    prog.state = PROGRAM_STATE_NOT_EXECUTED_YET;

    return prog;
}


