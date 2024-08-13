#ifndef PASM_CODE_H
#define PASM_CODE_H

#include "asm-defs.h"
#include "asm-printer.h"
#include "asm-lexer.h"
#include "asm-parser.h"
#include "asm-preprocess.h"
#include "asm-code-processor.h"

#define gencode pasm_generate_bytecode
#define compile pasm_compile

PASM_Prog pasm_generate_bytecode(PASM *self, PASM_Nodes nodes);
void pasm_compile(PASM *self);

#endif // PASM_CODE_H