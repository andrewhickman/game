#ifndef GRAPH_H
#define GRAPH_H

#include "cpnt.h"
#include "error.h"
#include "vec.h"
#include "set.h"

#include <stddef.h>
#include <limits.h>

struct graph {
	struct graph_node {
		struct set edges;
		struct vec data;
		/* Pathfinding data */
		double cost;
		unsigned from;
		enum graph_node_state {
			GRAPH_EMPTY = 0,
			GRAPH_FILLED,
			GRAPH_PM_DATA,
		} state;
	} *buf;
	size_t len, cap;
	unsigned start;
} graph_create(void);

struct graph_edge {
	unsigned lo, hi;
};

struct vec graph_get(struct graph const *, unsigned);
bool graph_contains(struct graph const *, unsigned);
struct set_iter graph_neighbours(struct graph const *, unsigned);

/* Reserve space for additional nodes. */
void graph_reserve(struct graph *, size_t);

unsigned graph_add_node(struct graph *, struct set, struct vec);
/* Remove a node and all its edges. */
void graph_rm_node(struct graph *, unsigned);

void graph_rm_blocked(
	struct graph *graph, 
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll
);

struct gs_world;

void graph_add_blocked(
	struct graph *graph,
	struct gs_world *world,
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll
);

void graph_add_path_data(struct graph *graph, unsigned node, unsigned from, unsigned cost); 
void graph_clear_path_data(struct graph *graph);

double graph_cost(struct graph const *graph, unsigned a, unsigned b);

struct graph_node_iter {
	struct graph_node const *buf;
	size_t idx, cap;
} graph_node_iter(struct graph const *), graph_node_iter_empty(void);

struct graph_node_iter_result {
	bool finished;
	unsigned node;
	struct graph_node const *data;
} graph_node_iter_next(struct graph_node_iter *);

bool graph_edge_iter_finished(struct graph_node_iter const *);

struct graph_edge_iter {
	struct graph_node const *buf;
	size_t idx, cap;
	struct set_iter iter;
} graph_edge_iter(struct graph const *);

bool graph_edge_iter_next(struct graph_edge_iter *, struct graph_edge *);

void graph_destroy(struct graph);

#endif