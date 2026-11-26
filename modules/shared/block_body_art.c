/* the core block's body and its early and late flashes, which the block and
 * the lift both draw */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/core_block.h"

enum {
    BLOCK_TEX_DZ_CC_BLACKBOX256,
    BLOCK_TEX_DZ_CC_BLACKBOXLIGHT128,
    BLOCK_TEX_DZ_CC_BLACKBOXLIGHTIN128,
    BLOCK_TEX_DZ_CC_BLACKBOXLIGHTOUT128,
    BLOCK_TEX_DZ_CC_BLACKBOXSIDE256
};

static Sint16 body_polygons[97] = {
#include "shared/block_body_art/body_polygons.inc"
};

static Sint32 body_vertices[147] = {
#include "shared/block_body_art/body_vertices.inc"
};

NJS_CNK_MODEL core_block_body = {
#include "shared/block_body_art/core_block_body.inc"
};

static NJS_CNK_OBJECT body_object = {
#include "shared/block_body_art/body_object.inc"
};

static Sint16 flash_late_polygons[83] = {
#include "shared/block_body_art/flash_late_polygons.inc"
};

static Sint32 flash_late_vertices[123] = {
#include "shared/block_body_art/flash_late_vertices.inc"
};

NJS_CNK_MODEL core_block_flash_late = {
#include "shared/block_body_art/core_block_flash_late.inc"
};

static NJS_CNK_OBJECT flash_late_object = {
#include "shared/block_body_art/flash_late_object.inc"
};

static Sint16 flash_early_polygons[83] = {
#include "shared/block_body_art/flash_early_polygons.inc"
};

static Sint32 flash_early_vertices[123] = {
#include "shared/block_body_art/flash_early_vertices.inc"
};

NJS_CNK_MODEL core_block_flash_early = {
#include "shared/block_body_art/core_block_flash_early.inc"
};

static NJS_CNK_OBJECT flash_early_object = {
#include "shared/block_body_art/flash_early_object.inc"
};

static NJS_CNK_OBJECT block_root = {
#include "shared/block_body_art/block_root.inc"
};
