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

void mark(void *ptr) {
    if (!ptr) return;
    Allocation *allocation = ptr - sizeof(Allocation);
    allocation->marked = 1;
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
