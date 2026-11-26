/* the texture lists, shaft wall poses and chunk models of STG37.PRS and
 * STG38.PRS, from the disc. its D precedes shaft_middle_wall_models.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_scenery_art.h"
#include "shared/shaft_wall_art.h"

struct shaft_inner_wall_poses_pose {
    Sint16 polygons[837];
    Sint32 vertices[547];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

static NJS_TEXNAME slots[1] = {
#include "shared/shaft_inner_wall_models/slots.inc"
};
NJS_TEXLIST inner_wall_textures = {
#include "shared/shaft_inner_wall_models/inner_wall_textures.inc"
};
struct shaft_inner_wall_pose shaft_inner_wall_poses[16] = {
#include "shared/shaft_inner_wall_models/shaft_inner_wall_poses.inc"
};
static NJS_CNK_OBJECT root = {
#include "shared/shaft_inner_wall_models/root.inc"
};
