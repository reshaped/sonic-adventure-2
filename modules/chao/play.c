/* the play bid, by boredom. a play that starts takes the bid at 0.99 and
 * spends the Chao's boredom. a play that can't start leaves both as they
 * were */
#include <sg_xpt.h>

#include "chao/animal_actions.h"
#include "chao/dance.h"
#include "chao/drawing.h"
#include "chao/emotion.h"
#include "chao/instruments.h"
#include "chao/intention.h"
#include "chao/needs.h"
#include "chao/singing.h"
#include "chao/small_toys.h"
#include "chao/tuning.h"
#include "system/util.h"

#define START_PLAY(chao, best, started)                                        \
    {                                                                          \
        chao_emotion_add(chao, CHAO_BOREDOM, -5000);                           \
        chao_clear_intention(chao);                                            \
        *(best) = 0.99F;                                                       \
        (started) = TRUE;                                                      \
    }

void score_play(struct task *chao, Float *best)
{
    Float score = 0.0F;
    Uint32 threshold = chao_tuning.boredom_threshold;
    Uint32 boredom = chao_emotion_get(chao, CHAO_BOREDOM);
    int started;
    Float activity;
    if (*best < 1.0F) {
        if (boredom > threshold) {
            score = chao_need_above(boredom, threshold);
            score *= chao_tuning.boredom_weight;
            chao_vary_score(&score);
        }
        if (score > *best) {
            started = FALSE;
            if (RANDOM_UNIT() < 0.4F) {
                activity = RANDOM_UNIT();
                if (activity < 0.25F) {
                    if (chao_start_band(chao)) {
                        START_PLAY(chao, best, started);
                    }
                } else if (activity < 0.5F) {
                    if (chao_start_dance(chao)) {
                        START_PLAY(chao, best, started);
                    }
                } else if (activity < 0.75F) {
                    if (chao_start_singing(chao)) {
                        START_PLAY(chao, best, started);
                    }
                } else {
                    if (chao_try_drawing(chao)) {
                        START_PLAY(chao, best, started);
                    }
                }
            } else if (RANDOM_UNIT() < 0.5F) {
                if (chao_choose_small_toy(chao)) {
                    START_PLAY(chao, best, started);
                }
            } else {
                if (chao_choose_animal_action(chao)) {
                    START_PLAY(chao, best, started);
                }
            }
            /* @hack redundant: started, which only guards a return at the
             * function's end */
            if (!started) {
                return;
            }
        }
    }
}
int fun_0c52e4f6(void)
{
    return 0;
}
