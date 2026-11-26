/* White Jungle's revolving vine, ROLTUTA in the stage's object list: a vine
 * whose top runs round a circle. a player who catches its tip is carried round
 * as the vine swings out and back. the vine then throws it. Green Forest's
 * vine of shared/revolving_vine.c is its twin, which a time stop halts and a
 * throw ends */
#include "stg04/revolving_vine/text.inc"
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/vine.h"
#include "sound/sound.h"
#include "stg04/revolving_vine.h"
#include "system/angle.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* the vine's modes, in its work's mode */
enum revolving_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct revolving_vine {
    struct vine_chain chain;
    Angle turn; /* how far the top has run round its circle */
    Angle swing; /* swing_degrees as an Angle */
    /* how far the vine swings out, in degrees, and how much that changed on
     * the last step */
    Float swing_degrees;
    Float swing_speed;
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

/* the steps a player has hung from the tip, up to HANG_STEPS_MOST */
/* @hack type: unk_3c read as a Sint16 through its address, where
 * (task)->unk_3c.w[0] would do */
#define HANG_STEPS(task) (*(Sint16 *)&(task)->unk_3c)

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
/* the sound of a hanging player, of the stage's sound list. it plays once the
 * player has hung for HANG_QUIET_STEPS. its key is the work's address, moved a
 * byte on every HANG_SOUND_KEY_STEPS steps. sound_play_for gives a new key a
 * slot of its own, so the sound starts anew. the key stops moving once
 * HANG_STEPS reaches HANG_STEPS_MOST */
#define HANG_SOUND 1
#define HANG_SOUND_PRIORITY 1
#define HANG_SOUND_VOLUME 0
#define HANG_SOUND_DURATION 10
#define HANG_QUIET_STEPS 30
#define HANG_SOUND_KEY_STEPS 120
#define HANG_STEPS_MOST 600
/* the pad shakes at the catch and at the throw */
#define CATCH_POWER 2
#define CATCH_FREQ 15
#define CATCH_TIME 1
#define THROW_POWER 5
#define THROW_FREQ 30

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
 * between throw_min and throw_max. the throw goes at the vine's speed, so the
 * three bound it too */
static Float throw_max = 10.0F;
static Float throw_min = 3.0F;
static Float throw_scale = 1.0F;

/* the vine's texture list and model, a chain of chunk objects from
 * vine_part_0. the source defines them, as they lie among the data the code
 * reads. part 0 caches the whole vine's polygons. parts 2 to 6 set the
 * vertices they use, and part 6 draws them */
static NJS_TEXNAME vine_texnames[1] = {
#include "stg04/revolving_vine/vine_texnames.inc"
};
NJS_TEXLIST white_jungle_revolving_vine_texlist = {
#include "stg04/revolving_vine/white_jungle_revolving_vine_texlist.inc"
};
static NJS_CNK_OBJECT vine_part_7 = {
#include "stg04/revolving_vine/vine_part_7.inc"
};
static Sint16 vine_part_6_polygons[2] = {
#include "stg04/revolving_vine/vine_part_6_polygons.inc"
};
static Sint32 vine_part_6_vertices[53] = {
#include "stg04/revolving_vine/vine_part_6_vertices.inc"
};
static NJS_CNK_MODEL vine_part_6_model = {
#include "stg04/revolving_vine/vine_part_6_model.inc"
};
static NJS_CNK_OBJECT vine_part_6 = {
#include "stg04/revolving_vine/vine_part_6.inc"
};
static Sint32 vine_part_5_vertices[68] = {
#include "stg04/revolving_vine/vine_part_5_vertices.inc"
};
static NJS_CNK_MODEL vine_part_5_model = {
#include "stg04/revolving_vine/vine_part_5_model.inc"
};
static NJS_CNK_OBJECT vine_part_5 = {
#include "stg04/revolving_vine/vine_part_5.inc"
};
static Sint32 vine_part_4_vertices[68] = {
#include "stg04/revolving_vine/vine_part_4_vertices.inc"
};
static NJS_CNK_MODEL vine_part_4_model = {
#include "stg04/revolving_vine/vine_part_4_model.inc"
};
static NJS_CNK_OBJECT vine_part_4 = {
#include "stg04/revolving_vine/vine_part_4.inc"
};
static Sint32 vine_part_3_vertices[89] = {
#include "stg04/revolving_vine/vine_part_3_vertices.inc"
};
static NJS_CNK_MODEL vine_part_3_model = {
#include "stg04/revolving_vine/vine_part_3_model.inc"
};
static NJS_CNK_OBJECT vine_part_3 = {
#include "stg04/revolving_vine/vine_part_3.inc"
};
static Sint32 vine_part_2_vertices[101] = {
#include "stg04/revolving_vine/vine_part_2_vertices.inc"
};
static NJS_CNK_MODEL vine_part_2_model = {
#include "stg04/revolving_vine/vine_part_2_model.inc"
};
static NJS_CNK_OBJECT vine_part_2 = {
#include "stg04/revolving_vine/vine_part_2.inc"
};
static NJS_CNK_OBJECT vine_part_1 = {
#include "stg04/revolving_vine/vine_part_1.inc"
};
static Sint16 vine_part_0_polygons[207] = {
#include "stg04/revolving_vine/vine_part_0_polygons.inc"
};
static NJS_CNK_MODEL vine_part_0_model = {
#include "stg04/revolving_vine/vine_part_0_model.inc"
};
static NJS_CNK_OBJECT vine_part_0 = {
#include "stg04/revolving_vine/vine_part_0.inc"
};

/* a hanging player lies at hang_offset from the tip, in the tip's axes.
 * player_turn adds to the angles that hang it along the tip's part */
static NJS_POINT3 hang_offset = {0.0F, 0.0F, 0.0F};
/* @bug logic: the player hangs at the tip itself, and player_turn turns it by
 * -2 and -7 Angles about x and y. Green Forest's twin has a hang_offset of
 * (-2, -7, 0) and a player_turn of 0, so the two numbers likely belong in
 * hang_offset. player_turn lies at 0x8c5a1ff8 */
static Angle player_turn[3] = {-2, -7, 0};
static Float dat_8c5a2004 = 0.015F;
static Float dat_8c5a2008 = 0.95F;
static Float dat_8c5a200c = -10.0F;
static Float dat_8c5a2010 = 0.06F;
Float white_jungle_vine_gravity = 0.2F;
/* the parts after the first lag the vine's motion. their turn about y
 * follows the vine's speed, scaled by lag_y. their turn about z follows
 * swing_speed, scaled by lag_z */
static Float lag_y = -2.0F;
static Float lag_z = -25.0F;

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
 * first part is object. the chain stands where the draw puts it, with radius
 * for scl.x and turn for the vine's turn. ang_x and ang_z get the turns about x
 * and z that align a hanging player with that part. nothing reads swing. count
 * must be 1 or more */
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

void white_jungle_revolving_vine_init(struct task *task)
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
    vine->chain.timer = 0;
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
    /* @hack spelling: owner, a copy of task that every use reads, where task
     * would do */
    struct task *owner = task;
    struct task_work *work = owner->work;
    struct revolving_vine *vine = owner->extra.p;
    NJS_VECTOR point;
    NJS_VECTOR dir;
    Float step;
    Float swing;
    Float scale;
    Float throw_speed;
    Float added;
    Sint16 bias;
    int player;
    struct task *player_task;

    /* @hack redundant: a block no path reaches, which draws a line and
     * calls njSin */
    if (0) {
        line_draw(1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F);
        njSin(0);
    }
    /* @hack redundant: an if (1) around the step, whose else holds a debug
     * preview no path reaches. SHC keeps the preview's loop */
    if (1) {
        if (owner->placement && !placement_get_state(owner)) {
            task_kill(owner);
            return;
        }
        if (!player_near(&work->pos, NEAR_RANGE)) {
            return;
        }
        if (vine->chain.timer < 0) {
            if (HANG_STEPS(owner) < HANG_STEPS_MOST) {
                ++HANG_STEPS(owner);
            }
            if (HANG_STEPS(owner) >= HANG_QUIET_STEPS) {
                sound_play_for(HANG_SOUND,
                               (char *)work +
                                   ((HANG_STEPS(owner) - HANG_QUIET_STEPS) /
                                    HANG_SOUND_KEY_STEPS),
                               HANG_SOUND_PRIORITY, HANG_SOUND_VOLUME,
                               HANG_SOUND_DURATION);
            }
        } else {
            HANG_STEPS(owner) = 0;
        }

        step = vine->speed / (work->scl.x * TWO_PI_ROUNDED) * DEGREES_PER_TURN;
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
        /* @hack spelling: scale set in a comma before the call, where a
         * statement of its own would do */
        scale = work->scl.y + 1.0F,
        revolving_vine_pose(
            vine->chain.objects,
            vine->speed / (work->scl.x * TWO_PI_ROUNDED) * lag_y * FULL_TURN,
            vine->swing_speed / (scale * (TWO_PI_ROUNDED * LENGTH)) * lag_z *
                FULL_TURN,
            vine->swing, scale);
        revolving_vine_find_tip(owner, vine->chain.objects,
                                vine->chain.count - 1, work->scl.x, vine->turn,
                                vine->swing, &vine->chain.tip,
                                &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
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
                vine->degrees = 0.0F;
            }
        } else {
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
            /* @bug logic: the carry never lets a missed player go. a time
             * limit that runs out once player 0 has no life left sets
             * PLAYER_MISSED at 0x8c056214. the fade to the game over then
             * steps the tasks on. the vine holds the player at its tip at
             * 0x8c502a32, and carries it round or throws it. Green Forest's
             * vine tests the flag at 0x8c506344, and lets its player go */
            player_set_pos(PLAYER(work), point.x, point.y, point.z);
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
                }
                vine->chain.timer = 0;
            }
        }
        /* @hack redundant: a block no path reaches, which turns about y,
         * pops a matrix and turns about y again */
        if (0) {
            njRotateY(NULL, work->ang.y + vine->turn);
            njPopMatrix(1);
            njRotateY(NULL, work->ang.y + vine->turn);
        }
        if (vine->chain.timer >= 0 && vine->chain.timer < CATCH_WAIT) {
            ++vine->chain.timer;
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

        preview_step =
            vine->speed / (work->scl.x * TWO_PI_ROUNDED) * DEGREES_PER_TURN;
        turn_step = preview_step * ANGLE_PER_DEGREE;
        swing_most = vine->swing_most;
        vine->degrees = 0.0F;
        i = 0;
        points[0] = vine->chain.tip;
        /* @hack redundant: five tests, seven copies of the tip and two draws
         * of a line, which the image doesn't hold */
        if (swing_most) {
            turn_step = 0;
        }
        if (&njSin) {
            turn_step = 0;
        }
        if (&njSin) {
            turn_step = 0;
        }
        if (&njSin) {
            turn_step = 0;
        }
        points[0] = vine->chain.tip;
        points[0] = vine->chain.tip;
        points[0] = vine->chain.tip;
        points[0] = vine->chain.tip;
        points[0] = vine->chain.tip;
        if (i) {
            turn_step = 0;
        }
        points[0] = vine->chain.tip;
        points[0] = vine->chain.tip;
        line_draw(points[0].x, points[0].y, points[0].z, points[1].x,
                  points[1].y, points[1].z);
        line_draw(points[0].x, points[0].y, points[0].z, points[1].x,
                  points[1].y, points[1].z);
        /* @hack spelling: the loop's pass written out four times, as SHC
         * unrolls it, where the pass written once would do */
        for (; i < PREVIEW_STEPS;) {
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
                         * nothing takes */
                        (void)(part ? vine->speed : vine->swing_most);
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                points[0] = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    owner, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                points[1] = vine->chain.tip;
                if (i > 0) {
                    line_draw(points[0].x, points[0].y, points[0].z,
                              points[1].x, points[1].y, points[1].z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(owner);
                }
            }
            ++i;
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        /* @hack spelling: throw_at, a copy of throw_degrees,
                         * where the field read in place would do */
                        Float throw_at = vine->throw_degrees;

                        part = (vine->degrees - throw_at) /
                               (vine->end_degrees - throw_at);
                        preview_swing =
                            njSin(((part * SWING_FALL) + SWING_RISE) *
                                  ANGLE_PER_DEGREE) *
                            swing_most;
                    } else {
                        part = vine->degrees / vine->throw_degrees;
                        /* @hack redundant: two conditionals on part whose
                         * values nothing takes */
                        (void)(part ? vine->speed : vine->swing_most);
                        (void)(part ? vine->speed : vine->swing_most);
                        preview_swing =
                            njSin(part * (SWING_RISE * ANGLE_PER_DEGREE)) *
                            swing_most;
                    }
                }
                vine->swing = preview_swing * ANGLE_PER_DEGREE;
                vine->turn += turn_step;
                points[0] = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    owner, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                points[1] = vine->chain.tip;
                if (i > 0) {
                    line_draw(points[0].x, points[0].y, points[0].z,
                              points[1].x, points[1].y, points[1].z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(owner);
                }
            }
            ++i;
            {
                Float part;

                vine->degrees += _builtin_fabsf(preview_step);
                if (vine->degrees < vine->end_degrees) {
                    if (vine->degrees > vine->throw_degrees) {
                        /* @hack spelling: throw_at, a copy of throw_degrees,
                         * where the field read in place would do */
                        Float throw_at = vine->throw_degrees;

                        part = (vine->degrees - throw_at) /
                               (vine->end_degrees - throw_at);
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
                points[0] = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    owner, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                points[1] = vine->chain.tip;
                if (i > 0) {
                    line_draw(points[0].x, points[0].y, points[0].z,
                              points[1].x, points[1].y, points[1].z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(owner);
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
                points[0] = vine->chain.tip;
                revolving_vine_pose(vine->chain.objects, 0, 0, vine->swing,
                                    work->scl.y + 1.0F);
                revolving_vine_find_tip(
                    owner, vine->chain.objects, vine->chain.count - 1,
                    work->scl.x, vine->turn, vine->swing, &vine->chain.tip,
                    &vine->chain.tip_ang_x, &vine->chain.tip_ang_z);
                points[1] = vine->chain.tip;
                if (i > 0) {
                    line_draw(points[0].x, points[0].y, points[0].z,
                              points[1].x, points[1].y, points[1].z);
                }
                if ((i & (PREVIEW_DRAW_EVERY - 1)) == 0) {
                    revolving_vine_draw(owner);
                }
            }
            ++i;
        }
    }
}

static void revolving_vine_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct revolving_vine *vine = task->extra.p;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
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
        njSetTexture(&white_jungle_revolving_vine_texlist);
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
    /* @hack spelling: owner, a copy of task, where task would do */
    struct task *owner = task;
    struct revolving_vine *vine = owner->extra.p;

    syFree(vine->chain.objects);
    syFree(vine);
    owner->extra.p = NULL;
}
