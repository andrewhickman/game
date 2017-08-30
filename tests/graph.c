#include "error.h"
#include "graph.h"

void test_graph()
{
	struct graph g = graph_create();
	struct set_iter iter;
	struct vec data = { 0, 0 };

	graph_reserve(&g, 4);

	ASSERT(graph_add_node(&g, data) == 0);
	ASSERT(graph_add_node(&g, data) == 1);
	ASSERT(graph_add_node(&g, data) == 2);
	ASSERT(graph_add_node(&g, data) == 3);

	graph_add_edge(&g, 0, 1);
	graph_add_edge(&g, 3, 2);
	graph_add_edge(&g, 3, 1);
	graph_add_edge(&g, 0, 1);

	graph_rm_node(&g, 0);

	ASSERT(graph_add_node(&g, data) == 0);
	iter = graph_neighbours(&g, 0);
	ASSERT(set_iter_next(&iter).finished);

	graph_destroy(g);
}