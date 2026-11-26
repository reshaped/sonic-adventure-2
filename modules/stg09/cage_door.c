#include <ninja.h>
#include "collision/collision.h"

#include "stg09/cage_door/text.inc"

#ifdef NONMATCHING
/* this side keeps the set editor's captions as arrays of their own. GCC drops
 * the literals cage_door_draw names */
__attribute__((used)) static const char cage_door_editor_ene_id[] =
    CAGE_DOOR_EDITOR_ENE_ID;
__attribute__((used)) static const char cage_door_editor_angle_y[] =
    CAGE_DOOR_EDITOR_ANGLE_Y;
__attribute__((used)) static const char cage_door_editor_shut_speed[] =
    CAGE_DOOR_EDITOR_SHUT_SPEED;
#endif

#include "stg09/cage_door_impl.h"

#include "stg09/fixtures/cage_door_materials.h"
#include "stg09/fixtures/cage_door.h" /* IWYU pragma: keep -- cage_door_object's child */
#include "stg09/fixtures/cage_door_frame.h" /* IWYU pragma: keep -- the object cage_door_impl.h draws */

NJS_TEXLIST *cage_door_texlists[3] = {
#include "stg09/cage_door/cage_door_texlists.inc"
};

/* clang-format off */
static struct collision_shape cage_door_shapes[CAGE_DOOR_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -7.0F, 0.0F}, CAGE_DOOR_HALF_WIDTH, 5.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 19.5F, 0.0F}, CAGE_DOOR_HALF_WIDTH, 19.5F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {POST_X, 19.5F, 0.0F}, 5.0F, 24.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-POST_X, 19.5F, 0.0F}, 5.0F, 24.5F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
