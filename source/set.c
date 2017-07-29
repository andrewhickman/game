#include "set.h"

#include <stdlib.h>
#include <string.h>

void fill(unsigned *ptr, size_t len)
{
	while (len--) *ptr++ = UINT_MAX;
}

void zero(unsigned *ptr, size_t len)
{
	while (len--) *ptr++ = 0;
}

struct set set_create_empty(void)
{
	struct set ret = { NULL, 0 };
	return ret;
}

struct set_result set_create_filled(size_t n)
{
	struct set_result ret;
	size_t nblocks = n / SET_BLOCK_BITS, nbits = n % SET_BLOCK_BITS;

	ret.value.len = nblocks + (nbits != 0);
	ret.value.buf = malloc(ret.value.len * SET_BLOCK_SIZE);

	if (ret.value.buf) {
		ret.result = RESULT_OK;
		fill(ret.value.buf, ret.value.len);
		if (nbits) ret.value.buf[nblocks] = (1u << nbits) - 1;
	} else {
		LOG_ERROR("out of memory"); 
		ret.result = RESULT_ERR;
	}

	return ret;
}

struct set_result set_create_with_capacity(size_t n)
{
	struct set_result ret;

	ret.value.len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);
	ret.value.buf = calloc(ret.value.len, SET_BLOCK_SIZE);
	if (ret.value.buf) {
		ret.result = RESULT_OK;
	} else {
		LOG_ERROR("out of memory"); 
		ret.result = RESULT_ERR;
	}

	return ret;
}

void set_clear(struct set *set)
{
	zero(set->buf, set->len);
}

void set_destroy(struct set set) 
{
	free(set.buf);
}

size_t set_capacity(struct set const *set)
{
	return set->len * SET_BLOCK_BITS;
}

enum result set_resize(struct set *set, size_t len)
{
	unsigned *buf = realloc(set->buf, len * SET_BLOCK_SIZE);
	if (!buf) {
		LOG_ERROR("out of memory"); 
		return RESULT_ERR;
	}

	zero(buf + set->len, len - set->len);
	set->buf = buf;
	set->len = len;

	return RESULT_OK;
}

enum result set_reserve(struct set *set, size_t n)
{
	size_t len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);
	if (len <= set->len) return RESULT_OK;
	return set_resize(set, len);
}

enum result set_shrink_to_fit(struct set *set)
{
	size_t len = set->len;
	unsigned *buf;

	for (; len && !set->buf[len - 1]; --len);

	if (len) {
		buf = realloc(set->buf, len * SET_BLOCK_SIZE);
		if (!buf) {
			LOG_ERROR("out of memory"); 
			return RESULT_ERR;
		}
	} else {
		free(set->buf);
		buf = NULL;
	}

	set->buf = buf;
	set->len = len;

	return RESULT_OK;
}

enum set_insert_result set_insert(struct set *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	if (block < set->len) {
		if (set->buf[block] & mask) {
			return false;
		} else {
			set->buf[block] |= mask;
			return true;
		}
	} else {
		if (set_resize(set, block + 1) == RESULT_ERR) return SET_ERR;
		set->buf[block] = mask;
		return true;
	}
}

bool set_remove(struct set *set, size_t elem)
{
	size_t block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1u << bit;

	if (block < set->len) {
		if (set->buf[block] & mask) {
			set->buf[block] &= !mask;
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
	ret.start = set->buf;
	ret.end = set->buf + set->len;
	ret.block = set->buf ? set->buf[0] : 0;
	ret.pos = 0;
	return ret;
}

struct set_iter_result set_iter_next(struct set_iter *iter)
{
	struct set_iter_result ret;
	while (!iter->block) {
		if (++iter->start == iter->end) {
			ret.finished = true;
			return ret;
		}
		iter->block = *iter->start;
	}
	while (!(iter->block & 0x1)) {
		iter->block >>= 1;
		++iter->pos;
	}
	iter->block >>= 1;
	++iter->pos;
	ret.finished = false;
	ret.value = iter->pos;
	return ret;
}