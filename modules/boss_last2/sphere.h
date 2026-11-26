/* the Finalhazard's energy spheres. a wave sends its spheres out in rows, and a
 * volley fires them from where they rest. a repeating wave grows a new set */
#ifndef BOSS_LAST2_SPHERE_H
#define BOSS_LAST2_SPHERE_H
#include <sg_xpt.h>
struct task;
struct last_boss_sphere_obstacle;

enum finalhazard_density { WAVE_FEW, WAVE_NORMAL, WAVE_MANY };

/* compile the spheres' two models into buffer, a near one and a far one. it
 * returns buffer past them */
Uint32 finalhazard_build_shots(Uint32 buffer);
/* make a wave of spheres as a child of parent, the Finalhazard's task, and
 * return its task. density picks how many spheres it spawns. every wave finds
 * its spheres in one table of slots, which the call clears. so an earlier wave
 * can no longer reach its spheres. the new wave keeps obstacles and count,
 * which nothing reads. its spheres wait for finalhazard_sphere_volley. the
 * wave kills its own task once a switch of players begins. the call returns
 * null where the heap can't give the task or its record. parent must not be
 * null: for a null parent the call returns no value */
struct task *
finalhazard_sphere_wave(struct task *parent,
                        const struct last_boss_sphere_obstacle *obstacles,
                        Sint16 count, Sint8 density);
/* start the wave's volley, which aims its first spheres 30 steps on. once its
 * spheres are gone, the wave ends */
void finalhazard_sphere_volley(struct task *task);
/* burst the wave's spheres, two a step, and end the wave. no code calls it.
 * this and finalhazard_sphere_volley do nothing for a null task */
void finalhazard_sphere_clear(struct task *task);
/* 1 once the wave has ended, or for a null task, else 0. at the end the call
 * kills the wave's task */
int finalhazard_sphere_finished(struct task *task);
/* make the wave repeat. 120 steps after its spheres are gone, it spawns
 * again. 60 steps after that, it starts its volley by itself. the call
 * returns 1, or -1 for a null task, which it leaves alone. no caller reads the
 * result */
int finalhazard_sphere_repeat(struct task *task);
#endif
