/* the goal ring: GOALRING in a stage's object list, a ring whose touch clears
 * the stage. under MISSION_CHAO a Chao is the goal. under MISSION_RINGS, and
 * for a GOAL_RING_GOAL under MISSION_CHAO, the touch sends the player back to
 * the start instead */
#ifndef SHARED_GOAL_RING_H
#define SHARED_GOAL_RING_H
#include <ninja.h>

struct task;

/* a ring's kind, its work's ang.x modulo GOAL_RING_KINDS, named by the
 * object's own labels. a GOAL_RING_BACKONDMG draws as a ring, but steps as a
 * Chao. it plays the Chao's sounds, and never spins or rises */
enum goal_ring_kind { GOAL_RING_GOAL, GOAL_RING_CHAO, GOAL_RING_BACKONDMG };
#define GOAL_RING_KINDS 3

/* the goal ring's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *goal_ring_texlists[];

/* the object list's step for GOALRING. task needs a work, whose pos and ang
 * its set entry gives. it needs no record in motion, where the ring keeps its
 * own.
 *
 * while player 0 is Knuckles or Rouge, a ring that isn't a GOAL_RING_CHAO
 * keeps this step for good. it never shows. under MISSION_RINGS, every ring
 * becomes a GOAL_RING_BACKONDMG instead, which never leaves range. it keeps
 * this step until player 0 has a work whose timer is 120 or more.
 *
 * outside MISSION_CHAO, a GOAL_RING_CHAO keeps this step for good. it holds an
 * item box of ITEM_1UP as its child, at the work's pos.
 *
 * where placement_try_leave(task) returns TRUE, the call does nothing more.
 * otherwise it gives task a record in task->motion and a collision record. it
 * then gives task the ring's own step, draw and destructor. without room for
 * the record, task keeps this step and tries again at its next step. where
 * collision_init fails, no player can touch the ring.
 *
 * the first player to touch the ring or the Chao clears the stage through
 * stage_clear, and gets MESSENGER_EVENT_GOAL_RING. under MISSION_RINGS, and
 * for a GOAL_RING_GOAL under MISSION_CHAO, the touch calls stage_go_back
 * instead */
void goal_ring_init(struct task *task);

#endif
