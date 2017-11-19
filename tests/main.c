#include "error.h"

#include "SDL2/SDL_log.h"

void test_es(), test_set(), test_graph(), test_heap();

int main() {
	test_es();
	test_set();
	test_graph();
	test_heap();

	SDL_Log("all tests passed.");
	return 0;
}