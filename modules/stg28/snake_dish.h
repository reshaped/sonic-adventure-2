/* the snake dish, SNAKEDISH in Pyramid Cave's object list: a solid dish with a
 * fire burning in it */
#ifndef STG28_SNAKE_DISH_H
#define STG28_SNAKE_DISH_H
#include <ninja.h>

struct task;

/* the dish's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_snake_dish_texlists[];

/* the object list's step for SNAKEDISH. task needs a work. its pos places the
 * dish, and its ang.y turns it. the call sets ang.x and ang.z to 0.
 *
 * the call gives task a collision record of ten shapes. the fire's hurts
 * players. the dish's nine push the shapes of players, enemies and objects,
 * and none of theirs pushes them. the fire's sound plays while player 0 lies
 * within 120 of a point 110 above pos. player 0's work must not be null,
 * unchecked */
void cave_snake_dish_init(struct task *task);

#endif
