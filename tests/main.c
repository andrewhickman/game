#include "error.h"

#include "SDL2/SDL_log.h"

void test_es(), test_set(), test_graph();

int main() {
	test_es();
	test_set();
	test_graph();

	SDL_Log("all tests passed.");
	return 0;
}