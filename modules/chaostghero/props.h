/* the Hero Garden's props: eight kinds of object that stand still and draw a
 * model each */
#ifndef CHAOSTGHERO_PROPS_H
#define CHAOSTGHERO_PROPS_H
#include "chao/garden_scene.h"

struct task;

/* the steps chaostghero_kinds holds for the props. each gives task the
 * prop's step, draw and destructor. hero_pillar_init and
 * hero_broken_pillar_init also give it a collision record. the task must
 * hold a work. it destroys itself once placement_try_leave finds it has left
 * range */
void hero_pillar_init(struct task *task);
void hero_broken_pillar_init(struct task *task);
void hero_red_flowers_init(struct task *task);
void hero_purple_flowers_init(struct task *task);
void hero_grass_a_init(struct task *task);
void hero_grass_b_init(struct task *task);
void hero_grass_c_init(struct task *task);
void hero_tower_init(struct task *task);

#endif
