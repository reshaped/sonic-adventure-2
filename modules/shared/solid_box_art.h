/* Both SOLIDBOX variants use this geometry with their own texture archive. */
#ifndef SHARED_SOLID_BOX_ART_H
#define SHARED_SOLID_BOX_ART_H
#include <ninja.h>
#include <sg_xpt.h>

#define SOLID_BOX_TEXTURES 2
#define SOLID_BOX_POINTS 8

static NJS_TEXNAME solid_box_texnames[SOLID_BOX_TEXTURES] = {
#include "shared/solid_box_art/solid_box_texnames.inc"
};
static NJS_TEXLIST solid_box_texlist = {
#include "shared/solid_box_art/solid_box_texlist.inc"
};

/* the box's model: a cube without a bottom. texture 1 covers its top, and
 * texture 0 each of its four sides */
static Sint16 solid_box_polygons[83] = {
#include "shared/solid_box_art/solid_box_polygons.inc"
};
static Sint32 solid_box_vertices[51] = {
#include "shared/solid_box_art/solid_box_vertices.inc"
};
static NJS_CNK_MODEL solid_box_model = {
#include "shared/solid_box_art/solid_box_model.inc"
};
static NJS_CNK_OBJECT solid_box_object = {
#include "shared/solid_box_art/solid_box_object.inc"
};

#endif
