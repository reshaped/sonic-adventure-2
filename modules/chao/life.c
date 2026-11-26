#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/emotion.h"
#include "chao/garden.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/knowledge.h"
#include "chao/race_chao.h"
#include "chao/race_record.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "player/player.h"
#include "system/task.h"
#include "system/util.h"

#define CHILD_SKILL_TOP 150
#define CHILD_SKILL_GAIN 4
#define SHAPE_UNITS 10000.0F
/* the chance, at each age tick, that the Chao catches an illness */
#define ILLNESS_CHANCE (1.0F / 6000.0F)
/* the illness chao_age never gives */
#define UNCAUGHT_ILLNESS 3
/* the count a caught illness starts at. the illness timer counts it up to 0,
 * and the illness lasts while it is below 0 */
#define CAUGHT_COUNT (-100)
#define BOREDOM_RESET 6000
/* pain above HURTING_PAIN costs condition */
#define HURTING_PAIN 100
#define AFFECTION_LIMIT 100
#define AFFECTION_MARK_FRAMES 180
#define HAS_ALIGNMENT(type, first)                                             \
    ((type) >= (first) && ((type) - (first)) % CHAO_FORM_ALIGNMENTS == 0)

static Uint32 garden_age_frames;
static Uint32 garden_age_turn;

Bool chao_type_is_hero(Uint8 type)
{
    return HAS_ALIGNMENT(type, CHAO_FIRST_HERO_TYPE);
}

Bool chao_type_is_dark(Uint8 type)
{
    return HAS_ALIGNMENT(type, CHAO_FIRST_DARK_TYPE);
}

Bool chao_is_hero(const struct task *chao)
{
    int type = ((const struct chao_work *)chao->work)->profile.parameters.type;
    return HAS_ALIGNMENT(type, CHAO_FIRST_HERO_TYPE);
}

Bool chao_is_dark(const struct task *chao)
{
    int type = ((const struct chao_work *)chao->work)->profile.parameters.type;
    return HAS_ALIGNMENT(type, CHAO_FIRST_DARK_TYPE);
}

/* the scale a trait gives a feeling's decay or a need's gain, in
 * CHAO_TRAIT_SCALE parts. it grows with the trait under MORE_WITH, and
 * shrinks under LESS_WITH */
#define MORE_WITH(trait) (chao_emotion_get(chao, (trait)) + CHAO_TRAIT_SCALE)
#define LESS_WITH(trait) (CHAO_TRAIT_SCALE - chao_emotion_get(chao, (trait)))
#define DECAY(feeling, factor)                                                 \
    chao_emotion_add(chao, (feeling),                                          \
                     -(chao_tuning.feeling_decay[(feeling)] * (factor)) /      \
                         CHAO_TRAIT_SCALE)
#define GAIN(need, base, factor)                                               \
    chao_emotion_add(chao, (need), (base) * (factor) / CHAO_TRAIT_SCALE)

void chao_age(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_profile *profile = &work->profile;
    struct chao_emotions *emotions = &profile->emotions;
    int illness;
    int i;
    int lifespan;

    if (!(work->flags & CHAO_AGES)) {
        return;
    }
    if (profile->parameters.age_timer++ >= chao_tuning.age_interval) {
        profile->parameters.age_timer = 0;
        profile->parameters.age++;
        if (profile->parameters.type == CHAO_CHILD) {
            chao_add_evolution(chao, chao_tuning.child_evolution);
            switch (chao_current_area()) {
            case CHAO_AREA_HERO:
                chao_add_alignment(chao, chao_tuning.child_garden_alignment);
                break;
            case CHAO_AREA_DARK:
                chao_add_alignment(chao, -chao_tuning.child_garden_alignment);
                break;
            }
            if (chao_skill_points(chao, CHAO_SWIM) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_SWIM, CHILD_SKILL_GAIN);
            }
            if (chao_skill_points(chao, CHAO_FLY) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_FLY, CHILD_SKILL_GAIN);
            }
            if (chao_skill_points(chao, CHAO_RUN) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_RUN, CHILD_SKILL_GAIN);
            }
            if (chao_skill_points(chao, CHAO_POWER) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_POWER, CHILD_SKILL_GAIN);
            }
            if (chao_skill_points(chao, CHAO_INTELLIGENCE) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_INTELLIGENCE, CHILD_SKILL_GAIN);
            }
            if (chao_skill_points(chao, CHAO_STAMINA) < CHILD_SKILL_TOP) {
                chao_add_skill(chao, CHAO_STAMINA, CHILD_SKILL_GAIN);
            }
        } else {
            lifespan = profile->parameters.lifespan;
            profile->parameters.adult_age++;
            if (profile->parameters.type < CHAO_CHAOS) {
                /* @hack spelling: the subtraction inside the test, where a
                 * statement of its own would do */
                if ((lifespan -= (profile->parameters.adult_age + 100) / 15) <
                    0) {
                    lifespan = 0;
                }
                profile->parameters.lifespan = lifespan;
            }
            if (profile->parameters.life > profile->parameters.lifespan) {
                profile->parameters.life = profile->parameters.lifespan;
            }
            chao_add_evolution(chao, chao_tuning.adult_evolution);
            switch (chao_current_area()) {
            case CHAO_AREA_HERO:
                chao_add_evolution(chao, chao_tuning.adult_garden_evolution);
                break;
            case CHAO_AREA_DARK:
                chao_add_evolution(chao, -chao_tuning.adult_garden_evolution);
                break;
            }
        }
        if (rand() * RAND_SCALE < ILLNESS_CHANCE) {
            illness = (Uint32)(rand() * (5.9999F * RAND_SCALE));
            /* @hack redundant: emotions found again from the task, which
             * leaves it as it was */
            emotions = &((struct chao_work *)chao->work)->profile.emotions;
            /* @hack spelling: two nested tests, where one test of both would
             * do */
            if (illness != UNCAUGHT_ILLNESS) {
                if (emotions->illness[illness] == 0) {
                    emotions->illness[illness] = CAUGHT_COUNT;
                }
            }
        }
    }
    if (emotions->feeling_timer++ >= chao_tuning.feeling_interval) {
        emotions->feeling_timer = 0;
        chao_emotion_add(chao, CHAO_PLEASURE,
                         -chao_tuning.feeling_decay[CHAO_PLEASURE]);
        DECAY(CHAO_ANGER, LESS_WITH(CHAO_AGGRESSION));
        DECAY(CHAO_ANGER, MORE_WITH(CHAO_KINDNESS));
        DECAY(CHAO_ANGER, MORE_WITH(CHAO_CALM));
        DECAY(CHAO_SORROW, MORE_WITH(CHAO_AGGRESSION));
        DECAY(CHAO_SORROW, MORE_WITH(CHAO_CALM));
        DECAY(CHAO_FEAR, MORE_WITH(CHAO_AGGRESSION));
        DECAY(CHAO_FEAR, MORE_WITH(CHAO_CALM));
        DECAY(CHAO_SURPRISE, MORE_WITH(CHAO_CALM));
        DECAY(CHAO_PAIN, MORE_WITH(CHAO_RECOVERY));
        DECAY(CHAO_RELAXATION, MORE_WITH(CHAO_CURIOSITY));
        DECAY(CHAO_RELAXATION, MORE_WITH(CHAO_FICKLENESS));
    }
    if (emotions->upkeep_timer++ >= chao_tuning.need_interval) {
        emotions->upkeep_timer = 0;
        chao_emotion_add(
            chao, CHAO_SLEEPINESS,
            /* the gain truncates to whole points before tiredness scales it */
            /* NOLINTNEXTLINE(bugprone-integer-division) */
            (Float)(chao_tuning.sleepiness_gain * MORE_WITH(CHAO_SLEEPY_HEAD) /
                    CHAO_TRAIT_SCALE) *
                ((chao_emotion_get(chao, CHAO_TIREDNESS) * 0.00006F) + 1.0F));
        GAIN(CHAO_HUNGER, chao_tuning.hunger_gain, MORE_WITH(CHAO_GLUTTONY));
        if (profile->parameters.type > CHAO_CHILD &&
            profile->parameters.type < CHAO_CHAOS) {
            GAIN(CHAO_BREEDING, chao_tuning.breeding_gain,
                 MORE_WITH(CHAO_VITALITY));
        }
        GAIN(CHAO_BOREDOM, chao_tuning.boredom_curiosity,
             MORE_WITH(CHAO_CURIOSITY));
        GAIN(CHAO_BOREDOM, chao_tuning.boredom_aggression,
             MORE_WITH(CHAO_AGGRESSION));
        GAIN(CHAO_BOREDOM, chao_tuning.boredom_calm, LESS_WITH(CHAO_CALM));
        if (chao_emotion_get(chao, CHAO_BOREDOM) > CHAO_NEED_MAX - 1) {
            if (rand() * RAND_SCALE < 0.1F) {
                chao_emotion_set(chao, CHAO_BOREDOM, BOREDOM_RESET);
            }
        }
        GAIN(CHAO_LONELINESS, chao_tuning.loneliness_gain,
             LESS_WITH(CHAO_SOLITUDE));
        GAIN(CHAO_TIREDNESS, chao_tuning.tiredness_gain,
             LESS_WITH(CHAO_VITALITY));
        chao_emotion_add(chao, CHAO_NOURISHMENT,
                         -chao_tuning.nourishment_decay);
        if (chao_emotion_get(chao, CHAO_NOURISHMENT) >
            chao_tuning.fed_threshold) {
            chao_emotion_add(chao, CHAO_CONDITION, chao_tuning.fed_gain);
        }
        if (chao_emotion_get(chao, CHAO_NOURISHMENT) <
            chao_tuning.hungry_threshold) {
            chao_emotion_add(chao, CHAO_CONDITION, -chao_tuning.hungry_loss);
        }
        if (chao_emotion_get(chao, CHAO_STRESS) >
            chao_tuning.stress_threshold) {
            chao_emotion_add(chao, CHAO_CONDITION, -chao_tuning.stress_loss);
        }
        if (chao_emotion_get(chao, CHAO_PAIN) > HURTING_PAIN) {
            chao_emotion_add(chao, CHAO_CONDITION, -chao_tuning.pain_loss);
        }
        chao_emotion_add(chao, CHAO_THIRST, chao_tuning.thirst_gain);
    }
    if (emotions->illness_timer++ >= chao_tuning.illness_interval) {
        emotions->illness_timer = 0;
        for (i = 0; i < CHAO_ILLNESSES; i++) {
            if (emotions->illness[i] < 0) {
                emotions->illness[i]++;
            } else if (emotions->illness[i] > 0) {
                emotions->illness[i]--;
            } else {
                /* @hack redundant: an empty else */
            }
        }
    }
    chao_age_memories(chao);
}

int chao_skill_points(const struct task *chao, Uint16 skill)
{
    const struct chao_work *work = (const struct chao_work *)chao->work;
    const struct chao_parameters *parameters = &work->profile.parameters;

    /* @hack redundant: skill >= 0 on the unsigned skill, which always
     * holds */
    if (skill >= 0 && skill < CHAO_SKILLS) {
        return parameters->stats[skill];
    }
    return -1;
}

void chao_add_skill(struct task *chao, Uint16 skill, int points)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    /* @hack redundant: a second read of the task's work, which nothing
     * uses */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct chao_work *unused_work = (struct chao_work *)chao->work;
    int value;
    int previous;

    /* @hack redundant: skill >= 0 on the unsigned skill, which always
     * holds */
    if (skill >= 0 && skill < CHAO_SKILLS) {
        value = previous = parameters->stats[skill];
        if (parameters->best_stats[skill] > previous) {
            points += points;
        }
        value += points;
        if (value > chao_tuning.skill_limit) {
            value = chao_tuning.skill_limit;
        }
        if (value < 0) {
            value = 0;
        }
        parameters->stats[skill] = value;
        if (parameters->best_stats[skill] < value) {
            parameters->best_stats[skill] = value;
        }
    }
}

void chao_add_race_stamina(struct task *chao, Sint16 delta)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    /* @hack redundant: a second read of the task's work, which nothing
     * uses */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct chao_work *unused_work = (struct chao_work *)chao->work;
    Sint32 value = parameters->race_stamina;

    value += delta;
    if (value < 0) {
        value = 0;
    }
    parameters->race_stamina = value;
}

void chao_add_evolution(struct task *chao, Float delta)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_parameters *parameters = &work->profile.parameters;
    struct chao_shape_work *shape = &work->shape;

    parameters->evolution += delta;
    if (parameters->evolution > chao_tuning.evolution_limit) {
        parameters->evolution = chao_tuning.evolution_limit;
    }
    if (parameters->evolution < 0.0F) {
        parameters->evolution = 0.0F;
    }
    shape->flags |= CHAO_SHAPE_RESHAPE;
}

void chao_add_run_power(struct task *chao, Float delta)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    struct chao_shape_work *shape = &((struct chao_work *)chao->work)->shape;

    parameters->run_power += delta;
    if (parameters->run_power > chao_tuning.shape_limit) {
        parameters->run_power = chao_tuning.shape_limit;
    }
    if (parameters->run_power < -chao_tuning.shape_limit) {
        parameters->run_power = -chao_tuning.shape_limit;
    }
    shape->flags |= CHAO_SHAPE_RESHAPE;
}

void chao_add_fly_swim(struct task *chao, Float delta)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    struct chao_shape_work *shape = &((struct chao_work *)chao->work)->shape;

    parameters->fly_swim += delta;
    if (parameters->fly_swim > chao_tuning.shape_limit) {
        parameters->fly_swim = chao_tuning.shape_limit;
    }
    if (parameters->fly_swim < -chao_tuning.shape_limit) {
        parameters->fly_swim = -chao_tuning.shape_limit;
    }
    shape->flags |= CHAO_SHAPE_RESHAPE;
}

void chao_add_alignment(struct task *chao, Float delta)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_parameters *parameters = &work->profile.parameters;
    struct chao_shape_work *shape = &work->shape;

    parameters->alignment += delta;
    if (parameters->alignment > 1.0F) {
        parameters->alignment = 1.0F;
    }
    if (parameters->alignment < -1.0F) {
        parameters->alignment = -1.0F;
    }
    shape->flags |= CHAO_SHAPE_RESHAPE;
}

void chao_add_life(struct task *chao, int delta)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    int life = parameters->life;
    int lifespan = parameters->lifespan;

    life += delta;
    if (life > lifespan) {
        life = lifespan;
    }
    if (life < 0) {
        life = 0;
    }
    parameters->life = life;
}

void chao_add_lifespan(struct task *chao, int delta)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    int life = parameters->life;
    int lifespan = parameters->lifespan + delta;

    if (lifespan > chao_tuning.lifespan_limit) {
        lifespan = chao_tuning.lifespan_limit;
    }
    if (lifespan < 0) {
        lifespan = 0;
    }
    if (life > lifespan) {
        life = lifespan;
    }
    parameters->life = life;
    parameters->lifespan = lifespan;
}

void chao_player_affection(struct task *chao, int delta, Float alignment)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    int affection = parameters->affection + delta;

    if (affection > AFFECTION_LIMIT) {
        affection = AFFECTION_LIMIT;
    } else if (affection < -AFFECTION_LIMIT) {
        affection = -AFFECTION_LIMIT;
    }
    parameters->affection = affection;
    switch (player_character(0)) {
    case PLAYER_SONIC:
    case PLAYER_TAILS:
    case PLAYER_KNUCKLES:
    case PLAYER_KIND_AMY:
        chao_add_alignment(chao, alignment);
        break;
    case PLAYER_SHADOW:
    case PLAYER_EGGMAN:
    case PLAYER_ROUGE:
        chao_add_alignment(chao, -alignment);
    }
    chao_remember_player(chao, delta);
    if (delta > 0) {
        chao_show_mark(chao, CHAO_MARK_HEART, AFFECTION_MARK_FRAMES);
    } else {
        chao_show_mark(chao, CHAO_MARK_ANGER, AFFECTION_MARK_FRAMES);
    }
}

void garden_age_reset(void)
{
    garden_age_frames = 0;
    garden_age_turn = 0;
}

void garden_age_step(void)
{
    struct task *chao;
    Uint32 index;

    if (++garden_age_frames >= chao_tuning.garden_age_interval) {
        garden_age_frames = 0;
        index = garden_age_turn % garden_object_capacities[GARDEN_OBJECT_CHAO];
        chao = garden_find_object(GARDEN_OBJECT_CHAO, index);
        if (chao != NULL) {
            chao_age(chao);
        }
        garden_age_turn++;
    }
}

Bool chao_take_age_tick(struct task *chao)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;

    if (parameters->age_timer == 0) {
        parameters->age_timer++;
        return TRUE;
    }
    return FALSE;
}
Bool chao_take_upkeep_tick(struct task *chao)
{
    struct chao_emotions *emotions =
        &((struct chao_work *)chao->work)->profile.emotions;

    if (emotions->upkeep_timer == 0) {
        emotions->upkeep_timer++;
        return TRUE;
    }
    return FALSE;
}

void chao_pack_racer(struct garden_race_chao *racer,
                     const struct chao_parameters *parameters)
{
    racer->type = parameters->type;
    racer->eye = parameters->eye;
    racer->mouth = parameters->mouth;
    racer->emotion_ball = parameters->emotion_ball;
    racer->hat = parameters->hat;
    racer->costume = parameters->costume;
    racer->medal = parameters->medal;
    racer->color = parameters->color;
    racer->texture = parameters->texture;
    racer->shiny = parameters->shiny;
    njMemCopy(racer->animal_parts, parameters->animal_parts,
              sizeof(racer->animal_parts));
    racer->run_power = parameters->run_power * SHAPE_UNITS;
    racer->fly_swim = parameters->fly_swim * SHAPE_UNITS;
    racer->alignment = parameters->alignment * SHAPE_UNITS;
    racer->evolution = parameters->evolution * SHAPE_UNITS;
    njMemCopy(racer->name, parameters->name, sizeof(racer->name));
    njMemCopy(racer->stats, parameters->stats, sizeof(racer->stats));
}

void chao_unpack_racer(struct chao_parameters *parameters,
                       const struct garden_race_chao *racer)
{
    parameters->type = racer->type;
    parameters->eye = racer->eye;
    parameters->mouth = racer->mouth;
    parameters->emotion_ball = racer->emotion_ball;
    parameters->hat = racer->hat;
    parameters->costume = racer->costume;
    parameters->medal = racer->medal;
    parameters->color = racer->color;
    parameters->texture = racer->texture;
    parameters->shiny = racer->shiny;
    njMemCopy(parameters->animal_parts, racer->animal_parts,
              sizeof(parameters->animal_parts));
    parameters->run_power = racer->run_power * (1.0F / SHAPE_UNITS);
    parameters->fly_swim = racer->fly_swim * (1.0F / SHAPE_UNITS);
    parameters->alignment = racer->alignment * (1.0F / SHAPE_UNITS);
    parameters->evolution = racer->evolution * (1.0F / SHAPE_UNITS);
    njMemCopy(parameters->name, racer->name, sizeof(parameters->name));
    njMemCopy(parameters->stats, racer->stats, sizeof(parameters->stats));
}
