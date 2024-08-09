#include "asm.h"

void pasm_compile(PASM *self, char *filename) {
    self->filename = filename;
    self->lexer.source = sv_from_file(self->filename);
    

    lex(self);
    parse(self);

    self->prog.entry.entry_set = false;

    preprocess(self);
    gencode(self);
}