/* the pulsing light, RLIGHT in a stage's object list: a lamp with a glow
 * that brightens and dims */
#ifndef SHARED_PULSING_LIGHT_H
#define SHARED_PULSING_LIGHT_H
#include <ninja.h>

struct task;

/* the lamp's and the glow's texture lists for texture_load_shared, ended by
 * null */
extern NJS_TEXLIST *pulsing_light_texlists[];

/* the object list's step for RLIGHT. it does nothing where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the light's
 * own step, draw and destructor. task needs a work, whose pos places the lamp
 * and whose ang.y and ang.z turn it. the work's timer holds the angle of the
 * glow's pulse */
void pulsing_light_init(struct task *task);

#endif
