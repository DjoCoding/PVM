#include "asm-parser.h"

PASM_Node pasm_parser_parse_statement(PASM *self);

// void inst_begin(PASM_Inst *inst) {
//     *inst = (Inst) {0};
//     DA_INIT(&inst->ops, sizeof(Inst_Op));
// }

// void inst_set_kind(PASM_Inst *inst, Inst_Kind kind) {
//     inst->kind = kind;
// }

// void inst_add_operand(PASM_Inst *inst, Inst_Op op) {
//     DA_APPEND(&inst->ops, op);
// }

PASM_Token pasm_parser_peek(PASM *self) {
    return self->tokens.items[self->parser.current];
}

void pasm_parser_advance(PASM *self) {
    self->parser.current++;
}

// end of tokens
int pasm_parser_eot(PASM *self) {
    return self->parser.current >= self->tokens.count;
}


// end of line
int pasm_parser_eol(PASM *self) {
    if (peot(self)) { return 1; }
    return self->tokens.items[self->parser.current].kind == TOKEN_KIND_NEW_LINE;
}


bool is_instruction(String_View s, Inst_Kind *kind) {
    char *instructions[] = {
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

    size_t inst_size = sizeof(instructions)/sizeof(instructions[0]);

    for (size_t i = 0; i < inst_size; ++i) {
        if (sv_eq(s, SV(instructions[i]))) {
            *kind = i;
            return true;
        }
    }

    return false;
}

Inst_Op pasm_parser_parse_operand(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected an instruction operand but end of line found"); }

    Inst_Op op = {0};
    PASM_Token token = ppeek(self);

    if (token.kind == TOKEN_KIND_STRING) { 
        op.kind = OP_KIND_STRING;
        op.value = (int64_t) cstr_from_sv(token.text);
        padv(self);
        return op;
    }

    if (token.kind == TOKEN_KIND_NUMBER) {
        op.kind = OP_KIND_NUMBER;
        if (!sv_parse_integer(token.text, &op.value)) { THROW_ERROR("could not parse `" SV_FMT "` as an integer", SV_UNWRAP(token.text)); }
        padv(self);
        return op;
    }

    if (token.kind == TOKEN_KIND_ID) {
        op.kind = OP_KIND_ID;
        op.value = (int64_t)cstr_from_sv(token.text);
        padv(self);
        return op;
    }

    TODO("add support for more operand types");
}

Inst_Ops pasm_parser_parse_operands(PASM *self) {
    if (peol(self)) { return (Inst_Ops) {0}; }

    Inst_Ops ops;
    DA_INIT(&ops, sizeof(Inst_Op));

    while (!peol(self)) {
        Inst_Op op = pasm_parser_parse_operand(self);
        DA_APPEND(&ops, op);
    }

    return ops;
}

PASM_Node pasm_parser_parse_instruction(PASM *self, Inst_Kind kind) {
    if (peol(self)) { THROW_ERROR("expected an instruction but end of line found"); }

    Inst inst = {0};
    inst.kind = kind;

    // consuming the instruction
    padv(self);

    Inst_Ops ops = pasm_parser_parse_operands(self);
    inst.ops = ops;

    return (PASM_Node) { .kind = NODE_KIND_INSTRUCTION, .as.inst = (Program_Inst) { .kind = PROGRAM_INST_INSTRUCTION, .as.inst = inst } };
}

PASM_Const pasm_parser_parse_const(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected constant identifier but end of line found"); }

    PASM_Token token = ppeek(self);

    if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected constant identifier but `" SV_FMT "` found", SV_UNWRAP(token.text)); }

    PASM_Const node = {0};
    node.name = token.text;
    
    // consuming the constant identifier
    padv(self);

    if (peol(self)) { THROW_ERROR("expected a value for the constant `" SV_FMT "` but end of line found", SV_UNWRAP(node.name)); }
    
    token = ppeek(self);
    node.value = token.text;

    if (token.kind == TOKEN_KIND_STRING) { node.type = TYPE_STRING; }
    else if (token.kind == TOKEN_KIND_NUMBER) { node.type = TYPE_NUMBER; } 
    else if (token.kind == TOKEN_KIND_CHAR) { node.type = TYPE_CHAR; }
    else { THROW_ERROR("expected a value for the constant `" SV_FMT "` but %s found", SV_UNWRAP(node.name), token_kind_to_cstr(token.kind)); }

    // consuming the constant value
    padv(self);

    return node;
}

PASM_Node pasm_parser_parse_const_declaration(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected the `const` keyword but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_PREPROCESS) { THROW_ERROR("expected a constants declaration but else found"); }

    if (!sv_eq(token.text, SV("const"))) { THROW_ERROR("expected `const` keyword but `" SV_FMT "` found", SV_UNWRAP(token.text)); }

    PASM_Consts consts = {0};
    DA_INIT(&consts, sizeof(PASM_Const));

    // consuming the `const` keyword
    padv(self);

    while (true) {
        PASM_Const node = pasm_parser_parse_const(self);
        if (peol(self)) { THROW_ERROR("expected the keyword `end` but end of line found"); }
        
        token = ppeek(self);
        
        if (token.kind == TOKEN_KIND_ID) {
            if (!sv_eq(token.text, SV("end"))) { THROW_ERROR("expected the keyword `end` but `" SV_FMT "` found", SV_UNWRAP(token.text)); }
            DA_APPEND(&consts, node);
            padv(self);
            break;
        }

        if (token.kind == TOKEN_KIND_COMMA) {
            padv(self);
            DA_APPEND(&consts, node);
        }
    }

    return (PASM_Node) { .kind = NODE_KIND_CONST_DEF, .as.constants = consts }; 
}

PASM_Node pasm_parser_parse_label_definition(PASM *self) {
    // a label defintion is defined by `:` after its name
    if (peol(self)) { THROW_ERROR("expected a label definition but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_PREPROCESS) { THROW_ERROR("expected a label defintion but else found"); }

    // consuming the label identifier
    padv(self);

    if (peol(self)) { THROW_ERROR("expected a `:` for the label definition but end of line found"); }
    if (ppeek(self).kind != TOKEN_KIND_COLON) { THROW_ERROR("expected a `:` for the label defintion but `" SV_FMT "` found", SV_UNWRAP(ppeek(self).text)); }

    // consuming the `:` token 
    padv(self);

    String_View label = token.text;

    return (PASM_Node) { .kind = NODE_KIND_LABEL_DEF, .as.label = label };
}

PASM_Node pasm_parser_parse_entry(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected entry defintion but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_PREPROCESS) { THROW_ERROR("expected the entry defintion but else found"); }

    padv(self);

    if (peol(self)) { THROW_ERROR("expected a `:` for the label definition but end of line found"); }
    if (ppeek(self).kind != TOKEN_KIND_COLON) { THROW_ERROR("expected a `:` for the label defintion but `" SV_FMT "` found", SV_UNWRAP(ppeek(self).text)); }

    // consuming the `:` token 
    padv(self);

    token = ppeek(self);
    if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected the entry point label argument but else found"); }

    String_View label_name = token.text;

    padv(self);

    return (PASM_Node) {  .kind = NODE_KIND_ENTRY, .as.label = label_name };
}

PASM_Node pasm_parser_parse_use(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected use statement but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_PREPROCESS) { THROW_ERROR("expected the use statement but else found"); }

    padv(self);

    if (peol(self)) { THROW_ERROR("expected a path to external file but end of line found"); }
    
    token = ppeek(self);
    if (token.kind != TOKEN_KIND_STRING) { THROW_ERROR("expected path to file for the use statement but `" SV_FMT "` found", SV_UNWRAP(ppeek(self).text)); }

    // consuming the file path token 
    padv(self);

    return (PASM_Node) {  .kind = NODE_KIND_USE, .as.file_path = token.text };
}

String_Slices pasm_parse_macro_args(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected macro arguments but end of line found"); }

    String_Slices op_names = {0};
    DA_INIT(&op_names, sizeof(String_View));

    while (!peol(self)) { 
        PASM_Token token = ppeek(self);
        if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected a macro argument of type id but `" SV_FMT "` found", SV_UNWRAP(token.text)); }
        if (sv_eq(token.text, SV("end"))) { break; }        
        DA_APPEND(&op_names, token.text);
        padv(self);
    }

    if (peol(self)) { THROW_ERROR("expected the `end` keyword but end of line found"); }
    
    // consume the `end` keyword
    padv(self);

    return op_names;
}

PASM_Nodes pasm_parse_macro_block(PASM *self, String_View macro_name) {
    // read tokens and parse them until we hit the end token
    PASM_Nodes block = {0};
    DA_INIT(&block, sizeof(PASM_Node));

    while (!peot(self)) {
        PASM_Token current = ppeek(self);

        if (current.kind == TOKEN_KIND_NEW_LINE) { padv(self); continue; }
        if (sv_eq(current.text, SV("end"))) { break; }
        
        PASM_Node node = pasm_parser_parse_statement(self);
        DA_APPEND(&block, node);
    }

    if (peot(self)) { THROW_ERROR("expected the `end` keyword in the macro `" SV_FMT "` definition", SV_UNWRAP(macro_name)); }

    // we consume the `end` keyword
    padv(self);

    return block;
}

PASM_Node pasm_parser_parse_macro_def(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected macro defintion but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_PREPROCESS) { THROW_ERROR("expected the macro defintion but else found"); }

    padv(self);

    if (peol(self)) { THROW_ERROR("expected the macro name but end of line found"); }
    

    token = ppeek(self);
    if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected macro name but `" SV_FMT "` found", SV_UNWRAP(token.text)); }

    PASM_Macro_Def macro = {0};
    macro.name = token.text;

    // consuming the `name` token
    padv(self);

    // parsing the macro operands
    macro.arg_names = pasm_parse_macro_args(self);

    // parsing the macro instructions
    PASM_Nodes block = pasm_parse_macro_block(self, macro.name); 
    macro.block = malloc(sizeof(PASM_Nodes));
    *macro.block = block;

    return (PASM_Node) {  .kind = NODE_KIND_MACRO, .as.macro_def = macro };
}

PASM_Node pasm_parser_parse_preprocess_statement(PASM *self) {
    PASM_Token token = ppeek(self);
    
    if (sv_eq(token.text, SV("const"))) { 
        return pasm_parser_parse_const_declaration(self); 
    }
    
    if (sv_eq(token.text, SV("entry"))) {
        return pasm_parser_parse_entry(self);
    }

    if (sv_eq(token.text, SV("use"))) {
        return pasm_parser_parse_use(self);
    }

    if (sv_eq(token.text, SV("macro"))) {
        return pasm_parser_parse_macro_def(self);
    }

    // it should be a label defintion
    return pasm_parser_parse_label_definition(self);
}

PASM_Node pasm_parser_parse_macro_call(PASM *self) {
    if (peol(self)) { THROW_ERROR("expected macro defintion but end of line found"); }

    PASM_Token token = ppeek(self);
    if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected the macro name but else found"); }
    padv(self);

    PASM_Macro_Call call = {0};
    call.name = token.text;

    call.args = pasm_parser_parse_operands(self);

    return (PASM_Node) { .kind = NODE_KIND_MACRO_CALL, .as.macro_call = call };
}

PASM_Node pasm_parser_parse_statement(PASM *self) {
    PASM_Token token = ppeek(self);

    if (token.kind == TOKEN_KIND_PREPROCESS) { return pasm_parser_parse_preprocess_statement(self); }
    if (token.kind != TOKEN_KIND_ID) { THROW_ERROR("expected token of type `instruction` or `preprocess` but `" SV_FMT "` found", SV_UNWRAP(token.text)); }

    Inst_Kind kind = 0;
    if (is_instruction(token.text, &kind)) { return pasm_parser_parse_instruction(self, kind); }

    // it should be a macro call
    return pasm_parser_parse_macro_call(self);
}

void pasm_parser_parse(PASM *self) {
    DA_INIT(&self->nodes, sizeof(PASM_Node));
    while (!peot(self)) {
        PASM_Token current = ppeek(self);
        if(current.kind == TOKEN_KIND_NEW_LINE || current.kind == TOKEN_KIND_NO_KIND) { padv(self); }
        else {
            PASM_Node node = pasm_parser_parse_statement(self);
            DA_APPEND(&self->nodes, node);
        }
    }
}