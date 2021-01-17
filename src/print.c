#include <stdio.h>
#include "tueslisp.h"

Any *print_any(FILE *f, Any *a) {
    RETNULL(a);
    switch (a->type) {
    case EOF:
	fprintf(f, "<End of file>");
	break;
    case NIL:
	fprintf(f, "()");
	break;
    case PAIR:
	fprintf(f, "(");
	while (a->type == PAIR) {
	    print_any(f, CAR(a));
	    fprintf(f, " ");
	    a = CDR(a);
	}
	if (a->type) {
	    fprintf(f, ". ");
	    print_any(f, a);
	}
	fprintf(f, ")");
	break;
    case SYMBOL:
	fprintf(f, "%s", a->sym->val);
	break;
    case NUMBER:
	if (a->num->bot == 1)
	    fprintf(f, "%d", a->num->top);
	else 
	    fprintf(f, "%d/%d", a->num->top, a->num->bot);
	break;
    case BUILTIN_FUNCTION:
	fprintf(f, "<Builtin function: %p>", a->builtin_function);
	break;
    case BUILTIN_MACRO:
	fprintf(f, "<Builtin macro: %d>", a->builtin_macro);
	break;
    case MACRO:
	fprintf(f, "<Macro ");
	print_any(f, a->macro->params);
	fprintf(f, " ");
	print_any(f, a->macro->body);
	fprintf(f, ">");
	break;
    case LAMBDA:
	fprintf(f, "<Lamda ");
	print_any(f, a->lambda->params);
	fprintf(f, " ");
	print_any(f, a->lambda->body);
	fprintf(f, ">");
	break;
    case FILE_POINTER:
        fprintf(f, "<File %p>", a->fp);
    }
    return a;
}
