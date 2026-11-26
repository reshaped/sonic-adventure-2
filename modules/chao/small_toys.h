/* a Chao's small toys, and its play with them alone */
#ifndef CHAO_SMALL_TOYS_H
#define CHAO_SMALL_TOYS_H
#include "chao/behavior.h"

struct task;

/* the small toys, each a bit of what a Chao has and its entry of
 * small_toy_actions */
enum chao_small_toy {
    CHAO_RATTLE_TOY = 0,
    CHAO_CAR_TOY = 1,
    CHAO_BOOK_TOY = 2,
    CHAO_DOLL_TOY = 4,
    CHAO_BROOM_TOY = 5,
    CHAO_STICK_TOY = 6,
    CHAO_POGO_STICK_TOY = 7,
    CHAO_CRAYON_TOY = 8,
    CHAO_BUBBLES_TOY = 9,
    CHAO_SHOVEL_TOY = 10,
    CHAO_WATERING_CAN_TOY = 11
};
#define CHAO_SMALL_TOYS 12
/* each small toy's behavior, or null for a toy that has none */
extern chao_behavior_fn small_toy_actions[CHAO_SMALL_TOYS];
/* the small toys a Chao has, as a set of bits. bit is a toy's number, below
 * CHAO_SMALL_TOYS */
void chao_clear_small_toys(struct task *chao);
void chao_add_small_toy(struct task *chao, int bit);
Bool chao_has_small_toy(struct task *chao, int bit);
/* set the behavior of one of actor's small toys, chosen at random, as
 * actor's own and return BEHAVIOR_FINISH. return BEHAVIOR_CONTINUE where none
 * of its toys has a behavior */
int chao_choose_small_toy(struct task *actor);
int chao_blow_bubbles(struct task *chao);
int chao_sweep(struct task *chao);
#ifdef NONMATCHING
/* kill chao's book, if it has one, and clear the link each holds to the
 * other. the book is a task of its own, which outlives its reader unless
 * released */
void chao_release_book(struct task *chao);
#endif

#endif
