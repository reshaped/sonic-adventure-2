/* the Biolizard's energy spheres, which rise from its pool. a wave's task keeps
 * them in eight sectors about the arena, and aims single spheres at the
 * player */
#ifndef BOSS_LAST1_SPHERE_H
#define BOSS_LAST1_SPHERE_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;
struct last_boss_sphere_obstacle;

/* compile the spheres' six models into buffer. they are a shell and a core at
 * each of three levels of detail. it returns buffer past them */
Uint32 biolizard_build_models(Uint32 buffer);
/* make a wave of spheres as a child of parent, the Biolizard's task, and
 * return its task. the wave is lifted where lift is TRUE. every wave finds its
 * spheres in one table of slots, which the call clears. so an earlier wave can
 * no longer reach its spheres. the new wave's spheres spawn clear of count
 * obstacles, which must stay valid while they spawn. the spheres then wait for
 * biolizard_sphere_volley. the call returns null where the heap can't give the
 * task or its record. parent must not be null: for a null parent the call
 * returns no value */
struct task *
biolizard_sphere_wave(struct task *parent, Sint32 lift,
                      const struct last_boss_sphere_obstacle *obstacles,
                      Sint16 count);
/* start the wave's volley, which aims its first spheres 180 steps on at the
 * soonest. once no more than half the wave's spheres remain, the volley ends
 * the wave. a lifted wave's spheres then lift away, and any other's fade */
void biolizard_sphere_volley(struct task *task);
/* burst the wave's spheres, two a step, and end the wave. this and
 * biolizard_sphere_volley do nothing for a null task */
void biolizard_sphere_clear(struct task *task);
/* 1 once the wave has ended, else 0. at the end the call kills the wave's
 * task, and stops a lifted wave's lift. task must not be null. a test returns
 * 1 for null, but the call reads through task before it */
int biolizard_sphere_finished(struct task *task);
/* step the sphere tests' menu, which prints the wave's settings, or the
 * lifted wave's where lift is 1. player 0's Y steps through them, and L or R
 * held lowers or raises the one picked */
void biolizard_debug_spheres(int lift);
/* make the wave the menu last showed, as biolizard_sphere_wave does, and
 * select its attack. a lifted wave takes no obstacles */
struct task *
biolizard_preview_spheres(struct task *parent,
                          const struct last_boss_sphere_obstacle *obstacles,
                          Sint16 count);
#endif
