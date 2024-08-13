#include "asm-code.h"

Inst generate_stop_instruction() {
    return (Inst) { .kind = INST_KIND_STOP, .ops = {0} };
}

Inst pasm_process_instruction(PASM *self, PASM_Node node) {
    return pasm_process_instruction_parts(self, node.as.inst.as.inst.kind, node.as.inst.as.inst.ops);
}

void pasm_set_entry_point(PASM *self, PASM_Prog *prog, PASM_Node node) {
    if (prog->entry.entry_set) {
        THROW_ERROR("entry point already set at %zu", prog->entry.ip);
    }

    // get the label name
    char *name = cstr_from_sv(node.as.label);
    
    PASM_Context_Value context_value = {0};

    if (!hashmap_get(&self->contexts.items[0].map, name, (void *)&context_value)) {
        free(name);
        THROW_ERROR("label `" SV_FMT "` not declared", SV_UNWRAP(node.as.label));
    }

    free(name);

    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
        THROW_ERROR("expected a label but got a constant `" SV_FMT "`", SV_UNWRAP(node.as.label));
    }

    prog->entry.entry_set = true;
    prog->entry.ip = context_value.as.label.mark;
}

PASM_Context_Value pasm_get_context_value_from_operand(Inst_Op op) {
    PASM_Context_Value context_value = {0};
        
    PASM_Context_Const argument = {0};
    {
        if (op.kind == OP_KIND_NUMBER) {
            argument.type = TYPE_INTEGER;
            argument.as.integer = op.value;
        }
        else if (op.kind == OP_KIND_STRING) {
            argument.type = TYPE_STRING;
            argument.as.string = (char *)op.value;
        } 
        else if (op.kind == OP_KIND_ID) {
            TODO("handle this case");
        }
    }

    context_value.type = PASM_CONTEXT_VALUE_TYPE_CONST;
    context_value.as.constant = argument;

    return context_value;
}

// this will associate the operands to their values
void pasm_set_the_new_macro_context(PASM *self, char *name, Inst_Ops values, PASM_Macro_Arg_Names names) {
    if (values.count != names.count) { THROW_ERROR("invalid number of paramaters passed to macro %s", name); }

    // get the current context
    PASM_Context context = self->contexts.items[self->contexts.count - 1];

    for (size_t i = 0; i < values.count; ++i) {
        PASM_Context_Value context_value = pasm_get_context_value_from_operand(values.items[i]);
        char *arg_name = cstr_from_sv(names.items[i]);
        pasm_add_context_value_to_context(&context, arg_name, context_value);
        free(arg_name);
    }

    self->contexts.items[self->contexts.count - 1] = context;
}

PASM_Nodes pasm_get_nodes_copy(PASM_Nodes nodes) {
    PASM_Nodes new_nodes = {0};
    new_nodes.items = malloc(sizeof(PASM_Node) *nodes.count);
    new_nodes.count = nodes.count;
    new_nodes.size = new_nodes.size;
    memcpy(new_nodes.items, nodes.items, sizeof(PASM_Node) * nodes.count);
    return new_nodes;
}

PASM_Prog *pasm_process_macro_call(PASM *self, PASM_Node node) {
    // get the macro name
    char *name = cstr_from_sv(node.as.macro_call.name);
    
    // replace the identifiers with their actual values in the macro block and the macro args
    PASM_Context context = pasm_process_macro_call_parts(self, name);
    PASM_Context_Value context_value = pasm_get_context_value_of_identifier(context, name);

    Inst_Ops args = node.as.macro_call.args;
    PASM_Nodes macro_block = context_value.as.macro.block;

    // add a new scope (context)
    pasm_push_context(self);

    // set the new context
    pasm_set_the_new_macro_context(self, name, args, context_value.as.macro.args);

    // pre process the macro (search for constant and labels declarations and evaluate them)
    size_t prog_size = self->prog_size;
    self->prog_size = 0; // for starting a new program // that's for marking the labels
    pasm_preprocess(self, context_value.as.macro.block);
    self->prog_size = prog_size; // getting back our prog size

    // replace all the identifiers with their actual values in the macro block    
    macro_block = pasm_process_block(self, macro_block);

    // set it to the context_value 
    context_value.as.macro.block = macro_block;

    // update the hashmap
    hashmap_update(&context.map, name, (void *)&context_value);

    // generate the program out of it 
    PASM_Prog prog = pasm_generate_bytecode(self, context_value.as.macro.block);

    // add the stop instruction to the prog
    DA_APPEND(&prog, ((Program_Inst) { .kind = PROGRAM_INST_INSTRUCTION, .as.inst = generate_stop_instruction() }));

    PASM_Prog *prog_ptr = malloc(sizeof(PASM_Prog));
    *prog_ptr = prog;

    // pop the context (for scopes and shit)
    pasm_pop_context(self);

    return prog_ptr;
}


PASM_Prog pasm_generate_bytecode(PASM *self, PASM_Nodes nodes) { 
    PASM_Prog prog = {0};
    DA_INIT(&prog, sizeof(Program_Inst));
    
    for (size_t i = 0; i < nodes.count; ++i) {
        if (nodes.items[i].kind == NODE_KIND_INSTRUCTION) {
            Inst inst = pasm_process_instruction(self, nodes.items[i]);
            DA_APPEND(&prog, ((Program_Inst) { .kind = PROGRAM_INST_INSTRUCTION, .as.inst = inst }));
            continue;
        }

        if (nodes.items[i].kind == NODE_KIND_MACRO_CALL) {
            PASM_Prog *macro = pasm_process_macro_call(self, nodes.items[i]);
            DA_APPEND(&prog, ((Program_Inst) { .kind = PROGRAM_INST_PROGRAM, .as.prog = macro }));
            continue;
        }
        
        if (nodes.items[i].kind == NODE_KIND_ENTRY) { 
            pasm_set_entry_point(self, &prog, nodes.items[i]);
            continue;
        }
    }  

    return prog;
}

void pasm_compile(PASM *self) {
    self->lexer.source = sv_from_file(self->filename);
    
    lex(self);
    parse(self);
    preprocess(self, self->nodes);    
    pasm_process_block(self, self->nodes);

    self->prog = gencode(self, self->nodes);
}