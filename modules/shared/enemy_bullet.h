/* the bullets the enemies fire */
#ifndef SHARED_ENEMY_BULLET_H
#define SHARED_ENEMY_BULLET_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* load the bullets' textures from E_BULTEX, and compile their models for
 * njDirectDrawObject, unless a task holds them already. that task runs at
 * task level 0, and releases the textures when it is destroyed. the heap
 * must have room for the task, unchecked.
 *
 * a call that makes the task leaves the state it compiles under. the
 * bullets' texture list stays current, and lights[8], the light the builds
 * are compiled under, stays applied. the fog stays on, and so does
 * njSetDirectClip's clipping.
 *
 * where the heap has no room for the bullet's compiled model, a bullet draws
 * by njCnkSimpleDrawModel. where it has none for the streak's, a bullet's
 * streaks draw nothing */
void enemy_bullet_load(void);

/* enemy_bullet_fire's kinds. a bullet of ENEMY_BULLET_PASSES flies through the
 * stage. one of any other kind bursts where it meets the stage */
#define ENEMY_BULLET_PASSES 0
#define ENEMY_BULLET_STOPS 1

/* fire a bullet of kind from pos at spd. a puff of dust, sparks and a sound
 * start at pos. the bullet is parent's child, or without a parent a task at
 * task level 3. the call loads the bullets' textures as enemy_bullet_load
 * does. where the heap has no room for the bullet's task, the call does
 * nothing.
 *
 * the bullet flies at spd, points along it, and attacks what it touches. a
 * bullet that has flown more than 20 steps bursts where it is hurt, whatever
 * its kind. a burst draws nothing. the bullet holds still while
 * game_state.time_stopped isn't 0. it ends with its burst, once parent is
 * killed, or once placement_try_leave finds it out of range.
 *
 * the heap must have room for the bullet's collision record and enemy_work,
 * unchecked. spd must not be zero, unchecked: the bullet's pitch divides by
 * spd's length */
void enemy_bullet_fire(struct task *parent, int kind, NJS_VECTOR *spd,
                       NJS_POINT3 *pos);

#endif
