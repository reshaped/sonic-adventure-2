/* Pumpkin Hill's breakable drum. Sky Rail and Aquatic Mine supply their own
 * geometry and hit responses in stg06/drum_can.h and stg07/drum_can.h. */
#ifndef SHARED_DRUM_CAN_H
#define SHARED_DRUM_CAN_H
#include <ninja.h>

struct task;

/* the can's texture lists and its pieces', ended by null. the stage's
 * texture groups hold the array */
extern NJS_TEXLIST *drum_can_texlists[];

/* the object list's step for DRMCN. unless placement_try_leave(task) returns
 * TRUE, it gives task the can's own step, draw and destructor. it also gives
 * the can a collision record and a missile target.
 *
 * task needs a work. pos and ang.y place the can. the low five bits of ang.z
 * are its id among the missiles' targets.
 *
 * a player's attack, as player_attacks_task tells it, hits the can. so does a
 * missile of its id. the hit throws a cloud of dust, and bounces player 0 as
 * enemy_bounce_player does, whichever player hit. at each of the five steps
 * after, the can plays its sound and scores 20 points. it breaks into its
 * pieces at the second of them. it kills itself at the fifth, by
 * placement_kill where a placement made it */
void drum_can_init(struct task *task);

#endif
