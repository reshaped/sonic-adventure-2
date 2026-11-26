/* the missile, MHMISSILE in Metal Harbor's object list: the rocket the stage
 * ends on. a player who grips one of its handholds before its count runs out
 * rides it up. the missile throws the rider off toward a point above the
 * harbor */
#ifndef STG10_HARBOR_MISSILE_H
#define STG10_HARBOR_MISSILE_H
#include <ninja.h>

struct task;

/* the missile's modes, in its work's mode */
enum harbor_missile_mode {
    HARBOR_MISSILE_MODE_WAIT, /* until a player comes near */
    /* 900 steps for a player to grip a handhold */
    HARBOR_MISSILE_MODE_COUNT,
    /* a player holds a handhold, and the missile stands on its pad */
    HARBOR_MISSILE_MODE_GRIPPED,
    HARBOR_MISSILE_MODE_LIFT_OFF, /* the missile rises with its rider */
    /* the count ran out, and the missile rises without a rider */
    HARBOR_MISSILE_MODE_EMPTY,
    /* the rider is thrown off, and the missile keeps rising */
    HARBOR_MISSILE_MODE_THROWN,
    /* the music changes, and the missile's task ends */
    HARBOR_MISSILE_MODE_END
};
/* the missile is above the clouds once its work's timer passes this in
 * HARBOR_MISSILE_MODE_LIFT_OFF or HARBOR_MISSILE_MODE_THROWN. the timer runs
 * on from the one mode into the other */
#define HARBOR_MISSILE_CLOUD_STEPS 240

/* the first missile's task, or null. harbor_missile_init sets it where it is
 * null, and that missile's destructor nulls it */
extern struct task *harbor_missile_task;

/* the object list's step for MHMISSILE. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion and a collision of
 * three shapes. it also gives task the missile's own step, draw and
 * destructor. where the heap can't give the record, this stays task's step
 * and tries again on the next.
 *
 * task needs a work. pos places the missile, and ang.y turns it. a player
 * within scl.x + 100 of pos starts the count */
void harbor_missile_init(struct task *task);

/* the missile's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_missile_texlists[];

#endif
