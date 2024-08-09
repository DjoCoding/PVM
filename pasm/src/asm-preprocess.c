#include "asm-preprocess.h"

bool preprocess_kinds[] = {
    false, // NODE_KIND_INSTRUCTION
    true,  // NODE_KIND_LABEL_DEF
    true,  // NODE_KIND_CONST_DEF
    false, // NODE_KIND_ENTRY
};

void pasm_process_const(PASM *self, PASM_Const constant) {
    char *const_name = cstr_from_sv(constant.name);
    if (hashmap_find(&self->context.consts.map, const_name)) {
        THROW_ERROR("constant `%s` declared twice", const_name);
    } 
    
    PASM_Context_Const new_const;
    new_const.type = constant.kind;

    if (constant.kind == TYPE_STRING) {
        char *const_value = cstr_from_sv(constant.value);
        new_const.value = (int64_t) const_value;
    } else if (constant.kind == TYPE_NUMBER) {
        int64_t value =  0;
        if (!sv_parse_integer(constant.value, &value)) {
            THROW_ERROR("could not parse `" SV_FMT "` as a number", SV_UNWRAP(constant.value));
        }
        new_const.value = value;
    } else if (constant.kind == TYPE_CHAR) {
        int value = (int)constant.name.content[0];
        new_const.value = (int64_t)value;
    } else {
        THROW_ERROR("type not implemented yet");
    }
    
    hashmap_add(&self->context.consts.map, const_name, &new_const);
    self->context.consts.count++;
}

void pasm_process_consts(PASM *self, PASM_Node node) {
    for (size_t i = 0; i < node.as.constants.count; ++i) {
        pasm_process_const(self, node.as.constants.items[i]);
    }
}

void pasm_process_labels(PASM *self, PASM_Node node) {
    char *label_name = cstr_from_sv(node.as.label);
    if (hashmap_find(&self->context.labels.map, label_name)) {
        THROW_ERROR("label `%s` declared twice", label_name);
    }

    hashmap_add(&self->context.labels.map, label_name, &self->prog_size);
    self->context.labels.count++;
}

void pasm_preprocess_node(PASM *self, PASM_Nodes nodes, size_t current) {
    if (nodes.items[current].kind == NODE_KIND_CONST_DEF) { 
        pasm_process_consts(self, nodes.items[current]);
        return;
    }

    if (nodes.items[current].kind == NODE_KIND_LABEL_DEF) {
        pasm_process_labels(self, nodes.items[current]);
        return;
    }

    ASSERT(false, "unreachable");
}

void pasm_preprocess(PASM *self) {
    // initialize the hashmaps
    hashmap_init(&self->context.consts.map, sizeof(PASM_Context_Const));
    hashmap_init(&self->context.labels.map, sizeof(int64_t));

    self->prog_size = 0;

    for (size_t i = 0; i < self->nodes.count; ++i) {
        if (preprocess_kinds[self->nodes.items[i].kind]) {
            pasm_preprocess_node(self, self->nodes, i);
        } else { self->prog_size++; }
    }
}