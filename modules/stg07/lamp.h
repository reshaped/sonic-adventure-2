/* the lamp, MILUMP and milump in Aquatic Mine's object list. it glows, and
 * lights each player near it */
#ifndef STG07_LAMP_H
#define STG07_LAMP_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the lamp and of its glow,
 * ended by null */
extern NJS_TEXLIST *lamp_texlists[];

/* the object list's step for MILUMP and milump. it does nothing where task
 * leaves at its placement's range. otherwise it gives task the lamp's step,
 * draw and destructor, and a collision record of kind COLLISION_OBJECT. each
 * later step tests the range as the first did.
 *
 * task needs a work, whose pos places the lamp. its ang turns the lamp in the
 * order z, y, x. each step lights players 0 and 1 within 30 of pos by
 * object_light_add_near. object_light_jitter then turns each light's dir and
 * scales its color at random */
void lamp_init(struct task *task);

#endif
