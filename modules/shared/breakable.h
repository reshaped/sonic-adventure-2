/* the work of the objects a missile breaks: Pumpkin Hill's shed and the drum
 * can of shared/drum_can.h */
#ifndef SHARED_BREAKABLE_H
#define SHARED_BREAKABLE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/task.h"

/* a breakable object's work, laid over the struct task_work that task_create
 * allocates. its fields are task_work's, but for hit, which lies at scl.x */
struct breakable_work {
    Sint8 mode;
    Sint8 submode;
    Uint8 id;
    /* the break's steps, counted from 1, or 0 while the object stands */
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    struct angles ang;
    NJS_POINT3 pos;
    /* where a missile of the object's id landed, or null.
     * missile_target_add takes its address */
    NJS_POINT3 *hit;
    Float scl_y;
    Float scl_z;
    struct collision_work *collision;
};
#define BREAKABLE(task) ((struct breakable_work *)(task)->work)

#endif
