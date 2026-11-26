/* the texture lists, shaft wall poses and chunk models of STG37.PRS and
 * STG38.PRS, from the disc. its D follows shaft_inner_wall_models.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"

struct shaft_middle_wall_poses_pose {
    Sint16 polygons[467];
    Sint32 vertices[259];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

struct shaft_outer_wall_poses_pose {
    Sint16 polygons[467];
    Sint32 vertices[259];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME shaft_middle_wall_slots[2] = {
#include "shared/shaft_middle_wall_models/shaft_middle_wall_slots.inc"
};
NJS_TEXLIST middle_wall_textures = {
#include "shared/shaft_middle_wall_models/middle_wall_textures.inc"
};
struct shaft_side_wall_pose shaft_middle_wall_poses[10] = {
#include "shared/shaft_middle_wall_models/shaft_middle_wall_poses.inc"
};
static NJS_CNK_OBJECT shaft_middle_wall_root = {
#include "shared/shaft_middle_wall_models/shaft_middle_wall_root.inc"
};
static NJS_TEXNAME shaft_outer_wall_slots[2] = {
#include "shared/shaft_middle_wall_models/shaft_outer_wall_slots.inc"
};
NJS_TEXLIST outer_wall_textures = {
#include "shared/shaft_middle_wall_models/outer_wall_textures.inc"
};
struct shaft_side_wall_pose shaft_outer_wall_poses[10] = {
#include "shared/shaft_middle_wall_models/shaft_outer_wall_poses.inc"
};
static NJS_CNK_OBJECT shaft_outer_wall_root = {
#include "shared/shaft_middle_wall_models/shaft_outer_wall_root.inc"
};
