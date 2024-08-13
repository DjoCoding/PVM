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

typedef struct Strings Strings;

struct Strings {
    char **items;
    size_t count;
    size_t size;
};

typedef Program PASM_Prog;

typedef enum PASM_Token_Kind PASM_Token_Kind;
typedef union PASM_Token_As PASM_Token_As;
typedef struct PASM_Token PASM_Token;
typedef struct PASM_Tokens PASM_Tokens;

typedef enum PASM_Context_Value_Type PASM_Context_Value_Type;
typedef union PASM_Context_Value_As PASM_Context_Value_As;
typedef struct PASM_Context_Value PASM_Context_Value;


typedef enum PASM_Arg_Type PASM_Arg_Type;
typedef union PASM_Arg_As PASM_Arg_As;
typedef struct PASM_Arg PASM_Arg;
typedef struct PASM_Args PASM_Args;

typedef struct PASM_Label PASM_Label;

typedef String_Slices PASM_Macro_Arg_Names;

typedef struct PASM_Macro_Def PASM_Macro_Def;
typedef struct PASM_Macro_Call PASM_Macro_Call;

typedef enum PASM_Node_Kind PASM_Node_Kind;
typedef union PASM_Node_As PASM_Node_As;
typedef struct PASM_Node PASM_Node;
typedef struct PASM_Nodes PASM_Nodes;

typedef struct PASM_Lexer PASM_Lexer;
typedef struct PASM_Parser PASM_Parser;

typedef struct PASM_Const PASM_Const;

typedef struct PASM_Consts PASM_Consts;
typedef PASM_Args PASM_Macro_Args;

typedef PASM_Arg PASM_Context_Const;
typedef struct PASM_Context_Macro PASM_Context_Macro;
typedef struct PASM_Context_Label PASM_Context_Label;
typedef struct PASM_Context PASM_Context;
typedef struct PASM_Contexts PASM_Contexts;

typedef PASM_Context PASM_Global_Context;
typedef PASM_Contexts PASM_SubRoutine_Contexts;

typedef Strings PASM_Used_Files;
typedef Strings PASM_Super_Files;

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

enum PASM_Arg_Type { 
    TYPE_STRING = 0,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_ID,  
};

union PASM_Arg_As {
    char *string;
    int64_t integer;
    double flt;
    char c;
    String_View id;
};

struct PASM_Arg {
    PASM_Arg_Type type;
    PASM_Arg_As as;
};

struct PASM_Args {
    PASM_Arg *items;
    size_t count;
    size_t size;
};

struct PASM_Macro_Def {
    String_View name;
    PASM_Macro_Arg_Names arg_names;
    PASM_Nodes *block;
};

struct PASM_Macro_Call {
    String_View name;
    Inst_Ops args;
};

struct PASM_Const {
    String_View name;
    PASM_Arg value;
};

struct PASM_Consts {
    PASM_Const *items;
    size_t count;
    size_t size;
};

enum PASM_Node_Kind {
    NODE_KIND_INSTRUCTION = 0,
    NODE_KIND_LABEL_DEF,
    NODE_KIND_CONST_DEF,
    NODE_KIND_ENTRY,
    NODE_KIND_USE,
    NODE_KIND_MACRO,
    NODE_KIND_MACRO_CALL,
};

union PASM_Node_As {
    Program_Inst inst;
    String_View label;
    PASM_Consts constants;
    PASM_Macro_Def macro_def;
    PASM_Macro_Call macro_call;
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

struct PASM_Context_Label {
    size_t mark;                      // defines the ip where the macro or the label starts
};

struct PASM_Context_Macro {
    char *name;
    PASM_Nodes block;
    PASM_Macro_Arg_Names args;
};

enum PASM_Context_Value_Type {
    PASM_CONTEXT_VALUE_TYPE_LABEL = 0,
    PASM_CONTEXT_VALUE_TYPE_CONST,
    PASM_CONTEXT_VALUE_TYPE_MACRO,
};

union PASM_Context_Value_As {
    PASM_Context_Label label;
    PASM_Context_Const constant;
    PASM_Context_Macro macro;
};

struct PASM_Context_Value {
    PASM_Context_Value_Type type;
    PASM_Context_Value_As as;
};

struct PASM_Context {
    PASM_Context *parent;
    HashMap map;
    size_t count;
};

#define PASM_CONTEXTS_CAP 100

struct PASM_Contexts {
    PASM_Context items[PASM_CONTEXTS_CAP];
    size_t count;
};

struct PASM {
    char *filename;

    PASM_Tokens tokens;
    PASM_Nodes nodes;

    PASM_Lexer lexer;
    PASM_Parser parser;
    
    PASM_Contexts contexts;                            // the context of the program
    // PASM_SubRoutine_Contexts sub_contexts;           // sub routine contexts

    PASM_Super_Files sup_files;                      // if a file `a` includes a file `b` then the file `a` is a super file of `b`
    PASM_Used_Files files;                           // for the files used in the this current pasm file

    PASM_Prog prog;
    size_t prog_size;                                // used for preprocessing
};


#endif // PASM_DEFS_H