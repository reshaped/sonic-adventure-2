/* the Biolizard's camera. a point camera stands behind player 0, on the far
 * side from a target. a task moves the target among the points the
 * Biolizard gives it, as the Biolizard's mode changes */
#ifndef BOSS_LAST1_CAMERA_H
#define BOSS_LAST1_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* a point that moves with the Biolizard: *base plus offset */
struct biolizard_camera_point {
    const NJS_POINT3 *base;
    NJS_VECTOR offset;
};

/* the points biolizard_camera_create takes, by index. the camera's motions
 * play about BIOLIZARD_CAMERA_ORIGIN */
enum biolizard_camera_point_index {
    BIOLIZARD_CAMERA_CENTER,
    BIOLIZARD_CAMERA_HEAD,
    BIOLIZARD_CAMERA_TAIL,
    BIOLIZARD_CAMERA_ORIGIN,
    BIOLIZARD_CAMERA_LIFE_SUPPORT,
    BIOLIZARD_CAMERA_FRONT,
    BIOLIZARD_CAMERA_POINTS
};

/* create the camera under parent. each step reads *mode, the Biolizard's
 * mode, and moves *target among the BIOLIZARD_CAMERA_POINTS of points. mode
 * and points must outlive the camera. target must outlive the point camera,
 * which reads it.
 *
 * the point camera is a task of its own, which a call creates where none
 * lives. it stays when the camera's task dies.
 *
 * returns the camera's task, which the calls below take, or null where the
 * heap can't give the task. parent must not be null: for a null parent the
 * result is undefined. so is the result where the heap can't give the
 * task's motion record */
struct task *
biolizard_camera_create(struct task *parent, const Sint8 *mode,
                        NJS_POINT3 *target,
                        const struct biolizard_camera_point *points);
/* have the target follow BIOLIZARD_CAMERA_HEAD or BIOLIZARD_CAMERA_TAIL,
 * which side names. any other side changes nothing */
void biolizard_camera_side(struct task *task, Sint8 side);
/* biolizard_camera_side under a second name, which body.c calls with an int
 * side */
#ifdef NONMATCHING
/* biolizard_camera_side by its own name: this side's objects don't define the
 * second name, which only symbols.txt places */
#define biolizard_select_camera_side biolizard_camera_side
#else
void biolizard_select_camera_side(struct task *task, int side);
#endif
/* play the camera's motion for a charge, or for the Biolizard's defeat */
void biolizard_camera_charge(struct task *task);
void biolizard_camera_defeat(struct task *task);
/* frame attack's wave of spheres. for BIOLIZARD_ATTACK_LIFTED_WAVE, frame
 * the life support instead */
void biolizard_camera_volley(struct task *task, Sint8 attack);
/* shake camera 0 by size for steps */
void biolizard_camera_shake(Float size, Uint16 steps);
/* take the mode on top of camera 0's stack off */
void biolizard_camera_restore(void);
#endif
