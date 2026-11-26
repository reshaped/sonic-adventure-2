/* the laser an enemy fires. it charges as a ring closes in, then stretches
 * out as a beam that flies and bursts */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/enemy_laser.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"

/* the laser's work->mode */
enum laser_mode {
    MODE_CHARGE,
    MODE_STRETCH,
    MODE_FLY,
    MODE_BURST,
    MODE_DONE /* the burst has ended, and the step that set it kills the task */
};

/* a charge raises CHARGE from CHARGE_START by CHARGE_STEP a step, and ends
 * once it reaches CHARGE_FULL */
#define CHARGE(work) ((work)->scl.x)
#define CHARGE_START 0.02F
#define CHARGE_STEP 0.02F
#define CHARGE_FULL 1.0F
/* pos is the beam's center, and each end lies HALF_LENGTH from it. the beam
 * stops growing once HALF_LENGTH reaches FULL_HALF_LENGTH */
#define HALF_LENGTH(work) ((work)->scl.z)
#define FULL_HALF_LENGTH(enemy) ((enemy)->unk_09c)

/* the ring's scale is RING_SCALE / sqrt(CHARGE) across the laser's heading,
 * so it closes in as CHARGE rises. its scale along the heading is
 * RING_DEPTH */
#define RING_SCALE 2.0F
#define RING_DEPTH 5.0F
/* the beam's model is about 10 long along z. the draw scales z by
 * BEAM_SCALE times HALF_LENGTH, so the model draws about as long as the
 * beam */
#define BEAM_SCALE 0.2F

/* a burst grows RADIUS from BURST_RADIUS by BURST_GROWTH a step, up to
 * BURST_RADIUS_MAX. it then lowers HOLD from 0 by HOLD_STEP a step, and ends
 * once HOLD lies below HOLD_END. nothing draws a burst, so the two only time
 * it */
#define RADIUS(work) ((work)->scl.x)
#define HOLD(work) ((work)->scl.y)
#define BURST_RADIUS 5.0F
#define BURST_RADIUS_MAX 25.0F
#define BURST_GROWTH 1.0F
#define HOLD_STEP 0.05F
#define HOLD_END (-1.0F)

/* a flying laser is armed once work->timer passes ARMED_STEPS. neither a
 * touch nor the work's flags burst it before. the timer counts from the
 * laser's start, its charge included */
#define ARMED_STEPS 20
/* what a flying laser adds to its ang.z, and a bursting one to its ang.y,
 * a step */
#define FLY_SPIN 0x1400
#define BURST_SPIN 0x100
/* enemy_meet_stage's probe reaches PROBE_SIZE above, below and to each side
 * of pos */
#define PROBE_SIZE 3.0F
#define LASER_LEVEL 3 /* the task level of a laser without a parent */
#define TEXTURE_LEVEL 0 /* the texture task's level */
#define CHARGE_SOUND 0x4001
#define BEAM_SOUND 0x4002

/* the beam's model, a chunk model of BEAM_POINTS vertices, each a point and
 * a color. the beam's length lies along z */
#define BEAM_POINTS 15
static Sint16 laser_beam_polygons[169] = {
#include "shared/enemy_laser/laser_beam_polygons.inc"
};
static Sint32 laser_beam_vertices[63] = {
#include "shared/enemy_laser/laser_beam_vertices.inc"
};
static NJS_CNK_MODEL laser_beam_model = {
#include "shared/enemy_laser/laser_beam_model.inc"
};
/* the node that holds the model, which nothing reads */
static NJS_CNK_OBJECT laser_beam_object = {
#include "shared/enemy_laser/laser_beam_object.inc"
};

/* the ring a charging laser draws, a chunk model of RING_POINTS vertices,
 * each a point and a color. the ring lies across z */
#define RING_POINTS 24
static Sint16 laser_ring_polygons[99] = {
#include "shared/enemy_laser/laser_ring_polygons.inc"
};
static Sint32 laser_ring_vertices[99] = {
#include "shared/enemy_laser/laser_ring_vertices.inc"
};
static NJS_CNK_MODEL laser_ring_model = {
#include "shared/enemy_laser/laser_ring_model.inc"
};
/* the node that holds the model, which nothing reads */
static NJS_CNK_OBJECT laser_ring_object = {
#include "shared/enemy_laser/laser_ring_object.inc"
};

#include "shared/enemy_laser/text.inc"

static NJS_TEXNAME laser_texname[1] = {
#include "shared/enemy_laser/laser_texname.inc"
};
static NJS_TEXLIST laser_texlist = {
#include "shared/enemy_laser/laser_texlist.inc"
};

/* the _with_id queries match the id stored in this shape */
static struct collision_shape laser_shape = {
#include "shared/enemy_laser/laser_shape.inc"
};

/* the task that keeps laser_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *laser_textures = NULL;

static void laser_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    njAddVector(&work->pos, &enemy->spd);
    enemy->unk_00c.x = 0.0F;
    enemy->unk_00c.y = 0.0F;
    enemy->unk_00c.z = 0.0F;
}

/* the MODE_BURST branch never runs: laser_step clears the draw in the step a
 * laser bursts */
static void laser_draw(struct task *task)
{
    struct task_work *work = task->work;

    njFogDisable();
    if (work->mode == MODE_CHARGE) {
        NJS_VECTOR scl;

        scl.x = njInvertSqrt(CHARGE(work)) * RING_SCALE;
        scl.y = scl.x;
        scl.z = RING_DEPTH;
        njSetTexture(&laser_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScaleEx(&scl);
        njCnkSimpleDrawModel(&laser_ring_model);
        njPopMatrixEx();
    } else if (work->mode == MODE_BURST) {
        struct angles ang;
        NJS_VECTOR scl;

        ang.x = ang.y = ang.z = 0;
        /* @hack spelling: the chain from scl.z to scl.x, where one from
         * scl.x to scl.z would do */
        scl.z = scl.y = scl.x = RADIUS(work);
        debug_draw_model(&work->pos, &ang, &scl, DEBUG_MODEL_SPHERE);
    } else {
        njSetTexture(&laser_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njScale(NULL, 1.0F, 1.0F, HALF_LENGTH(work) * BEAM_SCALE);
        njCnkEasyDrawModel(&laser_beam_model);
        njPopMatrixEx();
    }
    njFogEnable();
}

static void laser_check_hit(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: hit, a flag the test sets and the next test compares
     * with 1, where the first test alone would do */
    int hit = 0;

    if ((work->flags & (WORK_UNK_1 | WORK_HURT | WORK_UNK_8)) ||
        (work->collision->flags & COLLISION_TOUCHED)) {
        hit = 1;
    }
    if (hit == 1) {
        work->mode = MODE_BURST;
        work->timer = 0;
        RADIUS(work) = BURST_RADIUS;
        HOLD(work) = 0.0F;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

static void laser_charge(struct task_work *work, struct enemy_work *enemy)
{
    if (CHARGE(work) >= CHARGE_FULL) {
        work->mode = MODE_STRETCH;
        CHARGE(work) = CHARGE_FULL;
        sound_play_at(BEAM_SOUND, NULL, 0, 0, &work->pos);
        /* @hack redundant: the return that ends the arm, and the else after
         * it, where either alone would do */
        return;
    } else if (!game_state.time_stopped) {
        CHARGE(work) += CHARGE_STEP;
    }
}

/* grow the beam from its front end, by spd a step. pos, the beam's center,
 * moves by half of spd, so the back end holds still */
static void laser_stretch(struct task_work *work, struct enemy_work *enemy)
{
    if (HALF_LENGTH(work) >= FULL_HALF_LENGTH(enemy)) {
        work->mode = MODE_FLY;
        HALF_LENGTH(work) = FULL_HALF_LENGTH(enemy);
    } else if (!game_state.time_stopped) {
        work->pos.x += enemy->spd.x * 0.5F;
        work->pos.y += enemy->spd.y * 0.5F;
        work->pos.z += enemy->spd.z * 0.5F;
        HALF_LENGTH(work) += njScalor(&enemy->spd) * 0.5F;
    }
    work->collision->shapes->size_y = HALF_LENGTH(work);
    collision_fit_record_radius(work->collision);
}

static void laser_fly(struct task_work *work, struct enemy_work *enemy)
{
    if (work->timer > ARMED_STEPS) {
        laser_check_hit(work, enemy);
    }
    laser_move(work, enemy);
    enemy_meet_stage(work, enemy);
    if (enemy->flags & (ENEMY_UNDER_CEILING | ENEMY_ON_FLOOR | ENEMY_AT_WALL)) {
        work->mode = MODE_BURST;
        work->timer = 0;
        RADIUS(work) = BURST_RADIUS;
        HOLD(work) = 0.0F;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
    if (!game_state.time_stopped) {
        work->ang.z += FLY_SPIN;
    }
}

static void laser_burst(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (RADIUS(work) < BURST_RADIUS_MAX) {
        RADIUS(work) += BURST_GROWTH;
    } else {
        HOLD(work) -= HOLD_STEP;
        if (HOLD(work) < HOLD_END) {
            work->mode = MODE_DONE;
            work->timer = 0;
        }
    }
    work->ang.y += BURST_SPIN;
}

static void laser_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void laser_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_CHARGE:
        laser_charge(work, enemy);
        if (task->parent != NULL) {
            task->parent->work->flags |= ENEMY_LASER_CHARGING;
        }
        break;
    case MODE_STRETCH:
        laser_stretch(work, enemy);
        break;
    case MODE_FLY:
        laser_fly(work, enemy);
        if (work->mode == MODE_BURST) {
            task->draw = NULL;
        }
        break;
    case MODE_BURST:
        laser_burst(work, enemy);
        /* @hack redundant: the reads of the record's latest hit and of that
         * hit's kind, which nothing uses */
        if (work->collision->hit != NULL) {
            if (work->collision->hit->kind == 0) {
            }
        }
        if (work->mode != MODE_BURST) {
            task_kill(task);
            return;
        }
        break;
    }
    /* the shape's length lies along its y, and the beam's along z. a quarter
     * turn more about x lines the two up */
    work->collision->shapes->ang.x = work->ang.x + QUARTER_TURN;
    work->collision->shapes->ang.y = work->ang.y;
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer++;
    }
}

static void laser_init(struct task *task, Float length, NJS_VECTOR *dir)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;

    collision_init(task, &laser_shape, 1, COLLISION_ENEMY);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, length);
    enemy = enemy_work_alloc(task);
    /* @bug memory: writes spd through null where enemy_work_alloc finds no
     * room, at 0x508 of the object's code. enemy_laser_aim tests a laser's
     * enemy_work for null, at 0x726 of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: where enemy_work_alloc has killed the
     * laser, the init returns */
    if (enemy == NULL) {
        return;
    }
#endif
    enemy->spd = *dir;
    FULL_HALF_LENGTH(enemy) = length * 0.5F;
    HALF_LENGTH(work) = 0.0F;
    work->mode = MODE_CHARGE;
    CHARGE(work) = CHARGE_START;
    work->ang.x = (Angle)(asinf(-enemy->spd.y / njScalor(&enemy->spd)) *
                          ANGLE_PER_RADIAN);
    work->ang.y =
        (Angle)(atan2f(enemy->spd.x, enemy->spd.z) * ANGLE_PER_RADIAN);
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = PROBE_SIZE;
    enemy->radius = PROBE_SIZE;
    enemy->bottom = -PROBE_SIZE;
    enemy_set_home(work, enemy);
    task->destructor = laser_destroy;
    task->draw = laser_draw;
}

static void laser_textures_release(struct task *task)
{
    njReleaseTexture(&laser_texlist);
    laser_textures = NULL;
}

static void laser_textures_step(struct task *task)
{
}

void enemy_laser_load(void)
{
    if (laser_textures != NULL) {
        return;
    }
    texture_load_pvm("E_LASTEX", &laser_texlist);
    laser_textures =
        task_create(0, TEXTURE_LEVEL, laser_textures_step, "ManTex");
    laser_textures->destructor = laser_textures_release;
}

void enemy_laser_fire(struct task *parent, Float length, NJS_VECTOR *dir,
                      NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, laser_step, parent);
    } else {
        task = task_create(TASK_WORK, LASER_LEVEL, laser_step,
                           "EnemyLaserExecutor");
    }
    if (task == NULL) {
        return;
    }
    task->work->pos = *pos;
    laser_init(task, length, dir);
    enemy_laser_load();
    sound_play_at(CHARGE_SOUND, NULL, 0, 0, pos);
}

void enemy_laser_aim(struct task *parent, NJS_VECTOR *dir, NJS_POINT3 *pos)
{
    struct task *child;
    struct task *laser;
    struct task_work *work;
    struct enemy_work *enemy;

    if (parent == NULL) {
        return;
    }
    child = parent->child;
    if (child == NULL) {
        return;
    }
    laser = NULL;
    do {
        if (child->step == laser_step && child->work != NULL &&
            child->work->mode == MODE_CHARGE) {
            laser = child;
            break;
        }
        child = child->next;
    } while (child != parent->child);
    if (laser == NULL) {
        return;
    }
    work = laser->work;
    enemy = laser->motion.p;
    /* @hack redundant: the tests of work and of its mode, which the search
     * has just made */
    if (work == NULL || enemy == NULL || work->mode != MODE_CHARGE) {
        return;
    }
    if (pos != NULL) {
        work->pos = *pos;
    }
    /* @hack spelling: dir tested again for the angles, where one test for
     * the speed and the angles would do */
    if (dir != NULL) {
        enemy->spd = *dir;
    }
    if (dir != NULL) {
        work->ang.x =
            (Angle)(asinf(-dir->y / njScalor(dir)) * ANGLE_PER_RADIAN);
        work->ang.y = (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN);
    }
}

void enemy_laser_fire_charged(struct task *parent, Float length,
                              NJS_VECTOR *dir, NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, laser_step, parent);
    } else {
        task = task_create(TASK_WORK, LASER_LEVEL, laser_step,
                           "EnemyLaserExecutor");
    }
    if (task == NULL) {
        return;
    }
    task->work->pos = *pos;
    laser_init(task, length, dir);
    enemy_laser_load();
    task->work->mode = MODE_STRETCH;
    CHARGE(task->work) = CHARGE_FULL;
    sound_play_at(BEAM_SOUND, NULL, 0, 0, pos);
}
