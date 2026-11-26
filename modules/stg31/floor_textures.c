#include <ninja.h>

#include "stg31/floor_textures.h"
#include "stg31/platforms.h"

const char green_hill_floor_face_1[16] = {
#include "stg31/floor_textures/green_hill_floor_face_1.inc"
};
const char green_hill_floor_face_2[16] = {
#include "stg31/floor_textures/green_hill_floor_face_2.inc"
};
const char green_hill_floor_face_3[16] = {
#include "stg31/floor_textures/green_hill_floor_face_3.inc"
};
const char green_hill_floor_face_4[16] = {
#include "stg31/floor_textures/green_hill_floor_face_4.inc"
};
static const char floor_top[16] = {
#include "stg31/floor_textures/floor_top.inc"
};
const char green_hill_floor_edge[14] = {
#include "stg31/floor_textures/green_hill_floor_edge.inc"
};
static NJS_TEXNAME textures[6] = {
#include "stg31/floor_textures/textures.inc"
};
NJS_TEXLIST green_hill_crumbling_floor_texlist = {
    textures, sizeof(textures) / sizeof(textures[0])};
