/* SNAKEPOT in Egg Quarters' object list: a pot with eight flames on its rim */
#ifndef STG26_SNAKE_POT_H
#define STG26_SNAKE_POT_H
#include <ninja.h>

struct task;

/* the object list's step for SNAKEPOT. task needs a work. its pos places the
 * pot, and its ang.y turns it. scl.x + 1 scales the pot across, and scl.y + 1
 * scales it up. the call starts nothing where the heap can't give it a child
 * task.
 *
 * the players collide with copies of the pot's outer and inner shapes.
 * world_pool_alloc lends them, and the pot adds them to the collision world.
 * the pool must have both to lend, unchecked. task->extra holds the outer copy,
 * and the child's task->extra the inner. the flames, in task's collision
 * record, hurt players */
void quarters_snake_pot_init(struct task *task);
/* the pot's texture list, alone in a list ended by null */
extern NJS_TEXLIST *quarters_snake_pot_texlists[];
extern NJS_TEXLIST snake_pot_texlist;

#endif
