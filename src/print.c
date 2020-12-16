#include <stdio.h>
#include "tueslisp.h"

Any *print_any(FILE *f, Any *a) {
    if (!a) return NULL;
    switch (a->type) {
    case NIL:
	fprintf(f, "()");
	break;
    case PAIR:
	fprintf(f, "(");
	while (a->type == PAIR) {
	    print_any(f, a->pair->car);
	    fprintf(f, " ");
	    a = a->pair->cdr;
	}
	if (a->type) {
	    fprintf(f, ". ");
	    print_any(f, a);
	}
	fprintf(f, ")");
	break;
    case SYMBOL:
	fprintf(f, "%s", a->sym);
	break;
    case NUMBER:
	fprintf(f, "%d/%d", a->num->top, a->num->bottom);
	break;
    case BUILTIN:
	fprintf(f, "<Builtin function: %d>", a->builtin);
	break;
    case MACRO:
	fprintf(f, "<Macro>");
    case LAMBDA:
	fprintf(f, "<Lamda>");
    }
    return a;
}
