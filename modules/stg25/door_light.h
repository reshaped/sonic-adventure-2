/* the door light, DOOR LIGHT in Death Chamber's object list: a lamp of six
 * kinds beside a door. a glow over it pulses */
#ifndef STG25_DOOR_LIGHT_H
#define STG25_DOOR_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the lamp's texture lists for texture_load_shared, a kind's at its index,
 * ended by null */
extern NJS_TEXLIST *chamber_door_light_texlists[];

/* compile each kind's lamp at buf, and return buf past the builds. a model
 * compiles once, at the first call that finds its texture list loaded. the
 * lamps draw the builds once one call has compiled all six. the call sets
 * DRAW_CONTROL_SHADOW for the builds, and clears it after them. while
 * objects_fog_off isn't 0, it turns the fog off for the builds and on after
 * them */
Uint8 *chamber_door_light_build(Uint8 *buf);

/* the object list's step for DOOR LIGHT. task needs a work. its pos places the
 * lamp, and its ang.y turns it. the whole part of |scl.z| modulo 6 picks its
 * kind. kinds 0 to 2 are blue, green and red, and kinds 3 to 5 the same colors
 * with models of their own. the placement's names for them end in (3) and (4).
 * the glow over each lamp pulses in step with the scenery's tint of
 * shared/tint_pulse.h. it runs from black to white and back */
void chamber_door_light_init(struct task *task);

#endif
