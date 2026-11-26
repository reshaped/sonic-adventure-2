/* the E-1000, an enemy with two guns that a stage's object list names
 * "E 1000". it stands where it was set, or flies about it */
#ifndef SHARED_ENEMY_1000_H
#define SHARED_ENEMY_1000_H

struct task;

/* the kinds, named for their titles in the image */
enum e1000_kind {
    E1000_GUN_FIXED, /* stands where it was set */
    E1000_GUN_MOVING, /* flies about where it was set */
    E1000_KINDS
};

/* the object list's step for "E 1000". its first call sets the E-1000 up,
 * and each later call runs it. task needs a work and a placement, as
 * object/set.h's set manager gives them. the work holds the E-1000's entry
 * in the set file:
 *
 * - scl.x: the kind, by its whole part. one that names no kind gives
 *   E1000_GUN_FIXED;
 * - scl.y: the shots' speed, which must not be 0, unchecked;
 * - scl.z: the range, less 60;
 * - ang.x: in its low byte, signed, the pairs of shots in a burst. a pair is
 *   a shot from each gun;
 * - ang.y: the heading, with the group in its low byte, as
 *   enemy_work_alloc_grouped of enemy/enemy.h takes it;
 * - ang.z: in its low half, signed, the interval between bursts in steps.
 *   one below 60 gives 60.
 *
 * a player within the range of home, across x and z, puts the E-1000 on
 * guard. home is where it was set. on guard, an E-1000 waits more than the
 * interval before each burst at the nearest player. a fixed kind stands on
 * guard, and turns toward that player. its first wait is more than 60 steps
 * instead. a moving kind flies along its heading. it turns toward home while
 * out of range, toward the nearest player while on guard, and otherwise
 * toward points at random near home. an E-1000 with 0 pairs or fewer never
 * fires. such an E-1000 never leaves its guard. one hit beats an E-1000, and
 * its next step kills it by placement_kill.
 *
 * without room for the E-1000's own record, the call leaves task as it was,
 * and the next call tries again. the heap must then have room for an
 * enemy_work and for the task that keeps the E-1000's textures, unchecked.
 * without room for a collision record, the E-1000 reads through null as it
 * is beaten, and as it turns toward a point near home.
 *
 * while game_state.time_stopped is set, an E-1000 neither moves nor turns,
 * and its timer and motion hold. its guns still pitch toward the nearest
 * player */
void e1000_step(struct task *task);

#endif
