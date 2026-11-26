/* the cocoon a Chao closes in to evolve or to die, and a hatchling's first
 * motions */
#ifndef CHAO_COCOON_H
#define CHAO_COCOON_H
#include <ninja.h>

struct task;
extern NJS_CNK_OBJECT cocoon_model;
/* close the Chao in a cocoon, and change it to the adult form it leans to.
 * run_power, fly_swim and alignment choose the form. a Chao that leans little
 * either way may become a Chaos. it needs every animal taken in, more than 80
 * affection and at least two rebirths. a Hero or Dark form sets
 * GARDEN_HERO_OPEN or GARDEN_DARK_OPEN. the behavior finishes after the cocoon
 * opens and fades */
int chao_evolve_behavior(struct task *chao);
/* close the Chao in a cocoon, hide it and remove it. a Chao is reborn where
 * its affection passes chao_tuning.rebirth_affection and its stress is below
 * rebirth_stress_limit. an egg then takes its gene, profile and save
 * record */
int chao_die_behavior(struct task *chao);
/* play CHAO_ANIM_UMARERU_PON, and finish as it ends */
int chao_hatch_pop_behavior(struct task *chao);
/* play CHAO_ANIM_UMARERU_KYORO, and finish as it ends */
int chao_hatch_look_behavior(struct task *chao);
#endif
