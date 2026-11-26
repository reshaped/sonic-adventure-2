/* the lock-on missile, RMISSILE and MISSILE in Iron Gate's object list. it
 * rises from its base once a missile shutter of its id has shut. once its weak
 * point is hurt, it flies to its destination */
#ifndef STG11_LOCK_ON_MISSILE_H
#define STG11_LOCK_ON_MISSILE_H
#include <ninja.h>

struct task;

/* the texture list of every model of the missile and its base */
extern NJS_TEXLIST lock_on_missile_texlist;
/* the texture lists for texture_load_shared: lock_on_missile_texlist, the
 * burst's and the thrust's, ended by null */
extern NJS_TEXLIST *lock_on_missile_texlists[];

/* the object list's step for a lock-on missile. it does nothing where task
 * leaves at its placement's range. where the heap can't give the record in
 * task->motion, it does nothing, and the next step tries again. otherwise it
 * gives task the missile's step, draw and destructor. it makes a child task
 * that carries the missile's collision. where the heap can't give the child,
 * the risen missile never flies. the port frees the record instead, and the
 * next step tries again. it gives task a collision record of kind
 * COLLISION_OBJECT for the base. where the heap has no room for it, the base
 * goes on without one.
 *
 * task needs a work. pos places the base, and ang.y turns it. scl is the
 * missile's destination. the low 5 bits of ang.z are the missile's id. bit 4
 * of ang.x hangs the base upside down. the call clears the other bits of
 * ang.z, and those of ang.x above bit 4. it adds a half turn to ang.z for a
 * base upside down.
 *
 * the missile launches into view once a target in shared/missile.h's
 * missile_targets waits for it. bits 0 to 3 of ang.x, modulo 3, pick the
 * launch. under 2 the missile also waits for shared/switch.h's switch of its id
 * to be on. under 0 and 1 it waits for nothing more. it rises once one of those
 * targets has bit 31 of its id clear. a missile shutter's target clears the bit
 * as its door shuts. the risen missile makes a weak point where an upright base
 * has its cover. it does so for a base upside down too. a hurt to the weak
 * point throws the cover off. the missile then aims and flies to its
 * destination. it holds still while game_state.time_stopped is set.
 *
 * as it lands, the missile points the *hit of each target that waits for it
 * at its position. the step after returns it to its base, and sets each *hit
 * that still points there to null.
 *
 * a task's missile flies only once. once it has flown, and the task leaves at
 * its placement's range, the placement's later missiles never launch */
void lock_on_missile_init(struct task *task);

#endif
