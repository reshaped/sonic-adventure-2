/* a garden Chao's type and skills, and the measures that age and shape it */
#ifndef CHAO_LIFE_H
#define CHAO_LIFE_H
#include <sg_xpt.h>

struct task;

/* a Chao's type. the child and each adult form have CHAO_FORM_ALIGNMENTS
 * types in a row. a form's entry names the neutral one, and its hero and dark
 * types follow */
enum chao_type {
    CHAO_NONE = 0,
    CHAO_EGG = 1,
    CHAO_CHILD = 2,
    CHAO_NORMAL_ADULT = 5,
    CHAO_SWIM_ADULT = 8,
    CHAO_FLY_ADULT = 11,
    CHAO_RUN_ADULT = 14,
    CHAO_POWER_ADULT = 17,
    CHAO_CHAOS = 20,
    CHAO_HERO_CHAOS = 21,
    CHAO_DARK_CHAOS = 22
};
#define CHAO_FORM_ALIGNMENTS 3
/* a form's hero and dark types, given its neutral type */
#define CHAO_HERO_TYPE(form) ((form) + 1)
#define CHAO_DARK_TYPE(form) ((form) + 2)
/* the first hero adult type and the first dark one */
#define CHAO_FIRST_HERO_TYPE CHAO_HERO_TYPE(CHAO_NORMAL_ADULT)
#define CHAO_FIRST_DARK_TYPE CHAO_DARK_TYPE(CHAO_NORMAL_ADULT)

enum chao_skill {
    CHAO_SWIM,
    CHAO_FLY,
    CHAO_RUN,
    CHAO_POWER,
    CHAO_LUCK,
    CHAO_INTELLIGENCE,
    CHAO_STAMINA,
    CHAO_SKILLS
};

/* whether a type, or a Chao's, is a hero or a dark one. CHAO_HERO_CHAOS and
 * CHAO_DARK_CHAOS count */
Bool chao_type_is_hero(Uint8 type);
Bool chao_type_is_dark(Uint8 type);
Bool chao_is_hero(const struct task *chao);
Bool chao_is_dark(const struct task *chao);
/* move a living Chao's clocks on by one call. its age, its feelings, its
 * needs and its illnesses each change once their interval in chao_tuning
 * passes. its memories age too */
void chao_age(struct task *chao);
/* a skill's points, or -1 for a skill past CHAO_STAMINA */
int chao_skill_points(const struct task *chao, Uint16 skill);
/* add points to a skill, or twice the points while the skill is below its
 * best. the skill stays from 0 to chao_tuning.skill_limit. its best rises
 * with it */
void chao_add_skill(struct task *chao, Uint16 skill, int points);
/* add delta to the race stamina, which stops at 0 */
void chao_add_race_stamina(struct task *chao, Sint16 delta);
/* add delta to a measure of the Chao's shape, and have its body shaped
 * again. evolution stays from 0 to chao_tuning.evolution_limit. run_power
 * and fly_swim stay within chao_tuning.shape_limit of 0. alignment stays from
 * -1 to 1 */
void chao_add_evolution(struct task *chao, Float delta);
void chao_add_run_power(struct task *chao, Float delta);
void chao_add_fly_swim(struct task *chao, Float delta);
void chao_add_alignment(struct task *chao, Float delta);
/* add delta to the life, which stays from 0 to the lifespan */
void chao_add_life(struct task *chao, int delta);
/* add delta to the lifespan, which stays from 0 to
 * chao_tuning.lifespan_limit. the life stays within the new lifespan */
void chao_add_lifespan(struct task *chao, int delta);
/* add delta to the affection, which stays from -100 to 100. the Chao
 * remembers player 0's character by delta too. it shows CHAO_MARK_HEART for
 * 180 frames where delta is above 0, and CHAO_MARK_ANGER otherwise. for
 * Sonic, Tails, Knuckles or Amy, alignment moves the Chao toward hero. for
 * Shadow, Eggman or Rouge, it moves the Chao toward dark */
void chao_player_affection(struct task *chao, int delta, Float alignment);
/* garden_age_step ages the Chao of one garden slot each
 * chao_tuning.garden_age_interval calls. the slots take turns, the empty ones
 * too. garden_age_reset starts over from slot 0 */
void garden_age_reset(void);
void garden_age_step(void);
/* where the Chao's age timer is 0, start it and return TRUE. otherwise
 * return FALSE */
Bool chao_take_age_tick(struct task *chao);

#endif
