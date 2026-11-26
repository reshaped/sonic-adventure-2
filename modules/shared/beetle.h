/* the beetle, E KUMI in a stage's object list: an enemy that bobs as it
 * hovers. it comes in eight kinds. every kind but the bomber turns toward a
 * player in range */
#ifndef SHARED_BEETLE_H
#define SHARED_BEETLE_H

struct task;

/* a beetle's kind, the whole part of its scl.x. a whole part outside these
 * makes a BEETLE_FIXED */
enum beetle_kind {
    BEETLE_FIXED, /* hovers where it was set */
    BEETLE_MOVING, /* flies along its heading */
    BEETLE_ELECTRIC, /* discharges around itself at intervals */
    BEETLE_MOVING_ELECTRIC, /* flies and discharges */
    BEETLE_GUNNER, /* fires at a player in range */
    BEETLE_WARP, /* hides until a player comes in range */
    BEETLE_BUMPER, /* throws upward a player who touches it */
    BEETLE_BOMBER, /* circles where it was set, dropping bombs */
    BEETLE_KINDS
};

/* the object list's step for E KUMI. task needs a work and a placement, as
 * object/set.h's set manager gives them. the heap must have room for the
 * beetle's enemy_work and collision record, unchecked. it must also have room
 * for the tasks that keep its textures loaded. a hit defeats a beetle, and so
 * does a player's touch of a BEETLE_MOVING. the step after that passes task to
 * placement_kill.
 *
 * every kind reads these fields, but where the list below says otherwise. a
 * fraction in scl.x leaves the beetle without a shadow. a player within
 * scl.z + 60 of it across x and z is in range. the low byte of ang.y is its
 * group, as enemy_work_alloc_grouped takes it. it bobs about home's height,
 * scl.y above and below. the bob's angle is ang.x, its low byte cleared, plus
 * ang.z times game_state.steps.
 *
 * - the electric kinds, the gunner and the bomber bob 2 high. each takes scl.y
 *   for a number of its own.
 * - a beetle of either moving kind flies along ang.y, at about its starting
 *   height above the floor. it strays no farther from home than its range,
 *   which must be above 0: its scl.z must be above -60, unchecked. its bob
 *   leaves ang.x out, and is about the height it flies at.
 * - an electric beetle hovers n + 1 steps, n the whole part of scl.y. it then
 *   discharges for 301 steps, and repeats. ang.x's low byte plus
 *   game_state.steps places its start in that cycle of n + 302 steps. n must
 *   not be -302, unchecked. with n 0, the first discharge never ends. a moving
 *   electric beetle whose start falls in a discharge starts the whole discharge
 *   at its second step. it stops while it discharges, and bobs about home's
 *   height then.
 * - a gunner fires a burst of ang.x & 0xf shots once a player has stayed in
 *   range more than n steps. n is ang.z's low byte, or 60 where that byte is
 *   less. the shots' speed is scl.y, which must not be 0, unchecked:
 *   enemy_bullet_fire then divides 0 by 0. the shots fly through the stage
 *   where ang.x & 0xf0 isn't 0. the gunner measures its range from a point 1000
 *   times scl.z's fraction ahead of where it was set. the range takes scl.z's
 *   whole part alone. its bob clears ang.z's low byte.
 * - a warp beetle counts the steps of its own hover in place of
 *   game_state.steps. its bob leaves out ang.z's low bit.
 * - a bumper throws a player upward at a speed of ang.x's low byte. only a
 *   touch of the wider of its two collision shapes, above its body, throws.
 * - a bomber circles where it was set, scl.z away. its heading is ang.y, its
 *   low byte cleared, plus ang.z times game_state.steps. it drops a bomb each
 *   n + 1 steps, n the whole part of scl.y. the low byte of ang.x is the
 *   bomb's kind, and a byte outside the kinds counts as 0. at its first step,
 *   its bob leaves ang.x out.
 *
 * a gunner draws its model as njCnkDirectObjectCompile compiled it. so do the
 * fixed, moving, electric and moving electric kinds where ang.z's low bit is
 * set. each draws its model uncompiled where the heap had no room for the
 * compiled one */
void beetle_step(struct task *task);

#endif
