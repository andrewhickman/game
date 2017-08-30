#ifndef SET_H
#define SET_H

#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define SET_BLOCK_SIZE (sizeof(unsigned))
#define SET_BLOCK_BITS (SET_BLOCK_SIZE * CHAR_BIT)

/* A nullable bit-set. All functions assume the set is not null. */
struct set {
	/* len == 0 <==> buf == HEAP */
	unsigned *buf;
	size_t len;
};

struct set_result {
	enum result result;
	struct set value;
};

struct set set_create_null(void);
struct set set_create_empty(void);
struct set_result set_create_with_capacity(size_t);

void set_clear(struct set *set);
/* Take ownership of the set, setting it to NULL */
struct set set_take(struct set *set);
/* Destroy the set. Safe to call on a NULL set. */
void set_destroy(struct set);

bool set_is_null(struct set const *);
size_t set_capacity(struct set const *);
/* Reserve space for elems less than n. Set is unchanged on failure */
enum result set_reserve(struct set *, size_t);
/* Minimize allocated memory. Set is unchanged on failure. 
enum result set_shrink_to_fit(struct set *); */

enum set_insert_result {
	SET_WAS_FILLED = 0, SET_INSERTED = 1, SET_ERR
} set_insert(struct set *, size_t);
bool set_remove(struct set *, size_t);
bool set_contains(struct set const *, size_t);
size_t set_next_slot(struct set const *, size_t);

struct set_iter_result {
	size_t value;
	bool finished;
}; 
 
struct set_iter {
	struct set set;
	size_t index;
	unsigned block;
} set_iter(struct set const *);

struct set_iter_result set_iter_next(struct set_iter *);

#endif