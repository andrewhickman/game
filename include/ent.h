#ifndef ENTITY_H
#define ENTITY_H

#include "error.h"
#include "cpnt.h"

#include <stdbool.h>
#include <stddef.h>

struct ent {
	unsigned id;
	int gen;
};

struct ent_store {
	struct ent_data {
		enum cpnt cpnt;
		/* Negative = dead, 0 = never alive, positive = alive */
		int gen;
	} *buf;
	size_t len;
	unsigned start;
};

struct ent_store_result {
	enum result result;
	struct ent_store value;
} ent_store_create(size_t len);

struct ent_result {
	enum result result;	
	struct ent value;
} ent_store_new(struct ent_store *);

void ent_store_kill(struct ent_store *, struct ent ent);

struct ent_store_iter {
	struct ent_data *buf;
	size_t len;
	unsigned id;
} ent_store_iter(struct ent_store const *es);

struct ent_store_iter_result {
	bool finished;
	enum cpnt cpnt;
	struct ent ent;
} ent_store_iter_next(struct ent_store_iter *);

void ent_store_destroy(struct ent_store);

#endif