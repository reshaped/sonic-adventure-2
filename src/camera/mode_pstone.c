/* the camera's PSTONE mode, which keeps two players in sight */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

/* the players' plane runs through both and a point RISE above their
 * midpoint. the camera stands off it along its normal */
#define RISE 10.0F
/* the camera stands off the players' midpoint by their distance apart, but
 * at least NEAREST */
#define NEAREST 50.0F
/* the camera swings where the players stand closer than SWING_DIST. it also
 * swings where the normal it would stand along lies more than SWING_TURN
 * from work->side */
#define SWING_TURN ((Angle)(60 * ANGLE_PER_DEGREE))
#define SWING_DIST 200.0F
/* a swing stands the camera off the midpoint along dir, at that distance. lift
 * scales dir's y, and scale its x and z. each grows from its BASE at
 * SWING_DEGREES, by its PER_DEGREE for each degree of the swing's angle. that
 * is the angle from dir reversed to the camera's line to player 0. scale is
 * held between SCALE_BASE and SCALE_TOP. nothing holds lift, which turns
 * negative below about 21.7 degrees */
#define SWING_DEGREES 26.0F
#define LIFT_BASE 0.3F
#define LIFT_PER_DEGREE 0.07F
#define SCALE_BASE 0.9F
#define SCALE_TOP 1.0F
/* 0.1F * 0.1F, which rounds to the float just above 0.01F */
#define SCALE_PER_DEGREE (0.1F * 0.1F)
/* a swing keeps the camera at least ABOVE_PLAYER above the player nearer
 * it */
#define ABOVE_PLAYER 40.0F
/* each step moves the camera FAST of the way to its goal. it moves SLOW of
 * the way from the step a swing ends, and for SLOW_STEPS steps more. while
 * slot->steps is at most FIRST_STEPS, it stands at its goal */
#define SLOW 0.05F
#define FAST 0.2F
#define SLOW_STEPS 30
#define FIRST_STEPS 10

struct pstone_work {
    /* the unit vector from the players' midpoint toward the camera's goal
     * at the last step that didn't swing. the first step takes it from the
     * zone's point toward its camera_pos */
    NJS_VECTOR dir;
    /* the target_height of the current camera's player at the last step */
    Float target_height;
    Sint32 swung; /* 1 where the last step swung the camera, else 0 */
    Sint32 slow_steps; /* the steps the camera still moves at SLOW */
    /* dir as the first step found it, with a y of 0 */
    NJS_VECTOR side;
};

void camera_mode_pstone(struct camera_stack *stack, struct camera_slot *slot)
{
    struct pstone_work *work;
    struct camera_zone *zone;
    /* @hack spelling: the point between the players and the plane's normal
     * as one NJS_PLANE, where two vectors would do */
    NJS_PLANE plane;
    NJS_POINT3 corners[3];
    NJS_POINT3 pos;
    NJS_VECTOR apart;
    NJS_VECTOR side;
    NJS_VECTOR back;
    NJS_VECTOR toward;
    /* @hack spelling: the lines to the two players as an array, where two
     * vectors would do */
    NJS_VECTOR to[2];
    Float dist;
    Float degrees;
    Float lift;
    Float scale;
    Float rate;
    Float height;
    Float rise;
    Angle ang;
    int nearer;
    int swung;

    work = (struct pstone_work *)slot->work;
    zone = slot->zone;
    if (slot->steps == 0) {
        vector_subtract(&zone->camera_pos, &zone->point, &work->dir);
        njUnitVector(&work->dir);
        work->slow_steps = 0;
        work->side = work->dir;
        work->side.y = 0.0F;
        /* @hack redundant: a call for side's length, which nothing takes */
        njScalor(&work->side);
    }
    if (player_tasks[1] == NULL) {
        return;
    }
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    /* @hack redundant: height read here, which the end reads again before
     * anything takes it */
    height = player_records[camera_current_index]->consts.target_height;
    camera_state.target.x =
        (player_work[0]->pos.x + player_work[1]->pos.x) * 0.5F;
    camera_state.target.y =
        (player_work[0]->pos.y + player_work[1]->pos.y) * 0.5F;
    camera_state.target.z =
        (player_work[0]->pos.z + player_work[1]->pos.z) * 0.5F;
    corners[0] = player_work[0]->pos;
    corners[1] = player_work[1]->pos;
    corners[2] = camera_state.target;
    corners[2].y += RISE;
    njGetPlaneNormal(corners, (NJS_VECTOR *)&plane.vx);
    plane.px = camera_state.target.x;
    plane.py = camera_state.target.y;
    plane.pz = camera_state.target.z;
    njUnitVector((NJS_VECTOR *)&plane.vx);
    vector_subtract(&player_work[0]->pos, &player_work[1]->pos, &apart);
    dist = njScalor(&apart);
    if (dist < NEAREST) {
        dist = NEAREST;
    }
    corners[0].x = plane.px + (plane.vx * dist);
    corners[0].y = plane.py;
    corners[0].z = plane.pz + (plane.vz * dist);
    corners[1].x = plane.px - (plane.vx * dist);
    corners[1].y = plane.py;
    corners[1].z = plane.pz - (plane.vz * dist);
    if (vector_angle(&work->side, (NJS_VECTOR *)&plane.vx, NULL) <
        QUARTER_TURN) {
        pos = corners[0];
    } else {
        pos = corners[1];
        plane.vx *= -1.0F;
        plane.vz *= -1.0F;
    }
    side = work->side;
    /* @bug mismatched-call: vector_angle takes unit vectors, but side is
     * shorter than one wherever the zone's camera_pos lies above or below its
     * point: it is dir with its y set to 0, never scaled again. the angle
     * then comes out nearer a quarter turn than it is, so the swing starts
     * short of SWING_TURN. the call is at 0x8c0b74c0. the one below, at
     * 0x8c0b7520, takes back, which is dir flattened the same way */
    if (vector_angle(&side, (NJS_VECTOR *)&plane.vx, NULL) > SWING_TURN ||
        dist < SWING_DIST) {
        /* @hack redundant: dir's x and z times 1 */
        back.x = -(work->dir.x * 1.0F);
        back.y = 0.0F;
        back.z = -(work->dir.z * 1.0F);
        swung = 1;
        vector_subtract(&player_work[0]->pos,
                        &camera_views[camera_current_index]->pos, &toward);
        toward.y = 0.0F;
        njUnitVector(&toward);
        ang = vector_angle(&toward, &back, NULL);
        degrees = ang * (360.0F / FULL_TURN);
        lift = ((degrees - SWING_DEGREES) * LIFT_PER_DEGREE) + LIFT_BASE;
        pos.y = camera_state.target.y + (work->dir.y * dist * lift);
        scale = ((degrees - SWING_DEGREES) * SCALE_PER_DEGREE) + SCALE_BASE;
        if (scale > SCALE_TOP) {
            scale = SCALE_TOP;
        } else if (scale < SCALE_BASE) {
            scale = SCALE_BASE;
        }
        pos.x = camera_state.target.x + (work->dir.x * dist * scale);
        pos.z = camera_state.target.z + (work->dir.z * dist * scale);
        vector_subtract(&player_work[0]->pos, &pos, &to[0]);
        vector_subtract(&player_work[1]->pos, &pos, &to[1]);
        to[0].y = 0.0F;
        to[1].y = 0.0F;
        if (njScalor2(&to[0]) < njScalor2(&to[1])) {
            nearer = 0;
        } else {
            nearer = 1;
        }
        if (player_work[nearer]->pos.y + ABOVE_PLAYER > pos.y) {
            pos.y = player_work[nearer]->pos.y + ABOVE_PLAYER;
        }
        /* @hack redundant: rise, which nothing reads */
        rise = player_records[camera_current_index]->consts.target_height *
               (scale - SCALE_BASE) * 10.0F;
        if (player_records[camera_current_index]->consts.target_height > rise) {
            rise = player_records[camera_current_index]->consts.target_height;
        }
    } else {
        pos.y += dist * 0.5F;
        swung = 0;
        vector_subtract(&pos, &camera_state.target, &work->dir);
        njUnitVector(&work->dir);
    }
    if (work->slow_steps > 0) {
        work->slow_steps--;
        rate = SLOW;
    } else if (work->swung == 1 && swung == 0) {
        rate = SLOW;
        work->slow_steps = SLOW_STEPS;
    } else {
        rate = FAST;
        work->slow_steps = 0;
    }
    if (slot->steps > FIRST_STEPS) {
        camera_state.pos.x =
            camera_views[camera_current_index]->pos.x +
            ((pos.x - camera_views[camera_current_index]->pos.x) * rate);
        camera_state.pos.y =
            camera_views[camera_current_index]->pos.y +
            ((pos.y - camera_views[camera_current_index]->pos.y) * rate);
        camera_state.pos.z =
            camera_views[camera_current_index]->pos.z +
            ((pos.z - camera_views[camera_current_index]->pos.z) * rate);
    } else {
        camera_state.pos = pos;
    }
    height = player_records[camera_current_index]->consts.target_height;
    camera_state.target.y += height;
    work->target_height = height;
    work->swung = swung;
}
