/* the model that the wooden box, the iron box and the lid all draw */
#ifndef STG22_ART_FLOATING_BOX_H
#define STG22_ART_FLOATING_BOX_H

#include <ninja.h>
#include <sg_xpt.h>

/* the set editor's labels for a box's placement. no code reads them. each
 * field rounds its label up to whole words */
struct gadget_floating_box_captions {
    char crash_through[24];
    char crash_reflect[24];
    char angle_y[12];
    char animal[28];
    char range_x[16];
    char range_y[16];
    char range_z[16];
};

static const struct gadget_floating_box_captions floating_box_editor = {
#include "stg22/art/floating_box/floating_box_editor.inc"
};

static NJS_TEXNAME floating_box_textures[2] = {
#include "stg22/art/floating_box/floating_box_textures.inc"
};

static NJS_TEXLIST floating_box_texlist = {
#include "stg22/art/floating_box/floating_box_texlist.inc"
};

static Sint16 floating_box_polygons[97] = {
#include "stg22/art/floating_box/floating_box_polygons.inc"
};

static Sint32 floating_box_vertices[51] = {
#include "stg22/art/floating_box/floating_box_vertices.inc"
};

static NJS_CNK_MODEL floating_box_model[1] = {
#include "stg22/art/floating_box/floating_box_model.inc"
};

static NJS_CNK_OBJECT floating_box_object = {
#include "stg22/art/floating_box/floating_box_object.inc"
};

#endif
