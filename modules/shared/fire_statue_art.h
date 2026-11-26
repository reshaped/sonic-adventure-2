#ifndef SHARED_FIRE_STATUE_ART_H
#define SHARED_FIRE_STATUE_ART_H

/* Fire statue. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME fire_statue_texlist_names[4] = {
#include "shared/fire_statue_art/fire_statue_texlist_names.inc"
};

static NJS_TEXLIST fire_statue_texlist = {
#include "shared/fire_statue_art/fire_statue_texlist.inc"
};

static Sint16 fire_statue_object_child_sibling_polygons[87] = {
#include "shared/fire_statue_art/fire_statue_object_child_sibling_polygons.inc"
};

static Sint32 fire_statue_object_child_sibling_vertices[87] = {
#include "shared/fire_statue_art/fire_statue_object_child_sibling_vertices.inc"
};

static NJS_CNK_MODEL fire_statue_object_child_sibling_model = {
#include "shared/fire_statue_art/fire_statue_object_child_sibling_model.inc"
};

static NJS_CNK_OBJECT fire_statue_object_child_sibling = {
#include "shared/fire_statue_art/fire_statue_object_child_sibling.inc"
};

static Sint16 fire_statue_object_child_polygons[83] = {
#include "shared/fire_statue_art/fire_statue_object_child_polygons.inc"
};

static Sint32 fire_statue_object_child_vertices[51] = {
#include "shared/fire_statue_art/fire_statue_object_child_vertices.inc"
};

static NJS_CNK_MODEL fire_statue_object_child_model = {
#include "shared/fire_statue_art/fire_statue_object_child_model.inc"
};

static NJS_CNK_OBJECT fire_statue_object_child = {
#include "shared/fire_statue_art/fire_statue_object_child.inc"
};

static Sint16 fire_statue_polygons[519] = {
#include "shared/fire_statue_art/fire_statue_polygons.inc"
};

static Sint32 fire_statue_vertices[285] = {
#include "shared/fire_statue_art/fire_statue_vertices.inc"
};

static NJS_CNK_MODEL fire_statue_model = {
#include "shared/fire_statue_art/fire_statue_model.inc"
};

static NJS_CNK_OBJECT fire_statue_object[1] = {
#include "shared/fire_statue_art/fire_statue_object.inc"
};

static NJS_TEXNAME fire_texlist_names[10] = {
#include "shared/fire_statue_art/fire_texlist_names.inc"
};

static NJS_TEXLIST fire_texlist = {
#include "shared/fire_statue_art/fire_texlist.inc"
};

#endif
