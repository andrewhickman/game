#include "set.h"
#include "util.h"

#include <string.h>

void zero(unsigned *ptr, size_t len)
{
	while (len--) *ptr++ = 0;
}

struct set set_create_empty(void)
{
	struct set ret = { NULL, 0 };
	return ret;
}

struct set set_create_with_capacity(size_t n)
{
	struct set ret;

	ret.len = n / SET_BLOCK_BITS + (n % SET_BLOCK_BITS != 0);
	ret.buf = xcalloc(ret.len, SET_BLOCK_SIZE);

	return ret;
}

void set_clear(struct set *set)
{
	zero(set->buf, set->len);
}

void set_destroy(struct set set) 
{
	xfree(set.buf);
}

size_t set_capacity(struct set const *set)
{
	return set->len * SET_BLOCK_BITS;
}

/* Increase the size of the buffer. `len` must be greater than `set->len`. */
void set_resize(struct set *set, size_t len)
{
	ASSERT(len > set->len);
	set->buf = xrealloc(set->buf, len * SET_BLOCK_SIZE);
	zero(set->buf + set->len, len - set->len);
	set->len = len;
}

bool set_insert(struct set *set, unsigned elem)
{
	unsigned block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1U << bit;

	if (block < set->len) {
		if (set->buf[block] & mask) {
			return false;
		} else {
			set->buf[block] |= mask;
			return true;
		}
	} else {
		set_resize(set, block + 1);
		set->buf[block] = mask;
		return true;
	}
}

bool set_remove(struct set *set, unsigned elem)
{
	unsigned block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1U << bit;

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

bool set_contains(struct set const *set, unsigned elem)
{
	unsigned block = elem / SET_BLOCK_BITS, bit = elem % SET_BLOCK_BITS;
	unsigned mask = 1U << bit;

	if (block < set->len) {
		return set->buf[block] & mask;
	} else {
		return false;
	}
}

struct set_iter set_iter(struct set const *set)
{
	struct set_iter ret;

	ret.buf = set->buf;
	ret.len = set->len;
	ret.block = 0; 
	ret.idx = 0;
	return ret;
}

struct set_iter set_iter_empty(void)
{
	struct set_iter ret = { 0 };

	return ret;
}

bool set_iter_next(struct set_iter *iter, unsigned *res)
{
	while (!iter->block) {
		if (iter->idx == iter->len) return false;
		iter->block = iter->buf[iter->idx++];
	}
	*res = (iter->idx - 1) * SET_BLOCK_BITS + util_pop_lsb(&iter->block);

	return true;
}

bool set_iter_finished(struct set_iter const *iter)
{
	return iter->idx == iter->len && !iter->block;
}