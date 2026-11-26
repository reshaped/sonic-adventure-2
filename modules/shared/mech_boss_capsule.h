/* the mech boss's capsule: a child task whose collision capsule the players can
 * hurt. it follows a point and angles the boss keeps. Big Foot, Hot Shot and
 * Flying Dog each make one */
#ifndef SHARED_MECH_BOSS_CAPSULE_H
#define SHARED_MECH_BOSS_CAPSULE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

/* what mech_boss_capsule_create builds a capsule from */
struct mech_boss_capsule_spec {
    /* the boss whose capsule it is, an enum mech_boss of shared/mech_boss.h */
    Sint8 boss;
    NJS_POINT3 *pos; /* the point the capsule follows */
    struct angles *ang; /* the angles it takes */
    /* the capsule is a target only while its pos lies within range of the
     * world's y axis */
    Float range;
};

/* create the capsule as parent's child, and return its task. it returns null
 * where task_create_child fails, or where the heap can't give the capsule its
 * own record. that task then ends on its next step. parent must not be null:
 * for a null parent the result is undefined.
 *
 * the capsule takes spec->boss's shape, one SHAPE_CAPSULE in a COLLISION_ITEM
 * record of radius 80. each step its work takes *spec->pos and *spec->ang,
 * and it registers its record. the record is a COLLISION_TARGET while that pos
 * lies within spec->range of the world's y axis. spec->pos and spec->ang must
 * stay valid while the task lives.
 *
 * spec->boss must be one of enum mech_boss, unchecked. where it is none of
 * them, the work gets no record. the call then writes through null, as it
 * does where collision_init can't give one */
struct task *mech_boss_capsule_create(struct task *parent,
                                      struct mech_boss_capsule_spec *spec);
/* give the capsule's shape the attack and guard its boss's shape starts with.
 * clear WORK_HURT in its work. the capsule must hold a record, unchecked */
void mech_boss_capsule_reset(struct task *task);

#endif
