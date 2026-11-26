#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "chaostgkinder/camera.h"
#include "player/player.h"
#include "system/task.h"

/* the house's camera heads for player 0's z plus TRAIL_Z, or for TRAIL_Z
 * while that z isn't positive. it moves EASE of the way there each step */
#define TRAIL_Z 40.0F
#define EASE 0.1F

struct kinder_camera_start kinder_camera_start;

void kinder_room_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    struct kinder_camera_start *start = &kinder_camera_start;
    struct camera_state *camera;

    if (start->pending & 1) {
        camera = &camera_state;
        camera->type = CAMERA_POS_ANG_DIST;
        camera->unk_04 = 0;
        camera->trigger_push = 0;
        camera->pos.x = start->pos.x;
        camera->pos.y = start->pos.y;
        camera->pos.z = start->pos.z;
        camera->ang.x = start->pitch;
        camera->ang.y = start->yaw;
        camera->ang.z = 0;
    }
    start->pending = 0;
}

void kinder_house_camera(struct camera_stack *stack, struct camera_slot *slot)
{
    struct kinder_camera_start *start = &kinder_camera_start;
    struct camera_state *camera = &camera_state;
    Float z;
    Float player_z;

    if (start->pending & 1) {
        camera->type = CAMERA_POS_ANG_DIST;
        camera->unk_04 = 0;
        camera->trigger_push = 0;
        camera->pos.x = start->pos.x;
        camera->pos.y = start->pos.y;
        camera->pos.z = start->pos.z;
        camera->ang.x = start->pitch;
        camera->ang.y = start->yaw;
        camera->ang.z = 0;
        start->pending = 0;
    } else {
        z = stack->place.pos.z;
        player_z = player_work[0]->pos.z;
        if (player_z > 0.0F) {
            camera->pos.z = z + ((player_z - z + TRAIL_Z) * EASE);
        } else {
            camera->pos.z = z + ((TRAIL_Z - z) * EASE);
        }
    }
}
