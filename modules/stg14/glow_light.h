/* the glow light, GLIGHT in Radical Highway's object list: a glow that
 * streaks as the camera moves */
#ifndef STG14_GLOW_LIGHT_H
#define STG14_GLOW_LIGHT_H
#include <ninja.h>

struct task;

/* the glow's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *highway_glow_light_texlists[];

/* the object list's step for GLIGHT. unless placement_try_leave(task) returns
 * TRUE, it gives task a record of two streaks in task->motion. it also gives
 * task the light's own step, draw and destructor. where the heap can't give the
 * record, this stays task's step and tries again on the next. where the heap
 * can't give a streak, the draw on that streak's screen reads through null.
 *
 * task needs a work, whose pos places the glow. the glow streaks apart on each
 * of two players' screens. it streaks only while it lies less than 350 ahead
 * of the camera */
void highway_glow_light_init(struct task *task);

#endif
