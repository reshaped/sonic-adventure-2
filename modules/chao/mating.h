/* mating: one Chao calls for a mate, and another joins it and lays an egg */
#ifndef CHAO_MATING_H
#define CHAO_MATING_H
struct task;

/* the kind of the field a calling Chao makes, which a joining Chao touches */
#define CHAO_MATING_FIELD 171
/* make a ring of flowers about the Chao, as its child task, and return it, or
 * NULL. the flowers stay while the Chao sets calling_mate in each step */
struct task *chao_create_mating_flowers(struct task *chao);
/* the behaviors of the Chao that calls and of the one that joins it. the two
 * pair and dance. the joining Chao then lays an egg, where garden_unused_chao
 * finds a save slot for it */
int chao_call_mate(struct task *chao);
int chao_join_mate(struct task *chao);
#endif
