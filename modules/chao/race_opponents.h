/* the seven opponents in a garden race outside multiplayer. the course's
 * field names a template for each */
#ifndef CHAO_RACE_OPPONENTS_H
#define CHAO_RACE_OPPONENTS_H

#include <sg_xpt.h>
#include "chao/life.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/residents.h"

#define RACE_OPPONENTS (RACE_ENTRANTS - 1)
extern struct chao_save race_opponents[RACE_OPPONENTS];

/* a template draws a range's value from base - spread up to base + spread */
struct stat_range {
    Sint16 base;
    Uint16 spread;
};
struct shape_range {
    Float base, spread;
};

/* codes of race_template.type that draw an adult's form, or make a child
 * with another body. any other code is the type itself */
enum race_template_type {
    TEMPLATE_ANY_TYPE = 3, /* a child one time in five, else any adult */
    TEMPLATE_NEUTRAL_ADULT = 4,
    TEMPLATE_MINIMAL_CHILD = 23, /* in a minimal's body, of kind 0 to 8 */
    TEMPLATE_MODEL_2_CHILD = 24,
    TEMPLATE_MODEL_1_CHILD = 25,
    TEMPLATE_ALTERNATE_CHILD = 26,
    TEMPLATE_HERO_ADULT = 27,
    TEMPLATE_DARK_ADULT = 28
};

/* codes of race_template.eye and race_template.mouth that draw from a pool.
 * any other code is the eye or the mouth itself */
enum race_template_face {
    TEMPLATE_EYE_POOL_A = 11,
    TEMPLATE_EYE_POOL_B = 12,
    TEMPLATE_EYE_POOL_C = 13,
    TEMPLATE_MOUTH_POOL_A = 13,
    TEMPLATE_MOUTH_POOL_B = 14,
    TEMPLATE_MOUTH_POOL_C = 15
};

/* codes of race_template.animal_parts, and of race_template.shiny, which
 * draws parts again over them. TEMPLATE_ANY_PARTS draws from the first 12
 * animal kinds, or from 15 as a shiny code. a code named for a kind draws
 * from it and the two after it. TEMPLATE_DRA_SLOT puts ANIMAL_DRA in slot
 * TEMPLATE_PART_SLOT. any other shiny code draws nothing */
enum race_template_parts {
    TEMPLATE_NO_PARTS,
    TEMPLATE_ANY_PARTS,
    TEMPLATE_PEN_PARTS,
    TEMPLATE_KUJA_PARTS,
    TEMPLATE_USA_PARTS,
    TEMPLATE_KUMA_PARTS,
    TEMPLATE_DRA_PARTS,
    TEMPLATE_DRA_SLOT
};
#define TEMPLATE_PART_SLOT 1

/* the names a template takes from opponent_names_jp and opponent_names_en:
 * the first 7 of their 10 */
#define NAMES_TAKEN 7

/* a race opponent before its draws. name indexes opponent_names_jp and
 * opponent_names_en, and a name from NAMES_TAKEN up gives none. stat_mode
 * 0 and 2 give each skill stat_total / CHAO_SKILLS and a draw below 500.
 * stat_mode 1 draws each skill from its stats range */
struct race_template {
    Uint8 name, type, eye, mouth, emotion_ball, unk_05;
    Uint8 hat, costume, medal, color, texture, shiny, animal_parts;
    struct shape_range run_power, fly_swim, alignment, evolution;
    Uint16 stat_mode;
    Uint32 stat_total;
    struct stat_range stats[CHAO_SKILLS];
};

/* the opponents of one course and level, each an index into
 * opponent_templates. garden_race_build_opponents shuffles their order
 * unless fixed is set. featured indexes entrants: the opponent the
 * entrance's panel shows */
struct race_field {
    Bool fixed;
    Sint32 featured;
    Sint32 entrants[RACE_OPPONENTS];
};

extern char opponent_names_jp[][8], opponent_names_en[][8];
extern struct race_template opponent_templates[];
extern struct race_field beginner_fields[][RACE_BEGINNER_LEVELS];
extern struct race_field jewel_fields[][RACE_JEWEL_LEVELS];
extern struct race_field challenge_fields[];
extern struct race_field dark_fields[];
extern struct race_field hero_fields[];
/* fill destination from source, drawing what source leaves to chance */
void garden_race_make_opponent(struct chao_save *destination,
                               const struct race_template *source);

/* make race_opponents from the selected race's field, in its order or
 * shuffled. RACE_MULTI makes none */
void garden_race_build_opponents(void);

#endif
