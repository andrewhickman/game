#include "error.h"
#include "graph.h"

void test_graph()
{
	struct graph g = graph_create();
	struct bset_iter iter;
	struct graph_edge_iter edge_iter;
	struct graph_edge_iter_result result;
	struct vec data = { 0, 0 };

	graph_reserve(&g, 4);

	ASSERT(graph_add_node(&g, data) == 0);
	ASSERT(graph_add_node(&g, data) == 1);
	ASSERT(graph_add_node(&g, data) == 2);
	ASSERT(graph_add_node(&g, data) == 3);

	graph_add_edge(&g, 0, 1);
	graph_add_edge(&g, 3, 2);
	graph_add_edge(&g, 3, 1);
	graph_add_edge(&g, 2, 1);

	edge_iter = graph_edge_iter(&g);

	result = graph_edge_iter_next(&edge_iter);
	ASSERT(!result.finished);
	ASSERT(result.value.lo == 0);
	ASSERT(result.value.hi == 1);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(!result.finished);
	ASSERT(result.value.lo == 1);
	ASSERT(result.value.hi == 2);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(!result.finished);
	ASSERT(result.value.lo == 1);
	ASSERT(result.value.hi == 3);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(!result.finished);
	ASSERT(result.value.lo == 2);
	ASSERT(result.value.hi == 3);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(result.finished);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(result.finished);
	result = graph_edge_iter_next(&edge_iter);
	ASSERT(result.finished);

	graph_rm_node(&g, 0);

	ASSERT(graph_add_node(&g, data) == 0);
	iter = graph_neighbours(&g, 0);
	ASSERT(bset_iter_next(&iter).finished);

	graph_destroy(g);
}