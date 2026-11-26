/* the golem fights' camera, a machine of modes. each mode places camera 0 about
 * the golem and player 0. a mode has a start, which runs as the mode becomes
 * current, and a step */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bossinit.h"
#include "camera/bosspoint.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "player/player.h"
#include "shared/boss.h"
#include "shared/golem.h"
#include "shared/golem_camera.h"
#include "shared/golem_util.h"
#include "system/pad.h"
#include "system/task.h"

#define GOLEM(task)                                                            \
    ((struct golem *)((struct boss_record *)(task)->extra.p)->own)

/* a GOLEM_CAMERA_ mode that steers camera 0 by CAMERA_MODE_EASYSET puts
 * CAMERA_MODE_EASYSET at EASYSET_INDEX of camera 0's stack. its step fills
 * EASYSET, the state CAMERA_MODE_EASYSET gives camera 0 */
#define EASYSET_INDEX 2
#define EASYSET (&camera_easyset_states[0])

/* the ground below player 0 becomes floor_goal only above FLOOR_MIN */
#define FLOOR_MIN 100.0F
/* GOLEM_CAMERA_FAR gives way to GOLEM_CAMERA_FAR_TURNED where
 * golem_turn_to_player lies beyond TURNED_AWAY either way.
 * GOLEM_CAMERA_FAR_TURNED gives way back within FACING */
#define TURNED_AWAY ((Float)0x6800)
#define FACING ((Float)0x6000)

/* GOLEM_CAMERA_FAR's and GOLEM_CAMERA_FAR_TURNED's pitch, height and dist.
 * each mode looks at the golem's pos, with floor_y plus the mode's height for
 * the y. GOLEM_CAMERA_HANG takes far_pitch too, and GOLEM_CAMERA_CLOSE
 * turned_pitch */
static Angle far_pitch = 0xff00;
static Float far_height = 40.0F;
static Float far_dist = 330.0F;
static Angle turned_pitch = 0xec00;
static Float turned_height = -70.0F;
static Float turned_dist = 350.0F;
/* GOLEM_CAMERA_INTRO's shot closes in from intro_start_dist to intro_dist on
 * the point intro_height above the golem's pos. it then turns about the
 * point */
static Float intro_height = 280.0F;
static Float intro_start_dist = 375.0F;
static Float intro_dist = 300.0F;
/* GOLEM_CAMERA_POINT's point lies point_radius from the arena's center, along
 * player 0's heading. a negative radius puts it across the center from the
 * player. its y is point_height_share of the player's */
static Float point_radius = -100.0F;
static Float point_height_share = 0.5F;
/* GOLEM_CAMERA_POINT and GOLEM_CAMERA_POINT_CLOSE take player 0's heading
 * only beyond aim_min of the arena's center, along x and z.
 * GOLEM_CAMERA_CLOSE sets EASYSET's angles only beyond aim_min of
 * weak_spot_pos, along x, y and z */
static Float aim_min = 35.0F;
static Angle dat_golem_camera_0 = 0x4000;

static void camera_intro_start(struct task *task);
static void camera_intro_step(struct task *task);
static void camera_far_start(struct task *task);
static void camera_far_step(struct task *task);
static void camera_close_start(struct task *task);
static void camera_close_step(struct task *task);
static void camera_point_start(struct task *task);
static void camera_point_step(struct task *task);
static void camera_far_turned_start(struct task *task);
static void camera_far_turned_step(struct task *task);
static void camera_point_close_start(struct task *task);
static void camera_point_close_step(struct task *task);
static void camera_fixed_start(struct task *task);
static void camera_fixed_step(struct task *task);
static void camera_low_start(struct task *task);
static void camera_low_step(struct task *task);
static void camera_hang_start(struct task *task);
static void camera_hang_step(struct task *task);

static void (*mode_starts[GOLEM_CAMERA_MODES])(struct task *task) = {
    camera_intro_start, camera_far_start,        camera_close_start,
    camera_point_start, camera_far_turned_start, camera_point_close_start,
    camera_fixed_start, camera_low_start,        camera_hang_start,
};
static void (*mode_steps[GOLEM_CAMERA_MODES])(struct task *task) = {
    camera_intro_step, camera_far_step,        camera_close_step,
    camera_point_step, camera_far_turned_step, camera_point_close_step,
    camera_fixed_step, camera_low_step,        camera_hang_step,
};

/* GOLEM_CAMERA_INTRO's camera_bossinit speed and turn, and its ang less the
 * golem's ang.y */
static Float intro_speed = 0.8F;
static Angle intro_ang = 0x5800;
static Angle intro_turn = 0x6800;
/* floor_y moves 1 / (1 + floor_ease) of the way to floor_goal each step */
static Float floor_ease = 8.0F;
/* GOLEM_CAMERA_CLOSE looks at the mean of weak_spot_pos, weighted
 * close_spot_weight, and player 0's pos, weighted 1. its dist is
 * close_dist_scale times the player's distance from weak_spot_pos, plus
 * close_dist_add. the dist is never below close_dist_min */
static Float close_dist_scale = 1.0F;
static Float close_dist_add = 50.0F;
static Float close_spot_weight = 0.5F;
static Float close_dist_min = 75.0F;
static Float dat_golem_camera_1 = -100.0F;
/* GOLEM_CAMERA_POINT's camera_bosspoint dist and height */
static Float point_dist = 120.0F;
static Float point_height = 0.0F;
static Float dat_golem_camera_2 = 0.5F;
static Float dat_golem_camera_3 = -50.0F;
static Float dat_golem_camera_4 = 120.0F;
static Float dat_golem_camera_5 = 70.0F;
/* GOLEM_CAMERA_POINT_CLOSE's tuning, as GOLEM_CAMERA_POINT's */
static Float point_close_radius = -100.0F;
static Float point_close_height_share = 1.0F;
static Float point_close_dist = 50.0F;
static Float point_close_height = 0.0F;
/* GOLEM_CAMERA_FIXED looks at weak_spot_pos from the angles fixed_pitch and
 * fixed_yaw */
static Float fixed_dist = 100.0F;
static Float fixed_height = -25.0F;
static Angle fixed_pitch = 0xfe00;
static Angle fixed_yaw = 0xfc00;
/* GOLEM_CAMERA_LOW looks at a point along player 0's heading from the arena's
 * center. the point's distance from the center is linear in the player's. it
 * is low_radius_min for low_player_min, and low_radius_max for
 * low_player_max. the line isn't clamped at either end */
static Angle low_pitch = 0xf000;
static Float low_height = -30.0F;
static Float low_dist = 225.0F;
static Float low_radius_min = 125.0F;
static Float low_radius_max = 155.0F;
static Float low_player_min = 50.0F;
static Float low_player_max = 300.0F;
static Float dat_golem_camera_6 = -100.0F;
static Float dat_golem_camera_7 = 0.75F;
static Float dat_golem_camera_8 = 60.0F;
static Float dat_golem_camera_9 = 0.0F;
/* GOLEM_CAMERA_HANG looks at the golem's pos, at player 0's height */
static Float hang_dist = 310.0F;

void golem_camera_set(struct task *task, int mode)
{
    GOLEM(task)->camera_mode = mode;
    GOLEM(task)->camera_next = GOLEM_CAMERA_NONE;
    GOLEM(task)->camera_last = mode;
    GOLEM(task)->camera_steps = 0;
    mode_starts[GOLEM(task)->camera_mode](task);
}

void golem_camera_step(struct task *task)
{
    if (GOLEM(task)->camera_next != GOLEM_CAMERA_NONE) {
        GOLEM(task)->camera_last = GOLEM(task)->camera_mode;
        GOLEM(task)->camera_mode = GOLEM(task)->camera_next;
        GOLEM(task)->camera_next = GOLEM_CAMERA_NONE;
        GOLEM(task)->camera_steps = 0;
        if (camera_mode_at(0, camera_top(0)) == CAMERA_MODE_BOSSPOINT) {
            camera_remove_mode(0, camera_top(0));
            camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
        }
        mode_starts[GOLEM(task)->camera_mode](task);
    }
    mode_steps[GOLEM(task)->camera_mode](task);
    GOLEM(task)->camera_steps++;
}

#pragma inline(golem_camera_request)
void golem_camera_request(struct task *task, int mode)
{
    GOLEM(task)->camera_next = mode;
}

static void camera_intro_start(struct task *task)
{
    struct task_work *work = task->work;
    struct boss_record *record = task->extra.p;
    struct camera_bossinit camera;
    NJS_VECTOR offset;

    camera.speed = intro_speed;
    camera.ang = work->ang.y + intro_ang;
    /* @hack spelling: offset's x and z cleared in one chained assignment,
     * where two would do */
    offset.x = offset.z = 0.0F;
    offset.y = intro_height;
    boss_record_set_focus(record, &work->pos, &offset);
    camera.target = &record->focus;
    camera.start_dist = intro_start_dist;
    camera.dist = intro_dist;
    camera.turn = intro_turn;
    camera_bossinit_start(0, &camera);
    pad_disable_player(0);
}

/* camera 0's BOSSINIT mode runs the shot by itself */
static void camera_intro_step(struct task *task)
{
}

static void camera_far_start(struct task *task)
{
    pad_enable_player(0);
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    if (GOLEM(task)->camera_far_started) {
        camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
    }
    GOLEM(task)->camera_far_started = TRUE;
}

static void camera_far_step(struct task *task)
{
    struct task_work *work = task->work;

    if (player_records[0]->ground.below_y > FLOOR_MIN) {
        GOLEM(task)->floor_goal = player_records[0]->ground.below_y;
    }
    GOLEM(task)->floor_y = golem_weighted_mean(
        GOLEM(task)->floor_y, GOLEM(task)->floor_goal, 1.0F, floor_ease);
    EASYSET->target = work->pos;
    EASYSET->target.y = GOLEM(task)->floor_y + far_height;
    EASYSET->ang.x = far_pitch;
    EASYSET->ang.y = golem_player_heading();
    EASYSET->ang.z = 0;
    EASYSET->dist = far_dist;
    EASYSET->type = CAMERA_TARGET_ANG_DIST;
    EASYSET->unk_04 = 0;
    EASYSET->trigger_push = 0;
    if (player_work[0]->mode != PLAYER_MODE_HANG && golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_LOW);
    } else if (golem_player_near(task)) {
        golem_camera_request(task, GOLEM_CAMERA_POINT);
    } else if (!GOLEM(task)->dark_story &&
               njAbs((Float)golem_turn_to_player()) > TURNED_AWAY) {
        golem_camera_request(task, GOLEM_CAMERA_FAR_TURNED);
    }
}

static void camera_close_start(struct task *task)
{
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE5);
}

static void camera_close_step(struct task *task)
{
    /* @hack spelling: camera set before its first use and again before each
     * later one, where EASYSET at each use would do */
    struct camera_state *camera = camera_easyset_states;
    Float apart;
    Float dist;

    apart = njDistanceP2P(&GOLEM(task)->weak_spot_pos, &player_work[0]->pos);
    EASYSET->target.x =
        golem_weighted_mean(GOLEM(task)->weak_spot_pos.x, player_work[0]->pos.x,
                            1.0F, close_spot_weight);
    EASYSET->target.y =
        golem_weighted_mean(GOLEM(task)->weak_spot_pos.y, player_work[0]->pos.y,
                            1.0F, close_spot_weight);
    EASYSET->target.z =
        golem_weighted_mean(GOLEM(task)->weak_spot_pos.z, player_work[0]->pos.z,
                            1.0F, close_spot_weight);
    if (apart > aim_min) {
        camera = camera_easyset_states;
        camera->ang.x = turned_pitch;
        camera->ang.y = golem_heading(player_work[0]->pos.x - camera->target.x,
                                      player_work[0]->pos.z - camera->target.z);
        camera->ang.z = 0;
    }
    dist = (apart * close_dist_scale) + close_dist_add;
    if (dist < close_dist_min) {
        dist = close_dist_min;
    }
    camera = camera_easyset_states;
    /* @hack redundant: a test no path reaches, a third read of dist */
    if (0) {
        if (dist) {
        }
    }
    camera->dist = dist;
    camera->type = CAMERA_TARGET_ANG_DIST;
    camera->unk_04 = 0;
    camera->trigger_push = 0;
    if (GOLEM(task)->throwing) {
        golem_camera_request(task, GOLEM_CAMERA_POINT_CLOSE);
    } else if (golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_LOW);
    } else if (!golem_player_close(task)) {
        golem_camera_request(task, GOLEM_CAMERA_POINT);
    }
}

static void camera_point_start(struct task *task)
{
    struct camera_bosspoint camera;

    GOLEM(task)->camera_heading = golem_player_heading();
    golem_polar_xz(GOLEM(task)->camera_heading, point_radius,
                   &GOLEM(task)->camera_point.x, &GOLEM(task)->camera_point.z);
    GOLEM(task)->camera_point.y = player_work[0]->pos.y * point_height_share;
    camera.target = &GOLEM(task)->camera_point;
    camera.dist = point_dist;
    camera.height = point_height;
    camera.part = 1.0F;
    camera.adjust = CAMERA_ADJUST_RELATIVE5;
    camera.collides = 0;
    camera.trigger_push = 0;
    camera.lift = 0.0F;
    camera.level = 0;
    pad_enable_player(0);
    camera_bosspoint_start(0, &camera);
}

static void camera_point_step(struct task *task)
{
    Float x = player_work[0]->pos.x;
    Float z = player_work[0]->pos.z;
    Float apart = njSqrt((x * x) + (z * z));

    if (apart > aim_min) {
        GOLEM(task)->camera_heading = golem_player_heading();
    }
    golem_polar_xz(GOLEM(task)->camera_heading, point_radius,
                   &GOLEM(task)->camera_point.x, &GOLEM(task)->camera_point.z);
    GOLEM(task)->camera_point.y = player_work[0]->pos.y * point_height_share;
    if (golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_LOW);
    } else if (!golem_player_near(task)) {
        if (!GOLEM(task)->throwing) {
            if (!GOLEM(task)->dark_story &&
                njAbs((Float)golem_turn_to_player()) > TURNED_AWAY) {
                golem_camera_request(task, GOLEM_CAMERA_FAR_TURNED);
            } else {
                golem_camera_request(task, GOLEM_CAMERA_FAR);
            }
        }
    } else if (!GOLEM(task)->dark_story && golem_player_close(task)) {
        golem_camera_request(task, GOLEM_CAMERA_CLOSE);
    }
}

static void camera_far_turned_start(struct task *task)
{
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
}

static void camera_far_turned_step(struct task *task)
{
    struct task_work *work = task->work;

    if (player_records[0]->ground.below_y > FLOOR_MIN) {
        GOLEM(task)->floor_goal = player_records[0]->ground.below_y;
    }
    GOLEM(task)->floor_y = golem_weighted_mean(
        GOLEM(task)->floor_y, GOLEM(task)->floor_goal, 1.0F, floor_ease);
    EASYSET->target = work->pos;
    EASYSET->target.y = GOLEM(task)->floor_y + turned_height;
    EASYSET->ang.x = turned_pitch;
    EASYSET->ang.y = golem_player_heading();
    EASYSET->ang.z = 0;
    EASYSET->dist = turned_dist;
    EASYSET->type = CAMERA_TARGET_ANG_DIST;
    EASYSET->unk_04 = 0;
    EASYSET->trigger_push = 0;
    if (golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_LOW);
    } else if (golem_player_near(task)) {
        golem_camera_request(task, GOLEM_CAMERA_POINT);
    } else if (njAbs((Float)golem_turn_to_player()) < FACING) {
        golem_camera_request(task, GOLEM_CAMERA_FAR);
    }
}

static void camera_point_close_start(struct task *task)
{
    struct camera_bosspoint camera;

    GOLEM(task)->camera_heading = golem_player_heading();
    golem_polar_xz(GOLEM(task)->camera_heading, point_close_radius,
                   &GOLEM(task)->camera_point.x, &GOLEM(task)->camera_point.z);
    GOLEM(task)->camera_point.y =
        player_work[0]->pos.y * point_close_height_share;
    camera.target = &GOLEM(task)->camera_point;
    camera.dist = point_close_dist;
    camera.height = point_close_height;
    camera.part = 1.0F;
    camera.adjust = CAMERA_ADJUST_RELATIVE4;
    camera.collides = 0;
    camera.trigger_push = 0;
    camera.lift = 0.0F;
    camera.level = 0;
    pad_enable_player(0);
    camera_bosspoint_start(0, &camera);
}

static void camera_point_close_step(struct task *task)
{
    Float x = player_work[0]->pos.x;
    Float z = player_work[0]->pos.z;
    Float apart;

    apart = njSqrt((x * x) + (z * z));
    if (apart > aim_min) {
        GOLEM(task)->camera_heading = golem_player_heading();
    }
    golem_polar_xz(GOLEM(task)->camera_heading, point_close_radius,
                   &GOLEM(task)->camera_point.x, &GOLEM(task)->camera_point.z);
    GOLEM(task)->camera_point.y =
        player_work[0]->pos.y * point_close_height_share;
    if (golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_LOW);
    } else if (!golem_player_near(task) && !GOLEM(task)->throwing) {
        if (!GOLEM(task)->dark_story &&
            njAbs((Float)golem_turn_to_player()) > TURNED_AWAY) {
            golem_camera_request(task, GOLEM_CAMERA_FAR_TURNED);
        } else {
            golem_camera_request(task, GOLEM_CAMERA_FAR);
        }
    }
}

static void camera_fixed_start(struct task *task)
{
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
}

static void camera_fixed_step(struct task *task)
{
    EASYSET->target = GOLEM(task)->weak_spot_pos;
    EASYSET->target.y += fixed_height;
    EASYSET->ang.x = fixed_pitch;
    EASYSET->ang.y = fixed_yaw;
    EASYSET->ang.z = 0;
    EASYSET->dist = fixed_dist;
    EASYSET->type = CAMERA_TARGET_ANG_DIST;
    EASYSET->unk_04 = 0;
    EASYSET->trigger_push = 0;
}

static void camera_low_start(struct task *task)
{
    pad_enable_player(0);
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
}

static void camera_low_step(struct task *task)
{
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 target;
    Float part;
    Float range;
    Float apart;
    Float spread;
    Float radius;
    Angle heading;

    player = player_work[0]->pos;
    apart = njSqrt((player.x * player.x) + (player.z * player.z));
    range = low_player_max - low_player_min;
    spread = low_radius_max - low_radius_min;
    part = (apart - low_player_min) / range;
    radius = low_radius_min + (spread * part);
    heading = golem_player_heading();
    golem_polar_xz(heading, radius, &target.x, &target.z);
    target.y = player.y + low_height;
    EASYSET->target = target;
    EASYSET->ang.x = low_pitch;
    EASYSET->ang.y = heading;
    EASYSET->ang.z = 0;
    EASYSET->dist = low_dist;
    EASYSET->type = CAMERA_TARGET_ANG_DIST;
    EASYSET->unk_04 = 0;
    EASYSET->trigger_push = 0;
    if (player_work[0]->mode == PLAYER_MODE_HANG) {
        golem_camera_request(task, GOLEM_CAMERA_HANG);
    } else if (!golem_player_low(task)) {
        golem_camera_request(task, GOLEM_CAMERA_FAR);
    }
}

static void camera_hang_start(struct task *task)
{
    camera_set_mode(0, EASYSET_INDEX, CAMERA_MODE_EASYSET);
    camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_RELATIVE3);
}

static void camera_hang_step(struct task *task)
{
    struct task_work *work = task->work;

    EASYSET->target = work->pos;
    EASYSET->target.y = player_work[0]->pos.y;
    EASYSET->ang.x = far_pitch;
    EASYSET->ang.y = golem_player_heading();
    EASYSET->ang.z = 0;
    EASYSET->dist = hang_dist;
    EASYSET->type = CAMERA_TARGET_ANG_DIST;
    EASYSET->unk_04 = 0;
    EASYSET->trigger_push = 0;
    if (player_work[0]->mode == PLAYER_MODE_HANG && golem_player_low(task)) {
        return;
    }
    golem_camera_request(task, GOLEM_CAMERA_FAR);
}
