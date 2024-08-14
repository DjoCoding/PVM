#include "asm-clean.h"

void pasm_clean_prog(Program prog);
void pasm_clean_ast(PASM_Nodes nodes);

void pasm_clean_used_files(PASM *self) {
    for (size_t i = 0; i < self->used_files.count; ++i) {
        free(self->used_files.items[i]);
    }
    free(self->used_files.items);
}

void pasm_clean_sup_files(PASM *self) {
    for (size_t i = 0; i < self->sup_files.count; ++i) {
        free(self->sup_files.items[i]);
    }
    free(self->sup_files.items);
}

void pasm_clean_lexer(PASM *self) {
    free(self->lexer.source.content);
}

void pasm_clean_arg(PASM_Arg arg) {
    if (arg.type == TYPE_STRING) {
        free(arg.as.string);
    }
}

void pasm_clean_const(PASM_Const constant) {
    pasm_clean_arg(constant.value);
}

void pasm_clean_constants(PASM_Consts constants) {
    for (size_t i = 0; i < constants.count; ++i) {
        pasm_clean_const(constants.items[i]);
    }
    free(constants.items);
}

void pasm_clean_operand(Inst_Op op) {
    (void)op;
}

void pasm_clean_operands(Inst_Ops ops) {
    for (size_t i = 0; i < ops.count; ++i) {
        pasm_clean_operand(ops.items[i]);
    }
    free(ops.items);
}

void pasm_clean_prog_inst(Program_Inst prog_inst) {
    if (prog_inst.kind == PROGRAM_INST_INSTRUCTION) {
        pasm_clean_operands(prog_inst.as.inst.ops);
    } else if (prog_inst.kind == PROGRAM_INST_PROGRAM) {
        pasm_clean_prog(*prog_inst.as.prog);
        free(prog_inst.as.prog);
    } else { ASSERT(false, "unreachable"); }
}

void pasm_clean_prog(Program prog) {
    for (size_t i = 0; i < prog.count; ++i) {
        pasm_clean_prog_inst(prog.items[i]);
    }
    free(prog.items);
}

void pasm_clean_macro_call(PASM_Macro_Call call) {
    for (size_t i = 0; i < call.args.count; ++i) { 
        pasm_clean_operand(call.args.items[i]);
    }
    free(call.args.items);
} 

void pasm_clean_macro_def(PASM_Macro_Def def) {
    pasm_clean_ast(*def.block);
    free(def.arg_names.items);
    free(def.block);
}

void pasm_clean_node(PASM_Node node) {
    if (node.kind == NODE_KIND_CONST_DEF) {
        pasm_clean_constants(node.as.constants);
    } else if (node.kind == NODE_KIND_INSTRUCTION) {
        pasm_clean_prog_inst(node.as.inst);
    } else if (node.kind == NODE_KIND_MACRO_CALL) {
        pasm_clean_macro_call(node.as.macro_call);
    } else if (node.kind == NODE_KIND_MACRO) {
        pasm_clean_macro_def(node.as.macro_def);
    }
}

void pasm_clean_ast(PASM_Nodes nodes) {
    for(size_t i = 0; i < nodes.count; ++i) {
        pasm_clean_node(nodes.items[i]);
    }
    free(nodes.items);
}

void pasm_clean_context(PASM_Context context) {
    hashmap_destroy(&context.map);
}

void pasm_clean_contexts(PASM *self) {
    for (size_t i = 0; i < self->contexts.count; ++i) {
        pasm_clean_context(self->contexts.items[i]);
    }
}

void pasm_clean(PASM *self) {
    pasm_clean_used_files(self);
    pasm_clean_sup_files(self);
    pasm_clean_lexer(self);
    pasm_clean_ast(self->nodes);
    pasm_clean_contexts(self);
    pasm_clean_prog(self->prog);
}