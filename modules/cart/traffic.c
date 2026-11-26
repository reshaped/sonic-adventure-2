/* the cars of a story race. each drives one of its course's lanes, and a
 * player's kart that touches one throws it off. no code calls
 * cart_stop_traffic */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "cart/kart_art.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/traffic_lanes.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "object/set.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

struct traffic_start {
    Sint32 first, model;
    const NJS_POINT3 *path;
    Sint32 count;
};
struct traffic_car {
    Sint32 point;
    Float alpha;
    NJS_VECTOR velocity;
    struct angles spin;
    struct traffic_start start;
};

/* where each course's lanes start in paths, which holds LANES for each. car
 * i takes lane i % LANES */
#define LANES 2
#define TAILS_LANES 0
#define ROUGE_LANES 2
#define TAILS_HD_LANES 4
#define ROUGE_HD_LANES 6
/* a story race has CARS cars, or HD_CARS under MISSION_HD. a slot is a
 * lane's points over SLOTS, or over HD_SLOTS. car i starts i slots past point
 * PATH_MARGIN of its lane. at PATH_MARGIN points short of the lane's end, a
 * car goes back to point PATH_MARGIN */
#define CARS 30
#define HD_CARS 60
#define SLOTS 60
#define HD_SLOTS 150
#define PATH_MARGIN 10
#define MODELS 6
/* a thrown car keeps its lane's step at its point as its velocity. it gains
 * up to THROW_SIDE either way along x and z. along y it gains THROW_LIFT to
 * THROW_LIFT + THROW_LIFT_SPREAD, and gravity then takes THROW_FALL a step.
 * the car spins up to THROW_SPIN degrees a step about each axis, and fades
 * over THROWN_STEPS */
#define THROW_SIDE 2.0F
#define THROW_LIFT 2.0F
#define THROW_LIFT_SPREAD 5.0F
#define THROW_SPIN 5
#define THROW_FALL 0.1F
#define THROWN_STEPS 100
enum traffic_mode { DRIVE, THROWN, GONE };

KEPT static const NJS_POINT3 *paths[8] = {
#include "cart/traffic/paths.inc"
};
static Sint32 path_lengths[] = {
    ARRAY_COUNT(cart_tails_lane_0),    ARRAY_COUNT(cart_tails_lane_1),
    ARRAY_COUNT(cart_rouge_lane_0),    ARRAY_COUNT(cart_rouge_lane_1),
    ARRAY_COUNT(cart_tails_hd_lane_0), ARRAY_COUNT(cart_tails_hd_lane_1),
    ARRAY_COUNT(cart_rouge_hd_lane_0), ARRAY_COUNT(cart_rouge_hd_lane_1)};
/* clang-format off */
static struct collision_shape traffic_shape[] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(1), SHAPE_HURTABLE, {0, 0, 0}, 20,
     0, 0, 0, {0, 0, 0}}};
/* clang-format on */
static Bool stopping = 1;
KEPT static NJS_CNK_OBJECT *models[MODELS] = {
#include "cart/traffic/models.inc"
};
static void create_traffic(struct traffic_start *start);
static void traffic_step(struct task *task);
static void traffic_draw(struct task *task);
static void traffic_destroy(struct task *task);

void cart_start_traffic(void)
{
    Sint32 i;
    Sint32 count;
    struct traffic_start start;
    if (game_state.mission == MISSION_HD) {
        count = HD_CARS;
    } else {
        count = CARS;
    }
    for (i = 0; i < count; i++) {
        if (cart_setup.course_mode == CART_TAILS) {
            if (game_state.mission == MISSION_HD) {
                start.path = paths[(i % LANES) + TAILS_HD_LANES];
                start.count = path_lengths[(i % LANES) + TAILS_HD_LANES];
                start.first = (i * (start.count / HD_SLOTS)) + PATH_MARGIN;
            } else {
                start.path = paths[(i % LANES) + TAILS_LANES];
                start.count = path_lengths[(i % LANES) + TAILS_LANES];
                start.first = (i * (start.count / SLOTS)) + PATH_MARGIN;
            }
        } else {
            if (game_state.mission == MISSION_HD) {
                start.path = paths[(i % LANES) + ROUGE_HD_LANES];
                start.count = path_lengths[(i % LANES) + ROUGE_HD_LANES];
                start.first = (i * (start.count / HD_SLOTS)) + PATH_MARGIN;
            } else {
                start.path = paths[(i % LANES) + ROUGE_LANES];
                start.count = path_lengths[(i % LANES) + ROUGE_LANES];
                start.first = (i * (start.count / SLOTS)) + PATH_MARGIN;
            }
        }
        start.model = i % MODELS;
        create_traffic(&start);
    }
    stopping = 0;
}
void cart_stop_traffic(void)
{
    stopping = 1;
}

static void face_path(struct task *task, NJS_VECTOR *out)
{
    struct task_work *work = task->work;
    struct traffic_car *car = task->motion.p;
    NJS_VECTOR dir;
    vector_diff(&car->start.path[car->point + 1], &car->start.path[car->point],
                &dir);
    if (out) {
        *out = dir;
    }
    work->ang.y =
        (Angle)(atan2f(-dir.z, dir.x) * (Float)ANGLE_PER_RADIAN) + QUARTER_TURN;
    work->ang.x =
        (Angle)(atan2f(-dir.y, njSqrt((dir.x * dir.x) + (dir.z * dir.z))) *
                (Float)ANGLE_PER_RADIAN);
    work->ang.z = 0;
}
static void start_path(struct task *task)
{
    struct task_work *work = task->work;
    struct traffic_car *car = task->motion.p;
    car->point = car->start.first;
    work->pos = car->start.path[car->point];
    face_path(task, NULL);
}
static void create_traffic(struct traffic_start *start)
{
    struct task *task;
    struct traffic_car *car;
    task = task_create(TASK_WORK, 2, traffic_step, "jamaExecuter");
#ifdef NONMATCHING
    /* this side makes no car where task_create fails. the matching side writes
     * through the null */
    if (task == NULL) {
        return;
    }
#endif
    task->draw = traffic_draw;
    task->destructor = traffic_destroy;
    car = syCalloc(1, sizeof(*car));
    task->motion.p = car;
    /* @bug memory: writes the car's start through null where the heap can't
     * give one. traffic_destroy tests task->motion at 0x8c506ee8, but the copy
     * at 0x8c506a2e writes through it unchecked */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: without a car, it destroys the
     * task */
    if (car == NULL) {
        task_destroy(task);
        return;
    }
#endif
    car->start = *start;
    start_path(task);
#ifdef NONMATCHING
    /* this side destroys the car where collision_init fails, whose record the
     * car's step reads */
    if (!collision_init(task, traffic_shape, 1, COLLISION_ENEMY)) {
        task_destroy(task);
    }
#else
    collision_init(task, traffic_shape, 1, COLLISION_ENEMY);
#endif
}
static void traffic_step(struct task *task)
{
    struct task_work *work;
    struct traffic_car *car;
    NJS_VECTOR direction;
    NJS_VECTOR relative_velocity;
    struct task *other;
    struct cart_vehicle *kart;
    Sint32 i;
    if (stopping) {
        task_destroy(task);
        return;
    }
    if (cart_traffic_paused) {
        return;
    }
    work = task->work;
    car = task->motion.p;
    switch (work->mode) {
    case DRIVE:
        car->alpha = 1.0F;
        if (game_state.mission == MISSION_HD) {
            if (game_state.total_steps & 1) {
                vector_sum(&car->start.path[car->point],
                           &car->start.path[car->point + 1], &work->pos);
                work->pos.x *= 0.5F;
                work->pos.y *= 0.5F;
                work->pos.z *= 0.5F;
                car->point++;
            } else {
                work->pos = car->start.path[car->point];
            }
        } else {
            work->pos = car->start.path[car->point];
            car->point++;
        }
        face_path(task, &direction);
        if (car->point >= car->start.count - PATH_MARGIN) {
            car->point = PATH_MARGIN;
        }
        if (work->collision->flags & COLLISION_TOUCHED) {
            for (i = 0; i < work->collision->hit_count; i++) {
                other = work->collision->hits[i].task;
                if (cart_is_racer(other)) {
                    kart = other->motion.p;
                    if (kart->driver >= 0) {
                        /* @hack redundant: relative_velocity, which nothing
                         * reads */
                        vector_diff(&direction, &kart->velocity,
                                    &relative_velocity);
                        work->mode = THROWN;
                        work->timer = THROWN_STEPS;
                        car->spin.x = RANDOM_ANGLE(THROW_SPIN);
                        car->spin.y = RANDOM_ANGLE(THROW_SPIN);
                        car->spin.z = RANDOM_ANGLE(THROW_SPIN);
                        car->velocity = direction;
                        car->velocity.x +=
                            (rand() * (2 * THROW_SIDE * RAND_SCALE)) -
                            THROW_SIDE;
                        car->velocity.y +=
                            (rand() * (THROW_LIFT_SPREAD * RAND_SCALE)) +
                            THROW_LIFT;
                        car->velocity.z +=
                            (rand() * (2 * THROW_SIDE * RAND_SCALE)) -
                            THROW_SIDE;
                        cart_hit_traffic(other, &work->pos);
                    }
                }
            }
        }
        collision_register(task);
        break;
    case THROWN:
        vector_sum(&work->pos, &car->velocity, &work->pos);
        work->ang.x += car->spin.x;
        work->ang.y += car->spin.y;
        work->ang.z += car->spin.z;
        car->velocity.y -= THROW_FALL;
        if (!work->timer--) {
            work->mode = GONE;
        } else {
            car->alpha = work->timer * (1.0F / THROWN_STEPS);
        }
        break;
    case GONE:
        if (task->placement) {
            placement_kill(task);
        } else {
            task_destroy(task);
        }
        break;
    }
}

static void traffic_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct traffic_car *car = task->motion.p;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    if (car->alpha < 1.0F) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        constant_material_set(car->alpha, 1, 1, 1);
    }
    njSetTexture(&cart_story_textures);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    njCnkEasyDrawObject(models[car->start.model]);
    njPopMatrixEx();
    if (car->alpha < 1.0F) {
        draw_control_restore();
        draw_attr_restore();
    }
}
static void traffic_destroy(struct task *task)
{
    if (task->motion.p) {
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
}
