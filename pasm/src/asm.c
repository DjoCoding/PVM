#include "asm.h"

void pasm_compile(PASM *self, char *filename) {
    self->filename = filename;
    self->lexer.source = sv_from_file(self->filename);
    

    lex(self);
    parse(self);

    // set the entry point of the program to 0 by default
    self->prog.entry = 0;

    preprocess(self);
    gencode(self);
}