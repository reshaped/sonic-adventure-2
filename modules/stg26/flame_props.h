/* Egg Quarters' fire props and treasure chest, each in three colors */
#ifndef STG26_FLAME_PROPS_H
#define STG26_FLAME_PROPS_H
#include <ninja.h>

#include "stg26/prop_builds.h"

struct task;

/* the flames' texture lists, one for each color, and a small round shadow */
extern NJS_TEXLIST quarters_flame_blue_texlist;
extern NJS_TEXLIST quarters_flame_green_texlist;
extern NJS_TEXLIST quarters_flame_red_texlist;
extern NJS_TEXLIST quarters_round_shadow_small_texlist;
extern NJS_CNK_MODEL quarters_round_shadow_small_model;

/* each prop's texture lists, in CHAMBER_COLORS order and null-ended */
extern NJS_TEXLIST *quarters_fire_pot_texlists[];
extern NJS_TEXLIST *quarters_fire_mouth_texlists[];
extern NJS_TEXLIST *quarters_torch_texlists[];
extern NJS_TEXLIST *quarters_wall_torch_texlists[];
extern NJS_TEXLIST *quarters_treasure_texlists[];
extern NJS_TEXLIST *quarters_treasure_pieces_texlists[];

/* CHAMBER_COLOR picks each one's color from scl.z. the four flames hurt
 * players, and the treasure chest breaks when a hunter attacks it */
void quarters_fire_pot_init(struct task *task);
void quarters_fire_mouth_init(struct task *task);
void quarters_torch_init(struct task *task);
void quarters_wall_torch_init(struct task *task);
void quarters_treasure_init(struct task *task);

#endif
