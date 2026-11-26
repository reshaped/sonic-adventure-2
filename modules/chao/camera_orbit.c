/* a garden camera that circles player 0 at a fixed distance and height. its
 * target eases toward the player. in two of the player's modes, it eases
 * instead toward the object player_records[0]->hold_offer names */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/garden_camera.h"
#include "player/player.h"
#include "system/task.h"

/* how far across the ground the camera keeps from the player, and how high
 * above the player */
#define ORBIT_RADIUS 28.0F
#define ORBIT_HEIGHT 5.5F
/* the angle the triggers turn the camera about the player each frame */
#define ORBIT_TURN 0x200
/* each frame the target eases across the ground by EASE_ACROSS of its way to
 * the player. it eases up by EASE_UP, toward LOOK_HEIGHT above the player */
#define EASE_ACROSS 0.4F
#define EASE_UP 0.2F
#define LOOK_HEIGHT 4.0F
/* the same toward an object the player may hold */
#define HOLD_EASE 0.04F
#define HOLD_LOOK_HEIGHT 22.0F

void garden_orbit_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    NJS_POINT3 *position = &stack->place.pos;
    NJS_POINT3 *target = &stack->place.target;
    struct task_work *player = player_work[0];
    NJS_POINT3 *player_pos = &player->pos;
    Uint16 mode = player->mode;
    NJS_VECTOR back;
    struct camera_state *state;
    NJS_POINT3 *focus;
    struct task *carried;
    back.x = position->x - player_pos->x;
    back.y = 0.0F;
    back.z = position->z - player_pos->z;
    njUnitVector(&back);
    back.x *= ORBIT_RADIUS;
    back.z *= ORBIT_RADIUS;
    GARDEN_ORBIT_TRIGGER(PDD_DGT_TL, -ORBIT_TURN, back);
    GARDEN_ORBIT_TRIGGER(PDD_DGT_TR, ORBIT_TURN, back);
    state = &camera_state;
    if (mode == PLAYER_MODE_UNK_17 || mode == PLAYER_MODE_UNK_22) {
        carried = player_records[0]->hold_offer;
        if (carried) {
            focus = &carried->work->pos;
        } else {
            focus = player_pos;
        }
        state->target.x = target->x + ((focus->x - target->x) * HOLD_EASE);
        state->target.y =
            target->y + ((focus->y - target->y + HOLD_LOOK_HEIGHT) * HOLD_EASE);
        state->target.z = target->z + ((focus->z - target->z) * HOLD_EASE);
    } else {
        state->target.x =
            target->x + ((player_pos->x - target->x) * EASE_ACROSS);
        state->target.y =
            target->y + ((player_pos->y - target->y + LOOK_HEIGHT) * EASE_UP);
        state->target.z =
            target->z + ((player_pos->z - target->z) * EASE_ACROSS);
    }
    state->pos.x = player_pos->x + back.x;
    state->pos.y = player_pos->y + ORBIT_HEIGHT;
    state->pos.z = player_pos->z + back.z;
    state->ang.z = 0;
    state->type = CAMERA_POS_TARGET;
    state->unk_04 = 0;
    state->trigger_push = 0;
}
