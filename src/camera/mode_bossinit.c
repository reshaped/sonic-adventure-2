/* the camera's BOSSINIT mode: a boss's opening shot, which closes in on a
 * point and then turns about it */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/bossinit.h"
#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "system/vector.h"

/* the mode's phases, in the order its frames take it through them */
#define PHASE_START 0
#define PHASE_CLOSE 1
#define PHASE_TURN 2
#define PHASE_STAY 3
#define PHASE_END 4

struct bossinit_work {
    Float frame;
    Sint32 phase; /* a PHASE_ */
    Sint32 camera; /* the camera whose stack holds the mode */
    struct camera_bossinit param;
};

int camera_bossinit_start(int camera, struct camera_bossinit *param)
{
    struct camera_slot *slot;
    struct bossinit_work *work;

    if (camera_mode_at(camera, camera_top(camera)) == CAMERA_MODE_INIT) {
        camera_remove_mode(camera, camera_top(camera));
    }
    slot = camera_push_mode(camera, CAMERA_MODE_BOSSINIT);
    if (slot == NULL) {
        return 0;
    }
    camera_set_adjust(camera, camera_top(camera), CAMERA_ADJUST_NONE);
    work = (struct bossinit_work *)slot->work;
    work->frame = 0.0F;
    work->param = *param;
    work->phase = PHASE_START;
    work->camera = camera;
    return 1;
}

void camera_mode_bossinit(struct camera_stack *stack, struct camera_slot *slot)
{
    struct bossinit_work *work = (struct bossinit_work *)slot->work;
    NJS_VECTOR arm;
    Float part;

    if (work->phase == PHASE_START) {
        work->phase = PHASE_CLOSE;
    }
    work->frame += work->param.speed;
    if (work->frame > CAMERA_BOSSINIT_CLOSE_END) {
        work->phase = PHASE_TURN;
    }
    if (work->frame > CAMERA_BOSSINIT_TURN_END) {
        work->phase = PHASE_STAY;
    }
    if (work->frame > CAMERA_BOSSINIT_STAY_END) {
        work->phase = PHASE_END;
    }
    switch (work->phase) {
    case PHASE_CLOSE:
        part = work->frame * (1.0F / CAMERA_BOSSINIT_CLOSE_END);
        arm.x = ((work->param.start_dist - work->param.dist) * (1.0F - part)) +
                work->param.dist;
        arm.y = 0.0F;
        arm.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->param.ang);
        njCalcVector(NULL, &arm, &arm);
        camera_state.pos = *work->param.target;
        camera_state.target = *work->param.target;
        vector_add(&camera_state.pos, &arm, &camera_state.pos);
        njPopMatrixEx();
        break;
    case PHASE_TURN:
    case PHASE_STAY:
        if (work->phase == PHASE_TURN) {
            part =
                (work->frame - CAMERA_BOSSINIT_CLOSE_END) *
                (1.0F / (CAMERA_BOSSINIT_TURN_END - CAMERA_BOSSINIT_CLOSE_END));
        } else {
            part = 1.0F;
        }
        arm.x = work->param.dist;
        arm.y = 0.0F;
        arm.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, (Angle)(work->param.ang + (work->param.turn * part)));
        njCalcVector(NULL, &arm, &arm);
        camera_state.pos = *work->param.target;
        camera_state.target = *work->param.target;
        vector_add(&camera_state.pos, &arm, &camera_state.pos);
        njPopMatrixEx();
        break;
    case PHASE_END:
        camera_bossinit_end(work->camera);
        break;
    }
    camera_state.type = CAMERA_POS_TARGET;
    camera_state.unk_04 = 0;
    camera_state.ang.z = 0;
    camera_state.trigger_push = 0;
}

void camera_bossinit_end(int camera)
{
    camera_remove_mode(camera, camera_top(camera));
    camera_set_adjust(camera, camera_top(camera), CAMERA_ADJUST_NONE);
}
