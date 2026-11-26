/* the twining vine, KARAMI in a stage's object list: a vine that hangs from a
 * tree. a player who catches its tip is carried round the trunk, then swung
 * out and thrown */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/revolving_vine.h"
#include "shared/twining_vine.h"
#include "shared/vine.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the vine's modes, in its work's mode */
enum twining_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct twining_vine {
    struct vine_chain chain;
    /* how far the vine has swung out, up to QUARTER_TURN. it swings only once
     * turn reaches TURN_END */
    Angle swing;
    Angle slope; /* the tilt of the vine's top, as the turn sets it */
    Angle turn; /* how far the vine has turned round the trunk */
    Float unk_38;
    /* how far the last step moved the swing and the turn together, in
     * degrees */
    Float moved_degrees;
    /* the speed of the turn and the swing, in units of length a step */
    Float speed;
    Uint8 unk_44[0x10];
};

/* the slot of the player on the vine */
#define PLAYER(work) ((work)->tick)
/* the placement's state the vine sets */
#define STATE_LIVE 1

/* bits of the work's ang.z. SLOPE_BITS hold the most the vine's top tilts.
 * REVERSE turns the vine the other way round the trunk */
#define SLOPE_BITS 0x7fff
#define REVERSE 0x8000
/* how far the vine turns round the trunk: ang.x's low 16 bits. the slope
 * divides by it */
#define TURN_END(work) ((Uint16)(work)->ang.x)
/* the vine's length from its top to the work's pos. the turn runs the vine's
 * speed a step along a circle of that radius */
#define LENGTH_SCALED 285.0F
#define LENGTH_BASE 210.0F
#define LENGTH(work) ((((work)->scl.y + 1.0F) * LENGTH_SCALED) + LENGTH_BASE)
/* the swing runs the vine's speed a step along a circle. its radius is
 * SWING_RADIUS, less SWING_SHRINK times the swing's magnitude */
#define SWING_RADIUS 210.0F
#define SWING_SHRINK 0.0014381289F
#define SWING_LENGTH(vine)                                                     \
    (-_builtin_fabsf((vine)->swing * SWING_SHRINK) + SWING_RADIUS)
/* 2 pi to two places, as the turn and the swing take it */
#define TWO_PI_ROUNDED 6.28F
/* the speed the vine takes once its swing reaches QUARTER_TURN. it runs the
 * vine back through the swing and the turn, to rest. the throw's speed is
 * scl.z plus BACK_SPEED */
#define BACK_SPEED (-3.0F)
/* the throw leaves along (THROW_X, THROW_Y, 0), made a unit vector and turned
 * about y */
#define THROW_X 1.0F
#define THROW_Y 0.3F
/* the vine moves and draws only while a player lies within NEAR_RANGE of
 * its work's pos */
#define NEAR_RANGE 4000.0F
/* a player catches the vine where its first shape lies within CATCH_RANGE of
 * the tip. the catch waits until the chain's timer has counted CATCH_WAIT
 * steps */
#define CATCH_RANGE 10.0F
#define CATCH_WAIT 30
/* the vine's sound, of the stage's sound list */
#define VINE_SOUND 0
/* the vibrations of the player's pad as it catches the vine, and as the vine
 * throws it */
#define CATCH_POWER 2
#define CATCH_FREQ 15
#define CATCH_TIME 2
#define THROW_POWER 4
#define THROW_FREQ 15

/* the parts of the vine's chain, from its top. the slope tilts the
 * FRAME_PARTS, each by the slope over SLOPE_SHARES, and the swing bends the
 * BEND_PARTS. the END_PARTS trail the vine's motion */
#define FRAME_PARTS 4
#define SLOPE_SHARES 3
#define BEND_PARTS 9
#define END_PARTS 4
/* the swing spreads over 1 + BEND_SPREAD / sin(slope) of the bend parts, at
 * most BEND_PARTS */
#define BEND_SPREAD 0.20943953F

/* the preview runs the vine through its turn in PREVIEW_STEPS passes, then
 * through its swing in as many. each pass moves the swing, or the angle of the
 * slope's sine, by PREVIEW_ANGLE_STEP. it draws the vine on one pass in
 * PREVIEW_DRAW_EVERY */
#define PREVIEW_STEPS 32
#define PREVIEW_ANGLE_STEP (QUARTER_TURN / PREVIEW_STEPS)
#define PREVIEW_DRAW_EVERY 3

/* a hanging player lies at hang_offset from the tip, in the tip's axes.
 * player_turn adds to the angles that hang it along the tip's part */
static NJS_POINT3 hang_offset = {-2.0F, -7.0F, 0.0F};
static Angle player_turn[3] = {0, 0, 0};
/* a caught player's speed, times catch_speed_scale, is the vine's speed, held
 * to catch_speed_min to catch_speed_max */
static Float catch_speed_max = 30.0F;
static Float catch_speed_min = 4.0F;
static Float catch_speed_scale = 2.5F;

static void twining_vine_start(struct task *task);
static void twining_vine_run(struct task *task);
static void twining_vine_draw(struct task *task);
static void twining_vine_destroy(struct task *task);

static Float twining_vine_distance_squared(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = from->x - to->x;
    Float dy = from->y - to->y;
    Float dz = from->z - to->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* the first of players 0 and 1 whose first shape's center lies within range
 * of pos. -1 is neither. range is the square of a distance */
static int twining_vine_player_within(NJS_POINT3 *pos, Float range)
{
    if (player_work[0] != NULL &&
        twining_vine_distance_squared(
            &player_work[0]->collision->shapes->center, pos) < range) {
        return 0;
    }
    if (player_work[1] != NULL &&
        twining_vine_distance_squared(
            &player_work[1]->collision->shapes->center, pos) < range) {
        return 1;
    }
    return -1;
}

/* pose the chain whose first part is object. scale stretches the frame
 * parts, and slope tilts them. swing bends the bend parts about y. each in
 * turn takes most over the count BEND_SPREAD gives, until swing runs out. at
 * a slope of 0 that count divides by 0, and comes out BEND_PARTS. lag_y and
 * lag_z turn each end part about y and z, and the last by half. reverse turns
 * the bend and end parts about y the other way */
static void twining_vine_pose(NJS_CNK_OBJECT *object, int lag_y, int lag_z,
                              Angle slope, Float scale, Angle swing, int most,
                              int reverse)
{
    NJS_CNK_OBJECT *source = &vine_object;
    int i;
    int tilt = slope / SLOPE_SHARES;
    int parts;
    int full;
    Angle step;
    int rest;

    for (i = 0; i < FRAME_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->pos[0] = source->pos[0] * scale;
        object->ang[1] = 0;
        object->ang[2] = tilt;
    }
    parts = 1.0F + (1.0F / njCos(QUARTER_TURN - slope) * BEND_SPREAD);
    if (parts > BEND_PARTS) {
        parts = BEND_PARTS;
    }
    step = most / parts;
    full = swing / step;
    rest = swing - (full * step);
    if (reverse) {
        step = -step;
        rest = -rest;
    }
    for (i = 0; i < BEND_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        /* @hack spelling: one conditional inside another, where an if for
         * each case would do */
        /* NOLINTNEXTLINE(readability-avoid-nested-conditional-operator) */
        object->ang[1] = i < full ? step : i == full ? rest : 0;
        object->ang[2] = 0;
    }
    /* @hack redundant: a loop of no passes */
    for (i = 0; i < 0; i++, object = object->child, source = source->child) {
    }
    if (reverse) {
        lag_y = -lag_y;
    }
    for (i = 0; i < END_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->ang[1] = object->child ? lag_y : lag_y / 2;
        object->ang[2] = object->child ? lag_z : lag_z / 2;
    }
}

/* find the tip of the chain's first count parts, from object. the chain hangs
 * radius out from the trunk, at the vine's top. it is turned about y by the
 * work's ang.y, then by turn, the other way under REVERSE. point takes the
 * center of the last part's model. ang_x and ang_z take the turns that set a
 * hanging player along that part. count must be 1 or more */
static void twining_vine_find_tip(struct task *task, NJS_CNK_OBJECT *object,
                                  int count, Float radius, Angle turn,
                                  NJS_POINT3 *point, Angle *ang_x, Angle *ang_z)
{
    struct task_work *work = task->work;
    struct twining_vine *vine = task->extra.p;
    NJS_CNK_MODEL *model;
    NJS_VECTOR axis;
    int i;

    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &vine->chain.pos);
    njRotateY(NULL, work->ang.y);
    njRotateY(NULL, (work->ang.z & REVERSE) ? -turn : turn);
    njTranslate(NULL, radius, 0.0F, 0.0F);
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

void twining_vine_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        task->destructor = twining_vine_destroy;
        task->draw = twining_vine_draw;
        twining_vine_start(task);
        break;
    case MODE_RUN:
        twining_vine_run(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

static void twining_vine_start(struct task *task)
{
    struct task_work *work = task->work;
    struct twining_vine *vine;
    NJS_CNK_OBJECT *object;
    int count;
    int i;

    vine = syCalloc(1, sizeof(struct twining_vine));
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
    vine->swing = 0;
    vine->turn = 0;
    vine->slope = 0;
    vine->unk_38 = 0.0F;
    vine->moved_degrees = 0.0F;
    vine->speed = 0.0F;
    vine->chain.pos = work->pos;
    vine->chain.pos.y += LENGTH(work);
    /* @hack redundant: the store made again, though the one above set the
     * slope */
    vine->slope = 0;
    work->timer = 0;
    if (task->placement) {
        placement_set_state(task, STATE_LIVE);
    }
}

static void twining_vine_run(struct task *task)
{
    struct task_work *work = task->work;
    struct twining_vine *vine = task->extra.p;
    struct task *player;
    NJS_VECTOR dir;
    Angle old_swing;
    Angle old_turn;
    int slot;

    /* @hack redundant: a block no path reaches, which writes the work's pos
     * through dat_8c0db20c */
    if (0) {
        dat_8c0db20c.point = work->pos;
    }
    /* @hack redundant: a block no path reaches, which zeroes the swing and
     * sets the top's y to 1 */
    if (0) {
        vine->swing = 0;
        vine->chain.pos.y = 1.0F;
    }
    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object, with its loops' code. it previews the vine
     * through its turn and its swing */
    if (0) {
        NJS_POINT3 prev;
        Angle ang_x;
        Angle ang_z;
        int j;
        Angle step;

        vine->chain.pos = work->pos;
        vine->chain.pos.y += LENGTH(work);
        step = TURN_END(work) / PREVIEW_STEPS;
        vine->speed = work->scl.z;
        twining_vine_pose(vine->chain.objects, 0, 0, vine->slope,
                          work->scl.y + 1.0F, 0, QUARTER_TURN,
                          (work->ang.z & REVERSE) != 0);
        twining_vine_find_tip(task, vine->chain.objects, vine->chain.count - 1,
                              work->scl.x, vine->turn, &vine->chain.tip,
                              &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
        for (j = 0; j < PREVIEW_STEPS; j++) {
            twining_vine_pose(vine->chain.objects, 0, 0, vine->slope,
                              work->scl.y + 1.0F, 0, QUARTER_TURN,
                              (work->ang.z & REVERSE) != 0);
            prev = vine->chain.tip;
            twining_vine_find_tip(
                task, vine->chain.objects, vine->chain.count - 1, work->scl.x,
                vine->turn, &vine->chain.tip, &vine->chain.tip_ang_x,
                &vine->chain.tip_ang_z);
            dat_8c0db20c.point = vine->chain.tip;
            vine->turn += step;
            vine->slope =
                njSin(j * PREVIEW_ANGLE_STEP) * (work->ang.z & SLOPE_BITS);
            if (j > 0) {
                line_draw(prev.x, prev.y, prev.z, dat_8c0db20c.point.x,
                          dat_8c0db20c.point.y, dat_8c0db20c.point.z);
            }
            if (j % PREVIEW_DRAW_EVERY == 0) {
                twining_vine_draw(task);
            }
        }
        for (j = 0; j < PREVIEW_STEPS; j++) {
            twining_vine_pose(vine->chain.objects, 0, 0, vine->slope,
                              work->scl.y + 1.0F, j * PREVIEW_ANGLE_STEP,
                              QUARTER_TURN, (work->ang.z & REVERSE) != 0);
            prev = vine->chain.tip;
            twining_vine_find_tip(
                task, vine->chain.objects, vine->chain.count - 1, work->scl.x,
                vine->turn, &vine->chain.tip, &vine->chain.tip_ang_x,
                &vine->chain.tip_ang_z);
            dat_8c0db20c.point = vine->chain.tip;
            if (j > 0) {
                line_draw(prev.x, prev.y, prev.z, dat_8c0db20c.point.x,
                          dat_8c0db20c.point.y, dat_8c0db20c.point.z);
            }
            if (j % PREVIEW_DRAW_EVERY == 0) {
                twining_vine_draw(task);
            }
        }
        twining_vine_find_tip(task, vine->chain.objects, FRAME_PARTS,
                              work->scl.x, vine->turn, &prev, &ang_x, &ang_z);
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y + (work->ang.z & REVERSE) ? -vine->turn
                                                              : vine->turn);
        njTranslate(NULL, 0.0F, 0.0F, -10.0F);
        njRotateY(NULL, -(work->ang.y + (work->ang.z & REVERSE) ? -vine->turn
                                                                : vine->turn));
        njCalcPoint(NULL, &prev, &prev);
        njPopMatrix(1);
        njSetTexture(&vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &prev);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, QUARTER_TURN);
        njScale(NULL, 20.0F, 20.0F, 20.0F);
        njScale(NULL, 1.0F / 30.0F, 1.0F / 30.0F, 1.0F / 30.0F);
        njCnkEasyDrawObject(&vine_preview_object);
        njPopMatrix(1);
        {
            Float value = prev.x;
            Int loc = NJM_LOCATION(8, 23);
            char buf[20];

            sprintf(buf, "%08.2f", value);
            njPrintC(loc, buf);
        }
        {
            Float value = prev.y;
            Int loc = NJM_LOCATION(8, 24);
            char buf[20];

            sprintf(buf, "%08.2f", value);
            njPrintC(loc, buf);
        }
        {
            Float value = prev.z;
            Int loc = NJM_LOCATION(8, 25);
            char buf[20];

            sprintf(buf, "%08.2f", value);
            njPrintC(loc, buf);
        }
        njPrintC(NJM_LOCATION(17, 23), "<- WOOD POS X");
        njPrintC(NJM_LOCATION(17, 24), "<- WOOD POS Y");
        njPrintC(NJM_LOCATION(17, 25), "<- WOOD POS Z");
        njPrintC(NJM_LOCATION(17, 16), "<- ANGLE ( ROT WIDTH )");
        njPrintC(NJM_LOCATION(17, 17), "<- ANGLE ( ROT POS Y )");
        njPrintC(NJM_LOCATION(17, 18), "<- ANGLE ( TUTA SLOPE ) ");
        njPrintC(NJM_LOCATION(17, 19), "<- ROT RADIUS");
        njPrintC(NJM_LOCATION(17, 20), "<- TUTA LENGTH");
        njPrintC(NJM_LOCATION(17, 21), "<- ADD SPEED");
        return;
    } else if (task->placement) {
        placement_set_state(task, STATE_LIVE);
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
        old_swing = vine->swing;
        old_turn = vine->turn;
        if (vine->turn >= TURN_END(work)) {
            vine->swing +=
                (Angle)(vine->speed / (SWING_LENGTH(vine) * TWO_PI_ROUNDED) *
                        FULL_TURN);
            if (vine->chain.timer < 0 && njCos(old_swing) < 0.0F &&
                !(njCos(vine->swing) < 0.0F)) {
                sound_play(VINE_SOUND, NULL, 0, 0);
            }
            if (vine->swing >= QUARTER_TURN) {
                vine->swing = QUARTER_TURN;
                vine->speed = BACK_SPEED;
                if (vine->chain.timer < 0) {
                    dir.x = THROW_X;
                    dir.y = THROW_Y;
                    dir.z = 0.0F;
                    njUnitVector(&dir);
                    njPushMatrix(NULL);
                    njUnitMatrix(NULL);
                    /* @bug logic: turns the throw by the vine's turn alone,
                     * where twining_vine_find_tip turns the vine by ang.y and
                     * then by the turn. so the throw heads out from the trunk
                     * only where ang.y is 0. the test at 0xeda of the
                     * object's code takes the turn's sign from ang.y plus
                     * REVERSE's bit, as + binds tighter than ?: */
                    njRotateY(NULL, work->ang.y + (work->ang.z & REVERSE)
                                        ? -vine->turn
                                        : vine->turn);
                    njCalcVector(NULL, &dir, &dir);
                    njPopMatrix(1);
                    player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
                    vibration_start(PLAYER(work), THROW_POWER, THROW_FREQ,
                                    VIBRATION_UNTIMED);
                    VINE_DONE(task) = TRUE;
                    player_fling(PLAYER(work), vine->speed + work->scl.z, &dir);
                    vine->chain.timer = 0;
                }
            } else if (vine->swing < 0) {
                vine->swing = 0;
                vine->turn +=
                    (Angle)(vine->speed / (LENGTH(work) * TWO_PI_ROUNDED) *
                            FULL_TURN);
            }
        }
        if (vine->turn < TURN_END(work)) {
            vine->turn += (Angle)(vine->speed /
                                  (LENGTH(work) * TWO_PI_ROUNDED) * FULL_TURN);
            if (vine->turn > TURN_END(work)) {
                vine->turn = TURN_END(work);
            }
            vine->swing = 0;
            if (vine->turn < 0) {
                vine->swing = 0;
                vine->turn = 0;
                vine->speed = 0.0F;
            }
        }
        vine->moved_degrees =
            (vine->swing + vine->turn - old_swing - old_turn) *
            DEGREES_PER_ANGLE;
        vine->slope =
            njSin(vine->turn / (Float)TURN_END(work) * (Float)QUARTER_TURN) *
            (work->ang.z & SLOPE_BITS);
        twining_vine_pose(vine->chain.objects,
                          vine->speed / (SWING_LENGTH(vine) * TWO_PI_ROUNDED) *
                              vine_lag_y * FULL_TURN,
                          vine->moved_degrees /
                              (LENGTH(work) * TWO_PI_ROUNDED) * vine_lag_z *
                              FULL_TURN,
                          vine->slope, work->scl.y + 1.0F, vine->swing,
                          QUARTER_TURN, (work->ang.z & REVERSE) != 0);
        twining_vine_find_tip(task, vine->chain.objects, vine->chain.count - 1,
                              work->scl.x, vine->turn, &vine->chain.tip,
                              &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
    }
    if (vine->chain.timer < 0) {
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njTranslateV(NULL, &vine->chain.tip);
        njRotateZ(NULL, -vine->chain.tip_ang_z);
        njRotateX(NULL, vine->chain.tip_ang_x + HALF_TURN);
        njRotateY(NULL, ((work->ang.z & REVERSE) ? -vine->turn : vine->turn) +
                            work->ang.y);
        njCalcPoint(NULL, &hang_offset, &dir);
        njPopMatrix(1);
        player_set_pos(PLAYER(work), dir.x, dir.y, dir.z);
        player_work[PLAYER(work)]->ang.x =
            vine->chain.tip_ang_x + player_turn[0] + HALF_TURN;
        player_work[PLAYER(work)]->ang.y =
            -work->ang.y -
            ((work->ang.z & REVERSE) ? -vine->turn : vine->turn) +
            player_turn[1];
        player_work[PLAYER(work)]->ang.z =
            -vine->chain.tip_ang_z + player_turn[2];
        /* @hack redundant: a block no path reaches, which reads
         * player_turn[0] */
        if (0) {
            vine->swing = player_turn[0];
        }
        if ((player_records[PLAYER(work)]->flags & PLAYER_MISSED) ||
            game_state.cleared == 1) {
            player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
            vine->chain.timer = 0;
        }
    }
    if (vine->chain.timer >= 0) {
        slot = twining_vine_player_within(&vine->chain.tip,
                                          CATCH_RANGE * CATCH_RANGE);
        if (slot >= 0 && vine->chain.timer >= CATCH_WAIT) {
            player = player_tasks[slot];
            vine->speed =
                njScalor(&((struct player_motion *)player->motion.p)->spd);
            vine->speed *= catch_speed_scale;
            if (_builtin_fabsf(vine->speed) < catch_speed_min) {
                vine->speed =
                    vine->speed >= 0.0F ? catch_speed_min : -catch_speed_min;
            }
            if (_builtin_fabsf(vine->speed) > catch_speed_max) {
                vine->speed =
                    vine->speed >= 0.0F ? catch_speed_max : -catch_speed_max;
            }
            PLAYER(work) = slot;
            sound_play(VINE_SOUND, NULL, 0, 0);
            player_request(slot, PLAYER_REQUEST_ANIM, ANIM_VINE_HANG);
            vine->chain.timer = -1;
            /* @hack type: time_stopped read through dat_8c0db20c, where
             * game_state.time_stopped would do */
            VINE_HELD(task) = STAGE_TIME_STOPPED;
            vibration_start(slot, CATCH_POWER, CATCH_FREQ, CATCH_TIME);
        } else if (slot >= 0) {
            vine->chain.timer = 0;
        }
    }
    if (vine->chain.timer >= 0 && !game_state.time_stopped) {
        /* @hack spelling: the timer's step, cap and test of done as one
         * condition, where three statements would do */
        if ((vine->chain.timer++, 1) && vine->chain.timer >= CATCH_WAIT &&
            (vine->chain.timer = CATCH_WAIT, VINE_DONE(task))) {
            task_kill(task);
        }
    }
}

static void twining_vine_draw(struct task *task)
{
    struct task_work *work;
    struct twining_vine *vine;

    work = task->work;
    vine = task->extra.p;
    if (player_near(&work->pos, NEAR_RANGE)) {
        njSetTexture(&vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &vine->chain.pos);
        njRotateY(NULL, work->ang.y);
        njRotateY(NULL, (work->ang.z & REVERSE) ? -vine->turn : vine->turn);
        njTranslate(NULL, work->scl.x, 0.0F, 0.0F);
        njRotateZ(NULL, -QUARTER_TURN);
        njCnkEasyDrawObject(vine->chain.objects);
        njPopMatrix(1);
    }
}

static void twining_vine_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct twining_vine *vine = self->extra.p;

    syFree(vine->chain.objects);
    syFree(vine);
    self->extra.p = NULL;
}
