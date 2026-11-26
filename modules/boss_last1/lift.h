/* the lift under the Biolizard's lifted wave, which lifts player 0 and the
 * wave's spheres */
#ifndef BOSS_LAST1_LIFT_H
#define BOSS_LAST1_LIFT_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* create the lift under parent, the Biolizard's task. the lift rises from a
 * height of 0 to 150. unless released, it dies once the Biolizard's mode is
 * neither BIOLIZARD_SPHERE_ATTACK nor BIOLIZARD_PAUSE_ATTACK. returns the
 * lift's task, or null for a null parent or where the heap can't give it.
 * the calls below act on the lift created last, and do nothing without one */
struct task *biolizard_lift_create(struct task *parent);
/* release the lift. it lifts player 0 for its next 9 steps, and speeds the
 * spheres up from now on. the Biolizard's mode no longer kills it: it lives
 * until stopped */
void biolizard_lift_release(void);
/* kill the lift at its next step */
void biolizard_lift_stop(void);
/* move a sphere's height, pos->y, by its speed along y, *speed. the rising
 * lift carries a sphere below it up to its height. a sphere that falls below
 * the lift's height bounces up. *timer then counts down the steps before it
 * can bounce again. once the lift is released, a sphere's speed grows each
 * step instead */
void biolizard_lift_sphere(NJS_POINT3 *pos, Float *speed, Sint16 *timer);
#endif
