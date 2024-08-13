#ifndef ASM_H
#define ASM_H

#include "asm-code.h"

void pasm_write_prog_parts_to_file(FILE *f, Program prog);
void pasm_store_prog_in_file(char *filepath, PASM_Prog prog);

#endif