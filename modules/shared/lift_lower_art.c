/* the lower portal of BLOCK2, the vertical block of Cannon's Core, and the
 * portal's glow */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_block.h"

enum {
    BLOCK_TEX_MIU128_CG001,
    BLOCK_TEX_MIU128_CG022,
    BLOCK_TEX_MIU256_CC002,
    BLOCK_TEX_TX_CCPOISON001
};

static Sint16 lower_glow_sibling_polygons[25] = {
#include "shared/lift_lower_art/lower_glow_sibling_polygons.inc"
};

static Sint32 lower_glow_sibling_vertices[19] = {
#include "shared/lift_lower_art/lower_glow_sibling_vertices.inc"
};

static NJS_CNK_MODEL lower_glow_sibling_mesh = {
#include "shared/lift_lower_art/lower_glow_sibling_mesh.inc"
};

static NJS_CNK_OBJECT lower_glow_sibling_object = {
#include "shared/lift_lower_art/lower_glow_sibling_object.inc"
};

static Sint16 lower_glow_polygons[25] = {
#include "shared/lift_lower_art/lower_glow_polygons.inc"
};

static Sint32 lower_glow_vertices[19] = {
#include "shared/lift_lower_art/lower_glow_vertices.inc"
};

NJS_CNK_MODEL core_lift_lower_glow = {
#include "shared/lift_lower_art/core_lift_lower_glow.inc"
};

static NJS_CNK_OBJECT lower_glow_object = {
#include "shared/lift_lower_art/lower_glow_object.inc"
};

static Sint16 lower_portal_polygons[319] = {
#include "shared/lift_lower_art/lower_portal_polygons.inc"
};

static Sint32 lower_portal_vertices[179] = {
#include "shared/lift_lower_art/lower_portal_vertices.inc"
};

NJS_CNK_MODEL core_lift_lower_portal = {
#include "shared/lift_lower_art/core_lift_lower_portal.inc"
};

static NJS_CNK_OBJECT lower_portal_object = {
#include "shared/lift_lower_art/lower_portal_object.inc"
};

static NJS_CNK_OBJECT lower_portal_root = {
#include "shared/lift_lower_art/lower_portal_root.inc"
};
