/* the goal door, GOALDOOR in a stage's object list. a hit is a touch by a
 * task whose work's id is WORK_ID_ATTACK. the door breaks at the fourth hit
 * since its previous break or its task's start. it breaks three times. after
 * the last break it draws nothing. from the 120th step after that break, each
 * of its steps calls stage_clear for player 0 */
#ifndef SHARED_GOAL_DOOR_H
#define SHARED_GOAL_DOOR_H
#include <ninja.h>

struct task;

/* the texture lists of the door and its pieces for texture_load_shared, ended
 * by null */
extern NJS_TEXLIST *goal_door_texlists[];

/* the object list's step for GOALDOOR. it does nothing where task leaves at
 * its placement's range. otherwise it gives task the door's step, draw and
 * destructor. each later step tests the range as the first did.
 *
 * the placement's state keeps the door's count of breaks, 0 to 3, for the
 * placement's later tasks. a door that starts broken three times takes no
 * collision record. from the 120th step after this call, each of its steps
 * calls stage_clear for player 0. any other door takes a record of kind
 * COLLISION_OBJECT: a box that pushes players, enemies and objects. the box
 * stays after the last break. the door's steps read the record unchecked
 * until then, so the heap must have room for it.
 *
 * task needs a work and a placement. the work's pos places the door, and its
 * ang.y turns it */
void goal_door_init(struct task *task);

#endif
