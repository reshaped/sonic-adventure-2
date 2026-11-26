#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/carry.h"
#include "chao/face.h"
#include "chao/emotion.h"
#include "chao/garden_field.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/planting.h"
#include "chao/small_toys.h"
#include "chao/sound.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/mover.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

static struct chao_placement chao_shovel_attachment = {
#include "chao/plant/chao_shovel_attachment.inc"
};
static struct chao_placement chao_watering_can_attachment = {
#include "chao/plant/chao_watering_can_attachment.inc"
};

/* behavior->mode in plant_seed */
enum plant_mode {
    PLANT_START,
    PLANT_READY,
    PLANT_DIG,
    PLANT_SOW,
    PLANT_BURY,
    PLANT_FINISH
};
/* behavior->mode in chao_prepare_to_water */
enum water_mode {
    WATER_START,
    WATER_THINK,
    WATER_IDEA,
    WATER_BACK,
    WATER_POUR,
    WATER_END
};
static int find_planting_spot(struct task *chao);
static int plant_seed(struct task *chao);

int chao_examine_seed(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_HATENA_HATE2, 15);
        chao_set_timed_eye(chao, CHAO_EYE_OPEN, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_QUESTION, 300);
        sound_play_at(GARDEN_QUESTION_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                      &chao->work->pos);
        chao_hold_at_hand_4(chao);
        behavior->timer = RANDOM_COUNT(300, 121);
        behavior->mode++;
        break;
    }
    if (--behavior->timer <= 0) {
        chao_set_behavior(chao, find_planting_spot);
    }
    return !chao_place_held_item(chao);
}

static int find_planting_spot(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct mover *mover = chao->motion.p;
    struct task *plot;
    Float speed;
    int found;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_HAKOBU, 15);
        chao_set_timed_eye(chao, CHAO_EYE_OPEN, CHAO_FACE_HOLD);
        chao_hold_in_both_hands(chao);
        behavior->limit = RANDOM_COUNT(1800, 401);
        behavior->mode++;
        chao_pick_wander_target(chao);
        break;
    case 1:
        found = FALSE;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((plot = garden_hit_kind(chao, CHAO_PUSH_SHAPE,
                                    GARDEN_EMPTY_PLOT_HIT)) != NULL) {
            if (njDistanceP2P(&plot->work->pos, &work->base.pos) <
                (chao_skill_points(chao, CHAO_INTELLIGENCE) * 0.001F) + 20.0F) {
                garden_set_target(chao, plot);
                behavior->mode++;
                found = TRUE;
                sound_play_at(GARDEN_IDEA_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                              &chao->work->pos);
            }
        }
        if (!found) {
            mover_turn_to_target_by(chao, 0x100);
            if (chao_target_distance_squared(chao) < 64.0F) {
                if (RANDOM_UNIT() < 0.7F) {
                    chao_set_behavior(chao, chao_examine_seed);
                }
            }
        }
        break;
    case 2:
        garden_turn_to_target(chao, 640);
        if (garden_target_distance(chao) < 2.5F) {
            chao_set_behavior(chao, plant_seed);
        }
        break;
    }
    if (--behavior->limit <= 0) {
        return BEHAVIOR_FINISH;
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->base.ang.y) * chao_tuning.walk_speed * 0.8F;
        mover->acc.z = njCos(work->base.ang.y) * chao_tuning.walk_speed * 0.8F;
    }
    speed = njScalor(&mover->spd);
    chao_set_motion_speed(chao, speed * 18.0F);
    if (!chao_place_held_item(chao)) {
        return BEHAVIOR_FINISH;
    }
    return BEHAVIOR_CONTINUE;
}

static int plant_seed(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct garden_entry *entry;
    struct task *seed;
    struct task *plot;
    struct garden_entry *tree;
    Sint16 kind;

    switch (behavior->mode) {
    case PLANT_START:
        chao_link_motion_frames(chao, CHAO_ANIM_STAND, 15);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 180);
        chao_hold_at_hand_4(chao);
        chao_resist_displacement(chao);
        behavior->timer = RANDOM_COUNT(120, 121);
        behavior->mode++;
        break;
    case PLANT_READY:
        garden_turn_to_target(chao, 0x100);
        if (behavior->timer-- <= 0) {
            if (!chao_has_small_toy(chao, CHAO_SHOVEL_TOY)) {
                chao_show_mark(chao, CHAO_MARK_ANGER, 180);
                return BEHAVIOR_FINISH;
            } else {
                chao_link_motion_frames(chao, CHAO_ANIM_SEED_UERU_START, 15);
                chao_shovel_attachment.object = &chao_shovel_model;
                chao_equip_node_at(chao, CHAO_NODE_HAND_11,
                                   &chao_shovel_attachment);
                behavior->timer = RANDOM_COUNT(180, 121);
                behavior->mode++;
            }
        }
        break;
    case PLANT_DIG:
        if (behavior->timer-- <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_SEED_UERU_UERU, 15);
            behavior->mode++;
            behavior->timer = 0;
            behavior->count = 0;
        }
        break;
    case PLANT_SOW:
        if (behavior->timer++ % 35 == 0) {
            sound_play_at(0x1023, NULL, 0, -10, &chao->work->pos);
        }
        if (chao_motion_ended(chao)) {
            entry = garden_partner_in_category(chao, GARDEN_OBJECT_SEED);
            if (entry && entry->task) {
                seed = entry->task;
                kind = SEED_KIND(seed->work);
                plot = garden_hit_kind(chao, CHAO_PUSH_SHAPE,
                                       GARDEN_EMPTY_PLOT_HIT);
                if (plot) {
                    tree = plot->force.p;
                    plot->work->ang.y = seed->work->ang.y;
                    if (tree) {
                        garden_send_entry_state(tree, PLANT_SEED, kind);
                    }
                    garden_separate_partners(chao);
                    garden_set_target(chao, plot);
                }
                task_kill(entry->task);
            }
            chao_link_motion_frames(chao, CHAO_ANIM_SEED_UERU_UMERU_START, 15);
            behavior->timer = RANDOM_COUNT(180, 181);
            behavior->mode++;
        }
        break;
    case PLANT_BURY:
        if (behavior->timer % 35 == 0) {
            sound_play_at(0x1024, NULL, 0, -10, &chao->work->pos);
        }
        if (behavior->timer % 60 == 0) {
            if (RANDOM_UNIT() < 0.5F) {
                sound_play_at(0x6085, NULL, 0, 100, &chao->work->pos);
            }
        }
        if (behavior->timer-- <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_SEED_UERU_END, 15);
            behavior->mode++;
        }
        break;
    case PLANT_FINISH:
        if (chao_motion_held(chao)) {
            chao_set_behavior(chao, chao_prepare_to_water);
        }
        break;
    }
    chao_place_held_item(chao);
    return BEHAVIOR_CONTINUE;
}

static int water_plant(struct task *chao);

int chao_approach_plant(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct mover *mover = chao->motion.p;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_ARUKU, 15);
        chao_set_timed_eye(chao, CHAO_EYE_OPEN, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 300);
        behavior->limit = RANDOM_COUNT(1800, 121);
        behavior->mode++;
        break;
    }
    if (--behavior->limit <= 0) {
        return BEHAVIOR_FINISH;
    }
    garden_turn_to_target(chao, 640);
    if (garden_target_distance(chao) < 4.0F) {
        chao_set_behavior(chao, chao_prepare_to_water);
    }
    if (mover->flags & MOVER_ON_FLOOR) {
        mover->acc.x = njSin(work->base.ang.y) * chao_tuning.walk_speed * 0.8F;
        mover->acc.z = njCos(work->base.ang.y) * chao_tuning.walk_speed * 0.8F;
    }
    chao_set_motion_speed(chao, njScalor(&mover->spd) * 18.0F);
    return BEHAVIOR_CONTINUE;
}

int chao_prepare_to_water(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case WATER_START:
        chao_link_motion_frames(chao, CHAO_ANIM_OSYABURI, 15);
        behavior->timer = RANDOM_COUNT(60, 61);
        chao_resist_displacement(chao);
        behavior->mode++;
        break;
    case WATER_THINK:
        if (--behavior->timer <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_HIRAMEKI, 15);
            chao_show_mark(chao, CHAO_MARK_EXCLAMATION, 120);
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
            sound_play_at(GARDEN_IDEA_VOICE, NULL, 0, GARDEN_VOICE_VOLUME,
                          &chao->work->pos);
            behavior->timer = RANDOM_COUNT(60, 41);
            if (!chao_has_small_toy(chao, CHAO_WATERING_CAN_TOY)) {
                chao_emotion_add(chao, CHAO_THIRST, 2000);
                return BEHAVIOR_FINISH;
            }
            behavior->mode++;
        }
        break;
    case WATER_IDEA:
        if (--behavior->timer <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_ARUKU, 15);
            behavior->timer = 150;
            behavior->mode++;
        }
        break;
    case WATER_BACK:
        garden_move_forward(chao, -0.015F);
        garden_turn_to_target(chao, 0x200);
        if (--behavior->timer <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_JYOURO_START, 15);
            chao_watering_can_attachment.object = &chao_watering_can_model;
            chao_equip_node_at(chao, CHAO_NODE_HAND_11,
                               &chao_watering_can_attachment);
            behavior->timer = RANDOM_COUNT(240, 101);
            behavior->mode++;
        }
        break;
    case WATER_POUR:
        if (--behavior->timer <= 0) {
            chao_link_motion_frames(chao, CHAO_ANIM_JYOURO_END, 15);
            behavior->mode++;
        }
        break;
    case WATER_END:
        if (chao_motion_ended(chao)) {
            chao_emotion_add(chao, CHAO_THIRST, 2000);
            chao_set_behavior(chao, water_plant);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int water_plant(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    struct task_work *work;
    Angle yaw;
    NJS_POINT3 position;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_TREE_DANCE_NYOKI, 15);
        chao_resist_displacement(chao);
        behavior->mode++;
        break;
    case 1:
        if (chao_motion_held(chao)) {
            chao_link_motion_frames(chao, CHAO_ANIM_TREE_DANCE_NYOKI, 15);
            behavior->mode++;
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            work = chao->work;
            yaw = work->ang.y;
            position.x = (njSin(yaw) * 4.0F) + work->pos.x;
            position.y = work->pos.y;
            position.z = (njCos(yaw) * 4.0F) + work->pos.z;
            garden_timed_enemy_field(GARDEN_WATER_HIT, &position, 5.0F, 10);
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
