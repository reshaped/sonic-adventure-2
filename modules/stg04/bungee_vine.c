/* White Jungle's bungee vine, BANGIETUTA in the stage's object list: a vine
 * that hangs high above the ground. a player who catches its tip drops with
 * it, bounces on it and is thrown */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/vine.h"
#include "stg04/bungee_vine.h"
#include "stg04/revolving_vine.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

#include "stg04/bungee_vine/text.inc"

/* Each vine poses its own copy of this model's joint chain. */
static NJS_TEXNAME bungee_vine_textures[1] = {
#include "stg04/bungee_vine/bungee_vine_textures.inc"
};

NJS_TEXLIST white_jungle_bungee_vine_texlist = {
#include "stg04/bungee_vine/white_jungle_bungee_vine_texlist.inc"
};

#include "shared/bungee_vine_shape.h"

/* the vine's modes, in its work's mode */
enum bungee_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct bungee_vine {
    struct vine_chain chain;
    Sint32 unk_2c;
    Sint32 unk_30;
    Sint32 unk_34;
    NJS_VECTOR unk_38;
    Uint8 unk_44[0x10];
    /* how far above the vine's top the chain hangs from. it is TOP_HEIGHT
     * less fall, held to the work's scl.z once the vine has reached its
     * first bottom */
    Float lift;
    /* how far TOP_HEIGHT less fall passes scl.z, where lift is held to it.
     * the chain ripples with it */
    Float slack;
    /* how far the chain has fallen from TOP_HEIGHT above the top, where the
     * vine's spring rests */
    Float fall;
    Float fall_speed;
    /* how far the vine sways along x and z, and how fast */
    Float sway_x;
    Float sway_z;
    Float sway_speed_x;
    Float sway_speed_z;
};

/* the slot of the player on the vine */
#define PLAYER(work) ((work)->submode)
/* the placement's state the start sets. the vine ends once it is 0 */
#define STATE_LIVE 1

/* the vine's top lies TOP_HEIGHT above its work's pos */
#define TOP_HEIGHT 2600.0F
/* the vine stops at its bottom, where its fall speed turns 0. the work's
 * timer counts its bottoms, up to FIRST_BOTTOMS. each of those is a fall of
 * TOP_HEIGHT, and any later bottom a fall of TOP_HEIGHT less scl.z */
#define BOTTOMS(work) ((work)->timer)
#define FIRST_BOTTOMS 3
/* the vine turns back down at a fall of TOP_HEIGHT less scl.x before its
 * first bottom. after it, the vine turns back down at a fall of TOP_HEIGHT
 * less scl.z and BOUNCE_OVER */
#define BOUNCE_OVER 150.0F
/* the share of the fall speed the vine keeps as the speed reverses. it keeps
 * LET_GO_KEEP as it throws its player. it keeps all of it as it turns back
 * down. after its first bottom, it keeps only BOUNCE_KEEP there while the
 * chain's timer is above 0 */
#define BOUNCE_KEEP 0.7F
#define LET_GO_KEEP 0.8F
/* until the vine's first bottom, its spring pulls at SPRING_SLACK of its
 * strength */
#define SPRING_SLACK 0.1F
/* the sway's speed leans the ripple parts by LEAN_TURN a unit. the sway turns
 * the sway parts by SWAY_TURN a unit. the ripple's size is RIPPLE_SIZE
 * degrees times the sine of SLACK_TURN a unit of slack */
#define LEAN_TURN (14.0F * ANGLE_PER_DEGREE)
#define SWAY_TURN (0.3F * ANGLE_PER_DEGREE)
#define SLACK_TURN (0.6F * ANGLE_PER_DEGREE)
#define RIPPLE_SIZE 90.0F
/* the throw's velocity. its x is ang.z's low half times THROW_AHEAD, and its
 * y is |fall_speed| plus scl.y. ang.y turns it */
#define THROW_AHEAD 0.1F
/* a catch sets the sway's speed to the player's speed along x and z, in the
 * vine's axes. a player slower than STILL_SPEED gives a random speed instead,
 * up to half NUDGE either way */
#define STILL_SPEED 0.0005F
#define NUDGE 0.001F
/* the sway's speed a vine starts with, along x, and negated along z */
#define START_SWAY_SPEED 0.001F
/* the vine moves and draws only while a player lies within NEAR_RANGE of
 * its work's pos */
#define NEAR_RANGE 4000.0F
/* a player catches the vine where its first shape's squared distance from the
 * tip is below CATCH_RANGE. the catch waits until the chain's timer has
 * counted CATCH_WAIT steps */
#define CATCH_RANGE (10.0F * 10.0F)
#define CATCH_WAIT 120
/* the work's tick, which picks the ride's animation. it is RIDE_NONE until
 * the vine's first bottom. a RIDE_SLOW step plays ANIM_BUNGEE_RIDE. where the
 * vine rises at RIDE_FAST_RISE or faster, it plays ANIM_BUNGEE_RIDE_A instead
 * and turns RIDE_FAST. a RIDE_FAST step plays ANIM_BUNGEE_RIDE_A. it turns
 * RIDE_SLOW where the vine rises at RIDE_SLOW_RISE or faster. so through a
 * fast rise the two alternate, and ANIM_BUNGEE_RIDE_A plays. the rises are in
 * white_jungle_vine_gravity a step */
#define RIDE(work) ((work)->tick)
#define RIDE_NONE 0
#define RIDE_SLOW 1
#define RIDE_FAST 2
#define RIDE_FAST_RISE 20.0F
#define RIDE_SLOW_RISE 3.0F
/* a press of A throws the player while the fall speed is at most THROW_FALL
 * times white_jungle_vine_gravity. once the vine's bottoms reach
 * THROW_BOTTOMS, it throws without A too. it then throws on any step where it
 * falls faster than the gravity, or has slack */
#define THROW_FALL 3.0F
#define THROW_BOTTOMS 2

/* the chain's parts, from its top. the sway turns the first SWAY_PARTS, and
 * the slack ripples the next RIPPLE_PARTS. the end part comes after them */
#define SWAY_PARTS 3
#define RIPPLE_PARTS 19
/* the ripple's wave runs ripple_step a part. its size falls off as the cosine
 * of RIPPLE_FADE a part */
#define RIPPLE_FADE 862

/* each step the sway's speed loses sway_pull times the sway. where the
 * square of the sway passes sway_reach, the speed keeps only sway_damping of
 * itself too. the sway is held to sway_max, and its speed to sway_speed_max */
static Float sway_pull = 0.002F;
static Float sway_damping = 0.99F;
static Float sway_reach = 0.2F;
static Float sway_max = 5.0F;
static Float sway_speed_max = 0.2F;
/* the fall speed the vine drops at from a catch */
static Float drop_speed = 2.0F;
/* the vine's spring: each step its fall speed loses stiffness times the
 * fall */
static Float stiffness = 0.00014F;
static Angle ripple_step = EIGHTH_TURN;
/* a hanging player lies at hang_offset from the tip, in the tip's axes.
 * player_turn adds to the angles that hang it along the tip's part */
static NJS_POINT3 hang_offset = {1.0F, -14.5F, 0.3F};
static Angle player_turn[3] = {0, 0, 0};
/* each ripple part twists about x by its share of twist */
static Angle twist = 0;

static void bungee_vine_start(struct task *task);
static void bungee_vine_run(struct task *task);
static void bungee_vine_draw(struct task *task);
static void bungee_vine_destroy(struct task *task);

static Float bungee_vine_distance_squared(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = from->x - to->x;
    Float dy = from->y - to->y;
    Float dz = from->z - to->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* the first of players 0 and 1 whose first shape's center lies within range
 * of pos. -1 is neither. range is the square of a distance */
static int bungee_vine_player_within(NJS_POINT3 *pos, Float range)
{
    if (player_work[0] &&
        bungee_vine_distance_squared(&player_work[0]->collision->shapes->center,
                                     pos) < range) {
        return 0;
    }
    if (player_work[1] &&
        bungee_vine_distance_squared(&player_work[1]->collision->shapes->center,
                                     pos) < range) {
        return 1;
    }
    return -1;
}

/* pose the chain whose first part is object. the sway parts share sway_z's
 * turn about z and sway_y's about y, and take scale along their length. each
 * ripple part turns by lean_z about z, and by lean_y and its bend about y.
 * the bends make a ripple of up to ripple degrees. the part after them, where
 * there is one, turns back about each axis. it turns by the sum of the other
 * parts' turns, less the ripple parts' leans */
static void bungee_vine_bend(NJS_CNK_OBJECT *object, Angle lean_y, Angle lean_z,
                             Angle sway_z, Angle sway_y, Float ripple,
                             Float scale)
{
    NJS_CNK_OBJECT *source;
    Angle step_y;
    Angle step_z;
    Angle sum_x;
    Angle sum_y;
    Angle sum_z;
    Angle step_x;
    Angle prev;
    Angle turn;
    int i;
    /* @hack spelling: fade, a local for RIPPLE_FADE, where RIPPLE_FADE in
     * place would do */
    Angle fade;

    source = &bungee_vine_object;
    /* @hack spelling: one assignment to the three, where a statement for each
     * would do */
    sum_z = sum_x = sum_y = 0;
    step_z = sway_z / SWAY_PARTS;
    step_y = sway_y / SWAY_PARTS;
    for (i = 0; i < SWAY_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        object->pos[0] = source->pos[0] * scale;
        object->ang[1] = step_y;
        object->ang[2] = step_z;
        sum_z += step_z;
        sum_y += step_y;
    }
    prev = 0;
    step_x = twist / RIPPLE_PARTS;
    fade = RIPPLE_FADE;
    for (i = 0; i < RIPPLE_PARTS;
         i++, object = object->child, source = source->child) {
        object->evalflags &= ~NJD_EVAL_UNIT_ANG;
        turn = DEGREES(njSin(ripple_step * i) * ripple *
                       njSin(QUARTER_TURN - (fade * i))) +
               lean_y - prev;
        prev += turn;
        object->ang[1] = lean_y + turn;
        object->ang[0] = step_x;
        object->ang[2] = lean_z;
        sum_y += turn;
        sum_x += step_x;
    }
    if (object) {
        object->ang[0] = -sum_x;
        object->ang[1] = -sum_y;
        object->ang[2] = -sum_z;
    }
}

/* set tip to the center of the count-th part's model, in the chain whose first
 * part is object. the chain stands where the draw puts it. ang_x and ang_z get
 * the turns about x and z that align a hanging player with that part. count
 * must be 1 or more */
static void bungee_vine_find_tip(struct task *task, NJS_CNK_OBJECT *object,
                                 int count, NJS_POINT3 *tip, Angle *ang_x,
                                 Angle *ang_z)
{
    struct task_work *work;
    struct bungee_vine *vine;
    NJS_CNK_MODEL *model;
    NJS_VECTOR dir;
    int i;

    work = task->work;
    vine = task->extra.p;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslate(NULL, vine->chain.pos.x, vine->chain.pos.y + vine->lift,
                vine->chain.pos.z);
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
    njCalcPoint(NULL, &model->center, tip);
    dir.x = 1.0F;
    dir.y = 0.0F;
    dir.z = 0.0F;
    njCalcVector(NULL, &dir, &dir);
    *ang_z = (Angle)(atan2f(dir.x, dir.y) * ANGLE_PER_RADIAN);
    *ang_x = (Angle)(atan2f(dir.z, njSqrt(1.0F - (dir.z * dir.z))) *
                     ANGLE_PER_RADIAN);
    njPopMatrix(1);
}

void white_jungle_bungee_vine_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        task->destructor = bungee_vine_destroy;
        task->draw = bungee_vine_draw;
        bungee_vine_start(task);
        break;
    case MODE_RUN:
        bungee_vine_run(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

static void bungee_vine_start(struct task *task)
{
    struct task_work *work = task->work;
    struct bungee_vine *vine;
    NJS_CNK_OBJECT *object;
    int count;
    int i;

    vine = syCalloc(1, sizeof(struct bungee_vine));
    task->extra.p = vine;
    count = 0;
    for (object = &bungee_vine_object; object; object = object->child) {
        /* @hack spelling: count = count + 1, where count++ would do */
        count = count + 1;
    }
    vine->chain.objects = syCalloc(count, sizeof(NJS_CNK_OBJECT));
    vine->chain.count = count;
    for (object = &bungee_vine_object, i = 0; object != NULL;
         object = object->child, i++) {
        vine->chain.objects[i] = *object;
        if (i != 0) {
            vine->chain.objects[i - 1].child = &vine->chain.objects[i];
        }
    }
    work->mode = MODE_RUN;
    vine->chain.timer = 0;
    vine->unk_2c = 0;
    vine->unk_34 = 0;
    vine->unk_30 = 0;
    vine->unk_38.x = 0.0F;
    vine->unk_38.y = 0.0F;
    vine->unk_38.z = 0.0F;
    vine->chain.pos = work->pos;
    vine->chain.pos.y += TOP_HEIGHT;
    /* @hack redundant: the store made again, though the one above zeroed
     * unk_30 */
    vine->unk_30 = 0;
    vine->lift = work->scl.x;
    vine->slack = 0.0F;
    vine->fall = -vine->lift + TOP_HEIGHT;
    vine->fall_speed = 0.0F;
    RIDE(work) = RIDE_NONE;
    BOTTOMS(work) = 0;
    vine->sway_x = 0.0F;
    vine->sway_z = 0.0F;
    vine->sway_speed_x = START_SWAY_SPEED;
    vine->sway_speed_z = -START_SWAY_SPEED;
    if (task->placement) {
        placement_set_state(task, STATE_LIVE);
    }
}

static void bungee_vine_run(struct task *task)
{
    struct task_work *work = task->work;
    struct bungee_vine *vine = task->extra.p;
    Float one;
    struct task *player;
    NJS_VECTOR dir;
    /* @hack redundant: matrix and ang, locals nothing reads */
    NJS_MATRIX matrix;
    struct angles ang;
    Float reach;
    Float speed;
    Sint16 z;
    int i;

    /* @hack redundant: a block no path reaches, which calls njSin */
    if (0) {
        njSin(0);
    }
    /* @hack spelling: one, a local for 1.0F, where 1.0F in place would do */
    one = 1.0F;
    /* @hack redundant: a block no path reaches, of matrix calls, stores to the
     * chain's first part, a task_kill and a store to fall */
    if (0) {
        NJS_CNK_OBJECT *object;

        njSin(0);
        object = vine->chain.objects;
        object->pos[0] = 0.0F;
        object->pos[1] = 0.0F;
        njPopMatrix(1);
        object->pos[2] = 0.0F;
        object->scl[0] = 0.0F;
        njPopMatrix(1);
        object->scl[1] = 0.0F;
        object->scl[2] = 0.0F;
        njPushMatrix(NULL);
        object->pos[0] = 0.0F;
        object->pos[1] = one;
        njPushMatrix(NULL);
        object->pos[2] = one;
        object->scl[0] = one;
        njUnitMatrix(NULL);
        object->scl[1] = one;
        object->scl[2] = one;
        object->pos[0] = 2600.0F;
        object->pos[1] = 0.1F;
        object->pos[2] = 0.1F;
        object->ang[0] = 0;
        task_kill(task);
        vine->fall = work->pos.y;
        return;
    }
    /* @hack redundant: a block no path reaches, which adds to unk_30 */
    if (0) {
        vine->unk_30 += 0x1000;
    }
    if (task->placement && !placement_get_state(task)) {
        task_kill(task);
        return;
    }
    if (!player_near(&work->pos, NEAR_RANGE)) {
        return;
    }
    if (vine->sway_speed_x > sway_speed_max) {
        vine->sway_speed_x = sway_speed_max;
    }
    if (vine->sway_speed_x < -sway_speed_max) {
        vine->sway_speed_x = -sway_speed_max;
    }
    if (vine->sway_speed_z > sway_speed_max) {
        vine->sway_speed_z = sway_speed_max;
    }
    if (vine->sway_speed_z < -sway_speed_max) {
        vine->sway_speed_z = -sway_speed_max;
    }
    vine->sway_x += vine->sway_speed_x;
    vine->sway_z += vine->sway_speed_z;
    reach = (vine->sway_x * vine->sway_x) + (vine->sway_z * vine->sway_z);
    if (reach > sway_reach) {
        vine->sway_speed_x =
            (vine->sway_speed_x * sway_damping) - (vine->sway_x * sway_pull);
        vine->sway_speed_z =
            (vine->sway_speed_z * sway_damping) - (vine->sway_z * sway_pull);
    } else {
        vine->sway_speed_x = vine->sway_speed_x - (vine->sway_x * sway_pull);
        vine->sway_speed_z = vine->sway_speed_z - (vine->sway_z * sway_pull);
    }
    if (vine->sway_x > sway_max) {
        vine->sway_x = sway_max;
    }
    if (vine->sway_x < -sway_max) {
        vine->sway_x = -sway_max;
    }
    if (vine->sway_z > sway_max) {
        vine->sway_z = sway_max;
    }
    if (vine->sway_z < -sway_max) {
        vine->sway_z = -sway_max;
    }
    {
        /* @hack spelling: fall_speed, a local for vine->fall_speed, where the
         * field read in place would do */
        Float fall_speed = vine->fall_speed;

        vine->fall += fall_speed;
    }
    if (BOTTOMS(work) < FIRST_BOTTOMS) {
        if (vine->fall > TOP_HEIGHT) {
            vine->fall = TOP_HEIGHT;
            if (RIDE(work) == RIDE_NONE) {
                RIDE(work) = RIDE_SLOW;
            }
            vine->fall_speed = 0.0F;
            BOTTOMS(work)++;
        }
    } else if (vine->fall > TOP_HEIGHT - work->scl.z) {
        vine->fall = TOP_HEIGHT - work->scl.z;
        if (RIDE(work) == RIDE_NONE) {
            RIDE(work) = RIDE_SLOW;
        }
        vine->fall_speed = 0.0F;
    }
    if (RIDE(work) != RIDE_NONE) {
        if ((TOP_HEIGHT - BOUNCE_OVER) - work->scl.z > vine->fall) {
            vine->fall = (TOP_HEIGHT - BOUNCE_OVER) - work->scl.z;
            if (vine->chain.timer > 0) {
                vine->fall_speed = -(vine->fall_speed * BOUNCE_KEEP);
            } else {
                vine->fall_speed = -vine->fall_speed;
            }
        }
    } else if (TOP_HEIGHT - work->scl.x > vine->fall) {
        vine->fall = TOP_HEIGHT - work->scl.x;
        vine->fall_speed = -vine->fall_speed;
    }
    if (RIDE(work) != RIDE_NONE) {
        if (TOP_HEIGHT - work->scl.z > vine->fall) {
            vine->lift = work->scl.z;
            vine->slack = -work->scl.z - vine->fall + TOP_HEIGHT;
        } else {
            vine->lift = TOP_HEIGHT - vine->fall;
            vine->slack = 0.0F;
        }
    } else {
        vine->lift = TOP_HEIGHT - vine->fall;
        vine->slack = 0.0F;
    }
    /* @hack redundant: a block no path reaches, which stores a number in
     * fall */
    if (0) {
        vine->fall = 0.3F * ANGLE_PER_DEGREE;
    }
    bungee_vine_bend(
        vine->chain.objects, (Angle)(vine->sway_speed_z * LEAN_TURN),
        (Angle)(vine->sway_speed_x * LEAN_TURN),
        (Angle)(vine->sway_x * SWAY_TURN), (Angle)(vine->sway_z * SWAY_TURN),
        njSin((Angle)(vine->slack * SLACK_TURN)) * RIPPLE_SIZE, one);
    bungee_vine_find_tip(task, vine->chain.objects, vine->chain.count - 1,
                         &vine->chain.tip, &vine->chain.tip_ang_x,
                         &vine->chain.tip_ang_z);
    /* @hack redundant: an empty else */
    if (vine->chain.timer < 0) {
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njTranslateV(NULL, &vine->chain.tip);
        njRotateZ(NULL, -vine->chain.tip_ang_z);
        njRotateX(NULL, vine->chain.tip_ang_x + HALF_TURN);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &hang_offset, &dir);
        njPopMatrix(1);
        /* @bug logic: the ride never lets a missed player go, as the
         * revolving vine's carry doesn't. the vine holds the player at its tip
         * at 0x8c50193e, and bounces it or throws it. Green Forest's bungee
         * vine tests PLAYER_MISSED at 0x8c502fcc, and lets its player go */
        player_set_pos(PLAYER(work), dir.x, dir.y, dir.z);
        if (RIDE(work) != RIDE_NONE) {
            if (RIDE(work) == RIDE_SLOW) {
                if (vine->fall_speed <=
                    -(white_jungle_vine_gravity * RIDE_FAST_RISE)) {
                    player_request(PLAYER(work), PLAYER_REQUEST_ANIM,
                                   ANIM_BUNGEE_RIDE_A);
                    RIDE(work) = RIDE_FAST;
                } else {
                    player_request(PLAYER(work), PLAYER_REQUEST_ANIM,
                                   ANIM_BUNGEE_RIDE);
                }
            } else {
                if (vine->fall_speed <=
                    -(white_jungle_vine_gravity * RIDE_SLOW_RISE)) {
                    RIDE(work) = RIDE_SLOW;
                }
                player_request(PLAYER(work), PLAYER_REQUEST_ANIM,
                               ANIM_BUNGEE_RIDE_A);
            }
        }
        /* @hack redundant: a block no path reaches, of two calls of
         * player_request */
        if (0) {
            player_request(0, 0, 0);
            player_request(0, 0, 0);
        }
        player_work[PLAYER(work)]->ang.x =
            vine->chain.tip_ang_x + player_turn[0] + HALF_TURN;
        player_work[PLAYER(work)]->ang.y =
            -work->ang.y + player_turn[1] + HALF_TURN;
        player_work[PLAYER(work)]->ang.z =
            -vine->chain.tip_ang_z + player_turn[2];
        /* @hack spelling: the fall's and the slack's tests joined by |,
         * where || would do */
        if ((vine->fall_speed <= white_jungle_vine_gravity * THROW_FALL &&
             (player_pad_records[PLAYER(work)].press & PDD_DGT_TA)) ||
            (((vine->fall_speed > white_jungle_vine_gravity) |
              (vine->slack > 0.0F)) &&
             BOTTOMS(work) >= THROW_BOTTOMS)) {
            /* @hack type: ang.z's low half read through a Sint16 pointer,
             * where a cast of ang.z to Sint16 would do */
            z = *(Sint16 *)&work->ang.z;
            dir.x = (Float)z * THROW_AHEAD;
            dir.y = _builtin_fabsf(vine->fall_speed) + work->scl.y;
            dir.z = 0.0F;
            speed = njScalor(&dir);
            njPushMatrix(NULL);
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &dir, &dir);
            njPopMatrix(1);
            njUnitVector(&dir);
            player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
            player_fling(PLAYER(work), speed, &dir);
            vine->chain.timer = 0;
            vine->fall_speed *= -LET_GO_KEEP;
        }
        /* @hack redundant: a block no path reaches, which stores
         * white_jungle_vine_gravity in fall_speed */
        if (0) {
            vine->fall_speed = white_jungle_vine_gravity;
        }
        vine->fall_speed += white_jungle_vine_gravity;
    } else {
    }
    if (RIDE(work) != RIDE_NONE) {
        vine->fall_speed += -stiffness * vine->fall;
    } else {
        vine->fall_speed += -stiffness * vine->fall * SPRING_SLACK;
    }
    if (vine->chain.timer >= 0) {
        i = bungee_vine_player_within(&vine->chain.tip, CATCH_RANGE);
        if (i >= 0 && vine->chain.timer >= CATCH_WAIT) {
            player = player_tasks[i];
            njPushMatrix(NULL);
            njUnitMatrix(NULL);
            njRotateY(NULL, -work->ang.y);
            njCalcPoint(NULL, &((struct player_motion *)player->motion.p)->spd,
                        &dir);
            njPopMatrix(1);
            dir.y = 0.0F;
            if (njScalor(&dir) < STILL_SPEED) {
                dir.x = (((Float)rand() * RAND_SCALE) - 0.5F) * NUDGE;
                /* @hack redundant: a block no path reaches, which stores
                 * -0.5F in dir.x */
                if (0) {
                    dir.x = -0.5F;
                }
                dir.z = (((Float)rand() * RAND_SCALE) - 0.5F) * NUDGE;
            }
            vine->sway_speed_x = dir.x;
            vine->sway_speed_z = dir.z;
            vine->fall_speed = drop_speed;
            vine->chain.timer = -1;
            PLAYER(work) = i;
            player_request(i, PLAYER_REQUEST_ANIM, ANIM_BUNGEE_CATCH);
        }
    }
    if (vine->chain.timer >= 0 && vine->chain.timer < CATCH_WAIT) {
        vine->chain.timer++;
    }
}

static void bungee_vine_draw(struct task *task)
{
    struct task_work *work;
    struct bungee_vine *vine;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. they come from the SET
     * editor */
    if (0) {
        (void)"%08.2f";
        (void)"<- ANGLE ( ROT POS Y )";
        (void)"<- ADD X SPD ";
        (void)"<- TUTA OFS(START)";
        (void)"<- ADD Y SPD";
        (void)"<- TUTA OFF(JUMP)";
        (void)"%3.2f";
    }
    work = task->work;
    vine = task->extra.p;
    if (player_near(&work->pos, NEAR_RANGE)) {
        njSetTexture(&white_jungle_bungee_vine_texlist);
        njPushMatrix(NULL);
        njTranslate(NULL, vine->chain.pos.x, vine->chain.pos.y + vine->lift,
                    vine->chain.pos.z);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, -QUARTER_TURN);
        njCnkEasyDrawObject(vine->chain.objects);
        njPopMatrix(1);
    }
}

static void bungee_vine_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct bungee_vine *vine = self->extra.p;

    syFree(vine->chain.objects);
    syFree(vine);
    self->extra.p = NULL;
}
