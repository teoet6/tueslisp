#include <string.h>
#include <stdio.h>
#include "tueslisp.h"
#include "builtin.h"

Any *do_quasiquoting(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if(body->type == PAIR) {
	if (CAR(body)->type == SYMBOL && CAR(body)->sym->hash == make_hash("unquote")) {
	    if (list_len(CDR(body)) != 1) {
		eprintf("Error: unquote expects 1 argument.\n");
		return NULL;
	    }
	    return eval_any(stack, env, CAR(CDR(body)));
	}
	if (CAR(body)->type == SYMBOL && CAR(body)->sym->hash == make_hash("quasiquote")) {
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
