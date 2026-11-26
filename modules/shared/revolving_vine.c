/* the revolving vine, ROLTUTA in Green Forest's object list: a vine whose top
 * runs round a circle. a player who catches its tip is carried round as the
 * vine swings out and back. the vine then throws it. White Jungle's vine of
 * stg04/revolving_vine.c is its twin */
#include "shared/revolving_vine/text.inc"
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/revolving_vine.h"
#include "shared/vine.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the vine's modes, in its work's mode */
enum revolving_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct revolving_vine {
    struct vine_chain chain;
    Angle turn; /* how far the top has run round its circle */
    Angle swing; /* swing_degrees as an Angle */
    Float swing_degrees; /* how far the vine swings out, in degrees */
    Float swing_speed; /* how much swing_degrees changed on the last step */
    /* the top's speed round its circle, in units of length a step */
    Float speed;
    /* the top stops at end_degrees, and the vine throws its player past
     * throw_degrees. degrees counts how far the top has run round since the
     * start or the last catch. all three are in degrees */
    Float end_degrees;
    Float throw_degrees;
    Float degrees;
    Float swing_most; /* the most the vine swings out, in degrees */
};

/* the steps the vine has moved with a player on its tip, up to
 * HANG_QUIET_STEPS. it is byte 1 of unk_3c, between vine.h's VINE_HELD and
 * VINE_DONE */
/* @hack type: unk_3c read as bytes through its address, where
 * (task)->unk_3c.b[1] would do */
#define HANG_STEPS(task) (((Sint8 *)&(task)->unk_3c)[1])

/* the slot of the player on the vine */
#define PLAYER(work) ((work)->tick)
/* the placement's state the start sets. the vine ends once it is 0 */
#define STATE_LIVE 1
/* the vine moves and draws only while a player lies within NEAR_RANGE of its
 * work's pos */
#define NEAR_RANGE 4000.0F
/* a player catches the vine where its first shape lies within CATCH_RANGE of
 * the tip. the catch waits until the chain's timer has counted CATCH_WAIT
 * steps */
#define CATCH_RANGE 10.0F
#define CATCH_WAIT 30
/* 2 pi, as the vine's turn rounds it */
#define TWO_PI_ROUNDED 6.28F
#define DEGREES_PER_TURN 360.0F
/* the vine's length at an scl.y of 0. the start places the circle's center by
 * it, and the swing's lag divides by it */
#define LENGTH 100.0F
/* the swing is swing_most times the sine of a phase. the phase runs from 0
 * to SWING_RISE degrees as the top runs to throw_degrees. it then runs on by
 * SWING_FALL, to 180, as the top runs on to end_degrees */
#define SWING_RISE 100.0F
#define SWING_FALL 80.0F
/* from end_degrees on, speed and swing_speed keep SPEED_KEEP of themselves
 * each step */
#define SPEED_KEEP 0.95F
/* a vine whose throw_degrees is under FEW_DEGREES carries its player until
 * its speed falls under STILL_SPEED. the end of a carry throws the player only
 * above THROW_LEAST, so that vine throws none */
#define FEW_DEGREES 0.01F
#define STILL_SPEED 0.1F
#define THROW_LEAST 0.5F
/* the throw's speed gains ADDED_SPEED for each unit of ang.z's low half, a
 * signed number */
#define ADDED_SPEED 0.1F
/* the sound of a hanging player, of the stage's sound list. it plays on each
 * step the vine moves once the player has hung for HANG_QUIET_STEPS of them */
#define HANG_SOUND 1
#define HANG_SOUND_PRIORITY 1
#define HANG_SOUND_VOLUME 120
#define HANG_SOUND_DURATION 10
#define HANG_QUIET_STEPS 30
/* the pad shakes at the catch and at the throw */
#define CATCH_POWER 2
#define CATCH_FREQ 15
#define CATCH_TIME 2
#define THROW_POWER 4
#define THROW_FREQ 15

/* the debug preview poses the vine PREVIEW_STEPS times, a step of its speed
 * apart. it draws one pose in PREVIEW_DRAW_EVERY, and a line along the tip's
 * path */
#define PREVIEW_STEPS 144
#define PREVIEW_DRAW_EVERY 4

static void revolving_vine_start(struct task *task);
static void revolving_vine_run(struct task *task);
static void revolving_vine_draw(struct task *task);
static void revolving_vine_destroy(struct task *task);

/* the catch sets the vine's speed to the player's times throw_scale, held
 * between throw_min and throw_max */
static Float throw_max = 10.0F;
static Float throw_min = 3.0F;
static Float throw_scale = 1.0F;

/* the vine's texture list and model, a chain of chunk objects from
 * vine_part_0, and a panel nothing draws. part 0 caches the whole vine's
 * polygons, and parts 1 and 7 have no model. parts 2 to 6 set the vertices the
 * polygons use, blending their transforms by weight. part 6 draws them */
static NJS_TEXNAME vine_texnames[1] = {
#include "shared/revolving_vine/vine_texnames.inc"
};
NJS_TEXLIST revolving_vine_texlist = {
#include "shared/revolving_vine/revolving_vine_texlist.inc"
};
static NJS_CNK_OBJECT vine_part_7 = {
#include "shared/revolving_vine/vine_part_7.inc"
};
static Sint16 vine_part_6_polygons[2] = {
#include "shared/revolving_vine/vine_part_6_polygons.inc"
};
static Sint32 vine_part_6_vertices[53] = {
#include "shared/revolving_vine/vine_part_6_vertices.inc"
};
static NJS_CNK_MODEL vine_part_6_model = {
#include "shared/revolving_vine/vine_part_6_model.inc"
};
static NJS_CNK_OBJECT vine_part_6 = {
#include "shared/revolving_vine/vine_part_6.inc"
};
static Sint32 vine_part_5_vertices[68] = {
#include "shared/revolving_vine/vine_part_5_vertices.inc"
};
static NJS_CNK_MODEL vine_part_5_model = {
#include "shared/revolving_vine/vine_part_5_model.inc"
};
static NJS_CNK_OBJECT vine_part_5 = {
#include "shared/revolving_vine/vine_part_5.inc"
};
static Sint32 vine_part_4_vertices[68] = {
#include "shared/revolving_vine/vine_part_4_vertices.inc"
};
static NJS_CNK_MODEL vine_part_4_model = {
#include "shared/revolving_vine/vine_part_4_model.inc"
};
static NJS_CNK_OBJECT vine_part_4 = {
#include "shared/revolving_vine/vine_part_4.inc"
};
static Sint32 vine_part_3_vertices[89] = {
#include "shared/revolving_vine/vine_part_3_vertices.inc"
};
static NJS_CNK_MODEL vine_part_3_model = {
#include "shared/revolving_vine/vine_part_3_model.inc"
};
static NJS_CNK_OBJECT vine_part_3 = {
#include "shared/revolving_vine/vine_part_3.inc"
};
static Sint32 vine_part_2_vertices[101] = {
#include "shared/revolving_vine/vine_part_2_vertices.inc"
};
static NJS_CNK_MODEL vine_part_2_model = {
#include "shared/revolving_vine/vine_part_2_model.inc"
};
static NJS_CNK_OBJECT vine_part_2 = {
#include "shared/revolving_vine/vine_part_2.inc"
};
static NJS_CNK_OBJECT vine_part_1 = {
#include "shared/revolving_vine/vine_part_1.inc"
};
static Sint16 vine_part_0_polygons[207] = {
#include "shared/revolving_vine/vine_part_0_polygons.inc"
};
static NJS_CNK_MODEL vine_part_0_model = {
#include "shared/revolving_vine/vine_part_0_model.inc"
};
static NJS_CNK_OBJECT vine_part_0 = {
#include "shared/revolving_vine/vine_part_0.inc"
};
static NJS_TEXNAME panel_texnames[1] = {
#include "shared/revolving_vine/panel_texnames.inc"
};
static NJS_TEXLIST panel_texlist = {
#include "shared/revolving_vine/panel_texlist.inc"
};
static Sint16 panel_polygons[27] = {
#include "shared/revolving_vine/panel_polygons.inc"
};
static Sint32 panel_vertices[27] = {
#include "shared/revolving_vine/panel_vertices.inc"
};
static NJS_CNK_MODEL panel_model = {
#include "shared/revolving_vine/panel_model.inc"
};
static NJS_CNK_OBJECT panel_object = {
#include "shared/revolving_vine/panel_object.inc"
};

/* a hanging player lies at hang_offset from the tip, in the tip's axes.
 * player_turn adds to the angles that hang it along the tip's part */
static NJS_POINT3 hang_offset = {-2.0F, -7.0F, 0.0F};
static Angle player_turn[3] = {0, 0, 0};
static Float dat_revolving_vine_0 = 0.015F;
static Float dat_revolving_vine_1 = 0.95F;
static Float dat_revolving_vine_2 = -10.0F;
static Float dat_revolving_vine_3 = 0.06F;
Float vine_gravity = 0.2F;
Float vine_lag_y = -2.0F;
Float vine_lag_z = -25.0F;

static Float revolving_vine_distance_squared(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = from->x - to->x;
    Float dy = from->y - to->y;
    Float dz = from->z - to->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* the first of players 0 and 1 whose first shape's center is near pos, or -1
 * for neither. a center is near where its squared distance from pos is below
 * range_squared */
static int revolving_vine_player_within(NJS_POINT3 *pos, Float range_squared)
{
    if (player_work[0] != NULL &&
        revolving_vine_distance_squared(
            &player_work[0]->collision->shapes->center, pos) < range_squared) {
        return 0;
    }
    if (player_work[1] != NULL &&
        revolving_vine_distance_squared(
            &player_work[1]->collision->shapes->center, pos) < range_squared) {
        return 1;
    }
    return -1;
}

/* pose the chain whose first part is object. scale stretches it along its
 * length. the first part swings out by swing about z. the parts between turn
 * by turn_y about y and turn_z about z. the last part turns by half of
 * each */
static void revolving_vine_pose(NJS_CNK_OBJECT *object, Angle turn_y,
                                Angle turn_z, Angle swing, Float scale)
{
    int i = 0;
    NJS_CNK_OBJECT *source = &vine_part_0;
    Angle half_y = turn_y / 2;
    Angle half_z = turn_z / 2;

    while (object) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->pos[0] = source->pos[0] * scale;
        /* @hack spelling: one conditional inside another, where an if for
         * each case would do */
        /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
        object->ang[1] = i == 0 ? 0 : object->child ? turn_y : half_y;
        object->ang[2] = i == 0 ? swing : object->child ? turn_z : half_z;
        /* NOLINTEND(readability-avoid-nested-conditional-operator) */
        object = object->child;
        source = source->child;
        ++i;
    }
}

/* set point to the center of the count-th part's model, in the chain whose
 * first part is object. the chain stands as revolving_vine_draw places it.
 * radius stands in for scl.x, and turn for the vine's turn. ang_x and ang_z
 * get the turns about x and z that align a hanging player with that part.
 * nothing reads swing. count must be 1 or more */
static void revolving_vine_find_tip(struct task *task, NJS_CNK_OBJECT *object,
                                    int count, Float radius, Angle turn,
                                    Angle swing, NJS_POINT3 *point,
                                    Angle *ang_x, Angle *ang_z)
{
    struct task_work *work = task->work;
    struct revolving_vine *vine = task->extra.p;
    NJS_CNK_MODEL *model;
    NJS_VECTOR axis;
    int i;

    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &vine->chain.pos);
    njRotateY(NULL, work->ang.y);
    njRotateY(NULL, turn);
    njTranslate(NULL, radius, 0.0F, 0.0F);
    njRotateZ(NULL, -QUARTER_TURN);
    /* @hack spelling: model cleared in the loop's start, where a statement
     * of its own before the loop would do */
    for (i = 0, model = NULL; i < count; i++) {
        njTranslateV(NULL, (NJS_VECTOR *)object->pos);
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

void revolving_vine_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        task->destructor = revolving_vine_destroy;
        task->draw = revolving_vine_draw;
        revolving_vine_start(task);
        break;
    case MODE_RUN:
        revolving_vine_run(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

static void revolving_vine_start(struct task *task)
{
    struct task_work *work = task->work;
    struct revolving_vine *vine;
    NJS_CNK_OBJECT *source;
    int count;
    int i;
    Float swing_most;

    vine = syCalloc(1, sizeof(struct revolving_vine));
    task->extra.p = vine;
    count = 0;
    for (source = &vine_part_0; source; source = source->child) {
        ++count;
    }
    vine->chain.objects = syCalloc(count, sizeof(NJS_CNK_OBJECT));
    vine->chain.count = count;
    for (source = &vine_part_0, i = 0; source; source = source->child, ++i) {
        vine->chain.objects[i] = *source;
        if (i) {
            vine->chain.objects[i - 1].child = &vine->chain.objects[i];
        }
    }
    work->mode = MODE_RUN;
    vine->chain.timer = CATCH_WAIT;
    vine->turn = 0;
    vine->swing = 0;
    vine->swing_degrees = 0.0F;
    vine->swing_speed = 0.0F;
    vine->speed = 0.0F;
    vine->throw_degrees = work->scl.z * DEGREES_PER_TURN;
    vine->end_degrees =
        ((Uint32)(vine->throw_degrees * (1.0F / DEGREES_PER_TURN)) + 1.0F) *
        DEGREES_PER_TURN;
    vine->degrees = 0.0F;
    /* @hack spelling: swing_most, a local scaled by a statement of its own,
     * where the product stored in the field would do */
    swing_most = (Sint16)work->ang.x;
    swing_most *= DEGREES_PER_ANGLE;
    vine->swing_most = swing_most;
    vine->chain.pos = work->pos;
    vine->chain.pos.y += (work->scl.y + 1.0F) * njCos(work->ang.x) * LENGTH;
    vine->chain.pos.x -=
        work->scl.x + ((work->scl.y + 1.0F) * njSin(work->ang.x) * LENGTH);
    if (task->placement) {
        placement_set_state(task, STATE_LIVE);
    }
}

static void revolving_vine_run(struct task *task)
{
    struct revolving_vine *vine = task->extra.p;
    struct game_state *state;
    struct task_work *work;
    NJS_VECTOR point;
    NJS_VECTOR dir;
    Float step;
    Float swing;
    Float throw_speed;
    Float added;
    Sint16 bias;
    int player;
    struct task *player_task;

    /* @hack spelling: state and work each set by a statement after a block
     * no path reaches, where their declarations would set them */
    /* @hack redundant: a store of 0 to the chain's timer, in a block no path
     * reaches */
    if (0) {
        vine->chain.timer = 0;
    }
    state = &game_state;
    /* @hack redundant: a store of SWING_FALL to swing_degrees, in a block no
     * path reaches */
    if (0) {
        vine->swing_degrees = SWING_FALL;
    }
    work = task->work;

    /* @hack redundant: a debug preview no path reaches, in the else of an
     * if (1) around the step. SHC keeps the preview's loop, and drops the
     * statements before it */
    if (1) {
        if (task->placement && !placement_get_state(task)) {
            task_kill(task);
            return;
        }
        if (!player_near(&work->pos, NEAR_RANGE)) {
            return;
        }
        /* @hack spelling: time_stopped read through state, here and at the
         * catch, where game_state.time_stopped would do */
        if (!state->time_stopped) {
            VINE_HELD(task) = 0;
        }
        /* @hack redundant: a block no path reaches, which stores
         * time_stopped in four of the vine's fields */
        if (0) {
            vine->chain.timer = state->time_stopped;
            vine->chain.count = state->time_stopped;
            vine->turn = state->time_stopped;
            vine->swing_speed = state->time_stopped;
        }
        /* @hack redundant: a block no path reaches, which draws a line */
        if (0) {
            line_draw(vine->swing_speed, added, 1.0F, FEW_DEGREES,
                      TWO_PI_ROUNDED, throw_max);
        }
        if (VINE_HELD(task) || !game_state.time_stopped) {
            if (vine->chain.timer < 0) {
                if (HANG_STEPS(task) < HANG_QUIET_STEPS) {
                    ++HANG_STEPS(task);
                } else {
                    sound_play_for(HANG_SOUND, work, HANG_SOUND_PRIORITY,
                                   HANG_SOUND_VOLUME, HANG_SOUND_DURATION);
                }
            } else {
                HANG_STEPS(task) = 0;
            }

            step =
                vine->speed / (work->scl.x * TWO_PI_ROUNDED) * DEGREES_PER_TURN;
            vine->degrees += _builtin_fabsf(step);
            if (vine->degrees >= vine->end_degrees) {
                step = 0.0F;
                swing = 0.0F;
                vine->degrees = vine->end_degrees;
                vine->speed *= SPEED_KEEP;
                vine->swing_speed *= SPEED_KEEP;
            } else {
                if (vine->degrees > vine->throw_degrees) {
                    Float part = (vine->degrees - vine->throw_degrees) /
                                 (vine->end_degrees - vine->throw_degrees);
                    swing = njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            vine->swing_most;
                } else {
                    Float part = vine->degrees / vine->throw_degrees;
                    swing = njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            vine->swing_most;
                }
                vine->swing_speed = swing - vine->swing_degrees;
            }
            vine->swing_degrees = swing;
            vine->swing = swing * ANGLE_PER_DEGREE;
            vine->turn += (Angle)(step * ANGLE_PER_DEGREE);
            /* @hack redundant: a block no path reaches, which stores
             * vine_lag_y in swing_degrees and THROW_LEAST in the speed */
            if (0) {
                vine->swing_degrees = vine_lag_y;
                vine->speed = THROW_LEAST;
            }
            /* @hack redundant: a block no path reaches, which stores
             * ANGLE_PER_DEGREE in degrees */
            if (0) {
                vine->degrees = ANGLE_PER_DEGREE;
            }
            revolving_vine_pose(
                vine->chain.objects,
                vine->speed / (work->scl.x * TWO_PI_ROUNDED) * vine_lag_y *
                    FULL_TURN,
                vine->swing_speed /
                    ((work->scl.y + 1.0F) * (TWO_PI_ROUNDED * LENGTH)) *
                    vine_lag_z * FULL_TURN,
                vine->swing, work->scl.y + 1.0F);
            revolving_vine_find_tip(
                task, vine->chain.objects, vine->chain.count - 1, work->scl.x,
                vine->turn, vine->swing, &vine->chain.tip,
                &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
        }
        if (vine->chain.timer >= 0) {
            player = revolving_vine_player_within(&vine->chain.tip,
                                                  CATCH_RANGE * CATCH_RANGE);
            if (player >= 0 && vine->chain.timer >= CATCH_WAIT) {
                player_task = player_tasks[player];
                PLAYER(work) = player;
                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njRotateY(NULL, -work->ang.y - vine->turn);
                njCalcPoint(
                    NULL, &((struct player_motion *)player_task->motion.p)->spd,
                    &point);
                njPopMatrix(1);
                /* the rotation into the vine's axes keeps the speed's
                 * length. so the vine takes the player's speed whatever its
                 * direction */
                vine->speed = njScalor(&point) * throw_scale;
                if (vine->speed < throw_min) {
                    vine->speed = throw_min;
                }
                if (vine->speed > throw_max) {
                    vine->speed = throw_max;
                }
                player_request(player, PLAYER_REQUEST_ANIM, ANIM_VINE_HANG);
                vibration_start(player, CATCH_POWER, CATCH_FREQ, CATCH_TIME);
                vine->chain.timer = -1;
                VINE_HELD(task) = state->time_stopped;
                vine->degrees = 0.0F;
            } else if (player >= 0) {
                vine->chain.timer = 0;
            }
        } else if (VINE_HELD(task) || !game_state.time_stopped) {
            /* @hack spelling: throw_at, a copy of throw_degrees, where the
             * field read in place would do */
            Float throw_at;

            njPushMatrix(NULL);
            njUnitMatrix(NULL);
            njTranslateV(NULL, &vine->chain.tip);
            njRotateZ(NULL, -vine->chain.tip_ang_z);
            njRotateX(NULL, vine->chain.tip_ang_x + HALF_TURN);
            njRotateY(NULL, work->ang.y + vine->turn);
            njCalcPoint(NULL, &hang_offset, &point);
            njPopMatrix(1);
            player_set_pos(PLAYER(work), point.x, point.y, point.z);
            /* @hack redundant: a block no path reaches, which stores degrees
             * in the work's scl.x */
            if (0) {
                work->scl.x = vine->degrees;
            }
            player_work[PLAYER(work)]->ang.x =
                vine->chain.tip_ang_x + player_turn[0] + HALF_TURN;
            player_work[PLAYER(work)]->ang.y =
                -work->ang.y - vine->turn + player_turn[1];
            player_work[PLAYER(work)]->ang.z =
                -vine->chain.tip_ang_z + player_turn[2];
            throw_at = vine->throw_degrees;
            /* @hack redundant: the test's first term, 1, which always
             * holds */
            if ((1 && throw_at > FEW_DEGREES && vine->degrees > throw_at) ||
                (throw_at < FEW_DEGREES &&
                 _builtin_fabsf(vine->speed) < STILL_SPEED)) {
                throw_speed = _builtin_fabsf(vine->speed);
                if (throw_speed > THROW_LEAST) {
                    /* @hack type: ang.z's low half read through a Sint16
                     * pointer, where a cast of ang.z to Sint16 would do */
                    bias = *(Sint16 *)&work->ang.z;
                    added = bias * ADDED_SPEED;
                    dir.z = -vine->speed;
                    dir.y = 1.0F;
                    dir.x = -vine->swing_speed;
                    njUnitVector(&dir);
                    njPushMatrix(NULL);
                    njUnitMatrix(NULL);
                    njRotateY(NULL, work->ang.y + vine->turn);
                    njCalcVector(NULL, &dir, &dir);
                    njPopMatrix(1);
                    player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
                    player_fling(PLAYER(work), throw_speed + added, &dir);
                    vibration_start(PLAYER(work), THROW_POWER, THROW_FREQ,
                                    VIBRATION_UNTIMED);
                    VINE_DONE(task) = TRUE;
                }
                vine->chain.timer = 0;
            } else if ((player_records[PLAYER(work)]->flags & PLAYER_MISSED) ||
                       game_state.cleared == 1) {
                player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
                vine->chain.timer = 0;
            }
        }
        if (vine->chain.timer >= 0 && !game_state.time_stopped) {
            ++vine->chain.timer;
            if (vine->chain.timer >= CATCH_WAIT) {
                vine->chain.timer = CATCH_WAIT;
            }
            if (VINE_DONE(task)) {
                task_kill(task);
            }
        }
    } else {
        NJS_POINT3 points[2];
        /* @hack redundant: spare, 16 bytes of the frame that nothing reads */
        Float spare[4];
        Float preview_step;
        Float preview_swing;
        Float swing_most;
        Angle turn_step;
        int i;
        NJS_POINT3 *prev;
        NJS_POINT3 *cur;

        preview_step =
            vine->speed / (work->scl.x * TWO_PI_ROUNDED) * DEGREES_PER_TURN;
        swing_most = vine->swing_most;
        turn_step = preview_step * ANGLE_PER_DEGREE;
        vine->degrees = 0.0F;
        /* @hack redundant: a store of CATCH_WAIT in the timer. it stands in
         * for the preview's setup, which the image doesn't hold, and is kept
         * for the registers SHC then gives the step */
        vine->chain.timer = CATCH_WAIT;
        i = 0;
        prev = points;
        *prev = vine->chain.tip;
        /* @hack spelling: the loop's pass written out four times, as SHC
         * unrolls it, where the pass written once would do */
        while (i < PREVIEW_STEPS) {
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        part = (vine->degrees - vine->throw_degrees) /
                               (vine->end_degrees - vine->throw_degrees);
                        preview_swing =
                            njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            swing_most;
                    } else {
                        part = vine->degrees / vine->throw_degrees;
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                *prev = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    task, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                /* @hack spelling: cur set in the first pass, where once before
                 * the loop would do */
                cur = points + 1;
                *cur = vine->chain.tip;
                if (i > 0) {
                    line_draw(prev->x, prev->y, prev->z, cur->x, cur->y,
                              cur->z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(task);
                }
                /* @hack redundant: a block no path reaches, which stores
                 * degrees through prev */
                if (0) {
                    prev->x = vine->degrees;
                }
            }
            ++i;
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        part = (vine->degrees - vine->throw_degrees) /
                               (vine->end_degrees - vine->throw_degrees);
                        preview_swing =
                            njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            swing_most;
                    } else {
                        part = vine->degrees / vine->throw_degrees;
                        /* @hack redundant: a conditional on part whose value
                         * nothing takes. it stands in for code the image
                         * doesn't hold, and is kept for the frame slots SHC
                         * then gives the preview */
                        (void)(part ? vine->speed : vine->swing_most);
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                *prev = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    task, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                /* @hack redundant: cur set again, though the pass before set
                 * it */
                cur = points + 1;
                *cur = vine->chain.tip;
                if (i > 0) {
                    line_draw(prev->x, prev->y, prev->z, cur->x, cur->y,
                              cur->z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(task);
                }
                /* @hack redundant: a block no path reaches, which stores
                 * degrees through prev */
                if (0) {
                    prev->x = vine->degrees;
                }
            }
            ++i;
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        part = (vine->degrees - vine->throw_degrees) /
                               (vine->end_degrees - vine->throw_degrees);
                        preview_swing =
                            njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            swing_most;
                    } else {
                        part = vine->degrees / vine->throw_degrees;
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                *prev = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    task, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                /* @hack redundant: cur set again, though the pass before set
                 * it */
                cur = points + 1;
                *cur = vine->chain.tip;
                if (i > 0) {
                    line_draw(prev->x, prev->y, prev->z, cur->x, cur->y,
                              cur->z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(task);
                }
                /* @hack redundant: a block no path reaches, which stores
                 * degrees through prev */
                if (0) {
                    prev->x = vine->degrees;
                }
            }
            ++i;
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        part = (vine->degrees - vine->throw_degrees) /
                               (vine->end_degrees - vine->throw_degrees);
                        /* @hack redundant: a conditional on part whose value
                         * nothing takes. it stands in for code the image
                         * doesn't hold, and is kept for the frame slots SHC
                         * then gives the preview */
                        (void)(part ? vine->speed : vine->swing_most);
                        preview_swing =
                            njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            swing_most;
                    } else {
                        part = vine->degrees / vine->throw_degrees;
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                *prev = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    task, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                /* @hack redundant: cur set again, though the pass before set
                 * it */
                cur = points + 1;
                *cur = vine->chain.tip;
                if (i > 0) {
                    line_draw(prev->x, prev->y, prev->z, cur->x, cur->y,
                              cur->z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(task);
                }
                /* @hack redundant: a block no path reaches, which stores
                 * degrees through prev */
                if (0) {
                    prev->x = vine->degrees;
                }
            }
            ++i;
        }
        /* @hack redundant: a block no path reaches, which stores degrees
         * through prev and cur */
        if (0) {
            prev->x = vine->degrees;
            cur->x = vine->degrees;
            cur->y = vine->degrees;
            cur->z = vine->degrees;
        }
    }
}

static void revolving_vine_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct revolving_vine *vine = task->extra.p;

    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        (void)"<- ANGLE ( TUTA WIDTH )";
        (void)"<- ANGLE ( ROTY )";
        (void)"<- ADD SPEED ";
        (void)"<- ROT RADIUS";
        (void)"<- TUTA LENGTH";
        (void)"<- NUM OF REVOLUTION";
        (void)"%3.2f";
    }
    if (player_near(&work->pos, NEAR_RANGE)) {
        njSetTexture(&revolving_vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &vine->chain.pos);
        njRotateY(NULL, work->ang.y);
        njRotateY(NULL, vine->turn);
        njTranslate(NULL, work->scl.x, 0.0F, 0.0F);
        njRotateZ(NULL, -QUARTER_TURN);
        njCnkEasyDrawObject(vine->chain.objects);
        njPopMatrix(1);
    }
}

static void revolving_vine_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct revolving_vine *vine = self->extra.p;

    syFree(vine->chain.objects);
    syFree(vine);
    self->extra.p = NULL;
}
