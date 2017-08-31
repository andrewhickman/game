#ifndef BSET_H
#define BSET_H

#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define SET_BLOCK_SIZE (sizeof(unsigned))
#define SET_BLOCK_BITS (SET_BLOCK_SIZE * CHAR_BIT)

/* A nullable bit-bset. All functions assume the bset is not null. */
struct bset {
	/* len == 0 <==> buf == HEAP */
	unsigned *buf;
	size_t len;
};

struct bset bset_create_null(void);
struct bset bset_create_empty(void);

struct bset_result {
	enum result result;
	struct bset value;
} bset_create_with_capacity(size_t);

void bset_clear(struct bset *bset);
/* Take ownership of the bset, bsetting it to NULL */
struct bset bset_take(struct bset *bset);
/* Destroy the bset. Safe to call on a NULL bset. */
void bset_destroy(struct bset);

bool bset_is_null(struct bset const *);
size_t bset_capacity(struct bset const *);
/* Reserve space for elems less than n. Set is unchanged on failure */
enum result bset_reserve(struct bset *, size_t);
/* Minimize allocated memory. Set is unchanged on failure. 
enum result bset_shrink_to_fit(struct bset *); */

enum bset_insert_result {
	SET_WAS_FILLED = 0, SET_INSERTED = 1, SET_ERR
} bset_insert(struct bset *, size_t);
bool bset_remove(struct bset *, size_t);
bool bset_contains(struct bset const *, size_t);
size_t bset_next_slot(struct bset const *, size_t);

struct bset_iter_result {
	size_t value;
	bool finished;
}; 
 
struct bset_iter {
	struct bset set;
	size_t index;
	unsigned block;
} bset_iter(struct bset const *);

struct bset_iter_result bset_iter_next(struct bset_iter *);

#endif