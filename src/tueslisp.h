#ifndef __TUESLISP_H
#define __TUESLISP_H

#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__);

#define RETNULL(X) if(X == NULL) {                                \
        eprintf("%s %d: retnnulled.\n", __FILE__, __LINE__);        \
        return NULL;                                                \
    }                                                                \

/* The end of file type should never be external. */
/* It is for internal use only. */
typedef enum Type {
    END_OF_FILE = EOF,
    NIL = 0,
    SYMBOL,
    PAIR,
    NUMBER,
    BUILTIN,
    MACRO,
    LAMBDA,
} Type;

#define FOR_EVERY_BUILTIN(DO)                        \
    DO(BUILTIN_NIL_P,      "nil?")                \
    DO(BUILTIN_SYMBOL_P,   "symbol?")        \
    DO(BUILTIN_PAIR_P,     "pair?")                \
    DO(BUILTIN_NUMBER_P,   "number?")        \
    DO(BUILTIN_BUILTIN_P,  "builtin?")        \
    DO(BUILTIN_MACRO_P,    "macro?")        \
    DO(BUILTIN_LAMBDA_P,   "lambda?")        \
    DO(BUILTIN_QUOTE,      "quote")                \
    DO(BUILTIN_CONS,       "cons")                \
    DO(BUILTIN_CAR,        "car")                \
    DO(BUILTIN_CDR,        "cdr")                \
    DO(BUILTIN_QUASIQUOTE, "quasiquote")        \
    DO(BUILTIN_DEFSYM,     "defsym")        \
    DO(BUILTIN_EVAL,       "eval")                \
    DO(BUILTIN_PROGN,      "progn")                \
    DO(BUILTIN_SET,        "set!")                \
    DO(BUILTIN_LAMBDA,     "lambda")        \
    DO(BUILTIN_MACRO,      "macro")                \
    DO(BUILTIN_IF,         "if")                \
    DO(BUILTIN_PLUS,       "plus")                \
    DO(BUILTIN_MINUS,      "minus")                \
    DO(BUILTIN_EQUAL,      "equal")                \
    DO(BUILTIN_PRINT,      "print")                \

#define ENUM_BUILTIN(IN, EX) IN,
typedef enum Builtin {
    FOR_EVERY_BUILTIN(ENUM_BUILTIN)
} Builtin;
#undef ENUM_BUILTIN

typedef struct Any Any;
typedef struct Symbol Symbol;
typedef struct Pair Pair;
typedef struct Number Number;
typedef struct Callable Callable;

struct Symbol {
    unsigned long long hash;
    char *val;
};

struct Pair {
    Any *car;
    Any *cdr;
};
#define CAR(X) (X)->pair->car
#define CDR(X) (X)->pair->cdr

struct Number {
    int top;
    int bot;
};

struct Callable {
    Any *env;
    Any *params;
    Any *body;
};

struct Any {
    Type type;
    union {
        Pair *pair;
        Symbol *sym;
        Number *num;
        Builtin builtin;
        Callable *macro;
        Callable *lambda;
    };
};

Any *read_any(FILE*);
Any *eval_any(Any*, Any*, Any*);
Any *print_any(FILE *, Any*);

void *gcalloc(size_t);
Any *make_clone(Any*);
Any *make_eof();
Any *make_nil();
Any *make_pair(Any*, Any*);
unsigned long long make_hash(char*);
Any *make_symbol(char*);
Any *make_number(int, int);
Any *make_builtin(Builtin);
Any *make_macro(Any*, Any*, Any*);
Any *make_lambda(Any*, Any*, Any*);
Any *make_bool(int);

int mark(void*);
void mark_any(Any*);
void sweep();

Any *set(Any*, Any*);
void append(Any*, Any*);
int list_len(Any*);

#include "builtin.h"

#endif


