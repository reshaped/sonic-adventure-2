/* PNP in Sky Rail's object list: a platform with three arms, which a landing
 * pushes up and spins */
#ifndef STG06_PNP_H
#define STG06_PNP_H
#include <ninja.h>

struct task;

/* the texture lists for texture_load_shared of the platform, its two marks and
 * the dust, ended by null */
extern NJS_TEXLIST *pnp_texlists[];

/* the object list's step for PNP. it does nothing where task leaves at its
 * placement's range. otherwise it gives task its own step, draw and
 * destructor, and a record in task->motion from syCalloc. the destructor frees
 * the record. where syCalloc fails, the call does nothing, and the next step
 * tries again. the work takes WORK_ID_HOMING_TARGET.
 *
 * task needs a work. its pos places the platform's base, and ang.y turns the
 * platform. the platform rests 19 above the base, and rises no higher than
 * scl.x + 50 above it.
 *
 * a landing on the platform sinks it and then pushes it up, by the landing's
 * speed. it spins the arms by that speed too. a falling player who lands
 * bounces back the way it came. after a falling landing faster than 2, the
 * player faces the hub. its flat speed becomes 0.4 where it was over 0.4. it
 * becomes 0.2 where it was over 0.01 but no more than 0.4. a time stop holds
 * the platform still, unless a player touches it during the stop */
void pnp_init(struct task *task);

#endif
