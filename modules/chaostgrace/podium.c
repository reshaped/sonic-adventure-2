/* the podium's camera. it moves from podium_pan[0] to podium_pan[1] by
 * PAN_STEP of the way a frame. it looks at podium_pan[0] less LOOK_SHIFT in
 * x */
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chaostgrace/camera.h"

static NJS_POINT3 podium_pan[2] = {
#include "chaostgrace/podium/podium_pan.inc"
};
enum podium_mode { PODIUM_START, PODIUM_PAN, PODIUM_STILL };
#define PAN_STEP 0.002F
#define LOOK_SHIFT 5.0F

void race_camera_podium(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    struct camera_state *view = &camera->view;
    Float phase;

    switch (camera->mode) {
    case PODIUM_START:
        camera->mode = PODIUM_PAN;
        camera->frame = 0.0F;
        /* fall through */
    case PODIUM_PAN:
        camera->frame += PAN_STEP;
        if (camera->frame > 1.0F) {
            camera->frame = 1.0F;
            camera->mode = PODIUM_STILL;
        }
        break;
    case PODIUM_STILL:
        break;
    }
    phase = camera->frame;
    view->pos.x =
        podium_pan[0].x + (phase * (podium_pan[1].x - podium_pan[0].x));
    view->pos.y =
        podium_pan[0].y + (phase * (podium_pan[1].y - podium_pan[0].y));
    view->pos.z =
        podium_pan[0].z + (phase * (podium_pan[1].z - podium_pan[0].z));
    view->target.x = podium_pan[0].x - LOOK_SHIFT;
    view->target.y = podium_pan[0].y;
    view->target.z = podium_pan[0].z;
    view->type = CAMERA_POS_TARGET;
}
