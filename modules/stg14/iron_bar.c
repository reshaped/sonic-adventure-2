/* the iron bar, IRONBAR in Radical Highway's object list: a bar a player
 * swings round. the arc is a band the bar draws about the player's grip. a
 * press of A while the swing faces it throws the player toward the work's scl.
 * it is stg13/iron_bar.c's bar with a copy for player 1 and a test of the time
 * stop */
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
#include "stg14/iron_bar.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* by player: where the throw has reached, and what share of it has run, from
 * 0 to 1. the step writes them for iron_bar_camera */
static NJS_POINT3 highway_iron_bar_throw_pos[2];
static Float highway_iron_bar_throw_progress[2];

/* the bar's modes, in its work's mode */
enum iron_bar_mode {
    MODE_WAIT, /* no player holds the bar */
    MODE_SWING, /* a player swings round the bar */
    MODE_THROW /* the bar throws its player toward the landing */
};

/* the throw's phases, in the work's submode. the throw splits its steps into
 * PHASES equal parts. a phase starts after as many parts as its number */
#define PHASE_LEAVE 0
#define PHASE_FLY 1
#define PHASE_LAND 2
#define PHASES 3

/* the steps left of the bar's wait. a touch grabs the bar, and WINDOW_SOUND
 * plays, only while the wait is 0. each of these sets it to WAIT_STEPS: a
 * touch in MODE_WAIT, a press of A in a swing, and a release for a hurt or a
 * miss. the wait counts down on each step of a swing and on each untouched
 * step in MODE_WAIT. a throw doesn't count it down */
#define WAIT(work) ((work)->tick)
#define WAIT_STEPS 20
/* the slot of the player the bar serves, in the work's timer. the set file's
 * bar serves slot 0, and its copy for player 1 serves slot 1 */
#define SLOT(work) ((work)->timer)
/* the throw's landing. a throw ends at the grip moved by the landing less
 * pos */
#define LANDING(work) ((work)->scl)
/* the set file's settings: the bar's half length, and what it adds to the
 * bar's radius. the file gives the half length less 1 */
#define HALF_LENGTH(work) ((Uint8)(work)->ang.x + 1.0F)
#define ADDED_RADIUS(work) ((Uint8)(work)->ang.z)
/* TRUE where the grab reverses the swing, which then runs backward with its
 * speed below 0. the throw that follows spins its player backward too */
#define REVERSED(task) ((task)->unk_3c.l)

/* the bar's record, at task->motion */
struct iron_bar {
    Uint8 unk_00[0x0c];
    /* the swing's speed, below 0 while REVERSED. a throw eases it toward the
     * spin, which is above 0 */
    Float speed;
    Angle swing; /* how far round the bar the player has swung */
    Angle heading; /* the throw's heading, about y */
    /* the angle the throw window is measured from: atan2 of the throw's
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
    int elapsed; /* the steps the throw has run */
    /* game_state.time_stopped as the player took hold, until time runs
     * again. the bar runs through a time stop while held_time_stop isn't 0 */
    int held_time_stop;
};
#define BAR(task) ((struct iron_bar *)(task)->motion.p)
#define NO_CAMERA_MODE (-1)

static void iron_bar_destroy(struct task *task);
static void iron_bar_step(struct task *task);
static void iron_bar_draw(struct task *task);

/* the light the bar gives its player while the swing faces the arc, and its
 * index in lights */
#define IRON_BAR_LIGHT 4
#define IRON_BAR_SHAPES 1
/* the throw's top lies above the higher of the bar and the landing. its
 * height above that is ARC_RISE times the throw's distance across the ground,
 * plus ARC_CLEARANCE */
#define ARC_RISE 0.3F
#define ARC_CLEARANCE 20.0F
/* a grab starts the swing at SWING_SPEED_SCALE times the player's speed, plus
 * SWING_SPEED_MIN, but no faster than SWING_SPEED_MAX */
#define SWING_SPEED_SCALE 5.7324834F
#define SWING_SPEED_MIN 3.0F
#define SWING_SPEED_MAX 10.0F
/* s is the swing's sine, negated while REVERSED. each step of a swing turns it
 * by speed * (SWING_PULL - s) * SWING_TURN */
#define SWING_PULL 6.0F
#define SWING_TURN (ANGLE_PER_DEGREE / 5)
/* the swing faces the arc through THROW_WINDOW. while REVERSED, that runs
 * from the aim down to aim - THROW_WINDOW. otherwise it runs from aim +
 * THROW_WINDOW up to aim + 2 * THROW_WINDOW */
#define THROW_WINDOW THIRD_TURN
/* a thrown player spins by the size of the swing's speed plus SPIN_GAIN, in
 * degrees a step. the spin is held to LEAVE_SPIN_MAX in PHASE_LEAVE, and to
 * FLY_SPIN_MAX in PHASE_FLY. each step eases the speed SPIN_EASE of the way to
 * the spin */
#define SPIN_GAIN 0.8F
#define SPIN_EASE 0.3F
#define LEAVE_SPIN_MAX 3.6F
#define FLY_SPIN_MAX 36.0F
/* GRAB_SOUND, sound 4 of the character's list, plays at a grab.
 * WINDOW_SOUND, sound 0 of the stage's, plays while the swing faces the arc */
#define GRAB_SOUND 0x2004
#define WINDOW_SOUND 0
#define WINDOW_SOUND_STEPS 20
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15
/* the throw's camera mode goes one above the stack's top, raised to the
 * stack's last slot. so it takes that slot, or the index past it where the
 * top is that slot already. camera_set_user_mode refuses that index, and the
 * throw tries again on the next step */
#define CAMERA_MODE_LOWEST (CAMERA_STACK_DEPTH - 1)
/* the stick_lock a throw's end gives its player */
#define LANDING_LOCK 2
/* the most a thrown player's animation frame moves in a step toward the
 * swing's */
#define FRAME_STEP_MAX 1.0F

static Float camera_reach = 50.0F;
static Float camera_drop = 30.0F;
/* the arc the draw shows while the swing faces it. it is a band about the
 * grip, 9 to 13.5 out, across a third of a turn */

static Sint16 arc_polygons[33] = {
#include "stg14/iron_bar/arc_polygons.inc"
};

static Sint32 arc_vertices[91] = {
#include "stg14/iron_bar/arc_vertices.inc"
};
/* @hack type: arc_model an array of one, where a single model would do */
static NJS_CNK_MODEL arc_model[1] = {
#include "stg14/iron_bar/arc_model.inc"
};
static NJS_CNK_OBJECT arc_object = {
#include "stg14/iron_bar/arc_object.inc"
};
static struct light iron_bar_light = {
    {0.0F, 1.0F, 0.0F}, 1.0F, 0.9F, 1.0F, 0.5F, 0.5F};
NJS_TEXLIST *highway_iron_bar_texlists[1] = {
#include "stg14/iron_bar/highway_iron_bar_texlists.inc"
};
/* the bar: a cylinder that its own quarter turn about z lays along x.
 * highway_iron_bar_init sets its half length and adds to its radius */
/* clang-format off */
static struct collision_shape iron_bar_shapes[IRON_BAR_SHAPES] = {
    {0, SHAPE_CYLINDER2, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_UNK_800000,
     {0.0F, 0.0F, 0.0F}, 3.0F, 20.0F, 3.0F, 0, {0, 0, QUARTER_TURN}},
};
/* clang-format on */
/* swing_offset places the player's pos from the grip in a swing.
 * leave_offset, fly_offset and land_offset place it from the throw's point in
 * the phases they name. each turns about y with the bar or the throw's
 * heading. all but land_offset turn about z by the swing's angle too */
static NJS_POINT3 swing_offset = {0.0F, -10.0F, 0.0F};
static NJS_POINT3 fly_offset = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 leave_offset = {0.0F, -5.0F, 0.0F};
static NJS_POINT3 land_offset = {0.0F, 0.0F, 0.0F};
/* the arc fades from arc_color_near, as the swing enters the window, to
 * arc_color_far, as it leaves */
static Uint32 arc_color_far = 0x30804000;
static Uint32 arc_color_near = 0x70ff0000;
/* the camera's mode during a throw. it looks at the throw's point from where
 * the camera stood. across the ground, it comes in to no more than
 * camera_reach from the point. it stands camera_drop * cos(progress *
 * HALF_TURN) below the point. so it rises from below the point to above it */
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
    point = &highway_iron_bar_throw_pos[camera_current_index].x;
    *to = *from - *point++;
    from++;
    to++;
    *to = *from++ - *point++;
    to++;
    *to = *from - *point;
    to_camera.y = 0.0F;
    length = njScalor(&to_camera);
    if (length > camera_reach) {
        length = camera_reach / length;
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
    point = &highway_iron_bar_throw_pos[camera_current_index].x;
    to = (Float *)&pos;
    *to++ = *point++ + *from++;
    *to++ = *point++ + *from++;
    *to = *point + *from;
    pos.y =
        highway_iron_bar_throw_pos[camera_current_index].y -
        (njCos((Angle)(highway_iron_bar_throw_progress[camera_current_index] *
                       HALF_TURN)) *
         camera_drop);
    /* @hack spelling: target, a copy of the throw's point, where the point
     * stored in place would do */
    target = highway_iron_bar_throw_pos[camera_current_index];
    camera_state.target = target;
    camera_state.pos = pos;
    camera_state.ang.z = 0;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 1;
    camera_state.trigger_push = 0;
}

void highway_iron_bar_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task *copy;
    NJS_VECTOR to_landing;
    NJS_POINT3 start;
    NJS_POINT3 landing;
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
    if (SLOT(work) == 0 && placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct iron_bar));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = iron_bar_draw;
    task->step = iron_bar_step;
    task->destructor = iron_bar_destroy;
    work->mode = MODE_WAIT;
    collision_init(task, iron_bar_shapes, IRON_BAR_SHAPES, COLLISION_OBJECT);
    work->collision->shapes->size_y = HALF_LENGTH(work);
    work->collision->shapes->radius += ADDED_RADIUS(work);
    collision_fit_radius(task);
    if (SLOT(work) == 0 && player_tasks[1] != NULL) {
        copy = task_create_child(TASK_WORK, highway_iron_bar_init, task);
        if (copy != NULL) {
            copy->work->pos = work->pos;
            copy->work->ang = work->ang;
            copy->work->scl = work->scl;
            SLOT(copy->work) = SLOT(work) + 1;
        }
    }
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
    struct player *player;
    NJS_POINT3 pos;
    struct angles ang;
    struct angles unused;
    NJS_POINT3 offset;
    Float frames;
    Float share;
    Float frame;
    Float target;
    Float progress;
    Float limit;
    Float steps;
    Float speed_y; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    int slot; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float sine;
    Angle angle;
    Angle off_aim;
    Float frame_scale;
    int grab;
    Bool facing;
    Bool found;
    int i;
    Float *to;
    Float *by;
    struct camera_slot *mode_slot;

    lights[IRON_BAR_LIGHT] = iron_bar_light;
    if (work->mode == MODE_WAIT && SLOT(work) == 0 &&
        placement_try_leave(task)) {
        return;
    }
    if (game_state.cleared == 1) {
        task_kill(task);
        return;
    }
    if (work->mode != MODE_WAIT &&
        (player_work[BAR(task)->player]->mode == PLAYER_MODE_HURT ||
         (player_records[BAR(task)->player]->flags & PLAYER_MISSED))) {
        player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
        work->mode = MODE_WAIT;
        WAIT(work) = WAIT_STEPS;
    }
    if (!game_state.time_stopped) {
        BAR(task)->held_time_stop = 0;
    }
    if (work->mode == MODE_WAIT && work->collision != NULL) {
        found = FALSE;
        if (player_tasks[SLOT(work)] != NULL) {
            for (i = 0; i < work->collision->hit_count; i++) {
                if (work->collision->hits[i].task == player_tasks[SLOT(work)]) {
                    BAR(task)->player = SLOT(work);
                    found = TRUE;
                    break;
                }
            }
        }
        if (found) {
            if (WAIT(work) == 0) {
                /* @bug logic: reads the center of the player's shape whose
                 * index is the player's slot. for the player in slot 1, that
                 * is its second shape (MOV.L @(8,R1),R0 at 0x8c501c58).
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
                grab = (Uint16)angle;
                REVERSED(task) = FALSE;
                /* @hack redundant: the test of grab > 0xffff, which never
                 * holds, as grab holds a Uint16 */
                if ((grab >= HALF_TURN || grab <= QUARTER_TURN) &&
                    (grab <= 3 * QUARTER_TURN || grab > 0xffff)) {
                    BAR(task)->speed = -BAR(task)->speed;
                    REVERSED(task) = TRUE;
                }
                if (REVERSED(task)) {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_SWING_BACK);
                } else {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_SWING);
                }
                BAR(task)->held_time_stop = game_state.time_stopped;
                sound_play(GRAB_SOUND, NULL, 0, 0);
                vibration_start(BAR(task)->player, VIBRATION_POWER,
                                VIBRATION_FREQ, VIBRATION_UNTIMED);
            }
            WAIT(work) = WAIT_STEPS;
        } else if (WAIT(work)) {
            WAIT(work)--;
        }
    }
    if (BAR(task)->held_time_stop || !game_state.time_stopped) {
        facing = FALSE;
        /* @hack spelling: frame_scale, a local for 1 / ANGLE_MASK, where the
         * constant in each product would do */
        frame_scale = 1.0F / ANGLE_MASK;
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
            share = angle * frame_scale;
            frames =
                motion_table[player->anim.actions[player->anim.current].motion]
                    .motion->nbFrame -
                1;
            share = frames * share;
            player->anim.frame = share;
            player_set_pos(BAR(task)->player, pos.x, pos.y, pos.z);
            player_work[BAR(task)->player]->ang.x = ang.x;
            player_work[BAR(task)->player]->ang.y = HALF_TURN - ang.y;
            player_work[BAR(task)->player]->ang.z = ang.z;
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
                    vibration_start(BAR(task)->player, VIBRATION_POWER,
                                    VIBRATION_FREQ, VIBRATION_UNTIMED);
                    work->mode = MODE_THROW;
                    BAR(task)->elapsed = 0;
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
                BAR(task)->camera_mode = camera_top(BAR(task)->player) + 1;
                if (BAR(task)->camera_mode < CAMERA_MODE_LOWEST) {
                    BAR(task)->camera_mode = CAMERA_MODE_LOWEST;
                }
                mode_slot = camera_set_user_mode(
                    BAR(task)->player, BAR(task)->camera_mode, iron_bar_camera);
                if (mode_slot == NULL) {
                    BAR(task)->camera_mode = NO_CAMERA_MODE;
                }
            }
            if (BAR(task)->elapsed > BAR(task)->throw_steps) {
                player_request(BAR(task)->player, PLAYER_REQUEST_RELEASE, 0);
                steps = BAR(task)->elapsed;
                player_set_speed(
                    BAR(task)->player, BAR(task)->velocity.x,
                    BAR(task)->velocity.y -
                        (player_records[BAR(task)->player]->consts.gravity *
                         steps),
                    BAR(task)->velocity.z);
                player_records[BAR(task)->player]->stick_lock = LANDING_LOCK;
                work->mode = MODE_WAIT;
            } else {
                /* @hack redundant: slot, set to the bar's player and never
                 * read */
                slot = BAR(task)->player;
                steps = BAR(task)->elapsed;
                pos.x = work->pos.x + (BAR(task)->velocity.x * steps);
                pos.z = work->pos.z + (BAR(task)->velocity.z * steps);
                pos.y = (BAR(task)->velocity.y * steps) -
                        (player_records[BAR(task)->player]->consts.gravity *
                         steps * steps * 0.5F) +
                        work->pos.y;
                /* @hack redundant: speed_y, set to the player's speed in y
                 * and never read */
                speed_y =
                    BAR(task)->velocity.y -
                    (player_records[BAR(task)->player]->consts.gravity * steps);
                /* @hack redundant: the test of elapsed against throw_steps,
                 * which holds on this path, and its else */
                /* @hack redundant: the stores to unused, a local nothing
                 * reads */
                if (BAR(task)->elapsed <= BAR(task)->throw_steps) {
                    unused.y = 0;
                    unused.z = 0;
                    progress = (Float)BAR(task)->elapsed /
                               (Float)BAR(task)->throw_steps;
                } else {
                    unused.y = 0;
                    unused.z = 0;
                    progress = 1.0F;
                }
                offset.x = BAR(task)->grip;
                offset.y = 0.0F;
                offset.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njRotateY(NULL, work->ang.y);
                njCalcPoint(NULL, &offset, &offset);
                njPopMatrixEx();
                /* @hack spelling: the members reached through to, stepped one
                 * Float at a time, where the members written out would do */
                to = &offset.x;
                *to++ *= progress;
                *to++ *= progress;
                *to *= progress;
                /* @hack spelling: the members reached through to and by, by
                 * stepped as it is read and to after each sum, where the
                 * members written out would do */
                to = &pos.x;
                by = &offset.x;
                *to += *by++;
                to++;
                *to += *by++;
                to++;
                *to += *by;
                highway_iron_bar_throw_pos[BAR(task)->player] = pos;
                highway_iron_bar_throw_progress[BAR(task)->player] = progress;
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
                    BAR(task)->speed =
                        BAR(task)->speed +
                        ((target - BAR(task)->speed) * SPIN_EASE);
                    angle = target * ANGLE_PER_DEGREE;
                    if (REVERSED(task)) {
                        BAR(task)->swing -= angle;
                    } else {
                        BAR(task)->swing += angle;
                    }
                    player = player_records[BAR(task)->player];
                    angle = (Uint16)BAR(task)->swing;
                    share = angle * frame_scale;
                    frames =
                        motion_table[player->anim.actions[player->anim.current]
                                         .motion]
                            .motion->nbFrame -
                        1;
                    share = frames * share;
                    if (player->anim.mode != PLAYER_ANIM_BLEND) {
                        frame = player->anim.frame;
                        if (!(njAbs(share - frame) < frames * 0.5F)) {
                            if (share > frame) {
                                share -= frames;
                            } else {
                                share += frames;
                            }
                        }
                        /* @hack spelling: share takes the frame's change too,
                         * where a local of its own would do */
                        share -= frame;
                        share = share > FRAME_STEP_MAX ? FRAME_STEP_MAX : share;
                        share =
                            share < -FRAME_STEP_MAX ? -FRAME_STEP_MAX : share;
                        frame += share;
                        if (frame > frames) {
                            frame -= frames;
                        }
                        if (0.0F > frame) {
                            frame += frames;
                        }
                        player->anim.frame = frame;
                    }
                }
                player_work[BAR(task)->player]->ang.x = ang.x;
                player_work[BAR(task)->player]->ang.y = HALF_TURN - ang.y;
                player_work[BAR(task)->player]->ang.z = ang.z;
                player_set_pos(BAR(task)->player, pos.x, pos.y, pos.z);
                if (BAR(task)->elapsed >=
                        BAR(task)->throw_steps / PHASES * PHASE_LAND &&
                    work->submode == PHASE_FLY) {
                    player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                   ANIM_BAR_LAND);
                    work->submode = PHASE_LAND;
                }
                if (BAR(task)->elapsed >= BAR(task)->throw_steps / PHASES &&
                    work->submode == PHASE_LEAVE) {
                    work->submode = PHASE_FLY;
                    if (REVERSED(task)) {
                        player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                       ANIM_BAR_FLY_BACK);
                    } else {
                        player_request(BAR(task)->player, PLAYER_REQUEST_ANIM,
                                       ANIM_BAR_FLY);
                    }
                }
                BAR(task)->elapsed++;
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
    }
    collision_register(task);
}

static void iron_bar_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local nothing reads, which only sizes the
     * frame */
    Uint8 unused[36];
    Float fade;
    Uint32 color;
    int off_aim;

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
            vlist_set_color(arc_model[0].vlist, color);
            njCnkEasyDrawModel(arc_model);
            njPopMatrixEx();
            njFogEnable();
        }
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants, and for the registers SHC
     * gives the draw. the image holds none of its code, so its stores are
     * stand-ins */
    if (0) {
        (void)"<- LENGTH";
        (void)"<- ANGLE Y";
        (void)"<- COL SIZE";
        (void)"<- TGTPOS X";
        (void)"<- TGTPOS Y";
        (void)"<- TGTPOS Z";
        work->scl.x = 1.0F;
        work->scl.y = 0.0F;
        work->scl.z = 0.0F;
    }
}
