#include "asm.h"

void pasm_write_string_to_file(FILE *f, char *s, size_t size) {
    fwrite(&size, sizeof(size_t), 1, f);
    fwrite(s, sizeof(char), size, f);
}

void pasm_write_operand_to_file(FILE *f, Inst_Op op) {
    fwrite(&op.kind, sizeof(op.kind), 1, f);

    if (op.kind == OP_KIND_STRING) {
        char *s = (char *)op.value;
        size_t s_size = strlen(s);
        pasm_write_string_to_file(f, s, s_size);
    } else if (op.kind == OP_KIND_NUMBER) {
        fwrite(&op.value, sizeof(int64_t), 1, f);
    } else { ASSERT(false, "unreachable"); }
}

void pasm_write_operands_to_file(FILE *f, Inst_Ops ops) {
    fwrite(&ops.count, sizeof(size_t), 1, f);
    for (size_t i = 0; i < ops.count; ++i) {
        pasm_write_operand_to_file(f, ops.items[i]);
    }
}

void pasm_write_instruction_to_file(FILE *f, Inst inst) {
    fwrite(&inst.kind, sizeof(inst.kind), 1, f);
    pasm_write_operands_to_file(f, inst.ops);
}

void pasm_write_prog_inst_to_file(FILE *f, Program_Inst prog_inst) {
    fwrite(&prog_inst.kind, sizeof(prog_inst.kind), 1, f);

    if(prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        pasm_write_instruction_to_file(f, prog_inst.as.inst);
    } else {
        pasm_write_prog_parts_to_file(f, *prog_inst.as.prog);
    }    
}

void pasm_write_prog_to_file(FILE *f, PASM_Prog prog) {
    fwrite(&prog.count, sizeof(size_t), 1, f);
    for (size_t i = 0; i < prog.count; ++i) {
        pasm_write_prog_inst_to_file(f, prog.items[i]);
    }
}

void pasm_write_entry_to_file(FILE *f, Program_Entry entry) {
    fwrite(&entry, sizeof(entry), 1, f);
}

void pasm_write_prog_parts_to_file(FILE *f, Program prog) {
    pasm_write_entry_to_file(f, prog.entry);
    pasm_write_prog_to_file(f, prog);
}

void pasm_store_prog_in_file(char *filepath, PASM_Prog prog) {
    FILE *f = fopen(filepath, "wb");
    if (!f) { THROW_ERROR("could not open the file %s for writing", filepath); }

    fseek(f, 0, SEEK_SET);

    pasm_write_prog_parts_to_file(f, prog);

    fclose(f);
}
