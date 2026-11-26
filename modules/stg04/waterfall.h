/* The waterfall, its stationary bank and the three scrolling strips. */
#ifndef STG04_WATERFALL_H
#define STG04_WATERFALL_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static NJS_TEXNAME taki_textures[2] = {
#include "stg04/waterfall/taki_textures.inc"
};

static NJS_TEXLIST taki_texlist = {
#include "stg04/waterfall/taki_texlist.inc"
};

static Sint16 cascade_bank_polygons[97] = {
#include "stg04/waterfall/cascade_bank_polygons.inc"
};

static Sint32 cascade_bank_vertices[63] = {
#include "stg04/waterfall/cascade_bank_vertices.inc"
};

static NJS_CNK_MODEL cascade_bank_mesh = {
#include "stg04/waterfall/cascade_bank_mesh.inc"
};

static NJS_CNK_OBJECT cascade_bank = {
#include "stg04/waterfall/cascade_bank.inc"
};

static Sint16 cascade_polygons[213] = {
#include "stg04/waterfall/cascade_polygons.inc"
};

static Sint32 cascade_vertices[179] = {
#include "stg04/waterfall/cascade_vertices.inc"
};

static NJS_CNK_MODEL cascade_mesh = {
#include "stg04/waterfall/cascade_mesh.inc"
};

static NJS_CNK_OBJECT taki_object = {
#include "stg04/waterfall/taki_object.inc"
};

static Sint16 cascade_uvs[140] = {
#include "stg04/waterfall/cascade_uvs.inc"
};

static struct texture_anim taki_anim = {
#include "stg04/waterfall/taki_anim.inc"
};

#endif
