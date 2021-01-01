#include "tueslisp.h"
#include <stdio.h>

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

int main() {
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
    global_env = make_nil();
    append_builtins(global_env);
    FILE* pl = fopen("preload.tl", "r");
    while (pl) {
        Any *eval_or_eof = read_any(pl);
        if (eval_or_eof->type == EOF) break;
        eval_any(make_nil(), global_env, eval_or_eof);
    }
    while (1) {
        printf("TUES> ");
        Any *eval_or_eof = read_any(stdin);
        if (eval_or_eof->type == EOF) break;
        print_any(stdout, eval_any(make_nil(), global_env, eval_or_eof));
        printf("\n");
    }
}

