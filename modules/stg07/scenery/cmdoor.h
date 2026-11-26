#ifndef STG07_SCENERY_CMDOOR_H
#define STG07_SCENERY_CMDOOR_H

/* Cmdoor. */
#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME cmdoor_texlist_names[2] = {
#include "stg07/scenery/cmdoor/cmdoor_texlist_names.inc"
};

static NJS_TEXLIST cmdoor_texlist = {
#include "stg07/scenery/cmdoor/cmdoor_texlist.inc"
};

static Sint16 cmdoor_polygons[819] = {
#include "stg07/scenery/cmdoor/cmdoor_polygons.inc"
};

static Sint32 cmdoor_vertices[827] = {
#include "stg07/scenery/cmdoor/cmdoor_vertices.inc"
};

static NJS_CNK_MODEL cmdoor_model = {
#include "stg07/scenery/cmdoor/cmdoor_model.inc"
};

static NJS_CNK_OBJECT cmdoor_object = {
#include "stg07/scenery/cmdoor/cmdoor_object.inc"
};

static NJS_TEXNAME mine_sign_texlist_names[3] = {
#include "stg07/scenery/cmdoor/mine_sign_texlist_names.inc"
};

static NJS_TEXLIST mine_sign_texlist = {
#include "stg07/scenery/cmdoor/mine_sign_texlist.inc"
};

static Sint16 mine_sign_polygons[147] = {
#include "stg07/scenery/cmdoor/mine_sign_polygons.inc"
};

static Sint32 mine_sign_vertices[131] = {
#include "stg07/scenery/cmdoor/mine_sign_vertices.inc"
};

static NJS_CNK_MODEL mine_sign_model = {
#include "stg07/scenery/cmdoor/mine_sign_model.inc"
};

static NJS_CNK_OBJECT mine_sign_object[1] = {
#include "stg07/scenery/cmdoor/mine_sign_object.inc"
};

#endif
