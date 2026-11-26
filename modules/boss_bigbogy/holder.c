#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/arena.h"
#include "boss_bigbogy/holder.h"
#include "boss_bigbogy/stage.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/player.h"
#include "shared/ghost.h"
#include "shared/ring_sparkle.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* in the holder's work's flags, from a dash's start to its end */
#define HOLDER_DASHING 0x100
/* the ghost's sound as it lands from each bounce */
#define BOUNCE_SOUND 0x4023
/* the voices at a dash's start and at its end */
#define DASH_VOICE 0x887
#define RETURN_VOICE 0x888
#define GHOST_TURN_STEP 0x200 /* the most the ghost turns a step */
/* the height the ghost bounces up from, its mover's home.y. it is GHOST_LIFT
 * above the floor in the intro. as the ghost follows the boss, it is GHOST_LIFT
 * above his home.y. it is DASH_LIFT above the floor in a dash */
#define GHOST_LIFT 5.0F
#define DASH_LIFT 4.0F
/* the ghost's guard through a dash, which no attack is above. FOLLOW_GUARD is
 * its guard after the dash */
#define DASH_GUARD 3
#define FOLLOW_GUARD 0
enum holder_mode { HOLDER_FOLLOW, HOLDER_DASH, HOLDER_INTRO };
/* the turn about the center by which the ghost's place trails the boss on his
 * circle */
static Angle trail_turn = 0x2400;
/* the ghost throws a sparkle at each step where game_state.steps has none of
 * these bits */
static Sint8 sparkle_mask = 3;
/* the row of dash_arc that try_dash reads, which nothing changes */
static Sint8 dash_arc_row = 0;
/* the most turn about the center between the ghost and player 0 for a dash. its
 * column is the boss's health less 1 */
static Angle dash_arc[2][4] = {{26089, 20578, 15653, 13044},
                               {20578, 15653, 15653, 15653}};
static void holder_step(struct task *holder);
static void holder_destroy(struct task *task);

struct task *bigbogy_holder_create(struct task *boss)
{
    struct task *holder;
    struct mover *mover;
    struct task_work *work;
    holder = task_create_child(TASK_WORK, holder_step, boss);
    if (!holder) {
        return NULL;
    }
    mover = mover_alloc(holder);
    if (!mover) {
        task_kill(holder);
        return NULL;
    }
    mover->home.y = bigbogy_arena_center.y + GHOST_LIFT;
    work = holder->work;
    work->mode = HOLDER_INTRO;
    holder->destructor = holder_destroy;
    return holder;
}

static struct task *spawn_ghost(struct task *holder)
{
    struct mover *mover;
    struct task_work *boss_work;
    struct task_work *ghost_work;
    struct task_work *work;
    struct task *ghost;
    struct task *boss;
    /* @hack redundant: unused, which only sizes the frame */
    NJS_POINT3 unused[4];
    NJS_POINT3 point;
    (void)unused;
    task_create_child(TASK_WORK, ghost_step, holder);
    if (!holder->child) {
        return NULL;
    }
    holder->child->work->scl.x = GHOST_MAKIKO;
    mover = holder->motion.p;
    ghost = holder->child;
    boss = holder->parent;
    boss_work = boss->work;
    ghost_work = ghost->work;
    bigbogy_arena_advance(boss, &boss_work->pos, trail_turn, &point);
    ghost_work->pos.x = point.x;
    ghost_work->pos.y = mover->home.y;
    ghost_work->pos.z = point.z;
    work = holder->work;
    /* @hack redundant: mover read from holder again */
    mover = holder->motion.p;
    if (work->mode != HOLDER_INTRO) {
        work->mode = HOLDER_FOLLOW;
        mover->home.y = bigbogy_arena_center.y + GHOST_LIFT;
    }
    work->submode = 0;
    work->timer = 0;
    /* @hack spelling: the speeds and accelerations multiplied by 0.0F, where
     * assigning 0.0F would do for finite ones */
    mover->spd.x *= 0.0F;
    mover->spd.y *= 0.0F;
    mover->spd.z *= 0.0F;
    mover->acc.x *= 0.0F;
    mover->acc.y *= 0.0F;
    mover->acc.z *= 0.0F;
    work->flags = 0;
    return holder->child;
}

void bigbogy_holder_spawn_ghost(struct task *holder)
{
    spawn_ghost(holder);
}

static void holder_destroy(struct task *task)
{
}

/* bounce position on the mover's home.y. each step, spd.y loses gravity and
 * position moves by it. at or below home.y, spd.y first starts again from
 * jump, and BOUNCE_SOUND plays */
static void bounce(struct task *holder, NJS_POINT3 *position, Float jump,
                   Float gravity)
{
    /* @hack redundant: work, read from holder and never used */
    struct task_work *work = holder->work;
    struct mover *mover = holder->motion.p;
    (void)work;
    if (position->y <= mover->home.y) {
        mover->acc.y = jump;
        mover->spd.y = 0.0F;
        sound_play_at(BOUNCE_SOUND, NULL, 0, 0, &holder->child->work->pos);
    }
    mover->acc.y -= gravity;
    mover->spd.y += mover->acc.y;
    position->y += mover->spd.y;
    mover->acc.y = 0.0F;
}

/* the ang.y that turns the ghost at from to face to. it is the heading from
 * from to to, less a quarter turn */
#define GHOST_HEADING(from, to)                                                \
    ((Angle)(atan2f((to)->x - (from)->x, (to)->z - (from)->z) *                \
             ANGLE_PER_RADIAN) -                                               \
     QUARTER_TURN)

static void follow_boss(struct task *holder)
{
    struct task *boss = holder->parent;
    struct task_work *ghost = holder->child->work;
    NJS_POINT3 point;
    bigbogy_arena_advance(boss, &boss->work->pos, trail_turn, &point);
    ghost->pos.x = point.x;
    ghost->pos.z = point.z;
    /* @hack type: bigbogy_mode() cast to Sint8, where its int would do */
    if ((Sint8)bigbogy_mode() == BIGBOGY_RESTORE_DARK) {
        ghost->ang.y = angle_approach(
            ghost->ang.y, GHOST_HEADING(&ghost->pos, &boss->work->pos),
            GHOST_TURN_STEP);
    } else {
        ghost->ang.y = angle_approach(
            ghost->ang.y, GHOST_HEADING(&ghost->pos, &player_work[0]->pos),
            GHOST_TURN_STEP);
    }
}

#define SET_GUARD(shape, level)                                                \
    ((shape)->levels =                                                         \
         ((shape)->levels & ~SHAPE_GUARD) | SHAPE_GUARD_LEVEL(level))

/* a dash's acceleration toward its target. DASH_DRAG is the share of its
 * speed in x and z that a step keeps */
#define DASH_ACCELERATION 0.16F
#define DASH_DRAG 0.9F
/* the ghost turns back within DASH_TURN_RANGE of the boss's pos. it ends its
 * dash within DASH_END_RANGE of its place */
#define DASH_TURN_RANGE 50.0F
#define DASH_END_RANGE 40.0F
/* accelerate across the ground toward delta, which becomes a unit vector */
#define ACCELERATE(mover, delta)                                               \
    do {                                                                       \
        njUnitVector(&(delta));                                                \
        (mover)->acc.x = (delta).x * DASH_ACCELERATION;                        \
        (mover)->acc.z = (delta).z * DASH_ACCELERATION;                        \
        (mover)->spd.x *= DASH_DRAG;                                           \
        (mover)->spd.z *= DASH_DRAG;                                           \
        (mover)->spd.x += (mover)->acc.x;                                      \
        (mover)->spd.z += (mover)->acc.z;                                      \
    } while (0)

#pragma inline(move_point)
static void move_point(NJS_POINT3 *position, struct mover *mover)
{
    position->x += mover->spd.x;
    position->y += mover->spd.y;
    position->z += mover->spd.z;
}

/* the dash. the ghost flies to mover->target, the boss's pos as the dash
 * started. it then flies back to its place behind him */
static void dash(struct task *holder)
{
    struct task *boss;
    struct mover *mover;
    struct task_work *boss_work;
    struct task_work *work;
    struct task *ghost;
    struct task_work *ghost_work;
    NJS_VECTOR delta;
    NJS_POINT3 point;
    Float distance;
    boss = holder->parent;
    mover = holder->motion.p;
    boss_work = boss->work;
    ghost = holder->child;
    work = holder->work;
    ghost_work = ghost->work;
    switch (work->submode) {
    case 0:
        /* @hack spelling: ghost->work, where ghost_work would do */
        SET_GUARD(ghost->work->collision->shapes, DASH_GUARD);
        voice_play_priority_2(DASH_VOICE);
        work->submode++;
        /* fall through */
    case 1:
        bigbogy_horizontal_distance(&ghost_work->pos, &mover->target, &delta);
        /* @hack redundant: distance, bigbogy_horizontal_distance's result
         * computed again */
        distance = njSqrt((delta.x * delta.x) + (delta.z * delta.z));
        if (distance < DASH_TURN_RANGE) {
            bigbogy_arena_advance(boss, &boss_work->pos, trail_turn, &point);
            mover->target.x = point.x;
            mover->target.z = point.z;
            work->submode++;
        } else {
            ACCELERATE(mover, delta);
        }
        break;
    case 2:
        bigbogy_arena_advance(boss, &boss_work->pos, trail_turn, &delta);
        mover->target.x = delta.x;
        mover->target.z = delta.z;
        bigbogy_horizontal_distance(&ghost_work->pos, &mover->target, &delta);
        /* @hack redundant: distance, bigbogy_horizontal_distance's result
         * computed again */
        distance = njSqrt((delta.x * delta.x) + (delta.z * delta.z));
        if (distance < DASH_END_RANGE) {
            work->flags &= ~HOLDER_DASHING;
            mover->spd.x = 0.0F;
            mover->spd.z = 0.0F;
            /* @hack spelling: holder->parent->work, where boss_work would
             * do */
            bigbogy_arena_advance(boss, &holder->parent->work->pos, trail_turn,
                                  &point);
            ghost_work->pos.x = point.x;
            ghost_work->pos.z = point.z;
            voice_play_priority_2(RETURN_VOICE);
            SET_GUARD(ghost->work->collision->shapes, FOLLOW_GUARD);
            work->mode = HOLDER_FOLLOW;
            work->submode = 0;
        } else {
            ACCELERATE(mover, delta);
        }
        break;
    }
    ghost_work->ang.y = angle_approach(
        ghost_work->ang.y,
        GHOST_HEADING(&ghost_work->pos, &player_work[0]->pos), GHOST_TURN_STEP);
    move_point(&ghost_work->pos, mover);
    mover->acc.x = 0.0F;
    mover->acc.y = 0.0F;
    mover->acc.z = 0.0F;
}

/* the intro ends once its timer, the boss's timer at its start, counts below
 * INTRO_STEPS */
#define INTRO_STEPS 60
#define INTRO_SINK 0.25F
static void intro(struct task *holder)
{
    struct task_work *work = holder->work;
    struct mover *mover = holder->motion.p;
    struct task *boss = holder->parent;
    struct task_work *boss_work = boss->work;
    struct task *ghost = holder->child;
    struct task_work *ghost_work = ghost->work;
    switch (work->submode) {
    case 0:
        ghost_work->ang.y = boss_work->ang.y + QUARTER_TURN;
        work->timer = boss_work->timer;
        work->submode++;
        /* fall through */
    case 1:
        /* @bug logic: home.y never sinks here. the test at 0x8c5067f4 wants
         * it below the floor plus GHOST_LIFT, where bigbogy_holder_create set
         * it */
        if (mover->home.y < bigbogy_arena_center.y + GHOST_LIFT) {
            mover->home.y -= INTRO_SINK;
        }
        if (work->timer-- < INTRO_STEPS) {
            ghost_work->ang.y -= QUARTER_TURN;
            mover->home.y = bigbogy_arena_center.y + GHOST_LIFT;
            work->mode = HOLDER_FOLLOW;
            work->submode = 0;
        }
        break;
    }
}

/* the ghost's jump, and its gravity a step. the NEAR_ pair holds within
 * NEAR_RANGE of player 0 in x and z. the FAR_ pair holds beyond it, and the
 * DASH_ pair in a dash */
#define NEAR_RANGE 400.0F
#define NEAR_JUMP 1.4F
#define NEAR_GRAVITY 0.08F
#define FAR_JUMP 1.0F
#define FAR_GRAVITY 0.04F
#define DASH_JUMP 1.0F
#define DASH_GRAVITY 0.1F
/* a sparkle's speed across the ground, its rise a step, and its scale */
#define SPARKLE_SPEED 0.5F
#define SPARKLE_RISE 0.5F
#define SPARKLE_SCALE 1.5F
static void holder_step(struct task *holder)
{
    struct task_work *work = holder->work;
    struct task *ghost;
    struct task *boss;
    struct mover *mover;
    struct mover *boss_mover;
    NJS_POINT3 *position;
    NJS_VECTOR offset;
    NJS_POINT3 sparkle;
    NJS_VECTOR velocity;
    Float distance;
    Float jump;
    Float gravity;
    Angle angle;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.mini_event_running) {
        return;
    }
    if (!holder->child) {
        return;
    }
    if (!holder->parent) {
        return;
    }
    ghost = holder->child;
    boss = holder->parent;
    switch (work->mode) {
    case HOLDER_INTRO:
        break;
    case HOLDER_DASH:
        mover = holder->motion.p;
        mover->home.y = bigbogy_arena_center.y + DASH_LIFT;
        break;
    default:
        mover = holder->motion.p;
        boss_mover = boss->motion.p;
        mover->home.y = boss_mover->home.y + GHOST_LIFT;
        break;
    }
    switch (work->mode) {
    case HOLDER_FOLLOW:
        follow_boss(holder);
        break;
    case HOLDER_DASH:
        dash(holder);
        break;
    case HOLDER_INTRO:
        intro(holder);
        break;
    }
    position = &ghost->work->pos;
    bigbogy_horizontal_distance(position, &player_work[0]->pos, &offset);
    /* @hack redundant: distance, bigbogy_horizontal_distance's result
     * computed again */
    distance = njSqrt((offset.x * offset.x) + (offset.z * offset.z));
    if (work->mode != HOLDER_DASH) {
        if (distance <= NEAR_RANGE) {
            jump = NEAR_JUMP;
            gravity = NEAR_GRAVITY;
        } else {
            jump = FAR_JUMP;
            gravity = FAR_GRAVITY;
        }
    } else {
        jump = DASH_JUMP;
        gravity = DASH_GRAVITY;
    }
    bounce(holder, position, jump, gravity);
    if (!(game_state.steps & sparkle_mask)) {
        /* @hack spelling: ghost_work, a local for ghost->work, where
         * ghost->work would do */
        struct task_work *ghost_work = ghost->work;
        sparkle = ghost_work->pos;
        angle = rand() * (FULL_TURN * RAND_SCALE);
        velocity.x = njSin(angle) * SPARKLE_SPEED;
        velocity.y = SPARKLE_RISE;
        velocity.z = njCos(angle) * SPARKLE_SPEED;
        ring_sparkle_create_long(&sparkle, &velocity, SPARKLE_SCALE);
    }
}

Sint8 bigbogy_holder_try_dash(struct task *holder)
{
    struct task_work *work = holder->work;
    struct task_work *ghost_work;
    struct task_work *boss_work;
    NJS_POINT3 *player;
    Angle ghost_gap;
    Sint8 health;
    Angle boss_gap;
    /* @hack redundant: unused, which only sizes the frame */
    NJS_VECTOR unused;
    (void)unused;
    if (!holder->child) {
        return BIGBOGY_NO_GHOST;
    }
    ghost_work = holder->child->work;
    boss_work = holder->parent->work;
    player = &player_work[0]->pos;
    ghost_gap = bigbogy_arena_angle_between(&ghost_work->pos, player);
    health = bigbogy_health();
    if (health < 0) {
        return BIGBOGY_NO_DASH;
    }
    /* @hack spelling: boss_gap set in an else after the return, where a
     * statement after the if would do */
    if (ghost_gap >= dash_arc[dash_arc_row][health - 1]) {
        return BIGBOGY_NO_DASH;
    } else {
        boss_gap = bigbogy_arena_angle_between(&boss_work->pos, player);
    }
    if (ghost_gap > boss_gap) {
        return BIGBOGY_NO_DASH;
    }
    work->flags |= HOLDER_DASHING;
    /* @hack redundant: boss_work read from holder again */
    boss_work = holder->parent->work;
    ((struct mover *)holder->motion.p)->target = boss_work->pos;
    work->mode = HOLDER_DASH;
    work->submode = 0;
    return BIGBOGY_DASH_STARTED;
}

Bool bigbogy_holder_settle(struct task *holder)
{
    struct task_work *work = holder->work;
    struct mover *mover = holder->motion.p;
    if (work->mode == HOLDER_FOLLOW) {
        mover->spd.x = 0.0F;
        mover->spd.z = 0.0F;
        work->flags &= ~HOLDER_DASHING;
        return TRUE;
    }
    return FALSE;
}

Bool bigbogy_holder_is_dashing(struct task *holder)
{
    struct task_work *work = holder->work;
    if (work->flags & HOLDER_DASHING) {
        return TRUE;
    }
    return FALSE;
}
