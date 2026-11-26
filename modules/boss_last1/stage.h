/* the Biolizard's stage, the program of BOSS_LAST1.PRS. the overlay's head
 * names its init, its exit and its task's step */
#ifndef BOSS_LAST1_STAGE_H
#define BOSS_LAST1_STAGE_H
#include <ninja.h>
#include "object/set.h"
#include "sound/lists.h"

struct task;

/* the lights the stage sets for the Biolizard and its energy shots, and for
 * its life support */
#define BIOLIZARD_LIGHT 8
#define BIOLIZARD_LIFE_SUPPORT_LIGHT 6

/* the ids of biolizard_sounds that the Biolizard's sources play, by where
 * they play. the spheres' sounds, 10 to 12, are
 * shared/last_boss_sphere_motion.h's */
enum biolizard_sound {
    BIOLIZARD_SOUND_DAMAGE = 0, /* as BIOLIZARD_DAMAGE begins */
    BIOLIZARD_SOUND_DEFEAT = 1, /* again and again through BIOLIZARD_DEFEAT */
    BIOLIZARD_SOUND_START = 3, /* as the appearance ends and the fight starts */
    /* the two sounds of a leg's splash */
    BIOLIZARD_SOUND_SPLASH_A = 4,
    BIOLIZARD_SOUND_SPLASH_B = 5,
    BIOLIZARD_SOUND_SWING = 6, /* the tail's swing */
    BIOLIZARD_SOUND_ENERGY = 7, /* an energy shot's, while it lives */
    BIOLIZARD_SOUND_RELEASE = 8, /* as a volley's shots are released */
    BIOLIZARD_SOUND_PUSH = 9, /* as the body pushes the player */
    /* with each shake of the camera: at a wave's start, BIOLIZARD_DAMAGE's and
     * BIOLIZARD_DEFEAT's */
    BIOLIZARD_SOUND_SHAKE = 13,
    BIOLIZARD_SOUND_GLIDE = 14, /* through BIOLIZARD_OFFER_RAIL */
    BIOLIZARD_SOUND_IMPACT = 15, /* at the two impacts of the defeat's motion */
    /* late in the appearance, and at a wave's start */
    BIOLIZARD_SOUND_UNK_16 = 16,
    /* through BIOLIZARD_DAMAGE, and three times early in the defeat's
     * motion */
    BIOLIZARD_SOUND_HURT = 17,
    BIOLIZARD_SOUND_SWEEP = 18, /* as BIOLIZARD_SWEEP begins */
    BIOLIZARD_SOUND_UNK_19 = 19 /* now and then through a walk or the glide */
};

extern const NJS_POINT3 biolizard_arena_center;
/* the stage's sounds, which last1_stage_init loads */
extern const struct sound_list biolizard_sounds;
/* the stage's object list, which last1_stage_init gives to set_start */
extern struct object_list last1_object_list;

/* start the stage, and compile the land's and the spheres' models. it gives
 * player_hooks.drop_rings, .splash and .ripple the stage's calls where each
 * is null, and sets stage_hooks. it tests no call's result */
void last1_stage_init(void);
/* end the stage, and release what last1_stage_init took. it clears each of
 * player_hooks' calls that still holds the stage's */
void last1_stage_exit(void);
/* the stage task's step. the first step creates the Biolizard at
 * biolizard_arena_center. the task's later steps hold player 0 inside the
 * arena, and its draw draws the water */
void last1_stage_step(struct task *task);
/* draw biolizard_water_object at the origin. the draw first sets the
 * model's texture, in its own polygon list, to biolizard_water_texlist's
 * texture game_state.steps mod 16 */
void biolizard_water_draw(void);
#endif
