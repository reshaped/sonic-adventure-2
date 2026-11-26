/* the hidden animal, WSMMLS in a stage's object list. it is an unseen shape
 * that offers the player inside it an action. a player who takes the action
 * lets out a small animal */
#ifndef SHARED_HIDDEN_ANIMAL_H
#define SHARED_HIDDEN_ANIMAL_H

struct task;

/* the object list's step for WSMMLS. where task's placement has
 * PLACEMENT_UNK_20 of object/placement.h, the step passes task to
 * placement_kill. otherwise, unless placement_try_leave(task) returns TRUE,
 * it sets the shape up.
 *
 * task needs a work. bit 0 of ang.z picks the shape: 0 for an ellipsoid, 1
 * for a box. its half sizes along its own axes are scl.x, scl.y and scl.z,
 * each plus 10. ang.y turns it about y, then ang.x about x, and pos places
 * it.
 *
 * each step tests the player nearest pos. while that player's pos lies inside
 * the shape, the step offers it the action PLAYER_MODE_UNK_2A. where the
 * player's action_taken is PLAYER_MODE_UNK_2A, the step calls animal_release
 * of object/animal.h at pos instead. the animal's release kind is bits 4 to 7
 * of ang.z, modulo ANIMAL_RELEASE_KINDS. the step then sets the placement's
 * PLACEMENT_UNK_20 and passes task to placement_kill. it kills a task without
 * a placement */
void hidden_animal_init(struct task *task);

#endif
