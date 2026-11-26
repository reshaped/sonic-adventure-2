/* a garden's fruit, which a tree drops and a Chao eats in four bites */
#ifndef CHAO_FRUIT_H
#define CHAO_FRUIT_H
#include <ninja.h>

struct task;
/* the id of a fruit's PICKUP_LOWER shape, by which a Chao finds it */
#define GARDEN_FRUIT_HIT 131
/* a fruit's kind, which garden_fruit_models indexes. a planted tree of kind n
 * drops kind n - 1 */
enum garden_fruit {
    GARDEN_FRUIT_HERO_TREE_1 = 3,
    GARDEN_FRUIT_HERO_TREE_2 = 4,
    GARDEN_FRUIT_HERO_TREE_3 = 5,
    GARDEN_FRUIT_DARK_TREE_1 = 6,
    GARDEN_FRUIT_DARK_TREE_2 = 7,
    GARDEN_FRUIT_DARK_TREE_3 = 8,
    GARDEN_FRUIT_NEUTRAL = 9,
    GARDEN_FRUIT_HERO_GARDEN = 10,
    GARDEN_FRUIT_DARK_GARDEN = 11,
    GARDEN_FRUIT_CHAO = 12,
    GARDEN_FRUIT_HERO = 13,
    GARDEN_FRUIT_DARK = 14,
    GARDEN_FRUIT_HEART = 15,
    GARDEN_FRUIT_ACTIVE = 16,
    GARDEN_FRUIT_SMART = 17,
    GARDEN_FRUIT_CALM = 18,
    GARDEN_FRUIT_ROUND = 19,
    GARDEN_FRUIT_TRIANGLE = 20,
    GARDEN_FRUIT_SQUARE = 21
};

/* garden_bounce_on_floor for a fruit */
void garden_fruit_land(struct task *fruit);
/* a fruit of kind at position, facing yaw, with a non-null velocity as its
 * speed. it falls for 22 steps with its shapes off, then rolls free */
#ifdef NONMATCHING
/* this side returns the fruit, or null where it can't be made */
struct task *garden_fruit_create(int kind, NJS_POINT3 *position, Angle yaw,
                                 NJS_VECTOR *velocity);
#else
void garden_fruit_create(int kind, NJS_POINT3 *position, Angle yaw,
                         NJS_VECTOR *velocity);
#endif
/* the fruit's kind, a garden_fruit */
int garden_fruit_kind(struct task *fruit);
/* set the behavior taking a fruit the Chao touches, unless a Chao holds it.
 * TRUE where it sets it. a Chao eating, asleep or turning to a fruit already
 * tries none. the Chao turns to the fruit, takes it in both hands and eats
 * it. player 0 lets go of whatever it holds as the Chao takes the fruit. the
 * Chao gives up once it has faced the fruit's place over 60 frames with no
 * fruit touching it */
int chao_try_take_fruit(struct task *chao);
/* fetch the nearest fruit in sight and eat it. the behavior makes the fruit
 * the Chao's target and sets chao_stand_up_behavior. after it, it queues a
 * pause, the walk, chao_pick_up_target_behavior and chao_eat_fruit_behavior.
 * it returns BEHAVIOR_FINISH where no fruit is in sight or the nearest pairs
 * with another */
int chao_seek_fruit_behavior(struct task *chao);
/* eat the fruit the Chao holds, a bite at a time. a Dark Chao throws a Hero
 * fruit aside, and a Hero Chao a Dark one. so does a Chao whose taste refuses
 * the fruit's shape, or whose hunger runs out as it eats. it returns
 * BEHAVIOR_FINISH once no fruit pairs with the Chao both ways. a fruit the
 * player takes away angers and saddens the Chao */
int chao_eat_fruit_behavior(struct task *chao);

#endif
