#include "asm-preprocess.h"

bool preprocess_kinds[] = {
    false, // NODE_KIND_INSTRUCTION
    true,  // NODE_KIND_LABEL_DEF
    true,  // NODE_KIND_CONST_DEF
    false, // NODE_KIND_ENTRY
    true, // NODE_KIND_USE
};

void pasm_process_const(PASM *self, PASM_Const constant) {
    char *const_name = cstr_from_sv(constant.name);
    int64_t const_value = (int64_t)cstr_from_sv(constant.value);
    pasm_process_const_parts(self, const_name, constant.kind, const_value);  
}

void pasm_process_consts_node(PASM *self, PASM_Node node) {
    for (size_t i = 0; i < node.as.constants.count; ++i) {
        pasm_process_const(self, node.as.constants.items[i]);
    }
}

void pasm_process_label_node(PASM *self, PASM_Node node) {
    char *label_name = cstr_from_sv(node.as.label);
    pasm_process_label_parts(self, label_name, self->prog.count);
}

void pasm_process_context_value(PASM *self, char *name, PASM_Context_Value context_value) {
    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) { 
        pasm_process_label_parts(self, name, context_value.as.label); 
        return;
    }
    
    if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) { 
        pasm_process_const_parts(self, name, context_value.as.constant.type, context_value.as.constant.value);
        return;
    }

    ASSERT(false, "unreachable");
}

void pasm_add_to_context(PASM *self, char *name, PASM_Context_Value value) {
    // the call below will throw an error if the name of already exists in the main context
    pasm_check_identifier_in_context(self, name);

    // add it if no error occured
    hashmap_add(&self->context.map, name, (void *)&value);
}

void pasm_add_context(PASM *self, PASM_Context context) {
    for (size_t i = 0; i < HASH_MAP_SIZE; ++i) {
        HashMap_Node *node = context.map.nodes[i];
        if (!node) { continue; }
        pasm_add_to_context(self, node->key, *(PASM_Context_Value *)node->value);
    }
}

void pasm_add_program(PASM *self, PASM_Prog prog) {
    if (prog.count == 0) { return; }
    if (self->prog.size == 0) { DA_INIT(&self->prog, sizeof(Inst)); }
    
    for (size_t i = 0; i < prog.count; ++i) {
        Inst inst = prog.items[i];
        pasm_process_instruction_parts(self, inst.kind, inst.ops);
        DA_APPEND(&self->prog, inst);
    }
}

void pasm_process_external_file(PASM *self, PASM_Node node) {
    // for now i don't check for the circular uses

    // get the file name
    char *filename = cstr_from_sv(node.as.file_path);

    PASM pasm = {0};

    compile(&pasm, filename);

    // add the context
    pasm_add_context(self, pasm.context);

    // add the prog
    pasm_add_program(self, pasm.prog);

    free(filename);
}

void pasm_preprocess_node(PASM *self, PASM_Nodes nodes, size_t current) {
    if (nodes.items[current].kind == NODE_KIND_CONST_DEF) { 
        pasm_process_consts_node(self, nodes.items[current]);
        return;
    }

    if (nodes.items[current].kind == NODE_KIND_LABEL_DEF) {
        pasm_process_label_node(self, nodes.items[current]);
        return;
    }

    if (nodes.items[current].kind == NODE_KIND_USE) {
        pasm_process_external_file(self, nodes.items[current]);
        return;
    }

    ASSERT(false, "unreachable");
}

void pasm_preprocess(PASM *self) {
    // initialize the hashmap
    hashmap_init(&self->context.map, sizeof(PASM_Context_Value));

    self->prog_size = 0;

    for (size_t i = 0; i < self->nodes.count; ++i) {
        if (preprocess_kinds[self->nodes.items[i].kind]) {
            pasm_preprocess_node(self, self->nodes, i);
        } else { self->prog_size++; }
    }
}