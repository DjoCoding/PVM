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

typedef enum PASM_Context_Value_Type PASM_Context_Value_Type;
typedef union PASM_Context_Value_As PASM_Context_Value_As;
typedef struct PASM_Context_Value PASM_Context_Value;
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

typedef String_Slices PASM_External_Files;

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
    NODE_KIND_ENTRY,
    NODE_KIND_USE,
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
    String_View file_path;
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

enum PASM_Context_Value_Type {
    PASM_CONTEXT_VALUE_TYPE_LABEL = 0,
    PASM_CONTEXT_VALUE_TYPE_CONST,
};

union PASM_Context_Value_As {
    size_t label; // for the label mark
    PASM_Context_Const constant;
};

struct PASM_Context_Value {
    PASM_Context_Value_Type type;
    PASM_Context_Value_As as;
};

struct PASM_Context {
    HashMap map;
    size_t count;
};

struct PASM {
    char *filename;
    
    PASM_Tokens tokens;
    PASM_Nodes nodes;

    PASM_Lexer lexer;
    PASM_Parser parser;
    
    PASM_Context context; // the global context of the program
    
    PASM_Prog prog;
    size_t prog_size;     // used for preprocessing
};



#endif // PASM_DEFS_H