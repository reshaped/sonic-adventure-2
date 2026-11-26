#include <ninja.h>
#include <sg_xpt.h>

#include "shared/ocean_fence.h"

static const char material_saku1[12] = {
#include "shared/ocean_fence_art/material_saku1.inc"
};

static NJS_TEXNAME ocean_fence_bright_textures[1] = {
#include "shared/ocean_fence_art/ocean_fence_bright_textures.inc"
};

NJS_TEXLIST ocean_fence_bright_texlist = {
#include "shared/ocean_fence_art/ocean_fence_bright_texlist.inc"
};

static Sint16 ocean_fence_bright_polygons[293] = {
#include "shared/ocean_fence_art/ocean_fence_bright_polygons.inc"
};

static Sint32 ocean_fence_bright_vertices[319] = {
#include "shared/ocean_fence_art/ocean_fence_bright_vertices.inc"
};

NJS_CNK_MODEL ocean_fence_bright_model = {
#include "shared/ocean_fence_art/ocean_fence_bright_model.inc"
};

static NJS_CNK_OBJECT ocean_fence_bright_object = {
#include "shared/ocean_fence_art/ocean_fence_bright_object.inc"
};

static NJS_TEXNAME ocean_fence_dark_textures[1] = {
#include "shared/ocean_fence_art/ocean_fence_dark_textures.inc"
};

NJS_TEXLIST ocean_fence_dark_texlist = {
#include "shared/ocean_fence_art/ocean_fence_dark_texlist.inc"
};

static Sint16 ocean_fence_dark_polygons[293] = {
#include "shared/ocean_fence_art/ocean_fence_dark_polygons.inc"
};

static Sint32 ocean_fence_dark_vertices[331] = {
#include "shared/ocean_fence_art/ocean_fence_dark_vertices.inc"
};

NJS_CNK_MODEL ocean_fence_dark_model = {
#include "shared/ocean_fence_art/ocean_fence_dark_model.inc"
};

static NJS_CNK_OBJECT ocean_fence_dark_object = {
#include "shared/ocean_fence_art/ocean_fence_dark_object.inc"
};
