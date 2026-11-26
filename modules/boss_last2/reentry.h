/* the reentry's burning ring, and its embers */
#ifndef BOSS_LAST2_REENTRY_H
#define BOSS_LAST2_REENTRY_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* a pointer may be null only where a comment says so */

/* create the ring as a child of parent at pos. where pos is null, it lies at
 * (-300, -120, 0). it shows from 3:00 on game_state's clock, at half
 * strength. it grows to its whole size and strength by 4:00. its sound fades
 * in from 2:00. returns the task, or null without parent or where the heap
 * can't give it */
struct task *finalhazard_reentry_ring(struct task *parent,
                                      const NJS_POINT3 *pos);
/* create an ember within range of pos on each axis, as a child of parent.
 * where parent is null, it is a task of level 3. it drifts and fades for 11
 * steps. returns the task, or null where the heap can't give it. no code
 * calls it */
struct task *finalhazard_ember_create(struct task *parent,
                                      const NJS_POINT3 *pos, Float range);
#endif
