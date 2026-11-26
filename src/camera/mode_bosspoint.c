/* the camera's BOSSPOINT mode: the camera stands behind a player, on the far
 * side from a point the caller keeps, such as a boss */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bosspoint.h"
#include "camera/bosspoint_blend.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/probe.h"
#include "camera/stack.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/vector.h"

/* while the player lies within NEAR of target, the camera looks at the
 * player plus to_target */
#define NEAR 10.0F
/* the cast that finds the ground runs from PROBE_RISE above a point 1 from
 * the player to PROBE_RISE below it */
#define PROBE_RISE 10.0F
/* The first frames look directly at the target before easing begins. */
#define FIRST_STEPS 10

struct bosspoint_work {
    /* the steps left of a blend back to the camera's own place. nothing
     * starts one: camera_mode_bosspoint holds it at 0 */
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
    struct camera_bosspoint param;
};

/* a camera's place and the point it looks at */
struct bosspoint_view {
    NJS_POINT3 pos;
    NJS_POINT3 target;
};

/* the offset from the player of the point the ground cast runs through. the
 * step turns it about y by a quarter turn less the player's ang.y */
static const NJS_VECTOR ground_arm = {0.0F, 0.0F, -1.0F};

/* @hack redundant: rise, which nothing sets but to 0, added to the
 * player's pos.y each step */
static Float rise;
/* each camera's BOSSPOINT work, as camera_bosspoint_start last set it */
static struct bosspoint_work *bosspoint_works[CAMERAS];

int camera_bosspoint_start(int camera, struct camera_bosspoint *param)
{
    struct camera_slot *slot;
    struct bosspoint_work *work;

    if (camera_mode_at(camera, camera_top(camera)) == CAMERA_MODE_INIT) {
        camera_remove_mode(camera, camera_top(camera));
    }
    slot = camera_push_mode(camera, CAMERA_MODE_BOSSPOINT);
    if (slot == NULL) {
        return 0;
    }
    camera_set_adjust(camera, camera_top(camera), param->adjust);
    work = (struct bosspoint_work *)slot->work;
    work->param = *param;
    work->blend_steps = 0;
    work->player = camera_current_index;
    bosspoint_works[camera] = work;
    camera_bosspoint_ease_to(camera, work->param.height, work->param.dist,
                             work->param.part);
    bosspoint_works[camera]->param.blend = CAMERA_BOSSPOINT_BLEND;
    rise = 0.0F;
    return 1;
}

void camera_bosspoint_ease_to(int camera, Float height, Float dist, Float part)
{
    bosspoint_works[camera]->param.goal.x = height;
    bosspoint_works[camera]->param.goal.y = dist;
    bosspoint_works[camera]->param.goal.z = part;
}

void camera_bosspoint_set_level(int camera, int level)
{
    if (level != bosspoint_works[camera]->param.level) {
        bosspoint_works[camera]->param.level = level;
    }
}

void camera_bosspoint_follow(int player)
{
    bosspoint_works[0]->player = player;
}

void camera_mode_bosspoint(struct camera_stack *stack, struct camera_slot *slot)
{
    struct bosspoint_work *work = (struct bosspoint_work *)slot->work;
    struct bosspoint_view view;
    NJS_POINT3 saved;
    NJS_POINT3 pos;
    NJS_VECTOR dir;
    NJS_POINT3 from;
    /* @hack redundant: spare, a local that only sizes the frame */
    NJS_VECTOR spare; /* NOLINT(clang-diagnostic-unused-variable) */
    struct camera_probe probe;
    NJS_POINT3 *point;
    Float length;

    pos = player_work[work->player]->pos;
    pos.y += rise;
    /* @hack redundant: saved, a copy of target written back over it at the
     * end, which nothing between changes */
    saved = *work->param.target;
    work->param.height = BOSSPOINT_EASE(work->param.height, work->param.goal.x);
    work->param.dist = BOSSPOINT_EASE(work->param.dist, work->param.goal.y);
    work->param.part = BOSSPOINT_EASE(work->param.part, work->param.goal.z);
    /* @hack spelling: from set and read through point, where from itself
     * would do */
    point = &from;
    *point = *work->param.target;
    view.target.x = pos.x;
    view.target.y = pos.y;
    view.target.z = pos.z;
    dir.x = pos.x - point->x;
    dir.y = pos.y - point->y;
    dir.z = pos.z - point->z;
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
        view.target.x = BOSSPOINT_TARGET(
            pos.x, work->param.target->x, work->param.part,
            bosspoint_works[camera_current_index]->param.blend);
        view.target.y = BOSSPOINT_TARGET(
            pos.y, work->param.target->y, work->param.part,
            bosspoint_works[camera_current_index]->param.blend);
        view.target.z = BOSSPOINT_TARGET(
            pos.z, work->param.target->z, work->param.part,
            bosspoint_works[camera_current_index]->param.blend);
        view.target.y +=
            BOSSPOINT_LIFT(work->param.lift,
                           bosspoint_works[camera_current_index]->param.blend);
        vector_subtract(work->param.target, &pos, &work->to_target);
    }
    if (bosspoint_works[camera_current_index]->param.level) {
        bosspoint_works[camera_current_index]->param.blend--;
        if (bosspoint_works[camera_current_index]->param.blend <= 0) {
            bosspoint_works[camera_current_index]->param.blend = 0;
        }
    } else {
        bosspoint_works[camera_current_index]->param.blend++;
        if (bosspoint_works[camera_current_index]->param.blend >=
            CAMERA_BOSSPOINT_BLEND) {
            bosspoint_works[camera_current_index]->param.blend =
                CAMERA_BOSSPOINT_BLEND;
        }
    }
    dir = ground_arm;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateY(NULL, QUARTER_TURN - player_work[work->player]->ang.y);
    njCalcVector(NULL, &dir, &dir);
    njPopMatrix(1);
    vector_add(&pos, &dir, &from);
    probe.pos = from;
    probe.pos.y += PROBE_RISE;
    probe.next = from;
    probe.next.y -= PROBE_RISE;
    probe.skip = 0;
    if (camera_probe_cast(&probe)) {
        if (view.pos.y <
            player_records[work->player]->consts.target_height + probe.hit.y) {
            view.pos.y = player_records[work->player]->consts.target_height +
                         probe.hit.y;
        }
    }
    if (slot->steps > FIRST_STEPS) {
        camera_state.target.x = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.x, view.target.x);
        camera_state.target.y = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.y, view.target.y);
        camera_state.target.z = BOSSPOINT_TURN(
            camera_views[camera_current_index]->target.z, view.target.z);
    } else {
        camera_state.target = view.target;
    }
    camera_state.pos = view.pos;
    *work->param.target = saved;
    if (player_work[work->player]->flags &
        (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)) {
        work->grounded_target = *work->param.target;
    }
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.ang.z = 0;
    camera_state.unk_04 = work->param.collides;
    camera_state.trigger_push = work->param.trigger_push;
}
