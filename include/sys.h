#ifndef SYS_H
#define SYS_H

#include "SDL2/SDL.h"

#include "gs.h"
#include "resrc.h"
#include "error.h"

enum result sys_update(struct gs *);
enum result sys_draw(struct gs const *, SDL_Renderer *, SDL_Rect const *);

struct ent_result sys_new_unit(struct gs *, int x, int y, enum texture);

struct gs_find_result {
	bool found;
	struct ent ent;
}; 

struct gs_find_result sys_find_hover(struct gs *, int x, int y);
struct gs_find_result sys_find_select(struct gs *, int x, int y);

void sys_nuke(struct gs *gs, struct ent ent, int dmg);

struct sys_query_result {
	bool found;
	struct cpnt_hp hp;
	struct cpnt_draw draw;
} sys_query(struct gs const *, struct ent);

#endif