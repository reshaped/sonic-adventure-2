/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last2/attack_art.h"
#include "boss_last2/texture_names.h"

const char finalhazard_sphere_filenames[8] = {
#include "boss_last2/attacks/sphere/finalhazard_sphere_filenames.inc"
};

/* BOSS_LAST2.PRS's texture lists and chunk models, from the disc. its D lies
 * between joints.c and spheres.c */
static NJS_TEXNAME sphere_names[1] = {
#include "boss_last2/attacks/sphere/sphere_names.inc"
};
static NJS_TEXLIST sphere_textures = {
#include "boss_last2/attacks/sphere/sphere_textures.inc"
};
static Sint16 sphere_near_polygons[425] = {
#include "boss_last2/attacks/sphere/sphere_near_polygons.inc"
};
static Sint32 sphere_near_vertices[267] = {
#include "boss_last2/attacks/sphere/sphere_near_vertices.inc"
};
static NJS_CNK_MODEL sphere_near_mesh = {
#include "boss_last2/attacks/sphere/sphere_near_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_sphere_near = {
#include "boss_last2/attacks/sphere/finalhazard_sphere_near.inc"
};
static Sint16 sphere_far_polygons[165] = {
#include "boss_last2/attacks/sphere/sphere_far_polygons.inc"
};
static Sint32 sphere_far_vertices[129] = {
#include "boss_last2/attacks/sphere/sphere_far_vertices.inc"
};
static NJS_CNK_MODEL sphere_far_mesh = {
#include "boss_last2/attacks/sphere/sphere_far_mesh.inc"
};
NJS_CNK_OBJECT finalhazard_sphere_far = {
#include "boss_last2/attacks/sphere/finalhazard_sphere_far.inc"
};
