/* the light bar, LIGHTBAR in a stage's object list: a bobbing bar of seven
 * lights. the lit light runs along the bar, and the light it leaves fades */
#ifndef SHARED_LIGHT_BAR_H
#define SHARED_LIGHT_BAR_H
#include <ninja.h>

struct task;

/* the bar's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *light_bar_texlists[];

/* the object list's step for LIGHTBAR. unless placement_try_leave(task)
 * returns TRUE, it gives task the bar's own step, draw and destructor. it also
 * gives task a collision record of one cylinder, as COLLISION_OBJECT. where
 * the heap can't give the record, the bar bobs and draws without one.
 *
 * task needs a work. pos places the bar, and ang.y turns it. the bar and its
 * cylinder bob by up to 5 either way of pos. the lit light moves on every 16
 * steps of the game. where scl.x is 1 or more, it runs the other way */
void light_bar_init(struct task *task);

#endif
