/* Pumpkin Hill's scenery: eight kinds of object, each of which stands still and
 * draws one model. the stage's object list names them TIM, KUSA, KAKASI,
 * FENCES, FENCE, WOODEAD, PPK_SM and PPK_BG. the pumpkins and the dead tree
 * stretch and squash in a slow pulse. a hunter's punch sets a pumpkin
 * wobbling */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg05/scenery.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the set editor's labels for an object's placement */
static const char scenery_edit_angle_x[12] = "<- ANGLE X";
static const char scenery_edit_angle_y[12] = "<- ANGLE Y";
static const char scenery_edit_angle_z[12] = "<- ANGLE Z";

#include "stg05/scenery/text.inc"
#include "stg05/scenery_art/scenery.h"

/* an object's task keeps its kind in motion, and the kind's texture list in
 * force. a kind is an index of scenery_builds */
#define KIND(task) ((task)->motion.l)
#define TEXLIST(task) ((NJS_TEXLIST *)(task)->force.p)
/* a punch's wobble. the task keeps its speed in extra, and the wobble itself
 * in unk_3c */
/* @hack type: the speed read as a Float through the extra's address, where
 * (task)->extra.f would do */
#define WOBBLE_SPEED(task) (*(Float *)&(task)->extra)
/* @hack type: the wobble read as a Float through unk_3c's address, where
 * (task)->unk_3c.f would do */
#define WOBBLE(task) (*(Float *)&(task)->unk_3c)

/* the kinds, in the order of scenery_builds */
enum scenery_kind {
    KIND_TIM,
    KIND_GRASS,
    KIND_SCARECROW,
    KIND_FENCE_SMALL,
    KIND_FENCE,
    KIND_DEAD_TREE,
    KIND_PUMPKIN_SMALL,
    KIND_PUMPKIN_BIG,
    KINDS
};

/* in the work's mode: the object stands still, or pulses by pulse 1 or pulse
 * 2 */
enum scenery_mode { MODE_STILL, MODE_PULSE_1, MODE_PULSE_2 };
/* in the work's submode: the object turns by njRotateZ, njRotateY and
 * njRotateX, in that order. without it the object turns about y alone */
#define TURNED 3

/* a hunter's punch sets a pumpkin wobbling, shakes the puncher's pad and plays
 * PUNCH_SOUND. it does none of these while the work's tick counts down from
 * the last punch. each punch sets the tick to PUNCH_STEPS, and each unhurt step
 * takes 1 off */
#define PUNCH_STEPS 30
#define PUNCH_SOUND 0x100a
#define PUNCH_PRIORITY 0
#define PUNCH_VOLUME 0
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 50

#define TIM_SHAPES 2
#define SCARECROW_SHAPES 1
#define FENCE_SHAPES 2
#define DEAD_TREE_SHAPES 1
#define PUMPKIN_SHAPES 1

NJS_TEXLIST *scenery_texlists[9] = {
#include "stg05/scenery/scenery_texlists.inc"
};
/* the pulses. a pulse's angle is the game's steps plus the work's timer, times
 * the pulse's rate. along each axis, the object's scale is 1 plus the sine
 * times the axis's gain. an object's timer, its phase, is a random count of
 * steps below its pulse's phases */
static Angle pulse_1_rate = 0x300;
static Float pulse_1_gain_y = 0.04F;
static Float pulse_1_gain_xz = -0.02F;
static Sint32 pulse_1_phases = 60;
static Angle pulse_2_rate = 0x400;
static Float pulse_2_gain_y = 0.05F;
static Float pulse_2_gain_x = -0.05F;
static Float pulse_2_gain_z = -0.0F;
static Sint32 pulse_2_phases = 60;
/* TRUE where scenery_build compiled every kind's model */
static Bool scenery_built = FALSE;
static struct model_build scenery_builds[KINDS] = {
#include "stg05/scenery/scenery_builds.inc"
};
/* clang-format off */
static struct collision_shape tim_shapes[TIM_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-20.0F, 13.0F, 0.0F}, 7.0F, 19.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-9.0F, 0.0F, -1.0F}, 7.0F, 9.0F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
static struct collision_shape scarecrow_shapes[SCARECROW_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 7.0F, 0.0F}, 8.0F, 7.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape fence_small_shapes[FENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 9.0F, 0.0F}, 15.0F, 9.0F, 0.5F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 18.0F, 0.0F}, 3.0F, 15.0F, 1.0F, 0, {0, 0, 0}},
};
static struct collision_shape fence_shapes[FENCE_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 14.0F, 0.0F}, 17.0F, 14.0F, 0.5F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {0.0F, 28.0F, 0.0F}, 3.0F, 17.0F, 1.0F, 0, {0, 0, 0}},
};
static struct collision_shape dead_tree_shapes[DEAD_TREE_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_ALL_BUT_PLAYERS,
     {2.0F, 23.0F, 0.0F}, 10.0F, 25.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape pumpkin_small_shapes[PUMPKIN_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_GUARD_LEVEL(1) | SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_PICKUPS,
     {0.0F, 7.0F, 0.0F}, 7.5F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape pumpkin_big_shapes[PUMPKIN_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS,
     SHAPE_HURTABLE | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS |
         SHAPE_IGNORE_PICKUPS,
     {0.0F, 6.0F, 0.0F}, 13.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a punch starts the wobble's speed at punch_speed. each step the wobble moves
 * by its speed. the speed then keeps wobble_keep of itself, less wobble_pull of
 * the wobble */
static Float punch_speed = 0.07F;
static Float wobble_pull = 0.22F;
static Float wobble_keep = 0.96F;

static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count);
static void scenery_destroy(struct task *task);
static void scenery_step(struct task *task);
static void scenery_draw(struct task *task);

Uint8 *scenery_build(Uint8 *buf)
{
    if (model_builds_compile(&buf, scenery_builds, KINDS) == KINDS) {
        scenery_built = TRUE;
    }
    return buf;
}

void tim_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.l = KIND_TIM;
        task->force.p = &tim_texlist;
        scenery_start(task, tim_shapes, TIM_SHAPES);
    }
}

void grass_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->submode = TURNED;
        task->motion.l = KIND_GRASS;
        task->force.p = &grass_texlist;
        scenery_start(task, NULL, 0);
    }
}

void scarecrow_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.l = KIND_SCARECROW;
        task->force.p = &scarecrow_texlist;
        scenery_start(task, scarecrow_shapes, SCARECROW_SHAPES);
    }
}

void fence_small_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.l = KIND_FENCE_SMALL;
        task->force.p = &fence_small_texlist;
        scenery_start(task, fence_small_shapes, FENCE_SHAPES);
    }
}

void fence_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->motion.l = KIND_FENCE;
        task->force.p = &fence_texlist;
        scenery_start(task, fence_shapes, FENCE_SHAPES);
    }
}

void dead_tree_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->mode = MODE_PULSE_2;
        work->timer = (Float)rand() * pulse_2_phases * RAND_SCALE;
        task->motion.l = KIND_DEAD_TREE;
        task->force.p = &dead_tree_texlist;
        scenery_start(task, dead_tree_shapes, DEAD_TREE_SHAPES);
    }
}

void pumpkin_small_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->mode = MODE_PULSE_1;
        work->timer = (Float)rand() * pulse_1_phases * RAND_SCALE;
        task->motion.l = KIND_PUMPKIN_SMALL;
        task->force.p = &pumpkin_small_texlist;
        scenery_start(task, pumpkin_small_shapes, PUMPKIN_SHAPES);
    }
}

void pumpkin_big_init(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->mode = MODE_PULSE_1;
        work->timer = (Float)rand() * pulse_1_phases * RAND_SCALE;
        task->motion.l = KIND_PUMPKIN_BIG;
        task->force.p = &pumpkin_big_texlist;
        scenery_start(task, pumpkin_big_shapes, PUMPKIN_SHAPES);
    }
}

/* every object of a kind shares the kind's shapes: the record uses them in
 * place, without a copy */
static void scenery_start(struct task *task, struct collision_shape *shapes,
                          int count)
{
    task->draw = scenery_draw;
    task->step = scenery_step;
    task->destructor = scenery_destroy;
    if (shapes != NULL) {
        collision_init_in_place(task, shapes, count, COLLISION_OBJECT);
    }
}

/* task_destroy frees task->motion, task->force and task->extra once the
 * destructor returns. an object's three hold its kind, its texture list and
 * its wobble's speed. none is a block of the heap, so the destructor clears
 * them */
static void scenery_destroy(struct task *task)
{
    task->force.p = NULL;
    task->motion.p = NULL;
    task->extra.p = NULL;
}

static void scenery_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    int slot;
    int mode;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->collision == NULL) {
        return;
    }
    if (work->flags & WORK_HURT) {
        collision_restart_scans();
        while ((hit = collision_next_player(task)) != NULL) {
            slot = player_slot_of(hit);
            /* @hack spelling: mode, a local that holds player_character_of's
             * result first, where a switch on the call would do */
            mode = player_character_of(hit);
            switch (mode) {
            case PLAYER_KNUCKLES:
            case PLAYER_ROUGE:
                mode = player_work[slot]->mode;
                if (mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
                    mode <= PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
                    if (!work->tick) {
                        WOBBLE_SPEED(task) = punch_speed;
                        vibration_start(slot, VIBRATION_POWER, VIBRATION_FREQ,
                                        VIBRATION_UNTIMED);
                        sound_play(PUNCH_SOUND, NULL, PUNCH_PRIORITY,
                                   PUNCH_VOLUME);
                    }
                    work->tick = PUNCH_STEPS;
                }
                break;
            }
        }
        work->flags &= ~WORK_HURT;
    } else if (work->tick != 0) {
        work->tick--;
    }
    WOBBLE(task) += WOBBLE_SPEED(task);
    WOBBLE_SPEED(task) =
        -(WOBBLE(task) * wobble_pull) + (WOBBLE_SPEED(task) * wobble_keep);
    collision_register(task);
}

/* a pulsing object's wobble adds to its scale along x and z. half the wobble
 * comes off its scale along y. an object in MODE_STILL draws unscaled */
static void scenery_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float sine;
    Float scale_x;
    Float scale_y;
    Float scale_z;

    njSetTexture(TEXLIST(task));
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    if (work->submode) {
        njRotateZ(NULL, work->ang.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
    } else {
        njRotateY(NULL, work->ang.y);
    }
    switch (work->mode) {
    case MODE_PULSE_1:
        sine = njSin((game_state.steps + work->timer) * pulse_1_rate);
        scale_y = 1.0F + (sine * pulse_1_gain_y);
        scale_x = 1.0F + (sine * pulse_1_gain_xz);
        njScale(NULL, scale_x + WOBBLE(task), scale_y - (WOBBLE(task) * 0.5F),
                scale_x + WOBBLE(task));
        break;
    case MODE_PULSE_2:
        sine = njSin((game_state.steps + work->timer) * pulse_2_rate);
        scale_y = 1.0F + (sine * pulse_2_gain_y);
        scale_x = 1.0F + (sine * pulse_2_gain_x);
        scale_z = 1.0F + (sine * pulse_2_gain_z);
        njScale(NULL, scale_x + WOBBLE(task), scale_y - (WOBBLE(task) * 0.5F),
                scale_z + WOBBLE(task));
        break;
    }
    if (scenery_built) {
        njDirectDrawObject(scenery_builds[KIND(task)].built);
    } else {
        njCnkEasyDrawModel(scenery_builds[KIND(task)].object->model);
    }
    njPopMatrix(1);
}
