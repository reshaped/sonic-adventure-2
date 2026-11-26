/* the stopper's texture list, model and solid in Metal Harbor, whose object
 * list calls the stopper MHSLIDER */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg10/slider_art/text.inc"

#include "shared/stopper_model.h"

static NJS_MATERIAL stopper_material[1] = {
#include "stg10/slider_art/stopper_material.inc"
};

static Sint16 stopper_strips[59] = {
#include "stg10/slider_art/stopper_strips.inc"
};

static NJS_MESHSET stopper_faces[1] = {
#include "stg10/slider_art/stopper_faces.inc"
};

static NJS_POINT3 stopper_points[26] = {
#include "stg10/slider_art/stopper_points.inc"
};

static NJS_VECTOR stopper_normals[26] = {
#include "stg10/slider_art/stopper_normals.inc"
};

static NJS_MODEL stopper_hull = {
#include "stg10/slider_art/stopper_hull.inc"
};

NJS_OBJECT stopper_collision = {
#include "stg10/slider_art/stopper_collision.inc"
};

static NJS_TEXNAME stopper_textures[6] = {
#include "stg10/slider_art/stopper_textures.inc"
};

NJS_TEXLIST stopper_texlist = {
#include "stg10/slider_art/stopper_texlist.inc"
};

static Sint16 lamp_polygons[43] = {
#include "stg10/slider_art/lamp_polygons.inc"
};

static Sint32 lamp_vertices[39] = {
#include "stg10/slider_art/lamp_vertices.inc"
};

static NJS_CNK_MODEL lamp_mesh = {
#include "stg10/slider_art/lamp_mesh.inc"
};

static NJS_CNK_OBJECT lamp_object = {
#include "stg10/slider_art/lamp_object.inc"
};

static Sint16 body_polygons[365] = {
#include "stg10/slider_art/body_polygons.inc"
};

static Sint32 body_vertices[375] = {
#include "stg10/slider_art/body_vertices.inc"
};

static NJS_CNK_MODEL body_mesh = {
#include "stg10/slider_art/body_mesh.inc"
};

NJS_CNK_OBJECT stopper_object[1] = {
#include "stg10/slider_art/stopper_object.inc"
};
