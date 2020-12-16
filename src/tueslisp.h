#ifndef __TUESLISP_H
#define __TUESLISP_H

#include <stdio.h>

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

#define FOR_EVERY_BUILTIN(DO)			\
    DO(BUILTIN_QUOTE)				\
	DO(BUILTIN_CONS)			\
	DO(BUITLIN_CAR)				\
	DO(BUILTIN_CDR)				\
	DO(BUITLIN_QUASIQUOTE)			\
	DO(BUILTIN_LAMBDA)			\
	

#define ENUM_BUILTIN(ID) ID,
typedef enum Builtin {
    FOR_EVERY_BUILTIN(ENUM_BUILTIN)
} Builtin;
#undef ENUM_BUILTIN

typedef struct Any Any;
typedef struct Pair Pair;
typedef struct Number Number;
typedef struct Callable Callable;

struct Pair {
    Any *car;
    Any *cdr;
};

struct Number {
    int top;
    int bottom;
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
	char *sym;
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
Any *make_any();
Any *make_eof();
Any *make_nil();
Any *make_pair(Any*, Any*);
Any *make_symbol(char*);
Any *make_number(int, int);
Any *make_macro(Any*, Any*, Any*);
Any *make_lambda(Any*, Any*, Any*);

void mark(void*);
void mark_any(Any*);
void sweep();

#endif
