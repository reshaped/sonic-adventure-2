/* STG23.PRS's path list, texture lists, chunk models, texture group, texture
 * groups, texture pvms and Basic models, from the disc. its D lies between
 * the section's start and hidden_base.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "stage/path.h"
#include "stg23/background.h"
#include "stg23/base_desert_pyramid2.h"
#include "stg23/dat_8c545aec.h"
#include "stg23/hidden_base.h"
#include "stg23/hidden_base_sand_high_desert_filenames.h"
#include "stg23/hidden_base_sand_low_desert_filenames.h"
#include "system/texture.h"

struct path *hidden_base_paths[1] = {
#include "stg23/hidden_base_sand_far_pyramid/hidden_base_paths.inc"
};
static NJS_TEXNAME hidden_base_sand_far_pyramid_textures[1] = {
#include "stg23/hidden_base_sand_far_pyramid/hidden_base_sand_far_pyramid_textures.inc"
};
NJS_TEXLIST base_far_pyramid_texlist = {
#include "stg23/hidden_base_sand_far_pyramid/base_far_pyramid_texlist.inc"
};
static Sint16 base_far_pyramid_polygons[53] = {
#include "stg23/hidden_base_sand_far_pyramid/base_far_pyramid_polygons.inc"
};
static Sint32 base_far_pyramid_vertices[51] = {
#include "stg23/hidden_base_sand_far_pyramid/base_far_pyramid_vertices.inc"
};
NJS_CNK_MODEL base_far_pyramid = {
#include "stg23/hidden_base_sand_far_pyramid/base_far_pyramid.inc"
};
static NJS_CNK_OBJECT base_far_pyramid_object = {
#include "stg23/hidden_base_sand_far_pyramid/base_far_pyramid_object.inc"
};
static NJS_TEXNAME hidden_base_sand_low_desert_textures[2] = {
#include "stg23/hidden_base_sand_far_pyramid/hidden_base_sand_low_desert_textures.inc"
};
NJS_TEXLIST base_low_desert_texlist = {
#include "stg23/hidden_base_sand_far_pyramid/base_low_desert_texlist.inc"
};
static Sint16 base_low_desert_polygons[403] = {
#include "stg23/hidden_base_sand_far_pyramid/base_low_desert_polygons.inc"
};
static Sint32 base_low_desert_vertices[223] = {
#include "stg23/hidden_base_sand_far_pyramid/base_low_desert_vertices.inc"
};
NJS_CNK_MODEL base_low_desert = {
#include "stg23/hidden_base_sand_far_pyramid/base_low_desert.inc"
};
static NJS_CNK_OBJECT base_low_desert_object = {
#include "stg23/hidden_base_sand_far_pyramid/base_low_desert_object.inc"
};
static NJS_TEXNAME hidden_base_sand_high_desert_textures[3] = {
#include "stg23/hidden_base_sand_far_pyramid/hidden_base_sand_high_desert_textures.inc"
};
NJS_TEXLIST base_high_desert_texlist = {
#include "stg23/hidden_base_sand_far_pyramid/base_high_desert_texlist.inc"
};
static Sint16 base_high_desert_polygons[789] = {
#include "stg23/hidden_base_sand_far_pyramid/base_high_desert_polygons.inc"
};
static Sint32 base_high_desert_vertices[519] = {
#include "stg23/hidden_base_sand_far_pyramid/base_high_desert_vertices.inc"
};
NJS_CNK_MODEL base_high_desert = {
#include "stg23/hidden_base_sand_far_pyramid/base_high_desert.inc"
};
static NJS_CNK_OBJECT base_high_desert_object = {
#include "stg23/hidden_base_sand_far_pyramid/base_high_desert_object.inc"
};
static NJS_TEXNAME hidden_base_background_textures[5] = {
#include "stg23/hidden_base_sand_far_pyramid/hidden_base_background_textures.inc"
};
static NJS_TEXLIST bank = {
#include "stg23/hidden_base_sand_far_pyramid/bank.inc"
};
static NJS_TEXLIST *models[4] = {
#include "stg23/hidden_base_sand_far_pyramid/models.inc"
};
NJS_TEXLIST **base_background_groups[2] = {
#include "stg23/hidden_base_sand_far_pyramid/base_background_groups.inc"
};
struct texture_pvm base_background_pvms[2] = {
#include "stg23/hidden_base_sand_far_pyramid/base_background_pvms.inc"
};
static NJS_MATERIAL base_sink_0_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_materials.inc"
};
static Sint16 base_sink_0_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_faces.inc"
};
static NJS_MESHSET base_sink_0_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_meshsets.inc"
};
static NJS_POINT3 base_sink_0_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_points.inc"
};
static NJS_POINT3 base_sink_0_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_normals.inc"
};
static NJS_MODEL base_sink_0_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0_model.inc"
};
NJS_OBJECT base_sink_0 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_0.inc"
};
static NJS_MATERIAL base_sink_1_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_materials.inc"
};
static Sint16 base_sink_1_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_faces.inc"
};
static NJS_MESHSET base_sink_1_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_meshsets.inc"
};
static NJS_POINT3 base_sink_1_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_points.inc"
};
static NJS_POINT3 base_sink_1_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_normals.inc"
};
static NJS_MODEL base_sink_1_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1_model.inc"
};
NJS_OBJECT base_sink_1 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_1.inc"
};
static NJS_MATERIAL base_sink_2_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_materials.inc"
};
static Sint16 base_sink_2_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_faces.inc"
};
static NJS_MESHSET base_sink_2_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_meshsets.inc"
};
static NJS_POINT3 base_sink_2_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_points.inc"
};
static NJS_POINT3 base_sink_2_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_normals.inc"
};
static NJS_MODEL base_sink_2_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2_model.inc"
};
NJS_OBJECT base_sink_2 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_2.inc"
};
static NJS_MATERIAL base_sink_3_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_materials.inc"
};
static Sint16 base_sink_3_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_faces.inc"
};
static NJS_MESHSET base_sink_3_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_meshsets.inc"
};
static NJS_POINT3 base_sink_3_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_points.inc"
};
static NJS_POINT3 base_sink_3_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_normals.inc"
};
static NJS_MODEL base_sink_3_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3_model.inc"
};
NJS_OBJECT base_sink_3 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_3.inc"
};
static NJS_MATERIAL base_sink_4_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_materials.inc"
};
static Sint16 base_sink_4_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_faces.inc"
};
static NJS_MESHSET base_sink_4_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_meshsets.inc"
};
static NJS_POINT3 base_sink_4_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_points.inc"
};
static NJS_POINT3 base_sink_4_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_normals.inc"
};
static NJS_MODEL base_sink_4_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4_model.inc"
};
NJS_OBJECT base_sink_4 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_4.inc"
};
static NJS_MATERIAL base_sink_5_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_materials.inc"
};
static Sint16 base_sink_5_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_faces.inc"
};
static NJS_MESHSET base_sink_5_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_meshsets.inc"
};
static NJS_POINT3 base_sink_5_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_points.inc"
};
static NJS_POINT3 base_sink_5_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_normals.inc"
};
static NJS_MODEL base_sink_5_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5_model.inc"
};
NJS_OBJECT base_sink_5 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_5.inc"
};
static NJS_MATERIAL base_sink_6_materials[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_materials.inc"
};
static Sint16 base_sink_6_faces[5] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_faces.inc"
};
static NJS_MESHSET base_sink_6_meshsets[1] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_meshsets.inc"
};
static NJS_POINT3 base_sink_6_points[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_points.inc"
};
static NJS_POINT3 base_sink_6_normals[4] = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_normals.inc"
};
static NJS_MODEL base_sink_6_model = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6_model.inc"
};
NJS_OBJECT base_sink_6 = {
#include "stg23/hidden_base_sand_far_pyramid/base_sink_6.inc"
};
