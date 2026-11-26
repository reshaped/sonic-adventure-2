#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "chao/activities.h"
#include "chao/animal_actions.h"
#include "chao/behavior.h"
#include "chao/garden_field.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/perception.h"
#include "chao/sound.h"
#include "chao/tree.h"
#include "chao/walk.h"
#include "chao/work.h"
#include "chao/world.h"
#include "object/animal.h"
#include "shared/dust.h"
#include "shared/flame.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define VOICE(id)                                                              \
    sound_play_at((id), NULL, 0, GARDEN_VOICE_VOLUME, &chao->work->pos)
/* the skunk's imitation leaves a field of kind SKUNK_FIELD SKUNK_REACH behind
 * the Chao. the field is SKUNK_FIELD_RADIUS wide and lasts SKUNK_FIELD_FRAMES
 */
#define SKUNK_FIELD 151
#define SKUNK_REACH 3.0F
#define SKUNK_FIELD_RADIUS 5.0F
#define SKUNK_FIELD_FRAMES 60
/* the frames the dragon's imitation waits before it breathes fire, and the
 * flames' speed and size */
#define FLAME_DELAY 20
#define FLAME_SPEED 0.5F
#define FLAME_SIZE 0.25F
/* the tiger's imitation scratches a tree in sight within TREE_RANGE, with a
 * chance of 1 - TREE_SKIP */
#define TREE_SKIP 0.3F
#define TREE_RANGE 40.0F

static int imitate_cheetah(struct task *chao);
static int imitate_unicorn(struct task *chao);
static int imitate_gorilla(struct task *chao);
static int imitate_bear(struct task *chao);
static int imitate_boar(struct task *chao);
static int imitate_peacock(struct task *chao);
static int imitate_phoenix(struct task *chao);
static int imitate_skunk(struct task *chao);
static int imitate_dragon(struct task *chao);
static int imitate_rabbit(struct task *chao);
static int imitate_condor(struct task *chao);
static int imitate_sheep(struct task *chao);
static int imitate_seal(struct task *chao);
static int imitate_penguin(struct task *chao);
static int imitate_raccoon(struct task *chao);
static int imitate_tiger(struct task *chao);

/* by animal_kind. the otter, the parrot, the half fish, the skeleton dog and
 * the bat imitate nothing */
/* clang-format off */
chao_behavior_fn animal_actions[ANIMAL_KINDS] = {
    imitate_penguin, imitate_seal, NULL, imitate_rabbit, imitate_cheetah,
    imitate_boar, imitate_bear, imitate_tiger, imitate_gorilla,
    imitate_peacock, NULL, imitate_condor, imitate_skunk, imitate_sheep,
    imitate_raccoon, NULL, NULL, NULL, imitate_dragon, imitate_unicorn,
    imitate_phoenix,
};
/* clang-format on */

/* a behavior that plays motion until it has ended RANDOM_COUNT(least, choices)
 * times */
#define REPEAT_MOTION(action, motion, least, choices)                          \
    static int action(struct task *chao)                                       \
    {                                                                          \
        struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);                  \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            chao_link_motion(chao, (motion));                                  \
            behavior->mode++;                                                  \
            behavior->timer = RANDOM_COUNT((least), (choices));                \
            /* fall through */                                                 \
        case 1:                                                                \
            if (chao_motion_ended(chao) && --behavior->timer <= 0) {           \
                return BEHAVIOR_FINISH;                                        \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        return BEHAVIOR_CONTINUE;                                              \
    }
/* a behavior that plays motion until RANDOM_COUNT(least, choices) frames pass
 */
#define TIMED_MOTION(action, motion, least, choices)                           \
    static int action(struct task *chao)                                       \
    {                                                                          \
        struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);                  \
        switch (behavior->mode) {                                              \
        case 0:                                                                \
            chao_link_motion(chao, (motion));                                  \
            behavior->timer = RANDOM_COUNT((least), (choices));                \
            behavior->mode++;                                                  \
            /* fall through */                                                 \
        case 1:                                                                \
            if (behavior->timer-- <= 0) {                                      \
                return BEHAVIOR_FINISH;                                        \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        return BEHAVIOR_CONTINUE;                                              \
    }

REPEAT_MOTION(imitate_cheetah, CHAO_ANIM_KAOARAU, 2, 2.999999F)
int chao_repeat_song_pose(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_UTAU_LEFT);
        behavior->mode++;
        behavior->timer = RANDOM_COUNT(4, 2.999999F);
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            if (--behavior->timer <= 0) {
                return BEHAVIOR_FINISH;
            } else {
                chao_link_motion(chao, CHAO_ANIM_UTAU_LEFT);
            }
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int imitate_unicorn(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_BAKUTEN);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_held(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int imitate_gorilla(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DRAMING);
        behavior->timer = RANDOM_COUNT(120, 101);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            if (RANDOM_UNIT() < 0.4F) {
                return BEHAVIOR_FINISH;
            }
            if (RANDOM_UNIT() < 0.4F) {
                chao_link_motion(chao, CHAO_ANIM_DRAMING_WOO);
                VOICE(0x6087);
                behavior->mode++;
            } else {
                chao_link_motion(chao, CHAO_ANIM_DRAMING);
                behavior->timer = RANDOM_COUNT(160, 121);
            }
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            chao_link_motion(chao, CHAO_ANIM_DRAMING);
            behavior->timer = RANDOM_COUNT(160, 121);
            behavior->mode--;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

static int imitate_bear(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_SUMO_DOSUKOI);
        behavior->timer = RANDOM_COUNT(120, 121);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        garden_move_forward(chao, 0.05F);
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int imitate_boar(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_ON_YOUR_MARK);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao)) {
            chao_link_motion(chao, CHAO_ANIM_READY);
            behavior->mode++;
        }
        break;
    case 2:
        if (chao_motion_ended(chao)) {
            chao_link_motion(chao, CHAO_ANIM_GO);
            behavior->mode++;
        }
        break;
    case 3:
        if (chao_motion_ended(chao)) {
            chao_link_motion(chao, CHAO_ANIM_DASH);
            behavior->timer = RANDOM_COUNT(120, 61);
            behavior->mode++;
        }
        garden_move_forward(chao, 0.3F);
        break;
    case 4:
        garden_move_forward(chao, 0.3F);
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    chao_set_motion_speed(chao, 1.5F);
    return BEHAVIOR_CONTINUE;
}
static int imitate_peacock(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        if (RANDOM_UNIT() < 0.5F) {
            chao_link_motion(chao, CHAO_ANIM_OTAKAI_L);
        } else {
            chao_link_motion(chao, CHAO_ANIM_OTAKAI_R);
        }
        behavior->timer = RANDOM_COUNT(600, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
TIMED_MOTION(imitate_phoenix, CHAO_ANIM_HIP_DANCE, 300, 201)

static int imitate_skunk(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    NJS_POINT3 position;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_OSYABURI);
        behavior->timer = RANDOM_COUNT(120, 121);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            sound_play_at(0x1013, NULL, 0, 0, &chao->work->pos);
            behavior->timer = RANDOM_COUNT(120, 121);
            position.x = work->pos.x - (njSin(work->ang.y) * SKUNK_REACH);
            position.y = work->pos.y + 0.2F;
            position.z = work->pos.z - (njCos(work->ang.y) * SKUNK_REACH);
            dust_sphere(&position, 2.0F, 0.1F, 2.0F);
            garden_create_timed_field(chao, SKUNK_FIELD, &position,
                                      SKUNK_FIELD_RADIUS, SKUNK_FIELD_FRAMES);
            behavior->mode++;
        }
        break;
    case 2:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int imitate_dragon(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_behavior *behavior = &work->behavior;
    struct chao_shape_work *shape = &work->shape;
    NJS_VECTOR velocity;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_FIRE);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer++ > FLAME_DELAY) {
            velocity.x = shape->mouth_direction.x * FLAME_SPEED;
            velocity.y = shape->mouth_direction.y * FLAME_SPEED;
            velocity.z = shape->mouth_direction.z * FLAME_SPEED;
            flame_create(&shape->mouth_position, &velocity, FLAME_SIZE);
        }
        if (chao_motion_held(chao)) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
REPEAT_MOTION(imitate_rabbit, CHAO_ANIM_PYONPYON, 4, 3.999999F)
REPEAT_MOTION(perform_condor_pose, CHAO_ANIM_FUKKIN, 4, 3.999999F)

static int imitate_condor(struct task *chao)
{
    /* @hack redundant: a pointer to the behavior record, which nothing reads */
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    (void)behavior;
    chao_set_behavior(chao, chao_sit_down_behavior);
    chao_queue_behavior(chao, perform_condor_pose);
    return BEHAVIOR_CONTINUE;
}

static int imitate_sheep(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_DENGURI);
        behavior->mode++;
        behavior->timer = RANDOM_COUNT(4, 3.999999F);
        /* fall through */
    case 1:
        if (chao_motion_ended(chao)) {
            behavior->mode++;
        }
        break;
    case 2:
        if (chao_motion_ended(chao) && --behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        garden_move_forward(chao, 0.1F);
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int imitate_seal(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_behavior *behavior = &((struct chao_work *)work)->behavior;
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_GOROGORO);
        behavior->timer = RANDOM_COUNT(500, 301);
        behavior->mode++;
        /* fall through */
    case 1:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        work->pos.x += njSin(work->ang.y - QUARTER_TURN) * 0.05F;
        work->pos.z += njCos(work->ang.y - QUARTER_TURN) * 0.05F;
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int imitate_penguin(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_HAIHAI_SORU_UP);
        behavior->mode++;
        behavior->timer = RANDOM_COUNT(120, 61);
        /* fall through */
    case 1:
        if (chao_motion_ended(chao) && --behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        garden_move_forward(chao, 0.15F);
        break;
    }
    return BEHAVIOR_CONTINUE;
}
static int imitate_raccoon(struct task *chao)
{
    struct chao_behavior *behavior = CHAO_BEHAVIOR(chao);
    switch (behavior->mode) {
    case 0:
        chao_link_motion(chao, CHAO_ANIM_TAIL);
        behavior->mode++;
        behavior->timer = RANDOM_COUNT(400, 201);
        VOICE(0x6084);
        /* fall through */
    case 1:
        if (--behavior->timer <= 0) {
            return BEHAVIOR_FINISH;
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}
TIMED_MOTION(tiger_tree_pose, CHAO_ANIM_TOGU, 300, 201)

static int imitate_tiger(struct task *chao)
{
    struct chao_perceived_object *seen;
    Float threshold;
    struct task *tree;
    struct garden_tree *work;
    int stage;

    seen = chao_nearest_visible_tree(chao);
    if (seen == NULL) {
        return BEHAVIOR_FINISH;
    }
    threshold = TREE_SKIP;
    if (threshold < RANDOM_UNIT() && seen->distance < TREE_RANGE &&
        seen->entry != NULL) {
        tree = seen->entry->task;
        if (tree != NULL) {
            work = (struct garden_tree *)tree->work;
            stage = work->stage;
            switch (stage) {
            case TREE_STAGE_MATURE:
            case TREE_STAGE_PERMANENT:
                garden_set_target(chao, seen->entry->task);
                chao_set_behavior(chao, chao_walk_to_target_behavior);
                chao_queue_behavior(chao, chao_face_target_behavior);
                chao_queue_behavior(chao, tiger_tree_pose);
                return BEHAVIOR_CONTINUE;
            }
        }
    }
    return BEHAVIOR_FINISH;
}

Bool chao_choose_animal_action(struct task *actor)
{
    /* @hack spelling: a copy of actor, where actor would do */
    struct task *chao = actor;
    /* @hack type: known holds 32, where ANIMAL_KINDS would do */
    int known[32];
    int count;
    int i;
    count = 0;
    for (i = 0; i < ANIMAL_KINDS; i++) {
        if (chao_has_eaten_minimal(chao, i)) {
            if (animal_actions[i] != NULL) {
                known[count] = i;
                count++;
            }
        }
    }
    if (count > 0) {
        i = known[(unsigned int)RANDOM_BELOW(count)];
        chao_set_behavior(chao, animal_actions[i]);
        return TRUE;
    }
    return FALSE;
}
