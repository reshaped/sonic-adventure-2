/* the uni, E UNI in a stage's object list: a rolling ball that four spikes
 * circle */
#ifndef SHARED_ENEMY_UNI_H
#define SHARED_ENEMY_UNI_H

struct task;

/* the kinds, named by their titles in the object's data. the low byte of
 * scl.x's whole part picks one. a byte that names no kind picks UNI_UNIUNI */
enum uni_kind {
    UNI_UNIUNI, /* its spikes tilt as they circle */
    UNI_GHORA, /* its spikes reach out while it chases, and glow */
    UNI_KINDS
};

/* the object list's step for E UNI. its first call sets the uni up. each
 * later call runs it, unless placement_try_leave(task) returns TRUE.
 *
 * task needs a work and a placement, as object/set.h's set manager gives
 * them. pos places the uni and is its home. the low byte of ang.y is the
 * uni's group, which enemy_work_alloc_grouped takes and clears. the rest of
 * ang.y is the uni's heading. the setup clears ang.x and ang.z, and sets
 * pos.y to the floor below pos.
 *
 * the heap must have room for the uni's enemy_work and collision record,
 * unchecked. the first uni of a kind needs room too for the kind's texture
 * task, unchecked.
 *
 * the uni's range is scl.z + 80. the uni rolls along its heading. it turns
 * toward a point near home once it strays out of range or touches an object.
 * while it lies in range of home, a player that enemy_find_player finds
 * starts a chase. the chase lasts while that call finds a player. while
 * game_state.time_stopped is set, the uni neither rolls nor falls, and its
 * turn holds.
 *
 * the four spikes are the uni's child tasks. a spike whose task finds no room
 * is left out, and its enemy_work and record are unchecked. a UNI_GHORA's
 * spike bursts and ends where it touches another record or a wall.
 *
 * one hit beats the uni, for 100 points, or 200 where player_touch_attacks
 * returns TRUE. its spikes then burst and end. the next call breaks the uni
 * into its pieces and passes task to placement_kill */
void uni_step(struct task *task);

#endif
