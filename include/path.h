#ifndef PATH_H
#define PATH_H

#include "cpnt.h"
#include "graph.h"
#include "heap.h"

struct path_map {
	struct graph graph;
	struct heap open;
} path_map_create(void);

struct gs;
struct gs_world;

struct cpnt_coll path_add_coll(
	struct gs *gs, 
	struct cpnt_pos const *, 
	struct cpnt_draw const *, 
	enum cpnt_coll_shape
);

void path_rm_coll(
	struct gs *gs, 
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll coll
);

bool path_blocks(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll, 
	struct vec a, 
	struct vec b
);

bool path_los(struct gs_world *gs, struct vec a, struct vec b);

struct cpnt_path path_map_find_path(struct gs *, struct vec start, struct vec end);

void path_map_destroy(struct path_map);

#endif