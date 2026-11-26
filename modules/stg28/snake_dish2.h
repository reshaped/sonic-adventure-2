/* the second snake dish, SNAKEDISH2 in Pyramid Cave's object list: a solid
 * dish with a fire burning in it. its placement scales it */
#ifndef STG28_SNAKE_DISH2_H
#define STG28_SNAKE_DISH2_H
#include <ninja.h>

struct task;

/* the dish's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_snake_dish2_texlists[];

/* the object list's step for SNAKEDISH2. task needs a work. its pos places
 * the dish, and its ang.y turns it. the call sets ang.x and ang.z to 0. scl.x
 * plus 1 scales the dish, its fire and its shapes.
 *
 * the call gives task a collision record of eight shapes. the fire's hurts
 * players. the dish's seven push the shapes of players, enemies and objects,
 * and none of theirs pushes them. the fire's sound plays while player 0 lies
 * within 120 of a point above pos. the point's height is 110 times the
 * scale. player 0's work must not be null, unchecked */
void cave_snake_dish2_init(struct task *task);

#endif
