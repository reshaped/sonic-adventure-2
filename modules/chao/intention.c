#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/activities.h"
#include "chao/aggression.h"
#include "chao/animal_actions.h"
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/dance.h"
#include "chao/gait.h"
#include "chao/cocoon.h"
#include "chao/cry.h"
#include "chao/emotion.h"
#include "chao/idle.h"
#include "chao/garden_toys.h"
#include "chao/intention.h"
#include "chao/instruments.h"
#include "chao/small_toys.h"
#include "chao/life.h"
#include "chao/mating.h"
#include "chao/movement.h"
#include "chao/needs.h"
#include "chao/perception.h"
#include "chao/planting.h"
#include "chao/player_visit.h"
#include "chao/residents.h"
#include "chao/sleep.h"
#include "chao/singing.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/task.h"
#include "system/util.h"

#define TICKED_COOLDOWNS 16
#define NO_INTENTION 0
#define TABLE_INTENTION 17
/* a Chao's curiosity less its calm runs from -TEMPERAMENT to TEMPERAMENT */
#define TEMPERAMENT 200
#define JOIN_MATE_SCORE 0.88F

/* chao_action_table's tables */
enum action_table {
    IDLE_TABLE,
    ILLNESS_TABLE,
    INSTRUMENT_TABLE,
    ANIMAL_TABLE,
    SMALL_TOY_TABLE
};

/* the action table and entry TABLE_INTENTION runs. no code moves them from
 * IDLE_TABLE, which is chao_idle_behavior alone */
static Uint8 chao_action_table = IDLE_TABLE;
static Uint8 chao_action_index = 0;

Float chao_need_above(int value, int threshold)
{
    Float current = value;
    Float low = threshold;

    if (value > threshold) {
        return (current - low) / ((Uint16)chao_tuning.need_max - low);
    }
    return 0.0F;
}

Float chao_need_below(int value, int threshold)
{
    Float current = value;
    Float high = threshold;

    if (value < threshold) {
        return (high - current) / high;
    }
    return 0.0F;
}

Float chao_feeling_above(int value, int threshold)
{
    Float current = value;
    Float low = threshold;

    if (value > threshold) {
        return (current - low) / (CHAO_FEELING_MAX - low);
    }
    return 0.0F;
}

Float chao_feeling_below(int value, int threshold)
{
    Float current = value;
    Float high = threshold;

    if (value < threshold) {
        return (high - current) / high;
    }
    return 0.0F;
}

void chao_vary_score(Float *score)
{
    Float value;
    Float retained;
    Float fixed;
    Float variable;

    retained = chao_tuning.score_retained;
    value = *score;
    fixed = value * retained;
    variable = value * (1.0F - retained) * rand() * RAND_SCALE;
    *score = fixed + variable;
}

Uint32 chao_cooldown(struct task *chao, Uint16 index)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    return behavior->cooldown[index];
}

void chao_set_cooldown(struct task *chao, Uint16 index, Uint32 frames)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    behavior->cooldown[index] = frames;
}

#pragma inline(chao_clear_intention)
void chao_clear_intention(struct task *chao)
{
    chao_set_intention(chao, NO_INTENTION);
}

int chao_intention(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    return behavior->intention;
}

void chao_set_intention(struct task *chao, int intention)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;

    behavior->intention = intention;
    behavior->intention_age = 0;
}

static void score_thirst(struct task *chao, Float *best)
{
    Float score;
    Uint32 minimum;
    Uint32 thirst;

    score = 0.0F;
    minimum = chao_tuning.thirst_high;
    thirst = chao_emotion_get(chao, CHAO_THIRST);
    if (*best < 1.0F) {
        if (thirst > minimum) {
            score = chao_need_above(thirst, minimum);
            score *= chao_tuning.thirst_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            chao_set_behavior(chao, chao_seek_water);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}

static void score_plant(struct task *chao, Float *best)
{
    Float score;
    struct task *plant;
    Uint32 thirst;
    Uint32 limit;

    score = 0.0F;
    if (*best < 1.0F) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((plant = garden_hit_kind(chao, CHAO_PUSH_SHAPE,
                                     GARDEN_THIRST_HIT)) != NULL) {
            thirst = chao_emotion_get(chao, CHAO_THIRST);
            limit = chao_tuning.thirst_low;
            score = chao_need_below(thirst, limit);
            score *= chao_tuning.thirst_low_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            chao_set_behavior(chao, chao_approach_plant);
            garden_set_target(chao, plant);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}

static void score_tiredness(struct task *chao, Float *best)
{
    Float score;
    Uint32 low;
    Uint32 high;
    Uint32 tiredness;
    Uint32 threshold;
    int temperament;
    Float fraction;

    score = 0.0F;
    tiredness = chao_emotion_get(chao, CHAO_TIREDNESS);
    if (*best < 1.0F) {
        low = chao_tuning.tiredness_low;
        high = chao_tuning.tiredness_high;
        /* the Chao's curiosity less its calm places the threshold between
         * low, at -TEMPERAMENT, and high, at TEMPERAMENT */
        temperament = -chao_emotion_get(chao, CHAO_CALM) +
                      chao_emotion_get(chao, CHAO_CURIOSITY);
        temperament += TEMPERAMENT;
        fraction = temperament * (1.0F / (2 * TEMPERAMENT));
        threshold = low + (Uint32)((high - low) * fraction);
        if (tiredness > threshold) {
            score = chao_need_above(tiredness, threshold);
            score *= chao_tuning.tiredness_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            chao_set_behavior(chao, chao_sit_down_behavior);
            chao_queue_behavior(chao, chao_rest_behavior);
            chao_clear_intention(chao);
            *best = score;
        }
    }
}

static void score_evolution(struct task *chao, Float *best)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float limit;

    if (*best < 1.0F && parameters->type == CHAO_CHILD) {
        limit = chao_tuning.evolution_threshold;
        if (parameters->evolution < limit) {
            return;
        }
        parameters->evolution = limit;
        if (chao_settled(chao)) {
            chao_set_behavior(chao, chao_evolve_behavior);
            chao_clear_intention(chao);
            *best = 1.0F;
        }
    }
}

static void score_call_mate(struct task *chao, Float *best)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;

    if (*best < 1.0F) {
        switch (parameters->type) {
        case CHAO_CHILD:
        case CHAO_CHAOS:
        case CHAO_HERO_CHAOS:
        case CHAO_DARK_CHAOS:
            return;
        }
        if (garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_MATING_FIELD)) {
            return;
        }
        if (chao_emotion_get(chao, CHAO_BREEDING) < CHAO_NEED_MAX) {
            return;
        }
        if (garden_resident_count() < CHAO_PER_GARDEN) {
            if (chao_settled(chao)) {
                chao_set_behavior(chao, chao_sit_down_behavior);
                chao_queue_behavior(chao, chao_call_mate);
                chao_clear_intention(chao);
                *best = 1.0F;
            }
        } else {
            chao_emotion_set(chao, CHAO_BREEDING, 0);
        }
    }
}

static void score_join_mate(struct task *chao, Float *best)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float score;
    Uint32 threshold;
    Uint32 breeding;

    score = 0.0F;
    threshold = chao_tuning.breeding_threshold;
    breeding = chao_emotion_get(chao, CHAO_BREEDING);
    switch (parameters->type) {
    case CHAO_CHILD:
    case CHAO_CHAOS:
    case CHAO_HERO_CHAOS:
    case CHAO_DARK_CHAOS:
        return;
    }
    if (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_MATING_FIELD)) {
        return;
    }
    if (breeding > threshold) {
        score = JOIN_MATE_SCORE;
        score *= chao_tuning.breeding_weight;
        chao_vary_score(&score);
    }
    if (score > *best) {
        chao_set_behavior(chao, chao_join_mate);
        chao_clear_intention(chao);
        *best = score;
    }
}

static void score_death(struct task *chao, Float *best)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;

    /* @hack spelling: <= 0 on the unsigned life, where == 0 would do */
    if (*best < 1.0F && parameters->life <= 0) {
        if (chao_settled(chao)) {
            chao_set_behavior(chao, chao_die_behavior);
            chao_clear_intention(chao);
            *best = 1.0F;
        }
    }
}

void chao_choose_intention(struct task *chao)
{
    struct chao_parameters *parameters = &CHAO_WORK(chao)->profile.parameters;
    Float best;

    best = chao_tuning.score_floor;
    chao_notice_objects(chao);
    chao_clear_intention(chao);
    score_condition(chao, &best);
    score_drowsiness(chao, &best);
    score_waking(chao, &best);
    score_sleep(chao, &best);
    score_hunger(chao, &best);
    score_thirst(chao, &best);
    score_nearby_instrument(chao, &best);
    score_nearby_dance(chao, &best);
    score_hearing_song(chao, &best);
    score_hearing_bad_song(chao, &best);
    chao_score_television(chao, &best);
    chao_score_radio(chao, &best);
    chao_score_garden_toy(chao, &best);
    score_play(chao, &best);
    score_tiredness(chao, &best);
    score_player_fear(chao, &best);
    score_sorrow(chao, &best);
    score_pain(chao, &best);
    score_pleasure(chao, &best);
    score_evolution(chao, &best);
    if (parameters->type != CHAO_CHILD) {
        score_player_dislike(chao, &best);
        score_plant(chao, &best);
        score_player_visit(chao, &best);
        score_join_mate(chao, &best);
        score_call_mate(chao, &best);
        score_death(chao, &best);
    }
    if (best <= chao_tuning.score_floor) {
        chao_set_behavior(chao, chao_choose_destination);
        chao_clear_intention(chao);
    }
}

void chao_tick_intentions(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    int index;

    for (index = 0; index < TICKED_COOLDOWNS; index++) {
        if (behavior->cooldown[index] > 0) {
            behavior->cooldown[index]--;
        }
    }
}

void fun_0c50ee3a(struct task *chao)
{
}

int fun_0c50ee3e(struct task *chao)
{
    return 1;
}

void chao_run_intention(struct task *chao, int intention)
{
    switch (intention) {
    case NO_INTENTION:
        break;
    case TABLE_INTENTION:
        /* nothing reads the draw, but it still advances rand's sequence */
        rand();
        switch (chao_action_table) {
        case IDLE_TABLE:
            chao_set_behavior(chao, chao_idle_behavior);
            break;
        case ILLNESS_TABLE:
            chao_set_behavior(chao, illness_actions[chao_action_index]);
            break;
        case INSTRUMENT_TABLE:
            chao_set_behavior(chao, instrument_actions[chao_action_index]);
            break;
        case ANIMAL_TABLE:
            chao_set_behavior(chao, animal_actions[chao_action_index]);
            break;
        case SMALL_TOY_TABLE:
            chao_set_behavior(chao, small_toy_actions[chao_action_index]);
            break;
        }
        chao_clear_intention(chao);
        break;
    default:
        chao_set_behavior(chao, chao_idle_behavior);
        chao_clear_intention(chao);
        break;
    }
}
