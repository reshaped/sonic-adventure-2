/* the laser an enemy fires. it charges as a ring closes in, then stretches
 * out as a beam that flies and bursts */
#ifndef SHARED_ENEMY_LASER_H
#define SHARED_ENEMY_LASER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* in the work flags of a laser's parent. a charging laser sets it at each
 * step, and leaves clearing it to the parent */
#define ENEMY_LASER_CHARGING 0x800

/* load the lasers' texture from E_LASTEX, unless a task holds it already.
 * that task runs at task level 0, and releases the texture when it is
 * destroyed. the heap must have room for the task, unchecked */
void enemy_laser_load(void);

/* fire a laser of length from pos along dir, and play the charge's sound at
 * pos. the laser is parent's child where parent isn't null, and else a task
 * at task level 3. the call loads the texture as enemy_laser_load does.
 * where the heap has no room for the laser's task, the call does nothing.
 *
 * pos and dir must not be null, unchecked. dir must not be zero, unchecked:
 * the call divides by dir's length. length must not be negative, unchecked:
 * the beam then flies at once, with a negative length. a parent must have a
 * work, unchecked. the heap must have room for the laser's collision record
 * and enemy_work. where it has none, the call writes through null.
 *
 * the laser charges at pos for 50 steps, as a ring closes in. it then plays
 * the beam's sound. the beam grows from its front end by dir a step until it
 * is length long. it then flies by dir a step. it bursts where it meets the
 * stage or touches another record. it also bursts where its work's flags
 * hold WORK_HURT, WORK_UNK_1 or WORK_UNK_8. the beam hurts what it touches
 * until it bursts.
 *
 * the laser stops drawing as it bursts, and its task ends about 40 steps
 * later. the task also ends with parent, or once placement_try_leave finds
 * it out of range. while game_state.time_stopped is set, the laser holds
 * still */
void enemy_laser_fire(struct task *parent, Float length, NJS_VECTOR *dir,
                      NJS_POINT3 *pos);
/* aim parent's first child that is a charging laser. where pos isn't null,
 * the laser moves to pos. where dir isn't null, the laser takes dir as its
 * speed, and turns along dir. dir must not be zero, unchecked. the call
 * does nothing where parent is null or has no charging laser */
void enemy_laser_aim(struct task *parent, NJS_VECTOR *dir, NJS_POINT3 *pos);
/* fire a laser as enemy_laser_fire does, but without the charge or its
 * sound. the beam starts to grow at once, and the call plays the beam's
 * sound at pos. the laser is armed once it has taken more than 20 steps
 * outside a time stop. only an armed laser bursts at a touch or at those
 * work flags. the laser never sets ENEMY_LASER_CHARGING, so a parent needs no
 * work */
void enemy_laser_fire_charged(struct task *parent, Float length,
                              NJS_VECTOR *dir, NJS_POINT3 *pos);

#endif
