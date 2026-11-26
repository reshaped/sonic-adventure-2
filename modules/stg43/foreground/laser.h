#ifndef STG43_FOREGROUND_LASER_H
#define STG43_FOREGROUND_LASER_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME turret_textures[8] = {
#include "stg43/foreground/laser/turret_textures.inc"
};

static NJS_TEXLIST turret_texlist = {
#include "stg43/foreground/laser/turret_texlist.inc"
};

static Sint16 turret_elevating_barrel_polygons[237] = {
#include "stg43/foreground/laser/turret_elevating_barrel_polygons.inc"
};

static Sint32 turret_elevating_barrel_vertices[179] = {
#include "stg43/foreground/laser/turret_elevating_barrel_vertices.inc"
};

static NJS_CNK_MODEL turret_elevating_barrel_model = {
#include "stg43/foreground/laser/turret_elevating_barrel_model.inc"
};

static NJS_CNK_OBJECT turret_elevating_barrel = {
#include "stg43/foreground/laser/turret_elevating_barrel.inc"
};

static Sint16 turret_polygons[495] = {
#include "stg43/foreground/laser/turret_polygons.inc"
};

static Sint32 turret_vertices[455] = {
#include "stg43/foreground/laser/turret_vertices.inc"
};

static NJS_CNK_MODEL turret_model = {
#include "stg43/foreground/laser/turret_model.inc"
};

static NJS_CNK_OBJECT turret_object = {
#include "stg43/foreground/laser/turret_object.inc"
};

static NJS_TEXNAME turret_barrel_textures[4] = {
#include "stg43/foreground/laser/turret_barrel_textures.inc"
};

static NJS_TEXLIST turret_barrel_texlist = {
#include "stg43/foreground/laser/turret_barrel_texlist.inc"
};

static Sint16 turret_barrel_polygons[293] = {
#include "stg43/foreground/laser/turret_barrel_polygons.inc"
};

static Sint32 turret_barrel_vertices[151] = {
#include "stg43/foreground/laser/turret_barrel_vertices.inc"
};

static NJS_CNK_MODEL turret_barrel = {
#include "stg43/foreground/laser/turret_barrel.inc"
};

static NJS_CNK_OBJECT turret_barrel_object = {
#include "stg43/foreground/laser/turret_barrel_object.inc"
};

static NJS_TEXNAME turret_shard_textures[1] = {
#include "stg43/foreground/laser/turret_shard_textures.inc"
};

static NJS_TEXLIST turret_shard_texlist = {
#include "stg43/foreground/laser/turret_shard_texlist.inc"
};

static Sint16 turret_shard_polygons[25] = {
#include "stg43/foreground/laser/turret_shard_polygons.inc"
};

static Sint32 turret_shard_vertices[19] = {
#include "stg43/foreground/laser/turret_shard_vertices.inc"
};

static NJS_CNK_MODEL turret_shard[1] = {
#include "stg43/foreground/laser/turret_shard.inc"
};

static NJS_CNK_OBJECT turret_shard_object = {
#include "stg43/foreground/laser/turret_shard_object.inc"
};

static NJS_TEXNAME turret_wreck_textures[7] = {
#include "stg43/foreground/laser/turret_wreck_textures.inc"
};

static NJS_TEXLIST turret_wreck_texlist = {
#include "stg43/foreground/laser/turret_wreck_texlist.inc"
};

static Sint16 turret_wreck_polygons[545] = {
#include "stg43/foreground/laser/turret_wreck_polygons.inc"
};

static Sint32 turret_wreck_vertices[427] = {
#include "stg43/foreground/laser/turret_wreck_vertices.inc"
};

static NJS_CNK_MODEL turret_wreck_model = {
#include "stg43/foreground/laser/turret_wreck_model.inc"
};

static NJS_CNK_OBJECT turret_wreck = {
#include "stg43/foreground/laser/turret_wreck.inc"
};

#endif
