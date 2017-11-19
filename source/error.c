#include "error.h"

#include <stdlib.h>

void assert_impl(int val, char const *expr, char const *file, int line)
{
	if (!val) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "assertion '%s' failed in %s:%d, aborting.", 
		             expr, file, line);	
		abort();
	}
}

void oom(long unsigned size)
{
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
	             "allocation failed (requested: %luB).", size);
	abort();
}

void *xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (size && !ptr) oom(size);
	return ptr;
}

void *xcalloc(size_t count, size_t size)
{
	void *ptr = calloc(count, size);
	if (count && size && !ptr) oom(size);
	return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (size && !ptr) oom(size);
	return ptr;
}

void xfree(void *ptr)
{
	free(ptr);
}