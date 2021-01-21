#include "tueslisp.h"
#include <dlfcn.h>
#include <stdio.h>
#include "builtin.h"

unsigned long long unquote_hash;
unsigned long long quasiquote_hash;

Any *global_env;

Any *set(Any *dest, Any *src) {
    *dest = *src;
    return dest;
}

long long gcd(long long a, long long b) {
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;
    if (a < b) return gcd(b, a);
    if (b == 0) return a;
    return gcd(b, a % b);
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

void append_builtins(Any *env) {
    append(make_pair(make_symbol("eval"),  make_builtin_macro(BUILTIN_EVAL)),  env);
    append(make_pair(make_symbol("if"),    make_builtin_macro(BUILTIN_IF)),    env);
    append(make_pair(make_symbol("progn"), make_builtin_macro(BUILTIN_PROGN)), env);
    
#define APPEND_FUNCTION(IN, EX) \
    append(make_pair(make_symbol(EX), make_builtin_function(IN)), env);
    APPEND_FUNCTION(builtin_nil_p,            "nil?");              
    APPEND_FUNCTION(builtin_symbol_p,         "symbol?");       
    APPEND_FUNCTION(builtin_pair_p,           "pair?");         
    APPEND_FUNCTION(builtin_number_p,         "number?");       
    APPEND_FUNCTION(builtin_quote,            "quote");         
    APPEND_FUNCTION(builtin_quasiquote,       "quasiquote");    
    APPEND_FUNCTION(builtin_cons,             "cons");          
    APPEND_FUNCTION(builtin_car,              "car");               
    APPEND_FUNCTION(builtin_cdr,              "cdr");               
    APPEND_FUNCTION(builtin_cat,              "cat");               
    APPEND_FUNCTION(builtin_defsym,           "defsym");            
    APPEND_FUNCTION(builtin_set,              "set!");              
    APPEND_FUNCTION(builtin_lambda,           "lambda");            
    APPEND_FUNCTION(builtin_macro,            "macro");             
    APPEND_FUNCTION(builtin_plus,             "+");                 
    APPEND_FUNCTION(builtin_minus,            "-");             
    APPEND_FUNCTION(builtin_multiply,         "*");          
    APPEND_FUNCTION(builtin_divide,           "/");            
    APPEND_FUNCTION(builtin_equal,            "equal");             
    APPEND_FUNCTION(builtin_less_than,        "<");             
    APPEND_FUNCTION(builtin_greater_than,     ">");             
    APPEND_FUNCTION(builtin_whole_part,       "whole-part");        
    APPEND_FUNCTION(builtin_print,            "print");             
    APPEND_FUNCTION(builtin_import,           "import");
    APPEND_FUNCTION(builtin_extract_function, "extract-function");
    APPEND_FUNCTION(builtin_exit,             "exit");
#undef APPEND_FUNCTION
}

void eval_file(Any* stack, Any *env, FILE *fp) {
    while (fp) {
        Any *eval_or_eof = read_any(fp);
        if (eval_or_eof->type == EOF) break;
        stderr, eval_any(stack, env, eval_or_eof);
    }
}

void dl_test() {
    void *handle = dlopen("./libs/hello.so", RTLD_NOW);
    printf("%p\n", handle);
    if (!handle) {
        eprintf("%s\n", dlerror());
        return;
    }
        
    void (*dlfun)() = dlsym(handle, "print_twice");
    dlfun(make_symbol("asdf"));
    dlclose(handle);
}

int main(int argc, char *argv[]) {
    /* dl_test(); */
    /* return 0; */
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

    /* setbuf(stdout, NULL); */
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

