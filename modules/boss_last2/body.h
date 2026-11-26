/* the Finalhazard, the boss of BOSS_LAST2.PRS, whose body is the task
 * LastBoss2Exec */
#ifndef BOSS_LAST2_BODY_H
#define BOSS_LAST2_BODY_H
#include <sg_xpt.h>
struct task;

/* a pointer may be null only where a comment says so */

/* the Finalhazard's modes, in its work's mode. FINALHAZARD_EXPOSE moves the
 * weak point and waits before the next attack. the attacks are the three
 * waves of spheres, the three of lasers and the two of both. a hit leaves the
 * Finalhazard in FINALHAZARD_WAIT, until the other player's turn begins with
 * FINALHAZARD_EXPOSE */
enum finalhazard_mode {
    FINALHAZARD_INTRO,
    FINALHAZARD_WAIT,
    FINALHAZARD_EXPOSE,
    FINALHAZARD_HIT,
    FINALHAZARD_UNUSED, /* nothing sets it, and no step runs in it */
    FINALHAZARD_SPHERES_FEW,
    FINALHAZARD_SPHERES_NORMAL,
    FINALHAZARD_SPHERES_MANY,
    FINALHAZARD_PULSE, /* one pulsing laser */
    FINALHAZARD_SWEEP, /* one sweeping laser */
    FINALHAZARD_DOUBLE_SWEEP, /* two sweeping lasers */
    /* FINALHAZARD_DOUBLE_SWEEP and FINALHAZARD_SPHERES_MANY together */
    FINALHAZARD_BOTH,
    /* FINALHAZARD_BOTH with a wave that grows again, until a hit. the weak
     * point moves every 600 steps */
    FINALHAZARD_BOTH_REPEAT,
    FINALHAZARD_DEFEAT
};

/* create the Finalhazard at finalhazard_center, in FINALHAZARD_INTRO, with
 * its reentry ring. finalhazard_players must not be null. its weak point
 * starts on the active player's side. it pushes camera 0's BOSSPOINT mode,
 * whose target lives in the Finalhazard's record. returns the task. it
 * returns null where the heap can't give the task, its own block or its
 * motion record.
 * the heap must give the collision record, unchecked */
struct task *finalhazard_create(void);
/* the Finalhazard's task, or null without the Finalhazard */
struct task *finalhazard_task(void);
/* the weak point's task, or null without the Finalhazard or its weak point */
struct task *finalhazard_weak_point(void);
/* 1 from a hit on the weak point until the hit motion ends, and through the
 * defeat. else 0, or -1 without the Finalhazard. unused isn't read */
Sint8 finalhazard_hurt(const void *unused);
/* finalhazard_hurt, under a second name declared without the argument, as
 * laser.c calls it */
Sint8 finalhazard_laser_hurt(void);
/* 1 while the Finalhazard is stunned, else 0, or -1 without it. a stunned
 * Finalhazard counts its stun down in place of its mode's step */
int finalhazard_stunned(void);
/* @hack type: finalhazard_stunned's answer narrowed to Sint8, where the int
 * alone would do */
#define FINALHAZARD_STUNNED() ((Sint8)finalhazard_stunned())
/* stun the Finalhazard for FINALHAZARD_STUN_STEPS steps, and return them. it
 * returns -1 and stuns nothing without the Finalhazard, or while it is hurt */
int finalhazard_stun(void);
#define FINALHAZARD_STUN_STEPS 180
/* kill the wave of spheres and the lasers of task, the Finalhazard's, where
 * it has them */
void finalhazard_clear_attacks(struct task *task);
/* have task's next step, where it has a weak point, call
 * finalhazard_camera_blend. no code calls it */
void finalhazard_blend_next_frame(struct task *task);
/* the Finalhazard's draw, which also sets finalhazard_joint_positions and
 * finalhazard_panel_bases */
void finalhazard_draw(struct task *task);
#endif
