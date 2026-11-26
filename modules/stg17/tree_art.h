/* Ground quad and the street tree that turns to face the camera. */
#ifndef STG17_TREE_ART_H
#define STG17_TREE_ART_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_tree_textures[3] = {
#include "stg17/tree_art/street_tree_textures.inc"
};

static NJS_TEXLIST street_tree_texlist = {
#include "stg17/tree_art/street_tree_texlist.inc"
};

static Sint16 street_tree_billboard_polygons[199] = {
#include "stg17/tree_art/street_tree_billboard_polygons.inc"
};

static Sint32 street_tree_billboard_vertices[167] = {
#include "stg17/tree_art/street_tree_billboard_vertices.inc"
};

static NJS_CNK_MODEL street_tree_billboard_mesh = {
#include "stg17/tree_art/street_tree_billboard_mesh.inc"
};

static NJS_CNK_OBJECT street_tree_billboard = {
#include "stg17/tree_art/street_tree_billboard.inc"
};

static Sint16 street_tree_root_polygons[39] = {
#include "stg17/tree_art/street_tree_root_polygons.inc"
};

static Sint32 street_tree_root_vertices[35] = {
#include "stg17/tree_art/street_tree_root_vertices.inc"
};

static NJS_CNK_MODEL street_tree_root_mesh = {
#include "stg17/tree_art/street_tree_root_mesh.inc"
};

static NJS_CNK_OBJECT street_tree_root = {
#include "stg17/tree_art/street_tree_root.inc"
};

#endif
