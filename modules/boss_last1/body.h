/* the Biolizard, the boss of BOSS_LAST1.PRS, whose body is the task
 * LastBoss1Exec */
#ifndef BOSS_LAST1_BODY_H
#define BOSS_LAST1_BODY_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* the Biolizard's modes, in its work's mode */
enum biolizard_mode {
    BIOLIZARD_APPEAR = 1,
    /* a change to BIOLIZARD_BEGIN_WALK or BIOLIZARD_RESUME_WALK starts
     * BIOLIZARD_WALK instead. the first also sets the work's timer to
     * BIOLIZARD_WALK_STEPS, and the second leaves the timer as it is */
    BIOLIZARD_BEGIN_WALK = 2,
    BIOLIZARD_RESUME_WALK = 3,
    BIOLIZARD_WALK = 4,
    BIOLIZARD_SWEEP = 6, /* an attack on the player near its mouth */
    BIOLIZARD_ENERGY_ATTACK = 7,
    /* a glide that lays a rail along its body for the player */
    BIOLIZARD_OFFER_RAIL = 8,
    BIOLIZARD_SPHERE_ATTACK = 9,
    BIOLIZARD_DAMAGE = 10,
    BIOLIZARD_PAUSE_ATTACK = 11, /* a wait after an attack hurts the player */
    BIOLIZARD_DEFEAT = 12,
    /* the test menu's modes. BIOLIZARD_RESTORE_CAMERA calls
     * biolizard_camera_restore each step, and BIOLIZARD_PREVIEW_MOTION plays
     * each of the Biolizard's motions in turn */
    BIOLIZARD_RESTORE_CAMERA = 13,
    BIOLIZARD_PREVIEW_MOTION = 14
};

/* the steps a walk lasts, in the work's timer, before the next attack */
#define BIOLIZARD_WALK_STEPS 900

/* the Biolizard's attacks, as biolizard_select_attack takes them. the walks
 * run in BIOLIZARD_WALK, and the volleys in BIOLIZARD_ENERGY_ATTACK.
 * BIOLIZARD_ATTACK_RAIL runs in BIOLIZARD_OFFER_RAIL, and the waves in
 * BIOLIZARD_SPHERE_ATTACK */
enum biolizard_attack {
    BIOLIZARD_ATTACK_WALK,
    BIOLIZARD_ATTACK_FAST_WALK,
    BIOLIZARD_ATTACK_RAIL,
    /* volleys of the BIOLIZARD_ENERGY_ kinds, from the slowest */
    BIOLIZARD_ATTACK_SLOW_ENERGY,
    BIOLIZARD_ATTACK_ENERGY,
    BIOLIZARD_ATTACK_FAST_ENERGY,
    BIOLIZARD_ATTACK_WAVE, /* a wave of energy spheres */
    /* the wave with the lift under it, and four rails */
    BIOLIZARD_ATTACK_LIFTED_WAVE
};

/* create the Biolizard at *pos, turned to angle, in BIOLIZARD_APPEAR, with
 * its camera and its life support. player 0 starts 265 from the y axis and
 * 10 up, at the bearing angle + QUARTER_TURN. returns the task.
 *
 * the checked allocations are the task, the Biolizard's own block, its
 * motion record and its pose. where one fails, it returns null. where one
 * but the task fails, the task dies, and its record's parts are lost
 * unfreed. the heap must give the collision record, the camera and the life
 * support, unchecked */
struct task *biolizard_create(const NJS_POINT3 *pos, Angle angle);
/* the Biolizard's task, or null without the Biolizard */
struct task *biolizard_task(void);
/* the life support's task, or null without the Biolizard */
struct task *biolizard_life_support_task(void);
/* 1 while the Biolizard faces the other way round the arena, else 0, or -1
 * without the Biolizard */
Sint8 biolizard_reversed(void);
/* the bearing of the Biolizard's front from the arena's center, or 0
 * without the Biolizard */
Angle biolizard_body_angle(void);
/* biolizard_body_angle + HALF_TURN, or 0 without the Biolizard */
Angle biolizard_tail_angle(void);
/* the Biolizard's current attack, as biolizard_select_attack takes it, or
 * -1 without the Biolizard */
Sint8 biolizard_attack(void);
/* set the Biolizard's attack, and restart its order of attacks. returns the
 * Biolizard's mode for the attack, or -1 for an attack without one or with
 * no Biolizard */
int biolizard_select_attack(Sint8 attack);
/* kill the Biolizard's attack and its rails, where it has them */
void biolizard_clear_attacks(void);
/* a mark that one of the Biolizard's attacks has hurt the player. it holds
 * until the Biolizard clears it. biolizard_player_hurt returns 1 while it
 * holds, else 0 */
Bool biolizard_player_hurt(void);
void biolizard_mark_player_hurt(void);
void biolizard_clear_player_hurt(void);
/* set player 0's speed across the ground to dir's x and z, scaled to a
 * length of speed. its y takes dir's. a null dir scales player 0's own speed
 * to that length, and then sets its y to 1 */
void biolizard_push_player(const NJS_VECTOR *dir, Float speed);
#endif
