#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_last2/arena.h"
#include "boss_last2/panel_sparks.h"
#include "boss_last2/panels.h"
#include "camera/camera.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "shared/last_boss_arena.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

/* a spark flies out from its emitter, or toward the camera */
enum spark_kind { SPARK_RADIAL, SPARK_CAMERA };
/* an emitter's kind, which sets the sparks it throws */
enum emitter_kind { EMITTER_HIT, EMITTER_BODY };
/* the steps a hit's emitter and the body's throw sparks for */
#define HIT_SPARK_STEPS 60
#define BODY_SPARK_STEPS 30
/* the body's sparks and explosions start at a slot among the first
 * SPARK_COLUMNS of a row */
#define SPARK_COLUMNS 5
/* a spark shows one of the SPARK_TEXTURES from SPARK_FIRST_TEXTURE, at
 * SPRITE_SCALE for each unit of its size. it fades once it has lived
 * SPARK_FULL_STEPS */
#define SPARK_FIRST_TEXTURE 6
#define SPARK_TEXTURES 2
#define SPRITE_SCALE 0.15625F
#define SPARK_FULL_STEPS 3
/* an explosion's size, and the most it starts off its slot's front in x and
 * in z */
#define EXPLOSION_SIZE 15.0F
#define EXPLOSION_SPREAD 10.0F
/* a random number below picks: a random value below spread, as a byte, mod
 * picks */
#define RANDOM_PICK(spread, picks)                                             \
    ((Sint8)(rand() * ((spread) * RAND_SCALE)) % (picks))
/* each step, an emitter picks a number below its kind's _PICKS. a hit's
 * throws a spark out for a pick below HIT_OUT, and else one toward the
 * camera. the body's throws a spark out below BODY_OUT, one toward the
 * camera below BODY_TOWARD, and else none. each throws one more spark toward
 * the camera */
#define HIT_PICKS 5
#define HIT_OUT 2
#define BODY_PICKS 10
#define BODY_OUT 2
#define BODY_TOWARD 6

/* move point within range of where it is on each axis, and set delta to the
 * move */
#define SCATTER_POINT(point, delta, range)                                     \
    do {                                                                       \
        (delta).x = FINALHAZARD_SCATTER(range);                                \
        (delta).y = FINALHAZARD_SCATTER(range);                                \
        (delta).z = FINALHAZARD_SCATTER(range);                                \
        (point).x += (delta).x;                                                \
        (point).y += (delta).y;                                                \
        (point).z += (delta).z;                                                \
    } while (0)

/* an emitter's work: a task_work with kind for submode, and follow for
 * ang.x */
struct emitter_work {
    Sint8 mode;
    Sint8 kind;
    Uint8 id;
    Uint8 tick;
    Sint16 flags;
    Uint16 timer;
    const NJS_POINT3 *follow; /* the point it moves with, or null */
    Angle ang_y;
    Angle ang_z;
    NJS_POINT3 pos;
};
#define EMITTER(task) ((struct emitter_work *)(task)->work)

static const char spark_name[] = "BL2WeakPtclExec";
/* the point on a slot where the body's sparks and explosions start, and the
 * front from there. each pair is panels.c's panel_origin and panel_front */
static const NJS_POINT3 emitter_origin = {0, 1, 0};
static const NJS_VECTOR emitter_offset = {0, 15, 0};
static const NJS_POINT3 burst_origin = {0, 1, 0};
static const NJS_VECTOR burst_offset = {0, 15, 0};

struct panel_spark_tuning finalhazard_spark_tuning = {
    15, 25, 1.5F, .04F, .02F, .1F, 2.5F, .2F, .4F, .02F, .3F, .2F};
static NJS_TEXANIM cell = {
#include "boss_last2/panel_sparks/cell.inc"
};
static NJS_SPRITE spark_sprite = {
    {0, 0, 0}, 0, 0, 0, &finalhazard_effect_texlist, NULL};

static void panel_spark_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: sprite, a pointer to spark_sprite, where spark_sprite
     * itself would do */
    NJS_SPRITE *sprite = &spark_sprite;
    sprite->tanim = &cell;
    sprite->p = work->pos;
    sprite->sx = sprite->sy = SPARK_SIZE(work) * SPRITE_SCALE;
    sprite->tanim->texid = SPARK_TEXTURE(work) + SPARK_FIRST_TEXTURE;
    constant_material_set(SPARK_BRIGHTNESS(work), SPARK_BRIGHTNESS(work),
                          SPARK_BRIGHTNESS(work), SPARK_BRIGHTNESS(work));
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njDrawSprite3D(sprite, 0,
                   NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void panel_spark_destroy(struct task *task)
{
    struct task_motion *motion = task->motion.p;
    if (motion) {
        syFree(motion);
        task->motion.p = NULL;
    }
}

static struct task *panel_spark_create(const NJS_POINT3 *center, Sint8 kind)
{
    struct task *task;
    struct task_work *work;
    struct task_motion *motion;
    NJS_VECTOR offset;
    NJS_POINT3 eye;
    /* @hack spelling: range, a pointer to the spread, where the field in
     * SCATTER_POINT would do */
    Float *range;
    Float size;
    task = task_create(TASK_WORK | TASK_MOTION, 2, finalhazard_panel_spark_step,
                       spark_name);
    if (!task) {
        return NULL;
    }
    task->draw = panel_spark_draw;
    task->destructor = panel_spark_destroy;
    work = task->work;
    motion = task->motion.p;
    if (kind == SPARK_RADIAL) {
        work->pos = *center;
        range = &finalhazard_spark_tuning.scatter;
        SCATTER_POINT(work->pos, offset, *range);
        last_boss_vector_length(
            &offset,
            finalhazard_spark_tuning.speed +
                (rand() * finalhazard_spark_tuning.speed_range * RAND_SCALE),
            &motion->acc);
    } else {
        work->pos = *center;
        /* @hack spelling: eye, a copy of the camera's pos, where
         * camera_views[0]->pos would do */
        eye = camera_views[0]->pos;
        offset.x = eye.x - center->x;
        offset.y = eye.y - center->y;
        offset.z = eye.z - center->z;
        last_boss_vector_length(
            &offset,
            finalhazard_spark_tuning.camera_speed +
                (rand() * finalhazard_spark_tuning.camera_speed_range *
                 RAND_SCALE),
            &motion->acc);
        range = &finalhazard_spark_tuning.camera_scatter;
        SCATTER_POINT(work->pos, offset, *range);
    }
    SPARK_BRIGHTNESS(work) = 1.0F;
    size = finalhazard_spark_tuning.size;
    SPARK_SIZE(work) = size + (size * rand() * RAND_SCALE);
    SPARK_GROWTH(work) =
        finalhazard_spark_tuning.growth +
        (finalhazard_spark_tuning.growth_range * rand() * RAND_SCALE);
    SPARK_TEXTURE(work) = RANDOM_PICK(10.0F, SPARK_TEXTURES);
    SPARK_KIND(work) = kind;
    work->timer = SPARK_FULL_STEPS;
    return task;
}

static void panel_emitter_step(struct task *task)
{
    struct emitter_work *work = EMITTER(task);
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    if (work->follow) {
        work->pos = *work->follow;
    }
    if (work->kind == EMITTER_HIT) {
        Sint8 choice = RANDOM_PICK(20.0F, HIT_PICKS);
        if (choice < HIT_OUT) {
            panel_spark_create(&work->pos, SPARK_RADIAL);
        } else {
            panel_spark_create(&work->pos, SPARK_CAMERA);
        }
        panel_spark_create(&work->pos, SPARK_CAMERA);
    } else if (work->kind == EMITTER_BODY) {
        Sint8 choice = RANDOM_PICK(20.0F, BODY_PICKS);
        if (choice < BODY_OUT) {
            panel_spark_create(&work->pos, SPARK_RADIAL);
        } else if (choice < BODY_TOWARD) {
            panel_spark_create(&work->pos, SPARK_CAMERA);
        }
        panel_spark_create(&work->pos, SPARK_CAMERA);
    }
    if (--work->timer == 0) {
        task_kill(task);
    }
}

static void panel_emitter_destroy(struct task *task)
{
}

struct task *finalhazard_hit_sparks(struct task *parent,
                                    const NJS_POINT3 *front,
                                    struct angles *angles)
{
    struct emitter_work *work;
    struct task *task;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, panel_emitter_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = panel_emitter_destroy;
    work = EMITTER(task);
    work->follow = front;
    work->pos = *front;
    work->timer = HIT_SPARK_STEPS;
    work->kind = EMITTER_HIT;
    return task;
}

/* set row, column and slot to a slot picked at random, of the first
 * SPARK_COLUMNS of its row */
#define PICK_SLOT(row, column, slot)                                           \
    do {                                                                       \
        (row) = RANDOM_PICK(10.0F, PANEL_ROWS);                                \
        (column) = RANDOM_PICK(64.0F, SPARK_COLUMNS);                          \
        (slot) = &finalhazard_panel_slots[row][column];                        \
    } while (0)

/* set result to point on slot, in the world, as panels.c's position_panel
 * places the weak point. the slot's matrix stays pushed for SLOT_OFFSET */
#define SLOT_ORIGIN(slot, point, turns, local, world, result)                  \
    do {                                                                       \
        (local) = (point);                                                     \
        /* @hack spelling: turns, a pointer to the slot's angles taken before  \
         * the matrix loads, where &(slot)->angles.x in the call would do */   \
        (turns) = &(slot)->angles.x;                                           \
        matrix_push_unit();                                                    \
        njSetMatrix(NULL, &finalhazard_panel_bases[(slot)->base]);             \
        njTranslateEx(&(slot)->pos);                                           \
        njRotateEx((turns), 0);                                                \
        njCalcPoint(NULL, &(local), &(world));                                 \
        njCalcPoint(&finalhazard_joint_inverse, &(world), &(result));          \
    } while (0)

/* turn local by the slot's matrix, and add it to point. then pop the
 * matrix */
#define SLOT_OFFSET(point, local, world)                                       \
    do {                                                                       \
        /* @bug logic: turns local by the view's turn as well as the slot's,   \
         * and adds it to a world point, as panels.c's position_panel does.    \
         * the turns are at 0x8c507896 and 0x8c5079e8 */                       \
        njCalcVector(NULL, &(local), &(world));                                \
        vector_sum(&(point), &(world), &(point));                              \
        njPopMatrixEx();                                                       \
    } while (0)

struct task *finalhazard_body_sparks(struct task *parent)
{
    struct task *task;
    struct emitter_work *work;
    struct panel_slot *slot;
    Angle *angles;
    Sint8 row;
    Sint8 col;
    NJS_POINT3 origin;
    NJS_POINT3 world;
    NJS_POINT3 pos;
    NJS_VECTOR forward;
    NJS_VECTOR offset;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, panel_emitter_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = panel_emitter_destroy;
    work = EMITTER(task);
    PICK_SLOT(row, col, slot);
    SLOT_ORIGIN(slot, emitter_origin, angles, origin, world, pos);
    forward = emitter_offset;
    SLOT_OFFSET(pos, forward, offset);
    work->pos = pos;
    work->kind = EMITTER_BODY;
    work->timer = BODY_SPARK_STEPS;
    return task;
}

void finalhazard_body_explosion(void)
{
    struct panel_slot *slot;
    Angle *angles;
    Sint8 row;
    Sint8 col;
    NJS_POINT3 origin;
    NJS_POINT3 world;
    NJS_POINT3 pos;
    NJS_VECTOR forward;
    NJS_VECTOR offset;
    PICK_SLOT(row, col, slot);
    SLOT_ORIGIN(slot, burst_origin, angles, origin, world, pos);
    forward = burst_offset;
    forward.x +=
        EXPLOSION_SPREAD - (rand() * (2.0F * EXPLOSION_SPREAD * RAND_SCALE));
    forward.z +=
        EXPLOSION_SPREAD - (rand() * (2.0F * EXPLOSION_SPREAD * RAND_SCALE));
    SLOT_OFFSET(pos, forward, offset);
    forward = pos;
    njUnitVector(&forward);
    burst_create_wobbling(&pos, &forward, EXPLOSION_SIZE);
}
