/* the hornet, E NAMIE in a stage's object list: an enemy that carries bombs.
 * it hovers, and lets its bombs loose at a player ahead of it */
#ifndef SHARED_HORNET_H
#define SHARED_HORNET_H

struct task;

/* a hornet's kind, the whole part of its scl.x. a whole part outside these
 * makes a HORNET_3 */
enum hornet_kind {
    HORNET_3, /* carries 3 bombs */
    HORNET_6, /* carries 6 bombs */
    HORNET_9, /* carries 9 bombs */
    HORNET_LASER, /* fires lasers, and carries 3 bombs it never lets loose */
    HORNET_KINDS
};

/* the object list's step for E NAMIE. task needs a work and a placement, as
 * object/set.h's set manager gives them. it needs no record in extra, where
 * the hornet keeps its area. the heap must have room for the hornet's and
 * each bomb's enemy_work and collision record, unchecked. it must also have
 * room for the tasks that keep their textures loaded, unchecked.
 *
 * the first call sets the hornet up and makes its bombs as task's children.
 * without room for the area's record, the call sets nothing up and makes only
 * the bombs. the hornet then stays unset, so each later call makes more. once
 * the hornet is set up, each call returns at once where placement_try_leave
 * returns TRUE. the call after the hornet is beaten passes task to
 * placement_kill.
 *
 * the work holds the hornet's entry in the set file:
 *
 * - scl.x: the kind. a fraction leaves the hornet and its bombs without
 *   shadows;
 * - scl.y: how far the hornet may fly from home, less 60. it must be above
 *   -60, unchecked: enemy_keep_near_home takes the distance. a laser hornet
 *   may fly 60 from home. its scl.y is its lasers' speed instead, which must
 *   not be 0, unchecked;
 * - scl.z: the reach, less 60. a player within the reach of home, across x
 *   and z, is near;
 * - ang.y: the heading, with the group in its low byte, as
 *   enemy_work_alloc_grouped of enemy/enemy.h takes it;
 * - ang.x and ang.z: a laser hornet's lasers, or another kind's area.
 *
 * where neither ang.x nor ang.z is 0, any hornet but a laser hornet has an
 * area, a box. its loose bombs burst once outside the box, and pass through
 * the stage. the high byte of ang.x, signed, places the box's center along x
 * from pos. its low byte is the box's half width along x. ang.z does the same
 * along z. both bytes count in units of 2. the box's top lies 200 times
 * scl.y's fraction above pos, and its bottom 200 times scl.z's below pos. each
 * fraction is taken without its sign.
 *
 * a laser hornet fires once a player has stayed near longer than ang.z's low
 * half, signed, in steps. a laser's length is ang.x's low byte, which must
 * lie below 0x80, unchecked. the hornet reads the byte signed, and
 * enemy_laser_fire takes no negative length.
 *
 * any other hornet marks its placement used as it lets its bombs loose. so
 * does a bomb that a walker's lock holds as it dies. a hornet whose placement
 * is used lets no bombs loose. one made from a used placement carries none.
 *
 * an attack of work id WORK_ID_ATTACK is one other than a player's own body.
 * where collision_next_attack finds one in a step, the hornet and its carried
 * bombs ignore the step's hits. they take them where their enemy_work's
 * shot_by holds a player's slot */
void hornet_step(struct task *task);

#endif
