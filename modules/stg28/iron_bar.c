/* the iron bar, IRONBAR in Pyramid Cave's object list: a chained bar that a
 * player swings round. the arc is a band the bar draws about the player's
 * grip. a press of A while the swing faces it throws the player toward the
 * work's scl */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/vertex_colors.h"
#include "sound/sound.h"
#include "stg28/iron_bar.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

/* by player slot, the point a throw has reached and its progress. the progress
 * is the share of the throw's steps run, from 0 to 1. the step writes both for
 * iron_bar_camera */
static NJS_POINT3 cave_iron_bar_throw_pos[2];
static Float cave_iron_bar_throw_progress[2];

#include "stg28/iron_bar/text.inc"
#include "stg28/models/iron_bar.h"

/* the bar's modes, in its work's mode */
enum iron_bar_mode {
    MODE_WAIT, /* no player holds the bar */
    MODE_SWING, /* a player swings round the bar */
    MODE_THROW /* the bar throws its player toward the landing */
};

/* the throw's phases, in the work's submode. phase n starts once the throw has
 * run throw_steps / PHASES * n steps */
#define PHASE_LEAVE 0
#define PHASE_FLY 1
#define PHASE_LAND 2
#define PHASES 3

/* the steps left of the bar's wait. a touch grabs the bar only while the wait
 * is 0, and WINDOW_SOUND plays only then. each touch in MODE_WAIT sets the wait
 * to WAIT_STEPS, as do a press of A and a hurt. the wait counts down on each
 * step of a swing, and on each untouched step of MODE_WAIT */
#define WAIT(work) ((work)->tick)
#define WAIT_STEPS 20
/* the steps the throw has run */
#define THROW_STEP(work) ((work)->timer)
/* the throw's landing. a throw ends at the grip moved by the landing less
 * pos */
#define LANDING(work) ((work)->scl)
/* TRUE where the grab reversed the swing, whose speed is then below 0. the
 * throw after a reversed swing spins its player backward */
#define REVERSED(task) ((task)->unk_3c.l)

/* the bar's record, at task->motion */
struct iron_bar {
    Uint8 unk_00[0x0c];
    /* the swing's speed, below 0 while REVERSED. a throw eases it toward the
     * spin */
    Float speed;
    Angle swing; /* how far round the bar the player has swung */
    Angle heading; /* the throw's heading, about y */
    /* the angle THROW_WINDOW is measured from: atan2 of the throw's
     * velocity.y and velocity.x */
    Angle aim;
    NJS_VECTOR velocity; /* the throw's velocity at its start */
    /* the throw's steps up to its top, down from it, and in all */
    int rise_steps;
    int fall_steps;
    int throw_steps;
    /* where along the bar the player took hold, from its middle along its x */
    Float grip;
    /* the index of the throw's camera mode, or NO_CAMERA_MODE */
    int camera_mode;
    int player; /* the slot of the player on the bar */
};
#define BAR(task) ((struct iron_bar *)(task)->motion.p)
#define NO_CAMERA_MODE (-1)

static void iron_bar_destroy(struct task *task);
static void iron_bar_step(struct task *task);
static void iron_bar_draw(struct task *task);
static void iron_bar_posts_step(struct task *task);

/* the index in lights that the step fills with iron_bar_light. the bar selects
 * it for its player while the swing faces the arc */
#define IRON_BAR_LIGHT 4
#define IRON_BAR_SHAPES 1
#define POST_SHAPES 2
#define POST_X 37.1F
#define RADIUS_BITS 0xf
/* the throw's top lies above the higher of the bar and the landing. its height
 * above that is ARC_RISE times the throw's distance across the ground, plus
 * ARC_CLEARANCE */
#define ARC_RISE 0.3F
#define ARC_CLEARANCE 20.0F
/* the swing starts at SWING_SPEED_SCALE times the player's speed, plus
 * SWING_SPEED_MIN, and no faster than SWING_SPEED_MAX */
#define SWING_SPEED_SCALE 5.7324834F
#define SWING_SPEED_MIN 3.0F
#define SWING_SPEED_MAX 10.0F
/* each step adds speed * (SWING_PULL - s) * SWING_TURN to the swing's angle.
 * s is the sine of that angle, negated while REVERSED */
#define SWING_PULL 6.0F
#define SWING_TURN (ANGLE_PER_DEGREE / 5)
/* the swing faces the arc while its angle lies in a window THROW_WINDOW wide.
 * while REVERSED, the window runs from aim - THROW_WINDOW up to the aim.
 * otherwise it runs from aim + THROW_WINDOW up to aim + 2 * THROW_WINDOW */
#define THROW_WINDOW THIRD_TURN
/* a thrown player spins by |speed| + SPIN_GAIN degrees a step. the spin is held
 * to LEAVE_SPIN_MAX in PHASE_LEAVE, and to FLY_SPIN_MAX in PHASE_FLY. each step
 * moves the speed SPIN_EASE of the way to that spin */
#define SPIN_GAIN 0.8F
#define SPIN_EASE 0.3F
#define LEAVE_SPIN_MAX 3.6F
#define FLY_SPIN_MAX 36.0F
/* the most a thrown player's animation frame moves in a step toward the
 * swing's */
#define FRAME_STEP_MAX 1.0F
/* GRAB_SOUND, sound 4 of the character's list, plays at a grab. WINDOW_SOUND,
 * an id in pyramid_cave_sound_list, plays while the swing faces the arc */
#define GRAB_SOUND 0x2004
#define WINDOW_SOUND 14
#define WINDOW_SOUND_STEPS 20
/* the throw's camera mode takes the index above the stack's top. it takes no
 * lower than CAMERA_MODE_LOWEST, the stack's last slot. where the top is that
 * slot already, camera_set_user_mode refuses the index past it. the throw then
 * tries again on the next step */
#define CAMERA_MODE_LOWEST (CAMERA_STACK_DEPTH - 1)
/* the stick_lock a throw's end gives its player */
#define LANDING_LOCK 2
#define CHAIN_SCALE 0.01F
/* the turning piece turns TURN_RATE about x for each of game_state.steps */
#define TURN_RATE 384

/* the bar's length, which cave_iron_bar_init halves into its shape's size_y */
static Float iron_bar_length = 44.0F;
static struct light iron_bar_light = {
    {0.0F, 1.0F, 0.0F}, 1.0F, 0.9F, 1.0F, 0.5F, 0.5F};
NJS_TEXLIST *cave_iron_bar_texlists[2] = {
#include "stg28/iron_bar/cave_iron_bar_texlists.inc"
};
/* the bar: a cylinder that its quarter turn about z lays along x.
 * cave_iron_bar_init sets its half length and adds to its radius */
/* clang-format off */
static struct collision_shape iron_bar_shapes[IRON_BAR_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_800000,
     {0.0F, 0.0F, 0.0F}, 3.0F, 15.0F, 3.0F, 0, {0, 0, QUARTER_TURN}},
};
/* the frame's two posts, POST_X either side of the bar's middle */
static struct collision_shape iron_bar_post_shapes[POST_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_800000,
     {POST_X, 0.0F, 0.0F}, 10.0F, 7.2F, 0.0F, 0, {0, 0, QUARTER_TURN}},
    {0, SHAPE_CYLINDER2, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_800000,
     {-POST_X, 0.0F, 0.0F}, 10.0F, 7.2F, 0.0F, 0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */
/* swing_offset places the player's pos from the grip in a swing.
 * leave_offset, fly_offset and land_offset place it from the throw's point in
 * the phases they name. each turns about y with the bar or the throw's
 * heading. all but land_offset turn about z with the swing too */
static NJS_POINT3 swing_offset = {0.0F, -10.0F, 0.0F};
static NJS_POINT3 fly_offset = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 leave_offset = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 land_offset = {0.0F, 0.0F, 0.0F};
/* the arc fades from arc_color_near, as the swing enters the window, to
 * arc_color_far as it leaves */
static Uint32 arc_color_far = 0x30804000;
static Uint32 arc_color_near = 0x70ff0000;

/* the camera's mode during a throw. it looks at the throw's point from where
 * the camera stood. across the ground, it stays within
 * cave_iron_bar_camera_reach of the point. it stands
 * cave_iron_bar_camera_drop * cos(p * HALF_TURN) below the point, where p is
 * the throw's progress. so it rises from below the point to above it */
static void iron_bar_camera(struct camera_stack *stack,
                            struct camera_slot *slot)
{
    NJS_POINT3 target;
    NJS_POINT3 pos;
    NJS_VECTOR to_camera;
    Float length;
    Float *from;
    Float *point;
    Float *to;

    /* @hack spelling: the members reached through from, point and to, stepped
     * one Float at a time, where the members written out would do */
    from = &stack->place.pos.x;
    to = &to_camera.x;
    point = &cave_iron_bar_throw_pos[camera_current_index].x;
    *to = *from - *point++;
    from++;
    to++;
    *to = *from++ - *point++;
    to++;
    *to = *from - *point;
    to_camera.y = 0.0F;
    length = njScalor(&to_camera);
    if (length > cave_iron_bar_camera_reach) {
        length = cave_iron_bar_camera_reach / length;
        /* @hack spelling: the members reached through to, stepped one Float at
         * a time, where the members written out would do */
        to = &to_camera.x;
        *to++ *= length;
        *to++ *= length;
        *to *= length;
    }
    /* @hack spelling: the members reached through from, point and to, stepped
     * one Float at a time, where the members written out would do */
    from = &to_camera.x;
    point = &cave_iron_bar_throw_pos[camera_current_index].x;
    to = (Float *)&pos;
    *to++ = *point++ + *from++;
    *to++ = *point++ + *from++;
    *to = *point + *from;
    pos.y = cave_iron_bar_throw_pos[camera_current_index].y -
            (njCos((Angle)(cave_iron_bar_throw_progress[camera_current_index] *
                           HALF_TURN)) *
             cave_iron_bar_camera_drop);
    /* @hack spelling: target, a copy of the throw's point, where the point
     * stored in place would do */
    target = cave_iron_bar_throw_pos[camera_current_index];
    camera_state.target = target;
    camera_state.pos = pos;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

void cave_iron_bar_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *posts;
    NJS_VECTOR to_landing;
    NJS_POINT3 start;
    NJS_POINT3 landing;
    Float top;
    Float rise;
    Float fall;
    Float steps;
    Float distance;
    Sint16 turn;
    Angle heading;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((posts = task_create_child(TASK_WORK, iron_bar_posts_step, task)) ==
        NULL) {
        return;
    }
    posts->work->pos = work->pos;
    posts->work->ang.y = work->ang.y;
    posts->work->ang.x = posts->work->ang.z = 0;
    /* @bug logic: keeps the posts' child where the record can't be had, and
     * the next step's try makes another (the branch at 0x8c501fa6) */
    task->motion.p = syCalloc(1, sizeof(struct iron_bar));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = iron_bar_draw;
    task->step = iron_bar_step;
    task->destructor = iron_bar_destroy;
    work->ang.z &= RADIUS_BITS;
    work->mode = MODE_WAIT;
    collision_init(task, iron_bar_shapes, IRON_BAR_SHAPES, COLLISION_OBJECT);
    work->collision->shapes->size_y = iron_bar_length * 0.5F;
    work->collision->shapes->radius += work->ang.z & RADIUS_BITS;
    collision_fit_radius(task);
    collision_init(posts, iron_bar_post_shapes, POST_SHAPES, COLLISION_OBJECT);
    BAR(task)->swing = 0;
    start = work->pos;
    landing = LANDING(work);
    top = start.y;
    if (top < landing.y) {
        top = landing.y;
    }
    to_landing.x = LANDING(work).x - work->pos.x;
    to_landing.y = 0.0F;
    to_landing.z = LANDING(work).z - work->pos.z;
    distance = njScalor(&to_landing);
    top += (distance * ARC_RISE) + ARC_CLEARANCE;
    rise = top - start.y;
    rise = njSqrt(njAbs(rise * 2.0F / player_records[0]->consts.gravity));
    fall = njSqrt(
        njAbs((top - landing.y) * 2.0F / player_records[0]->consts.gravity));
    steps = rise + fall;
    BAR(task)->velocity.x = (LANDING(work).x - work->pos.x) / steps;
    BAR(task)->velocity.y = player_records[0]->consts.gravity * rise;
    BAR(task)->velocity.z = (LANDING(work).z - work->pos.z) / steps;
    BAR(task)->aim =
        RADIANS(atan2f(BAR(task)->velocity.y, BAR(task)->velocity.x));
    BAR(task)->rise_steps = rise;
    BAR(task)->fall_steps = fall;
    BAR(task)->throw_steps = steps;
    heading = RADIANS(
        atan2f(LANDING(work).x - work->pos.x, LANDING(work).z - work->pos.z));
    turn = heading - work->ang.y;
    if (turn > QUARTER_TURN || turn < -QUARTER_TURN) {
        work->ang.y += HALF_TURN;
    }
    BAR(task)->heading = heading;
    BAR(task)->camera_mode = NO_CAMERA_MODE;
    BAR(task)->player = 0;
}

static void iron_bar_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (work->mode == MODE_SWING || work->mode == MODE_THROW) {
        player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
    }
    if (BAR(task)->camera_mode != NO_CAMERA_MODE) {
        camera_remove_mode(BAR(task)->player, BAR(task)->camera_mode);
        BAR(task)->camera_mode = NO_CAMERA_MODE;
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void iron_bar_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player_task;
    struct player *player;
    NJS_POINT3 pos;
    struct angles ang;
    struct angles unused;
    NJS_POINT3 offset;
    Float last_frame;
    /* @hack spelling: frames declared ahead of share, where any order would
     * do */
    Float frames;
    Float share;
    Float frame;
    Float target;
    Float change;
    Float limit;
    Float speed_y; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float sine;
    Angle angle;
    Angle off_aim;
    Bool facing;
    Float *to;
    Float *by;
    struct camera_slot *mode_slot;

    lights[IRON_BAR_LIGHT] = iron_bar_light;
    if (work->mode == MODE_WAIT && placement_try_leave(task)) {
        return;
    }
    if (work->mode == MODE_WAIT) {
        player_task = collision_next_player(task);
        if (player_task != NULL) {
            if (player_tasks[1] == player_task) {
                BAR(task)->player = 1;
            } else {
                BAR(task)->player = 0;
            }
            if (WAIT(work) == 0) {
                /* @bug logic: reads the center of the player's shape whose
                 * index is the player's slot. for the player in slot 1, that
                 * is its second shape (MOV.L @(8,R1),R0 at 0x8c5022f0).
                 * player_near reads every player's first shape (FMOV.S
                 * @R4+,FR3 at 0x8c0496c4) */
                pos = player_work[BAR(task)->player]
                          ->collision->shapes[BAR(task)->player]
                          .center;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateY(NULL, -work->ang.y);
                njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
                njCalcPoint(NULL, &pos, &pos);
                njPopMatrixEx();
                BAR(task)->grip = pos.x;
                angle = RADIANS(atan2f(pos.y, pos.z));
                BAR(task)->speed =
                    (njScalor(&((struct player_motion *)
                                    player_tasks[BAR(task)->player]
                                        ->motion.p)
                                   ->spd) *
                     SWING_SPEED_SCALE) +
                    SWING_SPEED_MIN;
                if (BAR(task)->speed > SWING_SPEED_MAX) {
                    BAR(task)->speed = SWING_SPEED_MAX;
                }
                work->mode = MODE_SWING;
                BAR(task)->swing = angle;
                REVERSED(task) = FALSE;
                /* a grab within a quarter turn past 0 or past HALF_TURN
                 * reverses the swing */
                angle = (Uint16)angle;
                /* @hack redundant: the test of angle > 0xffff, which never
                 * holds, as angle holds a Uint16 */
                if ((angle >= HALF_TURN || angle <= QUARTER_TURN) &&
                    (angle <= 3 * QUARTER_TURN || angle > 0xffff)) {
                    BAR(task)->speed = -BAR(task)->speed;
                    REVERSED(task) = TRUE;
                }
                /* @hack spelling: a request in each arm, where one request of a
                 * ?: would do */
                if (REVERSED(task)) {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_SWING_BACK);
                } else {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_SWING);
                }
                sound_play(GRAB_SOUND, NULL, 0, 0);
            }
            WAIT(work) = WAIT_STEPS;
        } else if (WAIT(work)) {
            WAIT(work)--;
        }
    }
    facing = FALSE;
    if (work->mode == MODE_SWING) {
        if (player_work[BAR(task)->player]->mode == PLAYER_MODE_HURT) {
            player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
            work->mode = MODE_WAIT;
            WAIT(work) = WAIT_STEPS;
        }
        if (WAIT(work)) {
            WAIT(work)--;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njTranslate(NULL, 0.0F, 0.0F, BAR(task)->grip);
        njRotateZ(NULL, HALF_TURN - BAR(task)->swing);
        matrix_get_angles(NULL, &ang);
        njCalcPoint(NULL, &swing_offset, &pos);
        njPopMatrixEx();
        sine = njCos(QUARTER_TURN - BAR(task)->swing);
        if (REVERSED(task)) {
            sine = -sine;
        }
        BAR(task)->swing +=
            (Angle)(BAR(task)->speed * (SWING_PULL - sine) * SWING_TURN);
        player = player_records[BAR(task)->player];
        angle = (Uint16)BAR(task)->swing;
        /* @hack spelling: share set from the angle and then scaled, where one
         * statement would do */
        share = angle;
        share *= 1.0F / ANGLE_MASK;
        frames = (Float)(motion_table[player->anim.actions[player->anim.current]
                                          .motion]
                             .motion->nbFrame -
                         1);
        target = frames * share;
        player->anim.frame = target;
        player_set_pos(BAR(task)->player, pos.x, pos.y, pos.z);
        player_work[BAR(task)->player]->ang.x = ang.x;
        player_work[BAR(task)->player]->ang.y = HALF_TURN - ang.y;
        player_work[BAR(task)->player]->ang.z = ang.z;
        /* @hack spelling: off_aim set from the aim and then reduced, where one
         * statement would do */
        off_aim = BAR(task)->aim;
        off_aim -= BAR(task)->swing;
        if (BAR(task)->speed < 0.0F) {
            if ((Sint16)off_aim > 0 && (Sint16)off_aim < THROW_WINDOW) {
                facing = TRUE;
                if (!WAIT(work)) {
                    sound_start_for(WINDOW_SOUND, work, 0, 0,
                                    WINDOW_SOUND_STEPS);
                }
            }
        } else {
            off_aim += THROW_WINDOW;
            if ((Sint16)off_aim < 0 && (Sint16)off_aim > -THROW_WINDOW) {
                facing = TRUE;
                if (!WAIT(work)) {
                    sound_start_for(WINDOW_SOUND, work, 0, 0,
                                    WINDOW_SOUND_STEPS);
                }
            }
        }
        if (player_pads[BAR(task)->player]->press & PDD_DGT_TA) {
            player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
            work->mode = MODE_WAIT;
            WAIT(work) = WAIT_STEPS;
            if (facing) {
                player = player_records[BAR(task)->player];
                work->mode = MODE_THROW;
                THROW_STEP(work) = 0;
                /* @hack spelling: a request in each arm, where one request of a
                 * ?: would do */
                if (REVERSED(task)) {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_THROW_BACK);
                } else {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_THROW);
                }
                player->anim.frame = 0.0F;
                work->submode = PHASE_LEAVE;
            }
        }
    }
    if (work->mode == MODE_THROW) {
        if (player_work[BAR(task)->player]->mode == PLAYER_MODE_HURT) {
            player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
            work->mode = MODE_WAIT;
            WAIT(work) = WAIT_STEPS;
        }
        if (BAR(task)->camera_mode == NO_CAMERA_MODE) {
            /* @hack spelling: the store and the test joined by a comma in
             * one condition, where a statement and a test would do */
            if ((BAR(task)->camera_mode = camera_top(BAR(task)->player) + 1),
                BAR(task)->camera_mode < CAMERA_MODE_LOWEST) {
                BAR(task)->camera_mode = CAMERA_MODE_LOWEST;
            }
            mode_slot = camera_set_user_mode(
                BAR(task)->player, BAR(task)->camera_mode, iron_bar_camera);
            if (mode_slot == NULL) {
                BAR(task)->camera_mode = NO_CAMERA_MODE;
            }
        }
        if (THROW_STEP(work) > BAR(task)->throw_steps) {
            player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
            frame = THROW_STEP(work);
            player_set_speed(
                BAR(task)->player, BAR(task)->velocity.x,
                BAR(task)->velocity.y -
                    (player_records[BAR(task)->player]->consts.gravity * frame),
                BAR(task)->velocity.z);
            player_records[BAR(task)->player]->stick_lock = LANDING_LOCK;
            work->mode = MODE_WAIT;
        } else {
            frame = THROW_STEP(work);
            pos.x = work->pos.x + (BAR(task)->velocity.x * frame);
            pos.z = work->pos.z + (BAR(task)->velocity.z * frame);
            pos.y = (BAR(task)->velocity.y * frame) -
                    (player_records[BAR(task)->player]->consts.gravity * frame *
                     frame * 0.5F) +
                    work->pos.y;
            /* @hack redundant: speed_y, set to the player's speed in y and
             * never read */
            speed_y =
                BAR(task)->velocity.y -
                (player_records[BAR(task)->player]->consts.gravity * frame);
            /* @hack redundant: the test of the throw's step against
             * throw_steps, which holds on this path, and its else */
            /* @hack redundant: the stores to unused, a local nothing reads */
            if (THROW_STEP(work) <= BAR(task)->throw_steps) {
                unused.y = 0;
                unused.z = 0;
                share = (Float)THROW_STEP(work) / (Float)BAR(task)->throw_steps;
            } else {
                unused.y = 0;
                unused.z = 0;
                share = 1.0F;
            }
            offset.x = BAR(task)->grip;
            offset.y = 0.0F;
            offset.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &offset, &offset);
            njPopMatrixEx();
            /* @hack spelling: the members reached through to and by, stepped
             * one Float at a time, where the members written out would do */
            to = &offset.x;
            by = &offset.x;
            *to++ *= share;
            *to++ *= share;
            *to *= share;
            to = &pos.x;
            *to++ += *by++;
            *to++ += *by++;
            *to += *by;
            cave_iron_bar_throw_pos[BAR(task)->player] = pos;
            cave_iron_bar_throw_progress[BAR(task)->player] = share;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &pos);
            njRotateY(NULL, BAR(task)->heading + QUARTER_TURN);
            if (work->submode <= PHASE_FLY) {
                if (work->submode) {
                    offset = fly_offset;
                } else {
                    offset = leave_offset;
                }
                njRotateZ(NULL, HALF_TURN - BAR(task)->swing);
                matrix_get_angles(NULL, &ang);
                njCalcPoint(NULL, &offset, &pos);
            } else {
                matrix_get_angles(NULL, &ang);
                njCalcPoint(NULL, &land_offset, &pos);
            }
            njPopMatrixEx();
            if (work->submode <= PHASE_FLY) {
                target = njAbs(BAR(task)->speed);
                limit = work->submode ? FLY_SPIN_MAX : LEAVE_SPIN_MAX;
                target += SPIN_GAIN;
                if (target > limit) {
                    target = limit;
                }
                BAR(task)->speed = BAR(task)->speed +
                                   ((target - BAR(task)->speed) * SPIN_EASE);
                angle = target * ANGLE_PER_DEGREE;
                if (REVERSED(task)) {
                    BAR(task)->swing -= angle;
                } else {
                    BAR(task)->swing += angle;
                }
                player = player_records[BAR(task)->player];
                angle = (Uint16)BAR(task)->swing;
                share = angle * (1.0F / ANGLE_MASK);
                last_frame =
                    (Float)(motion_table[player->anim
                                             .actions[player->anim.current]
                                             .motion]
                                .motion->nbFrame -
                            1);
                target = last_frame * share;
                if (player->anim.mode != PLAYER_ANIM_BLEND) {
                    frame = player->anim.frame;
                    if (!(njAbs(target - frame) < last_frame * 0.5F)) {
                        if (target > frame) {
                            target -= last_frame;
                        } else {
                            target += last_frame;
                        }
                    }
                    change = target - frame;
                    if (change > FRAME_STEP_MAX) {
                        change = FRAME_STEP_MAX;
                    }
                    if (-FRAME_STEP_MAX > change) {
                        change = -FRAME_STEP_MAX;
                    }
                    frame += change;
                    if (frame > last_frame) {
                        frame -= last_frame;
                    }
                    if (0.0F > frame) {
                        frame += last_frame;
                    }
                    player->anim.frame = frame;
                }
            }
            player_work[BAR(task)->player]->ang.x = ang.x;
            player_work[BAR(task)->player]->ang.y = HALF_TURN - ang.y;
            player_work[BAR(task)->player]->ang.z = ang.z;
            player_set_pos(BAR(task)->player, pos.x, pos.y, pos.z);
            if (THROW_STEP(work) >=
                    BAR(task)->throw_steps / PHASES * PHASE_LAND &&
                work->submode == PHASE_FLY) {
                player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                               ANIM_BAR_LAND);
                work->submode = PHASE_LAND;
            }
            if (THROW_STEP(work) >= BAR(task)->throw_steps / PHASES &&
                work->submode == PHASE_LEAVE) {
                work->submode = PHASE_FLY;
                /* @hack spelling: a request in each arm, where one request of a
                 * ?: would do */
                if (REVERSED(task)) {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_FLY_BACK);
                } else {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_FLY);
                }
            }
            THROW_STEP(work)++;
        }
    } else if (BAR(task)->camera_mode != NO_CAMERA_MODE) {
        camera_remove_mode(BAR(task)->player, BAR(task)->camera_mode);
        BAR(task)->camera_mode = NO_CAMERA_MODE;
    }
    if (facing) {
        if (light_selected[BAR(task)->player] != IRON_BAR_LIGHT) {
            light_select(IRON_BAR_LIGHT, BAR(task)->player);
        }
    } else if (light_selected[BAR(task)->player] == IRON_BAR_LIGHT) {
        light_select(light_saved[BAR(task)->player], BAR(task)->player);
    }
    collision_register(task);
}

static void iron_bar_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    Uint8 unused[24];
    NJS_CNK_MODEL *chain;
    Float *radius;
    Float chain_radius;
    Float length;
    Float fade;
    Uint32 color;
    Angle off_aim;

    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cave_iron_bar_texlist);
    njCnkEasyDrawModel(&cave_iron_bar_frame_model);
    njPushMatrixEx();
    length = work->ang.x * CHAIN_SCALE;
    chain = &cave_iron_bar_chain_model;
    /* @hack spelling: radius set inside its first read, through chain, where
     * a statement of its own would do */
    chain_radius = *(radius = &chain->r);
    *radius = chain_radius * length;
    njScale(NULL, 1.0F, length, 1.0F);
    njCnkModDrawModel(&cave_iron_bar_chain_model);
    *radius = chain_radius;
    njPopMatrixEx();
    njRotateX(NULL, game_state.steps * TURN_RATE);
    njCnkEasyDrawModel(&cave_iron_bar_turning_model);
    njPopMatrixEx();
    if (work->mode == MODE_SWING) {
        fade = -1.0F;
        off_aim = BAR(task)->aim - BAR(task)->swing;
        if (BAR(task)->speed < 0.0F) {
            if ((Sint16)off_aim > 0 && (Sint16)off_aim < THROW_WINDOW) {
                fade = 1.0F - ((Sint16)off_aim * (1.0F / THROW_WINDOW));
            }
        } else {
            off_aim += THROW_WINDOW;
            if ((Sint16)off_aim < 0 && (Sint16)off_aim > -THROW_WINDOW) {
                fade = 1.0F - (-(Sint16)off_aim * (1.0F / THROW_WINDOW));
            }
        }
        if (!(0.0F > fade)) {
            color = color_blend(arc_color_far, arc_color_near, fade);
            njFogDisable();
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y - QUARTER_TURN);
            njTranslate(NULL, 0.0F, 0.0F, -BAR(task)->grip);
            njRotateZ(NULL, (BAR(task)->speed < 0.0F ? 0 : -THROW_WINDOW) +
                                BAR(task)->aim);
            vlist_set_color(cave_iron_bar_arc_model[0].vlist, color);
            njCnkEasyDrawModel(cave_iron_bar_arc_model);
            njPopMatrixEx();
            njFogEnable();
        }
    }

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for its stores and
     * calls, which the draw's code needs. the image holds nothing else of the
     * block, so the stores and calls stand in for what it held */
    if (0) {
        (void)"<- SHADOW LEN(  . m)";
        (void)"<- ANGLE Y";
        (void)"<- COL SIZE(  )";
        (void)"<- TGTPOS X";
        (void)"<- TGTPOS Y";
        (void)"<- TGTPOS Z";
        work->scl.x = 0.0F;
        work->scl.y = 0.0F;
        work->scl.z = 0.0F;
        njRotateY(NULL, work->ang.y);
        njPopMatrixEx();
    }
}

static void iron_bar_posts_step(struct task *task)
{
    collision_register(task);
}
