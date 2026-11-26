/* the fire dish, FIRE4 in a stage's object list: a dish with a flame that
 * hurts the players */
#ifndef SHARED_FIRE_DISH_H
#define SHARED_FIRE_DISH_H
#include <ninja.h>

struct task;

/* the dish's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *fire_dish_texlists[];

/* the object list's step for FIRE4. it starts the dish as wall_fire_init of
 * shared/wall_fire.h starts a wall fire. the dish's flame stands straight
 * above it. the call plays the stage's sound 2 once, for 16 steps, 12 times
 * the scale above pos. it plays only where player 0 lies within 100 of that
 * point, and no later step plays it */
void fire_dish_init(struct task *task);
extern NJS_CNK_MODEL fire_dish_model;
extern NJS_TEXLIST fire_dish_texlist;

#endif
