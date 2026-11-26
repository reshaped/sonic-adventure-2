/* the camera's MOTION mode, which plays a Ninja camera motion on a camera */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/gakugaku.h"
#include "camera/modes.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "system/game_state.h"
#include "system/vector.h"

/* the calls of camera_settle_step that the current camera waits out once a
 * motion ends */
#define SETTLE_DELAY 60

struct motion_work {
    Float frame;
    /* the point the motion plays about, as camera_motion_start copied it.
     * each step copies it again under CAMERA_MOTION_FOLLOWS */
    NJS_POINT3 origin;
    struct camera_motion motion;
    /* the current camera's place as camera_motion_start found it */
    struct camera_place start;
};

/* each camera's MOTION work, as camera_motion_start last set it */
static struct motion_work *motion_works[CAMERAS];

int camera_motion_start(int camera, struct camera_motion *motion)
{
    struct camera_slot *slot;
    struct motion_work *work;

#ifdef NONMATCHING
    /* Leave the current camera in place when its replacement has no keys. */
    if (motion == NULL || motion->motion == NULL ||
        motion->motion->nbFrame == 0) {
        return 0;
    }
#endif
    slot = camera_set_mode(camera, CAMERA_MOTION_SLOT, CAMERA_MODE_MOTION);
    if (slot == NULL) {
        return 0;
    }
    /* @hack spelling: the adjust set by a call in each branch, where one
     * call that picks the adjust with ?: would do */
    if (motion->flags & CAMERA_MOTION_START_BLENDS) {
        camera_set_adjust(camera, CAMERA_MOTION_SLOT, CAMERA_ADJUST_RELATIVE1);
    } else {
        camera_set_adjust(camera, CAMERA_MOTION_SLOT, CAMERA_ADJUST_NONE);
    }
    work = (struct motion_work *)slot->work;
    motion_works[camera] = work;
    work->frame = 0.0F;
    work->start = *camera_views[camera_current_index];
    if (motion->origin != NULL) {
        work->origin = *motion->origin;
    }
    work->motion = *motion;
    game_state.camera_motion_playing = 1;
    return 1;
}

void camera_mode_motion(struct camera_stack *stack, struct camera_slot *slot)
{
    struct motion_work *work = (struct motion_work *)slot->work;
    Bool ended = FALSE;
    NJS_CAMERA camera;
    NJS_CMOTION_DATA data;
    NJS_VECTOR arm;
    NJS_POINT3 center;
    Angle ang;

    njUnitMatrix(NULL);
    if ((work->motion.flags & CAMERA_MOTION_FOLLOWS) &&
        work->motion.origin != NULL) {
        work->origin = *work->motion.origin;
    }
    work->frame += work->motion.speed;
    if (work->frame + work->motion.speed >
        (Float)(work->motion.motion->nbFrame - 1)) {
        if (work->motion.flags & CAMERA_MOTION_HOLD) {
            work->frame = (Float)(work->motion.motion->nbFrame - 1);
        } else if (work->motion.flags & CAMERA_MOTION_LOOP) {
            work->frame = 0.0F;
        } else {
            ended = TRUE;
        }
    }
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    data.pos = &camera_state.pos.x;
    data.vect = &camera_state.dir.x;
    data.roll = &camera_state.ang.z;
    data.ang = &ang;
    njInitCamera(&camera);
    njGetCameraMotion(&camera, work->motion.motion, &data, work->frame);
    if (work->motion.origin != NULL) {
        vector_add(&camera_state.pos, &work->origin, &camera_state.pos);
        vector_add(&camera_state.dir, &work->origin, &camera_state.target);
        if (work->motion.flags & CAMERA_MOTION_TURNS) {
            center = *work->motion.origin;
        } else {
            center = camera_state.target;
        }
        vector_subtract(&camera_state.pos, &center, &arm);
        njPushMatrix(NULL);
        njUnitMatrix(NULL);
        njRotateY(NULL, work->motion.ang);
        njCalcVector(NULL, &arm, &camera_state.pos);
        vector_add(&camera_state.pos, &center, &camera_state.pos);
        if (work->motion.flags & CAMERA_MOTION_TURNS) {
            vector_subtract(&camera_state.target, &center, &arm);
            njUnitMatrix(NULL);
            njRotateY(NULL, work->motion.ang);
            njCalcVector(NULL, &arm, &camera_state.target);
            vector_add(&camera_state.target, &center, &camera_state.target);
        }
        njPopMatrix(1);
    }
    if (ended) {
        camera_motion_end(camera_current_index);
    }
}

#ifdef NONMATCHING
int camera_motion_skip(int camera)
{
    struct motion_work *work = motion_works[camera];
    Float last;
    Float next;
    int steps;

    if (work == NULL || camera_top(camera) != CAMERA_MOTION_SLOT ||
        (work->motion.flags & (CAMERA_MOTION_HOLD | CAMERA_MOTION_LOOP)) ||
        work->motion.speed <= 0.0F) {
        return 0;
    }
    /* take camera_mode_motion's steps, up to the one that would end the
     * motion */
    last = (Float)(work->motion.motion->nbFrame - 1);
    for (steps = 0;; steps++) {
        next = work->frame + work->motion.speed;
        if (next + work->motion.speed > last) {
            break;
        }
        work->frame = next;
    }
    return steps;
}
#endif

void camera_motion_end(int camera)
{
    struct motion_work *work = motion_works[camera];

    if (camera_top(camera) == CAMERA_MOTION_SLOT) {
        if ((work->motion.flags & CAMERA_MOTION_CALLS_DONE) &&
            work->motion.done != NULL) {
            work->motion.done();
        }
        camera_remove_mode(camera, CAMERA_MOTION_SLOT);
        if (work->motion.flags & CAMERA_MOTION_END_BLENDS) {
            camera_set_adjust(camera, camera_top(camera),
                              CAMERA_ADJUST_RELATIVE1);
        } else {
            camera_set_adjust(camera, camera_top(camera), CAMERA_ADJUST_NONE);
            camera_state.pos = work->start.pos;
            camera_state.target = work->start.target;
        }
    }
    game_state.camera_motion_playing = 0;
    camera_delay_settle(camera_current_index, SETTLE_DELAY);
}
