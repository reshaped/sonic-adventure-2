#ifndef STG07_SCENERY_SCENERY_H
#define STG07_SCENERY_SCENERY_H

/* Scenery. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME miring_texlist_names[1] = {
#include "stg07/scenery/scenery/miring_texlist_names.inc"
};

static NJS_TEXLIST miring_texlist = {
#include "stg07/scenery/scenery/miring_texlist.inc"
};

static Sint16 miring_polygons[127] = {
#include "stg07/scenery/scenery/miring_polygons.inc"
};

static Sint32 miring_vertices[79] = {
#include "stg07/scenery/scenery/miring_vertices.inc"
};

static NJS_CNK_MODEL miring_model = {
#include "stg07/scenery/scenery/miring_model.inc"
};

static NJS_CNK_OBJECT miring_object = {
#include "stg07/scenery/scenery/miring_object.inc"
};

static NJS_TEXNAME mifence_texlist_names[3] = {
#include "stg07/scenery/scenery/mifence_texlist_names.inc"
};

static NJS_TEXLIST mifence_texlist = {
#include "stg07/scenery/scenery/mifence_texlist.inc"
};

static Sint16 mifence_polygons[581] = {
#include "stg07/scenery/scenery/mifence_polygons.inc"
};

static Sint32 mifence_vertices[547] = {
#include "stg07/scenery/scenery/mifence_vertices.inc"
};

static NJS_CNK_MODEL mifence_model = {
#include "stg07/scenery/scenery/mifence_model.inc"
};

static NJS_CNK_OBJECT mifence_object = {
#include "stg07/scenery/scenery/mifence_object.inc"
};

static NJS_TEXNAME mirail_texlist_names[2] = {
#include "stg07/scenery/scenery/mirail_texlist_names.inc"
};

static NJS_TEXLIST mirail_texlist = {
#include "stg07/scenery/scenery/mirail_texlist.inc"
};

static Sint16 mirail_polygons[395] = {
#include "stg07/scenery/scenery/mirail_polygons.inc"
};

static Sint32 mirail_vertices[395] = {
#include "stg07/scenery/scenery/mirail_vertices.inc"
};

static NJS_CNK_MODEL mirail_model = {
#include "stg07/scenery/scenery/mirail_model.inc"
};

static NJS_CNK_OBJECT mirail_object = {
#include "stg07/scenery/scenery/mirail_object.inc"
};

static NJS_TEXNAME milatti_texlist_names[3] = {
#include "stg07/scenery/scenery/milatti_texlist_names.inc"
};

static NJS_TEXLIST milatti_texlist = {
#include "stg07/scenery/scenery/milatti_texlist.inc"
};

static Sint16 milatti_polygons[283] = {
#include "stg07/scenery/scenery/milatti_polygons.inc"
};

static Sint32 milatti_vertices[319] = {
#include "stg07/scenery/scenery/milatti_vertices.inc"
};

static NJS_CNK_MODEL milatti_model = {
#include "stg07/scenery/scenery/milatti_model.inc"
};

static NJS_CNK_OBJECT milatti_object = {
#include "stg07/scenery/scenery/milatti_object.inc"
};

static NJS_TEXNAME mikusa_texlist_names[1] = {
#include "stg07/scenery/scenery/mikusa_texlist_names.inc"
};

static NJS_TEXLIST mikusa_texlist = {
#include "stg07/scenery/scenery/mikusa_texlist.inc"
};

static Sint16 mikusa_polygons[393] = {
#include "stg07/scenery/scenery/mikusa_polygons.inc"
};

static Sint32 mikusa_vertices[311] = {
#include "stg07/scenery/scenery/mikusa_vertices.inc"
};

static NJS_CNK_MODEL mikusa_model = {
#include "stg07/scenery/scenery/mikusa_model.inc"
};

static NJS_CNK_OBJECT mikusa_object = {
#include "stg07/scenery/scenery/mikusa_object.inc"
};

static NJS_TEXNAME mitruckp_texlist_names[4] = {
#include "stg07/scenery/scenery/mitruckp_texlist_names.inc"
};

static NJS_TEXLIST mitruckp_texlist = {
#include "stg07/scenery/scenery/mitruckp_texlist.inc"
};

static Sint16 mitruckp_polygons[405] = {
#include "stg07/scenery/scenery/mitruckp_polygons.inc"
};

static Sint32 mitruckp_vertices[427] = {
#include "stg07/scenery/scenery/mitruckp_vertices.inc"
};

static NJS_CNK_MODEL mitruckp_model = {
#include "stg07/scenery/scenery/mitruckp_model.inc"
};

static NJS_CNK_OBJECT mitruckp_object = {
#include "stg07/scenery/scenery/mitruckp_object.inc"
};

#endif
