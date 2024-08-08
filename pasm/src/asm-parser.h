#ifndef PASM_PARSER_H
#define PASM_PARSER_H

#include "asm-defs.h"
#include "asm-printer.h"
#include "asm-clean.h"

#define MAKE_STRING_OPERAND(s)           ((Inst_Op) { .kind = OP_KIND_STRING,  .value = (int64_t)(s)  })
#define MAKE_NUMBER_OPERAND(i)           ((Inst_Op) { .kind = OP_KIND_NUMBER , .value = (int64_t)(i)  })
#define MAKE_ID_OPERAND(id)              ((Inst_Op) { .kind = OP_KIND_ID,      .value = (int64_t)(id) })

#define ppeek pasm_parser_peek
#define padv  pasm_parser_advance
#define peot  pasm_parser_eot
#define peol  pasm_parser_eol
#define parse pasm_parser_parse

void pasm_parser_parse(PASM *self);

#endif