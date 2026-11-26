/* the arc jump, the task ObjectSonicJump: the iron bar's throw without the
 * bar. it carries a player somersaulting along an arc from one point to
 * another. it gives the player's camera a mode that follows the player */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "stg03/arc_jump.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the throw's phases, in the work's submode, each a third of its steps. the
 * player plays the iron bar's throw animations through them */
enum arc_jump_phase { PHASE_LEAVE, PHASE_FLY, PHASE_LAND, PHASES };

/* the work's mode: MODE_CARRY while the arc carries its player, and
 * MODE_DONE once it lets go */
enum arc_jump_mode { MODE_DONE = 0, MODE_CARRY = 2 };

/* the bits of the task's unk_3c. ARC_JUMP_TURNED somersaults the player the
 * other way, with the _BACK animations. ARC_JUMP_THROUGH_STOP runs the arc
 * through a time stop, until the first step that finds time moving clears
 * it */
#define FLAGS(task) ((task)->unk_3c.l)
#define ARC_JUMP_TURNED 0x1
#define ARC_JUMP_THROUGH_STOP 0x2

/* the slot of the player to carry, which the creator leaves for the first
 * step */
#define PLAYER(work) ((work)->timer)
/* task_create's level for the arc jump */
#define ARC_JUMP_LEVEL 2

/* the arc's top lies above the higher of its ends. it lies ARC_RISE times
 * their distance across the ground higher, and ARC_CLEARANCE more */
#define ARC_RISE 0.3F
#define ARC_CLEARANCE 20.0F

/* a step turns the somersault by the size of the field spin plus SPIN_GAIN
 * degrees. LEAVE_SPIN_MAX holds that turn in PHASE_LEAVE, and FLY_SPIN_MAX in
 * PHASE_FLY. spin then eases SPIN_EASE of the way to the turn, so the caps
 * hold it too. in PHASE_LAND the player rides upright */
#define SPIN_GAIN 0.8F
#define LEAVE_SPIN_MAX 3.6F
#define FLY_SPIN_MAX 36.0F
#define SPIN_EASE 0.3F
/* the player's animation follows the somersault, by at most FRAME_STEP_MAX
 * frames a step */
#define FRAME_STEP_MAX 1.0F

/* the throw's camera mode takes the slot above the top of the player's camera
 * stack. it takes no slot lower than CAMERA_MODE_LOWEST, the stack's last. so
 * it takes the last slot, unless the top is that slot already. then
 * camera_set_user_mode refuses the slot past it, and the next step tries
 * again */
#define CAMERA_MODE_LOWEST (CAMERA_STACK_DEPTH - 1)
#define NO_CAMERA_MODE (-1)
/* the steps a landing player's stick_lock holds */
#define LANDING_LOCK 2

/* an arc jump's record, at task->motion, laid out as the iron bar's */
struct arc_jump {
    Uint8 unk_00[0x0c];
    Float spin; /* the eased turn of a step, in degrees */
    Angle roll; /* how far the player has somersaulted */
    Angle heading; /* the throw's heading, about y */
    /* the angle of a swing that lines up with the arc, as the iron bar takes
     * it. nothing here reads it */
    Angle aim;
    NJS_VECTOR velocity; /* the throw's speed as it starts, a step */
    int rise_steps;
    int fall_steps;
    int throw_steps; /* the steps the throw lasts */
    /* how far beside the arc the player rides, in full at its end. it stays
     * 0, as syCalloc left it */
    Float side;
    /* the index of the throw's mode in the player's camera stack, or
     * NO_CAMERA_MODE */
    int camera_mode;
    int player; /* the slot of the player it carries */
    int throw_step; /* the steps the throw has run */
};

#define JUMP(task) ((struct arc_jump *)(task)->motion.p)

/* Each camera follows its own player along the launch arc. */
static NJS_POINT3 arc_jump_points[2];
static Float arc_jump_progress[2];

/* the throw's camera stands within camera_reach of the player's point across
 * the ground. over the throw it rises from camera_drop below the point to
 * camera_drop above it */
static Float camera_reach = 50.0F;
static Float camera_drop = 30.0F;
static NJS_POINT3 dat_8c5faf48 = {0.0F, -10.0F, 0.0F};
/* the player's offset from its point on the arc in each phase. the offset
 * turns with the arc's heading, and through the first two phases with the
 * somersault too */
static NJS_POINT3 hang_fly = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 hang_leave = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 hang_land = {0.0F, 0.0F, 0.0F};

/* arc_jump_create_turned without ARC_JUMP_TURNED or ARC_JUMP_THROUGH_STOP.
 * nothing calls it, and no header declares it. it isn't static, since SHC
 * leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct task *arc_jump_create(int slot, NJS_POINT3 *from, NJS_POINT3 *to);
/* NOLINTEND(misc-use-internal-linkage) */

static void arc_jump_start(struct task *task);
static void arc_jump_destroy(struct task *task);
static void arc_jump_step(struct task *task);

/* the throw's camera mode, which looks at its player's point on the arc. the
 * camera keeps its place across the ground, but within camera_reach of the
 * point */
static void arc_jump_camera(struct camera_stack *stack,
                            struct camera_slot *slot)
{
    NJS_POINT3 target;
    NJS_POINT3 pos;
    NJS_VECTOR way;
    Float reach;
    Float *from;
    Float *sub;
    Float *out;

    /* @hack spelling: the members reached through from, sub and out, stepped
     * one Float at a time, where the members written out would do */
    from = &stack->place.pos.x;
    out = &way.x;
    sub = &arc_jump_points[camera_current_index].x;
    *out = *from - *sub++;
    from++;
    out++;
    *out = *from++ - *sub++;
    out++;
    *out = *from - *sub;
    way.y = 0.0F;
    reach = njScalor(&way);
    if (reach > camera_reach) {
        reach = camera_reach / reach;
        /* @hack spelling: the members reached through out, stepped one Float
         * at a time, where the members written out would do */
        out = &way.x;
        *out++ *= reach;
        *out++ *= reach;
        *out *= reach;
    }
    /* @hack spelling: the members reached through from, sub and out, stepped
     * one Float at a time, where the members written out would do */
    from = &way.x;
    sub = &arc_jump_points[camera_current_index].x;
    out = (Float *)&pos;
    *out++ = *sub++ + *from++;
    *out++ = *sub++ + *from++;
    *out = *sub + *from;
    pos.y =
        arc_jump_points[camera_current_index].y -
        (njCos((Angle)(arc_jump_progress[camera_current_index] * HALF_TURN)) *
         camera_drop);
    target = arc_jump_points[camera_current_index];
    camera_state.target = target;
    camera_state.pos = pos;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

static void arc_jump_start(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR across;
    NJS_POINT3 from;
    NJS_POINT3 to;
    Float top;
    Float rise;
    Float fall;
    Float steps;
    Float distance;
    Angle heading;
    Sint16 turn;

    if (game_state.cleared == 1) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct arc_jump));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = arc_jump_step;
    task->destructor = arc_jump_destroy;
    /* @hack redundant: roll set to 0, as syCalloc left it */
    JUMP(task)->roll = 0;
    from = work->pos;
    to = work->scl;
    top = from.y;
    if (top < to.y) {
        top = to.y;
    }
    across.x = work->scl.x - work->pos.x;
    across.y = 0.0F;
    across.z = work->scl.z - work->pos.z;
    distance = njScalor(&across);
    top += (distance * ARC_RISE) + ARC_CLEARANCE;
    /* @bug logic: works the arc out at player 0's gravity, though
     * arc_jump_step moves the carried player at that player's own. the two
     * agree: character_constants gives every character that can reach the
     * bungee vine a gravity of 0.08. player 0's record is read at 0x232 of
     * the object's code, and the carried player's at 0x560 */
    rise = top - from.y;
    rise = njSqrt(
        _builtin_fabsf((rise * 2.0F) / player_records[0]->consts.gravity));
    fall = top - to.y;
    fall = njSqrt(
        _builtin_fabsf((fall * 2.0F) / player_records[0]->consts.gravity));
    steps = rise + fall;
    JUMP(task)->velocity.x = (work->scl.x - work->pos.x) / steps;
    JUMP(task)->velocity.y = player_records[0]->consts.gravity * rise;
    JUMP(task)->velocity.z = (work->scl.z - work->pos.z) / steps;
    JUMP(task)->aim =
        (Angle)(atan2f(JUMP(task)->velocity.y, JUMP(task)->velocity.x) *
                ANGLE_PER_RADIAN);
    JUMP(task)->rise_steps = rise;
    JUMP(task)->fall_steps = fall;
    JUMP(task)->throw_steps = steps;
    heading =
        (Angle)(atan2f(work->scl.x - work->pos.x, work->scl.z - work->pos.z) *
                ANGLE_PER_RADIAN);
    /* both creators give ang.y this heading, so turn is 0 and ang.y stays */
    turn = heading - work->ang.y;
    if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
        work->ang.y += HALF_TURN;
    }
    JUMP(task)->heading = heading;
    JUMP(task)->camera_mode = NO_CAMERA_MODE;
    JUMP(task)->player = PLAYER(work);
    work->mode = MODE_CARRY;
    if (FLAGS(task) & ARC_JUMP_TURNED) {
        player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM,
                       ANIM_BAR_THROW_BACK);
    } else {
        player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM, ANIM_BAR_THROW);
    }
    player_records[JUMP(task)->player]->anim.frame = 0.0F;
    work->submode = PHASE_LEAVE;
}

static void arc_jump_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_CARRY) {
        player_request(JUMP(task)->player, PLAYER_REQUEST_RELEASE, 0);
    }
    if (JUMP(task)->camera_mode != NO_CAMERA_MODE) {
        camera_remove_mode(JUMP(task)->player, JUMP(task)->camera_mode);
        JUMP(task)->camera_mode = NO_CAMERA_MODE;
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void arc_jump_step(struct task *task)
{
    struct task_work *work = task->work;
    struct player *player;
    NJS_POINT3 pos;
    struct angles ang;
    struct angles unused;
    NJS_VECTOR side;
    struct camera_slot *slot;
    Float step;
    Float progress;
    Float turn;
    Float limit;
    Float frames;
    Float share;
    Float target;
    Float frame;
    Float move;
    Float fall_spd; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float *at;
    Float *add;
    int roll;
    Angle angle;

    if (game_state.cleared == 1 || work->mode == MODE_DONE ||
        player_work[JUMP(task)->player]->mode == PLAYER_MODE_HURT ||
        (player_records[JUMP(task)->player]->flags & PLAYER_MISSED)) {
        task_kill(task);
        return;
    }
    if (!(FLAGS(task) & ARC_JUMP_THROUGH_STOP) && game_state.time_stopped) {
        return;
    }
    if (!game_state.time_stopped) {
        FLAGS(task) &= ~ARC_JUMP_THROUGH_STOP;
    }
    if (JUMP(task)->camera_mode == NO_CAMERA_MODE) {
        JUMP(task)->camera_mode = camera_top(JUMP(task)->player) + 1;
        if (JUMP(task)->camera_mode < CAMERA_MODE_LOWEST) {
            JUMP(task)->camera_mode = CAMERA_MODE_LOWEST;
        }
        slot = camera_set_user_mode(JUMP(task)->player, JUMP(task)->camera_mode,
                                    arc_jump_camera);
        if (slot == NULL) {
            JUMP(task)->camera_mode = NO_CAMERA_MODE;
        }
    }
    if (JUMP(task)->throw_step > JUMP(task)->throw_steps) {
        player_request(JUMP(task)->player, PLAYER_REQUEST_RELEASE, 0);
        step = JUMP(task)->throw_step;
        player_set_speed(
            JUMP(task)->player, JUMP(task)->velocity.x,
            JUMP(task)->velocity.y -
                (player_records[JUMP(task)->player]->consts.gravity * step),
            JUMP(task)->velocity.z);
        player_records[JUMP(task)->player]->stick_lock = LANDING_LOCK;
        work->mode = MODE_DONE;
    } else {
        /* @hack redundant: player, a read of the player's record that
         * nothing uses */
        player = player_records[JUMP(task)->player];
        step = JUMP(task)->throw_step;
        pos.x = work->pos.x + (step * JUMP(task)->velocity.x);
        pos.z = work->pos.z + (step * JUMP(task)->velocity.z);
        pos.y = ((JUMP(task)->velocity.y * step) -
                 (player_records[JUMP(task)->player]->consts.gravity * step *
                  step * 0.5F)) +
                work->pos.y;
        /* @hack redundant: fall_spd, the player's speed along y, which
         * nothing reads */
        fall_spd = JUMP(task)->velocity.y -
                   (player_records[JUMP(task)->player]->consts.gravity * step);
        /* @hack redundant: the test of throw_step against throw_steps again,
         * whose else never runs: the test above has sent every later step
         * elsewhere */
        if (JUMP(task)->throw_step <= JUMP(task)->throw_steps) {
            /* @hack redundant: the stores to unused, a local nothing reads */
            unused.y = 0;
            unused.z = 0;
            progress =
                (Float)JUMP(task)->throw_step / (Float)JUMP(task)->throw_steps;
        } else {
            /* @hack redundant: the stores to unused, a local nothing reads */
            unused.y = 0;
            unused.z = 0;
            progress = 1.0F;
        }
        side.x = JUMP(task)->side;
        side.y = 0.0F;
        side.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &side, &side);
        njPopMatrixEx();
        /* @hack spelling: the members reached through at, stepped one Float at
         * a time, where the members written out would do */
        at = &side.x;
        *at++ *= progress;
        *at++ *= progress;
        *at *= progress;
        /* @hack spelling: the members reached through at and add, stepped one
         * Float at a time, where the members written out would do */
        at = &pos.x;
        add = &side.x;
        *at = *at + *add;
        at++;
        add++;
        *at = *at + *add;
        at++;
        add++;
        *at = *at + *add;
        arc_jump_points[JUMP(task)->player] = pos;
        arc_jump_progress[JUMP(task)->player] = progress;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &pos);
        njRotateY(NULL, JUMP(task)->heading + QUARTER_TURN);
        if (work->submode <= PHASE_FLY) {
            if (work->submode) {
                side = hang_fly;
            } else {
                side = hang_leave;
            }
            njRotateZ(NULL, HALF_TURN - JUMP(task)->roll);
            matrix_get_angles(NULL, &ang);
            njCalcPoint(NULL, &side, &pos);
        } else {
            matrix_get_angles(NULL, &ang);
            njCalcPoint(NULL, &hang_land, &pos);
        }
        njPopMatrixEx();
        if (work->submode <= PHASE_FLY) {
            turn = _builtin_fabsf(JUMP(task)->spin);
            limit = work->submode ? FLY_SPIN_MAX : LEAVE_SPIN_MAX;
            turn += SPIN_GAIN;
            if (turn > limit) {
                turn = limit;
            }
            JUMP(task)->spin =
                JUMP(task)->spin + ((turn - JUMP(task)->spin) * SPIN_EASE);
            angle = turn * ANGLE_PER_DEGREE;
            if (FLAGS(task) & ARC_JUMP_TURNED) {
                JUMP(task)->roll -= angle;
            } else {
                JUMP(task)->roll += angle;
            }
            player = player_records[JUMP(task)->player];
            /* @hack spelling: roll, a local for the somersault's angle, where
             * the angle read in place would do */
            roll = (Uint16)JUMP(task)->roll;
            share = roll * (1.0F / ANGLE_MASK);
            frames =
                motion_table[player->anim.actions[player->anim.current].motion]
                    .motion->nbFrame -
                1;
            target = frames * share;
            if (player->anim.mode != PLAYER_ANIM_BLEND) {
                frame = player->anim.frame;
                if (_builtin_fabsf(target - frame) >= frames * 0.5F) {
                    if (target > frame) {
                        target -= frames;
                    } else {
                        target += frames;
                    }
                }
                move = target - frame;
                if (move > FRAME_STEP_MAX) {
                    move = FRAME_STEP_MAX;
                }
                if (move < -FRAME_STEP_MAX) {
                    move = -FRAME_STEP_MAX;
                }
                frame += move;
                if (frame > frames) {
                    frame -= frames;
                }
                if (frame < 0.0F) {
                    frame += frames;
                }
                player->anim.frame = frame;
            }
        }
        player_work[JUMP(task)->player]->ang.x = ang.x;
        player_work[JUMP(task)->player]->ang.y = HALF_TURN - ang.y;
        player_work[JUMP(task)->player]->ang.z = ang.z;
        player_set_pos(JUMP(task)->player, pos.x, pos.y, pos.z);
        if (JUMP(task)->throw_step >=
                JUMP(task)->throw_steps / PHASES * PHASE_LAND &&
            work->submode == PHASE_FLY) {
            player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM,
                           ANIM_BAR_LAND);
            work->submode = PHASE_LAND;
        }
        if (JUMP(task)->throw_step >=
                JUMP(task)->throw_steps / PHASES * PHASE_FLY &&
            work->submode == PHASE_LEAVE) {
            work->submode = PHASE_FLY;
            if (FLAGS(task) & ARC_JUMP_TURNED) {
                player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM,
                               ANIM_BAR_FLY_BACK);
            } else {
                player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM,
                               ANIM_BAR_FLY);
            }
        }
        JUMP(task)->throw_step++;
    }
}

#pragma inline(arc_jump_way)
static void arc_jump_way(Float *from, Float *to, Float *way)
{
    *way++ = *to++ - *from++;
    *way++ = *to++ - *from++;
    *way = *to - *from;
}

struct task *arc_jump_create(int slot, NJS_POINT3 *from, NJS_POINT3 *to)
{
    struct task *task;
    NJS_VECTOR way;

    task = task_create(TASK_WORK, ARC_JUMP_LEVEL, arc_jump_start,
                       "ObjectSonicJump");
    if (task != NULL) {
        /* @hack spelling: the way from from to to worked out by
         * arc_jump_way, where three assignments would do */
        arc_jump_way((Float *)from, (Float *)to, (Float *)&way);
        task->work->pos = *from;
        task->work->scl = *to;
        PLAYER(task->work) = slot;
        task->work->ang.y = (Angle)(atan2f(way.x, way.z) * ANGLE_PER_RADIAN);
    }
    return task;
}

struct task *arc_jump_create_turned(int slot, NJS_POINT3 *from, NJS_POINT3 *to)
{
    struct task *task;
    NJS_VECTOR way;

    task = task_create(TASK_WORK, ARC_JUMP_LEVEL, arc_jump_start,
                       "ObjectSonicJump");
    if (task != NULL) {
        /* @hack spelling: the way from from to to worked out by
         * arc_jump_way, where three assignments would do */
        arc_jump_way((Float *)from, (Float *)to, (Float *)&way);
        task->work->pos = *from;
        task->work->scl = *to;
        PLAYER(task->work) = slot;
        task->work->ang.y = (Angle)(atan2f(way.x, way.z) * ANGLE_PER_RADIAN);
        FLAGS(task) = ARC_JUMP_TURNED;
        if (game_state.time_stopped) {
            FLAGS(task) |= ARC_JUMP_THROUGH_STOP;
        }
    }
    return task;
}
