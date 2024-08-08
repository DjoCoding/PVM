#ifndef PASM_DEFS_H
#define PASM_DEFS_H

#include "../../src/machine-defs.h"
#include "../../tools/sv.h"
#include "../../tools/hashmap.h"
#include "../../tools/dyn-arr.h"
#include "../../tools/error.h"

typedef struct Location Location;

struct Location {
    size_t line;
    size_t col;
};

#define LOC_FMT         "(%zu, %zu)"
#define LOC_UNWRAP(loc) loc.line, loc.col

typedef Inst PASM_Inst; 
typedef Program PASM_Prog;

typedef struct PASM_Const_Pool PASM_Const_Pool;
typedef struct PASM_Labels PASM_Labels;
typedef struct PASM_Context PASM_Context;

typedef enum PASM_Token_Kind PASM_Token_Kind;
typedef union PASM_Token_As PASM_Token_As;
typedef struct PASM_Token PASM_Token;
typedef struct PASM_Tokens PASM_Tokens;

typedef enum PASM_Type PASM_Type;

typedef struct PASM_Context_Const PASM_Context_Const;

typedef struct PASM_Const PASM_Const;
typedef struct PASM_Consts PASM_Consts;

typedef struct PASM_Label PASM_Label;

typedef union PASM_Node_As PASM_Node_As;
typedef enum PASM_Node_Kind PASM_Node_Kind;
typedef struct PASM_Node PASM_Node;
typedef struct PASM_Nodes PASM_Nodes;

typedef struct PASM_Lexer PASM_Lexer;
typedef struct PASM_Parser PASM_Parser;

typedef String_Slices PASM_Strings;

typedef struct PASM PASM;

struct PASM_Labels {
    HashMap map;
    size_t count;
};

enum PASM_Token_Kind {
    TOKEN_KIND_STRING,
    TOKEN_KIND_NUMBER,
    TOKEN_KIND_CHAR,
    TOKEN_KIND_NEW_LINE,
    TOKEN_KIND_COLON,
    TOKEN_KIND_PREPROCESS,
    TOKEN_KIND_ID,
    TOKEN_KIND_COMMA,
    TOKEN_KIND_NO_KIND,
};

union PASM_Token_As {
    String_View text;
    Inst_Kind inst;
};

struct PASM_Token {
    PASM_Token_Kind kind;
    String_View text;    
    Location loc;  
};

struct PASM_Tokens {
    PASM_Token *items;
    size_t count;
    size_t size;
    size_t current;
};

enum PASM_Type {
    TYPE_STRING = 0,
    TYPE_NUMBER,
    TYPE_CHAR, 
};

struct PASM_Const {
    PASM_Type kind;
    String_View name;
    String_View value;
};

enum PASM_Node_Kind {
    NODE_KIND_INSTRUCTION = 0,
    NODE_KIND_LABEL_DEF,
    NODE_KIND_CONST_DEF,
};

struct PASM_Consts {
    PASM_Const *items;
    size_t count;
    size_t size;
};

union PASM_Node_As {
    PASM_Inst inst;
    String_View label;
    PASM_Consts constants;
};

struct PASM_Node {
    PASM_Node_Kind kind;
    PASM_Node_As as;
};

struct PASM_Nodes {
    PASM_Node *items;
    size_t count;
    size_t size;
};


struct PASM_Lexer {
    String_View source;
    Location loc;
    size_t current;
};

struct PASM_Parser {
    size_t current;
};

struct PASM_Context_Const {
    PASM_Type type;
    int64_t value;
};

struct PASM_Const_Pool {
    HashMap map;
    size_t count;
};

struct PASM_Context {
    PASM_Const_Pool consts;
    PASM_Labels labels;
};

struct PASM {
    char *filename;
    PASM_Strings strings;
    PASM_Tokens tokens;
    PASM_Nodes nodes;
    PASM_Lexer lexer;
    PASM_Parser parser;

    PASM_Context context;
    PASM_Prog prog;

    size_t prog_size; // used for preprocessing
};



#endif // PASM_DEFS_H