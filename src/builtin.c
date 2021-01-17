#include <string.h>
#include <stdio.h>
#include "tueslisp.h"

extern unsigned long long unquote_hash;
extern unsigned long long quasiquote_hash;

#define BUILTIN_TYPE_P(NAME, TYPE)                                      \
    Any *builtin_ ## NAME ## _p(Any *stack, Any *env, Any *body) {       \
        if (list_len(body) != 1) {                                      \
            eprintf("Error, `%s` expects 1 argument.\n", #NAME "?");    \
            return NULL;                                                \
        }                                                               \
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
    if (list_len(body) != 1) {
        eprintf("Error, `quote` expects 1 argument.\n");
        return NULL;
    }
    return CAR(body);
}

Any *builtin_cons(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `cons` expects 2 arguments.\n");
        return NULL;
    }
    Any *car_val = eval_any(stack, env, CAR(body));
    RETNULL(car_val);
    append(car_val, CAR(stack));
    Any *cdr_val = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(cdr_val);
    return make_pair(car_val, cdr_val);
}

#define BUILTIN_CAR_OR_CDR(WHICH)                                       \
    Any *builtin_ ## WHICH(Any *stack, Any *env, Any *body) {           \
        if (list_len(body) != 1) {                                      \
            eprintf("Error: `%s` expects 1 argument.\n", #WHICH);       \
            return NULL;                                                \
        }                                                               \
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

BUILTIN_CAR_OR_CDR(cdr)
BUILTIN_CAR_OR_CDR(car)

Any *builtin_cat(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 0)
        return NULL;
    Any *ret = make_symbol("");
    while (body->type) {
        append(ret, CAR(stack));
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != SYMBOL) {
            eprintf("Error: all argument to `cat` should be symbols.\n");
            return NULL;
        }
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
	    if (list_len(CDR(body)) != 1) {
		eprintf("Error: unquote expects 1 argument.\n");
		return NULL;
	    }
	    return eval_any(stack, env, CAR(CDR(body)));
	}
	if (CAR(body)->type == SYMBOL && CAR(body)->sym->hash == quasiquote_hash) {
	    if (list_len(CDR(body)) != 1) {
		eprintf("Error: quasiquote expects 1 argument.\n");
		return NULL;
	    }
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
    if (list_len(body) != 1) {
        eprintf("Error: `quasiquote` expects 1 argument.\n");
        return NULL;
    }
    return do_quasiquoting(stack, env, CAR(body));
}

Any *builtin_defsym(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error `defsym` expects 2 arguments.\n");
        return NULL;
    }
    if (CAR(body)->type != SYMBOL) {
        eprintf("Error: first argument to `defsym` should be a symbol\n");
        return NULL;
    }
    Any *val = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val);
    append(make_pair(CAR(body), val), env);
    return val;
}

Any *builtin_set(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `set!` expects 2 arguments.\n");
        return NULL;
    }
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
        if (list_len(body) != 2) {                                      \
            eprintf("Error: `%s` expects 2 arguments.\n", #WHICH);      \
            return NULL;                                                \
        }                                                               \
        return make_ ## WHICH(env, CAR(body), CAR(CDR(body)));          \
    }

BUILTIN_LAMBDA_OR_MACRO(lambda);
BUILTIN_LAMBDA_OR_MACRO(macro);

Any *builtin_plus(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 0)
        return NULL;
    int top = 0;
    int bot = 1;
    while (body->type) {
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        if (cur->type != NUMBER) {
            eprintf("Error: all argument to `+` should be numbers.\n");
            return NULL;
        }
        top = top * cur->num->bot + cur->num->top * bot;
        bot *= cur->num->bot;
        body = CDR(body);
    }
    return make_number(top, bot);
}

Any *builtin_minus(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `minus` expects 2 arguments.\n");
        return NULL;
    }
    Any *val1 = eval_any(stack, env, CAR(body));
    append(val1, CAR(stack));
    RETNULL(val1);
    if (val1->type != NUMBER) {
        eprintf("Error first argument to `minus` is not a number.\n");
        return NULL;
    }
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val2->type != NUMBER) {
        eprintf("Error second argument to `minus` is not a number.\n");
        return NULL;
    }
    int top = val1->num->top * val2->num->bot - val2->num->top * val1->num->bot;
    int bot = val1->num->bot * val2->num->bot;
    return make_number(top, bot);
}

Any *builtin_multiply(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `multiply` expects 2 arguments.\n");
        return NULL;
    }
    Any *val1 = eval_any(stack, env, CAR(body));
    append(val1, CAR(stack));
    RETNULL(val1);
    if (val1->type != NUMBER) {
        eprintf("Error first argument to `multiply` is not a number.\n");
        return NULL;
    }
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val2->type != NUMBER) {
        eprintf("Error second argument to `multiply` is not a number.\n");
        return NULL;
    }
    int top = val1->num->top * val2->num->top;
    int bot = val1->num->bot * val2->num->bot;
    return make_number(top, bot);
}

Any *builtin_divide(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `divide` expects 2 arguments.\n");
        return NULL;
    }
    Any *val1 = eval_any(stack, env, CAR(body));
    append(val1, CAR(stack));
    RETNULL(val1);
    if (val1->type != NUMBER) {
        eprintf("Error first argument to `divide` is not a number.\n");
        return NULL;
    }
    Any *val2 = eval_any(stack, env, CAR(CDR(body)));
    RETNULL(val2);
    if (val2->type != NUMBER) {
        eprintf("Error second argument to `divide` is not a number.\n");
        return NULL;
    }
    int top = val1->num->top * val2->num->bot;
    int bot = val1->num->bot * val2->num->top;
    return make_number(top, bot);
}

Any *builtin_equal(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 2) {
        eprintf("Error: `plus` expects 2 arguments.\n");
        return NULL;
    }
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


Any *builtin_whole_part(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1) {
        eprintf("Error: `whole-part` expects 2 arguments.\n");
        return NULL;
    }
    Any *val = eval_any(stack, env, CAR(body));
    RETNULL(val);
    if (val->type != NUMBER) {
        eprintf("Error first argument to `whole-part` is not a number.\n");
        return NULL;
    }
    return make_number(val->num->top / val->num->bot, 1);
}

Any *builtin_print(Any *stack, Any *env, Any *body) {
    if (list_len(body) < 1) {
        eprintf("Error: `print` expects at least 1 argument.\n");
        return NULL;
    }
    while (body->type) {
        Any *cur = eval_any(stack, env, CAR(body));
        RETNULL(cur);
        print_any(stdout, cur);
        body = CDR(body);
    }
    return make_nil();
}

Any *builtin_open_file(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1) {
        eprintf("Error: `open-file` expects 1 argument.\n");
        return NULL;
    }
    Any *filename = eval_any(stack, env, CAR(body));
    RETNULL(filename);
    if (filename->type != SYMBOL) {
        eprintf("Error: first argument to `open-file` is not a symol.\n");
        return NULL;
    }
    return make_file_pointer(filename->sym->val);
}

Any *builtin_eval_file(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 1) {
        eprintf("Error: `eval-file` expects 1 argument.\n");
        return NULL;
    }
    Any *file = eval_any(stack, env, CAR(body));
    RETNULL(file);
    if (file->type != FILE_POINTER) {
        eprintf("Error: first argument to `open-file` is not a file pointer.\n");
        return NULL;
    }
    eval_file(stack, global_env, file->fp);
    return file;
}
