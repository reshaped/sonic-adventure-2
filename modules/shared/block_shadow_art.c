/* the core block's shadow, which core_block.c draws at the block's scale */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_block.h"

enum { BLOCK_TEX_DZ_CC_BLACKBOX_SHADOW32 };

static Sint16 shadow_polygons[25] = {
#include "shared/block_shadow_art/shadow_polygons.inc"
};

static Sint32 shadow_vertices[19] = {
#include "shared/block_shadow_art/shadow_vertices.inc"
};

NJS_CNK_MODEL core_block_shadow = {
#include "shared/block_shadow_art/core_block_shadow.inc"
};

static NJS_CNK_OBJECT shadow_object = {
#include "shared/block_shadow_art/shadow_object.inc"
};
