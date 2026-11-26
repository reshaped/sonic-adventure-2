/* the joining Chao walks to the caller and pairs with it. it sends the
 * caller MATE_NEXT as they pair, and again as their dance ends */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/behavior.h"
#include "chao/body_collision.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/genes.h"
#include "chao/icon.h"
#include "chao/mating.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/residents.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/mover.h"
#include "sound/adx.h"
#include "system/task.h"
#include "system/util.h"

static const char mating_music[] = "chao_g_dance.adx";

/* the command chao_call_mate waits for from the joining Chao */
#define MATE_NEXT 1

/* the caller turns to its partner after TURN_DELAY frames of rising. the two
 * push apart at frame PARTING_FRAME of their jump. the joining Chao dances
 * DANCE_FRAMES before it sends MATE_NEXT again */
#define TURN_DELAY 30
#define PARTING_FRAME 20
#define DANCE_FRAMES 300
#define CALL_RADIUS 30.0F
/* the caller's field ends FIELD_EARLY frames before its call does, and its
 * mark MARK_EARLY frames before */
#define FIELD_EARLY 120
#define MARK_EARLY 300
#define JOIN_MARK_FRAMES 400
#define EGG_AHEAD 2.0F

/* behavior->mode in chao_call_mate and in chao_join_mate */
enum call_mode {
    CALL_START,
    CALL_SIT,
    CALL_WAIT,
    CALL_RISE,
    CALL_DANCE,
    CALL_JUMP,
    CALL_CHEER
};
enum join_mode {
    JOIN_START,
    JOIN_RISE,
    JOIN_APPROACH,
    JOIN_DANCE,
    JOIN_JUMP,
    JOIN_CHEER
};

#define ADVANCE_DANCE()                                                        \
    behavior->mode++;                                                          \
    behavior->timer = 0;                                                       \
    behavior->count = 0
#define JUMP_APART()                                                           \
    mover->spd.x -= njSin(work->ang.y) * 0.25F;                                \
    mover->spd.y += 0.8F;                                                      \
    mover->spd.z -= njCos(work->ang.y) * 0.25F
/* part from the partner and cheer. the cheer lasts minimum to minimum + 3
 * ends of its motion, at random */
#define FINISH_DANCE(minimum)                                                  \
    garden_separate_partners(chao);                                            \
    chao_link_motion_frames(chao, CHAO_ANIM_BANZAI_JUMP, 15);                  \
    ADVANCE_DANCE();                                                           \
    behavior->count = RANDOM_COUNT((minimum), 3.999999F)

int chao_call_mate(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover = chao->motion.p;
    struct task_work *work = chao->work;
    struct garden_entry *partner;

    switch (behavior->mode) {
    case CALL_START:
        chao_resist_displacement(chao);
        behavior->timer = 0;
        chao_create_mating_flowers(chao);
        CHAO_WORK(chao)->field = garden_create_timed_field(
            chao, CHAO_MATING_FIELD, &chao->work->pos, CALL_RADIUS,
            chao_tuning.mating_frames - FIELD_EARLY);
        behavior->mode++;
        break;
    case CALL_SIT:
        chao_link_motion_frames(chao, CHAO_ANIM_HIMA_SIT, 15);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_show_mark(chao, CHAO_MARK_HEART,
                       chao_tuning.mating_frames - MARK_EARLY);
        behavior->mode++;
        break;
    case CALL_WAIT:
        behavior->timer++;
        if (behavior->timer == chao_tuning.mating_frames >> 1) {
            chao_emotion_set(chao, CHAO_BREEDING, 0);
        }
        if (behavior->timer > chao_tuning.mating_frames) {
            return BEHAVIOR_FINISH;
        }
        if (garden_take_command(chao) == MATE_NEXT) {
            partner = garden_partner_in_category(chao, GARDEN_OBJECT_CHAO);
            if (partner && partner->task && partner->task->work) {
                mover_set_target(chao, &partner->task->work->pos);
            }
            chao_link_motion_frames(chao, CHAO_ANIM_GENKI_TATI, 15);
            ADVANCE_DANCE();
            if (CHAO_WORK(chao)->field) {
                task_kill(CHAO_WORK(chao)->field);
                CHAO_WORK(chao)->field = NULL;
            }
        }
        break;
    case CALL_RISE:
        if (behavior->timer++ > TURN_DELAY) {
            mover_turn_to_target_by(chao, 0x400);
        }
        if (chao_motion_ended(chao)) {
            if (garden_partner_in_category(chao, GARDEN_OBJECT_CHAO)) {
                chao_link_motion_frames(chao, CHAO_ANIM_NADE_KIMOCHII, 15);
                ADVANCE_DANCE();
            } else {
                return BEHAVIOR_FINISH;
            }
        }
        break;
    case CALL_DANCE:
        if (garden_take_command(chao) == MATE_NEXT) {
            chao_link_motion_frames(chao, CHAO_ANIM_HASYAGU_JUMP, 15);
            chao_emotion_set(chao, CHAO_BREEDING, 0);
            ADVANCE_DANCE();
        }
        chao_set_motion_speed(chao, 1.5F);
        break;
    case CALL_JUMP:
        if (behavior->timer++ == PARTING_FRAME) {
            JUMP_APART();
        }
        if (chao_motion_ended(chao)) {
            FINISH_DANCE(3);
        }
        break;
    case CALL_CHEER:
        if (chao_motion_ended(chao)) {
            if (++behavior->timer >= behavior->count) {
                return BEHAVIOR_FINISH;
            }
        }
        break;
    }
    CHAO_WORK(chao)->calling_mate = TRUE;
    return BEHAVIOR_CONTINUE;
}

int chao_join_mate(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct mover *mover = chao->motion.p;
    struct task_work *work = chao->work;
    struct task *contact;
    struct task *partner;
    struct garden_entry *entry;
    struct chao_gene egg_gene;
    NJS_POINT3 egg_position;
    struct chao_save *slot;

    switch (behavior->mode) {
    case JOIN_START:
        if (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_MATING_FIELD)) {
            return BEHAVIOR_FINISH;
        }
        switch (chao_motion_pose(chao)) {
        case CHAO_ON_BACK:
            chao_link_motion_frames(chao, CHAO_ANIM_JUMP_UP, 15);
            break;
        case CHAO_SITTING:
            chao_link_motion_frames(chao, CHAO_ANIM_GENKI_TATI, 15);
            break;
        case CHAO_STANDING:
        default:
            chao_link_motion_frames(chao, CHAO_ANIM_KYOROKYORO_UP, 15);
            break;
        }
        behavior->timer = 0;
        behavior->mode++;
        break;
    case JOIN_RISE:
        if (chao_motion_ended(chao)) {
            chao_link_motion_frames(chao, CHAO_ANIM_RHYTHM_WALK_A, 15);
            chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
            chao_show_mark(chao, CHAO_MARK_HEART, JOIN_MARK_FRAMES);
            contact = garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_MATING_FIELD);
            if (!contact) {
                return BEHAVIOR_FINISH;
            }
            mover->target.x = contact->work->pos.x;
            mover->target.z = contact->work->pos.z;
            if (contact->parent) {
                garden_set_partner(chao, contact->parent);
            }
            behavior->mode++;
        }
        break;
    case JOIN_APPROACH:
        if (garden_take_command(chao) == PARTNER_CANCEL) {
            return BEHAVIOR_FINISH;
        } else {
            mover_turn_to_target_by(chao, 0x200);
            if (mover->flags & MOVER_ON_FLOOR) {
                mover->acc.x +=
                    njSin(work->ang.y) * chao_tuning.approach_acceleration;
                mover->acc.z +=
                    njCos(work->ang.y) * chao_tuning.approach_acceleration;
            }
            contact = garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_BODY_HIT);
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side: without a
             * partner, the behavior finishes */
            entry = garden_partner(chao);
            if (!entry) {
                return BEHAVIOR_FINISH;
            }
            partner = entry->task;
#else
            /* @bug memory: reads the task of garden_partner's entry untested
             * (0x0c5335b4). garden_partner returns NULL for a Chao with no
             * garden entry (0x0c51e26a) or no partner. JOIN_RISE sets a
             * partner only where the field the Chao touched has a parent */
            partner = garden_partner(chao)->task;
#endif
            if (partner == contact) {
                if (garden_paired_elsewhere(chao, partner)) {
                    return BEHAVIOR_FINISH;
                }
                garden_pair_objects(chao, partner);
                garden_command_partner(chao, MATE_NEXT);
                jingle_play(mating_music);
                chao_link_motion_frames(chao, CHAO_ANIM_NADE_KIMOCHII, 15);
                ADVANCE_DANCE();
            }
            chao_set_motion_speed(chao, 1.4F);
            if (!garden_hit_kind(chao, CHAO_PUSH_SHAPE, CHAO_MATING_FIELD)) {
                return BEHAVIOR_FINISH;
            }
            if (behavior->limit-- <= 0) {
                return BEHAVIOR_FINISH;
            }
        }
        break;
    case JOIN_DANCE:
        if (!garden_partner_in_category(chao, GARDEN_OBJECT_CHAO)) {
            return BEHAVIOR_FINISH;
        }
        if (behavior->timer++ > DANCE_FRAMES) {
            garden_command_partner(chao, MATE_NEXT);
            chao_link_motion_frames(chao, CHAO_ANIM_HASYAGU_JUMP, 15);
            chao_emotion_set(chao, CHAO_BREEDING, 0);
            ADVANCE_DANCE();
        }
        chao_set_motion_speed(chao, 1.5F);
        break;
    case JOIN_JUMP:
        if (behavior->timer++ == PARTING_FRAME) {
            JUMP_APART();
            entry = garden_partner_in_category(chao, GARDEN_OBJECT_CHAO);
            if (entry) {
                partner = entry->task;
                if (partner) {
                    egg_position = work->pos;
                    egg_position.x += njSin(work->ang.y) * EGG_AHEAD;
                    egg_position.z += njCos(work->ang.y) * EGG_AHEAD;
                    chao_inherit_traits(partner, chao, &egg_gene);
                    slot = garden_unused_chao();
                    if (slot) {
                        chao_create_egg(&egg_gene, &egg_position, EGG_LAID,
                                        slot);
                    }
                }
            }
        }
        if (chao_motion_ended(chao)) {
            FINISH_DANCE(4);
        }
        break;
    case JOIN_CHEER:
        if (chao_motion_ended(chao)) {
            if (++behavior->timer >= behavior->count) {
                return BEHAVIOR_FINISH;
            }
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
