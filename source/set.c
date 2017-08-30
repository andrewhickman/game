#include "set.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

/* A non-null pointer representing a zero-length buffer. */
#define EMPTY ((unsigned *)1)

void zero(unsigned *ptr, size_t len)
{
	while (len--) *ptr++ = 0;
}

struct set set_create_null(void)
{
	struct set ret = { NULL, 0 };
	return ret;
}

struct set set_create_empty(void)
{
	struct set ret = { EMPTY, 0 };
	return ret;
}

struct set_result set_create_with_capacity(size_t n)
{
	struct set_result ret;

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

void set_clear(struct set *set)
{
	ASSERT(!set_is_null(set));
	zero(set->buf, set->len);
}

struct set set_take(struct set *set)
{
	struct set ret = *set;

	ASSERT(!set_is_null(set));
	*set = set_create_null();

	return ret;
}

void set_destroy(struct set set) 
{
	if (set.len) free(set.buf);
}

bool set_is_null(struct set const *set)
{
	return set->buf == NULL;
}

size_t set_capacity(struct set const *set)
{
	ASSERT(!set_is_null(set));
	return set->len * SET_BLOCK_BITS;
}

/* Increase the size of the buffer. `len` must be greater than `set->len`. */
enum result set_resize(struct set *set, size_t len)
{
	unsigned *buf;
	ASSERT(len > set->len);

	if (set->len) {
		buf = realloc(set->buf, len * SET_BLOCK_SIZE);
		if (!buf) goto fail;

		zero(buf + set->len, len - set->len);
	} else {
		buf = calloc(len, SET_BLOCK_SIZE);
		if (!buf) goto fail;
	}

	set->buf = buf;
	set->len = len;

	return RESULT_OK;

fail:
	LOG_ERROR("out of memory"); 
	return RESULT_ERR;
}

enum result set_reserve(struct set *set, size_t n)
{
	size_t len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);

	ASSERT(!set_is_null(set));

	if (len <= set->len) return RESULT_OK;
	return set_resize(set, len);
}

/* TODO: remove? */
enum result set_shrink_to_fit(struct set *set)
{
	size_t len = set->len;
	unsigned *buf;

	ASSERT(!set_is_null(set));

	for (; len && !set->buf[len - 1]; --len);

	if (len) {
		buf = set->len
			? realloc(set->buf, len * SET_BLOCK_SIZE)
			: calloc(len, SET_BLOCK_SIZE);
		if (!buf) {
			LOG_ERROR("out of memory"); 
			return RESULT_ERR;
		}
	} else {
		if (set->len) free(set->buf);
		buf = EMPTY;
	}

	set->buf = buf;
	set->len = len;

	return RESULT_OK;
}

enum set_insert_result set_insert(struct set *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!set_is_null(set));

	if (block < set->len) {
		if (set->buf[block] & mask) {
			return SET_WAS_FILLED;
		} else {
			set->buf[block] |= mask;
			return SET_INSERTED;
		}
	} else {
		if (set_resize(set, block + 1) == RESULT_ERR) return SET_ERR;
		set->buf[block] = mask;
		return SET_INSERTED;
	}
}

bool set_remove(struct set *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!set_is_null(set));

	if (block < set->len) {
		if (set->buf[block] & mask) {
			set->buf[block] &= ~mask;
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool set_contains(struct set const *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!set_is_null(set));

	if (block < set->len) {
		return set->buf[block] & mask;
	} else {
		return false;
	}
}

size_t set_next_slot(struct set const *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	ASSERT(!set_is_null(set));

	while (block < set->len && (set->buf[block] & mask)){
		++elem;
		mask <<= 1;
		if (!mask) {
			++block;
			mask = 1;
		}
	}

	return elem;
}

struct set_iter set_iter(struct set const *set)
{
	struct set_iter ret;

	ASSERT(!set_is_null(set));

	ret.set = *set;
	ret.block = 0;
	ret.index = 0;
	return ret;
}

struct set_iter_result set_iter_next(struct set_iter *iter)
{
	struct set_iter_result ret;
	while (!iter->block) {
		if (iter->index == iter->set.len) {
			ret.finished = true;
			return ret;
		}
		iter->block = iter->set.buf[iter->index++];
	}
	ret.finished = false;
	ret.value = (iter->index - 1) * SET_BLOCK_BITS;
	ret.value += util_trailing_zeros(iter->block & ~(iter->block - 1));
	iter->block &= iter->block - 1;
	return ret;
}