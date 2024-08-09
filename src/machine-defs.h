#ifndef MACHINE_DEFINITION_H
#define MACHINE_DEFINITION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../tools/dyn-arr.h"
#include "../tools/sv.h"


typedef struct Memory_Cell Memory_Cell;
typedef struct Machine_Memory Machine_Memory;
typedef struct Machine_Stack Machine_Stack;
typedef enum Machine_State Machine_State;
typedef struct Machine_SubRoutine_Stack Machine_SubRoutine_Stack;
typedef String_Slices Machine_String_Stack;
typedef struct Machine Machine;

struct Memory_Cell {
    char *data;
    size_t size;
};

struct Machine_Memory {
    Memory_Cell *items;
    size_t count;
    size_t size;
};

struct Machine_Stack {
    int64_t *items;
    size_t count;
    size_t size;
};

#define SUBROUTINE_STACK_CAP 1000

struct Machine_SubRoutine_Stack {
    size_t items[SUBROUTINE_STACK_CAP];
    size_t count;
};


struct Machine {
    Machine_Stack stack;
    Machine_Memory memory;
    Machine_String_Stack str_stack; // will be used to load pre-defined strings from files
    Machine_SubRoutine_Stack subroutines;
    bool halted;
    size_t ip;
};


typedef enum Op_Kind Op_Kind;
typedef struct Inst_Op Inst_Op;
typedef enum Inst_Kind Inst_Kind;
typedef struct Inst_Ops Inst_Ops;
typedef struct Inst Inst;
typedef struct Program Program;

typedef enum SysCall_ID SysCall_ID;

enum SysCall_ID {
    SYSCALL_WRITE = 0,
    SYSCALL_READ,
};

enum Inst_Kind {
    INST_KIND_NOP,
    INST_KIND_HALT,
    INST_KIND_PUSH,
    INST_KIND_PUSHS,
    INST_KIND_POP,
    INST_KIND_ADD,
    INST_KIND_SUB,
    INST_KIND_MUL,
    INST_KIND_DIV,
    INST_KIND_MOD,
    INST_KIND_SWAP,
    INST_KIND_DUP,
    INST_KIND_INSWAP,
    INST_KIND_INDUP,
    INST_KIND_SYSCALL,
    INST_KIND_JMP,
    INST_KIND_CMP,
    INST_KIND_JZ,
    INST_KIND_JLE,
    INST_KIND_JGE,
    INST_KIND_JL,
    INST_KIND_JG,
    INST_KIND_PUTC,
    INST_KIND_CALL,
    INST_KIND_RET,
    INST_KIND_NO_KIND,
};

enum Op_Kind {
    OP_KIND_NUMBER = 0,
    OP_KIND_STRING,
    OP_KIND_ID,  // needed for parsing
};  

struct Inst_Op {
    Op_Kind kind;
    int64_t value;
}; 

struct Inst_Ops {
    Inst_Op *items;
    size_t count;
    size_t size;
};

struct Inst {
    Inst_Kind kind;
    Inst_Ops ops;
};

struct Program {
    Inst *items;
    size_t count;
    size_t size;
    size_t entry;
};


#endif // MACHINE_DEFINITION_H