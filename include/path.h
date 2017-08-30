#ifndef PATH_H
#define PATH_H

#include "cpnt.h"
#include "ent.h"
#include "graph.h"
#include "set.h"

struct path_data {
	struct cpnt_coll coll;
};

struct path_map {
	struct graph nodes;
	struct path_ent {
		struct cpnt_coll coll;
		struct set nodes;
		unsigned offset;
	} *ents;
	size_t len;
};

/* A queue of positions */
struct path {
	struct vec *buf;
	size_t idx;
};

enum result path_map_add(struct path_map *, struct ent, struct cpnt_coll const *);

void path_map_rm(struct path_map *, struct ent);

struct path_result {
	enum result result;
	struct path value;
} path_map_find_path(struct vec start, struct vec end);

#endif