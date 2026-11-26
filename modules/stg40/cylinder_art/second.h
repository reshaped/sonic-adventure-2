#ifndef STG40_CYLINDER_ART_SECOND_H
#define STG40_CYLINDER_ART_SECOND_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME textures_second[16] = {
#include "stg40/cylinder_art/second/textures_second.inc"
};

static NJS_TEXLIST gravity_cylinder_1_texlist = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_texlist.inc"
};

static Sint16 gravity_cylinder_1_rings_polygons[339] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_rings_polygons.inc"
};

static Sint32 gravity_cylinder_1_rings_vertices[259] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_rings_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_1_rings_model = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_rings_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_1_rings[1] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_rings.inc"
};

static Sint16 gravity_cylinder_1_shell_polygons[107] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_shell_polygons.inc"
};

static Sint32 gravity_cylinder_1_shell_vertices[123] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_shell_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_1_shell_model = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_shell_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_1_shell[1] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_shell.inc"
};

static Sint16 gravity_cylinder_1_body_polygons[1107] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_body_polygons.inc"
};

static Sint32 gravity_cylinder_1_body_vertices[1095] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_body_vertices.inc"
};

static NJS_CNK_MODEL gravity_cylinder_1_body_model = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_body_model.inc"
};

static NJS_CNK_OBJECT gravity_cylinder_1_body[1] = {
#include "stg40/cylinder_art/second/gravity_cylinder_1_body.inc"
};

#endif
