#ifndef __TUESLISP_H
#define __TUESLISP_H

#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__);

#define RETNULL(X) if(X == NULL) {                              \
        eprintf("%s %d: retnulled.\n", __FILE__, __LINE__);    \
        return NULL;                                            \
    }                                                           \

#define ERROR(...) { eprintf(__VA_ARGS__); return NULL; }

/* The end of file type should never be external. */
/* It is for internal use only. */
typedef enum Type {
    END_OF_FILE = EOF,
    NIL = 0,
    SYMBOL,
    PAIR,
    NUMBER,
    BUILTIN_MACRO,
    BUILTIN_FUNCTION,
    MACRO,
    LAMBDA,
} Type;

extern unsigned long long unquote_hash;
extern unsigned long long quasiquote_hash;

typedef enum Builtin_Macro {
    BUILTIN_EVAL,
    BUILTIN_IF,
    BUILTIN_PROGN,
} Builtin_Macro;

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
    long long top;
    long long bot;
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
        Builtin_Macro builtin_macro;
        Any *((*builtin_function)(Any*, Any*, Any*));
        Callable *macro;
        Callable *lambda;
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
Any *make_number(long long, long long);
Any *make_builtin_macro(Builtin_Macro);
Any *make_builtin_function(Any *(*)(Any *, Any*, Any*));
Any *make_macro(Any*, Any*, Any*);
Any *make_lambda(Any*, Any*, Any*);
Any *make_bool(int);

int mark(void*);
void mark_any(Any*);
void sweep();

long long gcd(long long, long long);
Any *set(Any*, Any*);
void append(Any*, Any*);
int list_len(Any*);

#endif
