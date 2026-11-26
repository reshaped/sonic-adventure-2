/* the door a flute stand opens, KDDOOR and DOORT in a stage's object list */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "shared/dust.h"
#include "shared/knudai.h"
#include "shared/knudai_door.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/knudai_door/text.inc"

#define KNUDAI_ID(work, mask) (((work)->ang.x & (mask)) % KNUDAI_IDS)

enum knudai_door_mode { MODE_SHUT, MODE_OPENING, MODE_GONE };
/* the placement's state once its door has started to open */
#define STATE_OPENED 1

/* a door's record, in task->motion */
struct knudai_door {
    Float slide; /* how far the door has slid */
};

#define DOOR(task) ((struct knudai_door *)(task)->motion.p)
/* TRUE for a door that knudai_door_flat_init starts, which lies flat and
 * slides along its z */
#define LIES_FLAT(task) ((task)->unk_3c.l)
/* the door's model scales by its scl plus 1 along each axis */
#define SCALE_X(work) ((work)->scl.x + 1.0F)
#define SCALE_Y(work) ((work)->scl.y + 1.0F)
#define SCALE_Z(work) ((work)->scl.z + 1.0F)
/* the door slides SLIDE_STEP a step. it is gone once it has slid
 * SLIDE_LIMIT, its model's height of DOOR_HEIGHT at the door's scale */
#define SLIDE_STEP (1.0F / 12)
#define DOOR_HEIGHT 55.0F
#define SLIDE_LIMIT(work) (SCALE_Y(work) * DOOR_HEIGHT)

#define OPEN_SOUND 0x1016
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 0
#define SOUND_DURATION 30 /* in sound updates */
/* a door not on its side raises a puff of dust at its foot each step it
 * slides. the puff starts DUST_DEPTH along the door's z, at random across
 * DUST_WIDTH */
#define DUST_WIDTH 50.0F
#define DUST_DEPTH (-2.5F)

#define KNUDAI_DOOR_SHAPES 5
#define PANEL_SHAPE 0

/* one sine of a shake. at the shake's step n, it adds amp * njSin(n * turn).
 * the work's timer counts the steps the door has slid */
struct shake_wave {
    Angle turn;
    Float amp;
};
#define SHAKE_WAVES 3

static void knudai_door_destroy(struct task *task);
static void knudai_door_step(struct task *task);
static void knudai_door_draw(struct task *task);

#define TEXTURES 3
static NJS_TEXNAME knudai_door_texnames[TEXTURES] = {
#include "shared/knudai_door/knudai_door_texnames.inc"
};
static NJS_TEXLIST knudai_door_texlist = {
#include "shared/knudai_door/knudai_door_texlist.inc"
};
#define KNUDAI_DOOR_POINTS 24
static Sint16 knudai_door_polygons[197] = {
#include "shared/knudai_door/knudai_door_polygons.inc"
};
static Sint32 knudai_door_vertices[147] = {
#include "shared/knudai_door/knudai_door_vertices.inc"
};
static NJS_CNK_MODEL knudai_door_model = {
#include "shared/knudai_door/knudai_door_model.inc"
};
/* the model's object, which nothing reads */
/* The original door member keeps the draw and collision root records. */
static NJS_CNK_OBJECT knudai_door_object = {
#include "shared/knudai_door/knudai_door_object.inc"
};
/* SHC refuses braces around the material's two colors, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL knudai_door_box_material[1] = {
#include "shared/knudai_door/knudai_door_box_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Sint16 knudai_door_box_meshes[15] = {
#include "shared/knudai_door/knudai_door_box_meshes.inc"
};
static NJS_MESHSET knudai_door_box_meshsets[1] = {
#include "shared/knudai_door/knudai_door_box_meshsets.inc"
};
static NJS_POINT3 knudai_door_box_points[8] = {
#include "shared/knudai_door/knudai_door_box_points.inc"
};
static NJS_VECTOR knudai_door_box_normals[8] = {
#include "shared/knudai_door/knudai_door_box_normals.inc"
};
static NJS_MODEL knudai_door_box_model = {
#include "shared/knudai_door/knudai_door_box_model.inc"
};
/* a basic model of a box the door's size, which nothing reads */
static NJS_OBJECT knudai_door_box_object = {
#include "shared/knudai_door/knudai_door_box_object.inc"
};

NJS_TEXLIST *knudai_door_texlists[4] = {
#include "shared/knudai_door/knudai_door_texlists.inc"
};
/* the door's shake along x, y and z. a door on its side turns its model a
 * quarter turn about z. its draw then shakes it by shake_y along x and by
 * shake_x along y */
static struct shake_wave shake_x[SHAKE_WAVES] = {
    {0x2700, 0.1F},
    {0x0e90, 0.12F},
    {0x0180, 0.1F},
};
static struct shake_wave shake_y[SHAKE_WAVES] = {
    {0x3090, 0.1F},
    {0x0e90, 0.12F},
    {0x00e0, 0.3F},
};
static struct shake_wave shake_z[SHAKE_WAVES] = {
    {0x2f10, 0.03F},
    {0x1310, 0.03F},
    {0x0860, 0.07F},
};
/* the panel, then the frame's foot, its two posts and its head.
 * knudai_door_step moves the panel's center as the door slides */
/* clang-format off */
static struct collision_shape knudai_door_shapes[KNUDAI_DOOR_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 27.5F, 0.0F}, 29.5F, 32.5F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -7.0F, 0.0F}, 32.5F, 5.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {34.5F, 27.5F, 0.0F}, 5.0F, 32.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-34.5F, 27.5F, 0.0F}, 5.0F, 32.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 62.0F, 0.0F}, 32.5F, 7.5F, 10.0F, 0, {0, 0, 0}},
};
/* the upright door's shapes with y and z swapped, for a flat door */
static struct collision_shape knudai_door_flat_shapes[KNUDAI_DOOR_SHAPES] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 27.5F}, 29.5F, 5.0F, 32.5F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, -7.0F}, 32.5F, 10.0F, 5.5F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {34.5F, 0.0F, 27.5F}, 5.0F, 10.0F, 32.5F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-34.5F, 0.0F, 27.5F}, 5.0F, 10.0F, 32.5F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 0.0F, 62.0F}, 32.5F, 10.0F, 7.5F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float dust_scale = 1.0F;
static Float dust_scale_range = 1.0F;
static Float dust_speed = 0.2F;

/* the sum of a shake's count waves at timer */
/* @hack type: timer an int cast to Uint16 where it is read, where a Uint16
 * parameter would do */
static Float knudai_door_shake(struct shake_wave *wave, int count, int timer)
{
    Float sum = 0.0F;

    while (count--) {
        sum += njSin(wave->turn * (Uint16)timer) * wave->amp;
        wave++;
    }
    return sum;
}

/* raise a puff of dust at offset in the door's frame, at random across
 * DUST_WIDTH along x. ang turns the puff's speed about x */
static void knudai_door_dust(struct task *task, NJS_VECTOR *offset, Angle ang)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    pos.x = ((rand() * RAND_SCALE) - 0.5F) * DUST_WIDTH;
    pos.y = 0.0F;
    pos.z = 0.0F;
    spd.x = 0.0F;
    spd.y = 0.0F;
    spd.z = dust_speed * SCALE_Y(work);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (LIES_FLAT(task)) {
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    } else {
        njTranslate(NULL, work->pos.x, work->pos.y + DOOR(task)->slide,
                    work->pos.z);
    }
    njRotateY(NULL, work->ang.y);
    if (LIES_FLAT(task)) {
        njRotateX(NULL, QUARTER_TURN);
        njTranslate(NULL, 0.0F, DOOR(task)->slide, 0.0F);
    }
    njScale(NULL, SCALE_X(work), SCALE_Y(work), SCALE_Z(work));
    njTranslateEx(offset);
    njRotateX(NULL, ang);
    njCalcPoint(NULL, &pos, &pos);
    njCalcVector(NULL, &spd, &spd);
    njPopMatrixEx();
    dust_short_create(&pos, &spd,
                      (dust_scale + (dust_scale_range * rand() * RAND_SCALE)) *
                          SCALE_Y(work));
}

void knudai_door_flat_init(struct task *task)
{
    LIES_FLAT(task) = TRUE;
    knudai_door_init(task);
}

void knudai_door_init(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL && placement_get_state(task)) {
        placement_kill(task);
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct knudai_door));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = knudai_door_draw;
    task->step = knudai_door_step;
    task->destructor = knudai_door_destroy;
    work->submode = MODE_SHUT;
    if (LIES_FLAT(task)) {
        collision_init(task, knudai_door_flat_shapes, KNUDAI_DOOR_SHAPES,
                       COLLISION_OBJECT);
    } else {
        collision_init(task, knudai_door_shapes, KNUDAI_DOOR_SHAPES,
                       COLLISION_OBJECT);
    }
    DOOR(task)->slide = 0.0F;
}

static void knudai_door_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

static void knudai_door_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR offset;
    /* @hack redundant: unused, a local nothing reads, which only sizes the
     * frame */
    NJS_COLOR unused;
    /* @hack spelling: horz, a local for KNUDAI_DOOR_HORZ, where the constant
     * in place would do */
    int horz;
    /* @hack spelling: mask, a local for KNUDAI_ID_MASK, where the constant in
     * place would do */
    int mask;

    /* @hack spelling: the locals set where the test fails, and the return in
     * an else, where a return under the test and the sets after it would
     * do */
    if (!placement_try_leave(task)) {
        mask = KNUDAI_ID_MASK;
        horz = KNUDAI_DOOR_HORZ;
    } else {
        return;
    }
    switch (work->submode) {
    case MODE_SHUT:
        if (knudai_played(KNUDAI_ID(work, mask)) == 1) {
            /* @hack redundant: a test of knudai_get_pos, which returns 1 for
             * every id knudai_played returns 1 for. nothing reads the pos it
             * writes */
            if (knudai_get_pos(KNUDAI_ID(work, mask), &offset)) {
                work->submode = MODE_OPENING;
                if (task->placement != NULL) {
                    placement_set_state(task, STATE_OPENED);
                }
            }
        }
        break;
    case MODE_OPENING:
        work->timer++;
        DOOR(task)->slide += SLIDE_STEP;
        sound_play_at_for(OPEN_SOUND, work, SOUND_PRIORITY, SOUND_VOLUME,
                          SOUND_DURATION, &work->pos);
        /* @hack spelling: else if after the return, where if alone would do */
        if (DOOR(task)->slide > SLIDE_LIMIT(work)) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            work->submode = MODE_GONE;
            return;
        } else if (!(work->ang.x & horz)) {
            offset.x = 0.0F;
            offset.y = 0.0F;
            offset.z = DUST_DEPTH;
            knudai_door_dust(task, &offset, 0);
        }
        break;
    /* @hack redundant: a case for MODE_GONE, which does nothing */
    case MODE_GONE:
        break;
    /* @hack redundant: a default, which does nothing */
    default:
        break;
    }
    if (work->collision != NULL && work->submode != MODE_GONE) {
        if (work->ang.x & horz) {
            if (work->ang.x & KNUDAI_DOOR_HORZ2) {
                work->collision->shapes[PANEL_SHAPE].center.x =
                    knudai_door_shapes[PANEL_SHAPE].center.x -
                    DOOR(task)->slide;
            } else {
                work->collision->shapes[PANEL_SHAPE].center.x =
                    knudai_door_shapes[PANEL_SHAPE].center.x +
                    DOOR(task)->slide;
            }
        } else if (LIES_FLAT(task)) {
            work->collision->shapes[PANEL_SHAPE].center.z =
                knudai_door_flat_shapes[PANEL_SHAPE].center.z +
                DOOR(task)->slide;
        } else {
            work->collision->shapes[PANEL_SHAPE].center.y =
                knudai_door_shapes[PANEL_SHAPE].center.y + DOOR(task)->slide;
        }
        collision_register(task);
    }
}

static void knudai_door_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its eight strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)KNUDAI_DOOR_DEBUG_ID;
        (void)KNUDAI_DOOR_HORIZONTAL_DOUBLE_LABEL;
        (void)KNUDAI_DOOR_HORIZONTAL_LABEL;
        (void)KNUDAI_DOOR_VERTICAL_LABEL;
        (void)KNUDAI_DOOR_DEBUG_YAW;
        (void)KNUDAI_DOOR_DEBUG_SCALE_X;
        (void)KNUDAI_DOOR_DEBUG_SCALE_Y;
        (void)KNUDAI_DOOR_DEBUG_SCALE_Z;
    }
    /* @hack redundant: shake_x, shake_y and shake_z cast to void, ahead of
     * the calls that read them */
    (void)shake_x;
    (void)shake_y;
    (void)shake_z;
    njSetTexture(&knudai_door_texlist);
    njPushMatrixEx();
    if (work->ang.x & KNUDAI_DOOR_HORZ) {
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        if (work->ang.x & KNUDAI_DOOR_HORZ2) {
            njTranslate(NULL, (DOOR_HEIGHT / 2) - DOOR(task)->slide,
                        DOOR_HEIGHT / 2, 0.0F);
            njScale(NULL, SCALE_X(work), SCALE_Y(work), SCALE_Z(work));
            if (work->timer) {
                njTranslate(
                    NULL, knudai_door_shake(shake_y, SHAKE_WAVES, work->timer),
                    knudai_door_shake(shake_x, SHAKE_WAVES, work->timer),
                    knudai_door_shake(shake_z, SHAKE_WAVES, work->timer));
            }
            njRotateZ(NULL, QUARTER_TURN);
        } else {
            njTranslate(NULL, DOOR(task)->slide - (DOOR_HEIGHT / 2),
                        DOOR_HEIGHT / 2, 0.0F);
            njScale(NULL, SCALE_X(work), SCALE_Y(work), SCALE_Z(work));
            if (work->timer) {
                njTranslate(
                    NULL, knudai_door_shake(shake_y, SHAKE_WAVES, work->timer),
                    knudai_door_shake(shake_x, SHAKE_WAVES, work->timer),
                    knudai_door_shake(shake_z, SHAKE_WAVES, work->timer));
            }
            njRotateZ(NULL, -QUARTER_TURN);
        }
        njCnkEasyDrawModel(&knudai_door_model);
    } else {
        if (LIES_FLAT(task)) {
            njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        } else {
            njTranslate(NULL, work->pos.x, work->pos.y + DOOR(task)->slide,
                        work->pos.z);
        }
        njRotateY(NULL, work->ang.y);
        if (LIES_FLAT(task)) {
            njRotateX(NULL, QUARTER_TURN);
            njTranslate(NULL, 0.0F, DOOR(task)->slide, 0.0F);
        }
        njScale(NULL, SCALE_X(work), SCALE_Y(work), SCALE_Z(work));
        if (work->timer) {
            njTranslate(NULL,
                        knudai_door_shake(shake_x, SHAKE_WAVES, work->timer),
                        knudai_door_shake(shake_y, SHAKE_WAVES, work->timer),
                        knudai_door_shake(shake_z, SHAKE_WAVES, work->timer));
        }
        njCnkEasyDrawModel(&knudai_door_model);
    }
    njPopMatrixEx();
}
