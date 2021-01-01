#include "tueslisp.h"
#include <string.h>

static Any *make_any() {
    Any *a = gcalloc(sizeof(Any));
    a->type = NIL;
    return a;
}

Any *make_clone(Any *src) {
    Any *c = make_any();
    *c = *src;
    return c;
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
    CAR(p) = car_val;
    CDR(p) = cdr_val;
    return p;
}

unsigned long long make_hash(char *s) {
    unsigned long long hash1 = 0, mod1 = 1000000007;
    unsigned long long hash2 = 0, mod2 = 1000000009;
    for (char *c = s; *c; c++) {
	hash1 *= 256;
	hash1 += *c;
	hash1 %= mod1;
	hash2 *= 256;
	hash2 += *c;
	hash2 %= mod2;
    }
    return hash1 | hash2 << 32;
}

Any *make_symbol(char *symbol_val) {
    Any *s = make_any();
    s->type = SYMBOL;
    s->sym = gcalloc(sizeof(Symbol));
    s->sym->val = gcalloc(strlen(symbol_val) + 1);
    strcpy(s->sym->val, symbol_val);
    s->sym->hash = make_hash(symbol_val);
    return s;
}

int gcd(int a, int b) {
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;
    if (a < b) return gcd(b, a);
    if (b == 0) return a;
    return gcd(b, a % b);
}

Any *make_number(int top_val, int bot_val) {
    int simple = gcd(top_val, bot_val);
    top_val /= simple;
    bot_val /= simple;
    Any *n = make_any();
    n->type = NUMBER;
    n->num = gcalloc(sizeof(Number));
    n->num->top = top_val;
    n->num->bot = bot_val;
    return n;
}

Any *make_builtin(Builtin builtin_val) {
    Any *b = make_any();
    b->type = BUILTIN;
    b->builtin = builtin_val;
    return b;
}

Any *make_macro(Any *env, Any *params, Any *body) {
    Any *m = make_any();
    m->macro = gcalloc(sizeof(Callable));
    m->type = MACRO;
    m->macro->env = env;
    m->macro->params = params;
    m->macro->body = body;
    return m;
}

Any *make_lambda(Any *env, Any *params, Any *body) {
    Any *l = make_any();
    l->type = LAMBDA;
    l->lambda = gcalloc(sizeof(Callable));
    l->lambda->env = env;
    l->lambda->params = params;
    l->lambda->body = body;
    return l;
}

Any *make_bool(int val) {
    if (val) return make_symbol("t");
    return make_nil();
}

void mark_any(Any *a) {
    if (mark(a)) return;
    switch(a->type) {
    case SYMBOL:
	mark(a->sym);
	mark(a->sym->val);
	return;
    case PAIR:
	mark(a->pair);
	mark_any(CAR(a));
	mark_any(CDR(a));
	return;
    case NUMBER:
	mark(a->num);
	return;
    case MACRO:
	mark(a->macro);
	mark_any(a->macro->env);
	mark_any(a->macro->params);
	mark_any(a->macro->body);
	return;
    case LAMBDA:
	mark(a->lambda);
	mark_any(a->lambda->env);
	mark_any(a->lambda->params);
	mark_any(a->lambda->body);
	return;
    case EOF: case NIL: case BUILTIN: return;
    }
}
