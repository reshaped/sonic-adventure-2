/* the checkup's findings, as diagnosis.h declares them, and the calls of
 * classroom.h */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/classroom.h"
#include "chao/diagnosis.h"
#include "chao/emotion.h"
#include "chao/garden_save.h"
#include "chao/life.h"
#include "chao/residents.h"
#include "chao/work.h"
#include "player/player.h"
#include "save/save.h"
#include "system/task.h"
#include "system/util.h"

#define PARAMETERS(chao) (&CHAO_WORK(chao)->profile.parameters)
#define NO_COMMENT (-1)
/* the count a cure sets. the Chao can't catch the illness again until the count
 * runs down to 0 */
#define CURE_IMMUNITY 100

int chao_headgear_comment(const struct task *chao)
{
    const struct chao_parameters *parameters = PARAMETERS(chao);
    switch (parameters->hat) {
    case 0:
    case 3:
        return NO_COMMENT;
    case 1:
        return 0;
    case 2:
        return 1;
    }
    /* @bug uninitialized: a hat above 3 returns no variant, and the caller
     * reads the hat left in R0 (0x0c50efac) */
#ifdef NONMATCHING
    /* this side keeps the tag's result: it returns the hat, which the image
     * leaves in R0 */
    return parameters->hat;
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-return-type) */
}

int chao_treat_illness(struct task *chao)
{
    struct chao_emotions *emotions = &CHAO_WORK(chao)->profile.emotions;
    int i;
    for (i = 0; i < CHAO_ILLNESSES; i++) {
        if (emotions->illness[i] < 0) {
            emotions->illness[i] = CURE_IMMUNITY;
            switch (i) {
            case 0:
                return 3;
            case 1:
                return 0;
            case 2:
                return 4;
            case 3:
                return 2;
            case 4:
                return 5;
            case 5:
                return 1;
            }
        }
    }
    return NO_COMMENT;
}

static int skill_rating(Uint16 points)
{
    if (points < 500) {
        return 0;
    } else if (points < 1500) {
        return 1;
    } else if (points < 3000) {
        return 2;
    } else if (points < 5000) {
        return 3;
    } else {
        return 4;
    }
}

int chao_swim_rating(const struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_SWIM);
    return skill_rating(points);
}
int chao_fly_rating(const struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_FLY);
    return skill_rating(points);
}
int chao_run_rating(const struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_RUN);
    return skill_rating(points);
}
int chao_power_rating(const struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_POWER);
    return skill_rating(points);
}
int chao_stamina_rating(const struct task *chao)
{
    Uint16 points = chao_skill_points(chao, CHAO_STAMINA);
    return skill_rating(points);
}

int chao_form_comment(const struct task *chao)
{
    const struct chao_parameters *parameters = PARAMETERS(chao);
    switch (parameters->type) {
    case CHAO_EGG:
        return 0;
    case CHAO_CHILD:
        return 1;
    case CHAO_NORMAL_ADULT:
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):
        return 2;
    case CHAO_SWIM_ADULT:
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
    case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):
        return 3;
    case CHAO_FLY_ADULT:
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
    case CHAO_DARK_TYPE(CHAO_FLY_ADULT):
        return 4;
    case CHAO_RUN_ADULT:
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
    case CHAO_DARK_TYPE(CHAO_RUN_ADULT):
        return 5;
    case CHAO_POWER_ADULT:
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
    case CHAO_DARK_TYPE(CHAO_POWER_ADULT):
        return 6;
    case CHAO_CHAOS:
        return 7;
    case CHAO_HERO_CHAOS:
        return 8;
    case CHAO_DARK_CHAOS:
        return 9;
    }
    return NO_COMMENT;
}

/* where test holds, a random draw above best makes comment the result.
 * each comment whose test holds is then as likely as the others */
#define CONSIDER(test, comment)                                                \
    {                                                                          \
        if (test) {                                                            \
            candidate = RANDOM_UNIT();                                         \
            if (candidate > best) {                                            \
                result = (comment);                                            \
                best = candidate;                                              \
            }                                                                  \
        }                                                                      \
    }
int chao_personality_comment(const struct task *chao)
{
    /* @hack redundant: work, which no code reads */
    struct chao_work *work = CHAO_WORK(chao);
    int result = NO_COMMENT;
    Float best = 0.0F;
    Float candidate;
    (void)work;
    if (chao_emotion_get(chao, CHAO_AGGRESSION) > 45) {
        best = RANDOM_UNIT();
        result = 2;
    }
    CONSIDER(chao_emotion_get(chao, CHAO_GLUTTONY) > 45, 4);
    CONSIDER(chao_emotion_get(chao, CHAO_CURIOSITY) > 45, 7);
    CONSIDER(chao_emotion_get(chao, CHAO_CALM) > 45, 8);
    CONSIDER(chao_emotion_get(chao, CHAO_AGGRESSION) < -45, 11);
    CONSIDER(chao_emotion_get(chao, CHAO_AGGRESSION) > 45 &&
                 chao_emotion_get(chao, CHAO_KINDNESS) < -45,
             12);
    CONSIDER(chao_emotion_get(chao, CHAO_SLEEPY_HEAD) < -40, 16);
    return result;
}
int chao_condition_comment(const struct task *chao)
{
    int result = NO_COMMENT;
    Float best = 0.0F;
    Float candidate;
    if (chao_emotion_get(chao, CHAO_HUNGER) > 7000) {
        best = RANDOM_UNIT();
        result = 1;
    }
    CONSIDER(chao_emotion_get(chao, CHAO_SLEEPINESS) > 7000, 0);
    CONSIDER(chao_emotion_get(chao, CHAO_BOREDOM) > 9000, 2);
    CONSIDER(chao_emotion_get(chao, CHAO_TIREDNESS) > 7000, 4);
    CONSIDER(chao_emotion_get(chao, CHAO_PLEASURE) > 100, 6);
    CONSIDER(chao_emotion_get(chao, CHAO_PAIN) > 30, 7);
    return result;
}
int chao_taste_comment(const struct task *chao)
{
    const struct chao_emotions *emotions = &CHAO_WORK(chao)->profile.emotions;
    switch (emotions->personality.preferences.taste) {
    case 0:
    case 1:
        return 0;
    case 2:
    case 3:
        return 1;
    case 4:
    case 5:
        return 2;
    }
    /* a taste above 5 sets no result, and SHC's code leaves the taste in R0.
     * the one caller drops the result */
#ifdef NONMATCHING
    /* this side returns the taste, which SHC's code leaves in R0 */
    return emotions->personality.preferences.taste;
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-return-type) */
}
int chao_age_comment(const struct task *chao)
{
    const struct chao_parameters *parameters = PARAMETERS(chao);
    /* @hack spelling: a switch of one case and a default, where an if would
     * do */
    switch (parameters->type) {
    case CHAO_CHILD:
        return 0;
    default:
        break;
    }
    if (parameters->age < 240) {
        return 1;
    }
    return 2;
}
int chao_reincarnation_count(const struct task *chao)
{
    const struct chao_parameters *parameters = PARAMETERS(chao);
    return parameters->reincarnations;
}
int chao_reincarnation_comment(const struct task *chao)
{
    int lives = chao_reincarnation_count(chao);
    if (lives < 3) {
        return 0;
    } else if (lives < 10) {
        return 1;
    } else {
        return 2;
    }
}
int chao_affection_comment(const struct task *chao)
{
    const struct chao_parameters *parameters = PARAMETERS(chao);
    int affection = parameters->affection;
    if (affection > 50) {
        return 0;
    } else if (affection < -50) {
        return 0;
    }
    return NO_COMMENT;
}
static int player_opinion(const struct task *chao, int character)
{
    const struct chao_knowledge *knowledge =
        &CHAO_WORK(chao)->profile.knowledge;
    Sint8 affinity = knowledge->players[character].affinity;
    if (affinity > 60) {
        return 0;
    } else if (affinity > 20) {
        return 1;
    } else if (affinity > -21) {
        return 2;
    } else if (affinity > -61) {
        return 3;
    } else {
        return 4;
    }
}
int chao_sonic_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_SONIC);
}
int chao_shadow_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_SHADOW);
}
int chao_tails_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_TAILS);
}
int chao_eggman_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_EGGMAN);
}
int chao_knuckles_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_KNUCKLES);
}
int chao_rouge_opinion(const struct task *chao)
{
    return player_opinion(chao, PLAYER_ROUGE);
}

Sint32 garden_play_time(void)
{
    return save_data.play_time;
}
int chao_set_classroom_slot(struct task *chao, int slot)
{
    struct chao_work *work = CHAO_WORK(chao);
    if (work->save) {
        work->save->record.saved.parameters.classroom_slot = slot;
        work->profile.parameters.classroom_slot = slot;
        return TRUE;
    }
    return FALSE;
}
struct task *garden_spawn_classmate(int slot, NJS_POINT3 *position, Angle yaw)
{
    int count = garden_save_slots();
    struct chao_save *cursor;
    struct chao_save *save;
    struct chao_save *record;
    Sint8 classroom_slot;
    for (cursor = garden_saved_chao; cursor < &garden_saved_chao[count];
         cursor++) {
        /* @hack redundant: record and save, two more names for cursor, where
         * cursor alone would do */
        record = save = cursor;
        classroom_slot = record->record.saved.parameters.classroom_slot;
        switch (save->record.saved.parameters.type) {
        case CHAO_NONE:
        case CHAO_EGG:
            break;
        default:
            if (classroom_slot == slot) {
                return garden_spawn_chao(cursor, position, yaw);
            }
            break;
        }
    }
    /* @bug uninitialized: where no saved Chao has the slot, the call returns
     * no task, and the caller takes R0 for one (0x0c50f4fe) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#else
    /* NOLINTNEXTLINE(clang-diagnostic-return-mismatch) */
    return;
#endif
}
