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
typedef enum Program_State Program_State;

typedef struct Program Program;

typedef String_Slices Machine_String_Stack;

typedef struct Machine_Programs Machine_Programs;

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

// struct Machine_SubRoutine {
//     Machine_SubRoutine_Kind kind; // label or macro jump
//     Machine_SubRoutine_Map map;   // map between the 
// };

struct Machine_SubRoutine_Stack {
    size_t items[SUBROUTINE_STACK_CAP]; 
    size_t count;
};

struct Machine_Programs {
    Program *items;
    size_t count;
    size_t size;
};

struct Machine {
    Machine_Stack stack;
    Machine_Memory memory;
    Machine_String_Stack str_stack; // will be used to load pre-defined strings from files
    Machine_SubRoutine_Stack subroutines;
    Machine_Programs progs;
    bool halted;
    size_t ip;
};


typedef enum Inst_Kind Inst_Kind;
typedef enum Op_Kind Op_Kind;
typedef struct Inst_Op Inst_Op;
typedef struct Inst_Ops Inst_Ops; 
typedef struct Inst Inst;

typedef enum Program_Inst_Kind Program_Inst_Kind;
typedef union Program_Inst_As Program_Inst_As;
typedef struct Program_Inst Program_Inst;

typedef struct Program_Entry Program_Entry;

typedef struct Program Program;

typedef enum SysCall_ID SysCall_ID;
typedef enum Open_File_Mode Open_File_Mode;

enum Open_File_Mode {
    READ_MODE = 0,
    WRITE_MODE,
    APPEND_MODE,
    READ_WRITE_MODE,
    WRITE_READ_MODE,            // erase the file once opened
    READ_APPEND_MODE,
    READ_BIN_MODE,
    WRITE_BIN_MODE,
    APPEND_BIN_MODE,
    READ_WRITE_BIN_MODE,
    WRITE_READ_BIN_MODE,
    READ_APPEND_BIN_MODE,
    MODES_COUNT,
};

enum SysCall_ID {
    SYSCALL_WRITE = 0,
    SYSCALL_READ,
    SYSCALL_ALLOC,
    SYSCALL_FREE,
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_EXIT,
    SYCALL_COUNT,
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
    INST_KIND_STOP,
    INST_KIND_SMEM,
    INST_KIND_GMEM,
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

enum Program_Inst_Kind {
    PROGRAM_INST_INSTRUCTION = 0,
    PROGRAM_INST_PROGRAM,
};

union Program_Inst_As {
    Inst inst;
    Program *prog;
};

struct Program_Inst {
    Program_Inst_Kind kind;
    Program_Inst_As as;
};

struct Program_Entry {
    size_t ip;
    bool entry_set;
};

enum Program_State {
    PROGRAM_STATE_NOT_EXECUTED_YET,
    PROGRAM_STATE_PENDING,
    PROGRAM_STATE_EXECTUED,
};

struct Program {
    Program_Inst *items;
    size_t count;
    size_t size;

    Program_Entry entry;
    Program_State state;

    size_t ret_ip;
    size_t prog_size; // use to determine the number of instructions in the program for marking the labels -- this concerns PASM more
};

#endif // MACHINE_DEFINITION_H
