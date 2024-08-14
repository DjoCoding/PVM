#include "asm-code-processor.h"

bool pasm_check_file_included(PASM *self, char *filename) {
    for (size_t i = 0; i < self->used_files.count; ++i) {
        if (strcmp(filename, self->used_files.items[i]) == 0) { return true; }
    }
    return false;
}

int pasm_has_this_identifier_in_this_context(PASM_Context context, char *name) {
    return (hashmap_find(&context.map, name));
}

// return the index of the context if the identifier is found else return -1
int64_t pasm_has_this_identifier_in_contexts(PASM *self, char *name) {
    size_t i;

    for (i = self->contexts.count - 1; i != (size_t)(-1); --i) {
        if (pasm_has_this_identifier_in_this_context(self->contexts.items[i], name)) { return (int64_t)i; }
    }

    return -1;
}

PASM_Context_Value pasm_get_context_value_of_identifier(PASM_Context context, char *name) {
    PASM_Context_Value context_value = {0};
    hashmap_get(&context.map, name, (void *)&context_value);
    return context_value;
}  

void pasm_add_context_value_to_context(PASM_Context *context, char *name, PASM_Context_Value context_value) {
    if (pasm_has_this_identifier_in_this_context(*context, name)) {
        THROW_ERROR("`%s` identifier already declared", name);
    }
    
    hashmap_add(&context->map, name, (void *)&context_value);
}

PASM_Context pasm_get_current_context(PASM *self) {
    return self->contexts.items[self->contexts.count - 1];
}

void pasm_process_const_parts(PASM *self, char *name, PASM_Arg value) {
    // get the current context
    PASM_Context current_context = pasm_get_current_context(self);

    if (pasm_has_this_identifier_in_this_context(current_context, name)) {
        // get the context value and report errors
        PASM_Context_Value context_value = pasm_get_context_value_of_identifier(current_context, name);

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_MACRO) {
            THROW_ERROR("`%s` already declared as a macro", name);
        }

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
            THROW_ERROR("`%s` already decalred as a label", name);
        }

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
            THROW_ERROR("`%s` constant declared twice", name);
        }

        ASSERT(false, "unreachable");
    }

    // now we add the constant to the current context
    PASM_Context_Const context_const = value;

    PASM_Context_Value context_value;
    context_value.type = PASM_CONTEXT_VALUE_TYPE_CONST;
    context_value.as.constant = context_const;

    pasm_add_context_value_to_context(&current_context, name, context_value);
    self->contexts.items[self->contexts.count - 1] = current_context;
}

void pasm_process_label_parts(PASM *self, char *label_name, size_t ip) {
    PASM_Context current_context = pasm_get_current_context(self);

    if (pasm_has_this_identifier_in_this_context(current_context, label_name)) {
        PASM_Context_Value context_value = pasm_get_context_value_of_identifier(current_context, label_name);

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_MACRO) {
            THROW_ERROR("`%s` already declared as a macro", label_name);
        }

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
            THROW_ERROR("`%s` already decalred as a constant", label_name);
        }

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
            THROW_ERROR("`%s` label declared twice", label_name);
        }

        ASSERT(false, "unreachable");
    }

    PASM_Context_Label new_label = {
        .mark = ip,
    };

    PASM_Context_Value new_value;
    new_value.type = PASM_CONTEXT_VALUE_TYPE_LABEL;
    new_value.as.label = new_label;

    pasm_add_context_value_to_context(&current_context, label_name, new_value);
    self->contexts.items[self->contexts.count - 1] = current_context;
}

Inst_Op pasm_process_operand(PASM *self, Inst_Op op) {
    if (op.kind != OP_KIND_ID) { return op; }

    char *id = (char *)op.value;

    int64_t index = 0;

    if ((index = pasm_has_this_identifier_in_contexts(self, id)) == -1) {
        THROW_ERROR("`%s` not declared yet", id);
    }

    PASM_Context_Value context_value = pasm_get_context_value_of_identifier(self->contexts.items[index], id);

    Inst_Op new_op = {0};

    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
        PASM_Context_Const constant = context_value.as.constant;
        
        if (constant.type == TYPE_STRING) {
            new_op.kind = OP_KIND_STRING;
            new_op.value = (int64_t)constant.as.string;
            return new_op;
        }

        if (constant.type == TYPE_INTEGER) {
            new_op.kind = OP_KIND_NUMBER;
            new_op.value = (int64_t)constant.as.integer;
            return new_op;
        }
        
        if (constant.type == TYPE_CHAR) {
            new_op.kind = OP_KIND_NUMBER;
            new_op.value = (int64_t)constant.as.c;
            return new_op;
        }

        ASSERT(false, "unreachable");
    }


    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
        size_t address = context_value.as.label.mark;
        new_op.kind = OP_KIND_NUMBER;
        new_op.value = (int64_t)address;
        return new_op;
    }

    ASSERT(false, "unreachable");
}

Inst_Ops pasm_process_operands(PASM *self, Inst_Ops ops) {
    Inst_Ops new_ops = {0};
    DA_INIT(&new_ops, sizeof(Inst_Op));
    for (size_t i = 0; i < ops.count; ++i) {
        Inst_Op op = pasm_process_operand(self, ops.items[i]);
        DA_APPEND(&new_ops, op);
    }
    return new_ops;
}

Inst pasm_process_instruction_parts(PASM *self, Inst_Kind kind, Inst_Ops ops) {
    Inst inst = {0};
    inst.kind = kind;
    inst.ops = pasm_process_operands(self, ops);
    return inst;
}

// replace all the identifiers in the block with their value taken from the context of pasm and return the modified nodes
PASM_Nodes pasm_process_block(PASM *self, PASM_Nodes nodes) {
    for (size_t i = 0; i < nodes.count; ++i) {
        PASM_Node node = nodes.items[i];
        
        if (node.kind == NODE_KIND_INSTRUCTION) {
            Inst_Ops ops = node.as.inst.as.inst.ops;
            node.as.inst.as.inst.ops = pasm_process_operands(self, ops);
            nodes.items[i] = node;
        } else if (node.kind == NODE_KIND_MACRO_CALL) {
            Inst_Ops args = node.as.macro_call.args;
            node.as.macro_call.args = pasm_process_operands(self, args);
            nodes.items[i] = node;
        }
    }

    return nodes;
}

// process the macro block (check if it's defined) and return the context this macro live in for later use
PASM_Context pasm_process_macro_call_parts(PASM *self, char *name) {
    // find the macro name in the pasm global context and all sub contexts
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

    return macro_context;
}

void pasm_add_super_file(PASM *self, char *filename) {
    DA_APPEND(&self->sup_files, filename);
}

void pasm_add_super_files(PASM *self, PASM_Super_Files sup_files) {
    for (size_t i = 0; i < sup_files.count; ++i) {
        pasm_add_super_file(self, sup_files.items[i]);
    }
}

void pasm_add_sub_file(PASM *self, char *filename) {
    DA_APPEND(&self->used_files, filename);
}

bool pasm_has_super_files(PASM *self) {
    return self->sup_files.count != 0;
}

bool pasm_has_super_file(PASM *self, char *filename) {
    for (size_t i = 0; i < self->sup_files.count; ++i) {
        if (strcmp(self->sup_files.items[i], filename) == 0) { return true; }
    }
    return false;
}