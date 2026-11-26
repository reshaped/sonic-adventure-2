#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/garden.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/menu.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"

enum camera_phase {
    OPENING_START,
    OPENING_RISE,
    OPENING_ADVANCE,
    OPENING_LOWER,
    OPENING_SETTLE,
    OPENING_DONE
};

#define START_Y 8
#define START_Z 124
#define START_PITCH ((Angle)(4 * ANGLE_PER_DEGREE))
#define RISE_SPEED 0.1F
#define RISE_TOP 24.0F
#define ADVANCE_ACCEL 0.033F
#define TOP_SPEED (-1.0F)
#define LOWER_Z (-15.0F)
/* the point the camera settles at before the menu, and the pitch it turns
 * toward as it lowers */
#define REST_Y 11.5F
#define REST_Z (-33.0F)
#define LOWER_PITCH ((Angle)(3 * ANGLE_PER_DEGREE))
/* the camera's z falls Z_DRIFT a frame past its ease toward the rest
 * point, and its y Y_DRIFT. z snaps to the point from within LOWER_SNAP as
 * the camera lowers. z and y snap from within SETTLE_SNAP as it settles */
#define Z_DRIFT 0.01F
#define Y_DRIFT 0.02F
#define LOWER_SNAP 0.1F
#define SETTLE_SNAP 0.01F
#define DROP_ACCEL 0.01F
#define DROP_SPEED (-0.4F)
/* the pitch's turn a frame in each phase */
#define RISE_TURN 4
#define ADVANCE_TURN 2
#define LOWER_TURN 10
#define SETTLE_TURN 14
/* A skips the opening only once more than SKIP_DELAY frames have run. a
 * skip puts the camera at the rest point, pitched SKIP_PITCH */
#define SKIP_DELAY 60
#define SKIP_PITCH (-14)
#define CAMERA_DIST 5

/* the share of its distance from the rest point the camera closes a frame,
 * by axis. the lowering sets y's and z's from the camera's speed then */
static NJS_VECTOR ease = {0.1F, 0.1F, 0.1F};
static int screen = 0;
static int phase;
static NJS_VECTOR speed;
static int frames;

static void opening_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    Float x;
    Float y;
    Float z;
    Angle pitch;
    Angle yaw;
    Angle roll;
    int settled;

    pitch = stack->place.ang.x;
    yaw = stack->place.ang.y;
    roll = stack->place.ang.z;
    x = stack->place.pos.x;
    y = stack->place.pos.y;
    z = stack->place.pos.z;
    camera_state.unk_04 = 0;
    camera_state.trigger_push = 0;
    switch (phase) {
    case OPENING_START:
        frames = 0;
        x = 0;
        y = START_Y;
        z = START_Z;
        pitch = START_PITCH;
        yaw = 0;
        roll = 0;
        phase = OPENING_RISE;
        speed.x = 0;
        speed.y = 0;
        speed.z = 0;
        break;
    case OPENING_RISE:
        y += RISE_SPEED;
        if (y > RISE_TOP) {
            phase = OPENING_ADVANCE;
        }
        pitch = angle_approach(pitch, 0, RISE_TURN);
        break;
    case OPENING_ADVANCE:
        speed.z -= ADVANCE_ACCEL;
        if (speed.z < TOP_SPEED) {
            speed.z = TOP_SPEED;
        }
        if (z < LOWER_Z) {
            ease.z = -speed.z / (z - REST_Z);
            phase = OPENING_LOWER;
        } else {
            z += speed.z;
        }
        pitch = angle_approach(pitch, 0, ADVANCE_TURN);
        break;
    case OPENING_LOWER:
        speed.z = (REST_Z - z) * ease.z;
        z += speed.z;
        z -= Z_DRIFT;
        if (z < REST_Z + LOWER_SNAP) {
            z = REST_Z;
        }
        speed.y -= DROP_ACCEL;
        if (speed.y < DROP_SPEED) {
            speed.y = DROP_SPEED;
            ease.y = -speed.y / (y - REST_Y);
            phase = OPENING_SETTLE;
        }
        y += speed.y;
        y -= Y_DRIFT;
        if (y < REST_Y) {
            y = REST_Y;
        }
        pitch = angle_approach(pitch, LOWER_PITCH, LOWER_TURN);
        break;
    case OPENING_SETTLE:
        settled = 0;
        speed.z = (REST_Z - z) * ease.z;
        z += speed.z;
        z -= Z_DRIFT;
        if (z < REST_Z + SETTLE_SNAP) {
            z = REST_Z;
            settled++;
        }
        speed.y = (REST_Y - y) * ease.y;
        y += speed.y;
        y -= Y_DRIFT;
        if (y < REST_Y + SETTLE_SNAP) {
            y = REST_Y;
            settled++;
        }
        if (settled >= 2) {
            entrance_menu_create();
            phase = OPENING_DONE;
        }
        /* angle_approach turns away from its target by a negative step.
         * after the lowering or a skip, pitch comes here below LOWER_PITCH,
         * so it falls SETTLE_TURN a frame */
        pitch = angle_approach(pitch, LOWER_PITCH, -SETTLE_TURN);
        break;
    case OPENING_DONE:
        break;
    }
    if (phase < OPENING_SETTLE) {
        if ((frames++ > SKIP_DELAY && (player_pads[0]->press & PDD_DGT_TA)) ||
            game_state.chao_race_menu != CHAO_RACE_MENU_NONE ||
            chao_previous_area() == CHAO_AREA_RACE) {
            y = REST_Y;
            z = REST_Z;
            speed.y = 0.0F;
            speed.z = 0.0F;
            pitch = SKIP_PITCH;
            phase = OPENING_SETTLE;
        }
    }
    camera_state.pos.x = x;
    camera_state.pos.y = y;
    camera_state.pos.z = z;
    camera_state.ang.x = pitch;
    camera_state.ang.y = yaw;
    camera_state.ang.z = roll;
    camera_state.dist = CAMERA_DIST;
    camera_state.type = CAMERA_POS_ANG_DIST;
}

void entrance_camera_start(void)
{
    phase = OPENING_START;
    camera_push_user_mode(screen, opening_camera);
}

void entrance_camera_stop(void)
{
    camera_remove_mode(screen, camera_top(screen));
}
