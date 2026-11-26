/* the air an open hatch draws in: specks that spiral along a column into the
 * hatch */
#ifndef STG24_HATCH_AIR_H
#define STG24_HATCH_AIR_H
#include <ninja.h>
#include <sg_xpt.h>

struct angles;
struct task;

/* make a task of count specks that spiral in toward pos, along a column's z
 * axis. ang turns the column. a speck starts up to length past the column's
 * foot, which lies near pos. once it reaches the foot, it starts again near the
 * far end. a nonzero at_base starts each speck at the foot instead. size is
 * half the side of a speck's largest sprite, as njPtclDrawSprite takes it.
 *
 * with parent, the task is parent's child. without, it is a task of level 3
 * that leaves where placement_try_leave says. returns the task, or null where
 * task_create or syCalloc fails, or where the task leaves at once */
struct task *hatch_air_create(NJS_POINT3 *pos, struct angles *ang, Float length,
                              int count, Uint8 at_base, Float size,
                              struct task *parent);

extern NJS_TEXLIST engine_hatch_air_texlist;

#endif
