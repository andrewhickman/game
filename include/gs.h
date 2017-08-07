#ifndef GS_H
#define GS_H

#include "SDL2/SDL.h"

#include "cpnt.h"
#include "ent.h"
#include "error.h"
#include "texture.h"
#include "store.h"

struct gs {
	struct ent_store entities;
	struct store_dense pos, vel; 
	struct store_sparse draw, select;
};

struct gs_result {
	enum result result;
	struct gs value;
	struct ent ents[4];
} gs_create(void);

enum result gs_update(struct gs *, SDL_Renderer *);

struct ent_result gs_new_unit(struct gs *, int x, int y, enum texture);

struct gs_find_result {
	bool found;
	struct ent ent;
}; 

struct gs_find_result gs_hover(struct gs *, int x, int y);
struct gs_find_result gs_select(struct gs *, int x, int y);

struct gs_query_result {
	bool found;
	int health;
} gs_query(struct gs const *, struct ent);

void gs_destroy(struct gs);

#endif