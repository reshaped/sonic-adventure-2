/* the garden's toys, and the intentions that lead a bored Chao to them */
#ifndef CHAO_GARDEN_TOYS_H
#define CHAO_GARDEN_TOYS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

#define GARDEN_TOY_MODE(work, next)                                            \
    {                                                                          \
        (work)->mode = (next);                                                 \
        (work)->submode = 0;                                                   \
        (work)->timer = 0;                                                     \
        (work)->tick = 0;                                                      \
    }

/* the kind each toy registers as a GARDEN_OBJECT_TOY. no object registers 5
 * or 6 */
enum garden_toy_kind {
    GARDEN_TOY_TV = 2,
    GARDEN_TOY_RADIO = 3,
    GARDEN_TOY_BALL = 4,
    GARDEN_TOY_5 = 5,
    GARDEN_TOY_6 = 6,
    GARDEN_TOY_HORSE = 7,
    GARDEN_TOY_JACK = 8
};

/* the ids of toys' fields. a field of GARDEN_TOY_ACTIVATE sets off a toy it
 * touches. one of GARDEN_TOY_NEXT moves the radio to its next track. a
 * jack-in-the-box springs one of GARDEN_JACK_SPRING as it opens, and a Chao
 * it touches falls over crying. the TV and the radio make a field of their
 * signal when they start */
enum garden_toy_field {
    GARDEN_JACK_SPRING = 162,
    GARDEN_TOY_ACTIVATE = 175,
    GARDEN_TOY_NEXT = 177,
    GARDEN_TV_SIGNAL = 201,
    GARDEN_RADIO_SIGNAL = 202
};

/* each toy's mode, or -1 where the garden has none */
int garden_jack_mode(void);
int garden_radio_mode(void);
int garden_tv_mode(void);
extern struct task *garden_television, *garden_radio;
/* set *position to a random seat 10 to 13 in front of the toy, and return 1.
 * the seat lies within 40 degrees of straight ahead. without the toy, return
 * 0 */
int garden_tv_seat(NJS_POINT3 *position);
int garden_radio_seat(NJS_POINT3 *position);
/* intention scorers. each scores a bored Chao's wish to play. a score that
 * beats *best becomes the bid in *best. the scorer then sets the behaviors
 * that take the Chao to the toy. the TV's and the radio's score only within
 * reach of their signal, and bid 0.99 */
void chao_score_television(struct task *chao, Float *best);
void chao_score_radio(struct task *chao, Float *best);
void chao_score_garden_toy(struct task *chao, Float *best);

#endif
