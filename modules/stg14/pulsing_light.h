/* the pulsing light, RLIGHT in Radical Highway's object list: a lamp with a
 * glow that brightens and dims */
#ifndef STG14_PULSING_LIGHT_H
#define STG14_PULSING_LIGHT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the lamp's texture list and its glow's, for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *highway_pulsing_light_texlists[];

/* build the lamp's model at buf for njDirectDrawObject, and return buf past
 * the build. nothing is built where the build would reach DEMO_AREA of
 * system/areas.h. the lamp then draws as a chunk model */
Uint8 *highway_pulsing_light_build(Uint8 *buf);
/* the object list's step for RLIGHT. unless placement_try_leave(task) returns
 * TRUE, it gives task the light's own step, draw and destructor.
 *
 * task needs a work. pos places the lamp, and ang.y and ang.z turn it. the
 * glow's color pulses by the sine of the work's timer. each step adds 0x180 to
 * the timer while game_state.time_stopped is 0 */
void highway_pulsing_light_init(struct task *task);

#endif
