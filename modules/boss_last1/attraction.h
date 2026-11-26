/* an emitter of particles that home on a point and fade there */
#ifndef BOSS_LAST1_ATTRACTION_H
#define BOSS_LAST1_ATTRACTION_H
#include <sg_xpt.h>
#include <ninja.h>
struct task;
/* an emitter's settings. a particle starts up to spread from target on each
 * axis, at rest. its speed grows by the same amount each step. that amount is
 * drawn as the particle starts, from acceleration up to acceleration +
 * acceleration_range. it fades within radius of target, or once it has homed
 * 181 steps */
struct attraction {
    NJS_POINT3 *target; /* the point the particles home on, read each step */
    Float radius;
    Float spread;
    Float scale; /* the particles' sprite scale */
    Float rate; /* particles per step, a fraction of one carried over */
    Float acceleration;
    Float acceleration_range;
    NJS_TEXLIST *textures;
    Sint16 texture; /* the particles' texture in textures */
};
/* start an emitter under parent with a copy of settings. target and textures
 * must outlive it, and textures mustn't be null. returns the emitter's task,
 * or null where the heap can't give it or its settings' copy. parent must
 * not be null: for a null parent the result is undefined */
struct task *biolizard_attraction_create(struct task *parent,
                                         const struct attraction *settings);
#endif
