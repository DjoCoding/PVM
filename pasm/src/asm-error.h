#ifndef ASM_ERROR_H
#define ASM_ERROR_H

#include "asm-defs.h"

#define PASM_ERROR_LOC(filename, loc, ...) \
    do { \
        fprintf(stderr, "%s:" LOC_FMT ": ", filename, LOC_UNWRAP(loc)); \
        THROW_ERROR(__VA_ARGS__); \
    } while(0)

#define PASM_ERROR(filename, ...) \
    do { \
        fprintf(stderr, "%s:", filename); \
        THROW_ERROR(__VA_ARGS__); \
    } while (0)

#define PASM_ERROR_LINE(filename, line, ...) \
    do { \
        fprintf(stderr, "%s:%zu: ", filename, line); \
        THROW_ERROR(__VA_ARGS__); \
    } while (0)    

#endif 
