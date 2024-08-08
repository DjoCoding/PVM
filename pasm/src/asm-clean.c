// #include "asm-clean.h"

// void pasm_clean(PASM *self) {
//     if (self->prog.items) { free(self->prog.items); }
//     if (self->source.content.content) { free(self->source.content.content); }
//     if (self->strings.items) { free(self->strings.items); }
//     if (self->tokens.items) { free(self->tokens.items); }
// 
//     hashmap_destroy(&self->labels.map);
// }