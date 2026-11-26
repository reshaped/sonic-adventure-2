/* Iron Gate registers its fallen camera head and gives it a distinct collision
 * shape. */
#include <ninja.h>
#include <sg_xpt.h>
#include "collision/collision.h"
#include "stg11/security_camera.h"
#include "stg11/security_camera/text.inc"

/* the placement editor's label, which no code reads, for the camera's
 * heading. its size rounds the label up to whole words, so what follows
 * starts on a word boundary */
static const char edit_yaw[12] = "<- ANGLE Y";

#include "shared/security_camera_art.h"

NJS_TEXLIST *iron_gate_security_camera_texlists[2] = {
#include "stg11/security_camera/iron_gate_security_camera_texlists.inc"
};
/* the most the head turns in a step, in Angle units */
static Sint32 security_camera_most_turn = 128;
#define CAMERA_RADIUS 8.5F
#define HEAD_RADIUS 4.0F
/* clang-format off */
static struct collision_shape security_camera_shapes[1] = {
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 0.0F, 0.0F}, CAMERA_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
};
static struct collision_shape security_camera_head_shapes[1] = {
    {10, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, SHAPE_UNK_800000, {0.0F, 0.0F, 0.0F}, HEAD_RADIUS, 0.0F, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */

/* NOLINTNEXTLINE(readability-identifier-naming) */
#define security_camera_init iron_gate_security_camera_init
#define SECURITY_CAMERA_BROKEN_END ((void)0)
#include "shared/security_camera_task.h" /* IWYU pragma: keep -- the code it compiles here */
