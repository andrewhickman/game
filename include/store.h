#ifndef STORE_H
#define STORE_H

#include "error.h"

/* A storage method for rarely-used components. */
struct store_sparse {
	void *buf;
	size_t len, cap;
	size_t *idx;
	size_t idx_len;
} store_sparse_create(size_t len, size_t size);

void *store_sparse_insert(struct store_sparse *, unsigned id, size_t size);
/* Index the storage */
void const *store_sparse_get(struct store_sparse const *, unsigned id, size_t size);
void *store_sparse_get_mut(struct store_sparse *, unsigned id, size_t size);
void store_sparse_destroy(struct store_sparse);

/* A storage method for commonly-used components. */
struct store_dense {
	void *buf;
	size_t len;
} store_dense_create(size_t len, size_t size);

void *store_dense_insert(struct store_dense *, unsigned id, size_t size);
void const *store_dense_get(struct store_dense const *, unsigned id, size_t size);
void *store_dense_get_mut(struct store_dense *, unsigned id, size_t size);
void store_dense_destroy(struct store_dense);

#endif