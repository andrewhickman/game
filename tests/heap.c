#include "heap.h"
#include "util.h"
#include "error.h"

#include <stdio.h>

bool int_lt(void const *a, void const *b)
{
	return *(int const *)a < *(int const *)b;
}

void test_heap(void)
{
	struct heap heap = heap_create();
	int vals[10] = { 8, 2, 5, 1, 6, 3, 4, 0, 7, 9 };
	int i;
	int val;

	for (i = 0; i < 10; ++i) {
		heap_push(&heap, &vals[i], sizeof(int), &int_lt);
	}

	for (i = 0; i < 10; ++i) {
		ASSERT((heap_pop(&heap, &val, sizeof(int), &int_lt), val) == i);
	}
}