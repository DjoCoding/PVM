#ifndef ASM_INIT_H
#define ASM_INIT_H

#include "asm-defs.h"

PASM pasm_init(char *filename);
void pasm_push_context(PASM *self);
void pasm_pop_context(PASM *self);

#endif 