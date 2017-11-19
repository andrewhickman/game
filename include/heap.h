#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stddef.h>

struct heap {
	void *buf;
	size_t len, cap;
};

struct heap heap_create(void);

void heap_push(
	struct heap *heap, 
	void const *val, 
	size_t size, 
	bool (*lt)(void const *, void const *)
);

void heap_pop(
	struct heap *heap, 
	void *val, 
	size_t size, 
	bool (*lt)(void const *, void const *)
);

void heap_clear(struct heap *heap);

void heap_destroy(struct heap);

#endif