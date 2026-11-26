/* the chase light, RBLK in Radical Highway's object list: a lamp of three
 * lights. they come on one at a time, in turn */
#ifndef STG14_CHASE_LIGHT_H
#define STG14_CHASE_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the lamp's texture list and its glow's, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *chase_light_texlists[];

/* build the lamp's body at buf for njDirectDrawObject, and return buf past
 * the build. nothing is built where the build would reach DEMO_AREA of
 * system/areas.h. the body then draws as a chunk model */
Uint8 *chase_light_build(Uint8 *buf);
/* the object list's step for RBLK. unless placement_try_leave(task) returns
 * TRUE, it gives task the light's own step, draw and destructor. it also gives
 * task a collision record of the lamp's box.
 *
 * task needs a work. pos places the lamp, and ang.y turns it. each light stays
 * lit for 32 steps, and only the lit light draws, with a glow. the lit light
 * stays lit while game_state.time_stopped isn't 0 */
void chase_light_init(struct task *task);

#endif
