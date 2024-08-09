#ifndef PASM_CODE_H
#define PASM_CODE_H

#include "asm-defs.h"
#include "asm-lexer.h"
#include "asm-parser.h"
#include "asm-preprocess.h"
#include "asm-code-processor.h"
#include "asm-printer.h"

#define gencode pasm_generate_bytecode
#define compile pasm_compile

void pasm_generate_bytecode(PASM *self);
void pasm_compile(PASM *self, char *filename);

#endif // PASM_CODE_H