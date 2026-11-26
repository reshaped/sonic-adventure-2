/* the Egg Golem, the boss of BOSS_GOLEM.PRS, and the parts that ride its
 * model's nodes. the golem is the task BossGolemExec, which the stage's step
 * creates. no object list names any of this. its twin in BOSS_GOLEM_E.PRS,
 * boss_golem_e/golem.c, has the same hands and sensors. the twin has a head
 * where this golem has the flash. it has no weak spot, and its sensors take
 * the hits instead */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#ifdef NONMATCHING
/* memset, with which this side's golem_destroy clears the tables of the golem's
 * tasks */
#include <string.h>
#endif

#include <stdlib.h>

#include "boss_golem/actions.h"
#include "boss_golem/attachment_art.h"
#include "boss_golem/footholds.h"
#include "boss_golem/golem.h"
#include "boss_golem/head_parts.h"
#include "boss_golem/platforms.h"
#include "boss_golem/stage.h"
#include "boss_golem/tactics.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/boss.h"
#include "shared/boss_title.h"
#include "shared/golem.h"
#include "shared/golem_camera.h"
#include "shared/golem_clips.h"
#include "shared/golem_collision.h"
#include "shared/golem_statue.h"
#include "shared/golem_util.h"
#include "shared/golem_wave.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the hands' and the flash's turns in the world's axes, as the golem's draw
 * last found them */
static struct angles golem_hand_a_ang;
static struct angles golem_hand_b_ang;
static struct angles golem_flash_ang;
/* the inverse of the view matrix, which the golem's draw keeps. it takes a
 * node's matrix into the world's axes */
static NJS_MATRIX golem_view_inverse;
static struct task *golem_sensor_tasks[GOLEM_SENSORS];

#define GOLEM_LEVEL 2
#define GOLEM_GRAVITY (-0.02F)
#define GOLEM_POWER 5.0F
/* what a hit on the weak spot takes from the golem's power */
#define HIT_POWER 1.0F
/* golem_create puts player 0 at (0, the floor, PLAYER_START_Z) */
#define PLAYER_START_Z 210.0F
/* each shape's level in a hit: the hands' and the flash's attack, and the
 * sensors' guard */
#define ATTACK_LEVEL 2
#define SENSOR_GUARD_LEVEL 3
/* the shapes each part takes */
#define HAND_SHAPES 1
#define FLASH_SHAPES 1
#define WEAK_SPOT_SHAPES 2
#define LOW_SENSOR_SHAPES 3
#define FULL_SENSOR_SHAPES 1

/* golem_sensor_place leaves an object in the world while a player lies within
 * its range plus SENSOR_REACH */
#define SENSOR_REACH 15.0F
/* sensor_index's result for a node that no sensor rides */
#define NO_SENSOR (-1)

/* whether index is the motion of stage in any of a move's three runs. a run
 * serves a foothold state, from FOOTHOLD_WHOLE to FOOTHOLD_SUNK_TWICE. the
 * move's motions start at first, with stages to a run */
#define IN_STAGE(index, first, stages, stage)                                  \
    ((index) == (first) + (stage) ||                                           \
     (index) == (first) + (FOOTHOLD_SUNK * (stages)) + (stage) ||              \
     (index) == (first) + (FOOTHOLD_SUNK_TWICE * (stages)) + (stage))
/* the motions in which each hand strikes. they are its own strike's first
 * blows, the other strike's second blows and the slam's blows */
#define HAND_A_STRIKES(index)                                                  \
    (IN_STAGE(index, GOLEM_MOTION_STRIKE_A, GOLEM_STRIKE_STAGES,               \
              GOLEM_STRIKE_BLOW) ||                                            \
     IN_STAGE(index, GOLEM_MOTION_STRIKE_B, GOLEM_STRIKE_STAGES,               \
              GOLEM_STRIKE_SECOND_BLOW) ||                                     \
     SLAM_BLOWS(index))
#define HAND_B_STRIKES(index)                                                  \
    (IN_STAGE(index, GOLEM_MOTION_STRIKE_A, GOLEM_STRIKE_STAGES,               \
              GOLEM_STRIKE_SECOND_BLOW) ||                                     \
     IN_STAGE(index, GOLEM_MOTION_STRIKE_B, GOLEM_STRIKE_STAGES,               \
              GOLEM_STRIKE_BLOW) ||                                            \
     SLAM_BLOWS(index))
#define SLAM_BLOWS(index)                                                      \
    IN_STAGE(index, GOLEM_MOTION_SLAM, GOLEM_SLAM_STAGES, GOLEM_SLAM_BLOWS)
#define SLAM_SLAM(index)                                                       \
    IN_STAGE(index, GOLEM_MOTION_SLAM, GOLEM_SLAM_STAGES, GOLEM_SLAM_SLAM)
#define SLAM_END(index)                                                        \
    IN_STAGE(index, GOLEM_MOTION_SLAM, GOLEM_SLAM_STAGES, GOLEM_SLAM_END)
#define HANDS_COMING_DOWN(index)                                               \
    (IN_STAGE(index, GOLEM_MOTION_HANDS_DOWN_A, GOLEM_HANDS_STAGES,            \
              GOLEM_HANDS_LOWER) ||                                            \
     IN_STAGE(index, GOLEM_MOTION_HANDS_DOWN_B, GOLEM_HANDS_STAGES,            \
              GOLEM_HANDS_LOWER))
#define HANDS_HELD(index)                                                      \
    (IN_STAGE(index, GOLEM_MOTION_HANDS_DOWN_A, GOLEM_HANDS_STAGES,            \
              GOLEM_HANDS_HOLD) ||                                             \
     IN_STAGE(index, GOLEM_MOTION_HANDS_DOWN_B, GOLEM_HANDS_STAGES,            \
              GOLEM_HANDS_HOLD))
/* the hands are down from this frame of a motion that lowers them */
#define HANDS_DOWN_FRAME 35.0F

/* the flash fades in from FLASH_LIGHT_FRAME of the slam's blows, and out from
 * FLASH_FADE_FRAME of the slam */
#define FLASH_LIGHT_FRAME 2.0F
#define FLASH_FADE_FRAME 22.0F
/* the golem's sound ids. each play of WEAK_SPOT_SOUND takes the duration
 * WEAK_SPOT_SOUND_STEPS, as sound_play_task_at_for counts it */
#define FLASH_SOUND 8
#define HIT_SOUND 0
#define WEAK_SPOT_SOUND 12
#define WEAK_SPOT_SOUND_STEPS 10
/* HIT_SOUND's key, of its own: an address a byte into the task */
#define HIT_SOUND_KEY(task) ((char *)(task) + 1)
/* a hit on the weak spot shakes player 0's pad */
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* a throw lands THROW_RADIUS from the arena's center. it tops out THROW_RISE
 * plus THROW_RISE_SHARE of its length above its higher end */
#define THROW_RADIUS 240.0F
#define THROW_RISE 20.0F
#define THROW_RISE_SHARE 0.3F

/* the sensors' world flags, before their classes */
#define SENSOR_WORLD_FLAGS (WORLD_TURNED | WORLD_MOVING | WORLD_SOLID)
/* the push of the hands' and the flash's shapes, and the weak spot's second. it
 * pushes no group */
#define PUSH_RESISTS PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)

/* the flash's modes, in the order it takes them. MODE_FADE_OUT ends back at
 * MODE_WAIT */
enum flash_mode { MODE_WAIT, MODE_FADE_IN, MODE_LIT, MODE_FADE_OUT };

/* the flash's record, in its task's extra. the flash draws its model twice, the
 * second a quarter turn about y from the first. draw i takes width across and
 * height up, each times alpha and scale[i] */
struct golem_flash {
    Float width;
    Float height;
    Float alpha;
    enum flash_mode mode;
    NJS_VECTOR scale[2];
};
#define FLASH(task) ((struct golem_flash *)(task)->extra.p)

static const struct motion_table_entry golem_motions[GOLEM_MOTIONS] = {
#include "boss_golem/golem/golem_motions.inc"
};
/* clang-format off */
/* @hack type: each shape of the hands and the flash declared as an array of
 * one, where the shape alone would do */
static const struct collision_shape hand_a_shape[HAND_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static const struct collision_shape hand_b_shape[HAND_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static const struct collision_shape flash_shape[FLASH_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static const struct collision_shape weak_spot_shapes[WEAK_SPOT_SHAPES] = {
    {0, SHAPE_CYLINDER2, GOLEM_PUSH_SOLID, 0, SHAPE_UNK_2000 | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 10.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_RESISTS, 0, SHAPE_UNK_2000,
     {0.0F, 0.0F, 0.0F}, 15.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

struct golem *golem = NULL;
/* the title: EGG GOLEM above a bar */
#include "shared/golem_title_card.h"
NJS_TEXLIST *golem_texlists[13] = {
#include "boss_golem/golem/golem_texlists.inc"
};
/* each hand's point, offset from its node, and its shape's radius and size_y,
 * in three poses. the poses are striking, down and held down. hand_a_offset and
 * hand_b_offset hold the offset of the pose each hand's step last took */
static Float hand_a_strike_x = 10.0F;
static Float hand_a_strike_y = -35.0F;
static Float hand_a_strike_z = 0.0F;
static Float hand_a_strike_radius = 45.0F;
static Float hand_a_strike_size_y = 30.0F;
static Float hand_a_down_x = 10.0F;
static Float hand_a_down_y = -60.0F;
static Float hand_a_down_z = 0.0F;
static Float hand_a_down_radius = 60.0F;
static Float hand_a_down_size_y = 15.0F;
static Float hand_a_held_x = 10.0F;
static Float hand_a_held_y = -35.0F;
static Float hand_a_held_z = 0.0F;
static Float hand_a_held_radius = 15.0F;
static Float hand_a_held_size_y = 70.0F;
static NJS_VECTOR hand_a_offset = {0.0F, 0.0F, 0.0F};
static Float hand_b_strike_x = -10.0F;
static Float hand_b_strike_y = -35.0F;
static Float hand_b_strike_z = 0.0F;
static Float hand_b_strike_radius = 45.0F;
static Float hand_b_strike_size_y = 30.0F;
static Float hand_b_down_x = -10.0F;
static Float hand_b_down_y = -60.0F;
static Float hand_b_down_z = 0.0F;
static Float hand_b_down_radius = 60.0F;
static Float hand_b_down_size_y = 15.0F;
static Float hand_b_held_x = -10.0F;
static Float hand_b_held_y = -35.0F;
static Float hand_b_held_z = 0.0F;
static Float hand_b_held_radius = 15.0F;
static Float hand_b_held_size_y = 70.0F;
static NJS_VECTOR hand_b_offset = {0.0F, 0.0F, 0.0F};
/* the flash's point, offset from the head's node, and its shape's radius */
static Float flash_x = 0.0F;
static Float flash_y = 100.0F;
static Float flash_z = 10.0F;
static Float flash_radius = 45.0F;
/* the weak spot's offset from GOLEM_SENSOR_HEAD's place */
static Float weak_spot_y = 105.0F;
static Float weak_spot_z = 10.0F;
static Float dat_8c6098dc = 15.0F;
/* its first shape's center, radius and size_y, and its second shape's
 * center */
static Float weak_spot_center_y = -5.0F;
static Float weak_spot_radius = 10.0F;
static Float weak_spot_size_y = 10.0F;
static Float weak_spot_top_y = 5.0F;
/* the shapes of GOLEM_SENSOR_LOW, which takes all three, and of
 * GOLEM_SENSOR_FULL, which takes the last */
#define SENSOR_SHAPE(form, center_y, radius, size_y)                           \
    {0,                                                                        \
     form,                                                                     \
     GOLEM_PUSH_SOLID,                                                         \
     SHAPE_GUARD_LEVEL(SENSOR_GUARD_LEVEL),                                    \
     SHAPE_UNK_2000,                                                           \
     {0.0F, center_y, 0.0F},                                                   \
     radius,                                                                   \
     size_y,                                                                   \
     0.0F,                                                                     \
     0,                                                                        \
     {0, 0, 0}}
#define SENSOR_SHAPES                                                          \
    SENSOR_SHAPE(SHAPE_CYLINDER2, 47.5F, 80.0F, 5.0F),                         \
        SENSOR_SHAPE(SHAPE_CYLINDER2, 10.0F, 77.5F, 5.0F),                     \
        SENSOR_SHAPE(SHAPE_CAPSULE, 15.0F, 70.0F, 95.0F)
/* @hack type: the shapes as a union whose second view names the last, where
 * &shapes[2] of an array would do */
union golem_sensor_shapes {
    struct collision_shape all[LOW_SENSOR_SHAPES];
    struct {
        struct collision_shape first[LOW_SENSOR_SHAPES - FULL_SENSOR_SHAPES];
        struct collision_shape last[FULL_SENSOR_SHAPES];
    } parts;
};
#ifdef NONMATCHING
/* this side braces the union's first member, which GCC needs and SHC refuses.
 * the shapes are the same */
static union golem_sensor_shapes sensor_shapes = {{SENSOR_SHAPES}};
#else
/* NOLINTBEGIN(clang-diagnostic-excess-initializers) */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static union golem_sensor_shapes sensor_shapes = {SENSOR_SHAPES};
/* NOLINTEND(clang-diagnostic-missing-braces) */
/* NOLINTEND(clang-diagnostic-excess-initializers) */
#endif
/* each sensor's model, the node it rides and its range. the range picks the
 * object's size class, and is the range golem_sensor_place takes */
KEPT static NJS_OBJECT *sensor_models[GOLEM_SENSORS] = {
#include "boss_golem/golem/sensor_models.inc"
};
KEPT static NJS_CNK_OBJECT *sensor_nodes[GOLEM_SENSORS] = {
#include "boss_golem/golem/sensor_nodes.inc"
};
static Float sensor_ranges[GOLEM_SENSORS] = {
    1000.0F, 400.0F, 500.0F, 500.0F, 400.0F, 500.0F, 500.0F, 400.0F, 1000.0F,
};
/* the same points as tactics.c's hand_points */
static NJS_POINT3 dat_8c6099ec = {-42.0F, 0.0F, 203.0F};
static NJS_POINT3 dat_8c6099f8 = {28.0F, 0.0F, 190.0F};
static struct task *golem_task = NULL;
/* the volumes of HIT_SOUND and WEAK_SPOT_SOUND */
static Sint32 hit_volume = 125;
static Sint32 weak_spot_volume = 75;
static Float dat_8c609a10 = 15.0F;
static Float dat_8c609a14 = 15.0F;
/* the flash's size, before its scales. each scale is flash_scale_low plus a
 * random share of flash_scale_spread, and the second takes flash_inner_scale of
 * that. the alpha moves by flash_fade_in or flash_fade_out a step. the flash
 * turns flash_spin a step about y */
static Float flash_width = 3.0F;
static Float flash_height = 6.0F;
static Float flash_scale_spread = 0.1F;
static Float flash_scale_low = 0.95F;
static Float flash_inner_scale = 0.9F;
static Float flash_fade_in = 0.025F;
static Float flash_fade_out = 0.05F;
static Sint32 flash_volume = 0; /* FLASH_SOUND's */
static Sint32 flash_spin = 0x400;

static void golem_destroy(struct task *task);
static void golem_step(struct task *task);
static void golem_place_parts(NJS_CNK_OBJECT *object);
static void golem_draw(struct task *task);
static void hand_a_step(struct task *task);
static void hand_b_step(struct task *task);
static void flash_step(struct task *task);
static void flash_destroy(struct task *task);
static void flash_draw(struct task *task);
static void weak_spot_step(struct task *task);
static void weak_spot_destroy(struct task *task);
static int sensor_index(NJS_CNK_OBJECT *node);
static void sensor_step(struct task *task);
static void sensor_create(struct task *parent, struct task **task,
                          NJS_OBJECT *model, Float range, NJS_OBJECT **object);
static void throw_target(NJS_POINT3 *pos);
static void throw_aim(struct task *task, NJS_VECTOR *spd);
static Bool hands_down(struct task *task);
static Bool hands_held(struct task *task);

struct task *golem_create(NJS_POINT3 *pos, Angle ang_y, NJS_POINT3 *home)
{
    struct task *task;
    struct task *hand_a;
    struct task *hand_b;
    struct task *flash;
    struct task *weak_spot;
    struct task_work *work;
    struct boss_record *record;
    struct mover *mover;
    Uint32 i;

    /* @hack spelling: the assignments inside the test, where statements of
     * their own would do */
    if (!(golem_task = task = task_create(TASK_WORK | TASK_FORCE, GOLEM_LEVEL,
                                          golem_step, "BossGolemExec"))) {
        return NULL;
    }
    work = task->work;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. it destroys the
     * task where the record or the golem's state can't be made */
    record = boss_record_create(task);
    if (record == NULL) {
        golem_task = NULL;
        task_destroy(task);
        return NULL;
    }
    golem = boss_record_alloc_own(record, sizeof(*golem));
    if (golem == NULL) {
        boss_record_free_parts(task);
        golem_task = NULL;
        task_destroy(task);
        return NULL;
    }
    /* golem starts zeroed, so golem_destroy frees only the sensors made before
     * a failure */
    task->destructor = golem_destroy;
    if (!golem_platforms_create(task) || !golem_head_parts_create(task)) {
        goto failed;
    }
#else
    /* @bug memory: stores own through a null boss record, as shared/golem.h
     * says (the state's test at 0x8c500650, the store at 0x8c5008ba) */
    record = boss_record_create(task);
    golem = syMalloc(sizeof(struct golem));
    if (!golem) {
        task_kill(task);
        return NULL;
    }
    golem_platforms_create(task);
    golem_head_parts_create(task);
#endif
    /* @hack spelling: each child's assignment inside its test, where a
     * statement of its own would do */
    /* @bug logic: returns null and leaves the golem's task to step where a
     * child fails, as shared/golem.h says (the kill at 0x8c500658, the return
     * at 0x8c500678) */
    if (!(hand_a =
              task_create_child(TASK_WORK | TASK_EXTRA, hand_a_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it destroys the unfinished
         * golem */
        goto failed;
#else
        return NULL;
#endif
    }
    if (!(hand_b =
              task_create_child(TASK_WORK | TASK_EXTRA, hand_b_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged at the first child: it destroys the
         * unfinished golem */
        goto failed;
#else
        return NULL;
#endif
    }
    if (!(flash = task_create_child(TASK_WORK, flash_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged at the first child: it destroys the
         * unfinished golem */
        goto failed;
#else
        return NULL;
#endif
    }
    flash->extra.p = syMalloc(sizeof(struct golem_flash));
    /* @bug memory: writes through a null flash record where the heap can't give
     * one. flash_destroy tests the record at 0x8c50197c, but flash_step writes
     * through it at 0x8c50171c, and flash_draw reads through it at
     * 0x8c501a3a */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it destroys the golem where the
     * heap has no flash record */
    if (flash->extra.p == NULL) {
        goto failed;
    }
#endif
    flash->draw = flash_draw;
    flash->destructor = flash_destroy;
    if (!(weak_spot = task_create_child(TASK_WORK, weak_spot_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged at the first child: it destroys the
         * unfinished golem */
        goto failed;
#else
        return NULL;
#endif
    }
    weak_spot->destructor = weak_spot_destroy;
    /* @hack spelling: the shapes and delta declared in blocks of their own,
     * where declarations at the function's top would do */
    {
        /* @hack type: each shape declared as an array of one, where the shape
         * alone would do */
        struct collision_shape hand_a_shapes[HAND_SHAPES];
        struct collision_shape hand_b_shapes[HAND_SHAPES];
        struct collision_shape flash_shapes[FLASH_SHAPES];

        hand_a_shapes[0] = hand_a_shape[0];
        hand_b_shapes[0] = hand_b_shape[0];
        flash_shapes[0] = flash_shape[0];
        hand_a_shapes[0].radius = hand_a_strike_radius;
        hand_a_shapes[0].size_y = hand_a_strike_size_y;
        hand_b_shapes[0].radius = hand_b_strike_radius;
        hand_b_shapes[0].size_y = hand_b_strike_size_y;
        flash_shapes[0].radius = flash_radius;
        collision_init(hand_a, hand_a_shapes, HAND_SHAPES, COLLISION_OBJECT);
        collision_init(hand_b, hand_b_shapes, HAND_SHAPES, COLLISION_OBJECT);
        collision_init(flash, flash_shapes, FLASH_SHAPES, COLLISION_OBJECT);
    }
    {
        struct collision_shape shapes[WEAK_SPOT_SHAPES];

        shapes[0] = weak_spot_shapes[0];
        shapes[1] = weak_spot_shapes[1];
        shapes[0].center.x = 0.0F;
        shapes[0].center.y = weak_spot_center_y;
        shapes[0].radius = weak_spot_radius;
        shapes[0].size_y = weak_spot_size_y;
        shapes[1].center.y = weak_spot_top_y;
        collision_init(weak_spot, shapes, WEAK_SPOT_SHAPES, COLLISION_OBJECT);
    }
#ifdef NONMATCHING
    /* this side destroys the golem where collision_init gave a child no
     * record */
    if (hand_a->work->collision == NULL || hand_b->work->collision == NULL ||
        flash->work->collision == NULL || weak_spot->work->collision == NULL) {
        goto failed;
    }
#endif
    record->own = golem;
    motion_init(record->block, golem_motions);
    motion_set(record->block, GOLEM_MOTION_OPENING);
    mover = task->motion.p;
    mover->home = *home;
    mover->floor_y = home->y;
    work->pos = *pos;
    work->ang.y = ang_y;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    {
        NJS_VECTOR delta;

        delta.x = work->pos.x - mover->home.x;
        delta.y = 0.0F;
        /* @bug logic: delta.z takes pos.x less home.z, as shared/golem.h says
         * (0x8c500926 reads pos.x, as delta.x's line does at 0x8c500912) */
        delta.z = work->pos.x - mover->home.z;
        /* the golem's distance from home, stored in the mover's wall_bounce.
         * no code reads it there: no mover step runs on the golem's task */
        mover->wall_bounce = njSqrt((delta.x * delta.x) + (delta.z * delta.z));
    }
    mover_set_gravity(task, GOLEM_GRAVITY);
#ifdef NONMATCHING
    for (i = 0; i < GOLEM_SENSORS; i++) {
        sensor_create(task, &golem_sensor_tasks[i], sensor_models[i],
                      sensor_ranges[i], &golem->sensors[i]);
        if (golem->sensors[i] == NULL ||
            ((i == GOLEM_SENSOR_LOW || i == GOLEM_SENSOR_FULL) &&
             golem_sensor_tasks[i]->work->collision == NULL)) {
            goto failed;
        }
    }
    /* this side moves player 0 only once each sensor it needs exists */
    player_set_pos(0, 0.0F, golem_floor_y, PLAYER_START_Z);
#else
    player_set_pos(0, 0.0F, golem_floor_y, PLAYER_START_Z);
    for (i = 0; i < GOLEM_SENSORS; i++) {
        sensor_create(task, &golem_sensor_tasks[i], sensor_models[i],
                      sensor_ranges[i], &golem->sensors[i]);
    }
#endif
    task->draw = golem_draw;
    task->destructor = golem_destroy;
    golem->dark_story = FALSE;
    golem_camera_set(task, GOLEM_CAMERA_INTRO);
    golem_action_start(task, GOLEM_ACTION_WAIT);
    golem->power_max = golem->power = GOLEM_POWER;
    golem->power_low = GOLEM_POWER_LOW;
    golem->turn = GOLEM_TURN_TO_PLAYER;
    golem->slot = 0;
    golem->heading = 0;
    golem->aim_heading = 0;
    golem->lift = 0.0F;
    golem->floor_goal = golem->floor_y = golem_floor_y;
    golem->throwing = FALSE;
    golem->unk_58 = 0;
    golem->camera_far_started = FALSE;
    golem->hands_down_state = FOOTHOLD_WHOLE;
    golem->slam_state = FOOTHOLD_WHOLE;
    golem->rest_steps = 0;
    golem->rest_wait = 0;
    golem->up_steps = 0;
    boss_gauge_create(task, &golem->power);
    boss_title_create(task, &golem_title);
    return task;

#ifdef NONMATCHING
/* this side's way out of a create that fails: task_destroy runs golem_destroy,
 * which frees what the create made */
failed:
    task_destroy(task);
    return NULL;
#endif
}

static void golem_destroy(struct task *task)
{
    Uint32 i;

    for (i = 0; i < GOLEM_SENSORS; i++) {
        if (golem->sensors[i]) {
            world_remove(golem_sensor_tasks[i], golem->sensors[i]);
            world_pool_free(golem->sensors[i]);
        }
#ifdef NONMATCHING
        /* this side clears the sensor's task, which dies with the golem, for a
         * golem_create that runs again */
        golem_sensor_tasks[i] = NULL;
#endif
    }
    boss_record_free_parts(task);
    golem = NULL;
#ifdef NONMATCHING
    /* this side clears golem_task and the tables of the golem's children, for a
     * golem_create that runs again. the tasks they name die with the golem */
    memset(golem_platform_tasks, 0, sizeof(golem_platform_tasks));
    memset(golem_head_tasks, 0, sizeof(golem_head_tasks));
    memset(golem_head_targets, 0, sizeof(golem_head_targets));
    golem_task = NULL;
#endif
}

static void golem_step(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion;

    golem->hold_motion = FALSE;
    golem_camera_step(task);
    golem_actions_step(task);
    golem_hover(task);
    golem->rest_steps++;
    golem_set_rest_wait();
    motion = record->block;
    if (!golem->hold_motion) {
        motion_step(motion);
    }
    if (golem->camera_mode) {
        golem_rubble_throw(task, GOLEM_SENSOR_HAND_A);
        golem_rubble_throw(task, GOLEM_SENSOR_HAND_B);
        golem_rubble_throw(task, GOLEM_SENSOR_FOREARM_A);
        golem_rubble_throw(task, GOLEM_SENSOR_FOREARM_B);
    }
    golem_count_up_steps(task);
}

/* the motion callback of the golem's draw, for each node with its matrix
 * current. it places the parts that ride the node, and keeps the hands' and
 * the flash's points. GOLEM_SENSOR_FULL's task collides at each draw, and
 * GOLEM_SENSOR_LOW's only while player 0 stands on platform B or C */
static void golem_place_parts(NJS_CNK_OBJECT *object)
{
    int index;
    /* @hack spelling: node, a copy of object, where object would do */
    NJS_CNK_OBJECT *node = object;

    if ((index = sensor_index(node)) != NO_SENSOR) {
        golem_sensor_place(golem_sensor_tasks[index], golem->sensors[index],
                           &golem_sensor_matrices[index], sensor_ranges[index],
                           &golem_sensor_bases[index]);
        if ((index == GOLEM_SENSOR_LOW && player_work[0] &&
             (player_work[0]->flags & PLAYER_WORK_ON_GROUND) &&
             player_records[0] && player_records[0]->ground_task &&
             player_records[0]->ground_task->work &&
             (player_records[0]->ground_task->work->id ==
                  WORK_ID_GOLEM_PLATFORM_B ||
              player_records[0]->ground_task->work->id ==
                  WORK_ID_GOLEM_PLATFORM_C)) ||
            index == GOLEM_SENSOR_FULL) {
            golem_sensor_tasks[index]->work->pos.x =
                golem->sensors[index]->pos[0];
            golem_sensor_tasks[index]->work->pos.y =
                golem->sensors[index]->pos[1];
            golem_sensor_tasks[index]->work->pos.z =
                golem->sensors[index]->pos[2];
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateZ(NULL, golem->sensors[index]->ang[2]);
            njRotateY(NULL, golem->sensors[index]->ang[1]);
            njRotateX(NULL, golem->sensors[index]->ang[0]);
            {
                struct task *task;
                struct task_work *work;
                /* @hack redundant: twelve bytes of the frame that no code
                 * touches */
                unsigned char unused[12];
                /* @hack redundant: kept, a copy of the sensor's task that
                 * nothing reads */
                struct task *kept;
                struct task *sensor = golem_sensor_tasks[index];

                (void)&kept;
                /* @hack spelling: task, a copy of sensor, where sensor would
                 * do */
                task = sensor;
                kept = sensor;
                work = task->work;
                current_matrix_angles_yxz(&work->ang.x, &work->ang.y,
                                          &work->ang.z);
            }
            njPopMatrixEx();
            collision_register(golem_sensor_tasks[index]);
        }
        switch (index) {
        case GOLEM_SENSOR_HEAD:
            golem_head_parts_place();
            break;
        case GOLEM_SENSOR_LOW:
            golem_platforms_place();
            break;
        }
    }
    if (object == &golem_statue_head) {
        njPushMatrixEx();
        matrix_multiply_after_pairs(&golem_view_inverse);
        njTranslate(NULL, flash_x, flash_y, flash_z);
        njGetTranslation(NULL, &golem_flash_pos);
        current_matrix_angles_yxz(&golem_flash_ang.x, &golem_flash_ang.y,
                                  &golem_flash_ang.z);
        njPopMatrixEx();
    }
    if (object == &golem_statue_hand_a) {
        njPushMatrixEx();
        matrix_multiply_after_pairs(&golem_view_inverse);
        njTranslate(NULL, hand_a_offset.x, hand_a_offset.y, hand_a_offset.z);
        njGetTranslation(NULL, &golem_hand_a_pos);
        current_matrix_angles_yxz(&golem_hand_a_ang.x, &golem_hand_a_ang.y,
                                  &golem_hand_a_ang.z);
        njPopMatrixEx();
    }
    if (object == &golem_statue_hand_b) {
        njPushMatrixEx();
        matrix_multiply_after_pairs(&golem_view_inverse);
        njTranslate(NULL, hand_b_offset.x, hand_b_offset.y, hand_b_offset.z);
        njGetTranslation(NULL, &golem_hand_b_pos);
        current_matrix_angles_yxz(&golem_hand_b_ang.x, &golem_hand_b_ang.y,
                                  &golem_hand_b_ang.z);
        njPopMatrixEx();
    }
}

/* keep the inverse of the view, which must be the current matrix */
static void golem_start_placing(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;

    matrix_get(&golem_view_inverse);
    njInvertMatrix(&golem_view_inverse);
    njCnkSetMotionCallback(golem_place_parts);
}

static void golem_draw(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct task_work *work = task->work;
    struct motion_control *motion = record->block;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    golem_start_placing(task);
    njTranslate(NULL, work->pos.x, work->pos.y + golem->lift, work->pos.z);
    njRotateY(NULL, golem->heading);
    njSetTexture(&golem_statue_texlist);
    motion_draw_simple(&golem_statue, motion);
    njCnkSetMotionCallback(NULL);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* a hand follows its point and its node's turn, as the golem's draw last found
 * them. its shape takes one of three poses, and collides only in them. while
 * the hand strikes, the shape turns a quarter turn about x from its node's.
 * once the hands are down, it stands upright. while they are held down, it
 * lies level, pointing the way its node's y axis leans. hand_b_step is hand
 * B's */
static void hand_a_step(struct task *task)
{
    /* @hack redundant: work declared register, where a plain local would do */
    register struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    Sint32 index;

    work->pos = golem_hand_a_pos;
    work->ang.x = golem_hand_a_ang.x;
    work->ang.y = golem_hand_a_ang.y;
    work->ang.z = golem_hand_a_ang.z;
    index = motion_index(motion);
    if (HAND_A_STRIKES(index)) {
        hand_a_offset.x = hand_a_strike_x;
        hand_a_offset.y = hand_a_strike_y;
        hand_a_offset.z = hand_a_strike_z;
        work->collision->shapes[0].radius = hand_a_strike_radius;
        work->collision->shapes[0].size_y = hand_a_strike_size_y;
        work->collision->shapes[0].ang.x = QUARTER_TURN;
        /* @hack spelling: the fit inside the register's argument, where a
         * statement of its own would do */
        collision_register((collision_fit_radius(task), task));
    } else if (hands_down(task)) {
        work->ang.x = work->ang.y = work->ang.z = 0;
        hand_a_offset.x = hand_a_down_x;
        hand_a_offset.y = hand_a_down_y;
        hand_a_offset.z = hand_a_down_z;
        work->collision->shapes[0].radius = hand_a_down_radius;
        work->collision->shapes[0].size_y = hand_a_down_size_y;
        work->collision->shapes[0].ang.x = 0;
        collision_fit_radius(task);
        collision_register(task);
    } else if (hands_held(task)) {
        NJS_VECTOR up;
        NJS_VECTOR dir;

        up.x = up.z = 0.0F;
        up.y = 1.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njCalcVector(NULL, &up, &dir);
        direction_angles_xz(dir.x, 0.0F, dir.z, &work->ang.x, &work->ang.z);
        njPopMatrixEx();
        hand_a_offset.x = hand_a_held_x;
        hand_a_offset.y = hand_a_held_y;
        hand_a_offset.z = hand_a_held_z;
        work->collision->shapes[0].radius = hand_a_held_radius;
        work->collision->shapes[0].size_y = hand_a_held_size_y;
        work->collision->shapes[0].ang.x = 0;
        collision_fit_radius(task);
        collision_register(task);
    }
}

static void hand_b_step(struct task *task)
{
    /* @hack redundant: work declared register, where a plain local would do */
    register struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    Sint32 index;

    work->pos = golem_hand_b_pos;
    work->ang.x = golem_hand_b_ang.x;
    work->ang.y = golem_hand_b_ang.y;
    work->ang.z = golem_hand_b_ang.z;
    index = motion_index(motion);
    if (HAND_B_STRIKES(index)) {
        hand_b_offset.x = hand_b_strike_x;
        hand_b_offset.y = hand_b_strike_y;
        hand_b_offset.z = hand_b_strike_z;
        work->collision->shapes[0].radius = hand_b_strike_radius;
        work->collision->shapes[0].size_y = hand_b_strike_size_y;
        work->collision->shapes[0].ang.x = QUARTER_TURN;
        /* @hack spelling: the fit inside the register's argument, where a
         * statement of its own would do */
        collision_register((collision_fit_radius(task), task));
    } else if (hands_down(task)) {
        work->ang.x = work->ang.y = work->ang.z = 0;
        hand_b_offset.x = hand_b_down_x;
        hand_b_offset.y = hand_b_down_y;
        hand_b_offset.z = hand_b_down_z;
        work->collision->shapes[0].radius = hand_b_down_radius;
        work->collision->shapes[0].size_y = hand_b_down_size_y;
        work->collision->shapes[0].ang.x = 0;
        collision_fit_radius(task);
        collision_register(task);
    } else if (hands_held(task)) {
        NJS_VECTOR up;
        NJS_VECTOR dir;

        up.x = up.z = 0.0F;
        up.y = 1.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njCalcVector(NULL, &up, &dir);
        direction_angles_xz(dir.x, 0.0F, dir.z, &work->ang.x, &work->ang.z);
        njPopMatrixEx();
        hand_b_offset.x = hand_b_held_x;
        hand_b_offset.y = hand_b_held_y;
        hand_b_offset.z = hand_b_held_z;
        work->collision->shapes[0].radius = hand_b_held_radius;
        work->collision->shapes[0].size_y = hand_b_held_size_y;
        work->collision->shapes[0].ang.x = 0;
        collision_fit_radius(task);
        collision_register(task);
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw task's first shape as golem_cylinder_draw's cylinder, at the shape's
 * sizes. the cylinder turns as task's work, then by the shape's ang.x. nothing
 * calls it. it is external because SHC leaves out a static function that
 * nothing names */
void golem_shape_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR size;

    njPushMatrixEx();
    size.x = size.z = work->collision->shapes->radius;
    size.y = work->collision->shapes->size_y * 2.0F;
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->collision->shapes->ang.x);
    njTranslate(NULL, 0.0F, -(size.y * 0.5F), 0.0F);
    golem_cylinder_draw(&size);
    njPopMatrixEx();
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the flash follows its point, as the golem's draw last found it. it acts and
 * draws only in the slam's last three motions: the blows, the slam and the
 * end. the blows' first frame sets it back to MODE_WAIT, unlit, and plays
 * FLASH_SOUND. it collides only during the slam. each step that it acts gives
 * its scales new random shares */
static void flash_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    Sint32 index;
    Float share;
    /* @hack redundant: frame, a read of the motion's frame that nothing uses */
    Float frame = motion->current.frame;

    work->pos = golem_flash_pos;
    index = motion_index(motion);
    if (SLAM_BLOWS(index) || SLAM_SLAM(index) || SLAM_END(index)) {
        if (SLAM_BLOWS(index) && motion_frame(motion) == 0.0F) {
            FLASH(task)->mode = MODE_WAIT;
            FLASH(task)->alpha = 0.0F;
            sound_play(FLASH_SOUND, task, 0, 0);
            sound_set_volume(task, SOUND_ANY_ID, flash_volume);
        }
        switch (FLASH(task)->mode) {
        case MODE_WAIT:
            if (SLAM_BLOWS(index) &&
                motion_frame(motion) >= FLASH_LIGHT_FRAME) {
                FLASH(task)->alpha = 0.0F;
                FLASH(task)->mode++;
            }
            break;
        case MODE_FADE_IN:
            FLASH(task)->alpha += flash_fade_in;
            if (FLASH(task)->alpha >= 1.0F) {
                FLASH(task)->alpha = 1.0F;
                FLASH(task)->mode++;
            }
            break;
        case MODE_LIT:
            if (SLAM_SLAM(index) && motion_frame(motion) >= FLASH_FADE_FRAME) {
                FLASH(task)->mode++;
            }
            break;
        case MODE_FADE_OUT:
            FLASH(task)->alpha -= flash_fade_out;
            if (FLASH(task)->alpha <= 0.0F) {
                FLASH(task)->alpha = 0.0F;
                FLASH(task)->mode = MODE_WAIT;
            }
            break;
        }
        FLASH(task)->width = flash_width;
        FLASH(task)->height = flash_height;
        /* @hack spelling: share set ahead of the stores, where its value in
         * place would do */
        share = 1.0F / 32768.0F;
        FLASH(task)->scale[0].x =
            flash_scale_low + ((Float)rand() * flash_scale_spread * share);
        FLASH(task)->scale[0].y =
            flash_scale_low + ((Float)rand() * flash_scale_spread * share);
        FLASH(task)->scale[0].z =
            flash_scale_low + ((Float)rand() * flash_scale_spread * share);
        FLASH(task)->scale[1].x =
            (flash_scale_low + ((Float)rand() * flash_scale_spread * share)) *
            flash_inner_scale;
        FLASH(task)->scale[1].y =
            (flash_scale_low + ((Float)rand() * flash_scale_spread * share)) *
            flash_inner_scale;
        FLASH(task)->scale[1].z =
            (flash_scale_low + ((Float)rand() * flash_scale_spread * share)) *
            flash_inner_scale;
    }
    if (SLAM_SLAM(index)) {
        work->collision->shapes->radius = flash_radius;
        collision_fit_radius(task);
        collision_register(task);
    }
}

static void flash_destroy(struct task *task)
{
    sound_stop_key(task);
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

/* the flash adds its color, times its alpha, to what lies behind it */
static void flash_draw(struct task *task)
{
    /* @hack spelling: the record read through a cast in index's initializer,
     * where a local would do */
    Sint32 index =
        motion_index(((struct boss_record *)task->parent->extra.p)->block);
    Float alpha;
    NJS_VECTOR size;

    if (SLAM_BLOWS(index) || SLAM_SLAM(index) || SLAM_END(index)) {
        alpha = FLASH(task)->alpha;
        njPushMatrixEx();
        njTranslateEx(&golem_flash_pos);
        njRotateZ(NULL, golem_flash_ang.z);
        njRotateX(NULL, golem_flash_ang.x);
        njRotateY(NULL, (game_state.steps * flash_spin) + golem_flash_ang.y);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
        njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
        constant_material_set(alpha, 1.0F, 1.0F, 1.0F);
        njSetConstantAttr(~NJD_FST_IL, 0);
        size.x = size.z = FLASH(task)->width;
        size.y = FLASH(task)->height;
        size.x *= alpha * FLASH(task)->scale[0].x;
        size.y *= alpha * FLASH(task)->scale[0].y;
        size.z *= alpha * FLASH(task)->scale[0].z;
        njPushMatrixEx();
        njSetTexture(&golem_flash_texlist);
        golem_model_draw_scaled_v(golem_flash_object->model, &size);
        njPopMatrixEx();
        size.x = size.z = FLASH(task)->width;
        size.y = FLASH(task)->height;
        size.x *= alpha * FLASH(task)->scale[1].x;
        size.y *= alpha * FLASH(task)->scale[1].y;
        size.z *= alpha * FLASH(task)->scale[1].z;
        njRotateY(NULL, QUARTER_TURN);
        golem_model_draw_scaled_v(golem_flash_object->model, &size);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
        njPopMatrixEx();
    }
}

/* the weak spot rides GOLEM_SENSOR_HEAD's place, as a target. a player's hit on
 * it takes HIT_POWER from the golem. the hit throws player 0 to a slot of the
 * arena, as throw_aim aims it. through the throw, player 0's pad is disabled
 * and the weak spot doesn't collide. outside a throw, WEAK_SPOT_SOUND plays at
 * its pos */
static void weak_spot_step(struct task *task)
{
    struct task_work *work;
    struct collision_work *collision;
    /* @hack redundant: hit, which nothing reads */
    struct task *hit;
    NJS_VECTOR spd;

    work = task->work;
    collision = work->collision;
    njPushMatrixEx();
    matrix_set(&golem_sensor_matrices[GOLEM_SENSOR_HEAD]);
    njTranslate(NULL, 0.0F, weak_spot_y, weak_spot_z);
    njGetTranslation(NULL, &work->pos);
    current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
    njPopMatrixEx();
    golem->weak_spot_pos = work->pos;
    /* @hack spelling: the record read again through task->work, where collision
     * would do */
    task->work->collision->flags |= COLLISION_TARGET;
    if (!golem->throwing) {
        if ((collision->flags & COLLISION_HURT) &&
            (hit = collision_next_player(task)) != NULL) {
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
            work->flags &= ~WORK_HURT;
            throw_aim(task, &golem->throw_spd);
            player_bounce(0, golem->throw_spd.x, golem->throw_spd.y,
                          golem->throw_spd.z);
            vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
            golem->power -= HIT_POWER;
            golem->throwing = TRUE;
            golem->unk_58 = 0;
            pad_disable_player(0);
            blast_create(BLAST_LARGE_RAISED, &player_work[0]->pos);
            sound_play(HIT_SOUND, HIT_SOUND_KEY(task), 0, 0);
            sound_set_volume(HIT_SOUND_KEY(task), SOUND_ANY_ID, hit_volume);
        }
        sound_play_task_at_for(WEAK_SPOT_SOUND, task, 0, weak_spot_volume,
                               WEAK_SPOT_SOUND_STEPS);
    } else {
        spd = golem->throw_spd;
        if (golem->throw_step == golem->rise_steps) {
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
        }
        if (golem->throw_step < golem->rise_steps) {
            player_bounce(0, spd.x,
                          spd.y - (player_records[0]->consts.gravity *
                                   (Float)golem->throw_step),
                          spd.z);
        } else {
            player_set_speed(0, spd.x,
                             spd.y - (player_records[0]->consts.gravity *
                                      (Float)golem->throw_step),
                             spd.z);
        }
        work->flags &= ~WORK_HURT;
        if (++golem->throw_step == golem->throw_steps) {
            golem->throwing = FALSE;
            pad_enable_player(0);
        } else {
            pad_disable_player(0);
        }
    }
    if (!golem->throwing) {
        collision_register(task);
    }
}

static void weak_spot_destroy(struct task *task)
{
}

/* the sensor that rides node, or NO_SENSOR */
static int sensor_index(NJS_CNK_OBJECT *node)
{
    int i;

    for (i = 0; i < GOLEM_SENSORS; i++) {
        if (node == sensor_nodes[i]) {
            return i;
        }
    }
    return NO_SENSOR;
}

static void sensor_step(struct task *task)
{
}

void golem_sensor_place(struct task *task, NJS_OBJECT *object,
                        NJS_MATRIX *matrix, Float range, NJS_MATRIX *base)
{
    NJS_POINT3 pos;
    NJS_POINT3 local;
    NJS_POINT3 move;
    struct angles turn;

    njSetMatrix(base, matrix);
    njPushMatrixEx();
    matrix_multiply_after_pairs(&golem_view_inverse);
    matrix_get(matrix);
    njGetTranslation(NULL, &pos);
    object->pos[0] = pos.x;
    object->pos[1] = pos.y;
    object->pos[2] = pos.z;
    current_matrix_angles_xyz(&object->ang[0], &object->ang[1],
                              &object->ang[2]);
    njPopMatrixEx();
    if (!player_near(&pos, range + SENSOR_REACH)) {
        task->work->flags &= ~WORLD_ADDED;
        return;
    }
    task->work->flags |= WORLD_ADDED;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_set(base);
    njInvertMatrix(NULL);
    njCalcPoint(NULL, &player_work[0]->pos, &local);
    matrix_multiply_before(matrix);
    current_matrix_angles_yxz(&turn.x, &turn.y, &turn.z);
    njPopMatrixEx();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    matrix_set(matrix);
    njCalcPoint(NULL, &local, &move);
    njPopMatrixEx();
    move.x -= player_work[0]->pos.x;
    move.y -= player_work[0]->pos.y;
    move.z -= player_work[0]->pos.z;
    TASK_FORCES_OF(task)->speed = move;
    TASK_FORCES_OF(task)->turn.x = turn.x;
    TASK_FORCES_OF(task)->turn.y = -turn.y;
    TASK_FORCES_OF(task)->turn.z = turn.z;
}

/* create a sensor's task in *task, as parent's child. its object in *object is
 * a copy of model at the origin, in range's size class. the head's and the low
 * sensor's objects crash a player who runs into them fast enough */
static void sensor_create(struct task *parent, struct task **task,
                          NJS_OBJECT *model, Float range, NJS_OBJECT **object)
{
    NJS_OBJECT *copy;
    Sint32 flags;

    copy = world_pool_alloc();
    /* @bug memory: writes through a null object, as shared/golem.h says
     * (golem_destroy's test at 0x8c500b5c, the write at 0x8c502108) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it makes no sensor where the pool
     * has no object */
    if (copy == NULL) {
        return;
    }
#endif
    *task = task_create_child(TASK_WORK | TASK_FORCE, sensor_step, parent);
    /* @bug memory: reads through a null task, as shared/golem.h says
     * (golem_create's test at 0x8c500678, the read at 0x8c502196) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. where task_create_child fails, it
     * gives the object back and makes no sensor */
    if (*task == NULL) {
        world_pool_free(copy);
        return;
    }
#endif
    *object = copy;
    *copy = *model;
    copy->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    copy->scl[0] = copy->scl[1] = copy->scl[2] = 1.0F;
    copy->ang[0] = copy->ang[1] = copy->ang[2] = 0;
    copy->pos[0] = copy->pos[1] = copy->pos[2] = 0.0F;
    flags = SENSOR_WORLD_FLAGS;
    flags |= golem_size_class_flags(range);
    if (model == &golem_sensor_head || model == &golem_sensor_low) {
        flags |= GROUND_CRASHES;
    }
    world_add(flags, *task, copy);
    (*task)->work->id = WORK_ID_GOLEM_SENSOR;
    if (model == &golem_sensor_low) {
        collision_init(*task, sensor_shapes.all, LOW_SENSOR_SHAPES,
                       COLLISION_OBJECT);
    } else if (model == &golem_sensor_full) {
        collision_init(*task, sensor_shapes.parts.last, FULL_SENSOR_SHAPES,
                       COLLISION_OBJECT);
    }
}

/* set *pos to a point on the floor, THROW_RADIUS from the arena's center. the
 * point takes the heading of the first slot with a whole foothold. without
 * one, it takes that of the first slot whose foothold is sunk once. with
 * neither, *pos is (THROW_RADIUS, golem_floor_y, 0) */
static void throw_target(NJS_POINT3 *pos)
{
    int pass;
    int slot;
    Angle ang;

    for (pass = FOOTHOLD_WHOLE; pass <= FOOTHOLD_SUNK; pass++) {
        for (slot = 0; slot < GOLEM_SLOTS; slot++) {
            if (golem_foothold_state(slot) == pass) {
                /* @hack spelling: ang set ahead of the call, where its value in
                 * place would do */
                ang = slot * GOLEM_SLOT_TURN;
                golem_polar_xz(ang, THROW_RADIUS, &pos->x, &pos->z);
                pos->y = golem_floor_y;
                return;
            }
        }
    }
    pos->x = THROW_RADIUS;
    pos->y = golem_floor_y;
    pos->z = 0.0F;
}

/* aim a throw of player 0 from task's pos to throw_target's point, under player
 * 0's gravity. it turns player 0 along the throw. *spd takes the throw's speed
 * at its start. golem's rise_steps takes the throw's steps to its top, and
 * throw_steps its steps in all. golem's count of the steps so far, throw_step,
 * starts at 0 */
static void throw_aim(struct task *task, NJS_VECTOR *spd)
{
    struct task_work *work;
    /* @hack spelling: the vectors declared in this order, where target, land,
     * pos and dist would do */
    NJS_VECTOR dist;
    /* @hack spelling: pos and land, copies of work's pos and of target, where
     * those would do */
    NJS_POINT3 pos;
    NJS_POINT3 land;
    NJS_POINT3 target;
    Float top;
    Float rise;
    Float fall;
    Float time;
    Float length;
    Float height;
    Angle ang;

    work = task->work;
    throw_target(&target);
    pos = work->pos;
    land = target;
    top = pos.y;
    if (land.y > top) {
        top = land.y;
    }
    dist.x = target.x - work->pos.x;
    dist.y = 0.0F;
    dist.z = target.z - work->pos.z;
    ang = golem_heading(dist.x, dist.z);
    player_set_angles(0, 0, ang, 0);
    length = njScalor(&dist);
    top += (length * THROW_RISE_SHARE) + THROW_RISE;
    /* @hack spelling: height set ahead of the call, where its value in place
     * would do */
    height = top - pos.y;
    golem->rise_steps = rise = njSqrt(
        _builtin_fabsf(height * 2.0F / player_records[0]->consts.gravity));
    fall = njSqrt(_builtin_fabsf((top - land.y) * 2.0F /
                                 player_records[0]->consts.gravity));
    golem->throw_steps = time = rise + fall;
    golem->throw_step = 0;
    spd->x = (target.x - work->pos.x) / time;
    spd->y = player_records[0]->consts.gravity * rise;
    spd->z = (target.z - work->pos.z) / time;
}

/* TRUE from HANDS_DOWN_FRAME of a motion that brings the hands down. task is a
 * child of the golem's */
static Bool hands_down(struct task *task)
{
    struct task *parent;
    struct boss_record *record;
    struct motion_control *motion;
    Float frame;
    Sint32 index;

    parent = task->parent;
    record = parent->extra.p;
    motion = record->block;
    frame = motion_frame(motion);
    index = motion_index(motion);
    return HANDS_COMING_DOWN(index) && frame >= HANDS_DOWN_FRAME;
}

/* TRUE in a motion that holds the hands down, while golem_holding is TRUE for
 * the golem. task is a child of the golem's */
static Bool hands_held(struct task *task)
{
    struct task *parent;
    struct boss_record *record;
    struct motion_control *motion;
    Sint32 index;

    parent = task->parent;
    record = parent->extra.p;
    motion = record->block;
    /* @hack redundant: a call of motion_frame whose result nothing reads */
    motion_frame(motion);
    index = motion_index(motion);
    return HANDS_HELD(index) && golem_holding(parent);
}
