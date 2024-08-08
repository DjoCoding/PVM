#ifndef ASM_LEXER_H
#define ASM_LEXER_H

#include "asm-defs.h"
#include "asm-clean.h"
#include <ctype.h>

#define lpeek    pasm_lexer_peek
#define ladvance pasm_lexer_advance
#define leof     pasm_lexer_eof
#define lread    pasm_lexer_read_token
#define lex      pasm_lexer_lex

void pasm_lexer_lex(PASM *self);

#endif // ASM_LEXER_H