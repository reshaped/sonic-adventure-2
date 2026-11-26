/* the light effect, LIGHTEFF in a stage's object list: a flat quad, added to
 * what lies behind it. it flickers between three textures */
#ifndef SHARED_LIGHT_EFFECT_H
#define SHARED_LIGHT_EFFECT_H
#include <ninja.h>

struct task;

/* the effect's texture lists for texture_load_shared, ended by null. the one
 * list names miu64_meh011 to miu64_meh013 */
extern NJS_TEXLIST *light_effect_texlists[];

/* the object list's step for LIGHTEFF. unless placement_try_leave(task)
 * returns TRUE, it gives task the effect's own step, draw and destructor.
 *
 * task needs a work. the quad is 2 * scl.x wide and 2 * scl.y high. it is
 * centered on pos and turned by ang. on every odd step of the game, it takes
 * one of the three textures at random. it then flips each of u and v with a
 * chance of a half */
void light_effect_init(struct task *task);

#endif
