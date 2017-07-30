#ifndef SET_H
#define SET_H

#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define SET_BLOCK_SIZE (sizeof(unsigned))
#define SET_BLOCK_BITS (SET_BLOCK_SIZE * CHAR_BIT)

/* A bit-set */
struct set {
	unsigned *buf;
	/* len == 0 <=> buf == 0 */
	size_t len;
};

struct set_result {
	enum result result;
	struct set value;
};

struct set set_create_empty(void);
struct set_result set_create_filled(size_t);
struct set_result set_create_with_capacity(size_t);

void set_clear(struct set *set);
void set_destroy(struct set);

size_t set_capacity(struct set const *);
/* Reserve space for elems less than n. Set is unchanged on failure */
enum result set_reserve(struct set *, size_t);
/* Minimize allocated memory. Set is unchanged on failure. */
enum result set_shrink_to_fit(struct set *);

enum set_insert_result {
	SET_WAS_FILLED, SET_INSERTED, SET_ERR
} set_insert(struct set *, size_t);
bool set_remove(struct set *, size_t);
bool set_contains(struct set const *, size_t);
size_t set_next_slot(struct set const *, size_t);

struct set_iter_result {
	bool finished;
	size_t value;
}; 

struct set_iter {
	struct set set;
	unsigned block;
	size_t index;
} set_iter(struct set const *);

struct set_iter_result set_iter_next(struct set_iter *);

struct set_and_iter {
	struct set const *sets;
	size_t nsets;
	unsigned block;
	size_t index;
} set_and_iter(struct set const *, size_t nsets);

struct set_iter_result set_and_iter_next(struct set_and_iter *);

#endif