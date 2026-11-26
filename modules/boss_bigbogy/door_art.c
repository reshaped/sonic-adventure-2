/* BOSS_BIGBOGY.PRS's texture lists, chunk models, texture list pointers and
 * texture names, from the disc. its C lies between sounds.c and sandglass.c,
 * and its D lies between dat_8c5860f0.c and door.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_bigbogy/door_art.h"

#include "boss_bigbogy/door_art/text.inc"

static NJS_TEXNAME floor_textures[1] = {
#include "boss_bigbogy/door_art/floor_textures.inc"
};
NJS_TEXLIST bigbogy_floor_door_texlist = {
#include "boss_bigbogy/door_art/bigbogy_floor_door_texlist.inc"
};
static Sint16 floor_polygons[91] = {
#include "boss_bigbogy/door_art/floor_polygons.inc"
};
static Sint32 floor_vertices[83] = {
#include "boss_bigbogy/door_art/floor_vertices.inc"
};
NJS_CNK_MODEL bigbogy_floor_door_model = {
#include "boss_bigbogy/door_art/bigbogy_floor_door_model.inc"
};
static NJS_CNK_OBJECT floor_door = {
#include "boss_bigbogy/door_art/floor_door.inc"
};
static NJS_TEXNAME wall_textures[1] = {
#include "boss_bigbogy/door_art/wall_textures.inc"
};
NJS_TEXLIST bigbogy_wall_door_texlist = {
#include "boss_bigbogy/door_art/bigbogy_wall_door_texlist.inc"
};
static Sint16 wall_polygons[91] = {
#include "boss_bigbogy/door_art/wall_polygons.inc"
};
static Sint32 wall_vertices[83] = {
#include "boss_bigbogy/door_art/wall_vertices.inc"
};
NJS_CNK_MODEL bigbogy_wall_door_model = {
#include "boss_bigbogy/door_art/bigbogy_wall_door_model.inc"
};
static NJS_CNK_OBJECT wall_door = {
#include "boss_bigbogy/door_art/wall_door.inc"
};
NJS_TEXLIST *bigbogy_door_texlists[3] = {
#include "boss_bigbogy/door_art/bigbogy_door_texlists.inc"
};
