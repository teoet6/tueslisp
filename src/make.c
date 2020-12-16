#include "tueslisp.h"
#include <string.h>

Any *make_any() {
    Any *a = gcalloc(sizeof(Any));
    return a;
}

Any *make_eof() {
    Any *n = make_any();
    n->type = EOF;
    return n;
}

Any *make_nil() {
    Any *n = make_any();
    n->type = NIL;
    return n;
}

Any *make_pair(Any *car_val, Any *cdr_val) {
    Any *p = make_any();
    p->type = PAIR;
    p->pair = gcalloc(sizeof(Pair));
    p->pair->car = car_val;
    p->pair->cdr = cdr_val;
    return p;
}

Any *make_symbol(char *symbol_val) {
    Any *s = make_any();
    s->type = SYMBOL;
    s->sym = gcalloc(strlen(symbol_val));
    strcpy(s->sym, symbol_val);
    return s;
}

Any *make_number(int top_val, int bottom_val) {
    Any *n = make_any();
    n->type = NUMBER;
    n->num = gcalloc(sizeof(Number));
    n->num->top = top_val;
    n->num->bottom = bottom_val;
    return n;
}

Any *make_macro(Any *env, Any *params, Any *body) {
    Any *m = make_any();
    m->type = MACRO;
    m->macro->env = env;
    m->macro->params = params;
    m->macro->body = body;
    return m;
}

Any *make_lambda(Any *env, Any *params, Any *body) {
    Any *l = make_any();
    l->type = LAMBDA;
    l->lambda->env = env;
    l->lambda->params = params;
    l->lambda->body = body;
    return l;
}
