#include <sg_xpt.h>

#include "chao/emotion.h"
#include "chao/work.h"
#include "system/task.h"

/* @hack redundant: the lower bounds. each test runs only where the test
 * before it has failed, so its lower bound holds already */
#define IS_NEED(kind) ((kind) >= CHAO_SLEEPINESS && (kind) <= CHAO_THIRST)
#define IS_TRAIT(kind) ((kind) >= CHAO_CURIOSITY && (kind) <= CHAO_FICKLENESS)

#define CLAMP_EMOTION(v, lo, hi)                                               \
    do {                                                                       \
        if ((v) > (hi)) {                                                      \
            (v) = (hi);                                                        \
        }                                                                      \
        if ((v) < (lo)) {                                                      \
            (v) = (lo);                                                        \
        }                                                                      \
    } while (0)

void chao_emotion_add(struct task *chao, Uint32 kind, int delta)
{
    struct chao_emotions *state =
        &((struct chao_work *)chao->work)->profile.emotions;
    int value;

    /* @hack spelling: each read and its add as two statements, where one
     * would set value to the sum */
    if (kind <= CHAO_TOTAL) {
        value = state->feelings[kind];
        value += delta;
        CLAMP_EMOTION(value, 0, CHAO_FEELING_MAX);
        state->feelings[kind] = value;
    } else if (IS_NEED(kind)) {
        value = state->needs[kind - CHAO_SLEEPINESS];
        value += delta;
        CLAMP_EMOTION(value, 0, CHAO_NEED_MAX);
        state->needs[kind - CHAO_SLEEPINESS] = value;
    } else if (IS_TRAIT(kind)) {
        value = state->personality.preferences.traits[kind - CHAO_CURIOSITY];
        value += delta;
        CLAMP_EMOTION(value, -CHAO_TRAIT_MAX, CHAO_TRAIT_MAX);
        state->personality.preferences.traits[kind - CHAO_CURIOSITY] = value;
    }
}

void chao_emotion_set(struct task *chao, Uint32 kind, int value)
{
    struct chao_emotions *state =
        &((struct chao_work *)chao->work)->profile.emotions;

    if (kind <= CHAO_TOTAL) {
        CLAMP_EMOTION(value, 0, CHAO_FEELING_MAX);
        state->feelings[kind] = value;
    } else if (IS_NEED(kind)) {
        CLAMP_EMOTION(value, 0, CHAO_NEED_MAX);
        state->needs[kind - CHAO_SLEEPINESS] = value;
    } else if (IS_TRAIT(kind)) {
        CLAMP_EMOTION(value, -CHAO_TRAIT_MAX, CHAO_TRAIT_MAX);
        state->personality.preferences.traits[kind - CHAO_CURIOSITY] = value;
    }
}

int chao_emotion_get(const struct task *chao, Uint32 kind)
{
    const struct chao_emotions *state =
        &((const struct chao_work *)chao->work)->profile.emotions;

    if (kind <= CHAO_TOTAL) {
        return state->feelings[kind];
    } else if (IS_NEED(kind)) {
        return state->needs[kind - CHAO_SLEEPINESS];
    } else if (IS_TRAIT(kind)) {
        return state->personality.preferences.traits[kind - CHAO_CURIOSITY];
    }

    /* @hack implicit: no return past the traits, where SHC's code returns
     * CHAO_THIRST, the bound its need test left */
#ifdef NONMATCHING
    /* this side returns CHAO_THIRST, which SHC's code leaves in R0 */
    return CHAO_THIRST;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
