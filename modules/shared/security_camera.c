/* Prison Lane's security camera watches the player until an attack detaches its
 * head. */
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "shared/security_camera.h"
#include "shared/security_camera/text.inc"

/* the placement editor's label, which no code reads, for the camera's
 * heading. its size rounds the label up to whole words, so what follows
 * starts on a word boundary */
static const char edit_yaw[12] = "<- ANGLE Y";

#include "shared/security_camera_art.h"

NJS_TEXLIST *security_camera_texlists[2] = {
#include "shared/security_camera/security_camera_texlists.inc"
};
/* the most the head turns in a step, in Angle units */
static Sint32 security_camera_most_turn = 128;
#define CAMERA_RADIUS 8.5F
#define HEAD_RADIUS 4.0F
/* clang-format off */
static struct collision_shape security_camera_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, CAMERA_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape security_camera_head_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, HEAD_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

#include "shared/security_camera_task.h" /* IWYU pragma: keep -- the code it compiles here */
