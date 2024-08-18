#include "asm-printer.h"


void print_indent(size_t indent) {
    for (size_t i = 0; i < indent; ++i) {
        printf("  ");
    }
}

char *token_kind_to_cstr(PASM_Token_Kind kind) {
    char *kinds[] = {
        "string",
        "number",
        "char",
        "new-line",
        "colon",
        "pre-process",
        "identifier",
        "comma",
        "no-kind"
    };
    return kinds[kind];
}

char *node_kind_to_cstr(PASM_Node_Kind kind) {
    char *kinds[] = {
        "instruction",
        "label-def",
        "const-def",
        "entry-def",
        "use",
        "macro",
        "macro-call"
    };
    return kinds[kind];
}

char *op_kind(Op_Kind kind) {
    char *kinds[] = {
        "number",
        "string",
        "id",
    };
    return kinds[kind];
}

char *type(PASM_Arg_Type type) {
    char *types[] = {
        "string",
        "number",
        "float",
        "char",
        "id",
    }; 
    return types[type];
}

char *inst_kind_to_cstr(Inst_Kind kind) {
    char *kinds[] = {
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

    size_t inst_size = sizeof(kinds)/sizeof(kinds[0]);

    if (kind < inst_size) return kinds[kind];

    ASSERT(false, "unreachable");
}

void pasm_print_token(PASM_Token token) {
    char *kind = token_kind_to_cstr(token.kind);
    printf("kind: %s", kind);

    if (sv_empty(token.text)) {
        printf("\n");
        return;
    }
    printf(" -> " SV_FMT "\n", SV_UNWRAP(token.text));
}


void pasm_print_tokens(PASM *self) {
    for (size_t i = 0; i < self->tokens.count; ++i) {
        pasm_print_token(self->tokens.items[i]);
    }
}

void pasm_print_operand(Inst_Op op) {
    printf("op type: %s ", op_kind(op.kind));
    if (op.kind == OP_KIND_STRING) { printf("value: \"%s\"", (char *)op.value); }
    else if (op.kind == OP_KIND_NUMBER) { printf("value: %ld", op.value); }
    else if (op.kind == OP_KIND_ID) { printf("name: `%s`", (char *)op.value); }
    else { ASSERT(false, "unreachable"); }
}  

void pasm_print_inst(Inst inst) {
    printf("kind: %s", inst_kind_to_cstr(inst.kind));
    for (size_t i = 0; i < inst.ops.count; ++i) {
        printf("\n\t\t");
        pasm_print_operand(inst.ops.items[i]);
    }
}

void pasm_print_arg_value(PASM_Arg arg) {
    if (arg.type == TYPE_STRING) {
        printf("%s", arg.as.string);
        return;
    } 

    if (arg.type == TYPE_INTEGER) {
        printf("%ld", arg.as.integer);
        return;
    }

    if (arg.type == TYPE_CHAR) {
        printf("%c", arg.as.c);
        return;
    }

    if (arg.type == TYPE_FLOAT) {
        printf("%lf", arg.as.flt);
        return;
    }

    if (arg.type == TYPE_ID) {
        printf(SV_FMT, SV_UNWRAP(arg.as.id));
        return;
    }

    ASSERT(false, "unreachable");
}

void pasm_print_const(PASM_Const c) {
    printf("type: %s ", type(c.value.type));
    pasm_print_arg_value(c.value);
} 

void pasm_print_consts(PASM_Consts consts) {
    for (size_t i = 0; i < consts.count; ++i) {
        pasm_print_const(consts.items[i]);
    }
}

void pasm_print_macro_call(PASM_Macro_Call call) {
    printf("call: " SV_FMT ": ", SV_UNWRAP(call.name));
    for (size_t i = 0; i < call.args.count; ++i) {
        printf("\n\t\t");
        pasm_print_operand(call.args.items[i]);
    }
}

void pasm_print_macro_def(PASM_Macro_Def def, size_t indent) {
    printf("macro `" SV_FMT "` ", SV_UNWRAP(def.name));
    for (size_t i = 0; i < def.arg_names.count; ++i) {
        printf("\n\t\t");
        printf(SV_FMT " ", SV_UNWRAP(def.arg_names.items[i]));
    }
    printf("\n");
    pasm_print_nodes(*def.block, indent + 1);
}

void pasm_print_node(PASM_Node node, size_t indent) {
    printf("node kind: %s:\n", node_kind_to_cstr(node.kind));
    print_indent(indent + 1);

    if (node.kind == NODE_KIND_INSTRUCTION) { pasm_print_inst(node.as.inst.as.inst); }
    else if (node.kind == NODE_KIND_CONST_DEF) { pasm_print_consts(node.as.constants); }
    else if (node.kind == NODE_KIND_LABEL_DEF) { printf(SV_FMT, SV_UNWRAP(node.as.label)); }
    else if (node.kind == NODE_KIND_ENTRY) { printf("entry: `" SV_FMT "`", SV_UNWRAP(node.as.label)); }
    else if (node.kind == NODE_KIND_USE) { printf("use: `" SV_FMT "` file", SV_UNWRAP(node.as.file_path)); }
    else if (node.kind == NODE_KIND_MACRO) { pasm_print_macro_def(node.as.macro_def, indent); }
    else if (node.kind == NODE_KIND_MACRO_CALL) { pasm_print_macro_call(node.as.macro_call); }
    else { ASSERT(false, "unreachable"); }
    
    printf("\n\n");
}

void pasm_print_nodes(PASM_Nodes nodes, size_t indent) {
    for (size_t i = 0; i < nodes.count; ++i) {
        print_indent(indent);
        printf("%zu - ", i + 1);
        pasm_print_node(nodes.items[i], indent + 1);
    }
}

void pasm_print_parsing_result(PASM *self) {
    pasm_print_nodes(self->nodes, 0);
}