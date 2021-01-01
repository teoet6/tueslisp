#ifndef __BUILTIN_H
#define __BUILTIN_H

#include "string.h"
#include "tueslisp.h"

Any *do_quasiquoting(Any*, Any*, Any*);

#define IMPLEMENT_BUILTIN_TYPE_P(IN, EX) {                        \
        if (list_len(body) != 1) {                                \
            eprintf("Error, `%s` expects 1 argument.\n", EX);        \
            return NULL;                                        \
        }                                                        \
        Any *evaled = eval_any(stack, env, CAR(body));                \
        RETNULL(evaled);                                        \
        if (evaled->type == IN) return make_symbol("t");        \
        else return make_nil();                                        \
    }

#define IMPLEMENT_BUILTIN_NIL_P     IMPLEMENT_BUILTIN_TYPE_P(NIL,     "nil?")
#define IMPLEMENT_BUILTIN_SYMBOL_P  IMPLEMENT_BUILTIN_TYPE_P(SYMBOL,  "symbol?")
#define IMPLEMENT_BUILTIN_PAIR_P    IMPLEMENT_BUILTIN_TYPE_P(PAIR,    "pair?")
#define IMPLEMENT_BUILTIN_NUMBER_P  IMPLEMENT_BUILTIN_TYPE_P(NUMBER,  "number?")
#define IMPLEMENT_BUILTIN_BUILTIN_P IMPLEMENT_BUILTIN_TYPE_P(BUILTIN, "builtin?")
#define IMPLEMENT_BUILTIN_MACRO_P   IMPLEMENT_BUILTIN_TYPE_P(MACRO,   "macro?")
#define IMPLEMENT_BUILTIN_LAMBDA_P  IMPLEMENT_BUILTIN_TYPE_P(LAMBDA,  "lambda?")

#define IMPLEMENT_BUILTIN_QUOTE {                                \
        if (list_len(body) != 1) {                                \
            eprintf("Error, `quote` expects 1 argument.\n");        \
            return NULL;                                        \
        }                                                        \
        return CAR(body);                                        \
    }

#define IMPLEMENT_BUILTIN_CONS {                                \
        if (list_len(body) != 2) {                                \
            eprintf("Error: `cons` expects 2 arguments.\n");        \
            return NULL;                                        \
        }                                                        \
        Any *car_val = eval_any(stack, env, CAR(body));                \
        RETNULL(car_val);                                        \
        append(car_val, CAR(stack));                                \
        Any *cdr_val = eval_any(stack, env, CAR(CDR(body)));        \
        RETNULL(cdr_val);                                        \
        return make_pair(car_val, cdr_val);                        \
    }

#define IMPLEMENT_BUILTIN_CAR_OR_CDR(WHICH) {                                \
        if (list_len(body) != 1) {                                        \
            eprintf("Error: `%s` expects 1 argument.\n", #WHICH);        \
            return NULL;                                                \
        }                                                                \
        Any *to_car_or_cdr = eval_any(stack, env, CAR(body));                \
        if (to_car_or_cdr->type != PAIR) {                                \
            eprintf("Error: ");                                                \
            print_any(stderr, CAR(body));                                \
            eprintf(" does not evaluate to a pair.\n");                        \
            return NULL;                                                \
        }                                                                \
        return to_car_or_cdr->pair->WHICH;                                \
    }

#define IMPLEMENT_BUILTIN_CAR IMPLEMENT_BUILTIN_CAR_OR_CDR(car)
#define IMPLEMENT_BUILTIN_CDR IMPLEMENT_BUILTIN_CAR_OR_CDR(cdr)

#define IMPLEMENT_BUILTIN_QUASIQUOTE {                                        \
        if (list_len(body) != 1) {                                        \
            eprintf("Error: `quasiquote` expects 1 argument.\n");        \
            return NULL;                                                \
        }                                                                \
        return do_quasiquoting(stack, env, CAR(body));                        \
    }

#define IMPLEMENT_BUILTIN_DEFSYM {                                        \
        if (list_len(body) != 2) {                                        \
            eprintf("Error `defsym` expects 2 arguments.\n");                \
            return NULL;                                                \
        }                                                                \
        if (CAR(body)->type != SYMBOL) {                                \
            eprintf("Error: first argument to `defsym` should be a symbol\n"); \
            return NULL;                                                \
        }                                                                \
        Any *val = eval_any(stack, env, CAR(CDR(body)));                \
        RETNULL(val);                                                        \
        append(make_pair(CAR(body), val), env);                                \
        return val;                                                        \
    }

#define IMPLEMENT_BUILTIN_EVAL {                                \
        if (list_len(body) != 1) {                                \
            eprintf("Error: `eval` expects 1 argument.\n");        \
            return NULL;                                        \
        }                                                        \
        body = eval_any(stack, env, CAR(body));                        \
        RETNULL(body);                                                \
        stack = CDR(stack);                                        \
        goto tail_rec;                                                \
    }                                                                \

#define IMPLEMENT_BUILTIN_PROGN { \
        if (list_len(body) < 1) { \
            eprintf("Error: `progn` expects at least 1 argument\n"); \
        } \
        while (CDR(body)->type) { \
            eval_any(stack, env, CAR(body)); \
            body = CDR(body); \
        } \
        body = CAR(body); \
        stack = CDR(stack); \
        goto tail_rec; \
    }

#define IMPLEMENT_BUILTIN_SET {                                        \
        if (list_len(body) != 2) {                                \
            eprintf("Error: `set!` expects 2 arguments.\n");        \
            return NULL;                                        \
        }                                                        \
        Any *dest = eval_any(stack, env, CAR(body));                \
        RETNULL(dest);                                                \
        append(dest, CAR(stack));                                \
        Any *src = eval_any(stack, env, CAR(CDR(body)));        \
        RETNULL(src);                                                \
        set(dest, src);                                                \
        return src;                                                \
    }

#define IMPLEMENT_BUILTIN_LAMBDA_OR_MACRO(WHICH) {                        \
        if (list_len(body) != 2) {                                        \
            eprintf("Error: `%s` expects 2 arguments.\n", #WHICH);        \
            return NULL;                                                \
        }                                                                \
        return make_ ## WHICH(env, CAR(body), CAR(CDR(body))); \
    }

#define IMPLEMENT_BUILTIN_LAMBDA IMPLEMENT_BUILTIN_LAMBDA_OR_MACRO(lambda)
#define IMPLEMENT_BUILTIN_MACRO IMPLEMENT_BUILTIN_LAMBDA_OR_MACRO(macro)

#define IMPLEMENT_BUILTIN_IF {                                        \
        if (list_len(body) != 3) {                                \
            eprintf("Error: `if` expects 3 arguments.\n");        \
            return NULL;                                        \
        }                                                        \
        Any *cond = eval_any(stack, env, CAR(body));                \
        RETNULL(cond);                                                \
        if (cond->type) body = CAR(CDR(body));                        \
        else body = CAR(CDR(CDR(body)));                        \
        stack = CDR(stack);                                        \
        goto tail_rec;                                                \
    }

#define IMPLEMENT_BUILTIN_PLUS {                                        \
        if (list_len(body) != 2) {                                        \
            eprintf("Error: `plus` expects 2 arguments.\n");                \
            return NULL;                                                \
        }                                                                \
        Any *val1 = eval_any(stack, env, CAR(body));                        \
        append(val1, CAR(stack));                                        \
        RETNULL(val1);                                                        \
        if (val1->type != NUMBER) {                                        \
            eprintf("Error first argument to `plus` is not a number.\n"); \
            return NULL;                                                \
        }                                                                \
        Any *val2 = eval_any(stack, env, CAR(CDR(body)));                \
        RETNULL(val2);                                                        \
        if (val2->type != NUMBER) {                                        \
            eprintf("Error second argument to `plus` is not a number.\n"); \
            return NULL;                                                \
        }                                                                \
        int top = val1->num->top * val2->num->bot + val2->num->top * val1->num->bot; \
        int bot = val1->num->bot * val2->num->bot;                        \
        return make_number(top, bot);                                        \
    }                                                                        \

#define IMPLEMENT_BUILTIN_MINUS {                                        \
        if (list_len(body) != 2) {                                        \
            eprintf("Error: `minus` expects 2 arguments.\n");                \
            return NULL;                                                \
        }                                                                \
        Any *val1 = eval_any(stack, env, CAR(body));                        \
        append(val1, CAR(stack));                                        \
        RETNULL(val1);                                                        \
        if (val1->type != NUMBER) {                                        \
            eprintf("Error first argument to `minus` is not a number.\n"); \
            return NULL;                                                \
        }                                                                \
        Any *val2 = eval_any(stack, env, CAR(CDR(body)));                \
        RETNULL(val2);                                                        \
        if (val2->type != NUMBER) {                                        \
            eprintf("Error second argument to `minus` is not a number.\n"); \
            return NULL;                                                \
        }                                                                \
        int top = val1->num->top * val2->num->bot - val2->num->top * val1->num->bot; \
        int bot = val1->num->bot * val2->num->bot;                        \
        return make_number(top, bot);                                        \
    }                                                                        \

#define IMPLEMENT_BUILTIN_EQUAL {                                        \
        if (list_len(body) != 2) {                                        \
            eprintf("Error: `plus` expects 2 arguments.\n");                \
            return NULL;                                                \
        }                                                                \
        Any *val1 = eval_any(stack, env, CAR(body));                        \
        RETNULL(val1);                                                        \
        append(val1, CAR(stack));                                        \
        Any *val2 = eval_any(stack, env, CAR(CDR(body)));                \
        RETNULL(val2);                                                        \
        if (val1->type != val2->type)                                        \
            return make_nil();                                                \
        switch(val1->type) {                                                \
        case EOF: return make_bool(1);                                        \
        case NIL: return make_bool(1);                                        \
        case SYMBOL: return make_bool(val1->sym->hash == val2->sym->hash); \
        case PAIR: return make_bool(val1->pair == val2->pair);                \
        case NUMBER: return make_bool(val1->num->top == val2->num->top && \
                                      val1->num->bot == val2->num->bot); \
        case BUILTIN: return make_bool(val1->builtin == val2->builtin); \
        case LAMBDA: return make_bool(val1->lambda == val2->lambda);        \
        case MACRO: return make_bool(val1->macro == val2->macro);        \
        }                                                                \
    }                                                                        \

#define IMPLEMENT_BUILTIN_PRINT {                                \
        if (list_len(body) != 1) {                                \
            eprintf("Error: `print` expects 1 argument.\n");        \
            return NULL;                                        \
        }                                                        \
        body = eval_any(stack, env, CAR(body));                        \
        RETNULL(body);                                                \
        print_any(stdout, body);                                \
        printf("\n");                                                \
        return body;                                                \
    }                                                                \

#endif
