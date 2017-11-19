#ifndef BSET_H
#define BSET_H

#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define SET_BLOCK_SIZE (sizeof(unsigned))
#define SET_BLOCK_BITS (SET_BLOCK_SIZE * CHAR_BIT)

struct set {
	unsigned *buf;
	size_t len;
};

struct set set_create_empty(void);
struct set set_create_with_capacity(size_t);

void set_clear(struct set *set);
/* Destroy the set. */
void set_destroy(struct set);

size_t set_capacity(struct set const *);

bool set_insert(struct set *, unsigned);
bool set_remove(struct set *, unsigned);
bool set_contains(struct set const *, unsigned);
 
/* Returns an iterator. */
struct set_iter {
	unsigned *buf;
	size_t idx, len;
	unsigned block;
} set_iter(struct set const *), set_iter_empty(void);

bool set_iter_next(struct set_iter *, unsigned *res);
bool set_iter_finished(struct set_iter const *);

#endif