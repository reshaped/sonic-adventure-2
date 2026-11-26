/* a Chao planting a seed and watering a tree. GARDEN_SEED_HIT,
 * GARDEN_EMPTY_PLOT_HIT, GARDEN_WATER_HIT and GARDEN_THIRST_HIT are the ids
 * of a seed's shape, an empty plot's, the water's field and a thirsty tree's
 * shape */
#ifndef CHAO_PLANTING_H
#define CHAO_PLANTING_H
struct task;
enum {
    GARDEN_SEED_HIT = 133,
    GARDEN_EMPTY_PLOT_HIT = 172,
    GARDEN_WATER_HIT = 173,
    GARDEN_THIRST_HIT = 174
};
/* a seed's kind, which its task's work keeps in ang.x */
#define SEED_KIND(work) ((work)->ang.x)
/* the command a Chao sends an empty plot, with the seed's kind as its state */
#define PLANT_SEED 5
int chao_try_take_seed(struct task *chao);
int chao_examine_seed(struct task *chao);
int chao_approach_plant(struct task *chao);
int chao_prepare_to_water(struct task *chao);
#endif
