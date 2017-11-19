#include "store.h"
#include "util.h"

struct store_sparse store_sparse_create(size_t len, size_t size)
{
	struct store_sparse ret;

	ret.buf = xmalloc(len * size);
	ret.len = len;
	ret.cap = len;
	ret.idx = xmalloc(len * sizeof(size_t));
	ret.idx_len = len;

	return ret;
}

void *store_sparse_insert(
	struct store_sparse *store, 
	unsigned id, 
	size_t size
) {
	if (store->len == store->cap) {
		store->cap = store->cap ? store->cap * 2 : 1;
		store->buf = xrealloc(store->buf, store->cap * size);
	}

	if (id >= store->idx_len) {
		store->idx_len = util_next_pow_2(id);
		store->idx = xrealloc(store->idx, store->idx_len * size);
	}

	store->idx[id] = store->len++;
	return store_sparse_get_mut(store, id, size);
}

void const *store_sparse_get(
	struct store_sparse const *store, 
	unsigned id, 
	size_t size
) {
	return (char const *)store->buf + store->idx[id] * size;
}

void *store_sparse_get_mut(
	struct store_sparse *store, 
	unsigned id, 
	size_t size
) {
	return (char *)store->buf + store->idx[id] * size;
}

void store_sparse_destroy(struct store_sparse store)
{
	xfree(store.buf);
	xfree(store.idx);
}

struct store_dense store_dense_create(size_t len, size_t size)
{
	struct store_dense ret;

	ret.buf = xmalloc(len * size);
	ret.len = len;

	return ret;
}

void *store_dense_insert(struct store_dense *store, unsigned id, size_t size)
{
	if (id >= store->len) {
		store->len = util_next_pow_2(id);
		store->buf = xrealloc(store->buf, store->len * size);
	}

	return store_dense_get_mut(store, id, size);
}

void const *store_dense_get(struct store_dense const *store, unsigned id, size_t size)
{
	return (char const *)store->buf + id * size;
}

void *store_dense_get_mut(struct store_dense *store, unsigned id, size_t size)
{
	return (char *)store->buf + id * size;
}

void store_dense_destroy(struct store_dense store)
{
	xfree(store.buf);
}