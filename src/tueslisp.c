#include "tueslisp.h"
#include <stdio.h>

unsigned long long unquote_hash;
unsigned long long quasiquote_hash;

Any *global_env;

Any *set(Any *dest, Any *src) {
    *dest = *src;
    return dest;
}

void append(Any *item, Any *list) {
    Any *old_list = make_clone(list);
    set(list, make_pair(item, old_list));
}

int list_len(Any *list) {
    int len = 0;
    while (list->type == PAIR) {
        len++;
        list = CDR(list);
    }
    if (list->type) return -1;
    return len;
}

#define APPEND(IN, EX) append(make_pair(make_symbol(EX), make_builtin(IN)), env);
void append_builtins(Any *env) {
    FOR_EVERY_BUILTIN(APPEND);
}
#undef APPEND

void eval_file(Any* stack, Any *env, FILE *fp) {
    while (fp) {
        Any *eval_or_eof = read_any(fp);
        if (eval_or_eof->type == EOF) break;
        stderr, eval_any(stack, env, eval_or_eof);
    }
}

int main(int argc, char *argv[]) {
    int opened_file = 0;
    global_env = make_nil();
    append_builtins(global_env);
    unquote_hash = make_hash("unquote");
    quasiquote_hash = make_hash("quasiquote");

    {
        Any *command_line_args = make_nil();
        Any *cur = command_line_args;
        for (int i = 1; i < argc; i++) {
            set(cur, make_pair(make_symbol(argv[i]), make_nil()));
            cur = CDR(cur);
        }
        append(make_pair(make_symbol("command-line-args"), command_line_args), global_env);
    }

    {
        FILE* pl = fopen("preload.tl", "r");
        eval_file(make_nil(), global_env, pl);
        if (pl) fclose(pl);
    }

    if (argc > 1) {
        FILE *file_arg = fopen(argv[1], "r");
        eval_file(make_nil(), global_env, file_arg);
        if (file_arg) fclose(file_arg);
        opened_file = 1;
    }
    if (opened_file) return 0;

    setbuf(stdout, NULL);
    printf("tueslisp е програма на Теодор Тотев. Направена е като проект\n");
    printf("по програмиране за края на първия срок на девети б клас випуск 2024 в\n");
    printf("Технологично училище Електронни системи към Технически Уневерситет - София.\n");
    printf("  _______                        ____      \n");
    printf(" / ____  | _________            / ___\\__  \n");
    printf("|  ____| |/ ____  | | _____    / / ____ \\ \n");
    printf("|_____/| |____  | | |/ ____|   \\ \\/  \\ \\ \\\n");
    printf("       |_______/|_  |____  |   /\\ \\__/ / /\n");
    printf("                  |_______/    \\ \\__/ /_/ \n");
    printf("                                \\____/    \n");
/*
    printf("  _______                        ____      \n");
    printf(" / ____  | _________            / ___\__  \n");
    printf("|  ____| |/ ____  | | _____    / / ____ \ \n");
    printf("|_____/| |____  | | |/ ____|   \ \/  \ \ \\n");
    printf("       |_______/|_  |____  |   /\ \__/ / /\n");
    printf("                  |_______/    \ \__/ /_/ \n");
    printf("                                \____/    \n");
*/
    while (1) {
        printf("TUES> ");
        Any *eval_or_eof = read_any(stdin);
        if (eval_or_eof->type == EOF) break;
        print_any(stdout, eval_any(make_nil(), global_env, eval_or_eof));
        printf("\n");
    }
}

