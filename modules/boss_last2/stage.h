/* the Finalhazard's stage, the program of BOSS_LAST2.PRS. the overlay's head
 * names its init, its exit and its task's step */
#ifndef BOSS_LAST2_STAGE_H
#define BOSS_LAST2_STAGE_H
#include "draw/light.h"
#include "sound/lists.h"
#include "object/set.h"

struct task;
/* the light the stage sets for the Finalhazard's body. its lasers, weak
 * point and spheres take shared/last_boss_sphere.h's LAST_BOSS_EFFECT_LIGHT */
#define FINALHAZARD_LIGHT 8

/* the lights last2_stage_init copies into the game's lights */
extern struct light finalhazard_lights[4];
/* last2_stage_step creates the Finalhazard where this is nonzero. it is 1,
 * and no code changes it */
extern Sint32 finalhazard_enabled;
/* null, as no code sets it. last2_stage_exit frees it where it isn't */
extern struct set_head *finalhazard_set;
/* the stage's sounds, which last2_stage_init loads */
extern const struct sound_list finalhazard_sounds;
/* the sounds the fight names, by their ids in finalhazard_sounds. the
 * spheres' ids, 10 to 12, are shared/last_boss_sphere_motion.h's */
enum finalhazard_sound {
    SOUND_WEAK_POINT_HIT = 0,
    SOUND_DEFEAT_LOOP = 1,
    SOUND_START = 3, /* as the fight starts */
    SOUND_SUPER_BURST = 4, /* as a super form starts an attacking mode */
    SOUND_WEAK_POINT_HUM = 5,
    SOUND_LASER_CHARGE = 6,
    SOUND_LASER_HUM = 7,
    SOUND_LASER_BEAM = 8,
    SOUND_CONTACT = 9, /* as the player touches the body */
    SOUND_REENTRY = 13,
    SOUND_LATE = 14, /* as the clock passes 4:00 */
    SOUND_INTRO = 15,
    SOUND_HIT = 16, /* in the hit's motion and the defeat's */
    SOUND_BREATH = 17,
    SOUND_DEFEAT = 18
};
/* a breath's pitch is random, up to FINALHAZARD_BREATH_PITCH either way */
#define FINALHAZARD_BREATH_PITCH 16383.0F
/* task's sound key number n, from 1. a second play of an id under such a
 * key takes a slot of its own */
#define FINALHAZARD_SOUND_KEY(task, n) ((char *)(task) + (n))

/* start the stage, and compile the spheres' models. it destroys the players'
 * tasks, and makes player 0 Super Sonic and player 1 Super Shadow. it gives
 * player_hooks.drop_rings ring_scatter where that is null, and sets
 * stage_hooks. it starts the stars about player 0's camera where they aren't
 * running. it tests no call's result */
void last2_stage_init(void);
/* end the stage, and release what last2_stage_init took. it clears
 * player_hooks.drop_rings where it still holds ring_scatter, and destroys the
 * players' tasks */
void last2_stage_exit(void);
/* the stage task's step. the first step creates the Finalhazard where
 * finalhazard_enabled is nonzero. the task's later steps and its draw are the
 * backdrop's */
void last2_stage_step(struct task *task);
#endif
