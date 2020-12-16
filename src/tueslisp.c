#include "tueslisp.h"
#include <stdio.h>

int main() {
    while (1) {
	print_any(stdout, read_any(stdin));
	printf("\n");
    }
}
