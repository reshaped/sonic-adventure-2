/* BOSS_LAST1.PRS's chunk models, from the disc. its D lies between
 * last_boss_defend.c and path_lights.c */
#include <ninja.h>
#include <sg_xpt.h>

static Sint16 light_polygons[139] = {
#include "boss_last1/biolizard_path_light_art/light_polygons.inc"
};
static Sint32 light_vertices[267] = {
#include "boss_last1/biolizard_path_light_art/light_vertices.inc"
};
static NJS_CNK_MODEL light_mesh = {
#include "boss_last1/biolizard_path_light_art/light_mesh.inc"
};
static NJS_CNK_OBJECT unused_path_light = {
#include "boss_last1/biolizard_path_light_art/unused_path_light.inc"
};
