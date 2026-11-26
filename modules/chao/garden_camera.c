/* the garden's camera. a body chases an eye point behind the player, and
 * walls and floors stop it. the camera views from the body toward a target.
 * the target follows the player, what the player holds, or the transfer
 * machine in use */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/garden_camera.h"
#include "chao/garden_scene.h"
#include "chao/ground.h"
#include "chao/machine.h"
#include "collision/collision.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"

#define CAMERA_SHAPES 2
/* clang-format off */
static const struct collision_shape camera_shapes[CAMERA_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_ENEMIES | PUSH_OBJECTS | PUSH_PICKUPS,
     SHAPE_GUARD_LEVEL(3), 0, {0.0F, 0.0F, 0.0F}, 3.0F, 1.5F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_PICKUPS |
         SHAPE_IGNORE_UNK_9,
     {0.0F, 0.0F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* for its first seven steps, the body starts START_DISTANCE from the
 * player, by the player's heading. its eye starts with it */
#define START_DISTANCE 10.0F
/* the turn of the eye about the player each step a trigger is held */
#define ORBIT_TURN 0x200
/* the shares of its gap the target closes each step. it closes SLOW_FOLLOW
 * toward what the player holds or toward the machine. toward the player it
 * closes ACROSS_FOLLOW across and RISE_FOLLOW up */
#define SLOW_FOLLOW 0.04F
#define ACROSS_FOLLOW 0.4F
#define RISE_FOLLOW 0.2F
/* while the transfer machine is in use, the target lies MACHINE_TARGET_HEIGHT
 * above it. the eye lies MACHINE_VIEW_DISTANCE from it, MACHINE_VIEW_HEIGHT
 * up, at MACHINE_VIEW_TURN past its heading */
#define MACHINE_TARGET_HEIGHT 10.0F
#define MACHINE_VIEW_DISTANCE 37.0F
#define MACHINE_VIEW_HEIGHT 12.0F
#define MACHINE_VIEW_TURN 0x5800
/* follow, the share of its gap to the eye the body closes each step. a wall
 * sets it to WALL_FOLLOW, and the machine to MACHINE_FOLLOW. otherwise it
 * grows FOLLOW_GAIN a step, up to follow_max */
#define WALL_FOLLOW 0.02F
#define MACHINE_FOLLOW 0.05F
#define FOLLOW_GAIN 0.01F
/* a body on a floor rises at FLOOR_RISE of rise_rate toward an eye less
 * than CLIMB_GAP above it */
#define FLOOR_RISE 0.1F
#define CLIMB_GAP 30.0F
/* the body stills where its step is shorter than STILL_STEP and no wall
 * pushes it. it stirs again where its step is longer than STIR_STEP, or a
 * wall pushes it. a still body skips garden_meet_floor */
#define STILL_STEP 0.005F
#define STIR_STEP 0.01F

static Sint32 camera_index = 0;
static struct task *camera_body = NULL;
static Float view_distance = 25.0F;
static Float follow_max = 0.13F;
static Float rise_rate = 0.1F;
static Float maximum_step = 3.0F;
static Float surface_clearance = 2.5F;

void garden_camera_reset(void) /* NOLINT(misc-use-internal-linkage) */
{
    garden_camera_reset_flag = 0;
}
void fun_0c53bc28(void) /* NOLINT(misc-use-internal-linkage) */
{
}
void garden_camera_push_free(void) /* NOLINT(misc-use-internal-linkage) */
{
    camera_push_user_mode(camera_index, garden_free_camera);
}
static void camera_body_step(struct task *camera)
{
    struct garden_camera *work = (struct garden_camera *)camera->work;
    struct task_work *player = player_work[0];
    NJS_POINT3 *eye = &work->eye;
    NJS_POINT3 *target = &work->target;
    NJS_POINT3 *position = &work->base.pos;
    NJS_POINT3 *player_pos = &player->pos;
    Uint16 mode = player->mode;
    NJS_VECTOR back;
    NJS_VECTOR step;
    NJS_POINT3 *focus;
    NJS_POINT3 *machine_pos;
    struct task *carried;
    struct task_work *machine;
    struct mover *mover;
    Angle angle;
    Float distance;
    /* for its first seven steps, modes 0 to 6, the body starts again
     * START_DISTANCE behind the player */
    switch (work->base.mode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        position->x =
            player_pos->x -
            (njSin(QUARTER_TURN - player_work[0]->ang.y) * START_DISTANCE);
        position->y = player_pos->y + work->eye_height;
        position->z =
            player_pos->z -
            (njCos(QUARTER_TURN - player_work[0]->ang.y) * START_DISTANCE);
        *eye = *position;
        work->base.mode++;
        break;
    }
    back.x = eye->x - player_pos->x;
    back.y = 0.0F;
    back.z = eye->z - player_pos->z;
    njUnitVector(&back);
    back.x *= view_distance;
    back.z *= view_distance;
    GARDEN_ORBIT_TRIGGER(PDD_DGT_TL, -ORBIT_TURN, back);
    GARDEN_ORBIT_TRIGGER(PDD_DGT_TR, ORBIT_TURN, back);
    if (mode == PLAYER_MODE_UNK_17 || mode == PLAYER_MODE_UNK_22) {
        carried = player_records[0]->hold_offer;
        if (carried) {
            focus = &carried->work->pos;
        } else {
            focus = player_pos;
        }
        target->x += (focus->x - target->x) * SLOW_FOLLOW;
        target->y += (focus->y + work->carry_height - target->y) * SLOW_FOLLOW;
        target->z += (focus->z - target->z) * SLOW_FOLLOW;
    } else if (garden_machine_in_use && garden_transfer_machine) {
        machine_pos = &garden_transfer_machine->work->pos;
        focus = &step;
        focus->x = machine_pos->x;
        focus->y = machine_pos->y + MACHINE_TARGET_HEIGHT;
        focus->z = machine_pos->z;
        target->x += (focus->x - target->x) * SLOW_FOLLOW;
        target->y += (focus->y - target->y) * SLOW_FOLLOW;
        target->z += (focus->z - target->z) * SLOW_FOLLOW;
    } else {
        target->x += (player_pos->x - target->x) * ACROSS_FOLLOW;
        /* @hack spelling: the share first, with a unary plus, where y and z
         * could take x's order */
        target->y +=
            +RISE_FOLLOW * (player_pos->y + work->target_height - target->y);
        target->z += +ACROSS_FOLLOW * (player_pos->z - target->z);
    }
    if (garden_machine_in_use && garden_transfer_machine) {
        machine = garden_transfer_machine->work;
        focus = &machine->pos;
        angle = machine->ang.y + MACHINE_VIEW_TURN;
        eye->x = focus->x + (njSin(angle) * MACHINE_VIEW_DISTANCE);
        eye->y = focus->y + MACHINE_VIEW_HEIGHT;
        eye->z = focus->z + (njCos(angle) * MACHINE_VIEW_DISTANCE);
    } else {
        eye->x = player_pos->x + back.x;
        eye->y = player_pos->y + work->eye_height;
        eye->z = player_pos->z + back.z;
    }
    mover = camera->motion.p;
    if (eye->y < mover->flat_y + surface_clearance) {
        eye->y = mover->flat_y + surface_clearance;
    }
    /* @hack spelling: mover read again from camera->motion inside the test,
     * where the one read above would do */
    if ((mover = camera->motion.p)->flags & MOVER_AT_WALL) {
        work->follow = WALL_FOLLOW;
    } else if (garden_machine_in_use) {
        work->follow = MACHINE_FOLLOW;
    } else {
        work->follow += FOLLOW_GAIN;
        if (work->follow > follow_max) {
            work->follow = follow_max;
        }
    }
    step.x = work->follow * (eye->x - position->x);
    if ((mover->flags & MOVER_ON_FLOOR) && eye->y - position->y < CLIMB_GAP) {
        step.y = rise_rate * (eye->y - position->y) * FLOOR_RISE;
    } else {
        step.y = rise_rate * (eye->y - position->y);
    }
    step.z = work->follow * (eye->z - position->z);
    distance = njScalor(&step);
    if (distance > 0.0F) {
        if (distance > maximum_step) {
            distance = maximum_step;
            njUnitVector(&step);
            step.x *= distance;
            step.y *= distance;
            step.z *= distance;
        }
    }
    njAddVector(position, &step);
    if (work->still) {
        if (distance > STIR_STEP || (mover->flags & MOVER_AT_WALL)) {
            work->still = FALSE;
        }
    } else {
        if (!(mover->flags & MOVER_AT_WALL)) {
            if (distance < STILL_STEP) {
                work->still = TRUE;
            }
        }
        garden_meet_floor(camera);
    }
    collision_register(camera);
}

static void camera_body_destroy(struct task *camera)
{
    camera_body = NULL;
}
struct task *garden_camera_create(void)
{
    struct task *camera;
    struct garden_camera *work;
    struct mover *mover;
    if (camera_body) {
        return NULL;
    }
    camera = task_create(0, 2, camera_body_step, "AL_NormalCameraExecutor");
#ifdef NONMATCHING
    /* this side returns null where task_create fails. the matching side writes
     * through the null */
    if (!camera) {
        return NULL;
    }
#endif
    camera->destructor = camera_body_destroy;
    work = syCalloc(1, sizeof(struct garden_camera));
    camera->work = (struct task_work *)work;
#ifdef NONMATCHING
    /* this side destroys the camera and returns null where the heap has no work
     * or collision_init fails */
    if (!work) {
        task_destroy(camera);
        return NULL;
    }
    if (!collision_init(camera, camera_shapes, CAMERA_SHAPES,
                        COLLISION_OBJECT_MUTUAL)) {
        task_destroy(camera);
        return NULL;
    }
#else
    collision_init(camera, camera_shapes, CAMERA_SHAPES,
                   COLLISION_OBJECT_MUTUAL);
#endif
    if (player_character(0) != PLAYER_EGGMAN) {
        work->target_height = 4.0F;
        work->carry_height = 22.0F;
        work->eye_height = 5.5F;
    } else {
        work->target_height = 10.0F;
        work->carry_height = 24.0F;
        work->eye_height = 11.5F;
    }
    work->follow = follow_max;
    mover = mover_alloc(camera);
#ifdef NONMATCHING
    /* this side destroys the camera and returns null where mover_alloc fails.
     * the matching side writes through the null */
    if (!mover) {
        task_destroy(camera);
        return NULL;
    }
#endif
    mover->radius = 5.0F;
    mover->probe_height = 1.0F;
    mover->bottom = -3.0F;
    mover_set_gravity(camera, 0.0F);
    work->type = CAMERA_POS_TARGET;
    camera_body = camera;
    return camera;
}
static void manager_step(struct task *manager)
{
    struct task_work *work = manager->work;
    work->timer++;
}
static void manager_draw(struct task *manager)
{
}
static void manager_destroy(struct task *manager)
{
    camera_remove_mode(camera_index, camera_top(camera_index));
}
static void garden_camera_mode(struct camera_stack *stack,
                               struct camera_slot *slot)
{
    struct garden_camera *work;
    struct camera_state *state;
    struct task *camera = camera_body;
    /* @hack spelling: clear, a local 0, where 0 would do */
    Sint32 clear = 0;
    if (camera) {
        work = (struct garden_camera *)camera->work;
    } else {
        work = &garden_camera_fallback;
    }
    state = &camera_state;
    state->unk_04 = clear;
    state->trigger_push = clear;
    state->pos.x = work->base.pos.x;
    state->pos.y = work->base.pos.y;
    state->pos.z = work->base.pos.z;
    state->target.x = work->target.x;
    state->target.y = work->target.y;
    state->target.z = work->target.z;
    state->ang.x = work->base.ang.x;
    state->ang.y = work->base.ang.y;
    state->ang.z = work->base.ang.z;
    state->dist = work->distance;
    state->type = work->type;
    state->unk_04 = clear;
    state->trigger_push = clear;
}
void garden_camera_start(void)
{
    struct task *manager;
    manager =
        task_create(TASK_WORK, 2, manager_step, "ALCAM_CameraManagerExecutor");
#ifdef NONMATCHING
    /* this side makes no manager where task_create fails. the matching side
     * writes through the null */
    if (!manager) {
        return;
    }
#endif
    manager->draw = manager_draw;
    manager->destructor = manager_destroy;
    garden_camera_reset_flag = 0;
    camera_push_user_mode(camera_index, garden_camera_mode);
}
