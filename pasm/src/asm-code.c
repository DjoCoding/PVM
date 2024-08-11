#include "asm-code.h"

Inst generate_stop_instruction() {
    return (Inst) { .kind = INST_KIND_STOP, .ops = {0} };
}

Program_Inst pasm_process_instruction(PASM *self, PASM_Node node) {
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
        if (op.kind == OP_KIND_NUMBER) argument.type = TYPE_NUMBER;
        else if (op.kind == OP_KIND_STRING) argument.type = TYPE_STRING;
        else { ASSERT(false, "unreachable"); }

        argument.value = op.value;
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
    DA_INIT(&new_nodes, sizeof(PASM_Node));

    for (size_t i = 0; i < nodes.count; ++i) {
        DA_APPEND(&new_nodes, nodes.items[i]);
    }

    return new_nodes;
}

Program_Inst pasm_process_macro_call(PASM *self, PASM_Node node) {
    // add a new context to the self contexts
    // make the mapping between the arguments and the values 
    // generate the bytecode and append it to the program

    // get the macro name
    char *name = cstr_from_sv(node.as.macro_call.name);

    // find the macro name in the current context
    int64_t index = 0;
    if ((index = pasm_has_this_identifier_in_contexts(self, name)) == -1) {
        THROW_ERROR("`%s` macro not defined", name);
    }
    
    // get the context where we think the macro is defined in
    PASM_Context macro_context = self->contexts.items[index];

    // get the context value
    PASM_Context_Value context_value = pasm_get_context_value_of_identifier(macro_context, name);

    if (context_value.type != PASM_CONTEXT_VALUE_TYPE_MACRO) { 
        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
            THROW_ERROR("`%s` is defined as a constant, not a macro", name);
        }

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
            THROW_ERROR("`%s` is defined as a label, not a macro", name);            
        }

        ASSERT(false, "unreachable");
    }

    // add a new scope (context)
    pasm_push_context(self);
    pasm_set_the_new_macro_context(self, name, node.as.macro_call.args, context_value.as.macro.args);

    // pre process the macro
    pasm_preprocess(self, context_value.as.macro.block);
    
    // generate the program out of it 
    PASM_Prog prog = pasm_generate_bytecode(self, context_value.as.macro.block);

    // add the ret instruction to the prog
    DA_APPEND(&prog, ((Program_Inst) { .kind = PROGRAM_INST_INSTRUCTION, .as.inst = generate_stop_instruction() }));
    
    
    PASM_Prog *prog_ptr = malloc(sizeof(PASM_Prog));
    *prog_ptr = prog;

    // pop the context (for scopes and shit)
    pasm_pop_context(self);

    return (Program_Inst) { .kind = PROGRAM_INST_PROGRAM, .as.prog = prog_ptr };
}


PASM_Prog pasm_generate_bytecode(PASM *self, PASM_Nodes nodes) { 
    PASM_Prog prog = {0};
    DA_INIT(&prog, sizeof(Program_Inst));
    
    for (size_t i = 0; i < nodes.count; ++i) {
        if (nodes.items[i].kind == NODE_KIND_INSTRUCTION) {
            Program_Inst inst = pasm_process_instruction(self, nodes.items[i]);
            DA_APPEND(&prog, inst);
            continue;
        }

        if (nodes.items[i].kind == NODE_KIND_MACRO_CALL) {
            Program_Inst inst = pasm_process_macro_call(self, nodes.items[i]);
            DA_APPEND(&prog, inst);
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

    self->prog = gencode(self, self->nodes);
}