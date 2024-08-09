#include "asm-code.h"

void pasm_process_operand(PASM *self, Inst_Op *op) {
    if (op->kind != OP_KIND_ID) { return; }

    char *id = (char *)op->value;

    PASM_Context_Value context_value = {0};

    if (!hashmap_get(&self->context.map, id, (void *)&context_value)) {
        THROW_ERROR("`%s` not declared yet", id);
    }

    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
        PASM_Context_Const constant = context_value.as.constant;
        
        if (constant.type == TYPE_STRING) {
            op->kind = OP_KIND_STRING;
            op->value = (int64_t)constant.value;
            return;
        }

        if (constant.type == TYPE_NUMBER) {
            op->kind = OP_KIND_NUMBER;
            op->value = constant.value;
            return;
        }
        
        if (constant.type == TYPE_CHAR) {
            op->kind = OP_KIND_NUMBER;
            op->value = constant.value;
            return;
        }

        ASSERT(false, "unreachable");
    }


    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
        size_t address = context_value.as.label;
        op->kind = OP_KIND_NUMBER;
        op->value = (int64_t)address;
        return;
    }

    ASSERT(false, "unreachable");
}

void pasm_process_instruction(PASM *self, PASM_Node node) {
    Inst_Ops ops = node.as.inst.ops;
    for (size_t i = 0; i < ops.count; ++i) {
        pasm_process_operand(self, &ops.items[i]);
    }
}

void pasm_set_entry_point(PASM *self, PASM_Node node) {
    if (self->prog.entry.entry_set) {
        THROW_ERROR("entry point already set at %zu", self->prog.entry.ip);
    }

    // get the label name
    char *name = cstr_from_sv(node.as.label);
    
    PASM_Context_Value context_value = {0};

    if (!hashmap_get(&self->context.map, name, (void *)&context_value)) {
        free(name);
        THROW_ERROR("label `" SV_FMT "` not declared", SV_UNWRAP(node.as.label));
    }

    free(name);

    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
        THROW_ERROR("expected a label but got a constant `" SV_FMT "`", SV_UNWRAP(node.as.label));
    }

    self->prog.entry.entry_set = true;
    self->prog.entry.ip = context_value.as.label;
}

void pasm_generate_bytecode(PASM *self) {   
    DA_INIT(&self->prog, sizeof(Inst));
    for (size_t i = 0; i < self->nodes.count; ++i) {
        if (self->nodes.items[i].kind == NODE_KIND_INSTRUCTION) {
            pasm_process_instruction(self, self->nodes.items[i]);
            DA_APPEND(&self->prog, self->nodes.items[i].as.inst);
        } else if (self->nodes.items[i].kind == NODE_KIND_ENTRY) { 
            pasm_set_entry_point(self, self->nodes.items[i]);
        }
    }  
}