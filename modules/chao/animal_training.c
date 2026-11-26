/* a Chao that takes in an animal or a Chaos Drive held in both hands */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/animal_training.h"
#include "chao/behavior.h"
#include "chao/carry.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/parts.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "hud/item_stock.h"
#include "object/animal.h"
#include "object/chaos_drive.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define TRAINING_ITEM_HIT 135
#define PARAMETERS(chao) (CHAO_WORK(chao)->profile.parameters)
/* an item's work keeps the item, as hud/item_stock.h numbers it, in tick */
#define ITEM(work) ((work)->tick)
/* the Chao turns toward an item by TAKE_TURN a frame. it takes the item once it
 * faces it within FACING_ITEM */
#define TAKE_TURN 0x600
#define FACING_ITEM (FULL_TURN / 36)
/* the spiral's work. RISE is its height above the Chao */
#define RISE(work) ((work)->pos.y)
#define SPIRAL_RADIUS 2.3F
#define SPIRAL_FRAMES 80
#define SPIRAL_STEP (FULL_TURN / 12)
#define SPIRAL_CLIMB 0.06F
/* the ring of sparkles the Chao gives off as it takes the item in */
#define RING_SPARKLES 40
#define RING_STEP (FULL_TURN / RING_SPARKLES)
#define RING_END 0xfff0

static const NJS_VECTOR still = {0.0F, 0.0F, 0.0F};
static int take_training_item(struct task *chao);
static int absorb_training_item(struct task *chao);

Bool chao_try_animal(struct task *chao)
{
    chao_behavior_fn current = chao_current_behavior(chao);
    struct task *item;
    if (current != absorb_training_item && current != take_training_item) {
        if ((item = collision_first_with_id(chao, TRAINING_ITEM_HIT)) != NULL &&
            !garden_object_locked(item)) {
            mover_set_target(chao, &item->work->pos);
            chao_set_behavior(chao, take_training_item);
            return TRUE;
        }
    }
    return FALSE;
}
static int take_training_item(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task *item;
    switch (behavior->mode) {
    case 0:
        chao_link_motion_frames(chao, CHAO_ANIM_CAPTURE_MORAU, 15);
        behavior->mode++;
        break;
    }
    if (mover_turn_to_target_by(chao, TAKE_TURN) < FACING_ITEM) {
        item = collision_first_with_id(chao, TRAINING_ITEM_HIT);
        if (item && !garden_object_locked(item)) {
            player_let_go(0);
            chao_take_in_both_hands(chao, item);
            chao_set_behavior(chao, absorb_training_item);
        } else {
            return BEHAVIOR_FINISH;
        }
    }
    return BEHAVIOR_CONTINUE;
}

int garden_player_side(void)
{
    switch (player_character(0)) {
    case PLAYER_SONIC:
    case PLAYER_TAILS:
    case PLAYER_KNUCKLES:
    case PLAYER_KIND_AMY:
        return 0;
    case PLAYER_SHADOW:
    case PLAYER_EGGMAN:
    case PLAYER_ROUGE:
        return 1;
    default:
        return 2;
    }
}

/* two trails of sparkles that circle the Chao in opposite directions as they
 * rise. they last SPIRAL_FRAMES + 1 frames */
static void training_spiral(struct task *effect)
{
    struct task_work *work = effect->work;
    struct task *chao = effect->parent;
    struct task_work *center = chao->work;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    if (work->mode == 0) {
        RISE(work) = 0.0F;
        work->mode++;
    }
    work->timer++;
    velocity = still;
    pos.x = (njSin(work->ang.y) * SPIRAL_RADIUS) + center->pos.x;
    pos.y = center->pos.y + RISE(work);
    pos.z = (njCos(work->ang.y) * SPIRAL_RADIUS) + center->pos.z;
    sparkle_create(&pos, &velocity, 1.0F);
    pos.x = (njSin(-work->ang.y) * SPIRAL_RADIUS) + center->pos.x;
    pos.y = center->pos.y + RISE(work);
    pos.z = (njCos(-work->ang.y) * SPIRAL_RADIUS) + center->pos.z;
    sparkle_create(&pos, &velocity, 1.0F);
    if (work->timer > SPIRAL_FRAMES) {
        task_kill(effect);
    }
    work->ang.y += SPIRAL_STEP;
    RISE(work) += SPIRAL_CLIMB;
}

void chao_create_training_spiral(struct task *chao)
{
    task_create_child(TASK_WORK, training_spiral, chao);
}

static int absorb_training_item(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct garden_entry *partner;
    struct task *item;
    struct task_work *item_work;
    struct task_work *work;
    NJS_POINT3 *to;
    NJS_POINT3 *from;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    Float evolution;
    Float choice;
    Angle angle;
    struct animal_training_points *reward;
    Uint8 type;
    int kind;
    int part;
    int slot;
    Uint32 turn;
    Float alignment;
    Uint8 drive;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_CAPTURE_SURISURI);
        behavior->mode = 1;
        /* @hack type: the random term converted to a Uint16, where (int)
         * would do */
        behavior->count = (Uint16)(RANDOM_UNIT() * 200.0F) + 90;
        sound_play_at(0x1004, NULL, 0, 0, &chao->work->pos);
        chao_create_training_spiral(chao);
        /* fall through */
    case 1:
        if (!chao_place_held_item(chao)) {
            return BEHAVIOR_FINISH;
        }
        if (chao_motion_ended(chao)) {
            partner = garden_mutual_partner(chao);
            chao_link_motion(chao, CHAO_ANIM_CAPTURE_KURUN);
            behavior->mode = 2;
            sound_play_at(0x1005, NULL, 0, 0, &chao->work->pos);
            work = chao->work;
            /* @hack spelling: the angle stepped in an unsigned turn, which
             * angle copies, where angle alone would do */
            for (turn = 0; (Sint32)turn < RING_END; turn = turn + RING_STEP) {
                angle = turn;
                pos.x = work->pos.x;
                pos.y = work->pos.y + (RANDOM_UNIT() * 0.3F) + 0.5F;
                pos.z = work->pos.z;
                velocity.x = njSin(angle) * 0.5F;
                velocity.y = (RANDOM_UNIT() * 0.5F) + 0.2F;
                velocity.z = njCos(angle) * 0.5F;
                sparkle_create(&pos, &velocity, 1.0F);
            }
            if (partner) {
                item = partner->task;
                item_work = item->work;
                to = &item_work->pos;
                from = &chao->work->pos;
                kind = ITEM(item_work);
                item_work->ang.z = 1;
                pos.x = to->x - from->x;
                pos.y = 0.0F;
                pos.z = to->z - from->z;
                njUnitVector(&pos);
                pos.x *= 0.4F;
                pos.y = 0.85F;
                pos.z *= 0.4F;
                mover_set_spd(item, &pos);
                if (kind < ANIMAL_KINDS) {
                    for (slot = 0; slot < CHAO_PART_SLOTS; slot++) {
                        chao_record_eaten_minimal(chao, kind);
                        if (PARAMETERS(chao).type == CHAO_CHILD) {
                            if (chao_child_has_animal_part(kind, slot)) {
                                if (RANDOM_UNIT() <
                                    chao_tuning.part_gain_chance) {
                                    chao_set_animal_part(chao, slot, kind);
                                }
                            } else if (RANDOM_UNIT() <
                                       chao_tuning.part_loss_chance) {
                                chao_remove_animal_part(chao, slot);
                                if (slot == 4 && kind != ANIMAL_BATMAN) {
                                    PARAMETERS(chao).costume = 0;
                                }
                            }
                        } else {
                            if (chao_adult_has_animal_part(kind, slot)) {
                                if (RANDOM_UNIT() <
                                    chao_tuning.part_gain_chance) {
                                    chao_record_eaten_minimal(chao, kind);
                                    chao_set_animal_part(chao, slot, kind);
                                }
                            } else if (RANDOM_UNIT() <
                                       chao_tuning.part_loss_chance) {
                                chao_remove_animal_part(chao, slot);
                                if (slot == 4 && kind != ANIMAL_BATMAN) {
                                    PARAMETERS(chao).costume = 0;
                                }
                            }
                        }
                    }
                    part = chao_animal_part(chao, 3);
                    if (part == ANIMAL_DRA) {
                        slot = chao_animal_part(chao, 1);
                        if (slot != -1 && slot != ANIMAL_DRA) {
                            chao_remove_animal_part(chao, 1);
                        }
                    }
                    if (part == ANIMAL_SHEP) {
                        slot = chao_animal_part(chao, 1);
                        if (slot != -1 && slot != ANIMAL_SHEP) {
                            chao_remove_animal_part(chao, 1);
                        }
                    }
                }
                if (RANDOM_UNIT() < chao_tuning.part_loss_chance &&
                    kind != ANIMAL_HAN) {
                    PARAMETERS(chao).emotion_ball = 0;
                }
                switch (kind) {
                case ANIMAL_HAN:
                    if (RANDOM_UNIT() < chao_tuning.part_gain_chance) {
                        chao_record_eaten_minimal(chao, kind);
                        if (PARAMETERS(chao).type < CHAO_CHAOS) {
                            PARAMETERS(chao).emotion_ball = 1;
                        }
                    }
                    break;
                case ANIMAL_BONE:
                    chao_record_eaten_minimal(chao, kind);
                    break;
                case ANIMAL_BATMAN:
                    if (RANDOM_UNIT() < chao_tuning.part_gain_chance) {
                        chao_record_eaten_minimal(chao, kind);
                        PARAMETERS(chao).costume = 1;
                    }
                    break;
                }
                type = PARAMETERS(chao).type;
                reward = &animal_training_rewards[kind];
                if (type == CHAO_CHILD) {
                    evolution = chao_tuning.animal_evolution[0];
                    alignment = chao_tuning.animal_gift_alignment[0];
                } else {
                    evolution = chao_tuning.animal_evolution[1];
                    alignment = chao_tuning.animal_gift_alignment[1];
                }
                chao_player_affection(chao, chao_tuning.animal_gift_affection,
                                      alignment);
                switch (kind) {
                case ANIMAL_PEN:
                case ANIMAL_GOMA:
                case ANIMAL_RAKO:
                    chao_add_fly_swim(chao, -evolution);
                    break;
                case ANIMAL_USA:
                case ANIMAL_CHIT:
                case ANIMAL_INO:
                    chao_add_run_power(chao, -evolution);
                    break;
                case ANIMAL_KUMA:
                case ANIMAL_TORA:
                case ANIMAL_GORI:
                    chao_add_run_power(chao, evolution);
                    break;
                case ANIMAL_KUJA:
                case ANIMAL_OUM:
                case ANIMAL_KON:
                    chao_add_fly_swim(chao, evolution);
                    break;
                case ANIMAL_SKA:
                case ANIMAL_SHEP:
                case ANIMAL_ARA:
                case ANIMAL_HAN:
                case ANIMAL_BONE:
                case ANIMAL_BATMAN:
                case ANIMAL_DRA:
                case ANIMAL_UNI:
                case ANIMAL_PHEN:
                    choice = RANDOM_UNIT();
                    if (choice < 0.25F) {
                        chao_add_run_power(chao, evolution);
                    } else if (choice < 0.5F) {
                        chao_add_run_power(chao, -evolution);
                    } else if (choice < 0.75F) {
                        chao_add_fly_swim(chao, evolution);
                    } else {
                        chao_add_fly_swim(chao, -evolution);
                    }
                    break;
                default:
                    drive = kind - ITEM_CHAOS_DRIVE;
                    switch (drive) {
                    case CHAOS_DRIVE_YELLOW:
                        chao_add_fly_swim(chao, -(evolution * 0.5F));
                        break;
                    case CHAOS_DRIVE_GREEN:
                        chao_add_run_power(chao, -(evolution * 0.5F));
                        break;
                    case CHAOS_DRIVE_RED:
                        chao_add_run_power(chao, evolution * 0.5F);
                        break;
                    case CHAOS_DRIVE_PURPLE:
                        chao_add_fly_swim(chao, evolution * 0.5F);
                        break;
                    }
                    break;
                }
                chao_add_skill(chao, CHAO_SWIM, reward->swim);
                chao_add_skill(chao, CHAO_FLY, reward->fly);
                chao_add_skill(chao, CHAO_RUN, reward->run);
                chao_add_skill(chao, CHAO_POWER, reward->power);
                switch (type) {
                case CHAO_NORMAL_ADULT:
                case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
                case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):
                    choice = RANDOM_UNIT() * 3.999F;
                    if (choice < 1.0F) {
                        chao_add_skill(chao, CHAO_SWIM, reward->swim);
                    } else if (choice < 2.0F) {
                        chao_add_skill(chao, CHAO_FLY, reward->fly);
                    } else if (choice < 3.0F) {
                        chao_add_skill(chao, CHAO_RUN, reward->run);
                    } else if (choice < 4.0F) {
                        chao_add_skill(chao, CHAO_POWER, reward->power);
                    }
                    break;
                case CHAO_SWIM_ADULT:
                case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
                case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):
                    chao_add_skill(chao, CHAO_SWIM, reward->swim);
                    break;
                case CHAO_FLY_ADULT:
                case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
                case CHAO_DARK_TYPE(CHAO_FLY_ADULT):
                    chao_add_skill(chao, CHAO_FLY, reward->fly);
                    break;
                case CHAO_RUN_ADULT:
                case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
                case CHAO_DARK_TYPE(CHAO_RUN_ADULT):
                    chao_add_skill(chao, CHAO_RUN, reward->run);
                    break;
                case CHAO_POWER_ADULT:
                case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
                case CHAO_DARK_TYPE(CHAO_POWER_ADULT):
                    chao_add_skill(chao, CHAO_POWER, reward->power);
                    break;
                default:
                    break;
                }
            }
            garden_separate_partners(chao);
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
