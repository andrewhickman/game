#ifndef ERROR_H
#define ERROR_H

#include "SDL2/SDL_log.h"

#include <stddef.h>

enum result {
	RESULT_OK = 0, RESULT_ERR = 1
};

void assert_impl(int val, char const *expr, char const *file, int line);

#define ASSERT(expr) assert_impl(expr, #expr, __FILE__, __LINE__)
#define LOG_ERROR(msg) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,              \
                                    "%s in %s:%d.", msg, __FILE__, __LINE__)
#define LOG_CHAIN() SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,                 \
                                 "   called from %s:%d.", __FILE__, __LINE__)
#define UNREACHABLE 0

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);
void xfree(void *);

#endif
