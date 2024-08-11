#ifndef PASM_PREPROCESSOR_H
#define PASM_PREPROCESSOR_H

#include "asm-defs.h"
#include "asm-code.h"
#include "asm-code-processor.h"
#include "asm-init.h"

#define preprocess pasm_preprocess

void pasm_add_program(PASM *self, PASM_Prog prog);
void pasm_preprocess(PASM *self, PASM_Nodes nodes);

#endif