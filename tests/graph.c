#include "error.h"
#include "graph.h"

void graph_add_edge(struct graph *graph, unsigned a, unsigned b)
{
	ASSERT(graph_contains(graph, a) && graph_contains(graph, b));
	set_insert(&graph->buf[a].edges, b);
	set_insert(&graph->buf[b].edges, a);
}

void test_graph()
{
	struct graph g = graph_create();
	struct set_iter iter;
	struct graph_edge_iter edge_iter;
	struct graph_edge result;
	struct vec data = { 0, 0 };
	unsigned set_res;

	graph_reserve(&g, 4);

	ASSERT(graph_add_node(&g, set_create_empty(), data) == 0);
	ASSERT(graph_add_node(&g, set_create_empty(), data) == 1);
	ASSERT(graph_add_node(&g, set_create_empty(), data) == 2);
	ASSERT(graph_add_node(&g, set_create_empty(), data) == 3);

	graph_add_edge(&g, 0, 1);
	graph_add_edge(&g, 3, 2);
	graph_add_edge(&g, 3, 1);
	graph_add_edge(&g, 2, 1);

	edge_iter = graph_edge_iter(&g);

	ASSERT(graph_edge_iter_next(&edge_iter, &result));
	ASSERT(result.lo == 0);
	ASSERT(result.hi == 1);
	ASSERT(graph_edge_iter_next(&edge_iter, &result));
	ASSERT(result.lo == 1);
	ASSERT(result.hi == 2);
	ASSERT(graph_edge_iter_next(&edge_iter, &result));
	ASSERT(result.lo == 1);
	ASSERT(result.hi == 3);
	ASSERT(graph_edge_iter_next(&edge_iter, &result));
	ASSERT(result.lo == 2);
	ASSERT(result.hi == 3);
	ASSERT(!graph_edge_iter_next(&edge_iter, &result));
	ASSERT(!graph_edge_iter_next(&edge_iter, &result));
	ASSERT(!graph_edge_iter_next(&edge_iter, &result));

	graph_rm_node(&g, 0);

	ASSERT(graph_add_node(&g, set_create_empty(), data) == 0);
	iter = graph_neighbours(&g, 0);
	ASSERT(!set_iter_next(&iter, &set_res));

	graph_destroy(g);
}