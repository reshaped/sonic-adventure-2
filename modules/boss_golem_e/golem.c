/* the dark story's Egg Golem, the boss of BOSS_GOLEM_E.PRS, and the parts that
 * ride its model's nodes. the golem is the task BossGolemExec, which the
 * stage's step creates. its twin in BOSS_GOLEM.PRS, boss_golem/golem.c, has the
 * same hands and sensors. this golem's head, which hurts in the slam, rides the
 * node where the twin's flash rides. it has no weak spot, and its sensors and
 * weak points take the hits instead */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_golem/footholds.h"
#include "boss_golem_e/actions.h"
#include "boss_golem_e/attachment_art.h"
#include "boss_golem_e/footholds.h"
#include "boss_golem_e/golem.h"
#include "boss_golem_e/stage.h"
#include "boss_golem_e/tactics.h"
#include "boss_golem_e/weak_points.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/motion.h"
#include "object/mover.h"
#include "player/player.h"
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
#include "system/math.h"
#include "system/matrix.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

/* the inverse of the view matrix, which the golem's draw keeps. it takes a
 * node's matrix into the world's axes */
static NJS_MATRIX golem_e_view_inverse;
static struct task *golem_e_sensor_tasks[GOLEM_SENSORS];

#define GOLEM_LEVEL 2
#define GOLEM_GRAVITY (-0.02F)
#define GOLEM_POWER 6.0F
#define PLAYER_START_Z 210.0F
/* the hands' and the head's attack level in a hit */
#define ATTACK_LEVEL 2
/* the shapes each part takes */
#define HAND_SHAPES 1
#define HEAD_SHAPES 1
#define SENSOR_SHAPES 1

/* sensor_place leaves a sensor's object in the world while a player lies within
 * the sensor's range plus SENSOR_REACH */
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
#define HANDS_DOWN_FRAME 36.0F

/* the sensors' world flags, before their classes. sensor_create adds
 * GROUND_CRASHES to the head's and the low sensor's */
#define SENSOR_WORLD_FLAGS (WORLD_TURNED | WORLD_MOVING | WORLD_SOLID)
#define SENSOR_START_SIZE 100.0F

/* each hit's _COUNT adds to golem_e's hits, and its _POWER comes off golem_e's
 * power */
#define HITS_MAX 60
#define SENSOR_HIT_COUNT 1
#define SENSOR_HIT_POWER 0.025F
#define WEAK_POINT_HIT_COUNT 3
#define WEAK_POINT_HIT_POWER 0.075F
/* an attacker HIT_FLOOR_MARGIN or more below golem_e_floor_y can't hit a
 * sensor */
#define HIT_FLOOR_MARGIN 20.0F
#define HIT_SOUND 11
/* HIT_SOUND's key, of its own: an address 8 bytes into the sensor's task.
 * sound_set_volume finds the sound by its key alone */
#define HIT_SOUND_KEY(task) ((char *)(task) + 8)

/* the form, center and sizes sensor_place gives a sensor's shape */
struct sensor_geometry {
    Uint8 form;
    NJS_POINT3 center;
    Float radius;
    Float size_y;
};

static const struct motion_table_entry golem_motions[GOLEM_MOTIONS] = {
#include "boss_golem_e/golem/golem_motions.inc"
};
/* clang-format off */
/* @hack type: each shape of the hands and the head declared as an array of
 * one, where the shape alone would do */
static const struct collision_shape hand_a_shape[HAND_SHAPES] = {
    {0, SHAPE_CYLINDER, GOLEM_E_PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static const struct collision_shape hand_b_shape[HAND_SHAPES] = {
    {0, SHAPE_CYLINDER, GOLEM_E_PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static const struct collision_shape head_shape[HEAD_SHAPES] = {
    {0, SHAPE_SPHERE, GOLEM_E_PUSH_RESISTS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 6.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

struct golem *golem_e = NULL;
Bool golem_e_parts_built = FALSE;
/* the title: EGG GOLEM above a bar */
#include "shared/golem_title_card.h"
NJS_TEXLIST *golem_e_texlists[11] = {
#include "boss_golem_e/golem/golem_e_texlists.inc"
};
/* each hand's point, offset from its node, and its shape's sizes. the _strike_
 * sizes hold while it strikes, and the _down_ sizes while the hands are down */
static Float hand_a_x = 10.0F;
static Float hand_a_y = -35.0F;
static Float hand_a_z = 0.0F;
static Float hand_a_strike_radius = 45.0F;
static Float hand_a_strike_size_y = 20.0F;
static Float hand_a_down_radius = 65.0F;
static Float hand_a_down_size_y = 5.0F;
static Float hand_b_x = -10.0F;
static Float hand_b_y = -35.0F;
static Float hand_b_z = 0.0F;
static Float hand_b_strike_radius = 45.0F;
static Float hand_b_strike_size_y = 20.0F;
static Float hand_b_down_radius = 65.0F;
static Float hand_b_down_size_y = 5.0F;
/* the head's point, offset from its node, and its shape's radius */
static Float head_x = 0.0F;
static Float head_y = 100.0F;
static Float head_z = 10.0F;
static Float head_radius = 45.0F;
/* each sensor's model, the node it rides and its range. the range picks the
 * object's size class, and is the range sensor_place takes */
KEPT static NJS_OBJECT *sensor_models[GOLEM_SENSORS] = {
#include "boss_golem_e/golem/sensor_models.inc"
};
KEPT static NJS_CNK_OBJECT *sensor_nodes[GOLEM_SENSORS] = {
#include "boss_golem_e/golem/sensor_nodes.inc"
};
static Float sensor_ranges[GOLEM_SENSORS] = {
    1000.0F, 400.0F, 500.0F, 500.0F, 400.0F, 500.0F, 500.0F, 400.0F, 1000.0F,
};
static struct task *golem_task = NULL;
/* clang-format off */
/* the shape each sensor's collision record copies. sensor_create makes the
 * copy a SHAPE_CYLINDER of SENSOR_START_SIZE, and sensor_place gives it its
 * form and sizes from sensor_geometries */
static struct collision_shape sensor_shape[SENSOR_SHAPES] = {
    {0, SHAPE_BOX, GOLEM_E_PUSH_RESISTS, SHAPE_GUARD_LEVEL(GOLEM_E_GUARD_LEVEL),
     SHAPE_UNK_2000, {0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static struct sensor_geometry sensor_geometries[GOLEM_SENSORS] = {
    {SHAPE_SPHERE, {0.0F, 50.0F, 0.0F}, 110.0F, 0.0F},
    {SHAPE_CYLINDER2, {0.0F, 50.0F, 0.0F}, 55.0F, 50.0F},
    {SHAPE_CYLINDER2, {0.0F, -25.0F, 0.0F}, 30.0F, 40.0F},
    {SHAPE_CYLINDER2, {0.0F, -37.0F, 0.0F}, 42.0F, 60.0F},
    {SHAPE_SPHERE, {10.0F, -35.0F, 5.0F}, 57.0F, 0.0F},
    {SHAPE_CYLINDER2, {0.0F, -25.0F, 0.0F}, 30.0F, 40.0F},
    {SHAPE_CYLINDER2, {0.0F, -37.0F, 0.0F}, 42.0F, 60.0F},
    {SHAPE_SPHERE, {-10.0F, -35.0F, 5.0F}, 57.0F, 0.0F},
    {SHAPE_CAPSULE, {0.0F, 15.0F, 0.0F}, 75.0F, 95.0F},
};
static Sint32 hit_volume = 25;
Float golem_e_flash_red = 0.8F;
Float golem_e_flash_green = 0.2F;
Float golem_e_flash_blue = 0.2F;

static void golem_destroy(struct task *task);
static void golem_step(struct task *task);
static void golem_place_parts(NJS_CNK_OBJECT *object);
static void golem_draw(struct task *task);
static void hand_a_step(struct task *task);
static void hand_b_step(struct task *task);
static void head_step(struct task *task);
static int sensor_index(NJS_CNK_OBJECT *node);
static void sensor_step(struct task *task);
static void sensor_take_hits(struct task *task);
static void sensor_place(int index);
static void sensor_create(struct task *parent, int index);
static Bool hands_down(struct task *task);

struct task *golem_e_create(NJS_POINT3 *pos, Angle ang_y, NJS_POINT3 *home)
{
    struct task *task;
    struct task *hand_a;
    struct task *hand_b;
    struct task *head;
    struct task_work *work;
    struct boss_record *record;
    struct mover *mover;
    int i;

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
    golem_e = boss_record_alloc_own(record, sizeof(*golem_e));
    if (golem_e == NULL) {
        boss_record_free_parts(task);
        golem_task = NULL;
        task_destroy(task);
        return NULL;
    }
    /* golem_destroy may run from here on. golem_e starts zeroed, so it frees
     * only the sensors made. it kills only the weak points golem_e_weak_points
     * holds, where each is null until golem_e_weak_points_create makes it */
    task->destructor = golem_destroy;
    if (!golem_e_weak_points_create(task)) {
        goto failed;
    }
#else
    /* @bug memory: stores own through a null boss record, as shared/golem.h
     * says (the state's test at 0x8c50066c, the store at 0x8c5007f6) */
    record = boss_record_create(task);
    golem_e = syMalloc(sizeof(struct golem));
    if (!golem_e) {
        task_kill(task);
        return NULL;
    }
    golem_e_weak_points_create(task);
#endif
    /* @hack spelling: each child's assignment inside its test, where a
     * statement of its own would do */
    /* @bug logic: returns null and leaves the golem's task to step where a
     * child fails, as shared/golem.h says (the kill at 0x8c500674, the return
     * at 0x8c50068e) */
    if (!(hand_a = task_create_child(TASK_WORK, hand_a_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it destroys the unfinished
         * golem */
        goto failed;
#else
        return NULL;
#endif
    }
    if (!(hand_b = task_create_child(TASK_WORK, hand_b_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged at the first child: it destroys the
         * unfinished golem */
        goto failed;
#else
        return NULL;
#endif
    }
    if (!(head = task_create_child(TASK_WORK, head_step, task))) {
#ifdef NONMATCHING
        /* this side repairs the bug tagged at the first child: it destroys the
         * unfinished golem */
        goto failed;
#else
        return NULL;
#endif
    }
    /* @hack spelling: the shapes declared in a block of their own, where
     * declarations at the function's top would do */
    {
        /* @hack type: each shape declared as an array of one, where the
         * shape alone would do */
        struct collision_shape hand_a_shapes[HAND_SHAPES];
        struct collision_shape hand_b_shapes[HAND_SHAPES];
        struct collision_shape head_shapes[HEAD_SHAPES];

        hand_a_shapes[0] = hand_a_shape[0];
        hand_b_shapes[0] = hand_b_shape[0];
        head_shapes[0] = head_shape[0];
        hand_a_shapes[0].radius = hand_a_strike_radius;
        hand_a_shapes[0].size_y = hand_a_strike_size_y;
        hand_b_shapes[0].radius = hand_b_strike_radius;
        hand_b_shapes[0].size_y = hand_b_strike_size_y;
        head_shapes[0].radius = head_radius;
        collision_init(hand_a, hand_a_shapes, HAND_SHAPES, COLLISION_OBJECT);
        collision_init(hand_b, hand_b_shapes, HAND_SHAPES, COLLISION_OBJECT);
        collision_init(head, head_shapes, HEAD_SHAPES, COLLISION_OBJECT);
    }
#ifdef NONMATCHING
    /* this side destroys the golem where collision_init gave a child no
     * record */
    if (hand_a->work->collision == NULL || hand_b->work->collision == NULL ||
        head->work->collision == NULL) {
        goto failed;
    }
#endif
    record->own = golem_e;
    motion_init(record->block, golem_motions);
    motion_set(record->block, GOLEM_MOTION_OPENING);
    mover = task->motion.p;
    mover->home = *home;
    mover->floor_y = home->y;
    work->pos = *pos;
    work->ang.y = ang_y;
    work->scl.x = work->scl.y = work->scl.z = 1.0F;
    /* @hack spelling: delta declared in a block of its own, where a
     * declaration at the function's top would do */
    {
        NJS_VECTOR delta;

        delta.x = work->pos.x - mover->home.x;
        delta.y = 0.0F;
        /* @bug logic: delta.z takes pos.x less home.z, as shared/golem.h says
         * (0x8c500864 reads pos.x, as delta.x's line does at 0x8c50084e) */
        delta.z = work->pos.x - mover->home.z;
        /* the golem's distance from home, stored in the mover's wall_bounce.
         * no code reads it there: no mover step runs on the golem's task */
        mover->wall_bounce = njSqrt((delta.x * delta.x) + (delta.z * delta.z));
    }
    mover_set_gravity(task, GOLEM_GRAVITY);
#ifdef NONMATCHING
    for (i = 0; i < GOLEM_SENSORS; i++) {
        sensor_create(task, i);
        if (golem_e->sensors[i] == NULL ||
            golem_e_sensor_tasks[i]->work->collision == NULL) {
            goto failed;
        }
    }
    /* this side moves player 0 only once every sensor exists */
    player_set_pos(0, 0.0F, golem_e_floor_y, PLAYER_START_Z);
#else
    player_set_pos(0, 0.0F, golem_e_floor_y, PLAYER_START_Z);
    for (i = 0; i < GOLEM_SENSORS; i++) {
        sensor_create(task, i);
    }
#endif
    task->draw = golem_draw;
    task->destructor = golem_destroy;
    golem_e->dark_story = TRUE;
    golem_camera_set(task, GOLEM_CAMERA_INTRO);
    golem_e_action_start(task, GOLEM_ACTION_WAIT);
    golem_e->power_max = golem_e->power = GOLEM_POWER;
    golem_e->power_low = GOLEM_POWER_LOW;
    golem_e->turn = GOLEM_TURN_TO_PLAYER;
    golem_e->slot = 0;
    golem_e->heading = 0;
    golem_e->aim_heading = 0;
    golem_e->lift = 0.0F;
    golem_e->hits = 0;
    golem_e->rubble_count = 0;
    golem_e->floor_goal = golem_e->floor_y = golem_e_floor_y;
    golem_e->throwing = FALSE;
    golem_e->unk_58 = 0;
    golem_e->flashing = FALSE;
    golem_e->flash_steps = 0;
    golem_e->camera_far_started = FALSE;
    golem_e->hands_down_state = FOOTHOLD_WHOLE;
    golem_e->slam_state = FOOTHOLD_WHOLE;
    golem_e->rest_steps = 0;
    golem_e->rest_wait = 0;
    golem_e->up_steps = 0;
    boss_gauge_create(task, &golem_e->power);
    boss_title_create(task, &golem_title);
    return task;

#ifdef NONMATCHING
failed:
    /* the weak points are tasks of their own, not the golem's children, so the
     * failure destroys them itself */
    for (i = 0; i < GOLEM_E_WEAK_POINTS; i++) {
        if (golem_e_weak_points[i] != NULL) {
            task_destroy(golem_e_weak_points[i]);
        }
    }
    task_destroy(task);
    return NULL;
#endif
}

static void golem_destroy(struct task *task)
{
    Uint32 i;

    for (i = 0; i < GOLEM_SENSORS; i++) {
        if (golem_e->sensors[i]) {
            world_remove(golem_e_sensor_tasks[i], golem_e->sensors[i]);
            world_pool_free(golem_e->sensors[i]);
        }
#ifdef NONMATCHING
        /* this side clears the sensor's task, which dies with the golem, for a
         * golem_e_create that runs again */
        golem_e_sensor_tasks[i] = NULL;
#endif
    }
    for (i = 0; i < GOLEM_E_WEAK_POINTS; i++) {
        if (golem_e_weak_points[i]) {
#ifdef NONMATCHING
            /* this side stops the weak point's draw for the step before the
             * kill takes effect. the draw reads the dying golem's sensor
             * matrices */
            golem_e_weak_points[i]->draw = NULL;
#endif
            task_kill(golem_e_weak_points[i]);
#ifdef NONMATCHING
            /* this side clears the slot of the weak point it kills, for a
             * golem_e_create that runs again */
            golem_e_weak_points[i] = NULL;
#endif
        }
    }
    boss_record_free_parts(task);
    golem_e = NULL;
#ifdef NONMATCHING
    /* this side clears golem_task, which names the dying task, for a
     * golem_e_create that runs again */
    golem_task = NULL;
#endif
}

/* the golem's step counts down one flash a step. that is golem_e's own while
 * it runs, or else the first weak point's that runs. it clears golem_e's
 * flashing once none runs */
static void golem_step(struct task *task)
{
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    Uint32 i;

    golem_e->hold_motion = FALSE;
    golem_camera_step(task);
    golem_e_actions_step(task);
    golem_e_hover(task);
    golem_e->rest_steps++;
    golem_e_set_rest_wait();
    if (!golem_e->hold_motion) {
        motion_step(motion);
    }
    if (golem_e->flash_steps > 0) {
        golem_e->flash_steps--;
    } else if (golem_e_weak_points[0] &&
               GOLEM_E_WEAK_POINT(golem_e_weak_points[0])->flash_steps > 0) {
        GOLEM_E_WEAK_POINT(golem_e_weak_points[0])->flash_steps--;
    } else if (golem_e_weak_points[1] &&
               GOLEM_E_WEAK_POINT(golem_e_weak_points[1])->flash_steps > 0) {
        GOLEM_E_WEAK_POINT(golem_e_weak_points[1])->flash_steps--;
    } else if (golem_e_weak_points[2] &&
               GOLEM_E_WEAK_POINT(golem_e_weak_points[2])->flash_steps > 0) {
        GOLEM_E_WEAK_POINT(golem_e_weak_points[2])->flash_steps--;
    } else {
        golem_e->flashing = FALSE;
    }
    if (golem_e->camera_mode) {
        golem_e_rubble_throw(task, GOLEM_SENSOR_HAND_A);
        golem_e_rubble_throw(task, GOLEM_SENSOR_HAND_B);
        golem_e_rubble_throw(task, GOLEM_SENSOR_FOREARM_A);
        golem_e_rubble_throw(task, GOLEM_SENSOR_FOREARM_B);
    }
    golem_e_count_up_steps(task);
    if (golem_e->camera_mode) {
        for (i = 0; i < GOLEM_SENSORS; i++) {
            sensor_take_hits(golem_e_sensor_tasks[i]);
        }
    }
}

/* the motion callback of the golem's draw, for each node with its matrix
 * current. it places the node's sensor, and stores the hands' and the head's
 * points */
static void golem_place_parts(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 pos;
    int index;

    if ((index = sensor_index(object)) != NO_SENSOR) {
        sensor_place(index);
    }
    if (object == &golem_statue_head) {
        njPushMatrixEx();
        njTranslate(NULL, head_x, head_y, head_z);
        njGetTranslation(NULL, &pos);
        njCalcPoint(&golem_e_view_inverse, &pos, &golem_e_head_pos);
        njPopMatrixEx();
    }
    if (object == &golem_statue_hand_a) {
        njPushMatrixEx();
        njTranslate(NULL, hand_a_x, hand_a_y, hand_a_z);
        njGetTranslation(NULL, &pos);
        njCalcPoint(&golem_e_view_inverse, &pos, &golem_e_hand_a_pos);
        njPopMatrixEx();
    }
    if (object == &golem_statue_hand_b) {
        njPushMatrixEx();
        njTranslate(NULL, hand_b_x, hand_b_y, hand_b_z);
        njGetTranslation(NULL, &pos);
        njCalcPoint(&golem_e_view_inverse, &pos, &golem_e_hand_b_pos);
        njPopMatrixEx();
    }
}

/* keep the inverse of the view, which must be the current matrix */
static void golem_start_placing(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    struct task_work *work = task->work;

    matrix_get(&golem_e_view_inverse);
    njInvertMatrix(&golem_e_view_inverse);
    njCnkSetMotionCallback(golem_place_parts);
}

static void golem_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct motion_control *motion = record->block;
    struct motion_play *play;
    Float share;
    Float red;
    Float green;
    Float blue;

    /* @bug logic: while golem_e isn't flashing, the draw restores the draw
     * control and attributes that the last save left. it restores them on
     * every path, at 0x8c500f5a and 0x8c500f6c. it saves them only while
     * golem_e flashes, at 0x8c500e84 */
    if (golem_e->flashing) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        share = (Float)golem_e->flash_steps * (1.0F / GOLEM_E_HIT_FLASH_STEPS);
        red = 1.0F - ((1.0F - golem_e_flash_red) * share);
        green = 1.0F - ((1.0F - golem_e_flash_green) * share);
        blue = 1.0F - ((1.0F - golem_e_flash_blue) * share);
        constant_material_set(1.0F, red, green, blue);
    }
    njPushMatrixEx();
    golem_start_placing(task);
    njTranslate(NULL, work->pos.x, work->pos.y + golem_e->lift, work->pos.z);
    njRotateY(NULL, golem_e->heading);
    njSetTexture(&golem_statue_texlist);
    /* @hack spelling: the motion set as the right operand of an &&, with play
     * set inside it, where an if around the two statements would do */
    !(motion->flags & MOTION_LINKING) &&
        (play = &motion->current,
         njSetCnkCurrentMotion(play->motion, play->frame), 0);
    motion_draw_simple(&golem_statue, motion);
    njCnkSetMotionCallback(NULL);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

/* a hand follows its point, as the golem's draw last found it. its shape
 * collides only while the hand strikes or the hands are down. it takes the
 * strike's sizes or the down sizes to match. hand_b_step is hand B's */
static void hand_a_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    Sint32 index;

    work->pos = golem_e_hand_a_pos;
    index = motion_index(motion);
    if (HAND_A_STRIKES(index)) {
        work->collision->shapes->radius = hand_a_strike_radius;
        work->collision->shapes->size_y = hand_a_strike_size_y;
        collision_fit_radius(task);
        collision_register(task);
    } else if (hands_down(task)) {
        work->collision->shapes->radius = hand_a_down_radius;
        work->collision->shapes->size_y = hand_a_down_size_y;
        collision_fit_radius(task);
        collision_register(task);
    }
}

static void hand_b_step(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    Sint32 index;

    work->pos = golem_e_hand_b_pos;
    index = motion_index(motion);
    if (HAND_B_STRIKES(index)) {
        work->collision->shapes->radius = hand_b_strike_radius;
        work->collision->shapes->size_y = hand_b_strike_size_y;
        collision_fit_radius(task);
        collision_register(task);
    } else if (hands_down(task)) {
        work->collision->shapes->radius = hand_b_down_radius;
        work->collision->shapes->size_y = hand_b_down_size_y;
        collision_fit_radius(task);
        collision_register(task);
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw task's first shape as golem_cylinder_draw's cylinder, at the shape's
 * sizes, centered on task's pos and unturned. nothing calls it. it is external
 * because SHC leaves out a static function that nothing names */
void golem_e_shape_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR size;

    njPushMatrixEx();
    size.x = size.z = work->collision->shapes->radius;
    size.y = work->collision->shapes->size_y * 2.0F;
    njTranslateEx(&work->pos);
    njTranslate(NULL, 0.0F, -(size.y * 0.5F), 0.0F);
    golem_cylinder_draw(&size);
    njPopMatrixEx();
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the head follows its point, as the golem's draw last found it. it collides
 * only in the motions of GOLEM_SLAM_SLAM. each step makes its record anew */
static void head_step(struct task *task)
{
    /* @hack redundant: work declared register, where a plain local would do */
    register struct task_work *work = task->work;
    struct boss_record *record = task->parent->extra.p;
    struct motion_control *motion = record->block;
    /* @hack type: the shape declared as an array of one, where the shape
     * alone would do */
    struct collision_shape shapes[HEAD_SHAPES];
    Sint32 index;

    work->pos = golem_e_head_pos;
    collision_free(task);
    shapes[0] = head_shape[0];
    shapes[0].radius = head_radius;
    collision_init(task, shapes, HEAD_SHAPES, COLLISION_OBJECT);
    index = motion_index(motion);
    if (SLAM_SLAM(index)) {
        collision_register(task);
    }
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

/* count a player's attack on task, a sensor, or on an active weak point. no
 * attack counts while golem_e's flashing is TRUE, or once its hits reach
 * HITS_MAX. a sensor collides in each step that counts no attack on it. from
 * HITS_MAX on, only GOLEM_SENSOR_FULL collides, the one sensor with
 * GOLEM_PUSH_SOLID */
static void sensor_take_hits(struct task *task)
{
    int i;
    struct task *attacker;

    for (i = 0; i < GOLEM_E_WEAK_POINTS; i++) {
        /* @hack spelling: weak_point, a copy of the table's entry, where the
         * entry read in place would do */
        struct task *weak_point = golem_e_weak_points[i];

        if (weak_point && GOLEM_E_WEAK_POINT(weak_point)->active) {
            if (golem_e->hits < HITS_MAX && !golem_e->flashing &&
                (golem_e_weak_points[i]->work->collision->flags &
                 COLLISION_TOUCHED) &&
                (attacker =
                     golem_e_weak_points[i]->work->collision->hit->owner) !=
                    NULL &&
                attacker->work && attacker->work->id == WORK_ID_ATTACK) {
                sound_play(HIT_SOUND, HIT_SOUND_KEY(task), 0, 0);
                sound_set_volume(HIT_SOUND_KEY(task), SOUND_ANY_ID, hit_volume);
                golem_e->hits += WEAK_POINT_HIT_COUNT;
                golem_e->power -= WEAK_POINT_HIT_POWER;
                GOLEM_E_WEAK_POINT(golem_e_weak_points[i])->flash_steps =
                    GOLEM_E_HIT_FLASH_STEPS;
                golem_e->flashing = TRUE;
                attacker->work->flags |= WORK_HURT;
                golem_e_rubble_hit(task, &attacker->work->pos);
            }
        }
    }
    if (golem_e->hits < HITS_MAX) {
        if (!golem_e->flashing &&
            (task->work->collision->flags & COLLISION_TOUCHED) &&
            (attacker = task->work->collision->hit->owner) != NULL &&
            attacker->work && attacker->work->id == WORK_ID_ATTACK &&
            attacker->work->pos.y > golem_e_floor_y - HIT_FLOOR_MARGIN) {
            sound_play(HIT_SOUND, HIT_SOUND_KEY(task), 0, 0);
            sound_set_volume(HIT_SOUND_KEY(task), SOUND_ANY_ID, hit_volume);
            golem_e->hits += SENSOR_HIT_COUNT;
            golem_e->power -= SENSOR_HIT_POWER;
            golem_e->flash_steps = GOLEM_E_HIT_FLASH_STEPS;
            golem_e->flashing = TRUE;
            attacker->work->flags |= WORK_HURT;
            golem_e_rubble_hit(task, &attacker->work->pos);
        } else {
            collision_register(task);
        }
    } else if (task->work->collision->shapes->push == GOLEM_PUSH_SOLID) {
        collision_register(task);
    }
}

/* place sensor index's object and task where the current matrix puts the
 * sensor's node, in the world's axes. it must run inside the golem's draw.
 * golem_e_sensor_bases takes the sensor's last place, and
 * golem_e_sensor_matrices the new one.
 *
 * while no player lies within the sensor's range plus SENSOR_REACH, the call
 * clears WORLD_ADDED in the task's work. the world then passes the object
 * over. otherwise the call sets WORLD_ADDED. the task's force then takes the
 * turn from the last place to the new. the force's speed takes how far player
 * 0 would move, riding the sensor. the shape then takes its form and sizes
 * from sensor_geometries, and GOLEM_SENSOR_FULL's also takes
 * GOLEM_PUSH_SOLID */
static void sensor_place(int index)
{
    NJS_OBJECT *object = golem_e->sensors[index];
    struct task *task = golem_e_sensor_tasks[index];
    NJS_POINT3 local;
    NJS_POINT3 move;
    struct angles turn;

    njSetMatrix(&golem_e_sensor_bases[index], &golem_e_sensor_matrices[index]);
    njPushMatrixEx();
    matrix_multiply_after_pairs(&golem_e_view_inverse);
    matrix_get(&golem_e_sensor_matrices[index]);
    njGetTranslation(NULL, &task->work->pos);
    object->pos[0] = task->work->pos.x;
    object->pos[1] = task->work->pos.y;
    object->pos[2] = task->work->pos.z;
    current_matrix_angles_xyz(&object->ang[0], &object->ang[1],
                              &object->ang[2]);
    current_matrix_angles_yxz(&task->work->ang.x, &task->work->ang.y,
                              &task->work->ang.z);
    njPopMatrixEx();
    if (!player_near(&task->work->pos, sensor_ranges[index] + SENSOR_REACH)) {
        task->work->flags &= ~WORLD_ADDED;
        return;
    }
    task->work->flags |= WORLD_ADDED;
    njPushMatrixEx();
    matrix_set(&golem_e_sensor_bases[index]);
    njInvertMatrix(NULL);
    njCalcPoint(NULL, &player_work[0]->pos, &local);
    matrix_multiply_before(&golem_e_sensor_matrices[index]);
    current_matrix_angles_yxz(&turn.x, &turn.y, &turn.z);
    njPopMatrixEx();
    njPushMatrixEx();
    matrix_set(&golem_e_sensor_matrices[index]);
    njCalcPoint(NULL, &local, &move);
    njPopMatrixEx();
    move.x -= player_work[0]->pos.x;
    move.y -= player_work[0]->pos.y;
    move.z -= player_work[0]->pos.z;
    TASK_FORCES_OF(task)->speed = move;
    TASK_FORCES_OF(task)->turn.x = turn.x;
    TASK_FORCES_OF(task)->turn.y = -turn.y;
    TASK_FORCES_OF(task)->turn.z = turn.z;
    task->work->collision->shapes->form = sensor_geometries[index].form;
    task->work->collision->shapes->center.x = sensor_geometries[index].center.x;
    task->work->collision->shapes->center.y = sensor_geometries[index].center.y;
    task->work->collision->shapes->center.z = sensor_geometries[index].center.z;
    task->work->collision->shapes->radius = sensor_geometries[index].radius;
    task->work->collision->shapes->size_y = sensor_geometries[index].size_y;
    if (index == GOLEM_SENSOR_FULL) {
        task->work->collision->shapes->push = GOLEM_PUSH_SOLID;
    }
    collision_fit_radius(task);
}

/* create sensor index's task as parent's child, and its object in the
 * collision world. the object is a copy of the sensor's model at the origin,
 * unturned and unscaled */
static void sensor_create(struct task *parent, int index)
{
    NJS_OBJECT *copy;
    struct task *task;
    Sint32 flags;

    copy = world_pool_alloc();
    /* @bug memory: writes through a null object, as shared/golem.h says
     * (golem_destroy's test at 0x8c500a4e, the write at 0x8c501862) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it makes no sensor where the pool
     * has no object */
    if (copy == NULL) {
        return;
    }
#endif
    task = golem_e_sensor_tasks[index] =
        task_create_child(TASK_WORK | TASK_FORCE, sensor_step, parent);
    /* @bug memory: reads through a null task, as shared/golem.h says
     * (golem_e_create's test at 0x8c50068e, the read at 0x8c5018fc) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above. where task_create_child fails, it
     * gives the object back and makes no sensor */
    if (task == NULL) {
        world_pool_free(copy);
        return;
    }
#endif
    golem_e->sensors[index] = copy;
    *copy = *sensor_models[index];
    copy->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    copy->scl[0] = copy->scl[1] = copy->scl[2] = 1.0F;
    copy->ang[0] = copy->ang[1] = copy->ang[2] = 0;
    copy->pos[0] = copy->pos[1] = copy->pos[2] = 0.0F;
    flags = SENSOR_WORLD_FLAGS;
    flags |= golem_size_class_flags(sensor_ranges[index]);
    if (sensor_models[index] == &golem_sensor_head ||
        sensor_models[index] == &golem_sensor_low) {
        flags |= GROUND_CRASHES;
    }
    world_add(flags, task, copy);
    collision_init(task, sensor_shape, SENSOR_SHAPES, COLLISION_OBJECT);
#ifdef NONMATCHING
    /* this side leaves the shape alone where collision_init gave the task no
     * record. the matching side writes through the null */
    if (task->work->collision == NULL) {
        return;
    }
#endif
    task->work->collision->shapes->form = SHAPE_CYLINDER;
    task->work->collision->shapes->center.x = 0.0F;
    task->work->collision->shapes->center.y = 0.0F;
    task->work->collision->shapes->center.z = 0.0F;
    task->work->collision->shapes->radius = SENSOR_START_SIZE;
    task->work->collision->shapes->size_y = SENSOR_START_SIZE;
    collision_fit_radius(task);
}

/* TRUE from HANDS_DOWN_FRAME of a motion that lowers the hands. it is TRUE too
 * in a motion that holds them, while golem_e_holding is TRUE. task is a child
 * of the golem's */
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
    return (HANDS_COMING_DOWN(index) && frame >= HANDS_DOWN_FRAME) ||
           (HANDS_HELD(index) && golem_e_holding(parent));
}
