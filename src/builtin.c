#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "tueslisp.h"

extern unsigned long long unquote_hash;
extern unsigned long long quasiquote_hash;

#define BUILTIN_TYPE_P(NAME, TYPE)                                      \
    Any *builtin_ ## NAME ## _p(Any *stack, Any *env, Any *body) {       \
        if (list_len(body) != 1)                                      \
            ERROR("Error, `%s` expects 1 argument!\n", #NAME "?"); \
        Any *evaled = eval_any(stack, env, CAR(body));                  \
        RETNULL(evaled);                                                \
        if (evaled->type == TYPE) return make_symbol("t");              \
        else return make_nil();                                         \
    }

BUILTIN_TYPE_P(nil, NIL);
BUILTIN_TYPE_P(symbol, SYMBOL);
BUILTIN_TYPE_P(pair, PAIR);
BUILTIN_TYPE_P(number, NUMBER);

Any *builtin_quote(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1)
        ERROR("Error, `quote` expects 1 argument.\n");
    return CAR(body);
}

Any *builtin_cons(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `cons` expects 2 arguments.\n");
    Any *car_val = eval_any(stack, env, CAR(body));
    RETNULL(car_val);
    append(car_val, CAR(stack));
    Any *cdr_val = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(cdr_val);
    return make_pair(car_val, cdr_val);
}

#define BUILTIN_CAR_OR_CDR(WHICH)                                       \
    Any *builtin_ ## WHICH(Any *stack, Any *env, Any *body) {           \
        if (list_len(body) != 1)                                      \
            ERROR("Error: `%s` expects 1 argument.\n", #WHICH);       \
        Any *to_car_or_cdr = eval_any(stack, env, CAR(body));           \
        if (to_car_or_cdr->type == NIL) return make_nil();              \
        if (to_car_or_cdr->type == SYMBOL) {                            \
            /* The problem is that we need to do very different */      \
            /* Thing on car compared to cdr */                          \
            /* This is a very hacky solution */                         \
            int car = 0;                                                \
            int cdr = 0;                                                \
            WHICH = 1;                                                  \
            if (car) {                                                  \
                char car_sym[2];                                        \
                car_sym[0] = to_car_or_cdr->sym->val[0];                \
                car_sym[1] = 0;                                         \
                return make_symbol(car_sym);                            \
            } else {                                                    \
                if (to_car_or_cdr->sym->val[1] == 0) return make_nil(); \
                return make_symbol(to_car_or_cdr->sym->val + 1);        \
            }                                                           \
        }                                                               \
        if (to_car_or_cdr->type != PAIR) {                              \
            eprintf("Error: ");                                         \
            print_any(stderr, CAR(body));                               \
            eprintf(" does not evaluate to a pair.\n");                 \
            return NULL;                                                \
        }                                                               \
        return to_car_or_cdr->pair->WHICH;                              \
    }

BUILTIN_CAR_OR_CDR(cdr);
BUILTIN_CAR_OR_CDR(car);

Any *builtin_cat(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) < 0)
        return NULL;
    Any *ret = make_symbol("");
    while (body->type) {
        append(ret, CAR(stack));
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != SYMBOL)
            ERROR("Error: all argument to `cat` should be symbols.\n");
        char new[strlen(ret->sym->val) + strlen(cur->sym->val) + 1];
        strcpy(new, ret->sym->val);
        strcat(new, cur->sym->val);
        ret = make_symbol(new);
        body = CDR(body);
    }
    if (ret->sym->val[0] == 0) return make_nil();
    return ret;
}


Any *do_quasiquoting(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if(body->type == PAIR) {
        if (CAR(body)->type == SYMBOL && CAR(body)->sym->hash == unquote_hash) {
            if (list_len(CDR(body)) != 1)
                ERROR("Error: unquote expects 1 argument.\n");
            return eval_any(stack, env, CAR(CDR(body)));
        }
        if (CAR(body)->type == SYMBOL && CAR(body)->sym->hash == quasiquote_hash) {
            if (list_len(CDR(body)) != 1)
                ERROR("Error: quasiquote expects 1 argument.\n");
            return body;
        }
        Any *car_val = do_quasiquoting(stack, env, CAR(body));
        RETNULL(car_val);
        append(car_val, CAR(stack));
        Any *cdr_val = do_quasiquoting(stack, env, CDR(body));
        RETNULL(cdr_val);
        return make_pair(car_val, cdr_val);
    }
    return body;
}

Any *builtin_quasiquote(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1)
        ERROR("Error: `quasiquote` expects 1 argument.\n");
    return do_quasiquoting(stack, env, CAR(body));
}

Any *builtin_defsym(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 2)
        ERROR("Error `defsym` expects at least 2 arguments.\n");
    if (CAR(body)->type != SYMBOL)
        ERROR("Error: first argument to `defsym` should be a symbol\n");
    Any *curr = CDR(body);
    while (curr->pair->cdr->type) {
        eval_any(stack, env, CAR(curr));
        curr = curr->pair->cdr;
    }
    Any *val = eval_any(stack, env, CAR(curr));
    RETNULL(val);
    append(make_pair(CAR(body), val), env);
    return val;
}

Any *builtin_set(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `set!` expects 2 arguments.\n");
    Any *dest = eval_any(stack, env, CAR(body));
    RETNULL(dest);
    append(dest, CAR(stack));
    Any *src = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(src);
    set(dest, src);
    return src;
}

#define BUILTIN_LAMBDA_OR_MACRO(WHICH)                                  \
    Any *builtin_ ## WHICH (Any *stack, Any *env, Any *body) {          \
        if (list_len(body) < 2)                                       \
            ERROR("Error: `%s` expects at least 2 arguments.\n", #WHICH); \
        return make_ ## WHICH(env, \
                              CAR(body), \
                              make_pair(make_symbol("progn"), \
                                        CDR(body))); \
    }

BUILTIN_LAMBDA_OR_MACRO(lambda);
BUILTIN_LAMBDA_OR_MACRO(macro);

Any *builtin_plus(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 0)
        return NULL;
    long long top = 0, bot = 1;
    while (body->type) {
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != NUMBER)
            ERROR("Error: all arguments to `+` should be numbers.\n");
        top = top * cur->num->bot + cur->num->top * bot;
        bot = bot * cur->num->bot;

        long long simple = gcd(top, bot);
        top /= simple;
        bot /= simple;

        body = CDR(body);
    }
    return make_number(top, bot);
}

Any *builtin_minus(Any *stack, Any *env, Any *body) {
    int len = list_len(body);
    if (len < 0)
        return NULL;
    if (len == 0)
        return make_number(0, 1);
    Any *cur;
    long long first_top = 0, first_bot = 1, rest_top = 0, rest_bot = 1;
    cur = eval_any(stack, env, CAR(body));
    RETNULL(cur);
    if (cur->type != NUMBER)
        ERROR("Error: all arguments to `-` should be numbers.\n");
    first_top = cur->num->top;
    first_bot = cur->num->bot;
    body = CDR(body);
    if (len == 1)
        return make_number(-first_top, first_bot);
    while (body->type) {
        cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != NUMBER)
            ERROR("Error: all arguments to `-` should be numbers.\n");
        rest_top = rest_top * cur->num->bot + cur->num->top * rest_bot;
        rest_bot *= cur->num->bot;

        long long simple = gcd(rest_top, rest_bot);
        rest_top /= simple;
        rest_bot /= simple;

        body = CDR(body);
    }
    return make_number(first_top * rest_bot - rest_top * first_bot,
                       first_bot * rest_bot);
}

Any *builtin_multiply(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 0)
        return NULL;
    long long top = 1, bot = 1;
    while (body->type) {
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != NUMBER)
            ERROR("Error: all arguments to `*` should be numbers.\n");
        top *= cur->num->top;
        bot *= cur->num->bot;

        long long simple = gcd(top, bot);
        top /= simple;
        bot /= simple;

        body = CDR(body);
    }
    return make_number(top, bot);
}

Any *builtin_divide(Any *stack, Any *env, Any *body) {
    int len = list_len(body);
    if (len < 0)
        return NULL;
    if (len == 0)
        return make_number(0, 1);
    Any *cur;
    long long first_top = 1, first_bot = 1, rest_top = 1, rest_bot = 1;
    cur = eval_any(stack, env, CAR(body));
    RETNULL(cur);
    if (cur->type != NUMBER)
        ERROR("Error: all arguments to `/` should be numbers.\n");
    first_top = cur->num->top;
    first_bot = cur->num->bot;
    body = CDR(body);
    if (len == 1)
        return make_number(first_bot, first_top);
    while (body->type) {
        cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != NUMBER)
            ERROR("Error: all arguments to `/` should be numbers.\n");
        rest_top *= cur->num->top;
        rest_bot *= cur->num->bot;

        long long simple = gcd(rest_top, rest_bot);
        rest_top /= simple;
        rest_bot /= simple;

        body = CDR(body);
    }
    return make_number(first_top * rest_bot, first_bot * rest_top);
}

Any *builtin_equal(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `equal` expects 2 arguments.\n");
    Any *val1 = eval_any(stack, env, CAR(body));
    RETNULL(val1);
    append(val1, CAR(stack));
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val1->type != val2->type)
        return make_nil();
    switch(val1->type) {
        case EOF: return make_bool(1);
        case NIL: return make_bool(1);
        case SYMBOL: return make_bool(val1->sym->hash == val2->sym->hash);
        case PAIR: return make_bool(val1->pair == val2->pair);
        case NUMBER: return make_bool(val1->num->top == val2->num->top &&
                             val1->num->bot == val2->num->bot);
        case BUILTIN_MACRO: return make_bool(val1->builtin_macro ==
                                    val2->builtin_macro);
        case BUILTIN_FUNCTION: return make_bool(val1->builtin_function ==
                                       val2->builtin_function);
        case LAMBDA: return make_bool(val1->lambda == val2->lambda);
        case MACRO: return make_bool(val1->macro == val2->macro);
    }
}

Any *builtin_less_than(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `<` expects 2 arguments.\n");
    Any *val1 = eval_any(stack, env, CAR(body));
    RETNULL(val1);
    append(val1, CAR(stack));
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val1->type == NUMBER && val2->type == NUMBER)
        return make_bool(val1->num->top * val2->num->bot <
                         val2->num->top * val1->num->bot);
    if (val1->type == SYMBOL && val2->type == SYMBOL)
        return make_bool(strcmp(val1->sym->val, val2->sym->val) < 0);
    ERROR("Error: arguments to `<` should be either two numbers or two symbols!\n");
}

Any *builtin_greater_than(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `>` expects 2 arguments.\n");
    Any *val1 = eval_any(stack, env, CAR(body));
    RETNULL(val1);
    append(val1, CAR(stack));
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val1->type == NUMBER && val2->type == NUMBER)
        return make_bool(val1->num->top * val2->num->bot >
                         val2->num->top * val1->num->bot);
    if (val1->type == SYMBOL && val2->type == SYMBOL)
        return make_bool(strcmp(val1->sym->val, val2->sym->val) > 0);
    ERROR("Error: arguments to `>` should be either two numbers or two symbols!\n");
}

Any *builtin_whole_part(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1)
        ERROR("Error: `whole-part` expects 2 arguments.\n");
    Any *val = eval_any(stack, env, CAR(body));
    RETNULL(val);
    if (val->type != NUMBER)
        ERROR("Error first argument to `whole-part` is not a number.\n");
    return make_number(val->num->top / val->num->bot, 1);
}

Any *builtin_print(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 1)
        ERROR("Error: `print` expects at least 1 argument.\n");
    while (body->type) {
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        print_any(stdout, cur);
        body = CDR(body);
    }
    return make_nil();
}

Any *builtin_import(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1)
        ERROR("Error: `import` expects one argument.\n");
    Any *filename = eval_any(stack, env, CAR(body));
    RETNULL(filename);
    if(filename->type != SYMBOL)
        ERROR("Error: first argument to `import` is not a symbol!\n");
    FILE *fp = fopen(filename->sym->val, "r");
    if (!fp) return make_nil();
    eval_file(stack, global_env, fp);
    return filename;
}

Any *builtin_extract_function(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 2)
        ERROR("Error: `extract-function` expects 2 arguments.\n");
    Any *dl_name = eval_any(stack, env, CAR(body));
    RETNULL(dl_name);
    if (dl_name->type != SYMBOL)
        ERROR("Error: first argument to `extract-function` doesn't evaluate to a symbol!.\n");
    append(dl_name, CAR(stack));
    Any *sig = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(sig);
    if (sig->type != SYMBOL)
        ERROR("Error: first argument to `extract function` doesn't evaluate to a symbol!.\n");
    void *handle = dlopen(dl_name->sym->val, RTLD_NOW);
    RETNULL(handle);
    void *fun = dlsym(handle, sig->sym->val);
    RETNULL(fun);
    return make_builtin_function(fun);
}

Any *builtin_exit(Any *stack, Any *env, Any *body) {
    int len = list_len(body);
    if (len < 0)
        return NULL;
    if (len == 0)
        exit(0);
    if (len == 1) {
        Any *exit_code = eval_any(stack, env, CAR(body));
        RETNULL(exit_code);
        if (exit_code->type != NUMBER)
            ERROR("Error: first argument to `exit` doesn't evaluate to a number!\n");
        exit(exit_code->num->top / exit_code->num->bot);
    }
    ERROR("Error: `exit` expects one or zero arguments!\n");
}
