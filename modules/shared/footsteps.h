/* footsteps: a task that plays a sound at each step of a player's feet */
#ifndef SHARED_FOOTSTEPS_H
#define SHARED_FOOTSTEPS_H

struct task;

/* make the footsteps of the player in slot, and return their task. the task
 * is an unnamed child of parent. where parent is null, it is a root task at
 * task level 3, named ObjectAsiotoExec. it returns null where slot is 2 or
 * more or has no player. it returns null too where the heap can't give the
 * task. slot must not be below 0, unchecked.
 *
 * the task takes unk_20c and unk_200 of the player's record as its feet.
 * it takes unk_2a8 and unk_29c as their ways down. it plays first_sound for
 * unk_20c's foot, and second_sound for unk_200's. a Sonic's foot steps where
 * njInnerProduct of its way down and the record's unk_2cc passes 0.97. a
 * Knuckles's foot steps where it passes 0.65, and no other character's ever
 * does. a stepping foot plays its sound once it lies over the player's
 * consts.radius from where it last played. until it first plays, that is
 * where it lay at the call.
 *
 * nothing plays while the player stands neither on ground nor on a shape, as
 * PLAYER_WORK_GROUNDED tells. nothing plays either while the record's
 * ground_flags lack GROUND_FOOTSTEPS of collision/world.h */
struct task *footsteps_create(int slot, int first_sound, int second_sound,
                              struct task *parent);

#endif
