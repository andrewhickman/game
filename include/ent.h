#ifndef ENTITY_H
#define ENTITY_H

#include "error.h"
#include "cpnt.h"

#include <stdbool.h>
#include <stddef.h>

/* Represents an object in the world. `gen` keeps track of whether `id` has been
 * re-used for another object. A generation of 0 represents a NULL entity.
 */
struct ent {
	/* All entities alive at any point have a unique id. */
	unsigned id;
	/* The generation the entity belongs to. */
	int gen;
};

/* Keeps track of all entities in the world */
struct ent_store {
	struct ent_data {
		/* The components an entity has. */
		enum cpnt cpnt;
		/* The number of times an id has been used. */
		int gen;
	} *buf;
	size_t len;
	unsigned start;
};

/* Create an entity store with room for `len` entities. */
struct ent_store_result {
	enum result result;
	struct ent_store value;
} ent_store_create(size_t len);

/* Allocate a new entity. */
struct ent_result {
	enum result result;	
	struct ent value;
} ent_store_spawn(struct ent_store *);

/* Kill an entity, if it is alive. */
void ent_store_kill(struct ent_store *, struct ent ent);

/* Add a component to an entity. Aborts if the entity is not alive. */
void ent_store_add_cpnt(struct ent_store *, struct ent, enum cpnt);

/* Remove a component from an entity, if it is still alive. */
bool ent_store_rm_cpnt(struct ent_store *, struct ent, enum cpnt);

enum cpnt ent_store_get_cpnt(struct ent_store const *, struct ent ent);

/* Check whether an entity is still alive. */
bool ent_store_is_alive(struct ent_store const *, struct ent ent);

/* Create an iterator over entities with the given components. */
struct ent_store_iter {
	struct ent_data *buf;
	size_t len;
	unsigned id;
	enum cpnt cpnt;
} ent_store_iter(struct ent_store const *es, enum cpnt cpnt);

/* Advance the iterator. */
struct ent_store_iter_result {
	struct ent ent;
	enum cpnt cpnt;
	bool finished;
} ent_store_iter_next(struct ent_store_iter *);

/* Destroy the entity store */
void ent_store_destroy(struct ent_store);

#endif