#ifndef ERROR_H
#define ERROR_H

#include "SDL2/SDL_log.h"

enum result {
	RESULT_OK = 0, RESULT_ERR = 1
};

#define LOG_ERROR(msg) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,              \
                                    "%s in %s:%d.", msg, __FILE__, __LINE__)

#endif
