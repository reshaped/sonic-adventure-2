/* King Boom Boo's motions, collision shapes, flight tunings and sound
 * periods */
#ifndef BOSS_BIGBOGY_SETTINGS_H
#define BOSS_BIGBOGY_SETTINGS_H
#include <ninja.h>
#include "collision/collision.h"
#include "draw/motion.h"
#include "shared/boss_title.h"

struct bigbogy_tuning {
    Float gravity;
    Float acceleration;
    Float lift;
    Float lift_variation;
    /* the boss's speed, as a multiple of player 0's top speed */
    Float speed_scale;
};
/* bigbogy_tuning's rows, and the four the fight reads by index. the breath
 * takes BIGBOGY_BREATH_TUNING's gravity. the throw takes BIGBOGY_THROW_TUNING.
 * while the wind-up motion plays past its link, it takes
 * BIGBOGY_THROW_LIFT_TUNING's lift. as each throw starts it takes
 * BIGBOGY_THROWN_TUNING */
#define BIGBOGY_TUNINGS 9
#define BIGBOGY_BREATH_TUNING 0
#define BIGBOGY_THROWN_TUNING 2
#define BIGBOGY_THROW_TUNING 3
#define BIGBOGY_THROW_LIFT_TUNING 4

extern struct boss_title bigbogy_title;

struct bigbogy_sound_period {
    Sint16 steps;
    Uint8 unused_02[2];
};

extern const struct motion_table_entry bigbogy_motions[24];
extern const struct collision_shape bigbogy_shapes[13];
extern struct bigbogy_tuning bigbogy_tuning[BIGBOGY_TUNINGS];
extern Float bigbogy_unused_8c585ed8, bigbogy_wall_speed;
extern struct bigbogy_sound_period shadow_sound_period, digging_sound_period;
extern struct bigbogy_sound_period fleeing_sound_period, grounded_sound_period;
/* five rows of bigbogy_tuning under second names, which symbols.txt places at
 * the rows */
#ifdef NONMATCHING
/* the rows by their own spelling: this side's objects don't define the second
 * names, which only symbols.txt places */
#define circle_tuning bigbogy_tuning[1]
#define dash_tuning bigbogy_tuning[5]
#define opening_tuning bigbogy_tuning[6]
#define bite_tuning (&bigbogy_tuning[7])
#define hit_tuning bigbogy_tuning[8]
#else
/* @hack spelling: the five rows read through second names, where
 * bigbogy_tuning[n] would do */
extern struct bigbogy_tuning circle_tuning, dash_tuning, opening_tuning;
extern struct bigbogy_tuning bite_tuning[], hit_tuning;
#endif
#endif
