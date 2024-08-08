#ifndef PASM_LEXER_H
#define PASM_LEXER_H

#include "asm-lexer.h"

static char pasm_lexer_peek(PASM *self) {
    return (self->lexer.source.content[self->lexer.current]);
}

static void pasm_lexer_advance(PASM *self) {
    self->lexer.current++;
    self->lexer.loc.col++;
}

static bool pasm_lexer_eof(PASM *self) {
    return (self->lexer.source.count <= self->lexer.current);
}

static PASM_Token pasm_lexer_lex_string(PASM *self) {
    PASM_Token token = {0};
    token.loc = self->lexer.loc;

    char *begin = &self->lexer.source.content[self->lexer.current];
    size_t size = 0;

    // consuming the `"` char
    ladvance(self); 

    // we want the string only without quotes
    begin++;
    
    char current = 0; // added for checks later

    while (!leof(self)) {
        current = lpeek(self);
        if (current == '\n') { THROW_ERROR("string `" SV_FMT "...` contains a new line char", SV_UNWRAP(SV_GET(begin, size))); }
        ladvance(self);
        if (current == '"') { break; }
        size++;
    }

    if (current != '"') {
        String_View org = SV_GET(begin, size);
        String_View unescape = unescape_string_to_sv(org);
        THROW_ERROR(LOC_FMT ": failed to parse the string \"" SV_FMT "\"\n", LOC_UNWRAP(token.loc), SV_UNWRAP(unescape));
        free(unescape.content);
        return token;
    }

    token.kind = TOKEN_KIND_STRING;
    token.text = SV_GET(begin, size);

    return token;
}

static PASM_Token pasm_lexer_lex_id(PASM *self) {
    PASM_Token token;
    token.loc = self->lexer.loc;
    
    char *begin = &self->lexer.source.content[self->lexer.current];
    size_t size = 0;

    char current = lpeek(self);

    if (isalpha(current)) {
        while (!leof(self)) {
            current = lpeek(self);
            if (!isalnum(current) && current != '_') { break; }
            size++;
            ladvance(self);
        }
        token.kind = TOKEN_KIND_ID;
        token.text = SV_GET(begin, size);
        return token;
    }

    ASSERT(false, "could not parse this, this isn't an id");
}

static PASM_Token pasm_lexer_lex_number(PASM *self) {
    PASM_Token token;
    token.loc = self->lexer.loc;
    
    char *begin = &self->lexer.source.content[self->lexer.current];
    size_t size = 0;

    char current = lpeek(self);

    if (isdigit(current)) {
        while (!leof(self)) {
            current = lpeek(self);
            if (!isdigit(current)) { break; }
            size++;
            ladvance(self);
        }
        token.kind = TOKEN_KIND_NUMBER;
        token.text = SV_GET(begin, size);
        return token;
    }

    ASSERT(false, "could not parse this, this isn't an id");
}

static PASM_Token pasm_lexer_read_token(PASM *self) {
    PASM_Token token = {0};
    token.loc = self->lexer.loc;
    char current = lpeek(self);

    if (current == '\n') {
        ladvance(self);
        self->lexer.loc.line++;
        self->lexer.loc.col = 1;
        token.kind = TOKEN_KIND_NEW_LINE;
        return token;
    }

    if (isspace(current)) {
        ladvance(self);
        token.kind = TOKEN_KIND_NO_KIND;
        return token;
    }

    if (current == '#') {
        ladvance(self);
        token = pasm_lexer_lex_id(self);
        token.kind = TOKEN_KIND_PREPROCESS;
        return token;
    }

    if (current == ',') {
        ladvance(self);
        token.kind = TOKEN_KIND_COMMA;
        return token;
    }

    if (current == ':') {
        ladvance(self);
        token.kind = TOKEN_KIND_COLON;
        return token;
    }

    if (current == '$') {
        ladvance(self);
        return pasm_lexer_lex_id(self);
    }

    if (current == '"') { return pasm_lexer_lex_string(self); }

    if (isdigit(current)) { return pasm_lexer_lex_number(self); }

    if (isalpha(current)) { return pasm_lexer_lex_id(self); }

    if (current == '-') {
        TODO("handle negative numbers");
    }

    if (current == '\'') {
        TODO("add chars to the lexer");
    }

    THROW_ERROR(LOC_FMT ": failed to identify the char `%c`\n", LOC_UNWRAP(token.loc), current);
}

void pasm_lexer_lex(PASM *self) {
    DA_INIT(&self->tokens, sizeof(PASM_Token));
    while (!leof(self)) {
        char current = lpeek(self);
        if (current == ' ' || current == '\t') { ladvance(self); }
        else {
            PASM_Token token = lread(self);
            DA_APPEND(&self->tokens, token);
        }
    }
}

#endif // PASM_LEXER_H