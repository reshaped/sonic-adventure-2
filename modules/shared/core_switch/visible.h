#ifndef SHARED_CORE_SWITCH_VISIBLE_H
#define SHARED_CORE_SWITCH_VISIBLE_H

#include <ninja.h>
#include <sg_xpt.h>

enum switch_texture_slot {
    SWITCH_TEXTURE_BOX,
    SWITCH_TEXTURE_PLATE,
    SWITCH_TEXTURE_BALL,
    SWITCH_TEXTURE_UN108,
    SWITCH_TEXTURE_UN404
};

static Sint16 plate_polygons[451] = {
#include "shared/core_switch/visible/plate_polygons.inc"
};

static Sint32 plate_vertices[231] = {
#include "shared/core_switch/visible/plate_vertices.inc"
};

static NJS_CNK_MODEL plate_model = {
#include "shared/core_switch/visible/plate_model.inc"
};

static NJS_CNK_OBJECT plate_object = {
#include "shared/core_switch/visible/plate_object.inc"
};

static Sint16 base_polygons[349] = {
#include "shared/core_switch/visible/base_polygons.inc"
};

static Sint32 base_vertices[195] = {
#include "shared/core_switch/visible/base_vertices.inc"
};

static NJS_CNK_MODEL base_model = {
#include "shared/core_switch/visible/base_model.inc"
};

static NJS_CNK_OBJECT base_object = {
#include "shared/core_switch/visible/base_object.inc"
};

static NJS_CNK_OBJECT switch_object = {
#include "shared/core_switch/visible/switch_object.inc"
};

#endif
