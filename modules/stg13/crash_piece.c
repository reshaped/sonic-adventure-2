/* crash pieces, the debris a crash throws. each is a task,
 * ObjectCrashObjExec. a piece falls and spins, bounces off walls, settles on
 * the floor and fades */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "sound/lists.h"
#include "stg13/crash_piece.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define PIECE_LEVEL 3
/* a piece's floor lies FIRST_DROP below it until a probe finds one */
#define FIRST_DROP 10000.0F
/* a piece's collision sphere's radius, as a part of its size's length */
#define RADIUS_PART 0.7F
/* a piece probes where its work's timer plus its delay is a multiple of
 * PROBE_STEPS. PROBE_STEPS must be a power of two. a probe starts PROBE_RISE
 * above the piece, or PROBE_LEAD steps of the piece's speed ahead of that */
#define PROBE_STEPS 32
#define PROBE_RISE 15.0F
#define PROBE_LEAD 10.0F
/* a floor whose normal's length across x and z is FLAT_LEAN or less takes no
 * turn about y */
#define FLAT_LEAN 0.01F
/* a probe's side is a wall where its normal's part along its axis passes
 * WALL_NORMAL toward the probe. a side with GROUND_DEFLECTS is no wall. a
 * piece that passes a wall stands at it. it leaves with WALL_BOUNCE of its
 * speed along that axis */
#define WALL_NORMAL 0.9F
#define WALL_BOUNCE 0.7F
/* a piece on its floor bounces up. each step there, it keeps FLOOR_BOUNCE of
 * its speed and of its spin about x. each step in the air, it keeps AIR_KEEP
 * of each */
#define FLOOR_BOUNCE 0.5F
#define AIR_KEEP 0.995F
/* a piece that lands once it has lived more than LANDED_STEPS steps cuts a
 * longer delay to LANDED_DELAY */
#define LANDED_STEPS 5
#define LANDED_DELAY 22
/* a piece stops where all three hold: its center lies less than REST_HEIGHT
 * above its floor, it spins less than SPIN_REST about each axis, and its speed
 * is below REST_FALL_STEPS steps of crash_gravity */
#define SPIN_REST 128
#define REST_HEIGHT 1.0F
#define REST_FALL_STEPS 3.0F
/* a piece ends once its draw finds it unseen UNSEEN_DRAWS times in a row. a
 * fading piece ends FADE_STEPS steps into its fade */
#define UNSEEN_DRAWS 20
#define FADE_STEPS 15

/* a piece's record, in task->motion */
struct crash_piece {
    NJS_VECTOR spd;
    /* its floor's frame: a point of the floor below it, and the floor's
     * heading and tilt */
    NJS_POINT3 floor_pos;
    struct angles floor_ang;
    struct angles spin; /* what its work's ang gains each step */
    Sint32 steps; /* the steps it has lived */
    struct surface_probe probe;
    Sint32 unseen_draws; /* the draws in a row that found it unseen */
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    NJS_DIRECT_HEAD *built;
    NJS_VECTOR size;
};

#define PIECE(task) ((struct crash_piece *)(task)->motion.p)
/* the steps left before the piece fades, in task->unk_3c. it is 0 once the
 * piece fades, and for a piece that never does */
#define DELAY(task) ((task)->unk_3c.l)

static void crash_piece_step(struct task *task);
static void crash_piece_draw(struct task *task);
static void crash_piece_destroy(struct task *task);

static const struct sound city_escape_sounds[18] = {
#include "stg13/crash_piece/city_escape_sounds.inc"
};
const struct sound_list city_escape_sound_list = {
#include "stg13/crash_piece/city_escape_sound_list.inc"
};

/* what a piece's speed along y loses each step */
static Float crash_gravity = 0.1F;
/* the degrees a step that a piece's random spin about each axis spans,
 * centered on 0 */
static Float crash_spin = 6.0F;
/* clang-format off */
static struct collision_shape crash_shape = {
    0, SHAPE_SPHERE, 0,
    SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |
        SHAPE_HURTS_ENEMIES,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 18.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static NJS_VECTOR crash_up = {0.0F, 1.0F, 0.0F};

struct task *crash_piece_create(NJS_TEXLIST *texlist, NJS_CNK_OBJECT *object,
                                NJS_DIRECT_HEAD *built, NJS_VECTOR *size,
                                NJS_POINT3 *pos, struct angles *ang,
                                NJS_VECTOR *spd, Sint32 delay)
{
    struct task *task;
    struct task_work *work;

    task = task_create(TASK_WORK, PIECE_LEVEL, crash_piece_step,
                       "ObjectCrashObjExec");
    if (task == NULL) {
        return NULL;
    }
    work = task->work;
    task->motion.p = syCalloc(1, sizeof(struct crash_piece));
    if (task->motion.p == NULL) {
        task_kill(task);
        return NULL;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateZXY(NULL, ang->x, ang->y, ang->z);
    njCalcPoint(NULL, &object->model->center, &work->pos);
    njPopMatrixEx();
    work->ang = *ang;
    PIECE(task)->spin.x =
        (((rand() * RAND_SCALE) - 0.5F) * crash_spin) * ANGLE_PER_DEGREE;
    PIECE(task)->spin.y =
        (((rand() * RAND_SCALE) - 0.5F) * crash_spin) * ANGLE_PER_DEGREE;
    PIECE(task)->spin.z =
        (((rand() * RAND_SCALE) - 0.5F) * crash_spin) * ANGLE_PER_DEGREE;
    PIECE(task)->spd = *spd;
    PIECE(task)->floor_pos = work->pos;
    PIECE(task)->floor_pos.y -= FIRST_DROP;
    PIECE(task)->texlist = texlist;
    PIECE(task)->built = built;
    PIECE(task)->object = object;
    PIECE(task)->size = *size;
    task->draw = crash_piece_draw;
    task->destructor = crash_piece_destroy;
    collision_init(task, &crash_shape, 1, COLLISION_OBJECT);
    work->collision->shapes->radius = njScalor(size) * RADIUS_PART;
    collision_fit_radius(task);
    DELAY(task) = delay;
    return task;
}

/* probe the surfaces about the piece. a floor the probe finds becomes the
 * piece's floor */
static void crash_piece_find_floor(struct task *task)
{
    struct task_work *work = task->work;
    struct surface_side *floor;
    NJS_VECTOR normal;
    Float len;

#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it tests the floor the piece's
     * last probe found. SHC's code tests the frame word the tag names, which C
     * has no name for */
    floor = &PIECE(task)->probe.sides[SURFACE_NEG_Y];
#endif
    /* @bug uninitialized: tests floor->found before floor is set. SHC's code
     * reads floor from the frame word that later holds normal.x, which no
     * path has written yet (MOV.L @R15,R3 at 0x8c50cb32). so whether the
     * probe starts ahead of the piece rests on a stale word */
    if (floor->found) { /* NOLINT(clang-diagnostic-uninitialized) */
        PIECE(task)->probe.pos.x =
            work->pos.x + (PIECE(task)->spd.x * PROBE_LEAD);
        PIECE(task)->probe.pos.z =
            work->pos.z + (PIECE(task)->spd.z * PROBE_LEAD);
        PIECE(task)->probe.pos.y =
            work->pos.y + (PIECE(task)->spd.y * PROBE_LEAD) + PROBE_RISE;
    } else {
        PIECE(task)->probe.pos = work->pos;
        PIECE(task)->probe.pos.y += PROBE_RISE;
    }
    surface_probe_sides(&PIECE(task)->probe);
    floor = &PIECE(task)->probe.sides[SURFACE_NEG_Y];
    if (floor->found) {
        PIECE(task)->floor_pos.x = work->pos.x;
        PIECE(task)->floor_pos.z = work->pos.z;
        PIECE(task)->floor_pos.y = floor->pos;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, floor->ang_z);
        njRotateX(NULL, floor->ang_x);
        njCalcVector(NULL, &crash_up, &normal);
        len = njSqrt((normal.x * normal.x) + (normal.z * normal.z));
        if (len > FLAT_LEAN) {
            PIECE(task)->floor_ang.y =
                atan2f(normal.x, normal.z) * ANGLE_PER_RADIAN;
        } else {
            PIECE(task)->floor_ang.y = 0;
        }
        /* @hack spelling: the length computed again, where len would do */
        PIECE(task)->floor_ang.x =
            atan2f(njSqrt((normal.x * normal.x) + (normal.z * normal.z)),
                   normal.y) *
            ANGLE_PER_RADIAN;
        PIECE(task)->floor_ang.z = 0;
        njPopMatrixEx();
    }
}

/* @hack spelling: two inline functions that walk a vector as Floats, where
 * its members written out at each use would do */
#pragma inline(vector_scale, vector_add)

static void vector_scale(Float scale, Float *vec)
{
    *vec++ *= scale;
    *vec++ *= scale;
    *vec *= scale;
}

static void vector_add(Float *vec, Float *add)
{
    *vec++ += *add++;
    *vec++ += *add++;
    *vec = *vec + *add;
}

static void crash_piece_step(struct task *task)
{
    struct task_work *work;
    struct surface_side *wall;
    Sint16 *spin;
    Float height;
    Float part;
    NJS_VECTOR spd;
    NJS_VECTOR size;
    NJS_POINT3 pos;

    work = task->work;
    if (DELAY(task)) {
        DELAY(task)--;
        if (!DELAY(task) && !work->timer) {
            work->timer++;
        }
    }
    if (!((work->timer + DELAY(task)) & (PROBE_STEPS - 1))) {
        crash_piece_find_floor(task);
    }
    PIECE(task)->spd.y -= crash_gravity;
    vector_add(&work->pos.x, &PIECE(task)->spd.x);

    wall = &PIECE(task)->probe.sides[SURFACE_NEG_X];
    if (work->pos.x < wall->pos && wall->found &&
        wall->normal.x > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        PIECE(task)->spd.x = njAbs(PIECE(task)->spd.x) * WALL_BOUNCE;
    }
    wall = &PIECE(task)->probe.sides[SURFACE_POS_X];
    if (work->pos.x > wall->pos && wall->found &&
        wall->normal.x < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.x = wall->pos;
        PIECE(task)->spd.x = -(njAbs(PIECE(task)->spd.x) * WALL_BOUNCE);
    }
    wall = &PIECE(task)->probe.sides[SURFACE_NEG_Z];
    if (work->pos.z < wall->pos && wall->found &&
        wall->normal.z > WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        PIECE(task)->spd.z = njAbs(PIECE(task)->spd.z) * WALL_BOUNCE;
    }
    wall = &PIECE(task)->probe.sides[SURFACE_POS_Z];
    if (work->pos.z > wall->pos && wall->found &&
        wall->normal.z < -WALL_NORMAL && !(wall->flags & GROUND_DEFLECTS)) {
        work->pos.z = wall->pos;
        PIECE(task)->spd.z = -(njAbs(PIECE(task)->spd.z) * WALL_BOUNCE);
    }

    work->ang.x += PIECE(task)->spin.x;
    work->ang.y += PIECE(task)->spin.y;
    work->ang.z += PIECE(task)->spin.z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateX(NULL, -PIECE(task)->floor_ang.x);
    njRotateY(NULL, -PIECE(task)->floor_ang.y);
    njTranslate(NULL, -PIECE(task)->floor_pos.x, -PIECE(task)->floor_pos.y,
                -PIECE(task)->floor_pos.z);
    njCalcPoint(NULL, &work->pos, &pos);
    njCalcVector(NULL, &PIECE(task)->spd, &spd);
    njRotateZXY(NULL, work->ang.x, work->ang.y, work->ang.z);
    njCalcVector(NULL, &PIECE(task)->size, &size);
    njPopMatrixEx();

    height = njAbs(size.y);
    if (pos.y < height) {
        if (!work->timer && PIECE(task)->steps > LANDED_STEPS &&
            DELAY(task) > LANDED_DELAY) {
            DELAY(task) = LANDED_DELAY;
        }
        pos.y = height;
        spd.y = njAbs(spd.y) * FLOOR_BOUNCE;
        spd.x *= FLOOR_BOUNCE;
        spd.z *= FLOOR_BOUNCE;
        /* @hack type: the spin about x read and written as a Sint16, here
         * and in the else, where the Angle would do while it lies within a
         * Sint16 */
        spin = (Sint16 *)&PIECE(task)->spin.x;
        part = *spin;
        *spin = part * FLOOR_BOUNCE;
    } else {
        spin = (Sint16 *)&PIECE(task)->spin.x;
        part = *spin;
        *spin = part * AIR_KEEP;
        vector_scale(AIR_KEEP, &spd.x);
    }
    /* @hack type: each spin cut to a Sint16 for the tests, where the Angle
     * would do while it lies within a Sint16 */
    if (pos.y < REST_HEIGHT && (Sint16)PIECE(task)->spin.x < SPIN_REST &&
        (Sint16)PIECE(task)->spin.x > -SPIN_REST &&
        (Sint16)PIECE(task)->spin.y < SPIN_REST &&
        (Sint16)PIECE(task)->spin.y > -SPIN_REST &&
        (Sint16)PIECE(task)->spin.z < SPIN_REST &&
        (Sint16)PIECE(task)->spin.z > -SPIN_REST &&
        njScalor(&spd) < crash_gravity * REST_FALL_STEPS) {
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&PIECE(task)->floor_pos);
    njRotateY(NULL, PIECE(task)->floor_ang.y);
    njRotateX(NULL, PIECE(task)->floor_ang.x);
    njCalcPoint(NULL, &pos, &work->pos);
    njCalcVector(NULL, &spd, &PIECE(task)->spd);
    njPopMatrixEx();

    if (PIECE(task)->unseen_draws < UNSEEN_DRAWS) {
        if (work->timer) {
            work->timer++;
            if (work->timer > FADE_STEPS) {
                if (task->placement != NULL) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
            } else if (work->collision != NULL) {
                collision_register(task);
            }
        } else if (work->collision != NULL) {
            collision_register(task);
        }
        PIECE(task)->steps++;
    } else if (task->placement != NULL) {
        placement_kill(task);
    } else {
        task_kill(task);
    }
}

/* a draw finds a piece unseen where its center lies more than DRAW_FAR ahead
 * of the camera. it finds it unseen too where a ball about the center lies
 * wholly behind the camera. the ball's radius is the model's plus
 * DRAW_MARGIN. a fading piece less than DRAW_NEAR ahead is unseen too */
#define DRAW_FAR 600.0F
#define DRAW_NEAR 10.0F
#define DRAW_MARGIN 30.0F
/* a fading piece's alpha falls from 1 by FADE_FALL a step, FADE_LOSS over
 * FADE_STEPS steps. the piece draws nothing at FADE_MIN or less */
#define FADE_LOSS 0.8F
#define FADE_FALL (FADE_LOSS * (1.0F / FADE_STEPS))
#define FADE_MIN 0.01F
/* a fading piece's tint takes its alpha as a byte for its alpha, green and
 * blue. its red is half that byte plus RED_BASE */
#define BYTE_MAX 255
#define RED_BASE 128
#define ARGB(a, r, g, b) (((Uint32)(a) << 24) + ((r) << 16) + ((g) << 8) + (b))

static void crash_piece_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = PIECE(task)->object;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[12];
    Float depth;
    Float alpha;
    /* @hack spelling: color holds the alpha as a byte, then the color packed
     * from it, where a local for each would do */
    int color;
    int red;
    NJS_CNK_MODEL *model;

    if (!sphere_in_front(&work->pos, object->model->r + DRAW_MARGIN, &depth) ||
        depth > DRAW_FAR || (depth < DRAW_NEAR && work->timer)) {
        PIECE(task)->unseen_draws++;
        /* @hack spelling: a goto to a return at the end, where a return
         * would do */
        goto done;
    }
    PIECE(task)->unseen_draws = 0;
    if (work->timer) {
        if (work->timer < FADE_STEPS) {
            alpha = 1.0F - (work->timer * FADE_FALL);
        } else {
            alpha = 0.0F;
        }
        if (alpha <= FADE_MIN) {
            return;
        }
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
        color = (int)(alpha * (Float)BYTE_MAX);
        if (color > BYTE_MAX) {
            color = BYTE_MAX;
        }
        if (color < 0) {
            color = 0;
        }
        red = (color >> 1) + RED_BASE;
        if (red > BYTE_MAX) {
            red = BYTE_MAX;
        }
        if (red < 0) {
            red = 0;
        }
        color = ARGB(color, red, color, color);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateEx(&work->ang.x, ZXY_TURN);
        njSetTexture(PIECE(task)->texlist);
        model = object->model;
        njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
        model_draw_tinted(object->model, color);
        njPopMatrixEx();
        draw_attr_restore();
        draw_control_restore();
    } else {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateEx(&work->ang.x, ZXY_TURN);
        njSetTexture(PIECE(task)->texlist);
        model = object->model;
        njTranslate(NULL, -model->center.x, -model->center.y, -model->center.z);
        if (PIECE(task)->built != NULL) {
            njDirectDrawObject(PIECE(task)->built);
        } else {
            njCnkEasyDrawModel(object->model);
        }
        njPopMatrixEx();
    }
done:
    return;
}

static void crash_piece_destroy(struct task *task)
{
    syFree(task->motion.p);
    task->motion.p = NULL;
    task->extra.p = NULL;
}
