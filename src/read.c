#include <string.h>
#include "tueslisp.h"

#define MAX_TKN_LEN 128

int line = 1;

char get_ch(FILE *f) {
    char c = getc(f);
    if (c == '\n') line++;
    return c;
}

void unget_ch(FILE *f, char c) {
    ungetc(c, f);
    if (c == '\n') line--;
}

char peek_ch(FILE *f) {
    char c = get_ch(f);
    unget_ch(f, c);
    return c;
}

void next_ch(FILE *f) {
    get_ch(f);
}

int is_ws(char c) {
    return
	c == ' ' ||
	c == '\n' ||
	c == '\t';
}

int is_special(char c) {
    return
	c == '(' ||
	c == ')' ||
	c == '\'' ||
	c == '`' ||
	c == ',' ||
	c == EOF;
}

void skip_ws(FILE *f) {
    while (is_ws(peek_ch(f))) next_ch(f);
}

/* Right now i dont really use buf_s */
char *get_tkn(FILE *f, char *buf, size_t buf_s) {
    char *cur = buf;
    skip_ws(f);

    if (is_special(peek_ch(f))) {
	*cur++ = get_ch(f);
	*cur++ = 0;
	return buf;
    }
    while (!is_special(peek_ch(f)) && !is_ws(peek_ch(f))) {
	*cur++ = get_ch(f);
    }
    *cur++ = 0;

    return buf;
}

void unget_tkn(FILE *f, char *tkn) {
    for (char *c = tkn + strlen(tkn) - 1; c >= tkn; c--)
        unget_ch(f, *c);
}

char *peek_tkn(FILE *f, char *tkn, size_t buf_s) {
    get_tkn(f, tkn, buf_s);
    unget_tkn(f, tkn);
    return tkn;
}

void next_tkn(FILE *f) {
    skip_ws(f);
    if (is_special(peek_ch(f))) {
        next_ch(f);
	return;
    }
    while (!is_special(peek_ch(f)) && !is_ws(peek_ch(f)))
        next_ch(f);
    return;
}

int unexpect (FILE* f, char *unexp) {
    char got[MAX_TKN_LEN];
    peek_tkn(f, got, MAX_TKN_LEN);
    int ret = strcmp(unexp, got);
    if (!ret) {
	fprintf(stderr, "Unexpected '%s'\n", unexp);
    }
    return ret;
}

int expect (FILE *f, char *exp) {
    char got[MAX_TKN_LEN];
    get_tkn(f, got, MAX_TKN_LEN);
    int ret = strcmp(exp, got);
    if (ret) {
	fprintf(stderr, "Expected '%s', got '%s'\n", exp, got);
    }
    return ret;
}

Any *read_any(FILE *f) {
    if (!unexpect(f, ")")) return NULL;
    if (!unexpect(f, ".")) return NULL;
    char tkn[MAX_TKN_LEN];
    get_tkn(f, tkn, MAX_TKN_LEN);
    if (tkn[0] == EOF) {
	return make_eof();
    }
    if (!strcmp(tkn, "(")) {
	Any *ret = make_any();
	Any *cur = ret;
	peek_tkn(f, tkn, MAX_TKN_LEN);
	while (strcmp(tkn, ")")) {
	    cur->type = PAIR;
	    cur->pair = gcalloc(sizeof(Pair));
	    cur->pair->car = read_any(f);
	    cur->pair->cdr = make_any();
	    Any *prev = cur;
	    cur = cur->pair->cdr;
	    peek_tkn(f, tkn, MAX_TKN_LEN);
	    if (!strcmp(tkn, ".")) {
		next_tkn(f);
		prev->pair->cdr = read_any(f);
		if (expect(f, ")")) return NULL;
		return ret;
	    }
	}
	next_tkn(f);
	cur->type = NIL;
	return ret;
    }
    if (!strcmp(tkn, "'")) {
	return make_pair(make_symbol("quote"),
			 make_pair(read_any(f),
				   make_nil()));
    }
    if (!strcmp(tkn, "`")) {
	return make_pair(make_symbol("quasiquote"),
			 make_pair(read_any(f),
				   make_nil()));
    }
    if (!strcmp(tkn, ",")) {
	return make_pair(make_symbol("unquote"),
			 make_pair(read_any(f),
				   make_nil()));
    }
    /* if (is_number(tkn)) { */
    /* 	return str_to_number(tkn); */
    /* } */
    return make_symbol(tkn);
}
