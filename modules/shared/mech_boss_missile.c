/* the mech bosses' missile: it rides its launcher, then flies a curve to a
 * target. it leaves a trail of smoke. it bursts where it lands or hits. Big
 * Foot, Hot Shot and Flying Dog fire it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "shared/gun_mech/effect_art.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/flash.h"
#include "shared/mech_boss.h"
#include "shared/mech_boss_missile.h"
#include "shared/spark_sprite.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the missile's own record, in task->motion */
struct missile {
    /* the move a straight step makes, before the curve and past its end */
    NJS_VECTOR spd;
    Uint8 unk_0c[0x10];
    /* the heading to the target at the last mech_boss_missile_turn */
    Angle aim;
    Uint8 unk_20[0x10];
    Angle yaw; /* how far the curve's middle turns about y */
    NJS_POINT3 *pos; /* where the missile rides */
    struct angles *ang; /* the launcher's angles */
    Float speed;
    /* along runs from 0 to 1 over the curve, by step a step */
    Float along;
    Float step;
    NJS_POINT3 target;
    NJS_POINT3 curve[4]; /* the control points of the curve, a cubic Bezier */
};

#define MISSILE(task) ((struct missile *)(task)->motion.p)
/* the work keeps create's radius in scl.x */
#define RADIUS(work) ((work)->scl.x)

/* the stages of a flight, in the missile's work's submode */
enum missile_flight {
    FLIGHT_LAUNCH, /* the first step, which sets the straight flight's move */
    FLIGHT_STRAIGHT,
    FLIGHT_CURVE
};

/* a launch flash's modes, in its work's mode */
enum flash_mode { FLASH_AHEAD, FLASH_BEHIND };

/* the straight flight lasts STRAIGHT_STEPS + 1 steps, at STRAIGHT_PART of the
 * missile's speed */
#define STRAIGHT_STEPS 5
#define STRAIGHT_PART 0.8F
/* the missile bursts past its radius plus OUT_MARGIN from the world's y axis */
#define OUT_MARGIN 40.0F
/* the ground's height. a missile bursts on it, and sparks bounce up off it */
#define GROUND_Y 0.0F
/* the model's offset along y from the missile's pos */
#define MISSILE_Y (-0.8F)
#define SOUND_LAUNCH 0x4000
#define SOUND_BURST 0x4019
#define SOUND_VOLUME 60
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 25

/* a puff of smoke lives SMOKE_STEPS steps. its shade, an index of
 * missile_smoke_draw's colors, is its timer times SMOKE_FADE */
#define SMOKE_STEPS 20
#define SMOKE_FADE 0.8F
/* the smoke is SMOKE_WIDTH across, and SMOKE_LENGTH times the move it marks */
#define SMOKE_WIDTH 1.2F
#define SMOKE_LENGTH 1.2F
#define SMOKE_POINTS 8
#define SMOKE_SHADES 17
#define SMOKE_LEVEL 3
/* the launch flash acts on FLASH_STEPS steps. the first acts
 * AHEAD_FLASH_DISTANCE ahead of the missile. the rest act
 * BEHIND_FLASH_DISTANCE behind it */
#define FLASH_STEPS 5
#define AHEAD_FLASH_DISTANCE 7.5F
#define BEHIND_FLASH_DISTANCE 4.0F
/* the sizes of the flashes of shared/flash.h ahead and behind, and of the
 * smoke of shared/black_smoke.h */
#define AHEAD_FLASH_SIZE 3.0F
#define BEHIND_FLASH_SIZE 5.0F
#define FLASH_SMOKE_SIZE 3.0F
/* the spreads of the sparks of shared/spark_sprite.h ahead and behind, and the
 * speed of those ahead */
#define AHEAD_SPARK_SPREAD 0.7F
#define AHEAD_SPARK_SPEED 1.5F
#define BEHIND_SPARK_SPREAD 1.0F

/* clang-format off */
static struct collision_shape mech_boss_missile_shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(3) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURTS_PLAYERS |
         SHAPE_HURTS_OBJECTS,
     SHAPE_TURN_YZX | SHAPE_HURTABLE, {0.0F, 2.0F, 3.5F}, 4.0F, 6.0F, 0.0F, 0,
     {0, QUARTER_TURN, QUARTER_TURN}},
};
/* clang-format on */
static Float dat_mech_boss_missile_0 = 1.0F;
static Float dat_mech_boss_missile_1 = 1.0F;
/* the sparks behind fly back at behind_spark_speed up to twice that, at
 * random. the smoke flies back at flash_smoke_speed */
static Float behind_spark_speed = 1.5F;
static Float flash_smoke_speed = 0.3F;

static void missile_destroy(struct task *task);
static void missile_ride(struct task *task);
static Angle missile_heading(struct task *task);
static void missile_burst(struct task *task);
static void missile_fly(struct task *task);
static void missile_hit(struct task *task);
static void missile_step(struct task *task);
static void missile_draw(struct task *task);
static void direction_angles(NJS_VECTOR *dir, struct angles *ang);
static void missile_smoke_draw(struct task *task);
static void missile_smoke_step(struct task *task);
static void missile_smoke_destroy(struct task *task);
static void missile_smoke_create(struct task *parent, NJS_POINT3 *from,
                                 NJS_POINT3 *to);
static void missile_flash_step(struct task *task);
static void missile_flash_destroy(struct task *task);
static struct task *missile_flash_create(struct task *parent, NJS_POINT3 *pos,
                                         NJS_VECTOR *dir);

struct task *mech_boss_missile_create(struct task *parent, NJS_POINT3 *pos,
                                      struct angles *ang, Float radius)
{
    struct task *task;
    struct task_work *work;
    struct missile *missile;

    if (parent == NULL) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, missile_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    missile = syCalloc(1, sizeof(struct missile));
    if (missile == NULL) {
        task_kill(task);
        return NULL;
    }
    task->motion.p = missile;
    task->draw = missile_draw;
    task->destructor = missile_destroy;
    work->mode = MECH_BOSS_MISSILE_RIDING;
    missile->pos = pos;
    missile->ang = ang;
    work->pos = *pos;
    RADIUS(work) = radius;
    work->id = WORK_ID_ATTACK;
    collision_init(task, mech_boss_missile_shape, 1, COLLISION_ENEMY);
    /* @hack spelling: the work reached again through task, where work
     * would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    return task;
}

static void missile_destroy(struct task *task)
{
}

static void missile_ride(struct task *task)
{
    struct task_work *work = task->work;
    struct missile *missile = MISSILE(task);

    work->pos = *missile->pos;
    work->ang = *missile->ang;
}

Float mech_boss_missile_target_distance2_flat(struct task *task)
{
    NJS_POINT3 *pos = &task->work->pos;
    NJS_POINT3 *target = &MISSILE(task)->target;

    return ((target->x - pos->x) * (target->x - pos->x)) +
           ((target->z - pos->z) * (target->z - pos->z));
}

static Angle missile_heading(struct task *task)
{
    struct task_work *work = task->work;
    struct missile *missile = MISSILE(task);
    NJS_POINT3 *pos = &work->pos;
    NJS_POINT3 *target = &missile->target;

    return atan2f(target->x - pos->x, target->z - pos->z) * ANGLE_PER_RADIAN;
}

Angle mech_boss_missile_turn(struct task *task, Angle step)
{
    struct task_work *work = task->work;
    struct missile *missile = MISSILE(task);

    missile->aim = missile_heading(task);
    work->ang.y = angle_approach(work->ang.y, missile->aim, step);
    return angle_difference(missile->aim, work->ang.y);
}

static void missile_burst(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: an initialized point nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    NJS_POINT3 unused = {0.0F, 0.2F, 0.0F};

    sound_play_at(SOUND_BURST, NULL, 0, SOUND_VOLUME, &work->pos);
    blast_create(BLAST_SMALL_RAISED, &work->pos);
    task_kill(task);
}

static void missile_fly(struct task *task)
{
    struct task_work *work = task->work;
    struct missile *missile = MISSILE(task);
    NJS_POINT3 last;

    last = work->pos;
    switch (work->submode) {
    case FLIGHT_LAUNCH:
        work->timer = STRAIGHT_STEPS;
        /* @hack spelling: back, dir, unit and speed declared in a block of
         * their own, where the function's top would do */
        {
            NJS_VECTOR back = {-1.0F, 0.0F, 0.0F};
            NJS_VECTOR dir;
            NJS_VECTOR unit;
            Float speed;

            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
            njCalcVector(NULL, &back, &dir);
            njPopMatrix(1);
            speed = missile->speed * STRAIGHT_PART;
            missile->spd.x = dir.x * speed;
            missile->spd.z = dir.z * speed;
            missile->spd.y = 0.0F;
            unit = missile->spd;
            njUnitVector(&unit);
            missile_flash_create(task, &work->pos, &unit);
            work->submode++;
        }
        /* fall through */
    case FLIGHT_STRAIGHT:
        work->pos.x += missile->spd.x;
        work->pos.z += missile->spd.z;
        if (work->timer-- == 0) {
            NJS_VECTOR middle;
            Float distance2;

            middle.x = (missile->target.x - work->pos.x) * 0.5F;
            middle.y = (missile->target.y - work->pos.y) * 0.5F;
            middle.z = (missile->target.z - work->pos.z) * 0.5F;
            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, missile->yaw);
            njCalcVector(NULL, &middle, &middle);
            njAddVector(&middle, &work->pos);
            njPopMatrix(1);
            missile->curve[0] = work->pos;
            missile->curve[1] = middle;
            missile->curve[2] = middle;
            missile->curve[2].x -= (middle.x - missile->target.x) * 0.5F;
            missile->curve[2].y -= (middle.y - missile->target.y) * 0.5F;
            missile->curve[2].z -= (middle.z - missile->target.z) * 0.5F;
            missile->curve[3] = missile->target;
            distance2 = ((missile->target.x - work->pos.x) *
                         (missile->target.x - work->pos.x)) +
                        ((missile->target.y - work->pos.y) *
                         (missile->target.y - work->pos.y)) +
                        ((missile->target.z - work->pos.z) *
                         (missile->target.z - work->pos.z));
            /* @hack spelling: distance2 reused for the step, where the
             * product stored in missile->step would do */
            distance2 = missile->speed * njInvertSqrt(distance2);
            missile->step = distance2;
            missile->along = 0.0F;
            work->submode++;
        }
        break;
    case FLIGHT_CURVE:
        if (missile->along <= 1.0F) {
            Float rest = 1.0F - missile->along;
            Float b0 = rest * rest * rest;
            Float b1 = rest * rest * missile->along * 3.0F;
            Float b2 = rest * missile->along * missile->along * 3.0F;
            Float b3 = missile->along * missile->along * missile->along;

            work->pos.x =
                (missile->curve[0].x * b0) + (missile->curve[1].x * b1) +
                (missile->curve[2].x * b2) + (missile->curve[3].x * b3);
            work->pos.y =
                (missile->curve[0].y * b0) + (missile->curve[1].y * b1) +
                (missile->curve[2].y * b2) + (missile->curve[3].y * b3);
            work->pos.z =
                (missile->curve[0].z * b0) + (missile->curve[1].z * b1) +
                (missile->curve[2].z * b2) + (missile->curve[3].z * b3);
            missile->along += missile->step;
            if (missile->along > 1.0F) {
                missile->spd.x = work->pos.x - last.x;
                missile->spd.y = work->pos.y - last.y;
                missile->spd.z = work->pos.z - last.z;
            }
        } else {
            njAddVector(&work->pos, &missile->spd);
        }
        missile_smoke_create(NULL, &last, &work->pos);
        break;
    }
    /* @hack spelling: move and across declared in a block of their own,
     * where the function's top would do */
    {
        NJS_VECTOR move;
        Float across;

        move.x = work->pos.x - last.x;
        move.y = work->pos.y - last.y;
        move.z = work->pos.z - last.z;
        across = njSqrt((move.x * move.x) + (move.z * move.z));
        work->ang.y = atan2f(move.x, move.z) * ANGLE_PER_RADIAN;
        work->ang.x = -(Angle)(atan2f(move.y, across) * ANGLE_PER_RADIAN);
    }
    if (work->pos.y <= GROUND_Y) {
        missile_burst(task);
        vibration_start(0, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_UNTIMED);
    }
    if (njSqrt((work->pos.x * work->pos.x) + (work->pos.z * work->pos.z)) >
        RADIUS(work) + OUT_MARGIN) {
        missile_burst(task);
    }
}

static void missile_hit(struct task *task)
{
    if (collision_next_player(task)) {
        missile_burst(task);
    }
    if (collision_next_object(task)) {
        missile_burst(task);
    }
    collision_register(task);
}

void mech_boss_missile_launch(struct task *task, Angle yaw, Float speed,
                              NJS_POINT3 target)
{
    struct task_work *work = task->work;
    struct missile *missile = MISSILE(task);

    work->ang = *missile->ang;
    missile->target = target;
    missile->yaw = yaw;
    missile->speed = speed;
    sound_play_at(SOUND_LAUNCH, NULL, 0, SOUND_VOLUME, &work->pos);
    work->mode = MECH_BOSS_MISSILE_FLYING;
    work->submode = FLIGHT_LAUNCH;
}

Sint8 mech_boss_missile_mode(struct task *task)
{
    struct task_work *work;

    /* @hack spelling: the two tests in arms of their own, where one test
     * joined with || would do */
    if (task == NULL) {
        return MECH_BOSS_MISSILE_GONE;
    } else if (task->work == NULL) {
        return MECH_BOSS_MISSILE_GONE;
    }
    work = task->work;
    return work->mode;
}

static void missile_step(struct task *task)
{
    struct task_work *work = task->work;

    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    switch (work->mode) {
    case MECH_BOSS_MISSILE_RIDING:
        missile_ride(task);
        break;
    case MECH_BOSS_MISSILE_FLYING:
        missile_fly(task);
        missile_hit(task);
        break;
    }
}

static void missile_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&grobo_texlist);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njTranslate(NULL, 0.0F, MISSILE_Y, 0.0F);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, QUARTER_TURN);
    njCnkSimpleDrawObject(&mech_boss_missile_object);
    if (work->mode == MECH_BOSS_MISSILE_FLYING) {
        draw_control_set(DRAW_CONTROL_SHADOW);
        njCnkModDrawObject(&mech_boss_missile_shadow);
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    njPopMatrixEx();
}

/* the angles that turn +z to point along dir: ang.y about y, then ang.x about
 * x. ang.z becomes 0. a dir of length 0 leaves ang as it was */
static void direction_angles(NJS_VECTOR *dir, struct angles *ang)
{
    Float length = njScalor(dir);

    if (length != 0.0F) {
        ang->y = atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN;
        ang->x = asinf(-dir->y / length) * ANGLE_PER_RADIAN;
        ang->z = 0;
    }
}

static void missile_smoke_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* a puff of the smoke: a prism of three sides along z from 0 to 1. it
     * draws as a strip of triangles. the even points stand at z = 1, the odd
     * at z = 0 */
    NJS_POLYGON_VTX points[SMOKE_POINTS] = {
#include "shared/mech_boss_missile/points.inc"
    };
    Uint32 colors[SMOKE_SHADES] = {
        0xff555555, 0xe0555555, 0xd0555555, 0xc0555555, 0xb0555555, 0xa0555555,
        0x90555555, 0x80555555, 0x70555555, 0x60555555, 0x50555555, 0x40555555,
        0x30555555, 0x20555555, 0x10555555, 0x00000000, 0x00000000,
    };
    Sint8 shade;

    shade = work->timer * SMOKE_FADE;
    points[0].col = points[2].col = points[4].col = points[6].col =
        colors[shade];
    /* @bug memory: reads one past colors on the puff's last draw. the step
     * that kills the puff, at a timer of SMOKE_STEPS, leaves it one more
     * draw, where shade is 16. the word past colors is points[0].x, 0.0F
     * (0x8d4 of the object's code) */
#ifdef NONMATCHING
    /* this side keeps what the image reads past colors: 0.0F's bits, a clear
     * color */
    points[1].col = points[3].col = points[5].col = points[7].col =
        (shade + 1 < SMOKE_SHADES) ? colors[shade + 1] : 0;
#else
    points[1].col = points[3].col = points[5].col = points[7].col =
        colors[shade + 1];
#endif
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njScale(NULL, SMOKE_WIDTH, SMOKE_WIDTH, work->scl.x);
    njDrawPolygon3DEx(points, SMOKE_POINTS, 1);
    njRotateX(NULL, QUARTER_TURN);
    njDrawPolygon3DEx(points, SMOKE_POINTS, 1);
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void missile_smoke_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!game_state.time_stopped && !game_state.unk_09) {
        if (++work->timer >= SMOKE_STEPS) {
            task_kill(task);
        }
    }
}

static void missile_smoke_destroy(struct task *task)
{
}

/* create a puff of the smoke along the move from *from to *to. it is parent's
 * child, or a task of SMOKE_LEVEL for a null parent */
static void missile_smoke_create(struct task *parent, NJS_POINT3 *from,
                                 NJS_POINT3 *to)
{
    struct task *task;
    struct task_work *work;
    NJS_VECTOR move;

    if (parent == NULL) {
        task = task_create(TASK_WORK, SMOKE_LEVEL, missile_smoke_step,
                           "BossGroboMSmokeCExec");
    } else {
        task = task_create_child(TASK_WORK, missile_smoke_step, parent);
    }
    if (task != NULL) {
        work = task->work;
        work->timer = 0;
        work->mode = 0;
        work->pos = *from;
        move = *to;
        njSubVector(&move, from);
        direction_angles(&move, &work->ang);
        work->scl.x = njScalor(&move) * SMOKE_LENGTH;
        task->destructor = missile_smoke_destroy;
        task->draw = missile_smoke_draw;
    }
}

static void missile_flash_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR *dir = task->motion.p;

    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case FLASH_AHEAD: {
        NJS_VECTOR spd;
        NJS_POINT3 pos;

        spd.x = spd.y = spd.z = 0.0F;
        pos = work->pos;
        pos.x += dir->x * AHEAD_FLASH_DISTANCE;
        pos.z += dir->z * AHEAD_FLASH_DISTANCE;
        flash_create(&pos, &spd, AHEAD_FLASH_SIZE, 0.0F);
        spd.x = dir->x * AHEAD_SPARK_SPEED;
        spd.y = 0.0F;
        spd.z = dir->z * AHEAD_SPARK_SPEED;
        spark_sprite_create(&pos, &spd, AHEAD_SPARK_SPREAD, GROUND_Y);
        spark_sprite_create(&pos, &spd, AHEAD_SPARK_SPREAD, GROUND_Y);
        work->mode++;
        break;
    }
    case FLASH_BEHIND:
        if (--work->tick > 0) {
            NJS_POINT3 pos;
            NJS_VECTOR spd;
            Float speed;

            pos = work->pos;
            pos.x -= dir->x * BEHIND_FLASH_DISTANCE;
            pos.z -= dir->z * BEHIND_FLASH_DISTANCE;
            speed =
                behind_spark_speed + (rand() * RAND_SCALE * behind_spark_speed);
            spd.x = -(dir->x * speed);
            spd.y = 0.0F;
            spd.z = -(dir->z * speed);
            spark_sprite_create(&pos, &spd, BEHIND_SPARK_SPREAD, GROUND_Y);
            spd.x = -(dir->x * flash_smoke_speed);
            spd.y = 0.0F;
            spd.z = -(dir->z * flash_smoke_speed);
            black_smoke_sinking_create(&pos, &spd, FLASH_SMOKE_SIZE);
            spd.x = spd.y = spd.z = 0.0F;
            flash_create(&pos, &spd, BEHIND_FLASH_SIZE, 0.0F);
        } else {
            task_kill(task);
        }
        break;
    }
}

static void missile_flash_destroy(struct task *task)
{
}

static struct task *missile_flash_create(struct task *parent, NJS_POINT3 *pos,
                                         NJS_VECTOR *dir)
{
    struct task *task;
    struct task_work *work;
    NJS_VECTOR *flash_dir;

    if (parent == NULL) {
        return NULL;
    }
    task =
        task_create_child(TASK_MOTION | TASK_WORK, missile_flash_step, parent);
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    flash_dir = task->motion.p;
    work->pos = *pos;
    *flash_dir = *dir;
    work->tick = FLASH_STEPS;
    task->destructor = missile_flash_destroy;
    return task;
}
