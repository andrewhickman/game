#include "store.h"
#include "util.h"

#include <stdlib.h>

struct store_sparse_result store_sparse_create(size_t len, size_t size)
{
	struct store_sparse_result ret;

	ret.value.buf = malloc(len * size);
	if (len && !ret.value.buf) goto fail_buf;
	ret.value.len = len;
	ret.value.cap = len;
	ret.value.indices = malloc(len * sizeof(size_t));
	if (len && !ret.value.indices) goto fail_indices;
	ret.value.indices_len = len;
	
	ret.result = RESULT_OK;
	return ret;

fail_indices:
	free(ret.value.buf);
fail_buf:
	LOG_ERROR("out of memory");
	ret.result = RESULT_ERR;
	return ret;
}

void *store_sparse_insert(
	struct store_sparse *store, 
	unsigned id, 
	size_t size
) {
	size_t idx;
	if (store->len == store->cap) {
		size_t cap = store->cap ? store->cap * 2 : 1;
		void *buf = realloc(store->buf, cap * size);
		if (!buf) {
			LOG_ERROR("out of memory");
			return NULL;
		}
		store->buf = buf;
		store->cap = cap;
	}
	idx = store->len++;

	if (id >= store->indices_len) {
		size_t len = util_next_pow_2(id);
		size_t *indices = realloc(store->indices, len * size);
		if (!indices) {
			LOG_ERROR("out of memory");
			return NULL;
		}
		store->indices = indices;
		store->indices_len = len;
	}
	store->indices[id] = idx;

	return store_sparse_get_mut(store, id, size);
}

void const *store_sparse_get(
	struct store_sparse const *store, 
	unsigned id, 
	size_t size
) {
	return (char *)store->buf + store->indices[id] * size;
}

void *store_sparse_get_mut(
	struct store_sparse *store, 
	unsigned id, 
	size_t size
) {
	return (char *)store->buf + store->indices[id] * size;
}

void store_sparse_destroy(struct store_sparse store)
{
	free(store.buf);
	free(store.indices);
}

struct store_dense_result store_dense_create(size_t len, size_t size)
{
	struct store_dense_result ret;

	ret.value.buf = malloc(len * size);
	if (len && !ret.value.buf) {
		LOG_ERROR("out of memory");
		ret.result = RESULT_ERR;
		return ret;
	}
	ret.value.len = len;
	ret.result = RESULT_OK;
	return ret;
}

void *store_dense_insert(struct store_dense *store, unsigned id, size_t size)
{
	if (id >= store->len) {
		size_t len = util_next_pow_2(id);
		void *buf = realloc(store->buf, len * size);
		if (!buf) {
			LOG_ERROR("out of memory");
			return NULL;
		}
		store->buf = buf;
		store->len = len;
	}
	return store_dense_get_mut(store, id, size);
}

void const *store_dense_get(struct store_dense const *store, unsigned id, size_t size)
{
	return (char *)store->buf + id * size;
}

void *store_dense_get_mut(struct store_dense *store, unsigned id, size_t size)
{
	return (char *)store->buf + id * size;
}

void store_dense_destroy(struct store_dense store)
{
	free(store.buf);
}