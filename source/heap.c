#include "heap.h"
#include "error.h"

#include <string.h>
 
void swap(void *a, void *b, size_t size)
{
	char tmp[size];

	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}

struct heap heap_create(void)
{
	struct heap ret = { NULL, 0, 0 };
	return ret;
}

void heap_push(
	struct heap *heap, 
	void const *val, 
	size_t size, 
	bool (*lt)(void const *, void const *)
) {
	size_t node;
	char (*buf)[size];

	if (heap->len == heap->cap) {
		heap->cap = 2 * heap->cap + 1;
		heap->buf = xrealloc(heap->buf, heap->cap * size);
	}
	buf = heap->buf;

	node = heap->len++;
	memcpy(buf + node, val, size);

	while (node) {
		size_t parent = (node - 1) / 2;
		
		if (lt(buf + node, buf + parent)) {
			swap(buf + parent, buf + node, size);
		}

		node = parent;
	}
}

void heap_sift_down(
	struct heap *heap,
	size_t i,
	size_t size,	
	bool (*lt)(void const *, void const *)
) {
	char (*buf)[size] = heap->buf;

	size_t l = 2 * i + 1;
	size_t r = 2 * i + 2;
	size_t min = i;

	if (l < heap->len && lt(buf + l, buf + min)) min = l;
	if (r < heap->len && lt(buf + r, buf + min)) min = r;

	if (min != i) {
		swap(buf + i, buf + min, size);
		heap_sift_down(heap, min, size, lt);
	}
}

void heap_pop(
	struct heap *heap, 
	void *val, 
	size_t size, 
	bool (*lt)(void const *, void const *)
) {
	char (*buf)[size] = heap->buf;

	ASSERT(heap->len);
	memcpy(val, buf, size);
	if (--heap->len) {
		memcpy(buf, buf + heap->len, size);
		heap_sift_down(heap, 0, size, lt);
	}
}

void heap_clear(struct heap *heap)
{
	heap->len = 0;
}

void heap_destroy(struct heap heap)
{
	xfree(heap.buf);
}