/* a garden Chao's emotions: its feelings, its needs and its personality
 * traits, under one numbering */
#ifndef CHAO_EMOTION_H
#define CHAO_EMOTION_H
#include <sg_xpt.h>

struct task;

/* CHAO_PLEASURE to CHAO_TOTAL are feelings, which range from 0 to
 * CHAO_FEELING_MAX. CHAO_SLEEPINESS to CHAO_THIRST are needs, from 0 to
 * CHAO_NEED_MAX. CHAO_CURIOSITY to CHAO_FICKLENESS are personality traits, from
 * -CHAO_TRAIT_MAX to CHAO_TRAIT_MAX */
#define CHAO_FEELING_MAX 200
#define CHAO_NEED_MAX 10000
#define CHAO_TRAIT_MAX 100
/* a trait scales a rate by CHAO_TRAIT_SCALE plus or less the trait, in
 * CHAO_TRAIT_SCALE parts: from 2/3 to 4/3 */
#define CHAO_TRAIT_SCALE 300
enum chao_emotion_kind {
    CHAO_PLEASURE,
    CHAO_ANGER,
    CHAO_SORROW,
    CHAO_FEAR,
    CHAO_SURPRISE,
    CHAO_PAIN,
    CHAO_RELAXATION,
    CHAO_TOTAL,
    CHAO_SLEEPINESS,
    CHAO_SLEEP_DEPTH,
    CHAO_HUNGER,
    CHAO_BREEDING,
    CHAO_BOREDOM,
    CHAO_LONELINESS,
    CHAO_TIREDNESS,
    CHAO_STRESS,
    CHAO_NOURISHMENT,
    CHAO_CONDITION,
    CHAO_THIRST,
    CHAO_CURIOSITY,
    CHAO_KINDNESS,
    CHAO_AGGRESSION,
    CHAO_SLEEPY_HEAD,
    CHAO_SOLITUDE,
    CHAO_VITALITY,
    CHAO_GLUTTONY,
    CHAO_RECOVERY,
    CHAO_SKILLFULNESS,
    CHAO_CHARM,
    CHAO_CHATTINESS,
    CHAO_CALM,
    CHAO_FICKLENESS
};

/* add delta to the Chao's emotion kind, clamped to the kind's range. a kind
 * past CHAO_FICKLENESS changes nothing */
void chao_emotion_add(struct task *chao, Uint32 kind, int delta);
/* set the Chao's emotion kind to value, clamped as chao_emotion_add clamps */
void chao_emotion_set(struct task *chao, Uint32 kind, int value);
/* the Chao's emotion kind. a kind past CHAO_FICKLENESS returns the number
 * CHAO_THIRST, 18, and not the Chao's thirst */
int chao_emotion_get(const struct task *chao, Uint32 kind);

#endif
