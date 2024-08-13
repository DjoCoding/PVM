#include "asm-init.h"

PASM pasm_init(char *filename) {
    PASM pasm = {0};

    // set the filename
    pasm.filename = filename;

    pasm.lexer.loc = (Location) {1, 1};
    
    // initialize the include files
    DA_INIT(&pasm.sup_files, sizeof(char *));
    DA_INIT(&pasm.files, sizeof(char *));

    // initialize the global pasm context
    pasm_push_context(&pasm);
    
    return pasm;
}

void pasm_push_context(PASM *self) {
    if (self->contexts.count == PASM_CONTEXTS_CAP) { THROW_ERROR("contexts stack overflow, did you just made an infinite recursion?"); }
    hashmap_init(&self->contexts.items[self->contexts.count++].map, sizeof(PASM_Context_Value));
}

void pasm_pop_context(PASM *self) {
    hashmap_destroy(&self->contexts.items[--self->contexts.count].map);
}