/* ObjectEggJump, which carries a player along an arc from one point to
 * another */
#include <mathf.h>
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "player/player.h"
#include "shared/common_object.h"
#include "stg43/egg_jump.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the jump's mode, in its work's mode. a JUMP_DONE jump ends at its next
 * step */
enum jump_mode { JUMP_DONE = 0, JUMP_CARRY = 2 };

/* the jump's record, in task->motion. velocity is the arc's speed at its
 * start, in units a step. elapsed counts the steps since the start, and the
 * jump lets go once it passes duration. nothing reads aim, rise_steps or
 * fall_steps */
struct egg_jump {
    Angle aim;
    Angle heading; /* the way from the start to the end, about y */
    NJS_VECTOR velocity;
    int rise_steps;
    int fall_steps;
    int duration;
    Uint32 unused_20;
    int player; /* the slot of the player it carries */
    int elapsed;
};
#define JUMP(task) ((struct egg_jump *)(task)->motion.p)
/* egg_jump_create leaves egg_jump_start the arc's end in the work's scl. it
 * leaves the slot of the player it carries in the work's timer */
#define JUMP_END(work) ((work)->scl)
#define JUMP_SLOT(work) ((work)->timer)
/* a bit of task->unk_3c: a time stop under way as the jump was made doesn't
 * hold it. the bit clears at the first step without a time stop */
#define THROUGH_TIME_STOP 2

#define ARC_LIFT 20.0F
#define ARC_RISE 0.3F
/* the player tilts with the arc as if its speed in y were TILT_SHARE of the
 * arc's */
#define TILT_SHARE 0.5F
/* the steps of stick_lock the player takes as the jump lets it go */
#define RELEASE_STICK_LOCK 2
#define JUMP_LEVEL 2

static void egg_jump_destroy(struct task *task);
static void egg_jump_step(struct task *task);

static void egg_jump_start(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR across;
    NJS_POINT3 from;
    NJS_POINT3 to;
    Float top;
    Float distance;
    Float rise;
    Float fall;
    Float duration;
    Angle heading;
    Sint16 turn;

    if (game_state.cleared == 1) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct egg_jump));
    if (task->motion.p == NULL) {
        return;
    }
    task->step = egg_jump_step;
    task->destructor = egg_jump_destroy;
    from = work->pos;
    to = JUMP_END(work);
    top = from.y;
    if (to.y > top) {
        top = to.y;
    }
    across.x = JUMP_END(work).x - work->pos.x;
    across.y = 0.0F;
    across.z = JUMP_END(work).z - work->pos.z;
    distance = njScalor(&across);
    top += (distance * ARC_RISE) + ARC_LIFT;
    /* @hack spelling: rise set to the height first, where the height in
     * place would do */
    rise = top - from.y;
    rise = njSqrt(njAbs(rise * 2.0F / player_records[0]->consts.gravity));
    fall =
        njSqrt(njAbs((top - to.y) * 2.0F / player_records[0]->consts.gravity));
    duration = rise + fall;
    JUMP(task)->velocity.x = (JUMP_END(work).x - work->pos.x) / duration;
    JUMP(task)->velocity.y = player_records[0]->consts.gravity * rise;
    JUMP(task)->velocity.z = (JUMP_END(work).z - work->pos.z) / duration;
    JUMP(task)->aim =
        (Angle)(atan2f(JUMP(task)->velocity.y, JUMP(task)->velocity.x) *
                ANGLE_PER_RADIAN);
    JUMP(task)->rise_steps = rise;
    JUMP(task)->fall_steps = fall;
    JUMP(task)->duration = duration;
    heading = (Angle)(atan2f(JUMP_END(work).x - work->pos.x,
                             JUMP_END(work).z - work->pos.z) *
                      ANGLE_PER_RADIAN);
    /* @hack redundant: a half turn of ang.y where heading lies more than a
     * quarter turn from it, which never happens. egg_jump_create set ang.y
     * from the same two points, and nothing reads ang.y after */
    turn = heading - work->ang.y;
    if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
        work->ang.y += HALF_TURN;
    }
    JUMP(task)->heading = heading;
    JUMP(task)->player = JUMP_SLOT(work);
    work->mode = JUMP_CARRY;
    player_request(JUMP(task)->player, PLAYER_REQUEST_ANIM, ANIM_STAND);
}

static void egg_jump_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == JUMP_CARRY) {
        player_request(JUMP(task)->player, PLAYER_REQUEST_RELEASE, 0);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void egg_jump_step(struct task *task)
{
    struct task_work *work;
    /* @hack spelling: state, a local for &game_state, where game_state by
     * its name would do */
    struct game_state *state;
    /* @hack redundant: slot, which nothing reads */
    int slot; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float elapsed;
    Float vertical_speed;
    NJS_POINT3 pos;
    struct angles rotation;

    state = &game_state;
    work = task->work;
    if (state->cleared == 1 || work->mode == JUMP_DONE ||
        (player_records[JUMP(task)->player]->flags & PLAYER_MISSED)) {
        task_kill(task);
        return;
    }
    /* @hack type: game_state's time_stopped read as game_state_time_stopped,
     * where game_state.time_stopped would do */
    if (!(task->unk_3c.l & THROUGH_TIME_STOP) && game_state_time_stopped) {
        return;
    }
    if (!state->time_stopped) {
        task->unk_3c.l &= ~THROUGH_TIME_STOP;
    }
    if (JUMP(task)->elapsed > JUMP(task)->duration) {
        player_request(JUMP(task)->player, PLAYER_REQUEST_RELEASE, 0);
        elapsed = JUMP(task)->elapsed;
        player_set_speed(
            JUMP(task)->player, JUMP(task)->velocity.x,
            JUMP(task)->velocity.y -
                (player_records[JUMP(task)->player]->consts.gravity * elapsed),
            JUMP(task)->velocity.z);
        /* @hack redundant: a test of elapsed with nothing under it, which no
         * path reaches */
        if (0) {
            if (elapsed != 0.0F) {
            }
        }
        player_records[JUMP(task)->player]->stick_lock = RELEASE_STICK_LOCK;
        work->mode = JUMP_DONE;
    } else {
        slot = JUMP(task)->player;
        elapsed = JUMP(task)->elapsed;
        pos.x = work->pos.x + (JUMP(task)->velocity.x * elapsed);
        pos.z = work->pos.z + (JUMP(task)->velocity.z * elapsed);
        pos.y = (JUMP(task)->velocity.y * elapsed) -
                (player_records[JUMP(task)->player]->consts.gravity * elapsed *
                 elapsed * 0.5F) +
                work->pos.y;
        vertical_speed =
            JUMP(task)->velocity.y -
            (player_records[JUMP(task)->player]->consts.gravity * elapsed);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &pos);
        njRotateY(NULL, JUMP(task)->heading + QUARTER_TURN);
        njRotateZ(NULL, (Angle)(atan2f(-(vertical_speed * TILT_SHARE),
                                       njSqrt((JUMP(task)->velocity.x *
                                               JUMP(task)->velocity.x) +
                                              (JUMP(task)->velocity.z *
                                               JUMP(task)->velocity.z))) *
                                ANGLE_PER_RADIAN));
        matrix_get_angles(NULL, &rotation);
        pos.x = 0.0F;
        pos.y = 0.0F;
        pos.z = 0.0F;
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        player_work[JUMP(task)->player]->ang.x = rotation.x;
        player_work[JUMP(task)->player]->ang.y = HALF_TURN - rotation.y;
        player_work[JUMP(task)->player]->ang.z = rotation.z;
        player_set_pos(JUMP(task)->player, pos.x, pos.y, pos.z);
        JUMP(task)->elapsed++;
    }
}

struct task *egg_jump_create(int slot, NJS_POINT3 *from, NJS_POINT3 *to)
{
    struct task *task;
    NJS_VECTOR direction;

    task = task_create(TASK_WORK, JUMP_LEVEL, egg_jump_start, "ObjectEggJump");
    if (task != NULL) {
        Float *out = (Float *)&direction;
        Float *end = (Float *)to;
        Float *start = (Float *)from;

        /* @hack spelling: the three components walked through Float
         * pointers, where direction.x = to->x - from->x and its kin would
         * do */
        *out++ = *end++ - *start++;
        *out++ = *end++ - *start++;
        *out = *end - *start;
        task->work->pos = *from;
        JUMP_END(task->work) = *to;
        JUMP_SLOT(task->work) = slot;
        task->work->ang.y =
            (Angle)(atan2f(direction.x, direction.z) * ANGLE_PER_RADIAN);
        /* @hack type: game_state's time_stopped read as
         * game_state_time_stopped, where game_state.time_stopped would do */
        if (game_state_time_stopped) {
            task->unk_3c.l |= THROUGH_TIME_STOP;
        }
    }
    return task;
}
