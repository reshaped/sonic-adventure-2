/* the mech bosses' sounds: the bank that Big Foot, Hot Shot and Flying Dog
 * load */
#ifndef SHARED_MECH_BOSS_SOUNDS_H
#define SHARED_MECH_BOSS_SOUNDS_H

/* the ids of the list's sounds that the mech bosses' objects play */
enum mech_boss_sound {
    MECH_BOSS_SOUND_HIT = 0,
    MECH_BOSS_SOUND_SPARKS = 1,
    MECH_BOSS_SOUND_TITLE_LETTER = 2,
    MECH_BOSS_SOUND_TITLE_BANNER = 3,
    MECH_BOSS_SOUND_HOVER = 4,
    MECH_BOSS_SOUND_ENGINE = 6,
    MECH_BOSS_SOUND_LAND = 7,
    MECH_BOSS_SOUND_STEP = 8,
    MECH_BOSS_SOUND_LASER_FIRE = 9,
    MECH_BOSS_SOUND_LASER_BEAM = 10,
    MECH_BOSS_SOUND_HATCH = 11,
    MECH_BOSS_SOUND_BEAM_CHARGE = 12,
    MECH_BOSS_SOUND_BEAM_SHOT = 13,
    MECH_BOSS_SOUND_BEAM_BOMB = 14, /* the beam's bomb and torus play it */
    MECH_BOSS_SOUND_CRASH = 15,
    MECH_BOSS_SOUND_COLLAPSE = 16,
    /* the sound Hot Shot and Flying Dog repeat in most of their opening, and
     * with one hit left */
    MECH_BOSS_SOUND_UNK_19 = 19
};

/* the priority of a sound that a boss or one of its effects keys by its own
 * task */
#define MECH_BOSS_SOUND_PRIORITY 1
/* the duration each step plays the hover's and the engine's sounds for */
#define MECH_BOSS_ENGINE_SOUND_STEPS 5
/* the most the engine's pitch bends, up or down */
#define MECH_BOSS_ENGINE_BEND 8192
/* the volumes of the hit, the hatch and the crash, and the crash's duration */
#define MECH_BOSS_HIT_VOLUME 120
#define MECH_BOSS_HATCH_VOLUME 10
#define MECH_BOSS_CRASH_VOLUME 120
#define MECH_BOSS_CRASH_SOUND_STEPS 120
/* while a boss is beaten, the sparks' sound plays every MECH_BOSS_SPARKS_STEPS.
 * each time, it takes a pitch, a volume and a pan at random, each up to its
 * _SPREAD above its base */
#define MECH_BOSS_SPARKS_STEPS 35
#define MECH_BOSS_SPARKS_PITCH (-1536.0F)
#define MECH_BOSS_SPARKS_PITCH_SPREAD 4608.0F
#define MECH_BOSS_SPARKS_VOLUME 60.0F
#define MECH_BOSS_SPARKS_VOLUME_SPREAD 60.0F
#define MECH_BOSS_SPARKS_PAN (-32.0F)
#define MECH_BOSS_SPARKS_PAN_SPREAD 64.0F
/* a second key for a boss's sounds: the address one byte past its task. a
 * sound keyed by it plays apart from one keyed by the task */
#define MECH_BOSS_SECOND_KEY(task) ((char *)(task) + 1)

/* load se_ac_bf.mlt and the list of its 20 sounds, as sound_load_stage loads
 * a stage's. sound ids 0 to 19 then name the list's sounds */
void mech_boss_sounds_load(void);

#endif
