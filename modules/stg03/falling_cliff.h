/* the falling cliff, GAKE in the stage's object list. its six pieces fall
 * away once a player comes near, each after a delay of its own */
#ifndef STG03_FALLING_CLIFF_H
#define STG03_FALLING_CLIFF_H
#include <ninja.h>

struct task;

/* the pieces' texture lists, by piece, ended by null. the stage passes the
 * array to texture_load_shared among its groups */
extern NJS_TEXLIST *falling_cliff_texlists[];

/* the object list's step for GAKE. unless placement_try_leave_by returns
 * TRUE, it gives task the cliff's own step and destructor. it creates a child
 * task for each piece. the cliff's record in task->extra comes from syMalloc,
 * unchecked. a piece the heap can't give a task never lands. the pads then
 * shake for as long as a player stays near.
 *
 * task needs a work. pos and ang.y place the cliff. bit 0 of ang.x gives the
 * pieces short, slow falls. otherwise MISSION_HD gives them falls of its own.
 * a player within |scl.x| of the point 520 above pos wakes the cliff. its
 * pieces then fall away, and each one's collision falls with it. until the
 * last piece lands, the cliff shakes the pads of the players near it. a
 * player is near within 1000 of it across the ground. its x must also lie
 * between 9900 and 10790. once awake, the cliff and its pieces hold still
 * while game_state.time_stopped isn't 0.
 *
 * where task has a placement, the cliff sets the placement's state to 1. it
 * ends once set_reactivate or a reset clears the state */
void falling_cliff_init(struct task *task);

#endif
