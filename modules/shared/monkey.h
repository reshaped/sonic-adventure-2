/* the monkey, E SARU in a stage's object list. it hops around the point it
 * was placed at. the normal kind throws bombs at a player in range */
#ifndef SHARED_MONKEY_H
#define SHARED_MONKEY_H

struct task;

/* a monkey's kind, the whole part of its scl.x. MONKEY_KINDS counts them */
enum monkey_kind {
    MONKEY_NORMAL, /* throws bombs */
    MONKEY_DOLL, /* only hops around */
    MONKEY_DOLL_NO_PUSH_OUT, /* a doll whose shape pushes only players */
    MONKEY_KINDS
};

/* the object list's step for a monkey. its first call sets the monkey up,
 * and later calls run it. one hit beats the monkey, and the call after the
 * hit passes task to placement_kill. while the monkey holds no bomb, a later
 * call returns at once where placement_try_leave returns TRUE.
 *
 * task needs a work and a placement, as object/set.h's set manager gives
 * them. the heap must have room for the monkey's enemy_work, its collision
 * record and its textures' task. a MONKEY_NORMAL also needs room for
 * enemy_bomb_load's task. scl.y must not be -40, at which the monkey divides
 * by 0. snapshot_current must not be null where the monkey throws BOMB_RING.
 * no call tests any of these.
 *
 * the work holds the pos, ang and scl of the monkey's set entry. a whole
 * part of scl.x that names no kind makes a MONKEY_NORMAL. the low byte of
 * ang.x is a MONKEY_NORMAL's enum bomb_kind. a byte that names no kind
 * counts as BOMB_NORMAL. the low half of ang.z is the monkey's interval
 * between throws, in steps. the low byte of ang.y is the monkey's group,
 * which enemy_work_alloc_grouped takes. where ang.x & 0xf000 isn't 0, the
 * setup moves pos down to the floor that surface_probe_floor finds. pos
 * stays where it finds none. where ang.x & 0xf00 isn't 0, the monkey raises
 * dust as it hops. the setup then clears ang.x and ang.z, and the monkey's
 * home is pos.
 *
 * the monkey hops around home, scl.y + 40 away. a player within scl.z + 80
 * of it, across x and z, is in range. a MONKEY_NORMAL throws a bomb at the
 * nearest player in range once each interval. a fumbled BOMB_HEAVY bursts in
 * its hands, and beats the monkey. a held bomb that draws a ring is thrown as a
 * scattered ring. the ring counts in game_state.ring_total and in
 * snapshot_current's */
void monkey_step(struct task *task);

#endif
