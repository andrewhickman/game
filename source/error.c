#include "error.h"

#include <stdlib.h>

void assert_impl(int val, char const *expr, char const *file, int line)
{
	if (!val) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "assertion '%s' failed in %s:%d, aborting.", 
		             expr, file, line);	
		exit(1);
	}
}