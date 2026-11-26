#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/parts.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* a part's modes, in its work's mode: its POSES poses, then TRANSITION.
 * TRANSITION turns the part toward next_mode's pose */
enum part_mode { SWAY, TRACK, FOLDED, SPIN, POSES, TRANSITION };
/* a part's record, in its task's motion. the part tilts its node by tilt
 * toward heading */
struct part_motion {
    Sint8 side;
    Sint8 unused_01;
    Sint8 next_mode;
    Sint8 unused_03;
    Angle heading;
    Angle phase; /* SWAY's phase, or SPIN's heading */
    Angle rest; /* its side's rest, which nothing reads */
    Angle tilt;
    Angle target_tilt; /* the tilt and heading TRANSITION turns toward */
    Angle target_heading;
    struct angles saved; /* object's ang at the part's creation */
    NJS_CNK_OBJECT *object;
    const struct task_work *boss;
    const Sint8 *boss_submode;
    const Angle *boss_heading;
    const NJS_POINT3 *position;
};

/* each pose's tilt, and SPIN's first heading. nothing reads mode */
struct part_pose {
    Sint32 mode;
    Angle tilt;
    Angle heading;
};
/* SWAY swings heading up to SWAY_ARC either side of the part's rest. its
 * phase gains SWAY_STEP a step */
enum sway_setting { SIDE_0_REST, SIDE_1_REST, SWAY_ARC, SWAY_STEP };
/* player 0 as a part sees it. yaw is the short turn from the boss's ang.y to
 * the heading toward player 0. pitch is the angle of player 0 above or below
 * the part's position */
struct target_direction {
    Angle yaw;
    Angle pitch;
    Angle unused;
};

static Angle sway[] = {-0x800, 0x800, 0x1800, 0x300};
/* SPIN's turn of heading a step. only spin_steps[0] is read */
static Angle spin_steps[6] = {0x400};
static struct part_pose poses[POSES] = {
    {SWAY, 0x1800, 0}, {TRACK, 0x2000, 0}, {FOLDED, 0, 0}, {SPIN, 0x1000, 0}};

static void part_step(struct task *task);
static void part_destroy(struct task *task);
static void write_angles(struct task *task, const Angle *src, Sint8 side);

struct task *bigbogy_part_create(struct task *parent,
                                 const struct bigbogy_part *spec)
{
    struct task *task;
    struct task_work *work;
    struct part_motion *part;
    struct angles angles;
    if (parent) {
        task = task_create_child(TASK_WORK, part_step, parent);
        if (!task) {
            return NULL;
        }
        work = task->work;
        part = syCalloc(1, sizeof(*part));
        if (!part) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = part;
        part->object = spec->object;
        angles.x = spec->object->ang[0];
        angles.y = spec->object->ang[1];
        angles.z = spec->object->ang[2];
        work->ang = angles;
        work->ang.y = 0;
        part->saved = angles;
        part->side = spec->side;
        if (!part->side) {
            /* @hack redundant: sway[SIDE_0_REST] set to the value it holds */
            sway[SIDE_0_REST] = -0x800;
            part->rest = sway[SIDE_0_REST];
        } else {
            /* @hack redundant: sway[SIDE_1_REST] set to the value it holds */
            sway[SIDE_1_REST] = 0x800;
            part->rest = sway[SIDE_1_REST];
        }
        part->position = spec->position;
        part->boss = spec->boss;
        part->boss_submode = spec->boss_submode;
        part->boss_heading = spec->boss_heading;
        work->mode = SWAY;
        part->tilt = poses[SWAY].tilt;
        task->destructor = part_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where parent is null.
     * the branch at 0x8c506d50 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void part_destroy(struct task *task)
{
    struct part_motion *part = task->motion.p;
    write_angles(task, &part->saved.x, 0);
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* copy the three angles at src to object's ang, for a side of 0 or 1. every
 * call passes 0 */
static void write_angles(struct task *task, const Angle *src, Sint8 side)
{
    struct part_motion *part = task->motion.p;
    Angle *dst;
    Sint8 i;
    if (side >= 0 && side <= 1) {
#ifdef NONMATCHING
        /* the hack's plain form, which reads src as the struct angles it points
         * to */
        const struct angles *angles = (const struct angles *)src;
        part->object->ang[0] = angles->x;
        part->object->ang[1] = angles->y;
        part->object->ang[2] = angles->z;
#else
        /* @hack type: the angles walked as an array, where a copy of each
         * field would do */
        dst = part->object->ang;
        for (i = 0; i < 3; i++) {
            *dst = *src;
            dst++;
            src++;
        }
#endif
    }
}

static struct target_direction player_direction(struct task *task)
{
    struct part_motion *part = task->motion.p;
    struct target_direction direction;
    /* @hack redundant: unused, which only sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR delta;
    /* @hack spelling: x and z, copies of delta's, where delta's would do */
    Float x;
    Float z;
    Angle yaw;
    (void)unused;
    delta.x = player_work[0]->pos.x - part->position->x;
    delta.y = player_work[0]->pos.y - part->position->y;
    delta.z = player_work[0]->pos.z - part->position->z;
    yaw = (Angle)(atan2f(delta.x, delta.z) * ANGLE_PER_RADIAN);
    direction.yaw = angle_difference(*part->boss_heading, yaw);
    x = delta.x;
    z = delta.z;
    direction.pitch =
        (Angle)(atan2f(_builtin_fabsf(delta.y), njSqrt((x * x) + (z * z))) *
                ANGLE_PER_RADIAN);
#ifdef NONMATCHING
    /* this side returns no unset field */
    direction.unused = 0;
#endif
    return direction;
}

static void transition_to(struct task *task, Sint8 mode)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    struct target_direction direction;
    Angle rest;
    if (mode >= TRANSITION || mode < 0) {
        return;
    }
    if (mode == work->mode) {
        return;
    }
    switch (mode) {
    case SWAY:
        part->target_tilt = poses[SWAY].tilt;
        if (part->side == 0) {
            rest = sway[SIDE_0_REST];
        } else {
            rest = sway[SIDE_1_REST];
        }
        part->target_heading = rest;
        part->phase = 0;
        break;
    case FOLDED:
        part->target_tilt = poses[FOLDED].tilt;
        part->target_heading = 0;
        break;
    case TRACK:
        part->target_tilt = poses[TRACK].tilt;
        /* @bug uninitialized: the turn to TRACK heads for direction.yaw
         * unset, at 0x8c506fca. player_direction's result is dropped */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the turn heads for the
         * player's yaw */
        direction = player_direction(task);
#else
        player_direction(task);
#endif
        part->target_heading = direction.yaw;
        break;
    case SPIN:
        part->target_tilt = poses[SPIN].tilt;
        part->target_heading = poses[SPIN].heading;
        part->phase = part->target_heading;
        break;
    default:
        return;
    }
    part->next_mode = mode;
    work->mode = TRANSITION;
    work->submode = 0;
}

/* the most tilt and heading turn a step in TRANSITION and TRACK */
#define TURN_STEP 0x80
#define SET_TILT(work, part, z_tilt)                                           \
    do {                                                                       \
        (work)->ang.x = (Float)(part)->tilt * njSin((part)->heading);          \
        (work)->ang.z = (Float)(z_tilt) * njCos((part)->heading);              \
    } while (0)

static void transition(struct task *task)
{
    struct part_motion *part = task->motion.p;
    struct task_work *work = task->work;
    part->tilt = angle_approach(part->tilt, part->target_tilt, TURN_STEP);
    part->heading =
        angle_approach(part->heading, part->target_heading, TURN_STEP);
    SET_TILT(work, part, part->tilt);
    if (part->tilt == part->target_tilt) {
        work->mode = part->next_mode;
        work->submode = 0;
    }
}

static void sway_part(struct task *task)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    Angle rest;
    if (part->side == 0) {
        rest = sway[SIDE_0_REST];
    } else {
        rest = sway[SIDE_1_REST];
    }
    part->heading = rest + (njSin(part->phase) * sway[SWAY_ARC]);
    SET_TILT(work, part, part->tilt);
    part->phase += sway[SWAY_STEP];
}

/* TRACK turns heading toward player 0. ang.z takes no more tilt than player
 * 0's pitch */
static void track_player(struct task *task)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    struct target_direction direction;
    Angle tilt;
    /* @hack spelling: angle, a local the new heading and the tilt pass
     * through, where the fields would do */
    Angle angle;
    direction = player_direction(task);
    angle = angle_approach(part->heading, direction.yaw, TURN_STEP);
    part->heading = angle;
    angle = part->tilt;
    tilt = angle;
    if (direction.pitch < angle) {
        tilt = direction.pitch;
    }
    SET_TILT(work, part, tilt);
}

static void fold(struct task *task)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    work->ang.x = 0;
    work->ang.z = 0;
    part->heading = 0;
}

static void spin(struct task *task)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    Angle step;
    if (part->side == 0) {
        step = -spin_steps[0];
    } else {
        step = spin_steps[0];
    }
    part->heading = part->phase;
    SET_TILT(work, part, part->tilt);
    part->phase += step;
}

/* SWAY turns to TRACK where player 0 lies within TRACK_RANGE of the boss in x
 * and z. player 0 must also lie within TRACK_TURN of his facing. TRACK turns
 * back to SWAY beyond SWAY_RANGE */
#define TRACK_RANGE 200.0F
#define TRACK_TURN 0x3000
#define SWAY_RANGE 300.0F
/* pick the pose the boss's mode asks for, and turn toward it */
static void select_pose(struct task *task)
{
    struct task_work *work = task->work;
    struct part_motion *part = task->motion.p;
    Float distance;
    Angle turn;
    Sint8 mode;
    if (work->mode == TRANSITION) {
        return;
    }
    switch (part->boss->mode) {
    case BIGBOGY_THROW:
        mode = TRACK;
        break;
    case BIGBOGY_START:
    case BIGBOGY_HOLDER_DASH:
    case BIGBOGY_CIRCLE:
        bigbogy_player_distance(task->parent, &distance, &turn);
        if (work->mode == TRACK) {
            if (distance > SWAY_RANGE) {
                mode = SWAY;
            } else {
                mode = work->mode;
            }
        } else if (work->mode == SWAY) {
            if (distance < TRACK_RANGE && turn < TRACK_TURN) {
                mode = TRACK;
            } else {
                mode = work->mode;
            }
        } else {
            mode = SWAY;
        }
        break;
    /* @hack spelling: cases of their own for BIGBOGY_BREATH and
     * BIGBOGY_RESTORE_DARK, where the default's FOLDED would do */
    case BIGBOGY_BREATH:
        mode = FOLDED;
        break;
    case BIGBOGY_INTRO:
        mode = SWAY;
        break;
    case BIGBOGY_GROUNDED:
    case BIGBOGY_PLAYER_HIT:
        mode = SPIN;
        break;
    case BIGBOGY_RESTORE_DARK:
        mode = FOLDED;
        break;
    default:
        mode = FOLDED;
        break;
    }
    if (mode != work->mode) {
        transition_to(task, mode);
    }
}

static void part_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped || game_state.unk_09) {
        return;
    }
    switch (work->mode) {
    case FOLDED:
        fold(task);
        break;
    case TRACK:
        track_player(task);
        break;
    case SWAY:
        sway_part(task);
        break;
    case SPIN:
        spin(task);
        break;
    case TRANSITION:
        transition(task);
        break;
    default:
        select_pose(task);
        return;
    }
    select_pose(task);
    write_angles(task, &work->ang.x, 0);
}

void bigbogy_part_fold(struct task *task)
{
    struct task_work *work = task->work;
    if (work->mode == TRANSITION) {
        return;
    }
    if (work->mode == FOLDED) {
        return;
    }
    transition_to(task, FOLDED);
}
