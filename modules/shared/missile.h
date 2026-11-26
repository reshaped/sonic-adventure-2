/* the missile, ROCKETMISSSILE in a stage's object list and MISSSILE in a
 * boss's, as the images spell them. a base raises the missile. a player on
 * the button beside the base sends it to its destination. a table holds the
 * targets that wait for a missile of their id */
#ifndef SHARED_MISSILE_H
#define SHARED_MISSILE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

#define MISSILE_LAUNCH_BITS 0xf /* in ang.x: the bits that pick the launch */
#define MISSILE_UPSIDE_DOWN 0x10 /* in ang.x: the base hangs upside down */
#define MISSILE_ID 0x1f /* in ang.z: the bits that hold the missile's id */
/* the placement state a missile destroyed in its spent mode leaves. any state
 * but 0 starts the placement's next missile spent */
#define MISSILE_SPENT_STATE 1

/* the object list's step for a missile. unless placement_try_leave(task)
 * returns TRUE, it gives task a record in task->motion, and its own step,
 * draw and destructor. it makes a child task that carries the missile's
 * collision. where the heap can't give the record, the task keeps this step to
 * try again.
 *
 * task needs a work. pos places the base, and ang.y turns it. scl is the
 * missile's destination. the call clears every bit of ang.x but
 * MISSILE_LAUNCH_BITS and MISSILE_UPSIDE_DOWN, and every bit of ang.z but
 * MISSILE_ID. it adds a half turn to ang.z for a base upside down.
 *
 * the missile rises out of its base at a launch. once risen, it waits for a
 * player on the button before it aims and flies. MISSILE_LAUNCH_BITS of
 * ang.x, modulo 4, pick when it launches. 0 launches it each time it is back
 * in its base. 1 launches it once for its placement. 2 launches it while
 * shared/switch.h's switch of its id is on. the missile turns that switch off
 * as it returns to its base. 3 launches it while a player lies within 100 of
 * the base. the enemy group of its id must also be empty. under 3, a missile
 * that has landed never launches again. every launch also needs a target
 * that waits for the missile's id */
void missile_init(struct task *task);

#define MISSILE_TARGETS 16
/* an entry of the targets' table. a target is a pointer, *hit. a missile
 * of the target's id points it at the missile's position once it lands */
struct missile_target {
    Sint32 id; /* -1 while the entry is free */
    NJS_POINT3 **hit; /* null while the entry is free */
};
/* a bit of a target's id above the 16 a missile matches. a missile shutter's
 * target holds it until its door shuts */
#define TARGET_UNSHUT 0x80000000
/* the missile id that target waits for: the low 16 bits of its id */
#define TARGET_MISSILE_ID(target) ((Uint16)(target)->id)

extern struct missile_target missile_targets[MISSILE_TARGETS];

/* missile_target_add takes a free entry for id and hit, sets *hit to null
 * and returns the entry. where every entry is taken, it returns null and
 * leaves *hit as it was. missile_target_remove frees the entry of hit, sets
 * *hit to null and returns the entry. where no entry holds hit, it returns
 * null and leaves *hit as it was.
 *
 * a target waits for the missile whose id is the low 16 bits of its own. a
 * missile that lands points the *hit of each target that waits for it at
 * its position. it points none where its collision record shows a touch. a
 * later step returns it to its base, and sets each *hit that still points
 * there to null */
struct missile_target *missile_target_add(Sint32 id, NJS_POINT3 **hit);
struct missile_target *missile_target_remove(NJS_POINT3 **hit);

/* the missile's models, motions and texture lists. the base draws
 * missile_base_object, and the collision world holds a copy of the basic
 * model missile_base_collision */
extern NJS_TEXLIST missile_texlist;
extern NJS_CNK_OBJECT missile_object;
extern NJS_MOTION missile_motion;
extern NJS_TEXLIST missile_glow_texlist;
extern NJS_CNK_MODEL missile_glow_model;
extern NJS_TEXLIST missile_button_texlist;
extern NJS_CNK_OBJECT missile_button_object[1];
extern NJS_MOTION missile_base_motion;
extern NJS_TEXLIST missile_base_texlist;
extern NJS_CNK_OBJECT missile_base_object;
extern NJS_OBJECT missile_base_collision;

#endif
