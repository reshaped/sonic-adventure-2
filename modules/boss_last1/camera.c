#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "boss_last1/arena.h"
#include "boss_last1/body.h"
#include "boss_last1/camera.h"
#include "boss_last1/motion_art.h"
#include "camera/bosspoint.h"
#include "camera/bosspoint_blend.h"
#include "camera/camera.h"
#include "camera/motion.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/last_boss_arena.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"
#ifdef NONMATCHING
/* zam_vec3_turn_y, by which this side turns a vector. for a finite vector it
 * gives the shipped bits, as turn.h says */
#include "engine/dc/turn.h"
#endif

/* what the target does, which camera_change_aim picks as the Biolizard's
 * mode changes */
enum aim {
    AIM_IDLE,
    AIM_INTRO,
    AIM_HEAD,
    AIM_TAIL,
    AIM_BREATH,
    AIM_CHARGE,
    AIM_VOLLEY_LOW,
    AIM_VOLLEY_HIGH,
    AIM_CLIMB_LOW,
    AIM_CLIMB_HIGH,
    AIM_RECOVER,
    AIM_DEFEAT
};

/* the controller's flags. FIX_HEIGHT holds the target's y at height, and
 * FOLLOW_POINT puts the target at the point each step. EXTEND_POINT has the
 * next such step move the target EXTEND past the point, away from player 0.
 * a point within EXTEND of player 0 across the ground leaves the target
 * where it is */
enum camera_flags { FIX_HEIGHT = 1, FOLLOW_POINT = 4, EXTEND_POINT = 8 };
#define EXTEND 30.0F

/* the mode a task's work starts at, which previous_mode starts at too */
#define NO_MODE 0
/* the steps after BIOLIZARD_DAMAGE begins before the target follows the
 * head again */
#define RECOVER_STEPS 30
/* the heights at which FIX_HEIGHT holds the target */
#define LOW_TARGET 20.0F
#define VOLLEY_TARGET 130.0F
#define LIFTED_TARGET 150.0F
#define CLIMB_TARGET 160.0F
/* a climb's or a volley's aim rises once player 0 is above its RISE height.
 * it falls once player 0 is below its FALL height. a climb's aim also rises
 * while player 0 rides a rail, and doesn't fall during the ride */
#define CLIMB_RISE 60.0F
#define CLIMB_FALL 50.0F
#define VOLLEY_RISE 130.0F
#define VOLLEY_FALL 110.0F

/* the camera task's motion record */
struct aim_controller {
    Uint8 flags;
    Sint8 previous_mode; /* the Biolizard's mode at the last step */
    const Sint8 *boss_mode;
    Sint8 aim;
    Uint16 timer; /* AIM_RECOVER's steps left */
    Float height; /* the target's y under FIX_HEIGHT */
    Float unused_distance;
    const struct biolizard_camera_point *points;
    NJS_POINT3 *target;
    struct biolizard_camera_point point; /* the point the target follows */
};

/* the point camera's work, in its slot of camera 0's stack */
struct point_work {
    /* the steps left of a blend back to the camera's own place. nothing
     * starts one: point_step holds it at 0 */
    Sint32 blend_steps;
    Sint32 player; /* the player the camera follows */
    /* the camera's pos less the player's, which nothing reads */
    NJS_VECTOR arm;
    /* target less the player's pos, from the last step at which the two lay
     * NEAR or more apart */
    NJS_VECTOR to_target;
    /* target, from the last step at which the player stood on the ground or
     * a shape. nothing reads it */
    NJS_POINT3 grounded_target;
    NJS_POINT3 target; /* the point the camera looks at */
    NJS_POINT3 *eye; /* the point camera's task's pos, where it stands */
    struct camera_bosspoint param;
};

/* a camera's place and the point it looks at */
struct point_view {
    NJS_POINT3 pos;
    NJS_POINT3 target;
};

/* clang-format off */
static const struct collision_shape camera_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS), SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), 0,
    {0, 0, 0}, 10, 0, 0, 0, {0, 0, 0}};
/* clang-format on */
/* the offset from the player of the point the ground cast runs through. the
 * step turns it about y by a quarter turn less the player's ang.y */
static const NJS_VECTOR ground_arm = {0, 0, -1};
static struct task *point_task = NULL; /* the point camera's task, or null */
/* the point camera's settings, by these indexes. it takes START_PARAMS at
 * its start and after a wave of spheres */
enum { START_PARAMS, WAVE_PARAMS, LIFTED_WAVE_PARAMS };
static struct camera_bosspoint shots[3] = {
#include "boss_last1/camera/shots.inc"
};
/* the camera motions the intro, the charge and the defeat play */
static struct camera_motion intro = {
#include "boss_last1/camera/intro.inc"
};
static struct camera_motion charge = {
#include "boss_last1/camera/charge.inc"
};
static struct camera_motion defeat = {
#include "boss_last1/camera/defeat.inc"
};
/* @hack redundant: rise, which nothing sets but to 0, added to the
 * player's pos.y each step */
static Float rise;
/* the point camera's work, and its slot in camera 0's stack, as point_open
 * last set them. the point camera runs on camera 0 alone */
#define POINT_CAMERAS 1
static struct point_work *point_works[POINT_CAMERAS];
static struct camera_slot *point_slots[POINT_CAMERAS];

static void point_apply(struct camera_stack *stack, struct camera_slot *slot);
static void camera_copy_point(struct task *task,
                              const struct biolizard_camera_point *point);
static void camera_blend_point(struct task *task,
                               const struct biolizard_camera_point *point,
                               Sint16 adjust);
static void point_ease(int camera, Float height, Float distance, Float part);
static struct task *point_start(struct camera_bosspoint *param);

#define PLAY_MOTION(task, controller, preset)                                  \
    do {                                                                       \
        struct camera_motion motion;                                           \
        Angle yaw;                                                             \
        motion = (preset);                                                     \
        yaw = biolizard_body_angle();                                          \
        motion.ang = yaw + QUARTER_TURN;                                       \
        camera_copy_point(task,                                                \
                          &(controller)->points[BIOLIZARD_CAMERA_ORIGIN]);     \
        motion.origin = (controller)->target;                                  \
        camera_motion_start(0, &motion);                                       \
        (controller)->flags &= ~FIX_HEIGHT;                                    \
        (controller)->flags &= ~FOLLOW_POINT;                                  \
    } while (0)

static void camera_intro(struct task *task)
{
    /* @hack redundant: unused, a read of the task's work that nothing uses */
    struct task_work *unused = task->work;
    struct aim_controller *camera = task->motion.p;
    PLAY_MOTION(task, camera, intro);
}

static void camera_leave_intro(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    camera_remove_mode(0, camera_top(0));
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
    camera_copy_point(task, &camera->points[BIOLIZARD_CAMERA_HEAD]);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
    camera->flags |= FOLLOW_POINT;
    camera->flags |= FIX_HEIGHT;
    camera->aim = AIM_HEAD;
}

void biolizard_camera_charge(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    PLAY_MOTION(task, camera, charge);
    camera->aim = AIM_CHARGE;
}

void biolizard_camera_volley(struct task *task, Sint8 attack)
{
    struct aim_controller *camera = task->motion.p;
    struct camera_bosspoint *params;
    camera_remove_mode(0, camera_top(0));
    boss_vector_sum(camera->target, camera->point.base, &camera->point.offset);
    if (attack == BIOLIZARD_ATTACK_LIFTED_WAVE) {
        params = &shots[LIFTED_WAVE_PARAMS];
        camera_copy_point(task, &camera->points[BIOLIZARD_CAMERA_LIFE_SUPPORT]);
        point_ease(0, params->height, params->dist, params->part);
        camera->height = LIFTED_TARGET;
    } else {
        params = &shots[WAVE_PARAMS];
        point_ease(0, params->height, params->dist, params->part);
        camera->height = LOW_TARGET;
    }
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE2);
    camera->flags |= FOLLOW_POINT;
    camera->flags |= FIX_HEIGHT;
    camera->flags |= EXTEND_POINT;
    camera->aim = AIM_VOLLEY_LOW;
}

#define FOLLOW_AIM(task, camera, point, y, next)                               \
    do {                                                                       \
        camera_blend_point(task, &(camera)->points[point],                     \
                           CAMERA_ADJUST_RELATIVE2);                           \
        (camera)->height = (y);                                                \
        (camera)->flags |= FOLLOW_POINT;                                       \
        (camera)->flags |= FIX_HEIGHT;                                         \
        (camera)->aim = (next);                                                \
    } while (0)

static void camera_follow_head(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    FOLLOW_AIM(task, camera, BIOLIZARD_CAMERA_HEAD, LOW_TARGET, AIM_HEAD);
}

static void camera_follow_tail(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    FOLLOW_AIM(task, camera, BIOLIZARD_CAMERA_TAIL, LOW_TARGET, AIM_TAIL);
}

void biolizard_camera_side(struct task *task, Sint8 side)
{
    if (side == BIOLIZARD_CAMERA_HEAD) {
        camera_follow_head(task);
    } else if (side == BIOLIZARD_CAMERA_TAIL) {
        camera_follow_tail(task);
    }
}

static void camera_follow_breath(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    FOLLOW_AIM(task, camera, BIOLIZARD_CAMERA_HEAD, LOW_TARGET, AIM_BREATH);
}

static void camera_climb_low(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    FOLLOW_AIM(task, camera, BIOLIZARD_CAMERA_FRONT, LOW_TARGET, AIM_CLIMB_LOW);
}

static void camera_climb_high(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    FOLLOW_AIM(task, camera, BIOLIZARD_CAMERA_LIFE_SUPPORT, CLIMB_TARGET,
               AIM_CLIMB_HIGH);
}

static void camera_volley_low(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    camera_blend_point(task, &camera->points[BIOLIZARD_CAMERA_LIFE_SUPPORT],
                       CAMERA_ADJUST_RELATIVE2);
    camera->height = LOW_TARGET;
    camera->flags |= FIX_HEIGHT;
    camera->aim = AIM_VOLLEY_LOW;
}

static void camera_volley_high(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    camera_blend_point(task, &camera->points[BIOLIZARD_CAMERA_LIFE_SUPPORT],
                       CAMERA_ADJUST_RELATIVE2);
    camera->height = VOLLEY_TARGET;
    camera->flags |= FIX_HEIGHT;
    camera->aim = AIM_VOLLEY_HIGH;
}

void biolizard_camera_defeat(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    PLAY_MOTION(task, camera, defeat);
    camera->aim = AIM_DEFEAT;
}

void biolizard_camera_shake(Float size, Uint16 steps)
{
    camera_shake(0, steps, size);
}

static void camera_extend_target(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    NJS_VECTOR direction;
    NJS_POINT3 point;
    NJS_POINT3 target;
    boss_vector_sum(&point, camera->point.base, &camera->point.offset);
    vector_diff(&point, &player_work[0]->pos, &direction);
    direction.y = 0;
    if (njScalor(&direction) < EXTEND) {
        return;
    }
    njUnitVector(&direction);
    direction.x *= EXTEND;
    direction.y *= EXTEND;
    direction.z *= EXTEND;
    vector_sum(&point, &direction, &target);
    target.y = point.y;
    *camera->target = target;
}

static void camera_blend_point(struct task *task,
                               const struct biolizard_camera_point *point,
                               Sint16 adjust)
{
    struct aim_controller *camera = task->motion.p;
    camera->point.base = point->base;
    camera->point.offset = point->offset;
    camera_set_adjust(0, camera_top(0), adjust);
}

static void camera_copy_point(struct task *task,
                              const struct biolizard_camera_point *point)
{
    struct aim_controller *camera = task->motion.p;
    camera->point.base = point->base;
    camera->point.offset = point->offset;
    boss_vector_sum(camera->target, camera->point.base, &camera->point.offset);
}

static void camera_watch_ascent(struct task *task)
{
    struct task_work *player = player_work[0];
    if (player->mode == PLAYER_MODE_SONIC_RAIL || player->pos.y > CLIMB_RISE) {
        camera_climb_high(task);
    }
}

static void camera_watch_descent(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    struct task_work *player;
    camera->flags |= EXTEND_POINT;
    player = player_work[0];
    if (player->mode != PLAYER_MODE_SONIC_RAIL && player->pos.y < CLIMB_FALL) {
        camera_climb_low(task);
    }
}

static void camera_recover(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    if (camera->timer > 0) {
        camera->timer--;
        if (camera->timer == 0) {
            camera_blend_point(task, &camera->points[BIOLIZARD_CAMERA_HEAD],
                               CAMERA_ADJUST_RELATIVE5);
            camera->flags |= FOLLOW_POINT;
            camera->flags &= ~FIX_HEIGHT;
        }
    }
}

static void camera_volley_rise(struct task *task)
{
    if (biolizard_attack() != BIOLIZARD_ATTACK_LIFTED_WAVE &&
        player_work[0]->pos.y > VOLLEY_RISE) {
        camera_volley_high(task);
    }
}

static void camera_volley_fall(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    if (biolizard_attack() != BIOLIZARD_ATTACK_LIFTED_WAVE) {
        if (player_work[0]->pos.y < VOLLEY_FALL) {
            camera_volley_low(task);
        }
    } else {
        camera->flags |= EXTEND_POINT;
    }
}

static Sint8 camera_change_aim(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    Sint8 mode = *camera->boss_mode;
    Angle angle;
    if (mode == camera->previous_mode) {
        return camera->aim;
    }
    if (mode == BIOLIZARD_PAUSE_ATTACK) {
        return camera->aim;
    }
    if (camera->previous_mode == BIOLIZARD_PAUSE_ATTACK) {
        return camera->aim;
    }
    switch (camera->previous_mode) {
    case BIOLIZARD_APPEAR:
        camera_leave_intro(task);
        return camera->aim;
    case BIOLIZARD_SPHERE_ATTACK:
        point_ease(0, shots[START_PARAMS].height, shots[START_PARAMS].dist,
                   shots[START_PARAMS].part);
        break;
    }
    switch (mode) {
    case NO_MODE:
        camera->aim = AIM_IDLE;
        break;
    case BIOLIZARD_APPEAR:
        camera->aim = AIM_INTRO;
        camera_intro(task);
        break;
    case BIOLIZARD_WALK:
        angle = last_boss_angle_distance(
            camera->points[BIOLIZARD_CAMERA_HEAD].base, &player_work[0]->pos);
        if (angle > QUARTER_TURN) {
            camera_follow_tail(task);
        } else {
            camera_follow_head(task);
        }
        break;
    case BIOLIZARD_ENERGY_ATTACK:
        camera_follow_breath(task);
        break;
    case BIOLIZARD_SPHERE_ATTACK:
        break;
    case BIOLIZARD_OFFER_RAIL:
        camera_climb_low(task);
        break;
    case BIOLIZARD_DAMAGE:
        camera->timer = RECOVER_STEPS;
        camera->aim = AIM_RECOVER;
        break;
    }
    return camera->aim;
}

static void aim_hold(struct task *task)
{
    /* @hack redundant: camera, a read of the controller that nothing uses */
    struct aim_controller *camera = task->motion.p;
}

static void controller_step(struct task *task)
{
    struct aim_controller *camera = task->motion.p;
    Sint8 aim = camera_change_aim(task);
    switch (aim) {
    case AIM_IDLE:
        break;
    case AIM_INTRO:
        break;
    case AIM_HEAD:
    case AIM_TAIL:
        aim_hold(task);
        break;
    case AIM_BREATH:
        break;
    case AIM_CHARGE:
        break;
    case AIM_VOLLEY_LOW:
        camera_volley_rise(task);
        break;
    case AIM_VOLLEY_HIGH:
        camera_volley_fall(task);
        break;
    case AIM_CLIMB_LOW:
        camera_watch_ascent(task);
        break;
    case AIM_CLIMB_HIGH:
        camera_watch_descent(task);
        break;
    case AIM_RECOVER:
        camera_recover(task);
        break;
    }
    if (camera->flags & FOLLOW_POINT) {
        if (camera->flags & EXTEND_POINT) {
            camera_extend_target(task);
            camera->flags &= ~EXTEND_POINT;
        } else {
            boss_vector_sum(camera->target, camera->point.base,
                            &camera->point.offset);
        }
    }
    if (camera->flags & FIX_HEIGHT) {
        camera->target->y = camera->height;
    }
    camera->previous_mode = *camera->boss_mode;
}

#ifdef NONMATCHING
/* take the point camera's mode off camera 0's stack, at whatever depth.
 * forget its slot and its work */
static void point_detach(void)
{
    struct camera_slot *slot = point_slots[0];
    if (slot && slot->step == point_apply) {
        camera_remove_mode(0, slot - camera_stacks[0].modes);
    }
    point_slots[0] = NULL;
    point_works[0] = NULL;
}
#endif

static void controller_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* ends the point camera with the camera, before the Biolizard frees the
     * target it reads */
    if (point_task) {
        point_detach();
        task_kill(point_task);
    }
#endif
}

struct task *
biolizard_camera_create(struct task *parent, const Sint8 *mode,
                        NJS_POINT3 *target,
                        const struct biolizard_camera_point *points)
{
    struct task *task;
    struct aim_controller *camera;
    if (!parent) {
        /* @bug uninitialized: returns no value for a null parent, at
         * 0x8c506496 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task = task_create_child(0, controller_step, parent);
    /* @hack implicit: a return without a value, which leaves
     * task_create_child's null as the result, where return NULL would do */
    if (!task) {
#ifdef NONMATCHING
        /* the plain form the hack tag names. GCC refuses a return with no value
         * in a function that returns one */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    camera = syCalloc(1, sizeof(*camera));
    if (!camera) {
        task_kill(task);
        /* @bug uninitialized: returns no value where syCalloc can't give the
         * controller, at 0x8c5064c2 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns null */
        return NULL;
#else
        return; /* NOLINT(clang-diagnostic-return-mismatch) */
#endif
    }
    task->motion.p = camera;
    camera->previous_mode = NO_MODE;
    camera->boss_mode = mode;
    camera->target = target;
    camera->points = points;
    camera->point.base = points->base;
    camera->point.offset = points->offset;
    camera->unused_distance = 40.0F;
    shots[START_PARAMS].target = camera->target;
#ifdef NONMATCHING
    /* this side destroys the controller and returns null where point_start
     * makes no point camera */
    if (!point_start(&shots[START_PARAMS])) {
        task_destroy(task);
        return NULL;
    }
#else
    point_start(&shots[START_PARAMS]);
#endif
    task->destructor = controller_destroy;
    return task;
}

void biolizard_camera_restore(void)
{
    camera_remove_mode(0, camera_top(0));
}

/* while the player lies within NEAR of target, the camera looks at the
 * player plus to_target */
#define NEAR 20.0F
/* the cast that finds the ground runs from PROBE_RISE above a point 1 from
 * the player to PROBE_RISE below it */
#define PROBE_RISE 10.0F
/* until its slot has run more than FIRST_STEPS steps, the camera looks
 * straight at its view's target. after that each step turns it a fifth of
 * the way there */
#define FIRST_STEPS 10

static int point_open(int camera, const struct camera_bosspoint *param,
                      struct task *task)
{
    struct camera_slot *slot;
    struct point_work *work;
    if (camera_mode_at(camera, camera_top(camera)) == CAMERA_MODE_INIT) {
        camera_remove_mode(camera, camera_top(camera));
    }
    slot = camera_push_user_mode(camera, point_apply);
    if (!slot) {
        return 0;
    }
    camera_set_adjust(camera, camera_top(camera), param->adjust);
    work = (struct point_work *)slot->work;
    work->param = *param;
    work->blend_steps = 0;
    work->player = camera_current_index;
    point_works[camera] = work;
    point_slots[camera] = slot;
    point_ease(camera, work->param.height, work->param.dist, work->param.part);
    point_works[camera]->param.blend = CAMERA_BOSSPOINT_BLEND;
    work->eye = &task->work->pos;
    rise = 0.0F;
    return 1;
}

static void point_ease(int camera, Float height, Float distance, Float part)
{
#ifdef NONMATCHING
    /* this side eases only a slot that still runs point_apply. the matching
     * side writes through point_works whatever the slot now holds */
    if (!point_slots[camera] || point_slots[camera]->step != point_apply) {
        return;
    }
#endif
    point_works[camera]->param.goal.x = height;
    point_works[camera]->param.goal.y = distance;
    point_works[camera]->param.goal.z = part;
}

static void point_apply(struct camera_stack *stack, struct camera_slot *slot)
{
    struct point_work *work = (struct point_work *)point_slots[0]->work;
    camera_state.target = work->target;
    camera_state.pos = *work->eye;
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.ang.z = 0;
    camera_state.unk_04 = work->param.collides;
    camera_state.trigger_push = work->param.trigger_push;
}

static void point_step(struct task *task)
{
    struct task_work *task_work = task->work;
    struct camera_slot *slot = point_slots[0];
    struct point_work *work;
    struct point_view view;
    NJS_POINT3 saved;
    NJS_POINT3 pos;
    NJS_VECTOR dir;
    Float length;
#ifdef NONMATCHING
    /* this side steps only while camera 0 is current and its slot still runs
     * point_apply. the work below is that slot's */
    if (!slot || slot->step != point_apply || camera_current_index != 0) {
        return;
    }
#endif
    work = (struct point_work *)slot->work;
    if (game_state.time_stopped) {
        return;
    }
    pos = player_work[work->player]->pos;
    pos.y += rise;
    /* @hack redundant: saved, a copy of target written back over it at the
     * end, which nothing between changes */
    saved = *work->param.target;
    work->param.height = BOSSPOINT_EASE(work->param.height, work->param.goal.x);
    work->param.dist = BOSSPOINT_EASE(work->param.dist, work->param.goal.y);
    work->param.part = BOSSPOINT_EASE(work->param.part, work->param.goal.z);
    /* @hack spelling: target declared in a block of its own, where a local
     * of point_step would do */
    {
        NJS_POINT3 target;
        target = *work->param.target;
        view.target.x = pos.x;
        view.target.y = pos.y;
        view.target.z = pos.z;
        dir.x = pos.x - target.x;
        dir.y = pos.y - target.y;
        dir.z = pos.z - target.z;
    }
    njUnitVector(&dir);
    view.pos.x = view.target.x + (work->param.dist * dir.x);
    view.pos.y = view.target.y + (work->param.dist * dir.y);
    view.pos.z = view.target.z + (work->param.dist * dir.z);
    if (work->param.level == 0) {
        view.pos.y += work->param.height;
    }
    vector_subtract(&view.pos, &pos, &work->arm);
    /* @hack redundant: the blend back to the camera's own place, which never
     * runs: nothing sets blend_steps above 0, so each step takes it below 0
     * and back to 0 */
    work->blend_steps--;
    if (work->blend_steps < 0) {
        work->blend_steps = 0;
    } else {
        view.pos.x = BOSSPOINT_BLEND_EYE(
            view.pos.x, camera_views[camera_current_index]->pos.x,
            work->blend_steps);
        view.pos.y = BOSSPOINT_BLEND_EYE(
            view.pos.y, camera_views[camera_current_index]->pos.y,
            work->blend_steps);
        view.pos.z = BOSSPOINT_BLEND_EYE(
            view.pos.z, camera_views[camera_current_index]->pos.z,
            work->blend_steps);
    }
    vector_subtract(work->param.target, &pos, &dir);
    length = njScalor(&dir);
    if (length < NEAR) {
        vector_add(&pos, &work->to_target, &view.target);
    } else {
        view.target.x =
            BOSSPOINT_TARGET(pos.x, work->param.target->x, work->param.part,
                             point_works[camera_current_index]->param.blend);
        view.target.y =
            BOSSPOINT_TARGET(pos.y, work->param.target->y, work->param.part,
                             point_works[camera_current_index]->param.blend);
        view.target.z =
            BOSSPOINT_TARGET(pos.z, work->param.target->z, work->param.part,
                             point_works[camera_current_index]->param.blend);
        view.target.y += BOSSPOINT_LIFT(
            work->param.lift, point_works[camera_current_index]->param.blend);
        vector_subtract(work->param.target, &pos, &work->to_target);
    }
    if (point_works[camera_current_index]->param.level) {
        point_works[camera_current_index]->param.blend--;
        if (point_works[camera_current_index]->param.blend <= 0) {
            point_works[camera_current_index]->param.blend = 0;
        }
    } else {
        point_works[camera_current_index]->param.blend++;
        if (point_works[camera_current_index]->param.blend >=
            CAMERA_BOSSPOINT_BLEND) {
            point_works[camera_current_index]->param.blend =
                CAMERA_BOSSPOINT_BLEND;
        }
    }
    dir = ground_arm;
#ifdef NONMATCHING
    /* for a finite vector, the kernel gives the bits of the matrix calls below,
     * as engine/dc/turn.h says */
    zam_vec3_turn_y(QUARTER_TURN - player_work[work->player]->ang.y, &dir.x,
                    &dir.x);
#else
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateY(NULL, QUARTER_TURN - player_work[work->player]->ang.y);
    njCalcVector(NULL, &dir, &dir);
    njPopMatrix(1);
#endif
    /* @hack spelling: ground, spare and probe declared in a block of their
     * own, where locals of point_step would do */
    {
        NJS_POINT3 ground;
        /* @hack redundant: spare, a local that only sizes the frame */
        NJS_VECTOR spare;
        struct camera_probe probe;
        vector_add(&pos, &dir, &ground);
        probe.pos = ground;
        probe.pos.y += PROBE_RISE;
        probe.next = ground;
        probe.next.y -= PROBE_RISE;
        probe.skip = 0;
        if (camera_probe_cast(&probe)) {
            if (player_records[work->player]->consts.target_height +
                    probe.hit.y >
                view.pos.y) {
                view.pos.y =
                    player_records[work->player]->consts.target_height +
                    probe.hit.y;
            }
        }
    }
    if (slot->steps > FIRST_STEPS) {
        work->target.x = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.x, view.target.x);
        work->target.y = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.y, view.target.y);
        work->target.z = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.z, view.target.z);
    } else {
        work->target = view.target;
    }
    task_work->pos = view.pos;
    *work->param.target = saved;
    if (player_work[work->player]->flags &
        (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)) {
        work->grounded_target = *work->param.target;
    }
    collision_register(task);
}

static void point_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side ends only point_task's camera, and takes its mode off the
     * camera's stack. a task that point_start destroys before it starts isn't
     * point_task */
    if (point_task != task) {
        return;
    }
    point_detach();
#endif
    point_task = NULL;
}

static struct task *point_start(struct camera_bosspoint *param)
{
    struct task *task;
    if (point_task) {
        return NULL;
    }
    task = task_create(TASK_WORK | TASK_MOTION, 2, point_step,
                       "BL2BossPointCameraExec");
    if (!task) {
        return NULL;
    }
    task->destructor = point_destroy;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. it destroys the
     * task and returns null where point_open finds no camera slot */
    if (!point_open(0, param, task)) {
        task_destroy(task);
        return NULL;
    }
#else
    /* @bug memory: reads and writes through point_slots[0] and
     * point_works[0] where point_open finds camera 0's stack full, which
     * leaves them null, or as an earlier open set them. point_open tests the
     * push at 0x8c506550. point_step reads through the slot at 0x8c5066e6,
     * and point_ease writes through the work at 0x8c50666c, both untested */
    point_open(0, param, task);
#endif
    collision_init(task, &camera_shape, 1, COLLISION_OBJECT_MUTUAL);
#ifdef NONMATCHING
    /* this side ends the point camera and returns null where collision_init
     * gave the task no record. point_step's collision_register reads that
     * record */
    if (!task->work->collision) {
        point_detach();
        task_destroy(task);
        return NULL;
    }
#endif
    point_task = task;
    return task;
}
