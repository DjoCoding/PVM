#include "disasm.h"

// clean 

void depasm_clean_operand(Inst_Op op) {
    if (op.kind == OP_KIND_STRING) {
        free((char *)op.value);
    }
}

void depasm_clean_operands(Inst_Ops ops) {
    for (size_t i = 0; i < ops.count; ++i) {
        depasm_clean_operand(ops.items[i]);
    }
    free(ops.items);
}

void depasm_clean_prog_inst(Program_Inst prog_inst) {
    if (prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        depasm_clean_operands(prog_inst.as.inst.ops);
    } else if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        depasm_clean_prog(*prog_inst.as.prog);
        free(prog_inst.as.prog);
    } else { ASSERT(false, "unreachable"); }
}

void depasm_clean_prog(Program prog) {
    for (size_t i = 0; i < prog.count; ++i) {
        depasm_clean_prog_inst(prog.items[i]);
    }
    free(prog.items);
}

size_t line = 0;

void depasm_write_line(FILE *f) {
    fprintf(f, "%zu: ", line);
    line++;
}

void depasm_write_inst_kind(FILE *f, Inst_Kind kind) {
    char *op_codes[] = {
         "nop",
        "halt",
        "push",
        "pushs",
        "pop",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "swap",
        "dup",
        "inswap",
        "indup",
        "syscall",
        "jmp",
        "cmp",
        "cmple",
        "cmpl",
        "cmpge",
        "cmpg",
        "jz",
        "jle",
        "jge",
        "jl",
        "jg",
        "putc",
        "readc",
        "call",
        "ret",
        "stop",
        "store",
        "load",
        "loadb",
        "strb"
    };

    fprintf(f, "%s ", op_codes[kind]);
}

void depasm_write_operand(FILE *f, Inst_Op op) {
    if (op.kind == OP_KIND_NUMBER) {
        fprintf(f, "%ld ", op.value);
    } else if (op.kind == OP_KIND_STRING) {
        char *s = unescape_string((char *)op.value, strlen((char *)op.value));
        fprintf(f, "\"%s\" ", s);
        free(s);
    } else { ASSERT(false, "unreachable"); }
}

void depasm_write_operands(FILE *f, Inst_Ops ops) {
    for (size_t i = 0; i < ops.count; ++i) {
        depasm_write_operand(f, ops.items[i]);
    }
    fprintf(f, "\n");
}

void depasm_inst(FILE *f, Inst inst) {
    depasm_write_line(f);
    depasm_write_inst_kind(f, inst.kind);
    depasm_write_operands(f, inst.ops);
}

void depasm_prog_inst(FILE *f, Program_Inst prog_inst) {
    if (prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        depasm_inst(f, prog_inst.as.inst);
    } else if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        depasm(f, *prog_inst.as.prog);
    }
}

void depasm(FILE *f, Program prog) {
    for (size_t i = 0; i < prog.count; ++i) {
        depasm_prog_inst(f, prog.items[i]);
    }
}