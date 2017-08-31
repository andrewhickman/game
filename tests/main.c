#include "error.h"

#include "SDL2/SDL_log.h"

void test_es(), test_bset(), test_graph(), test_hset();

int main() {
	test_es();
	test_bset();
	test_graph();
	test_hset();

	SDL_Log("all tests passed.");
	return 0;
}