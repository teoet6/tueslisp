#include "tueslisp.h"
#include <stdlib.h>

typedef struct Allocation Allocation;
struct Allocation {
    int marked;
    Allocation *next;
};

Allocation *global_allocations;

void *gcalloc(size_t s) {
    void *all = malloc(s + sizeof(Allocation));
    Allocation *allocation = all;
    allocation->marked = 0;
    allocation->next = global_allocations;
    global_allocations = allocation;
    
    void *data = all + sizeof(Allocation);
    return data;
}

int mark(void *ptr) {
    if (!ptr) return 1;
    Allocation *allocation = ptr - sizeof(Allocation);
    if (allocation->marked) return 1;
    allocation->marked = 1;
    return 0;
}

void sweep() {
    Allocation **p, *a;
    p = &global_allocations;
    while (a = *p) {
	if (a->marked) {
	    a->marked = 0;
	    p = &a->next;
	} else {
	    *p = a->next;
	    free(a);
	}
    }
}
