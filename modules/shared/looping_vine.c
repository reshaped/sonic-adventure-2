/* the looping vine, ROLTUTAT in a stage's object list: a vine that hangs from
 * a tree. a player who catches its tip swings with it to the trunk. the vine
 * then loops round the trunk and throws its player */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/looping_vine.h"
#include "shared/revolving_vine.h"
#include "shared/vine.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

enum looping_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct looping_vine {
    struct vine_chain chain;
    /* how far the bend parts have curled round the trunk, up to WRAP_END */
    Angle wrap;
    /* the vine's turn about its top, from SWING_START to SWING_END */
    Angle swing;
    Float unk_34;
    Float unk_38;
    /* the speed of the swing and the wrap, in units of length a step */
    Float speed;
    Uint8 unk_40[0x10];
    Sint32 wobble_steps; /* the steps the vine has moved, for its wobble */
};

/* a vine's scale is its scl.y + 1. a vine of scale s hangs LENGTH_SCALED * s
 * + LENGTH_BASE from its top to its work's pos. the swing takes that as its
 * radius */
#define LENGTH_SCALED 285.0F
#define LENGTH_BASE 210.0F
#define SCALE(work) ((work)->scl.y + 1.0F)
#define LENGTH(work) ((SCALE(work) * LENGTH_SCALED) + LENGTH_BASE)
/* the swing runs from minus the low 16 bits of ang.x on to the trunk, scl.x
 * degrees further */
#define SWING_START(work) (-(Uint16)(work)->ang.x)
#define SWING_END(work) (SWING_START(work) + DEGREES((work)->scl.x))
/* the wrap's radius is WRAP_RADIUS less WRAP_SHRINK for each Angle unit it
 * has curled. the bend parts curl by up to WRAP_MOST. the wrap stops at
 * WRAP_END, where the vine throws its player */
#define WRAP_RADIUS 210.0F
#define WRAP_SHRINK 0.0014381289F
#define WRAP_MOST (FULL_TURN + HALF_TURN)
#define WRAP_END(work) (-(work)->ang.z + WRAP_MOST)
/* a speed over TWO_PI_ROUNDED times a radius is the share of a turn it moves
 * along that circle */
#define TWO_PI_ROUNDED 6.28F
/* the swing and the wrap move by their speed times 1 + SPEED_GAIN *
 * sin(-wrap - swing) */
#define SPEED_GAIN 0.7F
/* the end parts wobble about y by up to the vine's speed in degrees. the
 * wobble's sine gains WOBBLE_RATE degrees a step. about z the parts lag the
 * vine's motion, and tilt by up to WOBBLE_TILT degrees as it loops */
#define WOBBLE_RATE 3
#define WOBBLE_TILT 5.0F
/* a vine with no player settles while its wrap is past QUARTER_TURN. its speed
 * then keeps SETTLE_DRAG of itself each step. it also falls by SETTLE_PULL for
 * each Angle unit of the wrap and the swing past FULL_TURN */
#define SETTLE_DRAG 0.98F
#define SETTLE_PULL (0.5F / ANGLE_MASK)
/* the vine moves and draws only while a player lies within NEAR_RANGE of
 * its work's pos */
#define NEAR_RANGE 4000.0F
/* a player catches the vine where its first shape's center comes within
 * CATCH_RANGE of the tip. the catch waits until the vine's timer has counted
 * CATCH_WAIT steps */
#define CATCH_RANGE 10.0F
#define CATCH_WAIT 30
/* the vine's sound, of the stage's sound list */
#define VINE_SOUND 0
/* the animation a hanging player plays */
#define ANIM_HANG 168
/* vibration_start's power, frequency and time for a catch, and its power
 * and frequency for a throw */
#define CATCH_POWER 2
#define CATCH_FREQ 15
#define CATCH_TIME 2
#define THROW_POWER 4
#define THROW_FREQ 15

/* the parts of the vine's chain, from its top. the swing tilts the
 * FRAME_PARTS, and the wrap curls the BEND_PARTS. the END_PARTS trail the
 * vine's motion */
#define FRAME_PARTS 4
#define BEND_PARTS 9
#define END_PARTS 4
/* each frame part tilts about z by the swing / SWING_SHARES */
#define SWING_SHARES 3

/* the debug preview's steps through the swing and through the wrap. it marks
 * the trunk at the center of a box. the box bounds the tips of the chain's
 * PREVIEW_FIRST_PART-th to PREVIEW_LAST_PART-th parts */
#define PREVIEW_STEPS 16
#define PREVIEW_FIRST_PART 4
#define PREVIEW_LAST_PART 12

#include "shared/looping_vine/text.inc"

/* The twining and looping vines pose this joint chain. Their debug preview
 * uses the separate tutates model at the end of the bank. */

static NJS_TEXNAME vine_textures[1] = {
#include "shared/looping_vine/vine_textures.inc"
};

NJS_TEXLIST vine_texlist = {
#include "shared/looping_vine/vine_texlist.inc"
};

static NJS_CNK_OBJECT vine_object_node_16 = {
#include "shared/looping_vine/vine_object_node_16.inc"
};

static Sint16 vine_object_node_15_polygons[2] = {
#include "shared/looping_vine/vine_object_node_15_polygons.inc"
};

static Sint32 vine_object_node_15_vertices[111] = {
#include "shared/looping_vine/vine_object_node_15_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_15_model = {
#include "shared/looping_vine/vine_object_node_15_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_15 = {
#include "shared/looping_vine/vine_object_node_15.inc"
};

static Sint32 vine_object_node_14_vertices[96] = {
#include "shared/looping_vine/vine_object_node_14_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_14_model = {
#include "shared/looping_vine/vine_object_node_14_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_14 = {
#include "shared/looping_vine/vine_object_node_14.inc"
};

static Sint32 vine_object_node_13_vertices[47] = {
#include "shared/looping_vine/vine_object_node_13_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_13_model = {
#include "shared/looping_vine/vine_object_node_13_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_13 = {
#include "shared/looping_vine/vine_object_node_13.inc"
};

static Sint32 vine_object_node_12_vertices[98] = {
#include "shared/looping_vine/vine_object_node_12_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_12_model = {
#include "shared/looping_vine/vine_object_node_12_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_12 = {
#include "shared/looping_vine/vine_object_node_12.inc"
};

static Sint32 vine_object_node_11_vertices[21] = {
#include "shared/looping_vine/vine_object_node_11_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_11_model = {
#include "shared/looping_vine/vine_object_node_11_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_11 = {
#include "shared/looping_vine/vine_object_node_11.inc"
};

static Sint32 vine_object_node_10_vertices[21] = {
#include "shared/looping_vine/vine_object_node_10_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_10_model = {
#include "shared/looping_vine/vine_object_node_10_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_10 = {
#include "shared/looping_vine/vine_object_node_10.inc"
};

static Sint32 vine_object_node_9_vertices[21] = {
#include "shared/looping_vine/vine_object_node_9_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_9_model = {
#include "shared/looping_vine/vine_object_node_9_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_9 = {
#include "shared/looping_vine/vine_object_node_9.inc"
};

static Sint32 vine_object_node_8_vertices[21] = {
#include "shared/looping_vine/vine_object_node_8_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_8_model = {
#include "shared/looping_vine/vine_object_node_8_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_8 = {
#include "shared/looping_vine/vine_object_node_8.inc"
};

static Sint32 vine_object_node_7_vertices[21] = {
#include "shared/looping_vine/vine_object_node_7_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_7_model = {
#include "shared/looping_vine/vine_object_node_7_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_7 = {
#include "shared/looping_vine/vine_object_node_7.inc"
};

static Sint32 vine_object_node_6_vertices[21] = {
#include "shared/looping_vine/vine_object_node_6_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_6_model = {
#include "shared/looping_vine/vine_object_node_6_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_6 = {
#include "shared/looping_vine/vine_object_node_6.inc"
};

static Sint32 vine_object_node_5_vertices[21] = {
#include "shared/looping_vine/vine_object_node_5_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_5_model = {
#include "shared/looping_vine/vine_object_node_5_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_5 = {
#include "shared/looping_vine/vine_object_node_5.inc"
};

static Sint32 vine_object_node_4_vertices[21] = {
#include "shared/looping_vine/vine_object_node_4_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_4_model = {
#include "shared/looping_vine/vine_object_node_4_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_4 = {
#include "shared/looping_vine/vine_object_node_4.inc"
};

static Sint32 vine_object_node_3_vertices[21] = {
#include "shared/looping_vine/vine_object_node_3_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_3_model = {
#include "shared/looping_vine/vine_object_node_3_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_3 = {
#include "shared/looping_vine/vine_object_node_3.inc"
};

static Sint32 vine_object_node_2_vertices[39] = {
#include "shared/looping_vine/vine_object_node_2_vertices.inc"
};

static NJS_CNK_MODEL vine_object_node_2_model = {
#include "shared/looping_vine/vine_object_node_2_model.inc"
};

static NJS_CNK_OBJECT vine_object_node_2 = {
#include "shared/looping_vine/vine_object_node_2.inc"
};

static NJS_CNK_OBJECT vine_object_node_1 = {
#include "shared/looping_vine/vine_object_node_1.inc"
};

static Sint16 vine_polygons[423] = {
#include "shared/looping_vine/vine_polygons.inc"
};

static NJS_CNK_MODEL vine_model = {
#include "shared/looping_vine/vine_model.inc"
};

NJS_CNK_OBJECT vine_object = {
#include "shared/looping_vine/vine_object.inc"
};

static NJS_TEXNAME vine_preview_textures[1] = {
#include "shared/looping_vine/vine_preview_textures.inc"
};

NJS_TEXLIST vine_preview_texlist = {
#include "shared/looping_vine/vine_preview_texlist.inc"
};

static Sint16 vine_preview_polygons[125] = {
#include "shared/looping_vine/vine_preview_polygons.inc"
};

static Sint32 vine_preview_vertices[87] = {
#include "shared/looping_vine/vine_preview_vertices.inc"
};

static NJS_CNK_MODEL vine_preview_model = {
#include "shared/looping_vine/vine_preview_model.inc"
};

NJS_CNK_OBJECT vine_preview_object = {
#include "shared/looping_vine/vine_preview_object.inc"
};

/* a catch gives the vine the player's speed times throw_scale, held to
 * throw_min to throw_max */
static Float throw_max = 30.0F;
static Float throw_min = 3.0F;
static Float throw_scale = 2.5F;
/* a hanging player lies at hang_offset from the tip, in the tip's axes.
 * player_turn turns it further about x, y and z */
static NJS_POINT3 hang_offset = {-2.0F, -7.0F, 0.0F};
static Angle player_turn[3] = {0, 0, 0};

static void looping_vine_start(struct task *task);
static void looping_vine_run(struct task *task);
static void looping_vine_draw(struct task *task);
static void looping_vine_destroy(struct task *task);

static Float looping_vine_distance_squared(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = from->x - to->x;
    Float dy = from->y - to->y;
    Float dz = from->z - to->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* the first player, 0 or 1, whose first shape's center lies nearer pos than
 * range. -1 for neither. range is a squared distance. a player with a work
 * must have a collision record and a shape, unchecked */
static int looping_vine_player_within(NJS_POINT3 *pos, Float range)
{
    if (player_work[0] != NULL &&
        looping_vine_distance_squared(
            &player_work[0]->collision->shapes->center, pos) < range) {
        return 0;
    }
    if (player_work[1] != NULL &&
        looping_vine_distance_squared(
            &player_work[1]->collision->shapes->center, pos) < range) {
        return 1;
    }
    return -1;
}

/* pose the chain from object, a copy of vine_object's. swing tilts the frame
 * parts, whose offsets along x scale stretches. wrap curls the bend parts,
 * each by up to wrap_most / BEND_PARTS from the top down. turn_y and turn_z
 * turn the end parts about y and z. the last part, which has no child, turns
 * by half */
static void looping_vine_pose(NJS_CNK_OBJECT *object, Angle turn_y,
                              Angle turn_z, Angle swing, Float scale,
                              Angle wrap, Angle wrap_most)
{
    NJS_CNK_OBJECT *source = &vine_object;
    int i;
    Angle tilt = swing / SWING_SHARES;
    int full;
    Angle step;
    Angle rest;

    for (i = 0; i < FRAME_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->pos[0] = source->pos[0] * scale;
        object->ang[1] = 0;
        object->ang[2] = tilt;
    }
    step = wrap_most / BEND_PARTS;
    full = wrap / step;
    rest = wrap - (full * step);
    for (i = 0; i < BEND_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->ang[1] = 0;
        /* @hack spelling: one conditional inside another, where an if for
         * each case would do */
        /* NOLINTNEXTLINE(readability-avoid-nested-conditional-operator) */
        object->ang[2] = i < full ? step : i == full ? rest : 0;
    }
    /* @hack redundant: a loop of no passes */
    for (i = 0; i < 0; i++, object = object->child, source = source->child) {
    }
    for (i = 0; i < END_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->ang[1] = object->child ? turn_y : turn_y / 2;
        object->ang[2] = object->child ? turn_z : turn_z / 2;
    }
}

/* set point to the center of the model of the chain's count-th part from
 * object. the chain hangs from the vine's top. *ang_z is the turn of the part's
 * x axis from y toward x. *ang_x is its rise toward z. count must be 1 or more,
 * unchecked */
static void looping_vine_locate_part(struct task *task, NJS_CNK_OBJECT *object,
                                     int count, NJS_POINT3 *point, Angle *ang_x,
                                     Angle *ang_z)
{
    struct task_work *work = task->work;
    struct looping_vine *vine = task->extra.p;
    NJS_CNK_MODEL *model;
    NJS_VECTOR axis;
    int i;

    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &vine->chain.pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, -QUARTER_TURN);
    /* @hack spelling: model cleared in the loop's start, where a statement
     * of its own before the loop would do */
    for (i = 0, model = NULL; i < count; i++) {
        njTranslateV(NULL, (NJS_VECTOR *)object->pos);
        njScale(NULL, object->scl[0], object->scl[1], object->scl[2]);
        njRotateZ(NULL, object->ang[2]);
        njRotateY(NULL, object->ang[1]);
        njRotateX(NULL, object->ang[0]);
        model = object->model;
        object = object->child;
    }
    njCalcPoint(NULL, &model->center, point);
    axis.x = 1.0F;
    axis.y = 0.0F;
    axis.z = 0.0F;
    njCalcVector(NULL, &axis, &axis);
    *ang_z = (Angle)(atan2f(axis.x, axis.y) * ANGLE_PER_RADIAN);
    *ang_x = (Angle)(atan2f(axis.z, njSqrt(1.0F - (axis.z * axis.z))) *
                     ANGLE_PER_RADIAN);
    njPopMatrix(1);
}

void looping_vine_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        task->destructor = looping_vine_destroy;
        task->draw = looping_vine_draw;
        looping_vine_start(task);
        break;
    case MODE_RUN:
        looping_vine_run(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
    if (task->placement != NULL) {
        placement_set_state(task, 1);
    }
}

static void looping_vine_start(struct task *task)
{
    struct task_work *work = task->work;
    struct looping_vine *vine;
    NJS_CNK_OBJECT *object;
    int count;
    int i;

    vine = syCalloc(1, sizeof(struct looping_vine));
    task->extra.p = vine;
    count = 0;
    for (object = &vine_object; object != NULL; object = object->child) {
        count++;
    }
    vine->chain.objects = syCalloc(count, sizeof(NJS_CNK_OBJECT));
    vine->chain.count = count;
    for (object = &vine_object, i = 0; object != NULL;
         object = object->child, i++) {
        vine->chain.objects[i] = *object;
        if (i != 0) {
            vine->chain.objects[i - 1].child = &vine->chain.objects[i];
        }
    }
    work->mode = MODE_RUN;
    vine->chain.timer = CATCH_WAIT;
    /* @hack spelling: one assignment to the two, where a statement for each
     * would do */
    vine->swing = vine->wrap = 0;
    vine->unk_34 = 0.0F;
    vine->unk_38 = 0.0F;
    vine->speed = 0.0F;
    vine->chain.pos = work->pos;
    vine->chain.pos.y += LENGTH(work);
    vine->swing = SWING_START(work);
    work->timer = 0;
}

static void looping_vine_run(struct task *task)
{
    struct task_work *work = task->work;
    struct looping_vine *vine = task->extra.p;
    Angle *offset;
    struct task *player;
    NJS_VECTOR dir;
    Angle old;
    int i;

    /* @hack redundant: a block no path reaches, which calls njSin and
     * stores 0 in time_stopped through dat_8c0db20c */
    if (0) {
        njSin(0);
        STAGE_TIME_STOPPED = 0;
    }
    /* @hack redundant: a block no path reaches, which makes a request of
     * player 0 and stores 1 in dir.x */
    if (0) {
        player_request(0, 0, 0x18000);
        dir.x = 1.0F;
    }
    /* @hack redundant: a block no path reaches, which stores CATCH_WAIT in
     * the timer */
    if (0) {
        vine->chain.timer = CATCH_WAIT;
    }
    /* @hack redundant: a debug preview no path reaches, whose loops SHC keeps.
     * it steps the vine through its swing and its wrap, and draws a line along
     * the tip's path. it then marks the trunk, and prints the work's fields
     * that set the vine */
    if (0) {
        NJS_POINT3 prev;
        Float max_x;
        Float min_x;
        Float max_y;
        Float min_y;
        Float max_z;
        Float min_z;
        Angle ang_x;
        Angle ang_z;
        int j;
        Angle step;

        step = (Angle)(njSin(vine->swing) * 210.0F);
        for (j = 0; j < PREVIEW_STEPS; j++) {
            looping_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                              SCALE(work), 0, WRAP_MOST);
            prev = vine->chain.tip;
            looping_vine_locate_part(task, vine->chain.objects,
                                     vine->chain.count - 1, &vine->chain.tip,
                                     &vine->chain.tip_ang_x,
                                     &vine->chain.tip_ang_z);
            dat_8c0db20c.point = vine->chain.tip;
            vine->swing += step;
            if (j > 0) {
                line_draw(prev.x, prev.y, prev.z, dat_8c0db20c.point.x,
                          dat_8c0db20c.point.y, dat_8c0db20c.point.z);
            }
            looping_vine_draw(task);
        }
        step = (WRAP_MOST - (Uint16)work->ang.z) / PREVIEW_STEPS;
        for (j = 0; j < PREVIEW_STEPS; j++) {
            looping_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                              SCALE(work), step * j, WRAP_MOST);
            prev = vine->chain.tip;
            looping_vine_locate_part(task, vine->chain.objects,
                                     vine->chain.count - 1, &vine->chain.tip,
                                     &vine->chain.tip_ang_x,
                                     &vine->chain.tip_ang_z);
            dat_8c0db20c.point = vine->chain.tip;
            if (j > 0) {
                line_draw(prev.x, prev.y, prev.z, dat_8c0db20c.point.x,
                          dat_8c0db20c.point.y, dat_8c0db20c.point.z);
            }
            looping_vine_draw(task);
        }
        for (j = PREVIEW_FIRST_PART; j <= PREVIEW_LAST_PART; j++) {
            looping_vine_locate_part(task, vine->chain.objects, j, &prev,
                                     &ang_x, &ang_z);
            if (j == PREVIEW_FIRST_PART) {
                max_x = prev.x;
                min_x = prev.x;
                max_y = prev.y;
                min_y = prev.y;
                min_z = prev.z;
                max_z = prev.z;
            } else {
                if (max_x < prev.x) {
                    max_x = prev.x;
                }
                if (min_x > prev.x) {
                    min_x = prev.x;
                }
                if (max_y < prev.y) {
                    max_y = prev.y;
                }
                if (min_y > prev.y) {
                    min_y = prev.y;
                }
                if (max_z < prev.z) {
                    max_z = prev.z;
                }
                if (min_z > prev.z) {
                    min_z = prev.z;
                }
            }
        }
        prev.x = (max_x + min_x) * 0.5F;
        prev.y = (max_y + min_y) * 0.5F;
        prev.z = (max_z + min_z) * 0.5F;
        njSetTexture(&vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &prev);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawObject(&vine_preview_object);
        njPopMatrix(1);
        {
            Float value = prev.x;
            Int loc = NJM_LOCATION(8, 23);
            char buf[20];

            sprintf(buf, LOOPING_VINE_PADDED_FLOAT, value);
            njPrintC(loc, buf);
        }
        {
            Float value = prev.y;
            Int loc = NJM_LOCATION(8, 24);
            char buf[20];

            sprintf(buf, LOOPING_VINE_PADDED_FLOAT, value);
            njPrintC(loc, buf);
        }
        {
            Float value = prev.z;
            Int loc = NJM_LOCATION(8, 25);
            char buf[20];

            sprintf(buf, LOOPING_VINE_PADDED_FLOAT, value);
            njPrintC(loc, buf);
        }
        njPrintC(NJM_LOCATION(17, 23), LOOPING_VINE_EDITOR_WOOD_POS_X);
        njPrintC(NJM_LOCATION(17, 24), LOOPING_VINE_EDITOR_WOOD_POS_Y);
        njPrintC(NJM_LOCATION(17, 25), LOOPING_VINE_EDITOR_WOOD_POS_Z);
        njPrintC(NJM_LOCATION(17, 16),
                 LOOPING_VINE_EDITOR_ANGLE_START_POSITION);
        njPrintC(NJM_LOCATION(17, 17), LOOPING_VINE_EDITOR_ANGLE_ROTY);
        njPrintC(NJM_LOCATION(17, 18),
                 LOOPING_VINE_EDITOR_ANGLE_LAUNCH_POSITION);
        njPrintC(NJM_LOCATION(17, 19),
                 LOOPING_VINE_EDITOR_ANGLE_START_TO_WOOD_WIDTH);
        njPrintC(NJM_LOCATION(17, 20), LOOPING_VINE_EDITOR_TUTA_LENGTH);
        njPrintC(NJM_LOCATION(17, 21), LOOPING_VINE_EDITOR_ADD_SPEED);
        return;
    } else if (task->placement && !placement_get_state(task)) {
        task_kill(task);
        return;
    }
    if (!player_near(&work->pos, NEAR_RANGE)) {
        return;
    }
    /* @hack type: time_stopped read through dat_8c0db20c, where
     * game_state.time_stopped would do */
    if (!STAGE_TIME_STOPPED) {
        VINE_HELD(task) = 0;
    }
    if (VINE_HELD(task) || !game_state.time_stopped) {
        if (vine->swing >= SWING_END(work)) {
            old = vine->wrap;
            vine->swing = SWING_END(work);
            vine->wrap += (Angle)(vine->speed /
                                  ((-_builtin_fabsf(vine->wrap * WRAP_SHRINK) +
                                    WRAP_RADIUS) *
                                   TWO_PI_ROUNDED) *
                                  (1.0F + (SPEED_GAIN *
                                           njSin(-vine->wrap - vine->swing))) *
                                  FULL_TURN);
            if (vine->chain.timer < 0 && njCos(old) < 0.0F &&
                !(njCos(vine->wrap) < 0.0F)) {
                sound_play(VINE_SOUND, NULL, 0, 0);
            }
            if (vine->wrap > WRAP_END(work)) {
                vine->wrap = WRAP_END(work);
                if (vine->chain.timer < 0) {
                    dir.x = 0.0F;
                    dir.y = -1.0F;
                    dir.z = 0.0F;
                    njPushMatrix(NULL);
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y);
                    njRotateZ(NULL, vine->swing + vine->wrap);
                    njCalcVector(NULL, &dir, &dir);
                    njPopMatrix(1);
                    player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
                    vibration_start(work->tick, THROW_POWER, THROW_FREQ,
                                    VIBRATION_UNTIMED);
                    VINE_DONE(task) = TRUE;
                    player_fling(work->tick, vine->speed + work->scl.z, &dir);
                    vine->chain.timer = 0;
                    vine->speed = 0.0F;
                }
            }
        } else {
            vine->wrap = 0;
        }
        vine->wobble_steps++;
        looping_vine_pose(
            vine->chain.objects,
            DEGREES(njSin(DEGREES(vine->wobble_steps * WOBBLE_RATE)) *
                    vine->speed),
            (Angle)(vine->speed *
                    /* @hack spelling: the swing read for nothing before the
                     * sine's sum, where the sum alone would do */
                    (1.0F + (SPEED_GAIN * njSin(((void)vine->swing,
                                                 -vine->wrap - vine->swing)))) /
                    (LENGTH(work) * TWO_PI_ROUNDED) * vine_lag_z * FULL_TURN) +
                DEGREES(-(njSin(vine->wrap + vine->swing) * WOBBLE_TILT)),
            vine->swing, SCALE(work), vine->wrap, WRAP_MOST);
        looping_vine_locate_part(
            task, vine->chain.objects, vine->chain.count - 1, &vine->chain.tip,
            &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
    }
    if (vine->chain.timer < 0) {
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njTranslateV(NULL, &vine->chain.tip);
        njRotateZ(NULL, -vine->chain.tip_ang_z);
        njRotateX(NULL, vine->chain.tip_ang_x + HALF_TURN);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &hang_offset, &dir);
        njPopMatrix(1);
        player_set_pos(work->tick, dir.x, dir.y, dir.z);
        /* @hack spelling: offset, a local set to player_turn by a comma, where
         * player_turn read in place would do */
        offset = player_turn, player_work[work->tick]->ang.x =
                                  vine->chain.tip_ang_x + offset[0] + HALF_TURN;
        /* @hack redundant: a block no path reaches, which holds dir.x and dir.z
         * to 210 */
        if (0) {
            if (dir.x > 210.0F) {
                dir.x = 210.0F;
            }
            if (dir.z > 210.0F) {
                dir.z = 210.0F;
            }
        }
        player_work[work->tick]->ang.y = -work->ang.y + offset[1];
        player_work[work->tick]->ang.z = -vine->chain.tip_ang_z + offset[2];
        if (VINE_HELD(task) || !game_state.time_stopped) {
            vine->swing +=
                (Angle)(vine->speed / (LENGTH(work) * TWO_PI_ROUNDED) *
                        (1.0F +
                         (SPEED_GAIN * njSin(-vine->wrap - vine->swing))) *
                        FULL_TURN);
        }
        if ((player_records[work->tick]->flags & PLAYER_MISSED) ||
            game_state.cleared == 1) {
            player_request(work->tick, PLAYER_REQUEST_RELEASE, 0);
            vine->chain.timer = 0;
        }
    } else if (VINE_HELD(task) || !game_state.time_stopped) {
        if (vine->wrap > QUARTER_TURN) {
            vine->speed *= SETTLE_DRAG;
            /* @hack spelling: old, a local for the wrap plus the swing, where
             * the sum in place would do */
            old = vine->wrap + vine->swing;
            vine->speed -= (old - FULL_TURN) * SETTLE_PULL;
        }
    }
    if (vine->chain.timer >= 0) {
        i = looping_vine_player_within(&vine->chain.tip,
                                       CATCH_RANGE * CATCH_RANGE);
        if (i >= 0 && vine->chain.timer >= CATCH_WAIT) {
            player = player_tasks[i];
            vine->speed =
                njScalor(&((struct player_motion *)player->motion.p)->spd);
            vine->speed *= throw_scale;
            if (vine->speed < throw_min) {
                vine->speed = throw_min;
            }
            if (vine->speed > throw_max) {
                vine->speed = throw_max;
            }
            work->tick = i;
            sound_play(VINE_SOUND, NULL, 0, 0);
            player_request(i, PLAYER_REQUEST_ANIM, ANIM_HANG);
            vibration_start(i, CATCH_POWER, CATCH_FREQ, CATCH_TIME);
            /* @hack type: time_stopped read through dat_8c0db20c, where
             * game_state.time_stopped would do */
            VINE_HELD(task) = STAGE_TIME_STOPPED;
            vine->chain.timer = -1;
        } else if (i >= 0) {
            vine->chain.timer = 0;
        }
    }
    if (vine->chain.timer >= 0 && !game_state.time_stopped) {
        vine->chain.timer++;
        if (vine->chain.timer >= CATCH_WAIT) {
            vine->chain.timer = CATCH_WAIT;
            if (VINE_DONE(task)) {
                task_kill(task);
            }
        }
    }
}

static void looping_vine_draw(struct task *task)
{
    struct task_work *work;
    struct looping_vine *vine;

    work = task->work;
    vine = task->extra.p;
    if (player_near(&work->pos, NEAR_RANGE)) {
        njSetTexture(&vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &vine->chain.pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, -QUARTER_TURN);
        njCnkEasyDrawObject(vine->chain.objects);
        njPopMatrix(1);
    }
}

static void looping_vine_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct looping_vine *vine = self->extra.p;

    syFree(vine->chain.objects);
    syFree(vine);
    self->extra.p = NULL;
}
