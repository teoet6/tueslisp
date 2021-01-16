#include <string.h>
#include <stdio.h>
#include "tueslisp.h"

// Leave a value of zero to turn off gc
#define MAX_EVALS_BEFORE_GC 10000

static Any *eval_args(Any *stack, Any *env, Any *args) {
    stack = make_pair(make_nil(), stack);
    Any *ret = make_nil();
    Any *cur = ret;
    append(ret, CAR(stack));
    while (args->type) {
        Any *cur_eval = eval_any(stack, env, CAR(args));
        set(cur, make_pair(cur_eval, make_nil()));
        cur = CDR(cur);
        args = CDR(args);
    }
    return ret;
}

static Any *make_env_from_lambda(Any *stack, Any *arg_env, Any *fun, Any *args) {
    if (list_len(args) == -1) {
        eprintf("Error: ");
        print_any(stderr, args);
        eprintf(" is not a proper list.\n");
        return NULL;
    }
    Any *evaled_args = eval_args(stack, arg_env, args);
    Any *lambda_env = make_clone(fun->lambda->env);
    Any *params = fun->lambda->params;
    while (params->type == PAIR) {
        if (!evaled_args->type) {
            eprintf("Error: not enough arguments given.\n");
            return NULL;
        }
        append(make_pair(CAR(params), CAR(evaled_args)), lambda_env);
        params = CDR(params);
        evaled_args = CDR(evaled_args);
    }
    if (!params->type) {
        if (evaled_args->type) {
            eprintf("Error: too much arguments given.\n");
            print_any(stderr, fun);
            return NULL;
        }
        return lambda_env;
    }
    Any *varargs = make_nil();
    Any *cur = varargs;
    append(make_pair(params, varargs), lambda_env);
    while (evaled_args->type) {
        set(cur, make_pair(CAR(evaled_args), make_nil()));
        cur = CDR(cur);
        evaled_args = CDR(evaled_args);
    }
    set(cur, make_nil());
    return lambda_env;
}

static Any *make_body_from_macro(Any *stack, Any *arg_env, Any *fun, Any *args) {
    Any *macro_env = make_clone(fun->lambda->env);
    Any *params = fun->macro->params;
    if (list_len(args) == -1) {
        eprintf("Error: ");
        eprintf(" is not a proper list.\n");
        return NULL;
    }
    while (params->type == PAIR) {
        if (!args->type) {
            eprintf("Error: not enough arguments given.\n");
            return NULL;
        }
        Any *cur = CAR(args);
        RETNULL(cur);
        append(make_pair(CAR(params), cur), macro_env);
        params = CDR(params);
        args = CDR(args);
    }
    if (!params->type) {
        if (args->type) {
            eprintf("Error: too much arguments given.\n");
            return NULL;
        }
        return eval_any(stack, macro_env, fun->macro->body);
    }
    Any *varargs = make_nil();
    Any *cur = varargs;
    append(make_pair(params, varargs), macro_env);
    while (args->type) {
        set(cur, make_pair(CAR(args), make_nil()));
        cur = CDR(cur);
        args = CDR(args);
    }
    set(cur, make_nil());
    return eval_any(stack, macro_env, fun->macro->body);
}

// The macros that implement builtins expect these arguments to be exactly
// `stack`, `env` and `body`.
int evals = 0;
Any *eval_any(Any *stack, Any *env, Any *body) {
 tail_rec:
    stack = make_pair(make_nil(), stack);
    append(env, CAR(stack));
    append(body, CAR(stack));
    if (MAX_EVALS_BEFORE_GC && ++evals >= MAX_EVALS_BEFORE_GC) {
        mark_any(stack);
        sweep();
        evals = 0;
    }
    if (body->type == SYMBOL) {
        Any *cur = env;
        while (cur->type && CAR(CAR(cur))->sym->hash != body->sym->hash) {
            cur = CDR(cur);
        }
        if (!cur->type) {
            eprintf("Error: symbol '");
            print_any(stderr, body);
            eprintf("' not bound\n");
            return NULL;
        }
        return CDR(CAR(cur));
    }
    if (body->type == PAIR) {
        Any *fun = eval_any(stack, env, CAR(body));
        body = CDR(body);
        RETNULL(fun);
        append(fun, CAR(stack));
        if (fun->type == BUILTIN) {
#define CASE(IN, EX) case IN: IMPLEMENT_ ## IN; break;
            switch(fun->builtin) {
                FOR_EVERY_BUILTIN(CASE);
            }
#undef CASE
        }
        if (fun->type == LAMBDA) {
            env = make_env_from_lambda(stack, env, fun, body);
            RETNULL(env);
            body = fun->lambda->body;
            stack = CDR(stack);
            goto tail_rec;
        }
        if (fun->type == MACRO) {
            body = make_body_from_macro(stack, env, fun, body);
            RETNULL(body);
            stack = CDR(stack);
            goto tail_rec;
        }
    }
    return body;
}
