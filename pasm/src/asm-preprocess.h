#ifndef PASM_PREPROCESSOR_H
#define PASM_PREPROCESSOR_H

#include "asm-defs.h"
#include "asm-code.h"
#include "asm-code-processor.h"

#define preprocess pasm_preprocess

void pasm_preprocess(PASM *self);

#endif