#ifndef __TUESLISP_H
#define __TUESLISP_H

#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__);

#define RETNULL(X) if(X == NULL) {                              \
        eprintf("%s %d: retnulled.\n", __FILE__, __LINE__);    \
        return NULL;                                            \
    }                                                           \

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
    FILE_POINTER,
} Type;

/* This approach was made to solve the following problem: */
/* Tail recursion cannot be implemented cross-function in C. */
/* However, this way of solving the problem is not scalable, */
/* and quite frankly complicates the code drastically. */
/* You want to be able to call dynamic libraries from your code (like gtk or sdl) */
/* It might be needed to fix this approach in the near future to do the graphics calculator. */
#define FOR_EVERY_BUILTIN(DO)                   \
    DO(BUILTIN_NIL_P,      "nil?")              \
        DO(BUILTIN_SYMBOL_P,   "symbol?")       \
        DO(BUILTIN_PAIR_P,     "pair?")         \
        DO(BUILTIN_NUMBER_P,   "number?")       \
        DO(BUILTIN_BUILTIN_P,  "builtin?")      \
        DO(BUILTIN_MACRO_P,    "macro?")        \
        DO(BUILTIN_LAMBDA_P,   "lambda?")       \
        DO(BUILTIN_QUOTE,      "quote")         \
        DO(BUILTIN_QUASIQUOTE, "quasiquote")    \
        DO(BUILTIN_CONS,       "cons")          \
    DO(BUILTIN_CAR,        "car")               \
    DO(BUILTIN_CDR,        "cdr")               \
    DO(BUILTIN_CAT,        "cat")               \
    DO(BUILTIN_DEFSYM,     "defsym")            \
    DO(BUILTIN_EVAL,       "eval")              \
    DO(BUILTIN_PROGN,      "progn")             \
    DO(BUILTIN_SET,        "set!")              \
    DO(BUILTIN_LAMBDA,     "lambda")            \
    DO(BUILTIN_MACRO,      "macro")             \
    DO(BUILTIN_IF,         "if")                \
    DO(BUILTIN_PLUS,       "+")                 \
    DO(BUILTIN_MINUS,      "minus")             \
    DO(BUILTIN_MULTIPLY,   "multiply")          \
    DO(BUILTIN_DIVIDE,     "divide")            \
    DO(BUILTIN_EQUAL,      "equal")             \
    DO(BUILTIN_WHOLE_PART, "whole-part")        \
    DO(BUILTIN_PRINT,      "print")             \
    DO(BUILTIN_OPEN_FILE,  "open-file")         \
    DO(BUILTIN_EVAL_FILE,  "eval-file")

extern unsigned long long unquote_hash;
extern unsigned long long quasiquote_hash;

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
        FILE *fp;
    };
};

extern Any *global_env;
Any *read_any(FILE*);
Any *eval_any(Any*, Any*, Any*);
void eval_file(Any*, Any*, FILE*);
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
Any *make_file_pointer(char*);
Any *make_bool(int);

int mark(void*);
void mark_any(Any*);
void sweep();

Any *set(Any*, Any*);
void append(Any*, Any*);
int list_len(Any*);

#include "builtin.h"

#endif
