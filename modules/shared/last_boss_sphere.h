/* the energy spheres of the two last fights, and the light the fights draw
 * their effects in. each sphere's task keeps a struct last_boss_sphere in
 * extra */
#ifndef SHARED_LAST_BOSS_SPHERE_H
#define SHARED_LAST_BOSS_SPHERE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/light.h"

struct task;

/* a sphere about a joint of the boss. position is the joint's position plus
 * offset, or stays fixed where joint is below 0. the Biolizard's spheres keep
 * clear of it. one that would rise within radius of position moves out along
 * its bearing by the difference */
struct last_boss_sphere_obstacle {
    Sint8 joint;
    NJS_VECTOR offset;
    NJS_POINT3 position;
    Float radius;
};

struct last_boss_sphere {
    Sint8 kind;
    Uint8 slot;
    Sint16 lift_timer;
    Angle pulse;
    Angle swing;
    Angle swing_step;
    Sint32 hold;
    Float player_distance;
    Float camera_distance;
    Float curve_speed;
    Float curve_accel;
    Float curve_time;
    Float fade;
    NJS_VECTOR velocity;
    NJS_VECTOR scale;
    NJS_VECTOR scale_phase;
    Float wobble;
    Uint8 unused_54[20];
    NJS_POINT3 target;
    NJS_POINT3 start;
    NJS_POINT3 first_control;
    NJS_POINT3 second_control;
    NJS_POINT3 center;
};

/* draw a trail that fades from from to to. its task is a child of parent, or a
 * task of its own where parent is null */
void last_boss_sphere_trail(struct task *parent, NJS_POINT3 *from,
                            const NJS_POINT3 *to);
/* the kinds of burst: a short one of one ring, and a long one of two.
 * LAST_BOSS_BURST_KINDS counts them */
enum last_boss_burst_kind {
    LAST_BOSS_BURST_SHORT,
    LAST_BOSS_BURST_LONG,
    LAST_BOSS_BURST_KINDS
};
/* the light each last fight's stage sets for its effects. the spheres' meshes
 * are compiled in it. the spheres' flashes, the guard flare and the
 * Finalhazard's lasers and weak point are drawn in it */
#define LAST_BOSS_EFFECT_LIGHT LIGHT_EFFECT
/* start a flash at position and kind's rings, one after another, as children of
 * parent. return the flash's task, or null where parent is null or the flash
 * can't be made. in the Biolizard's fight, each ring reads *position each step.
 * position must then stay valid until the last ring fades */
struct task *last_boss_sphere_burst(struct task *parent,
                                    const NJS_POINT3 *position, Sint8 kind);
#endif
