/* the container, DLCONT in Dry Lagoon's object list: a crate a player breaks
 * for points. a broken box can release one of the stage's small animals. a box
 * falls when a box under it breaks */
#ifndef STG18_CONTAINER_H
#define STG18_CONTAINER_H
#include <sg_xpt.h>

struct task;

/* build at buf the bright box's model and its 24 pieces', each not built yet.
 * a model waits until its texture list is loaded. return buf past what it
 * built. once a single call has built all 25, container_built is TRUE. nothing
 * in the game calls it */
Uint8 *lagoon_container_build(Uint8 *buf);

/* the object list's step for DLCONT. unless placement_try_leave(task) returns
 * TRUE, it gives task the box's step, draw and destructor, and a collision
 * record. the heap must have room for the record, unchecked. task needs a work,
 * and no record in extra, where the box keeps its floor's height. the box puts
 * itself in stack_tasks by stack_add of shared/common_object.h. where stack_add
 * returns 0, the box never falls.
 *
 * the whole part of scl.x's magnitude picks the box's variant. an even part
 * makes the box bright, and an odd one dark. while container_built is FALSE, a
 * box draws its variant's model and throws its variant's pieces. while it is
 * TRUE, a box sets its own variant's texture list and draws the bright box's
 * build. it throws the bright pieces' builds under their texture lists,
 * whatever its own variant.
 *
 * a hit breaks the box where player_attacks_task of player/player.h returns
 * TRUE for the hitting task. work->collision->hit is the record the box's
 * record touched latest this step. where that is a player's, the player stops
 * dead, and vibration_start shakes its pad. that player need not be the task
 * whose hit broke the box. Sonic in PLAYER_MODE_SONIC_BOUNCE neither stops nor
 * shakes.
 *
 * bit 0 of ang.x lets the box's pieces fall through their floor. the low byte
 * of ang.z sets the box's small animal. its high 4 bits are the chance of one,
 * in percent. a chance of 10 or more is certain. its low 4 bits, modulo
 * ANIMAL_RELEASE_KINDS, are the animal's kind, as animal_release in
 * object/animal.h takes it. the box rolls for the animal once its pieces have
 * flown */
void lagoon_container_init(struct task *task);

#endif
