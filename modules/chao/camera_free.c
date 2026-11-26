/* the garden's debug cameras: one the third controller flies, and one that
 * holds still */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/debug.h"
#include "chao/garden_camera.h"
#include "system/pad.h"

/* a stick's tilt as the camera reads it, per unit of the pad's */
#define STICK_SCALE 256
/* the tilt within which the stick counts as centered */
#define DEAD_ZONE 5000
/* the tilt from which the stick steps the camera a whole unit a frame */
#define STEP_TILT 20000
/* each frame the triggers turn the camera by TURN_STEP. the d-pad slides it by
 * SLIDE_STEP, or moves it up or down by RISE_STEP. CAMERA_DISTANCE is the dist
 * both cameras give camera_state */
#define TURN_STEP 0x200
#define SLIDE_STEP 0.3F
#define RISE_STEP 0.2F
#define CAMERA_DISTANCE 5.0F

void garden_free_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    Angle yaw = stack->place.ang.y;
    struct camera_state *state = &camera_state;
    Float x = stack->place.pos.x;
    Float z = stack->place.pos.z;
    Sint16 stick_x;
    Sint16 stick_y;
    Uint16 trigger; /* NOLINT(clang-diagnostic-unused-but-set-variable) */
    Float speed;
    int turn;
    Angle heading;
    state->unk_04 = 0;
    state->trigger_push = 0;
    stick_x = player_pads[CHAO_DEBUG_PAD]->x1 * STICK_SCALE;
    stick_y = player_pads[CHAO_DEBUG_PAD]->y1 * STICK_SCALE;
    if (stick_x > -DEAD_ZONE && stick_x < DEAD_ZONE) {
        stick_x = 0;
    }
    if (stick_y > -DEAD_ZONE && stick_y < DEAD_ZONE) {
        stick_y = 0;
    }
    /* @hack spelling: yaw copied to heading for one use, where yaw would do */
    heading = yaw;
    /* @hack redundant: the read of the pad's r, which nothing uses */
    trigger = player_pads[CHAO_DEBUG_PAD]->r;
    if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TL) {
        state->ang.y -= TURN_STEP;
    }
    if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TR) {
        state->ang.y += TURN_STEP;
    }
    /* @hack spelling: the d-pad's slide held in speed, where the constant
     * would do */
    speed = SLIDE_STEP;
    if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_KL) {
        x -= njCos(heading) * speed;
        z += njSin(yaw) * speed;
    } else if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_KR) {
        x += njCos(yaw) * speed;
        z -= njSin(yaw) * speed;
    }
    if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_KU) {
        state->pos.y = stack->place.pos.y + RISE_STEP;
    } else if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_KD) {
        state->pos.y = stack->place.pos.y - RISE_STEP;
    }
    if (player_pads[CHAO_DEBUG_PAD]->on & PDD_DGT_TX) {
        state->ang.y = stack->place.ang.y + (-stick_x / STICK_SCALE);
        state->ang.x = stack->place.ang.x + (stick_y / STICK_SCALE);
    } else {
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        x += (Float)(stick_y / STEP_TILT) * njSin(yaw);
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        z += (Float)(stick_y / STEP_TILT) * njCos(yaw);
        /* @hack spelling: -stick_x held in turn, where the expression would
         * do */
        turn = -stick_x;
        state->ang.y = stack->place.ang.y + (turn / (STICK_SCALE / 2));
    }
    state->pos.x = x;
    state->pos.z = z;
    state->ang.z = 0;
    state->dist = CAMERA_DISTANCE;
    state->type = CAMERA_POS_ANG_DIST;
}
void garden_fixed_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_state *state = &camera_state;
    state->unk_04 = 0;
    state->trigger_push = 0;
    state->pos.x = stack->place.pos.x;
    state->pos.y = stack->place.pos.y;
    state->pos.z = stack->place.pos.z;
    state->ang.x = stack->place.ang.x;
    state->ang.y = stack->place.ang.y;
    state->ang.z = 0;
    state->dist = CAMERA_DISTANCE;
    state->type = CAMERA_POS_ANG_DIST;
}
