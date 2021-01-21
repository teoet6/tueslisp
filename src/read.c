#include <string.h>
#include "tueslisp.h"

#define MAX_TKN_LEN 1024

static int line = 1;

static char get_ch(FILE *f) {
    char c = getc(f);
    if (c == '\n') line++;
    return c;
}

static void unget_ch(FILE *f, char c) {
    ungetc(c, f);
    if (c == '\n') line--;
}

static char peek_ch(FILE *f) {
    char c = get_ch(f);
    unget_ch(f, c);
    return c;
}

static void next_ch(FILE *f) {
    get_ch(f);
}

static int is_ws(char c) {
    return
        c == ' ' ||
        c == '\n' ||
        c == '\t';
}

static int is_special(char c) {
    return
        c == '(' ||
        c == ')' ||
        c == '\'' ||
        c == '`' ||
        c == ',' ||
        c == '#' ||
        c == EOF;
}

static void skip_ws(FILE *f) {
    while (1) {
        while (is_ws(peek_ch(f))) next_ch(f);
        if (peek_ch(f) == '#')
            while (peek_ch(f) != '\n' && peek_ch(f) != EOF) next_ch(f);
        else return;
    }
}

/* Right now i dont really use buf_s */
static char *get_tkn(FILE *f, char *buf, size_t buf_s) {
    char *cur = buf;
    skip_ws(f);

    if (is_special(peek_ch(f))) {
        *cur++ = get_ch(f);
        *cur++ = 0;
        return buf;
    }
    while (!is_special(peek_ch(f)) && !is_ws(peek_ch(f))) {
        *cur++ = peek_ch(f);
        if (peek_ch(f) == '\\') {
            next_ch(f);
            *cur++ = peek_ch(f);
        }
        next_ch(f);
    }
    *cur++ = 0;

    return buf;
}

static void unget_tkn(FILE *f, char *tkn) {
    for (char *c = tkn + strlen(tkn) - 1; c >= tkn; c--)
        unget_ch(f, *c);
}

static char *peek_tkn(FILE *f, char *tkn, size_t buf_s) {
    get_tkn(f, tkn, buf_s);
    unget_tkn(f, tkn);
    return tkn;
}

static void next_tkn(FILE *f) {
    skip_ws(f);
    if (is_special(peek_ch(f))) {
        next_ch(f);
        return;
    }
    while (!is_special(peek_ch(f)) && !is_ws(peek_ch(f))) {
        if (peek_ch(f) == '\\')
            next_ch(f);
        next_ch(f);
    }
    return;
}

static int unexpect(FILE* f, char *unexp) {
    char got[MAX_TKN_LEN];
    peek_tkn(f, got, MAX_TKN_LEN);
    int ret = strcmp(unexp, got);
    if (!ret) {
        eprintf("Unexpected '%s'\n", unexp);
    }
    return ret;
}

static int expect(FILE *f, char *exp) {
    char got[MAX_TKN_LEN];
    get_tkn(f, got, MAX_TKN_LEN);
    int ret = strcmp(exp, got);
    if (ret) {
        eprintf("Expected '%s', got '%s'\n", exp, got);
    }
    return ret;
}

static int is_number(char *num) {
    for (char *c = num; *c; c++)
        if (*c != '.' && (*c < '0' || *c > '9')) return 0;
    int dots = 0;
    for (char *c = num; *c; c++)
        if (*c == '.') dots++;
    if (dots > 1) return 0;
    return 1;
}

static Any* str_to_number(char *num) {
    long long top = 0;
    long long bot = 1;
    long long bot_mul = 1;
    for (char *c = num; *c; c++) {
        if (*c == '.') {
            bot_mul = 10;
            continue;
        }
        top *= 10;
        top += *c - '0';
        bot *= bot_mul;
    }
    return make_number(top, bot);
}

static char* escape(char *str) {
    char *from = str, *to = str;
    while (*from) {
        if (*from == '\\') {
            from++;
            if (*from == 'n') *from = '\n';
            if (*from == 't') *from = '\t';
        }
        *to++ = *from++;
    }
    *to = 0;
    return str;
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
        Any *ret = make_nil();
        Any *cur = ret;
        peek_tkn(f, tkn, MAX_TKN_LEN);
        while (strcmp(tkn, ")")) {
            Any *cur_read = read_any(f);
            set(cur, make_pair(cur_read, make_nil()));
            Any *prev = cur;
            cur = CDR(cur);
            peek_tkn(f, tkn, MAX_TKN_LEN);
            if (!strcmp(tkn, ".")) {
                next_tkn(f);
                CDR(prev) = read_any(f);
                if (expect(f, ")")) return NULL;
                return ret;
            }
        }
        next_tkn(f);
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
    if (is_number(tkn))
        return str_to_number(tkn);
    return make_symbol(escape(tkn));
}
