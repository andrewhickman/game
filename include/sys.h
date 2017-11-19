#ifndef SYS_H
#define SYS_H

#include "SDL2/SDL.h"

#include "gs.h"
#include "resrc.h"
#include "error.h"

enum result sys_update(struct gs *);

enum sys_select_state {
	SYS_SELECT_NONE,	
	SYS_SELECT_HOVERED,
	SYS_SELECT_SELECTED
};

enum result sys_draw(
	struct cpnt_pos const *pos, 
	struct cpnt_draw const *draw, 
	enum sys_select_state select,
	SDL_Renderer *renderer,
	SDL_Rect const *cam
);

struct ent_result {
	struct ent value;
	enum result result;
} sys_new_unit(struct gs *, struct vec, enum texture);

struct sys_find_result {
	bool found;
	struct ent ent;
}; 

struct sys_find_result sys_find(struct gs *, struct vec);

void sys_nuke(struct gs *gs, struct ent ent, int dmg);

struct sys_query_result {
	bool found;
	struct cpnt_hp hp;
	struct cpnt_draw draw;
} sys_query(struct gs const *, struct ent);

#endif