#ifndef STORE_H
#define STORE_H

#include "error.h"

/* A storage method for rarely-used components. */
struct store_sparse {
	void *buf;
	size_t len, cap;
	size_t *indices;
	size_t indices_len;
};

struct store_sparse_result {
	enum result result;
	struct store_sparse value;
} store_sparse_create(size_t len, size_t size);

/* Return a space for insertion of new data or NULL on failure */
void *store_sparse_insert(struct store_sparse *, unsigned id, size_t size);

/* Index the storage */
void *store_sparse_get(struct store_sparse *, unsigned id, size_t size);

void store_sparse_destroy(struct store_sparse);

/* A storage method for commonly-used components. */
struct store_dense {
	void *buf;
	size_t len;
};

struct store_dense_result {
	enum result result;
	struct store_dense value;
} store_dense_create(size_t len, size_t size);

/* Return a space for insertion of new data or NULL on failure */
void *store_dense_insert(struct store_dense *, unsigned id, size_t size);

void *store_dense_get(struct store_dense *, unsigned id, size_t size);

void store_dense_destroy(struct store_dense);

#endif