#include "asm-preprocess.h"

bool preprocess_kinds[] = {
    false, // NODE_KIND_INSTRUCTION
    true,  // NODE_KIND_LABEL_DEF
    true,  // NODE_KIND_CONST_DEF
    false, // NODE_KIND_ENTRY
};

void pasm_process_const(PASM *self, PASM_Const constant) {
    char *const_name = cstr_from_sv(constant.name);
    
    PASM_Context_Value context_value = {0};
    if (hashmap_get(&self->context.map, const_name, (void *)&context_value)) {
        free(const_name);

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_LABEL) {
            THROW_ERROR("constant `" SV_FMT "` already declared as a label", SV_UNWRAP(constant.name));
        }

        THROW_ERROR("constant `" SV_FMT "` declared twice", SV_UNWRAP(constant.name));
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

    context_value.type = PASM_CONTEXT_VALUE_TYPE_CONST;
    context_value.as.constant = new_const;
    

    hashmap_add(&self->context.map, const_name, (void *)&context_value);
    self->context.count++;
}

void pasm_process_consts(PASM *self, PASM_Node node) {
    for (size_t i = 0; i < node.as.constants.count; ++i) {
        pasm_process_const(self, node.as.constants.items[i]);
    }
}

void pasm_process_labels(PASM *self, PASM_Node node) {
    char *label_name = cstr_from_sv(node.as.label);

    PASM_Context_Value context_value = {0};
    if (hashmap_get(&self->context.map, label_name, (void *)&context_value)) {
        free(label_name);

        if (context_value.type == PASM_CONTEXT_VALUE_TYPE_CONST) {
            THROW_ERROR("label `" SV_FMT "` already declared as a constant", SV_UNWRAP(node.as.label));
        }

        THROW_ERROR("label `" SV_FMT "` declared twice", SV_UNWRAP(node.as.label));
    }

    context_value.type = PASM_CONTEXT_VALUE_TYPE_LABEL;
    context_value.as.label = self->prog_size;

    hashmap_add(&self->context.map, label_name, (void *)&context_value);
    self->context.count++;
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
    // initialize the hashmap
    hashmap_init(&self->context.map, sizeof(PASM_Context_Value));

    self->prog_size = 0;

    for (size_t i = 0; i < self->nodes.count; ++i) {
        if (preprocess_kinds[self->nodes.items[i].kind]) {
            pasm_preprocess_node(self, self->nodes, i);
        } else { self->prog_size++; }
    }
}