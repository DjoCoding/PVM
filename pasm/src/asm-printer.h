#ifndef PASM_PRINTER_H
#define PASM_PRINTER_H

#include "asm-defs.h"

char *token_kind_to_cstr(PASM_Token_Kind kind);
void pasm_print_tokens(PASM *self);
void pasm_print_nodes(PASM *self);


#endif // PASM_PRINTER_H