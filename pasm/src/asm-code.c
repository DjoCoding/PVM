#include "asm-code.h"

void pasm_process_instruction(PASM *self, PASM_Node node) {
    pasm_process_instruction_parts(self, node.as.inst.kind, node.as.inst.ops);
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
    if (self->prog.count == 0) { DA_INIT(&self->prog, sizeof(PASM_Inst)); }
    
    for (size_t i = 0; i < self->nodes.count; ++i) {
        if (self->nodes.items[i].kind == NODE_KIND_INSTRUCTION) {
            pasm_process_instruction(self, self->nodes.items[i]);
            DA_APPEND(&self->prog, self->nodes.items[i].as.inst);
            continue;
        }
        
        if (self->nodes.items[i].kind == NODE_KIND_ENTRY) { 
            pasm_set_entry_point(self, self->nodes.items[i]);
            continue;
        }
    }  
}

void pasm_compile(PASM *self, char *filename) {
    self->filename = filename;
    self->lexer.source = sv_from_file(self->filename);
    
    lex(self);
    parse(self);
    
    self->prog.entry.entry_set = false;
    
    preprocess(self);
    gencode(self);
}