/* LAZER in Security Hall's object list: a beam between two pipes that hurts
 * what touches it. the placement's ang turns the laser, and scl.x + 1
 * scales its length. the beam swings scl.y either way along its z. the swing
 * is a sine whose phase turns scl.z degrees a step. a laser draws its beam on
 * alternate steps, out of step with the laser made before it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "sound/sound.h"
#include "stg08/fixture_art.h"
#include "stg08/background.h"
#include "stg08/laser.h"
#include "stg08/machines.h"
#include "stg08/security_hall.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the shapes of the laser's collision, which a child of its task carries */
enum laser_shape { LEFT_END, RIGHT_END, BEAM };
#define LASER_FLAGS (SHAPE_UNK_800000 | SHAPE_TURN_YZX)
#define BEAM_HIT                                                               \
    (SHAPE_HURTS_ALL | SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(1))
/* the beam hums while player 0 is within HUM_RANGE of its line. the laser
 * keeps its child only while player 0 is within CONTACT_RANGE of the line */
#define HUM_RANGE 200.0F
#define CONTACT_RANGE 50.0F
/* the sounds, ids in the stage's sound list. a play of HUM_SOUND lasts
 * HUM_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define HUM_SOUND 0
#define SPARK_SOUND 1
#define HUM_SOUND_PRIORITY 1
#define SPARK_SOUND_PRIORITY 0
#define HUM_SOUND_STEPS 10
/* a touch of the beam while the work's tick is 0 makes a flash at the
 * toucher. the flash is FLASH_SCALE in size, and draws FLASH_DEPTH_BIAS
 * nearer. it throws SPARKS sparks a step */
#define FLASH_SCALE 8.0F
#define SPARKS 2
#define FLASH_DEPTH_BIAS 12.0F
/* the tick is SMOKE_STEPS while the beam is touched, and counts down to 0
 * once it isn't. so a new flash waits for the tick to reach 0. smoke rises
 * from the last flash's place while the tick is above SMOKE_END */
#define SMOKE_STEPS 38
#define SMOKE_END 5
/* a puff of smoke moves up to SMOKE_DRIFT / 2 a step either way along each
 * axis. it rises SMOKE_RISE a step besides. its scale is SMOKE_SCALE, and up
 * to SMOKE_SCALE_RANGE more. it draws SMOKE_DEPTH_BIAS nearer */
#define SMOKE_DRIFT 0.1F
#define SMOKE_RISE 0.3F
#define SMOKE_SCALE 1.2F
#define SMOKE_SCALE_RANGE 0.5F
#define SMOKE_DEPTH_BIAS 10.0F
/* a pipe draws while its squared distance from the camera is
 * PIPE_DRAW_DISTANCE2 or less */
#define PIPE_DRAW_DISTANCE2 90000.0F
/* the laser draws only near the camera. its center must lie within the
 * body's radius times LENGTH_SCALE times RANGE_IN_RADII. security_laser_build
 * sets that radius to BODY_RADIUS */
#define RANGE_IN_RADII 5.0F
#define BODY_RADIUS 2000.0F
/* the scale of the laser's length along its x */
#define LENGTH_SCALE(work) ((work)->scl.x + 1.0F)
/* a random Float from -0.5 up to 0.5 */
#define RANDOM_CENTERED() (RANDOM_UNIT() - 0.5F)

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's -fpack-struct=4 caps each member's alignment at 4. pack(8) lifts the
 * cap for the matrix, which GCC aligns to 8. the matrix comes first. the
 * record's 136 bytes are a multiple of 8. so no member moves, and the size
 * stays SHC's */
#pragma pack(push, 8)
#endif
#endif
/* a laser's record, in task->motion. place_laser sets all but spark each
 * step */
struct laser {
    NJS_MATRIX matrix; /* the laser's place and turn, ang.x's included */
    /* the pipes in the laser's frame, scaled along x by LENGTH_SCALE */
    NJS_POINT3 left;
    NJS_POINT3 right;
    NJS_POINT3 spark; /* where the last flash was made */
    /* the pipes in the world, without ang.x's turn */
    union vector_floats world_left;
    union vector_floats world_right;
    NJS_POINT3 center; /* the beam's middle in the world, swing included */
};

#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct laser */
#pragma pack(pop)
#endif
#endif

/* the beam's swing along its z this step, a Float in the bits of
 * task->unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define SWING(task) (*(Float *)&(task)->unk_3c)
#define LASER(task) ((struct laser *)(task)->motion.p)

NJS_TEXLIST *security_laser_textures[] = {&security_laser_texlist,
                                          &flash_texlist, &dust_texlist, NULL};
static NJS_DIRECT_HEAD *compiled_body = NULL;
static NJS_DIRECT_HEAD *compiled_left = NULL;
static NJS_DIRECT_HEAD *compiled_right = NULL;
/* the beam and the pipes turn about x by these a step */
static Angle beam_spin = 896;
static Angle end_spin = -128;
/* clang-format off */
static struct collision_shape shapes[] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, LASER_FLAGS,
     {0.0F, 0.0F, 0.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, LASER_FLAGS,
     {0.0F, 0.0F, 0.0F}, 4.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), BEAM_HIT,
     LASER_FLAGS | SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 1.5F, 5.0F, 0.0F, 0,
     {0, 0, QUARTER_TURN}},
};
/* clang-format on */
static int laser_count = 0;

static void laser_step(struct task *task);
static void laser_draw(struct task *task);
static void laser_destroy(struct task *task);

#pragma inline(zero_vector, difference)
static void zero_vector(NJS_VECTOR *vector)
{
    vector->x = 0.0F;
    vector->y = 0.0F;
    vector->z = 0.0F;
}

static void difference(const Float *from, const Float *to, Float *result)
{
    *result++ = *to++ - *from++;
    *result++ = *to++ - *from++;
    *result = *to - *from;
}

/* object_build of shared/common_object.h, but with the laser's texture list
 * current */
static NJS_DIRECT_HEAD *build_laser_node(Uint8 **buffer, NJS_CNK_OBJECT *object)
{
    NJS_CNK_OBJECT node;
    NJS_DIRECT_HEAD *built;
    Uint8 *end;

    node = *object;
    node.evalflags = NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL |
                     NJD_EVAL_BREAK;
    node.child = NULL;
    node.sibling = NULL;
    built = NULL;
    njSetTexture(&security_laser_texlist);
    end = *buffer + njCnkDirectObjectCompileSize(&node);
    if (end < DEMO_AREA) {
        njCnkDirectObjectCompile(&node, *buffer);
        built = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return built;
}

Uint8 *security_laser_build(Uint8 *buffer)
{
    security_laser_body->model->r = BODY_RADIUS;
    njFogDisable();
    compiled_body = build_laser_node(&buffer, security_laser_body);
    njFogEnable();
    compiled_left = build_laser_node(&buffer, security_laser_left);
    compiled_right = build_laser_node(&buffer, security_laser_right);
    return buffer;
}

static void place_laser(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR travel;

    SWING(task) = njSin((Angle)game_state.steps *
                        (Angle)(work->scl.z * ANGLE_PER_DEGREE)) *
                  work->scl.y;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njScale(NULL, LENGTH_SCALE(work), 1.0F, 1.0F);
    njCalcPoint(NULL, (NJS_POINT3 *)security_laser_left->pos,
                &LASER(task)->left);
    njCalcPoint(NULL, (NJS_POINT3 *)security_laser_right->pos,
                &LASER(task)->right);
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCalcPoint(NULL, &LASER(task)->left, &LASER(task)->world_left.vector);
    njCalcPoint(NULL, &LASER(task)->right, &LASER(task)->world_right.vector);
    njRotateX(NULL, work->ang.x);
    njGetMatrix(&LASER(task)->matrix);
    travel.z = SWING(task);
    travel.x = 0.0F;
    travel.y = 0.0F;
    njCalcPoint(NULL, &travel, &LASER(task)->center);
    njPopMatrixEx();
}

void security_laser_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct laser));
    if (!task->motion.p) {
        return;
    }
    laser_count++;
    task->draw = laser_draw;
    task->step = laser_step;
    task->destructor = laser_destroy;
    work->mode = laser_count & 1;
    place_laser(task);
}

static void laser_destroy(struct task *task)
{
    /* @hack redundant: work, a local that no line reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *work = task->work;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void laser_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *hit_pos;
    struct {
        NJS_POINT3 origin;
        union vector_floats direction;
    } axis;
    NJS_POINT3 closest;
    Float distance;
    Float length;
    Float left_distance;
    Float right_distance;

    if (placement_try_leave(task)) {
        return;
    }
    work->timer++;
    place_laser(task);
    if (task->child &&
        (task->child->work->collision->flags & COLLISION_TOUCHED) &&
        task->child->work->collision->hit_shape == BEAM) {
        if (work->tick == 0) {
            /* @hack redundant: a clear of axis.direction, which no line reads
             * before the next write */
            zero_vector(&axis.direction.vector);
            hit_pos = &task->child->work->collision->hits[0].task->work->pos;
            LASER(task)->spark = *hit_pos;
            flash_manager_create_sparks_depth(hit_pos->x, hit_pos->y,
                                              hit_pos->z, FLASH_SCALE, SPARKS,
                                              FLASH_DEPTH_BIAS);
            sound_play(SPARK_SOUND, NULL, SPARK_SOUND_PRIORITY, 0);
        }
        work->tick = SMOKE_STEPS;
    } else {
        if (work->tick > SMOKE_END) {
            axis.direction.vector.x = RANDOM_CENTERED() * SMOKE_DRIFT;
            axis.direction.vector.y =
                (RANDOM_CENTERED() * SMOKE_DRIFT) + SMOKE_RISE;
            axis.direction.vector.z = RANDOM_CENTERED() * SMOKE_DRIFT;
            dust_steady_create(&LASER(task)->spark, &axis.direction.vector,
                               (RANDOM_UNIT() * SMOKE_SCALE_RANGE) +
                                   SMOKE_SCALE,
                               SMOKE_DEPTH_BIAS);
        }
        if (work->tick) {
            work->tick--;
        }
    }
    if (player_work[0]) {
        /* @hack spelling: the beam's direction found by difference, a Float
         * at a time through pointers, where a difference of each coordinate
         * would do */
        difference(LASER(task)->world_right.component,
                   LASER(task)->world_left.component, axis.direction.component);
        /* @hack spelling: axis.origin copied a coordinate at a time, where one
         * assignment of center would do */
        axis.origin.x = LASER(task)->center.x;
        axis.origin.y = LASER(task)->center.y;
        axis.origin.z = LASER(task)->center.z;
        distance =
            njDistanceP2L(&player_work[0]->pos, (NJS_LINE *)&axis, &closest);
        if (distance < HUM_RANGE) {
            length = njDistanceP2P(&LASER(task)->world_left.vector,
                                   &LASER(task)->world_right.vector);
            left_distance =
                njDistanceP2P(&LASER(task)->world_left.vector, &closest);
            right_distance =
                njDistanceP2P(&LASER(task)->world_right.vector, &closest);
            /* the hum plays at the point of the beam's line nearest player
             * 0. where that point lies past the pipes, it plays at the nearer
             * pipe */
            if (left_distance + right_distance > length) {
                if (left_distance > right_distance) {
                    sound_play_at_for(HUM_SOUND, work, HUM_SOUND_PRIORITY, 0,
                                      HUM_SOUND_STEPS,
                                      &LASER(task)->world_right.vector);
                } else {
                    sound_play_at_for(HUM_SOUND, work, HUM_SOUND_PRIORITY, 0,
                                      HUM_SOUND_STEPS,
                                      &LASER(task)->world_left.vector);
                }
            } else {
                sound_play_at_for(HUM_SOUND, work, HUM_SOUND_PRIORITY, 0,
                                  HUM_SOUND_STEPS, &closest);
            }
        }
        if (distance < CONTACT_RANGE) {
            if (!task->child) {
                model_object_create(task, NULL, NULL, shapes,
                                    sizeof(shapes) / sizeof(shapes[0]),
                                    OBJECT_TURNED);
            }
            /* @hack spelling: task->child tested twice, where one test would
             * do */
            if (task->child) {
                if (task->child && task->child->work->collision) {
                    task->child->work->pos = LASER(task)->center;
                    task->child->work->ang.y = work->ang.y;
                    task->child->work->ang.z = work->ang.z;
                    task->child->work->ang.x = 0;
                    task->child->work->collision->shapes[LEFT_END].center =
                        LASER(task)->left;
                    task->child->work->collision->shapes[RIGHT_END].center =
                        LASER(task)->right;
                    task->child->work->collision->shapes[BEAM].size_y =
                        njDistanceP2P(&LASER(task)->left, &LASER(task)->right) *
                        0.5F;
                    collision_fit_record_radius(task->child->work->collision);
                }
            }
        } else if (task->child) {
            task_kill(task->child);
        }
    }
}

static void laser_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack type: camera a pointer to its slot of camera_views, where the
     * slot's camera_place pointer would do */
    struct camera_place **camera;
    NJS_POINT3 scratch;
    /* @hack redundant: scratch, a point that no line reads, and unused, its
     * address */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    Float *unused;
    Float distance;
    NJS_CNK_OBJECT *body;

    /* @hack redundant: statements kept for their strings, the set editor's
     * labels for ang and scl, which SHC places in this source's constants */
    (void)"<- ANGLE X";
    (void)"<- ANGLE Y";
    (void)"<- ANGLE Z";
    (void)"<- LAZERSCL";
    (void)"<- WIDTH";
    (void)"<- SPD";

    camera = &camera_views[camera_current_index];
    /* the laser draws only on the camera's floor */
    if (security_floor_height < (*camera)->pos.y) {
        if (!(LASER(task)->center.y > security_floor_height)) {
            return;
        }
    } else if (LASER(task)->center.y > security_floor_height) {
        return;
    }
    unused = &scratch.x;
    body = security_laser_body;
    distance = njDistanceP2P(&LASER(task)->center, &(*camera)->pos);
    if (distance > body->model->r * LENGTH_SCALE(work) * RANGE_IN_RADII) {
        return;
    }
    njSetTexture(&security_laser_texlist);
    njPushMatrixEx();
    njMultiMatrix(NULL, &LASER(task)->matrix);
    njTranslate(NULL, 0.0F, 0.0F, SWING(task));
    njPushMatrixEx();
    if ((work->mode + (game_state.steps & 1)) & 1) {
        njScale(NULL, LENGTH_SCALE(work), 1.0F, 1.0F);
        njRotateX(NULL, work->timer * beam_spin);
        if (compiled_body) {
            njDirectDrawObject(compiled_body);
        } else {
            njFogDisable();
            njCnkEasyDrawModel(body->model);
            njFogEnable();
        }
        matrix_load_parent();
    }
    if (camera_near(&LASER(task)->world_left.vector, PIPE_DRAW_DISTANCE2)) {
        njTranslateEx(&LASER(task)->left);
        njRotateX(NULL, work->timer * end_spin);
        if (compiled_left) {
            njDirectDrawObject(compiled_left);
        } else {
            njCnkEasyDrawModel(security_laser_left->model);
        }
        matrix_load_parent();
    }
    if (camera_near(&LASER(task)->world_right.vector, PIPE_DRAW_DISTANCE2)) {
        njTranslateEx(&LASER(task)->right);
        njRotateX(NULL, work->timer * end_spin);
        if (compiled_right) {
            njDirectDrawObject(compiled_right);
        } else {
            njCnkEasyDrawModel(security_laser_right->model);
        }
        matrix_load_parent();
    }
    njPopMatrix(2);
}
