#ifndef GAMESTATE_CPNT_H
#define GAMESTATE_CPNT_H

#include "error.h"
#include "set.h"

#include <stddef.h>

/*
  vel   draw
     \ /
     pos
*/

enum gs_cpnt {
	GS_CPNT_NONE = 0,
	GS_CPNT_POS = 1 << 0,
	GS_CPNT_VEL = (1 << 1) | GS_CPNT_POS,
	GS_CPNT_DRAW = (1 << 2) | GS_CPNT_POS
};

/* A storage method for rarely-used components. */
struct gs_cpnt_sparse {
	void *buf;
	size_t len, cap;
	size_t *indices;
	size_t indices_len;
};

struct gs_cpnt_sparse_result {
	enum result result;
	struct gs_cpnt_sparse value;
} gs_cpnt_sparse_create(size_t len, size_t size);

/* Return a space for insertion of new data */
void *gs_cpnt_sparse_insert(struct gs_cpnt_sparse *, unsigned id, size_t size);

/* Index the storage */
void *gs_cpnt_sparse_get(struct gs_cpnt_sparse *, unsigned id, size_t size);

void gs_cpnt_sparse_destroy(struct gs_cpnt_sparse);

/* A storage method for commonly-used components. */
struct gs_cpnt_dense {
	void *buf;
	size_t len;
};

struct gs_cpnt_dense_result {
	enum result result;
	struct gs_cpnt_dense value;
} gs_cpnt_dense_create(size_t len, size_t size);

/* Return a space for insertion of new data or NULL on failure */
void *gs_cpnt_dense_insert(struct gs_cpnt_dense *, unsigned id, size_t size);

void *gs_cpnt_dense_get(struct gs_cpnt_dense *, unsigned id, size_t size);

void gs_cpnt_dense_destroy(struct gs_cpnt_dense);

/* Components */

struct gs_pos {
	int x, y;
};

struct gs_vel {
	int x, y;
};

struct gs_draw {
	struct SDL_Texture *texture;
	int w, h;
};

#endif