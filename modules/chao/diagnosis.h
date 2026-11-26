/* the Kindergarten checkup's findings on a Chao. a finding is the variant
 * of its group of the checkup's messages, or -1 for none */
#ifndef CHAO_DIAGNOSIS_H
#define CHAO_DIAGNOSIS_H

struct task;

/* 0 or 1 for hat 1 or 2, and -1 for hat 0 or 3. a higher hat's result is
 * undefined */
int chao_headgear_comment(const struct task *chao);
/* cure the first illness the Chao has, and return its variant, or -1 where
 * it has none. the cure sets the illness's count to 100. the Chao can't
 * catch it again until the count runs down to 0 */
int chao_treat_illness(struct task *chao);
/* a skill's rating, from 0 below 500 points to 4 at 5000 or more */
int chao_swim_rating(const struct task *chao);
int chao_fly_rating(const struct task *chao);
int chao_run_rating(const struct task *chao);
int chao_power_rating(const struct task *chao);
int chao_stamina_rating(const struct task *chao);
/* the Chao's form: 0 for an egg and 1 for a child. a normal, swim, fly, run or
 * power adult of any alignment gives 2 to 6. a Chaos, a Hero Chaos and a Dark
 * Chaos give 7 to 9 */
int chao_form_comment(const struct task *chao);
/* the finding for one trait past its limit, chosen at random, or -1 where
 * none is */
int chao_personality_comment(const struct task *chao);
/* chao_personality_comment, for the needs and feelings */
int chao_condition_comment(const struct task *chao);
/* 0, 1 or 2 for each pair of tastes from 0 to 5. a higher taste's result is
 * undefined */
int chao_taste_comment(const struct task *chao);
/* 0 for a child, else 1 below age 240 and 2 from it */
int chao_age_comment(const struct task *chao);
/* the times the Chao has been reborn */
int chao_reincarnation_count(const struct task *chao);
/* 0 below 3 rebirths, 1 below 10 and 2 from 10 */
int chao_reincarnation_comment(const struct task *chao);
/* 0 where affection is above 50 or below -50 */
int chao_affection_comment(const struct task *chao);
/* the Chao's opinion of the character each function names, from 0 above 60
 * affinity to 4 at -61 or below */
int chao_sonic_opinion(const struct task *chao);
int chao_shadow_opinion(const struct task *chao);
int chao_tails_opinion(const struct task *chao);
int chao_eggman_opinion(const struct task *chao);
int chao_knuckles_opinion(const struct task *chao);
int chao_rouge_opinion(const struct task *chao);

#endif
