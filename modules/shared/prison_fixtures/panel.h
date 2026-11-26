#ifndef SHARED_PRISON_FIXTURES_PANEL_H
#define SHARED_PRISON_FIXTURES_PANEL_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME panel_names[3] = {
#include "shared/prison_fixtures/panel/panel_names.inc"
};

static NJS_TEXLIST panel_texlist = {
#include "shared/prison_fixtures/panel/panel_texlist.inc"
};

static Sint16 panel_polygons[155] = {
#include "shared/prison_fixtures/panel/panel_polygons.inc"
};

static Sint32 panel_vertices[83] = {
#include "shared/prison_fixtures/panel/panel_vertices.inc"
};

static NJS_CNK_MODEL panel_model = {
#include "shared/prison_fixtures/panel/panel_model.inc"
};

static NJS_CNK_OBJECT panel_object[1] = {
#include "shared/prison_fixtures/panel/panel_object.inc"
};

static NJS_MATERIAL panel_solid_materials = {
#include "shared/prison_fixtures/panel/panel_solid_materials.inc"
};

static Sint16 panel_solid_strips[19] = {
#include "shared/prison_fixtures/panel/panel_solid_strips.inc"
};

static NJS_MESHSET panel_solid_faces = {
#include "shared/prison_fixtures/panel/panel_solid_faces.inc"
};

static NJS_POINT3 panel_solid_points[8] = {
#include "shared/prison_fixtures/panel/panel_solid_points.inc"
};

static NJS_VECTOR panel_solid_normals[8] = {
#include "shared/prison_fixtures/panel/panel_solid_normals.inc"
};

static NJS_MODEL panel_solid_model = {
#include "shared/prison_fixtures/panel/panel_solid_model.inc"
};

static NJS_OBJECT panel_collision = {
#include "shared/prison_fixtures/panel/panel_collision.inc"
};

#endif
