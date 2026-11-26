/* a Chao taking fruit and eating it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/carry.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/fruit.h"
#include "chao/idle.h"
#include "chao/intention.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/perception.h"
#include "chao/race_chao.h"
#include "chao/sleep.h"
#include "chao/sound.h"
#include "chao/tuning.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define TASTE(chao)                                                            \
    (CHAO_WORK(chao)->profile.emotions.personality.preferences.taste)
/* RANDOM_COUNT(least, span)'s number, with the draw narrowed rather than
 * the sum */
#define RANDOM_WHOLE_FROM(least, span)                                         \
    /* @hack type: the random term converted to a Uint16, where (int) would    \
     * do */                                                                   \
    ((least) + (Uint16)(rand() * ((span) * RAND_SCALE)))
#define VOICE(id)                                                              \
    sound_play_at((id), NULL, 0, GARDEN_VOICE_VOLUME, &chao->work->pos)
/* the Chao turns toward an offered fruit by TAKE_TURN a frame. it takes the
 * fruit once it faces it within FACING_FRUIT, and gives up after OFFER_WAIT
 * frames */
#define TAKE_TURN 0x600
#define FACING_FRUIT (FULL_TURN / 360)
#define OFFER_WAIT 60
/* the hunger and gluttony past which a Chao eats hungrily */
#define HUNGRY 8000
#define GLUTTONOUS 50
/* the frames a Chao picking up its target may take before it gives up */
#define PICK_UP_LIMIT 1800
/* the anger and sorrow a Chao feels when its fruit is taken from it */
#define ROBBED_FEELING 200
/* the throw's heading from the Chao's yaw, and its speed across and up */
#define THROW_TURN (FULL_TURN * 160 / 360)
#define THROW_SPEED 0.3F
#define THROW_LIFT 0.7F
/* the frame of CHAO_ANIM_SUTERU_SIT past which the fruit flies */
#define THROW_FRAME 4.0F

enum eating_mode {
    EAT_START,
    EAT_INSPECT,
    EAT_WAIT_TO_TASTE,
    EAT_TASTE,
    EAT_BITE,
    EAT_GOBBLE,
    EAT_FEED,
    EAT_CHEW,
    EAT_SWALLOW,
    EAT_REFUSE,
    EAT_TOSS,
    EAT_SIT,
    EAT_REST,
    EAT_FINISH
};
enum eating_style { EAT_UNDECIDED, EAT_HUNGRILY, EAT_NORMALLY };

#define REFUSE_FRUIT(chao, behavior)                                           \
    {                                                                          \
        (behavior)->mode = EAT_REFUSE;                                         \
        chao_player_affection((chao), -1,                                      \
                              -chao_tuning.disliked_fruit_affection_loss);     \
        VOICE(0x603a);                                                         \
    }
#define CHOOSE_CHEW_TIME(behavior)                                             \
    {                                                                          \
        if (RANDOM_UNIT() < 0.3F) {                                            \
            (behavior)->count = 10;                                            \
        } else {                                                               \
            (behavior)->count = RANDOM_WHOLE_FROM(40, 200);                    \
        }                                                                      \
    }

static int take_offered_fruit(struct task *chao);

int chao_try_take_fruit(struct task *chao)
{
    chao_behavior_fn action = chao_current_behavior(chao);
    struct task *fruit;
    if (action != chao_eat_fruit_behavior && action != chao_sleep_behavior &&
        action != take_offered_fruit) {
        if ((fruit = collision_first_with_id(chao, GARDEN_FRUIT_HIT)) != NULL &&
            !garden_object_locked(fruit)) {
            mover_set_target(chao, &fruit->work->pos);
            chao_set_behavior(chao, take_offered_fruit);
            return TRUE;
        }
    }
    return FALSE;
}
static int take_offered_fruit(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task *fruit;
    /* @hack spelling: a one-case switch on mode, where an if would do */
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_CAPTURE_MORAU, 15);
        behavior->mode++;
        behavior->timer = OFFER_WAIT;
        break;
    }
    if (mover_turn_to_target_by(chao, TAKE_TURN) < FACING_FRUIT) {
        fruit = collision_first_with_id(chao, GARDEN_FRUIT_HIT);
        if (fruit) {
            if (!garden_object_locked(fruit)) {
                player_let_go(0);
                chao_take_in_both_hands(chao, fruit);
                chao_set_behavior(chao, chao_eat_fruit_behavior);
            }
        } else if (behavior->timer-- <= 0) {
            VOICE(0x6037);
            return BEHAVIOR_FINISH;
        }
    }
    return BEHAVIOR_CONTINUE;
}

int chao_eat_fruit_behavior(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    switch (behavior->mode) {
    case EAT_START:
        chao_link_motion(chao, CHAO_ANIM_TABERU_SUWARU);
        behavior->mode = EAT_INSPECT;
        behavior->count = RANDOM_WHOLE_FROM(100, 300);
        /* fall through */
    case EAT_INSPECT:
        if (chao_motion_ended(chao)) {
            struct garden_entry *entry =
                garden_partner_in_category(chao, GARDEN_OBJECT_FRUIT);
            struct task *fruit = NULL;
            if (entry) {
                fruit = entry->task;
            }
            if (chao_emotion_get(chao, CHAO_GLUTTONY) >= 0 ||
                chao_emotion_get(chao, CHAO_CURIOSITY) >= 0 ||
                chao_emotion_get(chao, CHAO_HUNGER) > 5000) {
                if (RANDOM_UNIT() > 0.5F) {
                    chao_link_motion(chao, CHAO_ANIM_EAT_KUNKUN);
                    behavior->mode = EAT_WAIT_TO_TASTE;
                    behavior->count = 120;
                } else {
                    behavior->mode = EAT_BITE;
                }
            } else {
                behavior->mode = EAT_BITE;
            }
            if (fruit) {
                int kind = garden_fruit_kind(fruit);
                switch (kind) {
                case GARDEN_FRUIT_HERO_TREE_1:
                case GARDEN_FRUIT_HERO_TREE_2:
                case GARDEN_FRUIT_HERO_TREE_3:
                case GARDEN_FRUIT_HERO:
                    if (chao_is_dark(chao)) {
                        REFUSE_FRUIT(chao, behavior);
                    }
                    break;
                case GARDEN_FRUIT_DARK_TREE_1:
                case GARDEN_FRUIT_DARK_TREE_2:
                case GARDEN_FRUIT_DARK_TREE_3:
                case GARDEN_FRUIT_DARK:
                    if (chao_is_hero(chao)) {
                        REFUSE_FRUIT(chao, behavior);
                    }
                    break;
                case GARDEN_FRUIT_ROUND: {
                    int taste = TASTE(chao);
                    switch (taste) {
                    case 2:
                    case 4:
                        REFUSE_FRUIT(chao, behavior);
                    }
                    break;
                }
                case GARDEN_FRUIT_TRIANGLE: {
                    int taste = TASTE(chao);
                    switch (taste) {
                    case 0:
                    case 5:
                        REFUSE_FRUIT(chao, behavior);
                    }
                    break;
                }
                case GARDEN_FRUIT_SQUARE: {
                    int taste = TASTE(chao);
                    switch (taste) {
                    case 1:
                    case 3:
                        REFUSE_FRUIT(chao, behavior);
                    }
                    break;
                }
                }
            }
        }
        break;
    case EAT_WAIT_TO_TASTE:
        if (behavior->count > 0) {
            --behavior->count;
        } else if (chao_motion_ended(chao)) {
            behavior->count = 60;
            chao_link_motion(chao, CHAO_ANIM_EAT_KUNKUN_UPPER);
            chao_set_eye(chao, CHAO_EYE_GLAD);
            behavior->mode = EAT_TASTE;
            VOICE(0x6039);
        }
        break;
    case EAT_TASTE:
        if (behavior->count > 0) {
            --behavior->count;
        } else if (chao_motion_ended(chao)) {
            behavior->mode = EAT_BITE;
        }
        break;
    case EAT_BITE:
        switch (chao_behavior_value(chao)) {
        case EAT_UNDECIDED:
            if (chao_emotion_get(chao, CHAO_GLUTTONY) > GLUTTONOUS ||
                chao_emotion_get(chao, CHAO_HUNGER) > HUNGRY) {
                chao_link_motion(chao, CHAO_ANIM_EAT_GATUGATU);
                chao_set_behavior_value(chao, EAT_HUNGRILY);
                VOICE(0x603c);
            } else {
                chao_link_motion(chao, CHAO_ANIM_EAT);
                chao_set_behavior_value(chao, EAT_NORMALLY);
                VOICE(0x603b);
            }
            break;
        case EAT_NORMALLY:
            chao_link_motion(chao, CHAO_ANIM_EAT);
            VOICE(0x603b);
            break;
        case EAT_HUNGRILY:
            chao_link_motion(chao, CHAO_ANIM_EAT_GATUGATU);
            VOICE(0x603c);
            break;
        }
        behavior->mode = EAT_FEED;
        chao_set_eye(chao, CHAO_EYE_GLAD);
        /* fall through */
    case EAT_FEED:
        garden_command_partner(chao, PARTNER_USE);
        if (garden_take_command(chao) == PARTNER_USED_UP) {
            behavior->count = RANDOM_WHOLE_FROM(180, 200);
            chao_link_motion_frames(chao, CHAO_ANIM_MANPUKU_LOOP, 60);
            behavior->mode = EAT_SIT;
            garden_separate_partners(chao);
            chao_set_eye(chao, CHAO_EYE_GLAD);
            chao_set_mouth(chao, 2);
            VOICE(0x6042);
            break;
        }
        if (--behavior->count <= 0) {
            if (chao_emotion_get(chao, CHAO_GLUTTONY) > GLUTTONOUS &&
                RANDOM_UNIT() < 0.4F) {
                chao_link_motion(chao, CHAO_ANIM_EAT_TUMARU);
                behavior->count = RANDOM_WHOLE_FROM(2, 5);
                behavior->mode = EAT_GOBBLE;
                VOICE(0x603d);
                chao_set_eye(chao, CHAO_EYE_DISTRESSED);
            } else {
                if (RANDOM_UNIT() < 0.5F) {
                    chao_link_motion(chao, CHAO_ANIM_EAT_LOOKUP_L);
                } else {
                    chao_link_motion(chao, CHAO_ANIM_EAT_LOOKUP_R);
                }
                CHOOSE_CHEW_TIME(behavior);
                chao_set_eye(chao, CHAO_EYE_OPEN);
                behavior->mode = EAT_CHEW;
            }
        }
        if (chao_take_upkeep_tick(chao)) {
            chao_emotion_add(chao, CHAO_HUNGER,
                             -chao_tuning.feeding_hunger_loss);
            chao_emotion_add(chao, CHAO_TIREDNESS,
                             -chao_tuning.feeding_hunger_loss);
            if (chao_emotion_get(chao, CHAO_HUNGER) <= 0) {
                behavior->mode = EAT_REFUSE;
            }
        }
        break;
    case EAT_GOBBLE:
        if (chao_motion_ended(chao) && --behavior->count <= 0) {
            chao_link_motion(chao, CHAO_ANIM_TABERU_MODORU);
            behavior->mode = EAT_SWALLOW;
        }
        break;
    case EAT_CHEW:
        if (chao_motion_ended(chao) && --behavior->count <= 0) {
            if (RANDOM_UNIT() < 0.65F) {
                chao_link_motion(chao, CHAO_ANIM_TABERU_MODORU);
                behavior->mode = EAT_SWALLOW;
            } else {
                switch (chao_motion_index(chao)) {
                case CHAO_ANIM_EAT_LOOKUP_L:
                    chao_link_motion_frames(chao, CHAO_ANIM_EAT_LOOKUP_R, 25);
                    break;
                case CHAO_ANIM_EAT_LOOKUP_R:
                    chao_link_motion_frames(chao, CHAO_ANIM_EAT_LOOKUP_L, 25);
                    break;
                }
                CHOOSE_CHEW_TIME(behavior);
            }
        }
        break;
    case EAT_SWALLOW:
        if (chao_motion_ended(chao)) {
            behavior->mode = EAT_BITE;
            behavior->count = RANDOM_WHOLE_FROM(180, 400);
        }
        break;
    case EAT_REFUSE:
        chao_link_motion(chao, CHAO_ANIM_SUTERU_SIT);
        behavior->mode = EAT_TOSS;
        /* fall through */
    case EAT_TOSS:
        if (chao_motion_index(chao) == CHAO_ANIM_SUTERU_SIT &&
            chao_motion_frame(chao) > THROW_FRAME) {
            struct garden_entry *entry = garden_mutual_partner(chao);
            behavior->mode = EAT_FINISH;
            if (entry) {
                struct task *fruit = entry->task;
                /* @hack redundant: fruit_work and x, which no code reads */
                struct task_work *fruit_work = fruit->work;
                Float x = chao->work->pos.x;
                Angle throw_angle = chao->work->ang.y + THROW_TURN;
                NJS_VECTOR velocity;
                (void)fruit_work;
                (void)x;
                velocity.x = njSin(throw_angle) * THROW_SPEED;
                velocity.y = THROW_LIFT;
                velocity.z = njCos(throw_angle) * THROW_SPEED;
                mover_set_spd(fruit, &velocity);
            }
            garden_separate_partners(chao);
        }
        break;
    case EAT_SIT:
        if (behavior->count-- <= 0) {
            if (RANDOM_UNIT() < 0.6F) {
                chao_link_motion(chao, CHAO_ANIM_MANPUKU_SURI);
                behavior->mode = EAT_REST;
                behavior->timer = 0;
                behavior->count = 0;
                chao_set_eye(chao, CHAO_EYE_GLAD);
                chao_set_mouth(chao, 2);
            } else if (RANDOM_UNIT() < 0.2F) {
                chao_link_motion(chao, CHAO_ANIM_STANDUP);
                behavior->mode = EAT_FINISH;
            } else {
                chao_clear_intention(chao);
                return BEHAVIOR_FINISH;
            }
        }
        break;
    case EAT_REST:
        if (chao_motion_ended(chao)) {
            chao_link_motion(chao, CHAO_ANIM_MANPUKU_LOOP);
            behavior->mode = EAT_SIT;
            behavior->count = RANDOM_WHOLE_FROM(60, 100);
            chao_set_eye(chao, CHAO_EYE_OPEN);
            chao_set_mouth(chao, CHAO_MOUTH_GLAD);
        }
        break;
    case EAT_FINISH:
        if (chao_motion_held(chao)) {
            chao_clear_intention(chao);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (!chao_place_held_item(chao)) {
        if (garden_take_command(chao) == PARTNER_STOLEN) {
            chao_player_affection(chao, -1,
                                  -chao_tuning.stolen_fruit_affection_loss);
            chao_emotion_add(chao, CHAO_ANGER, ROBBED_FEELING);
            chao_emotion_add(chao, CHAO_SORROW, ROBBED_FEELING);
        }
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_pick_up_target_behavior(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HIROU_KAGAMU);
        sound_play_at(0x1001, NULL, 0, 0, &chao->work->pos);
        behavior->limit = PICK_UP_LIMIT;
        ++behavior->mode;
        break;
    case 1:
        if (chao_motion_ended(chao)) {
            struct task *target = garden_target_object(chao);
            if (!garden_paired_elsewhere(chao, target)) {
                chao_take_in_both_hands(chao, target);
                chao_place_held_item(chao);
                ++behavior->mode;
                behavior->timer = 0;
                behavior->count = 0;
            }
        }
        /* fall through */
    case 2:
        chao_place_held_item(chao);
        if (chao_motion_ended(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    if (--behavior->limit <= 0) {
        return BEHAVIOR_BREAK;
    }
    return BEHAVIOR_CONTINUE;
}

int chao_seek_fruit_behavior(struct task *chao)
{
    struct task *fruit = chao_nearest_fruit_task(chao);
    if (fruit) {
        if (garden_paired_elsewhere(chao, fruit)) {
#ifdef NONMATCHING
            /* the plain form the hack tag names. GCC refuses a return with no
             * value in a function that returns one */
            return BEHAVIOR_FINISH;
#else
            /* @hack implicit: a bare return, whose result is
             * garden_paired_elsewhere's TRUE, where return BEHAVIOR_FINISH
             * would do */
            return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
        }
        garden_set_target(chao, fruit);
        chao_set_behavior(chao, chao_stand_up_behavior);
        chao_queue_behavior(chao, chao_pause_with_mark_behavior);
        chao_queue_behavior(chao, chao_walk_to_target_behavior);
        chao_queue_behavior(chao, chao_pick_up_target_behavior);
        chao_queue_behavior(chao, chao_eat_fruit_behavior);
        return BEHAVIOR_CONTINUE;
    }
    return BEHAVIOR_FINISH;
}
