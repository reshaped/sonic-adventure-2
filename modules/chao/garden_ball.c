/* the garden's ball. it turns as far as it rolls. a push that moves it
 * further than its speed would adds to its speed */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/body_collision.h"
#include "chao/bounce.h"
#include "chao/carry.h"
#include "chao/garden.h"
#include "chao/garden_ball.h"
#include "chao/garden_scene.h"
#include "chao/garden_toys.h"
#include "chao/movement.h"
#include "chao/toy_art.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "object/fall.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

enum ball_mode { BALL_REST, BALL_FREE, BALL_HELD, BALL_CARRIED };
enum ball_shape { BALL_PICKUP, BALL_CONTACT, BALL_LOWER, BALL_SHAPES };

#define BALL_RADIUS 1.5F

/* a push adds PUSH_GAIN times its excess over the ball's speed. a Chao's
 * body, which kicks, adds KICK_GAIN times that, and KICK_LIFT upward. no
 * push adds more than MAX_PUSH along its way */
#define PUSH_GAIN 1.1F
#define KICK_GAIN 2.9F
#define KICK_LIFT 0.28F
#define MAX_PUSH 1.0F
/* the garden's shadow under the ball is scaled SHADOW_WIDTH across and
 * SHADOW_HEIGHT up */
#define SHADOW_WIDTH 1.8F
#define SHADOW_HEIGHT 0.7F
/* garden_ball_rolling holds while the ball rolls free faster than this */
#define ROLLING_SPEED 0.4F
/* the shares of its speed a ball keeps. a floating ball keeps FLOAT_DRAG
 * across and FLOAT_DRAG_Y up each step, and FLOAT_SPIN of its roll. on the
 * floor it keeps ROLL_DRAG across. a let-go ball keeps RELEASE_DAMPING across
 * and RELEASE_DAMPING_Y up, once. a ball wholly under the surface is pushed up
 * at SUNK_BUOYANCY times its gravity */
#define FLOAT_DRAG 0.975F
#define SUNK_BUOYANCY 2.0F
#define FLOAT_DRAG_Y 0.93F
#define FLOAT_SPIN 0.96000004F
#define ROLL_DRAG 0.99F
#define RELEASE_DAMPING 0.4F
#define RELEASE_DAMPING_Y 0.7F

/* after more than USE_STEPS steps of PARTNER_USE, the ball takes one from
 * tick */
#define USE_STEPS 180

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's -fpack-struct=4 caps each member's alignment at 4. pack(8) lifts the
 * cap for the matrix, which GCC aligns to 8. orientation lies at 24 and the
 * record takes 88 bytes, both multiples of 8. so the layout stays SHC's */
#pragma pack(push, 8)
#endif
#endif
/* the ball's roll, at its task's extra. each step it turns by angle about
 * axis, and orientation gathers the turns */
struct ball_roll {
    Float inverse_radius;
    Sint32 unk_04;
    Angle angle;
    NJS_VECTOR axis;
    NJS_MATRIX orientation;
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) pushed above struct ball_roll */
#pragma pack(pop)
#endif
#endif

/* clang-format off */
static const struct collision_shape ball_shapes[BALL_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_HURTABLE | SHAPE_UNK_2000, {0.0F, 1.5F, 0.0F}, 15.0F, 3.0F, 0.0F,
     0.0F, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS, SHAPE_GUARD_LEVEL(3), 0,
     {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0.0F, {0, 0, 0}},
    {-96, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     0, {0.0F, -2.0F, 0.0F}, 4.2F, 5.0F, 0.0F, 0.0F, {0, 0, 0}},
};
/* clang-format on */

struct task *garden_ball;
Bool garden_ball_rolling;
Bool garden_ball_floating;

void garden_ball_land(struct task *ball)
{
    garden_bounce_on_floor(ball);
}

#define ENABLE_FREE_BALL(ball)                                                 \
    {                                                                          \
        collision_shape_on((ball), BALL_PICKUP);                               \
        collision_shape_on((ball), BALL_CONTACT);                              \
        collision_shape_off((ball), BALL_LOWER);                               \
    }

static void ball_rest(struct task *ball)
{
    struct task_work *work = ball->work;

    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        ENABLE_FREE_BALL(ball);
        work->submode++;
        break;
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        work->mode = BALL_HELD;
        work->submode = 0;
        return;
    }
    if (collision_touched(ball)) {
        work->mode = BALL_FREE;
        work->submode = 0;
    }
}

static void ball_roll(struct task *ball)
{
    struct task_work *work = ball->work;
    struct mover *mover = ball->motion.p;
    struct ball_roll *roll = ball->extra.p;
    Float length;

    /* @hack redundant: the test of garden_objects_running's answer, which
     * nothing uses */
    if (garden_objects_running(ball)) {
    }
    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        ENABLE_FREE_BALL(ball);
        work->submode++;
        break;
    }
    if (collision_pushed(ball)) {
        NJS_VECTOR displacement;
        NJS_VECTOR direction;
        Float speed;
        Float travel;

        displacement.x = work->pos.x - mover->last_pos.x;
        displacement.y = work->pos.y - mover->last_pos.y;
        displacement.z = work->pos.z - mover->last_pos.z;
        direction = displacement;
        length = njScalor(&direction);
        if (!(length < 0.01F)) {
            njUnitVector(&direction);
        }
        speed = njInnerProduct(&direction, &mover->spd);
        travel = njInnerProduct(&direction, &displacement);
        if (travel > speed) {
            speed = (travel - speed) * PUSH_GAIN;
            if (garden_hit_kind(ball, BALL_CONTACT, CHAO_BODY_HIT)) {
                speed *= KICK_GAIN;
                mover->spd.y += KICK_LIFT;
            }
            if (speed > MAX_PUSH) {
                speed = MAX_PUSH;
            }
            mover->spd.x += direction.x * speed;
            mover->spd.y += direction.y * speed;
            mover->spd.z += direction.z * speed;
        }
    }
    /* the ball floats while its bottom lies under a flat surface, as water's.
     * it rests with its center at the surface */
    if (mover->flat_y + BALL_RADIUS > work->pos.y) {
        if (work->pos.y > mover->flat_y - BALL_RADIUS) {
            mover->acc.y +=
                -(mover->gravity * (mover->flat_y - work->pos.y + BALL_RADIUS) *
                  (1.0F / BALL_RADIUS));
        } else {
            mover->acc.y += -(mover->gravity * SUNK_BUOYANCY);
        }
        mover->spd.x *= FLOAT_DRAG;
        mover->spd.y *= FLOAT_DRAG_Y;
        mover->spd.z *= FLOAT_DRAG;
        roll->angle = roll->angle * FLOAT_SPIN;
        garden_ball_floating = TRUE;
    }
    mover_step(ball);
    mover_save_pos(ball);
    mover_meet_stage(ball);
    if (work->flags & MOVER_WORK_ON_FLOOR) {
        mover->spd.x *= ROLL_DRAG;
        mover->spd.z *= ROLL_DRAG;
        length = njScalor(&mover->spd);
        if (length > 0.0001F) {
            NJS_VECTOR normal;
            NJS_VECTOR tangent;
            Float vertical;
            static const NJS_VECTOR up = {0.0F, 1.0F, 0.0F};
            static const NJS_VECTOR floor_normal = {0.0F, 1.0F, 0.0F};

            normal = up;
            njOuterProduct(&normal, &mover->spd, &roll->axis);
            if (njScalor(&roll->axis) > 0.0F) {
                njUnitVector(&roll->axis);
            } else {
                roll->axis.x = 0.0F;
                roll->axis.y = 1.0F;
                roll->axis.z = 0.0F;
            }
            normal = floor_normal;
            vertical = njInnerProduct(&normal, &mover->spd);
            normal.x *= vertical;
            normal.y *= vertical;
            normal.z *= vertical;
            tangent.x = mover->spd.x - normal.x;
            tangent.y = mover->spd.y - normal.y;
            tangent.z = mover->spd.z - normal.z;
            length = njScalor(&tangent);
            roll->angle = length * roll->inverse_radius * ANGLE_PER_RADIAN;
        } else {
            roll->angle = 0;
        }
    }
    if (roll->angle) {
        NJS_MATRIX orientation;

        njUnitMatrix(&orientation);
        njRotate(&orientation, &roll->axis, roll->angle);
        njMultiMatrix(&orientation, &roll->orientation);
        njSetMatrix(&roll->orientation, &orientation);
    }
    if (work->flags & WORK_HELD_BY_PLAYER) {
        roll->angle = 0;
        work->mode = BALL_HELD;
        work->submode = 0;
    }
}

static void ball_held(struct task *ball)
{
    struct task_work *work = ball->work;
    struct mover *mover = ball->motion.p;

    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        collision_shape_off(ball, BALL_PICKUP);
        collision_shape_off(ball, BALL_CONTACT);
        collision_shape_on(ball, BALL_LOWER);
        mover->aim_y = work->ang.y + player_work[0]->ang.y;
        garden_separate_partners(ball);
        work->submode++;
        break;
    }
    /* @hack spelling: the negated heading added, where aim_y less the
     * heading would do */
    work->ang.y = -player_work[0]->ang.y + mover->aim_y;
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        mover->spd.x *= RELEASE_DAMPING;
        mover->spd.y *= RELEASE_DAMPING_Y;
        mover->spd.z *= RELEASE_DAMPING;
        mover_clear_acc(ball);
        mover_save_pos(ball);
        work->mode = BALL_FREE;
        work->submode = 0;
    }
}

static void ball_carried(struct task *ball)
{
    struct task_work *work = ball->work;
    int command = garden_take_command(ball);

    /* @hack spelling: a switch on submode, where an if would do */
    switch (work->submode) {
    case 0:
        collision_shape_off(ball, BALL_PICKUP);
        collision_shape_off(ball, BALL_CONTACT);
        collision_shape_off(ball, BALL_LOWER);
        work->submode++;
        work->timer = 0;
        break;
    }
    switch (command) {
    case PARTNER_USE:
        if (work->timer++ > USE_STEPS) {
            work->timer = 0;
            work->tick--;
            if (work->tick == 0) {
                garden_command_partner(ball, PARTNER_USED_UP);
                task_kill(ball);
            }
        }
        break;
    case PARTNER_CANCEL:
        work->mode = BALL_FREE;
        work->submode = 0;
        break;
    }
    if (!garden_follow_chao(ball)) {
        work->mode = BALL_FREE;
        work->submode = 0;
    }
}

static void ball_step(struct task *ball)
{
    struct task_work *work = ball->work;
    struct mover *mover = ball->motion.p;

    garden_ball_floating = FALSE;
    mover->flags |= MOVER_SLOPE_BOUNCE;
    switch (work->mode) {
    case BALL_REST:
        ball_rest(ball);
        break;
    case BALL_FREE:
        ball_roll(ball);
        break;
    case BALL_HELD:
        ball_held(ball);
        break;
    case BALL_CARRIED:
        ball_carried(ball);
        break;
    }
    if (work->mode == BALL_FREE) {
        if (njScalor(&mover->spd) > ROLLING_SPEED) {
            garden_ball_rolling = TRUE;
        } else {
            garden_ball_rolling = FALSE;
        }
    } else {
        garden_ball_rolling = FALSE;
    }
    collision_register(ball);
}

static void ball_draw(struct task *ball)
{
    struct task_work *work = ball->work;
    struct ball_roll *roll = ball->extra.p;

    if (garden_visible_at_height(ball, 2.0F, 0.0F)) {
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        if (work->flags & WORK_HELD_BY_PLAYER) {
            njTranslate(NULL, 0.0F, 0.4F, 0.0F);
        }
        njPushMatrixEx();
        if (garden_hit_kind(ball, BALL_CONTACT, GARDEN_SHADE_HIT)) {
            draw_control_set(DRAW_CONTROL_SHADOW);
        }
        njMultiMatrix(NULL, &roll->orientation);
        switch (chao_current_area()) {
        case CHAO_AREA_NEUTRAL:
            njCnkEasyDrawObject(&garden_neutral_ball_model);
            break;
        case CHAO_AREA_HERO:
            njSetTexture(&garden_toy_texlist);
            njCnkEasyDrawObject(&garden_hero_ball_model);
            break;
        case CHAO_AREA_DARK:
            njSetTexture(&garden_toy_texlist);
            njCnkEasyDrawObject(&garden_dark_ball_model);
            break;
        }
        draw_control_clear(DRAW_CONTROL_SHADOW);
        njPopMatrixEx();
        if (((struct garden_entry *)ball->force.p)->distance <
            chao_tuning.object_shadow_distance) {
            if (garden_hit_kind(ball, BALL_CONTACT, GARDEN_SHADE_HIT)) {
                njTranslate(NULL, 0.0F, -1.45F, 0.0F);
            } else {
                njTranslate(NULL, 0.0F, -1.0F, 0.0F);
            }
            njScale(NULL, SHADOW_WIDTH, SHADOW_HEIGHT, SHADOW_WIDTH);
            njCnkModDrawModel(&garden_shadow_model);
        }
        njPopMatrix(1);
    }
}

static void ball_destroy(struct task *ball)
{
    garden_unregister_object(ball);
    /* @bug memory: a Chao's chase reads a destroyed ball's work. the
     * destructor at 0x0c53d9f0 leaves garden_ball pointing at the ball, and
     * the chase reads through it at 0x0c53197a */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the destroy clears garden_ball
     * where it names this ball */
    if (garden_ball == ball) {
        garden_ball = NULL;
        garden_ball_rolling = FALSE;
        garden_ball_floating = FALSE;
    }
#endif
}

static void ball_init(struct task *ball)
{
#ifdef NONMATCHING
    /* garden_ball_create has given this side's ball its shapes, fall kind and
     * registry entry */
    ball->step = ball_step;
    ball->draw = ball_draw;
    garden_ball_floating = FALSE;
#else
    struct task_work *work = ball->work;

    collision_init(ball, ball_shapes, BALL_SHAPES, COLLISION_OBJECT_MUTUAL);
    fall_set_kind(work, ball->motion.p, FALL_KIND_NORMAL);
    ball->step = ball_step;
    ball->destructor = ball_destroy;
    ball->draw = ball_draw;
    work->mode = BALL_FREE;
    work->submode = 0;
    garden_register_object(GARDEN_OBJECT_TOY, ball, GARDEN_TOY_BALL);
#endif
    garden_ball = ball;
}

void garden_ball_create(NJS_POINT3 *position, NJS_VECTOR *velocity)
{
    struct task *ball = task_create(TASK_WORK, 2, ball_init, "ALO_Ball");
#ifdef NONMATCHING
    /* this side reads the ball's work only once the test below finds the ball
     * made */
    struct task_work *work;
#else
    struct task_work *work = ball->work;
#endif
    struct ball_roll *roll;
    struct mover *mover;
#ifdef NONMATCHING
    /* this side makes no ball where task_create fails. the matching side writes
     * through the null */
    if (ball == NULL) {
        return;
    }
    work = ball->work;
#endif

    roll = ball->extra.p = syCalloc(1, sizeof(*roll));
#ifdef NONMATCHING
    /* this side destroys the ball where the heap has no roll. the matching side
     * writes through the null */
    if (roll == NULL) {
        task_destroy(ball);
        return;
    }
#endif
    roll->inverse_radius = 1.0F / BALL_RADIUS;
    roll->unk_04 = 0;
    roll->angle = 0;
    roll->axis.x = 0.0F;
    roll->axis.y = 1.0F;
    roll->axis.z = 0.0F;
    njUnitMatrix(&roll->orientation);
    mover = mover_alloc(ball);
#ifdef NONMATCHING
    /* this side destroys the ball where mover_alloc fails. the matching side
     * writes through the null */
    if (mover == NULL) {
        task_destroy(ball);
        return;
    }
#endif
    work->pos = *position;
    mover->spd = *velocity;
    mover_set_gravity(ball, -0.05F);
    mover->unk_b4 = 1.5F;
    mover->bottom = -2.5F;
    mover->floor_bounce = 0.85F;
    mover->probe_height = 1.0F;
    mover->friction = 0.99F;
    if (player_character(0) != PLAYER_EGGMAN) {
        mover->hold_turn = 0xd00;
    } else {
        mover->hold_turn = -0x1300;
    }
    mover_save_pos(ball);
#ifdef NONMATCHING
    /* this side gives the ball its shapes and its registry entry here. it
     * destroys the ball where either fails. the first step still makes it
     * garden_ball */
    fall_set_kind(work, ball->motion.p, FALL_KIND_NORMAL);
    work->mode = BALL_FREE;
    work->submode = 0;
    if (!collision_init(ball, ball_shapes, BALL_SHAPES,
                        COLLISION_OBJECT_MUTUAL) ||
        !garden_register_object(GARDEN_OBJECT_TOY, ball, GARDEN_TOY_BALL)) {
        task_destroy(ball);
        return;
    }
    ball->destructor = ball_destroy;
#else
    garden_ball_floating = FALSE;
#endif
}
