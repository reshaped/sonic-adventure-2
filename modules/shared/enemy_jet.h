/* the jets the enemies send out: puffs that fly, fade and end */
#ifndef SHARED_ENEMY_JET_H
#define SHARED_ENEMY_JET_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a jet's kind sets its size and how fast it fades. the kinds run from the
 * largest to the smallest. ENEMY_JET_MEDIUM fades fastest, and
 * ENEMY_JET_SMALL slowest */
enum enemy_jet_kind { ENEMY_JET_LARGE, ENEMY_JET_MEDIUM, ENEMY_JET_SMALL };

/* load the jets' texture, jet1, unless a task holds it already. that task
 * runs at task level 0, and releases the texture when it is destroyed. the
 * heap must have room for the task, unchecked */
void enemy_jet_load_texture(void);
/* send a jet of kind from pos, which moves by spd each step, and fades until
 * it ends. the jet is parent's child where parent isn't null, and else a task
 * at task level 3. a kind outside enemy_jet_kind counts as ENEMY_JET_LARGE.
 * the call loads the texture as enemy_jet_load_texture does. where the heap
 * has no room for the jet's task, the call does nothing.
 *
 * a jet also ends with parent, or, without a parent, once
 * placement_try_leave finds it out of range. while game_state.time_stopped is
 * set, a jet neither moves nor fades */
void enemy_jet_create(struct task *parent, enum enemy_jet_kind kind,
                      NJS_POINT3 *pos, NJS_VECTOR *spd);

#endif
