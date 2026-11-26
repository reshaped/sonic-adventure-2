#ifndef STG28_MODELS_SHIP_H
#define STG28_MODELS_SHIP_H
#include <ninja.h>
#include <sg_xpt.h>

enum {
    SHIP_NORMAL_TEX_DC_PC_CROUCHWALL64,
    SHIP_NORMAL_TEX_DZ_PC_CHAIN8,
    SHIP_NORMAL_TEX_DZ_PC_HASIRA128,
    SHIP_NORMAL_TEX_DZ_PC_POT_WALL128,
    SHIP_NORMAL_TEX_DZ_PC_SHIP128,
    SHIP_NORMAL_TEX_DZ_PC_SHIPRUST64,
    SHIP_NORMAL_TEX_DZ_PC_SKIN64,
    SHIP_NORMAL_TEX_KD_PC_HASIRA1,
    SHIP_NORMAL_TEX_KD_PC_HASIRA4,
    SHIP_NORMAL_TEX_KD_PC_HEBI2,
    SHIP_NORMAL_TEX_KD_PC_YUKA1,
    SHIP_NORMAL_TEX_KD_PC_YUKA3
};

enum {
    SHIP_EGGMAN_TEX_DC_PC_CROUCHWALL64,
    SHIP_EGGMAN_TEX_DZ_PC_CHAIN8,
    SHIP_EGGMAN_TEX_DZ_PC_HASIRA128,
    SHIP_EGGMAN_TEX_DZ_PC_POT_WALL128,
    SHIP_EGGMAN_TEX_DZ_PC_SHIP128,
    SHIP_EGGMAN_TEX_DZ_PC_SHIPRUST64,
    SHIP_EGGMAN_TEX_DZ_PC_SKIN64,
    SHIP_EGGMAN_TEX_KD_PC_DOOR1,
    SHIP_EGGMAN_TEX_KD_PC_HASIRA1,
    SHIP_EGGMAN_TEX_KD_PC_HASIRA4,
    SHIP_EGGMAN_TEX_KD_PC_YUKA1,
    SHIP_EGGMAN_TEX_KD_PC_YUKA3
};

static NJS_TEXNAME cave_ship_normal_texture_names[12] = {
#include "stg28/models/ship/cave_ship_normal_texture_names.inc"
};

static NJS_TEXLIST cave_ship_normal_texlist = {
#include "stg28/models/ship/cave_ship_normal_texlist.inc"
};

static Sint16 cave_ship_normal_polygons[1641] = {
#include "stg28/models/ship/cave_ship_normal_polygons.inc"
};

static Sint32 cave_ship_normal_vertices[1023] = {
#include "stg28/models/ship/cave_ship_normal_vertices.inc"
};

static NJS_CNK_MODEL cave_ship_normal_model = {
#include "stg28/models/ship/cave_ship_normal_model.inc"
};

static NJS_CNK_OBJECT cave_ship_normal_object = {
#include "stg28/models/ship/cave_ship_normal_object.inc"
};

static NJS_MKEY_A cave_ship_normal_motion_angle_keys[186] = {
#include "stg28/models/ship/cave_ship_normal_motion_angle_keys.inc"
};

static NJS_MDATA2 cave_ship_normal_motion_tracks[1] = {
#include "stg28/models/ship/cave_ship_normal_motion_tracks.inc"
};

static NJS_MOTION cave_ship_normal_motion = {
#include "stg28/models/ship/cave_ship_normal_motion.inc"
};

static NJS_TEXNAME cave_ship_eggman_texture_names[12] = {
#include "stg28/models/ship/cave_ship_eggman_texture_names.inc"
};

static NJS_TEXLIST cave_ship_eggman_texlist = {
#include "stg28/models/ship/cave_ship_eggman_texlist.inc"
};

static Sint16 cave_ship_eggman_polygons[1733] = {
#include "stg28/models/ship/cave_ship_eggman_polygons.inc"
};

static Sint32 cave_ship_eggman_vertices[943] = {
#include "stg28/models/ship/cave_ship_eggman_vertices.inc"
};

static NJS_CNK_MODEL cave_ship_eggman_model = {
#include "stg28/models/ship/cave_ship_eggman_model.inc"
};

static NJS_CNK_OBJECT cave_ship_eggman_object = {
#include "stg28/models/ship/cave_ship_eggman_object.inc"
};

static NJS_MKEY_A cave_ship_eggman_motion_angle_keys[186] = {
#include "stg28/models/ship/cave_ship_eggman_motion_angle_keys.inc"
};

static NJS_MDATA2 cave_ship_eggman_motion_tracks[1] = {
#include "stg28/models/ship/cave_ship_eggman_motion_tracks.inc"
};

static NJS_MOTION cave_ship_eggman_motion = {
#include "stg28/models/ship/cave_ship_eggman_motion.inc"
};

#endif
