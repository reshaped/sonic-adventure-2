/* the cameras' stacks: clearing them, and each camera's step of the mode on
 * top of its stack */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/gakugaku.h"
#include "camera/stack.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/pad.h"

/* the angle of view camera_run_mode has a camera head for after a step */
#define VIEW_ANGLE ((Angle)(70 * ANGLE_PER_DEGREE))

/* a trigger pushes no mode until camera_steps is above TRIGGER_DELAY */
#define TRIGGER_DELAY 300

static void camera_step_trigger_mode(int camera)
{
    if (camera_stacks[camera].trigger_pushed) {
        if (camera_stacks[camera].standing == 0) {
            camera_remove_mode(camera, camera_top(camera));
            camera_set_adjust(camera, camera_top(camera),
                              CAMERA_ADJUST_RELATIVE1);
            camera_stacks[camera].trigger_pushed = FALSE;
        }
    } else if (camera_state.trigger_push != 0 &&
               pad_player_is_enabled(camera) != 0 &&
               camera_steps > TRIGGER_DELAY) {
        /* @hack spelling: the stack's and the triggers' tests in an if of
         * their own, where && with the tests above would do */
        /* @hack spelling: each trigger's test as its value less 128 against
         * -128, where a test against 0 would do */
        if (camera_stacks[camera].standing == 1 &&
            (player_pad_records[camera].l - 128 != -128 ||
             player_pad_records[camera].r - 128 != -128)) {
            camera_push_mode(camera, CAMERA_MODE_KNUKLES);
            camera_set_adjust(camera, camera_top(camera),
                              CAMERA_ADJUST_RELATIVE1);
            camera_stacks[camera].trigger_pushed = TRUE;
        }
    }
}

void camera_clear_stacks(void)
{
    Uint8 *byte = (Uint8 *)camera_stacks;
    int left;

    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(camera_stacks); left != 0; left--) {
        *byte++ = 0;
    }
}

void camera_run_mode(int camera)
{
    struct camera_slot *mode;

    switch (camera_mode_at(camera, camera_top(camera))) {
    case CAMERA_MODE_KNUKLES:
    case CAMERA_MODE_KLAMATH:
    case CAMERA_MODE_POINT:
    case CAMERA_MODE_CARMEL:
        if (stage_current_id() < STAGE_STG_END &&
            !(player_pad_records[camera_current_index].on &
              (PDD_DGT_TR | PDD_DGT_TL)) &&
            camera_settle_step(camera) != 0) {
            camera_push_mode(camera, CAMERA_MODE_GAKUGAKU);
            camera_set_adjust(camera, camera_top(camera), CAMERA_ADJUST_NONE);
        }
        break;
    }
    mode = &camera_stacks[camera].modes[camera_top(camera)];
    if (mode->step != NULL) {
        mode->step(&camera_stacks[camera], mode);
        mode->steps++;
        if (camera_mode_at(camera, camera_top(camera)) != CAMERA_MODE_ASHLAND) {
            camera_set_view_angle(camera, VIEW_ANGLE,
                                  CAMERA_VIEW_TURN_USE_DEFAULT);
        }
    }
    camera_step_trigger_mode(camera_current_index);
}
