#ifndef DEPASM_H
#define DEPASM_H

#include <stdio.h>
#include "../../src/machine.h"
#include "../../tools/sv.h"

void depasm(FILE *f, Program prog);
void depasm_clean_prog(Program prog);

#endif 