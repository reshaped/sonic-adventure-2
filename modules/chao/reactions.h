/* what pain.c and pleasure.c share. REACTION_SCORE scores action once the
 * Chao's feeling of kind passes the tuning's threshold. the more of trait
 * the Chao has, and the higher weight, the higher the score. the action
 * becomes the Chao's behavior where its score beats *best */
#ifndef CHAO_REACTIONS_H
#define CHAO_REACTIONS_H
#include <sg_xpt.h>
#include "chao/behavior.h"
#include "chao/emotion.h"
#include "chao/intention.h"
#include "chao/tuning.h"
#define REACTION_SCORE(kind, trait, threshold, weight, action)                 \
    struct chao_tuning *tuning;                                                \
    Uint32 feeling;                                                            \
    Float score;                                                               \
    score = 0.0F;                                                              \
    tuning = &chao_tuning;                                                     \
    feeling = chao_emotion_get(chao, (kind));                                  \
    if (feeling > tuning->threshold) {                                         \
        score = 1.0F - ((100 - chao_emotion_get(chao, (trait))) *              \
                        (1.0F - tuning->weight) * 0.005F);                     \
        chao_vary_score(&score);                                               \
        if (score > *best) {                                                   \
            chao_set_behavior(chao, (action));                                 \
            chao_clear_intention(chao);                                        \
            *best = score;                                                     \
        }                                                                      \
    }

#endif
