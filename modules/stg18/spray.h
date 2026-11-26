/* the spray, EffectShibukiExec: a splash of water, drawn as 32 models, one a
 * frame */
#ifndef STG18_SPRAY_H
#define STG18_SPRAY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the frames' texture lists for texture_load_shared, a frame's at its index,
 * ended by null */
extern NJS_TEXLIST *spray_texlists[];

/* start a spray at pos, as a task at level 1, with its models scaled by
 * scale. returns the task, or null where task_create fails.
 *
 * the spray's frame starts at 0, and each step adds 1. the step that reaches
 * frame 31 kills the task, so that frame draws once. each draw turns the
 * frame's model about y by the current camera's ang.y */
struct task *spray_create(NJS_POINT3 *pos, Float scale);

#endif
