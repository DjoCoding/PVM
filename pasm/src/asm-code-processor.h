#ifndef ASM_CODE_PROCESSOR_H
#define ASM_CODE_PROCESSOR_H

#include "asm-defs.h"

PASM_Nodes pasm_process_block(PASM *self, PASM_Nodes nodes);

void pasm_process_const_parts(PASM *self, char *name, PASM_Arg value);
void pasm_process_label_parts(PASM *self, char *label_name, size_t ip);
Inst pasm_process_instruction_parts(PASM *self, Inst_Kind kind, Inst_Ops ops);
PASM_Context pasm_process_macro_call_parts(PASM *self, char *name);

bool pasm_check_file_included(PASM *self, char *filename);
void pasm_add_super_file(PASM *self, char *filename);
void pasm_add_sub_file(PASM *self, char *filename); 
bool pasm_has_super_files(PASM *self);
bool pasm_has_super_file(PASM *self, char *filename);
void pasm_add_super_files(PASM *self, PASM_Super_Files sup_files);

int pasm_has_this_identifier_in_this_context(PASM_Context context, char *name);
int64_t pasm_has_this_identifier_in_contexts(PASM *self, char *name);
PASM_Context_Value pasm_get_context_value_of_identifier(PASM_Context context, char *name);
void pasm_add_context_value_to_context(PASM_Context *context, char *name, PASM_Context_Value context_value);

#endif