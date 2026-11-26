#ifndef STG43_FOREGROUND_GATE_H
#define STG43_FOREGROUND_GATE_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

static Sint16 gate_scroll_uvs[16] = {
#include "stg43/foreground/gate/gate_scroll_uvs.inc"
};

static struct texture_anim gate_scroll = {
#include "stg43/foreground/gate/gate_scroll.inc"
};

static NJS_TEXNAME gate_textures[4] = {
#include "stg43/foreground/gate/gate_textures.inc"
};

static NJS_TEXLIST gate_texlist = {
#include "stg43/foreground/gate/gate_texlist.inc"
};

static NJS_TEXNAME gate_lit_textures[4] = {
#include "stg43/foreground/gate/gate_lit_textures.inc"
};

static NJS_TEXLIST gate_lit_texlist = {
#include "stg43/foreground/gate/gate_lit_texlist.inc"
};

static Sint16 gate_panel_polygons[31] = {
#include "stg43/foreground/gate/gate_panel_polygons.inc"
};

static Sint32 gate_panel_vertices[27] = {
#include "stg43/foreground/gate/gate_panel_vertices.inc"
};

static NJS_CNK_MODEL gate_panel_model = {
#include "stg43/foreground/gate/gate_panel_model.inc"
};

static NJS_CNK_OBJECT gate_panel[1] = {
#include "stg43/foreground/gate/gate_panel.inc"
};

static Sint16 gate_housing_polygons[437] = {
#include "stg43/foreground/gate/gate_housing_polygons.inc"
};

static Sint32 gate_housing_vertices[403] = {
#include "stg43/foreground/gate/gate_housing_vertices.inc"
};

static NJS_CNK_MODEL gate_housing_model = {
#include "stg43/foreground/gate/gate_housing_model.inc"
};

static NJS_CNK_OBJECT gate_housing[1] = {
#include "stg43/foreground/gate/gate_housing.inc"
};

static NJS_MATERIAL gate_material = {
#include "stg43/foreground/gate/gate_material.inc"
};

static Sint16 gate_indices[11] = {
#include "stg43/foreground/gate/gate_indices.inc"
};

static NJS_MESHSET gate_faces = {
#include "stg43/foreground/gate/gate_faces.inc"
};

static NJS_POINT3 gate_points[8] = {
#include "stg43/foreground/gate/gate_points.inc"
};

static NJS_VECTOR gate_normals[8] = {
#include "stg43/foreground/gate/gate_normals.inc"
};

static NJS_MODEL gate_solid_model = {
#include "stg43/foreground/gate/gate_solid_model.inc"
};

static NJS_OBJECT gate_collision = {
#include "stg43/foreground/gate/gate_collision.inc"
};

#endif
