#include <stdio.h>
#include <tueslisp.h>

Any *print_twice(Any *a) {
    print_any(stdout, a);
    print_any(stdout, a);
    return a;
}
