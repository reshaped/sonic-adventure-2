#ifndef STG40_CYLINDER_ART_FIRST_H
#define STG40_CYLINDER_ART_FIRST_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_first[15] = {
#include "stg40/cylinder_art/first/textures_first.inc"
};

static NJS_TEXLIST gravity_cylinder_0_texlist = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_texlist.inc"
};

static Sint16 gravity_cylinder_0_shell_polygons[107] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_shell_polygons.inc"
};

static Sint32 gravity_cylinder_0_shell_vertices[123] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_shell_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_0_shell_model = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_shell_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_0_shell[1] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_shell.inc"
};

static Sint16 gravity_cylinder_0_rings_polygons[339] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_rings_polygons.inc"
};

static Sint32 gravity_cylinder_0_rings_vertices[259] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_rings_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_0_rings_model = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_rings_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_0_rings[1] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_rings.inc"
};

static Sint16 gravity_cylinder_0_body_polygons[933] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_body_polygons.inc"
};

static Sint32 gravity_cylinder_0_body_vertices[971] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_body_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_0_body_model = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_body_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_0_body[1] = {
#include "stg40/cylinder_art/first/gravity_cylinder_0_body.inc"
};

#endif
