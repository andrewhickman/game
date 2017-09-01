#include "bset.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

/* A non-null pointer representing a zero-length buffer. */
#define EMPTY ((unsigned *)1)

void zero(unsigned *ptr, size_t len)
{
	while (len--) *ptr++ = 0;
}

struct bset bset_create_null(void)
{
	struct bset ret = { NULL, 0 };
	return ret;
}

struct bset bset_create_empty(void)
{
	struct bset ret = { EMPTY, 0 };
	return ret;
}

struct bset_result bset_create_with_capacity(size_t n)
{
	struct bset_result ret;

	ret.value.len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);
	if (ret.value.len) {
		ret.value.buf = calloc(ret.value.len, SET_BLOCK_SIZE);
		if (!ret.value.buf) {
			LOG_ERROR("out of memory"); 
			ret.result = RESULT_ERR;
			return ret;
		}
	} else {
		ret.value.buf = EMPTY;
	}

	ret.result = RESULT_OK;
	return ret;
}

void bset_clear(struct bset *bset)
{
	ASSERT(!bset_is_null(bset));
	zero(bset->buf, bset->len);
}

struct bset bset_take(struct bset *bset)
{
	struct bset ret = *bset;

	ASSERT(!bset_is_null(bset));
	*bset = bset_create_null();

	return ret;
}

void bset_destroy(struct bset bset) 
{
	if (bset.len) free(bset.buf);
}

bool bset_is_null(struct bset const *bset)
{
	return bset->buf == NULL;
}

size_t bset_capacity(struct bset const *bset)
{
	ASSERT(!bset_is_null(bset));
	return bset->len * SET_BLOCK_BITS;
}

/* Increase the size of the buffer. `len` must be greater than `bset->len`. */
enum result bset_resize(struct bset *bset, size_t len)
{
	unsigned *buf;
	ASSERT(len > bset->len);

	if (bset->len) {
		buf = realloc(bset->buf, len * SET_BLOCK_SIZE);
		if (!buf) goto fail;

		zero(buf + bset->len, len - bset->len);
	} else {
		buf = calloc(len, SET_BLOCK_SIZE);
		if (!buf) goto fail;
	}

	bset->buf = buf;
	bset->len = len;

	return RESULT_OK;

fail:
	LOG_ERROR("out of memory"); 
	return RESULT_ERR;
}

enum result bset_reserve(struct bset *bset, size_t n)
{
	size_t len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);

	ASSERT(!bset_is_null(bset));

	if (len <= bset->len) return RESULT_OK;
	return bset_resize(bset, len);
}

/* TODO: remove? */
enum result bset_shrink_to_fit(struct bset *bset)
{
	size_t len = bset->len;
	unsigned *buf;

	ASSERT(!bset_is_null(bset));

	for (; len && !bset->buf[len - 1]; --len);

	if (len) {
		buf = bset->len
			? realloc(bset->buf, len * SET_BLOCK_SIZE)
			: calloc(len, SET_BLOCK_SIZE);
		if (!buf) {
			LOG_ERROR("out of memory"); 
			return RESULT_ERR;
		}
	} else {
		if (bset->len) free(bset->buf);
		buf = EMPTY;
	}

	bset->buf = buf;
	bset->len = len;

	return RESULT_OK;
}

enum bset_insert_result bset_insert(struct bset *bset, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!bset_is_null(bset));

	if (block < bset->len) {
		if (bset->buf[block] & mask) {
			return SET_WAS_FILLED;
		} else {
			bset->buf[block] |= mask;
			return SET_INSERTED;
		}
	} else {
		if (bset_resize(bset, block + 1) == RESULT_ERR) return SET_ERR;
		bset->buf[block] = mask;
		return SET_INSERTED;
	}
}

bool bset_remove(struct bset *bset, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!bset_is_null(bset));

	if (block < bset->len) {
		if (bset->buf[block] & mask) {
			bset->buf[block] &= ~mask;
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool bset_contains(struct bset const *bset, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!bset_is_null(bset));

	if (block < bset->len) {
		return bset->buf[block] & mask;
	} else {
		return false;
	}
}

size_t bset_next_slot(struct bset const *bset, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!bset_is_null(bset));

	while (block < bset->len && (bset->buf[block] & mask)){
		++elem;
		mask <<= 1;
		if (!mask) {
			++block;
			mask = 1;
		}
	}

	return elem;
}

struct bset_iter bset_iter(struct bset const *set)
{
	struct bset_iter ret;

	ASSERT(!bset_is_null(set));

	ret.buf = set->buf;
	ret.len = set->len;
	ret.block = 0; 
	ret.idx = 0;
	return ret;
}

struct bset_iter bset_iter_empty(void)
{
	struct bset_iter ret = { 0 };

	return ret;
}

struct bset_iter_result bset_iter_next(struct bset_iter *iter)
{
	struct bset_iter_result ret;

	while (!iter->block) {
		if (iter->idx == iter->len) {
			ret.finished = true;
			return ret;
		}
		iter->block = iter->buf[iter->idx++];
	}
	ret.finished = false;
	ret.value = (iter->idx - 1) * SET_BLOCK_BITS 
		+ util_trailing_zeros(iter->block & ~(iter->block - 1));
	iter->block &= iter->block - 1;

	return ret;
}

bool bset_iter_finished(struct bset_iter const *iter)
{
	return iter->idx == iter->len && !iter->block;
}