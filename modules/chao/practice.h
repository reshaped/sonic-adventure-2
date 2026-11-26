/* a Chao's practices: a bow, a seated laugh, singing and four instruments
 * played without a band. each runs until another behavior replaces it */
#ifndef CHAO_PRACTICE_H
#define CHAO_PRACTICE_H
#include "chao/behavior.h"
#define CHAO_PRACTICES 7

/* chao_practice_names labels each practice. chao_practice_actions holds
 * their behaviors in the same order, then NULL */
extern const char *chao_practice_names[CHAO_PRACTICES];
extern chao_behavior_fn chao_practice_actions[CHAO_PRACTICES + 1];

struct task;
int chao_bow(struct task *chao);
int chao_laugh_seated(struct task *chao);
int chao_practice_singing(struct task *chao);
int chao_practice_maracas(struct task *chao);
int chao_practice_drums(struct task *chao);
int chao_practice_cymbals(struct task *chao);
int chao_practice_tambourine(struct task *chao);
#endif
