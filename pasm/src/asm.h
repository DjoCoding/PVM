#ifndef ASM_H
#define ASM_H

#include "asm-defs.h"
#include "asm-lexer.h"
#include "asm-parser.h"
#include "asm-preprocess.h"
#include "asm-code.h"
#include "asm-printer.h"

#define compile pasm_compile

void pasm_compile(PASM *self, char *filename);

#endif // ASM_H