#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <string.h>

#include "chao/life.h"
#include "chao/race.h"
#include "chao/race_opponents.h"
#include "chao/residents.h"
#include "chao/work.h"
#include "object/animal.h"
#include "system/game_state.h"
#include "system/text.h"
#include "system/util.h"

#define SWAPS 32
/* the tops of two draws of a type. ADULT_TYPE_DRAW draws any of the 15
 * adult types, and ADULT_FORM_DRAW one of the five adult forms */
#define ADULT_TYPE_DRAW 14.99F
#define ADULT_FORM_DRAW 4.99F
/* the top of the draw of a minimal's kind, 0 to 8 */
#define MINIMAL_DRAW 8.99F
/* the top of the draw of a skill above stat_total / CHAO_SKILLS */
#define STAT_DRAW 500.0F
/* the top of the draw of one of the RACE_OPPONENTS places to swap */
#define PLACE_DRAW 6.9999F

#ifdef NONMATCHING
/* this side keeps the result of the bug tagged at the eye's draw in
 * garden_race_make_opponent. its fifth entry holds the 0 that the image reads
 * from opponent_eyes_b */
static Sint16 opponent_eyes_a[] = {0, 4, 9, 10, 0};
#else
static Sint16 opponent_eyes_a[] = {0, 4, 9, 10};
#endif
static Sint16 opponent_eyes_b[] = {0, 4};
static Sint16 opponent_eyes_c[] = {0, 10};
static Sint16 opponent_mouths_a[] = {0, 1, 3, 4, 7, 8, 9, 10, 12};
static Sint16 opponent_mouths_b[] = {0, 1, 3, 7, 12};
static Sint16 opponent_mouths_c[] = {0, 1, 4, 9, 10};

/* a random Float at least 0 and below limit */
#define DRAW(limit) (rand() * ((limit) * RAND_SCALE))
/* a random Float from range's base - spread up to base + spread */
#define SCATTER(range) ((range).base + ((range).spread * (DRAW(2.0F) - 1.0F)))

void garden_race_make_opponent(struct chao_save *destination,
                               const struct race_template *source)
{
    /* @hack spelling: model, a copy of source taken before parameters, where
     * source would do */
    const struct race_template *model = source;
    struct chao_parameters *parameters = &destination->record.saved.parameters;
    int slot;
    int last;
    Uint32 base;
    const struct stat_range *cursor;
    const struct stat_range *range;

    memset(destination, 0, sizeof(*destination));
    if (model->name < NAMES_TAKEN) {
        if (game_state.text_language == TEXT_JAPANESE) {
            njMemCopy(parameters->name, opponent_names_jp[model->name],
                      sizeof(parameters->name));
        } else {
            njMemCopy(parameters->name, opponent_names_en[model->name],
                      sizeof(parameters->name));
        }
    } else {
        /* @hack redundant: an empty switch on names 7 to 9 */
        switch (model->name) {
        case 7:
        case 8:
        case 9:
            break;
        }
    }

    switch (model->type) {
    case TEMPLATE_ANY_TYPE:
        if (DRAW(1.0F) < 0.2F) {
            parameters->type = CHAO_CHILD;
        } else {
            parameters->type = (int)DRAW(ADULT_TYPE_DRAW) + CHAO_NORMAL_ADULT;
        }
        break;
    case TEMPLATE_NEUTRAL_ADULT:
        parameters->type = ((int)DRAW(ADULT_FORM_DRAW) * CHAO_FORM_ALIGNMENTS) +
                           CHAO_NORMAL_ADULT;
        break;
    case TEMPLATE_HERO_ADULT:
        parameters->type = ((int)DRAW(ADULT_FORM_DRAW) * CHAO_FORM_ALIGNMENTS) +
                           CHAO_FIRST_HERO_TYPE;
        break;
    case TEMPLATE_DARK_ADULT:
        parameters->type = ((int)DRAW(ADULT_FORM_DRAW) * CHAO_FORM_ALIGNMENTS) +
                           CHAO_FIRST_DARK_TYPE;
        break;
    case TEMPLATE_MINIMAL_CHILD:
        parameters->body = CHAO_BODY_MINIMAL;
        parameters->minimal = (int)DRAW(MINIMAL_DRAW);
        parameters->type = CHAO_CHILD;
        break;
    case TEMPLATE_MODEL_2_CHILD:
        parameters->body = CHAO_BODY_MODEL_2;
        parameters->type = CHAO_CHILD;
        break;
    case TEMPLATE_MODEL_1_CHILD:
        parameters->body = CHAO_BODY_MODEL_1;
        parameters->type = CHAO_CHILD;
        break;
    case TEMPLATE_ALTERNATE_CHILD:
        parameters->body = CHAO_BODY_ALTERNATE;
        parameters->type = CHAO_CHILD;
        break;
    /* @hack redundant: the Chaos types' own cases, where the default would
     * do */
    case CHAO_CHAOS:
        parameters->type = CHAO_CHAOS;
        break;
    case CHAO_HERO_CHAOS:
        parameters->type = CHAO_HERO_CHAOS;
        break;
    case CHAO_DARK_CHAOS:
        parameters->type = CHAO_DARK_CHAOS;
        break;
    default:
        parameters->type = model->type;
        break;
    }

    /* @hack type: each pool's index converted to Uint16, where int would do */
    switch (model->eye) {
    case TEMPLATE_EYE_POOL_A:
        /* @bug memory: can read opponent_eyes_a[4], past its four entries,
         * at 0x0c516974. the image finds opponent_eyes_b's first entry, 0,
         * there */
        parameters->eye = opponent_eyes_a[(Uint16)DRAW(4.999F)];
        break;
    case TEMPLATE_EYE_POOL_B:
        parameters->eye = opponent_eyes_b[(Uint16)DRAW(1.999F)];
        break;
    case TEMPLATE_EYE_POOL_C:
        parameters->eye = opponent_eyes_c[(Uint16)DRAW(1.999F)];
        break;
    default:
        parameters->eye = model->eye;
        break;
    }
    switch (model->mouth) {
    case TEMPLATE_MOUTH_POOL_A:
        parameters->mouth = opponent_mouths_a[(Uint16)DRAW(8.999F)];
        break;
    case TEMPLATE_MOUTH_POOL_B:
        parameters->mouth = opponent_mouths_b[(Uint16)DRAW(4.999F)];
        break;
    case TEMPLATE_MOUTH_POOL_C:
        parameters->mouth = opponent_mouths_c[(Uint16)DRAW(4.999F)];
        break;
    default:
        parameters->mouth = model->mouth;
        break;
    }
    parameters->emotion_ball = model->emotion_ball;
    parameters->hat = model->hat;
    parameters->costume = model->costume;
    parameters->medal = model->medal;
    parameters->color = model->color;
    parameters->texture = model->texture;
    parameters->shiny = model->shiny;

#define SELECT_PARTS(limit, first)                                             \
    for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {                           \
        if (DRAW(1.0F) < 0.6F) {                                               \
            parameters->animal_parts[slot] = (int)DRAW(limit) + (first);       \
        } else {                                                               \
            parameters->animal_parts[slot] = CHAO_NO_PART;                     \
        }                                                                      \
    }
    switch (model->animal_parts) {
    case TEMPLATE_NO_PARTS:
    default:
        for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {
            parameters->animal_parts[slot] = CHAO_NO_PART;
        }
        break;
    case TEMPLATE_ANY_PARTS:
        SELECT_PARTS(11.999F, ANIMAL_PEN)
        break;
    case TEMPLATE_PEN_PARTS:
        SELECT_PARTS(2.999F, ANIMAL_PEN)
        break;
    case TEMPLATE_KUJA_PARTS:
        SELECT_PARTS(2.999F, ANIMAL_KUJA)
        break;
    case TEMPLATE_USA_PARTS:
        SELECT_PARTS(2.999F, ANIMAL_USA)
        break;
    case TEMPLATE_KUMA_PARTS:
        SELECT_PARTS(2.999F, ANIMAL_KUMA)
        break;
    case TEMPLATE_DRA_PARTS:
        SELECT_PARTS(2.999F, ANIMAL_DRA)
        break;
    case TEMPLATE_DRA_SLOT:
        for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {
            parameters->animal_parts[slot] = CHAO_NO_PART;
        }
        parameters->animal_parts[TEMPLATE_PART_SLOT] = ANIMAL_DRA;
        break;
    }
    /* the template's shiny doubles as a second parts code, whose draws
     * replace the parts above. a slot whose draw fails keeps its part */
    switch (model->shiny) {
    case TEMPLATE_NO_PARTS:
        break;
    case TEMPLATE_ANY_PARTS:
        for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {
            if (DRAW(1.0F) < 0.7F) {
                parameters->animal_parts[slot] =
                    (int)DRAW(14.999F) + ANIMAL_PEN;
            }
        }
        break;
    case TEMPLATE_DRA_PARTS:
        for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {
            if (DRAW(1.0F) < 0.7F) {
                parameters->animal_parts[slot] = (int)DRAW(2.999F) + ANIMAL_DRA;
            }
        }
        break;
    case TEMPLATE_DRA_SLOT:
        parameters->animal_parts[TEMPLATE_PART_SLOT] = ANIMAL_DRA;
        break;
    }
    /* @hack redundant: last, a copy of slot that nothing reads */
    last = slot;
    (void)last;
    parameters->run_power = SCATTER(model->run_power);
    parameters->fly_swim = SCATTER(model->fly_swim);
    parameters->alignment = SCATTER(model->alignment);
    parameters->evolution = SCATTER(model->evolution);
    if (parameters->evolution < 0.0F) {
        parameters->evolution = 0.0F;
    }
    if (parameters->evolution > 1.0F) {
        parameters->evolution = 1.0F;
    }

    /* @hack spelling: modes 0 and 2 in cases of their own, where one case for
     * both would do */
    switch (model->stat_mode) {
    case 0:
        base = model->stat_total / CHAO_SKILLS;
        for (slot = 0; slot < CHAO_SKILLS; slot++) {
            parameters->stats[slot] = (int)DRAW(STAT_DRAW) + base;
        }
        break;
    case 1:
        /* @hack spelling: advance a separate cursor, where indexing by
         * skill would do */
        cursor = model->stats;
        for (slot = 0; slot < CHAO_SKILLS; slot++) {
            range = cursor++;
            parameters->stats[slot] =
                (int)(range->spread * (DRAW(2.0F) - 1.0F)) + range->base;
        }
        break;
    case 2:
        base = model->stat_total / CHAO_SKILLS;
        for (slot = 0; slot < CHAO_SKILLS; slot++) {
            parameters->stats[slot] = (int)DRAW(STAT_DRAW) + base;
        }
        break;
    }
}

/* @hack spelling: three copies of one function, fill_beginner, fill_jewel
 * and fill_special, where one would do */
#define FILL_FIELD(name)                                                       \
    static void name(struct race_field *field)                                 \
    {                                                                          \
        int order[RACE_OPPONENTS];                                             \
        int slot;                                                              \
        int left;                                                              \
        int right;                                                             \
        int entry;                                                             \
        int other;                                                             \
                                                                               \
        for (slot = 0; slot < RACE_OPPONENTS; slot++) {                        \
            order[slot] = slot;                                                \
        }                                                                      \
        if (!field->fixed) {                                                   \
            for (slot = 0; slot < SWAPS; slot++) {                             \
                left = (int)DRAW(PLACE_DRAW);                                  \
                right = (int)DRAW(PLACE_DRAW);                                 \
                /* @hack spelling: both entries read before the swap writes    \
                 * either, where a temporary for one would do */               \
                entry = order[left];                                           \
                other = order[right];                                          \
                order[left] = other;                                           \
                order[right] = entry;                                          \
            }                                                                  \
        }                                                                      \
        for (slot = 0; slot < RACE_OPPONENTS; slot++) {                        \
            entry = field->entrants[order[slot]];                              \
            garden_race_make_opponent(&race_opponents[slot],                   \
                                      &opponent_templates[entry]);             \
        }                                                                      \
    }

FILL_FIELD(fill_beginner)
FILL_FIELD(fill_jewel)
FILL_FIELD(fill_special)

void garden_race_build_opponents(void)
{
    switch (garden_race_group()) {
    case RACE_BEGINNER:
        fill_beginner(
            &beginner_fields[garden_race_event()][garden_race_level()]);
        break;
    case RACE_JEWEL:
        fill_jewel(&jewel_fields[garden_race_event() - RACE_BEGINNER_COURSES]
                                [garden_race_level()]);
        break;
    case RACE_CHALLENGE:
        fill_special(
            &challenge_fields[challenge_races[garden_race_level()].opponents]);
        break;
    case RACE_DARK_COURSES:
        fill_special(&dark_fields[garden_race_level()]);
        break;
    case RACE_HERO_COURSES:
        fill_special(&hero_fields[garden_race_level()]);
        break;
    }
}
