#include "error.h"
#include "path.h"
#include "geom.h"
#include "gs.h"

#include <math.h>

struct path_map path_map_create(void)
{
	struct path_map ret = { graph_create(), heap_create() };
	return ret;
}

struct path_nodes_result {
	struct set nodes;
	unsigned offset;
};

bool path_blocks(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll const *coll, 
	struct vec a, 
	struct vec b
) {
	switch (coll->shape) {
	case CPNT_COLL_CIRCLE:
		return geom_circle_line_collide(vec_sub(a, *pos), vec_sub(b, a), draw->w / 2);
	case CPNT_COLL_RECT: {
		struct vec p = *pos;
		p.x -= draw->w / 2;
		p.y -= draw->h / 2;
		return geom_rect_line_collide(vec_sub(a, p), vec_sub(b, a), draw->w, draw->h);
	}}

	ASSERT(UNREACHABLE);
	return false;
}

bool path_los(struct gs_world *gs, struct vec a, struct vec b)
{
	struct ent_store_iter iter = ent_store_iter(&gs->ents, CPNT_POS | CPNT_DRAW | CPNT_COLL);
	struct ent_store_iter_result res;

	while (ent_store_iter_next(&iter, &res)) {
		struct cpnt_pos const *pos = 
			store_dense_get(&gs->pos, res.ent.id, sizeof(*pos));
		struct cpnt_draw const *draw = 
			store_sparse_get(&gs->draw, res.ent.id, sizeof(*draw));
		struct cpnt_coll const *coll = 
			store_dense_get(&gs->coll, res.ent.id, sizeof(*coll));

		if (path_blocks(pos, draw, coll, a, b)) {
			return false;
		}
	}

	return true;
}

struct set path_edges(
	struct gs *gs, 
	struct vec vec
) {
	struct set edges;
	struct graph_node_iter iter = graph_node_iter(&gs->path.graph);
	struct graph_node_iter_result res;

	edges = set_create_with_capacity(gs->path.graph.cap);
	while (!(res = graph_node_iter_next(&iter)).finished) {
		if (path_los(&gs->cpnt, vec, res.data->data)) {
			set_insert(&edges, res.node);
		}
	}

	return edges;
}

unsigned path_add_node(struct gs *gs, struct vec vec)
{
	return graph_add_node(&gs->path.graph, path_edges(gs, vec), vec);
}

#define COUNT 12
#define STEP (2.0 * PI / COUNT)

struct path_nodes_result path_nodes_circle(
	struct gs *gs, 
	struct cpnt_pos const *pos,
	struct cpnt_draw const *draw
) {
	struct path_nodes_result ret;
	double cx = pos->x, cy = pos->y;
	size_t i;
	double r = (draw->w / 2) / cos(STEP);
	double angle = 0;
	struct vec vec;

	ret.nodes = set_create_with_capacity(COUNT);
	graph_reserve(&gs->path.graph, COUNT);

	vec.x = lround(cx + r);
	vec.y = lround(cy);
	ret.offset = path_add_node(gs, vec);
	set_insert(&ret.nodes, 0);
	for (i = 1, angle = STEP; i != COUNT; ++i, angle += STEP) {
		unsigned id;

		vec.x = lround(cx + r * cos(angle));
		vec.y = lround(cy + r * sin(angle));

		id = path_add_node(gs, vec);
		set_insert(&ret.nodes, id - ret.offset);
	}

	return ret;
}

struct path_nodes_result path_nodes_rect(
	struct gs *gs, 
	struct cpnt_pos const *pos,
	struct cpnt_draw const *draw
) {
	struct path_nodes_result ret;
	unsigned id;
	struct vec vec, p = *pos;
	p.x -= draw->w / 2;
	p.y -= draw->h / 2;

	ret.nodes = set_create_with_capacity(4);
	graph_reserve(&gs->path.graph, 4);

	vec.x = p.x - 1;
	vec.y = p.y - 1;
	ret.offset = path_add_node(gs, vec);
	set_insert(&ret.nodes, 0);

	vec.x = p.x + draw->w + 1;
	id = path_add_node(gs, vec);
	set_insert(&ret.nodes, id - ret.offset);

	vec.y = p.y + draw->h + 1;
	id = path_add_node(gs, vec);
	set_insert(&ret.nodes, id - ret.offset);

	vec.x = p.x - 1;
	id = path_add_node(gs, vec);
	set_insert(&ret.nodes, id - ret.offset);

	return ret;
}
 
struct cpnt_coll path_add_coll(
	struct gs *gs, 
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	enum cpnt_coll_shape shape
) {
	struct cpnt_coll coll;

	coll.shape = shape;

	/* Insert the new nodes */
	{
		struct path_nodes_result res;
		switch (shape) {
		case CPNT_COLL_CIRCLE:
			res = path_nodes_circle(gs, pos, draw);
			break;
		case CPNT_COLL_RECT:
			res = path_nodes_rect(gs, pos, draw);
			break;
		default:
			ASSERT(UNREACHABLE);
		}
		coll.nodes = res.nodes;
		coll.offset = res.offset;
	}

	/* Remove edges that are now blocked */
	graph_rm_blocked(&gs->path.graph, pos, draw, &coll);

	return coll;
}

void path_rm_coll(
	struct gs *gs, 
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	struct cpnt_coll coll
) {
	struct set_iter iter = set_iter(&coll.nodes);
	unsigned node;

	while(set_iter_next(&iter, &node)) {
		graph_rm_node(&gs->path.graph, coll.offset + node);
	}

	graph_add_blocked(&gs->path.graph, &gs->cpnt, pos, draw, &coll);

	cpnt_coll_destroy(coll);
}

struct pm_data {
	unsigned id;
	double cost;
};

bool pm_data_lt(void const *a, void const *b)
{
	return ((struct pm_data const *)a)->cost < ((struct pm_data const *)b)->cost;
}

struct cpnt_path path_map_find_path(struct gs *gs, struct vec start_data, struct vec end_data)
{
	struct cpnt_path ret = { NULL, 0 };

	struct graph *graph = &gs->path.graph;
	struct heap *open = &gs->path.open;

	struct pm_data data;
	unsigned start, end, id;
	size_t i;

	/* Shortcut for common case */
	if (path_los(&gs->cpnt, start_data, end_data)) {
		ret.buf = xmalloc(sizeof(struct vec));
		ret.buf[ret.idx = 0] = end_data;
		return ret;
	}

	/* Prepare data structures */
	graph_reserve(graph, 2);
	start = path_add_node(gs, start_data);
	end = path_add_node(gs, end_data);

	heap_clear(open);
	data.cost = 0.0;
	data.id = start;
	heap_push(open, &data, sizeof(data), &pm_data_lt);

	graph_clear_path_data(graph);
	graph_add_path_data(graph, start, -1, 0.0);

	while (open->len) {
		unsigned cur, next;
		struct set_iter iter;

		heap_pop(open, &data, sizeof(data), &pm_data_lt);
		cur = data.id;

		if (cur == end) goto found;

		iter = graph_neighbours(graph, cur);
		while (set_iter_next(&iter, &next)) {
			unsigned cost = graph->buf[cur].cost + graph_cost(graph, cur, next);
			if (graph->buf[next].state != GRAPH_PM_DATA || 
			    cost < graph->buf[next].cost) {
				graph_add_path_data(graph, next, cur, cost);
				data.id = next; 
				data.cost = cost + graph_cost(graph, next, end);
				heap_push(open, &data, sizeof(data), &pm_data_lt);
			}
		}
	}

	ret.buf = NULL;

	graph_rm_node(graph, start);
	graph_rm_node(graph, end);
	return ret;

found:
	for (i = 0, id = end; id != start; id = graph->buf[id].from) ++i;

	ret.idx = i - 1;
	ret.buf = xmalloc(i * sizeof(struct vec));

	for (i = 0, id = end; id != start; id = graph->buf[id].from) {
		ret.buf[i++] = graph_get(graph, id);
	}

	graph_rm_node(graph, start);
	graph_rm_node(graph, end);
	return ret;
}

void path_map_destroy(struct path_map pm)
{
	graph_destroy(pm.graph);	
	heap_destroy(pm.open);
}