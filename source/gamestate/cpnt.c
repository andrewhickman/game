#include "gamestate/cpnt.h"

#include <stdlib.h>

struct gs_cpnt_sparse_result gs_cpnt_sparse_create(size_t len, size_t size)
{
	struct gs_cpnt_sparse_result ret;

	ret.value.buf = malloc(len * size);
	if (!ret.value.buf) goto fail_buf;
	ret.value.len = len;
	ret.value.cap = len;
	ret.value.indices = malloc(len * sizeof(size_t));
	if (!ret.value.indices) goto fail_indices;
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

void *gs_cpnt_sparse_insert(struct gs_cpnt_sparse *store, unsigned id, size_t size)
{
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
		size_t len = store->indices_len ? store->indices_len * 2 : 1;
		size_t *indices = realloc(store->indices, len * sizeof(size_t));
		if (!indices) {
			LOG_ERROR("out of memory");
			return NULL;
		}
		store->indices = indices;
		store->indices_len = len;
	}
	store->indices[id] = idx;

	return (char *)store->buf + idx * size;
}

void *gs_cpnt_sparse_get(struct gs_cpnt_sparse *cpnt, unsigned id, size_t size)
{
	return (char *)cpnt->buf + cpnt->indices[id] * size;
}

void gs_cpnt_sparse_destroy(struct gs_cpnt_sparse store)
{
	free(store.buf);
	free(store.indices);
}

struct gs_cpnt_dense_result gs_cpnt_dense_create(size_t len, size_t size);

void *gs_cpnt_dense_insert(struct gs_cpnt_dense *, unsigned id, size_t size);

void *gs_cpnt_dense_get(struct gs_cpnt_dense *cpnt, unsigned id, size_t size)
{
	return (char *)cpnt->buf + id * size;
}

void gs_cpnt_dense_destroy(struct gs_cpnt_dense);