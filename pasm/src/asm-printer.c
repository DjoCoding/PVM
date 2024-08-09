#include "asm-printer.h"

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
    };
    return kinds[kind];
}

char *type_to_string(PASM_Type type) {
    char *types[] = {
        "string",
        "number",
        "char",
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
        "jz",
        "jle",
        "jge",
        "jl",
        "jg",
        "putc",
        "call",
        "ret",
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
    printf("op type: %s ", type_to_string((PASM_Type) op.kind));
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

void pasm_print_const(PASM_Const c) {
    printf("type: %s ", type_to_string(c.kind));
    printf(SV_FMT, SV_UNWRAP(c.value));
} 

void pasm_print_consts(PASM_Consts consts) {
    for (size_t i = 0; i < consts.count; ++i) {
        pasm_print_const(consts.items[i]);
    }
}

void pasm_print_node(PASM_Node node) {
    printf("node kind: %s:\n", node_kind_to_cstr(node.kind));
    printf("\t");

    if (node.kind == NODE_KIND_INSTRUCTION) { pasm_print_inst(node.as.inst); }
    else if (node.kind == NODE_KIND_CONST_DEF) { pasm_print_consts(node.as.constants); }
    else if (node.kind == NODE_KIND_LABEL_DEF) { printf(SV_FMT, SV_UNWRAP(node.as.label)); }
    else if (node.kind == NODE_KIND_ENTRY) { printf("entry: `" SV_FMT "`", SV_UNWRAP(node.as.label)); }
    else if (node.kind == NODE_KIND_USE) { printf("use: `" SV_FMT "` file", SV_UNWRAP(node.as.file_path)); }
    else { ASSERT(false, "unreachable"); }
    
    printf("\n\n");
}

void pasm_print_nodes(PASM *self) {
    for (size_t i = 0; i < self->nodes.count; ++i) {
        printf("%zu - ", i + 1);
        pasm_print_node(self->nodes.items[i]);
    }
}