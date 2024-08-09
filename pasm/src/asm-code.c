#include "asm-code.h"

void pasm_process_operand(PASM *self, Inst_Op *op) {
    if (op->kind != OP_KIND_ID) { return; }

    char *id = (char *)op->value;

    // search in the hashmaps
    PASM_Context_Const constant = {0};
    if (hashmap_get(&self->context.consts.map, id, &constant)) {
        if (constant.type == TYPE_STRING) { op->kind = OP_KIND_STRING; }
        else if (constant.type == TYPE_CHAR || constant.type == TYPE_NUMBER) { op->kind = OP_KIND_NUMBER; }
        else { THROW_ERROR("type not implemented yet"); }
        op->value = constant.value;
        return;
    }

    size_t ip = 0;
    if (hashmap_get(&self->context.labels.map, id, &ip)) {
        op->kind = OP_KIND_NUMBER;
        op->value = ip;
        return;
    } 
    
    THROW_ERROR("%s not declared yet", id);
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

    // search for the label in the labels context
    // set the ip in the self->prog.entry field

    // get the label name
    char *name = cstr_from_sv(node.as.label);
    
    size_t ip = 0;

    if (!hashmap_get(&self->context.labels.map, name, (void *)&ip)) {
        free(name);
        THROW_ERROR("label `" SV_FMT "` not declared", SV_UNWRAP(node.as.label));
    }

    free(name);

    self->prog.entry.entry_set = true;
    self->prog.entry.ip = ip;
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