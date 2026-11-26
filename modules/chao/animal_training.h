/* a Chao that takes in an animal or a Chaos Drive, and the skills each gives */
#ifndef CHAO_ANIMAL_TRAINING_H
#define CHAO_ANIMAL_TRAINING_H
#include <sg_xpt.h>

#include "hud/item_stock.h"

struct task;
/* the points an item adds to each racing skill */
struct animal_training_points {
    Sint16 swim, fly, run, power;
};
/* each item's points, by the item's number in hud/item_stock.h */
extern struct animal_training_points animal_training_rewards[ITEMS];

/* set the behavior taking in an animal or a drive the Chao touches, unless
 * a Chao holds it. TRUE where it sets one. a Chao taking one in already tries
 * none */
Bool chao_try_animal(struct task *chao);
/* player 0's side: 0 for Sonic, Tails, Knuckles and Amy. 1 for Shadow,
 * Eggman and Rouge, and 2 for any other character. no code calls it */
int garden_player_side(void);
/* start two trails of sparkles winding up around the Chao, in a child task */
void chao_create_training_spiral(struct task *chao);
#endif
