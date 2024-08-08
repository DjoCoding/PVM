#include "asm.h"

void pasm_compile(PASM *self, char *filename) {
    self->filename = filename;
    self->lexer.source = sv_from_file(self->filename);
    lex(self);
    parse(self);
    preprocess(self);
    gencode(self);
}