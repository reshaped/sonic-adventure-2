/* Egg Quarters' rooms: their texture lists, models, shadow volumes and kill
 * area */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg26/egg_quarters.h"
#include "stg26/room_scenery.h"
#include "system/texture.h"

typedef NJS_TEXLIST *quarters_texture_binding;

static const char band_material[16] = {
#include "stg26/room_scenery/band_material.inc"
};
static const char pillar_material[16] = {
#include "stg26/room_scenery/pillar_material.inc"
};
static const char water_material_01[16] = {
#include "stg26/room_scenery/water_material_01.inc"
};
static const char water_material_02[16] = {
#include "stg26/room_scenery/water_material_02.inc"
};
static const char water_material_03[16] = {
#include "stg26/room_scenery/water_material_03.inc"
};
static const char water_material_04[16] = {
#include "stg26/room_scenery/water_material_04.inc"
};
static const char water_material_05[16] = {
#include "stg26/room_scenery/water_material_05.inc"
};
static const char water_material_06[16] = {
#include "stg26/room_scenery/water_material_06.inc"
};
static const char water_material_07[16] = {
#include "stg26/room_scenery/water_material_07.inc"
};
static const char water_material_08[16] = {
#include "stg26/room_scenery/water_material_08.inc"
};
static const char water_material_09[16] = {
#include "stg26/room_scenery/water_material_09.inc"
};
static const char water_material_10[16] = {
#include "stg26/room_scenery/water_material_10.inc"
};
static const char water_material_11[16] = {
#include "stg26/room_scenery/water_material_11.inc"
};
static const char water_material_12[16] = {
#include "stg26/room_scenery/water_material_12.inc"
};
static const char water_material_13[16] = {
#include "stg26/room_scenery/water_material_13.inc"
};
static const char water_material_14[16] = {
#include "stg26/room_scenery/water_material_14.inc"
};
static const char background_archive[8] = {
#include "stg26/room_scenery/background_archive.inc"
};

#pragma section ATA

/* Wall bands. */
static NJS_TEXNAME band_textures[1] = {
#include "stg26/room_scenery/band_textures.inc"
};

NJS_TEXLIST quarters_band_texlist = {
#include "stg26/room_scenery/quarters_band_texlist.inc"
};

static Sint16 north_band_polygons[89] = {
#include "stg26/room_scenery/north_band_polygons.inc"
};

static Sint32 north_band_vertices[67] = {
#include "stg26/room_scenery/north_band_vertices.inc"
};

NJS_CNK_MODEL quarters_north_band = {
#include "stg26/room_scenery/quarters_north_band.inc"
};

NJS_CNK_OBJECT quarters_north_band_object = {
#include "stg26/room_scenery/quarters_north_band_object.inc"
};

static NJS_TEXNAME side_band_textures[1] = {
#include "stg26/room_scenery/side_band_textures.inc"
};

static NJS_TEXLIST side_band_texlist = {
#include "stg26/room_scenery/side_band_texlist.inc"
};

static Sint16 side_band_polygons[129] = {
#include "stg26/room_scenery/side_band_polygons.inc"
};

static Sint32 side_band_vertices[131] = {
#include "stg26/room_scenery/side_band_vertices.inc"
};

NJS_CNK_MODEL quarters_side_band = {
#include "stg26/room_scenery/quarters_side_band.inc"
};

NJS_CNK_OBJECT quarters_side_band_object = {
#include "stg26/room_scenery/quarters_side_band_object.inc"
};

/* Pillars. */
static NJS_TEXNAME pillar_0_textures[1] = {
#include "stg26/room_scenery/pillar_0_textures.inc"
};

NJS_TEXLIST quarters_pillar_0_texlist = {
#include "stg26/room_scenery/quarters_pillar_0_texlist.inc"
};

static Sint16 pillar_0_polygons[55] = {
#include "stg26/room_scenery/pillar_0_polygons.inc"
};

static Sint32 pillar_0_vertices[51] = {
#include "stg26/room_scenery/pillar_0_vertices.inc"
};

NJS_CNK_MODEL quarters_pillar_0_model = {
#include "stg26/room_scenery/quarters_pillar_0_model.inc"
};

NJS_CNK_OBJECT quarters_pillar_0_object = {
#include "stg26/room_scenery/quarters_pillar_0_object.inc"
};

static NJS_TEXNAME pillar_1_textures[1] = {
#include "stg26/room_scenery/pillar_1_textures.inc"
};

NJS_TEXLIST quarters_pillar_1_texlist = {
#include "stg26/room_scenery/quarters_pillar_1_texlist.inc"
};

static Sint16 pillar_1_polygons[55] = {
#include "stg26/room_scenery/pillar_1_polygons.inc"
};

static Sint32 pillar_1_vertices[51] = {
#include "stg26/room_scenery/pillar_1_vertices.inc"
};

NJS_CNK_MODEL quarters_pillar_1_model = {
#include "stg26/room_scenery/quarters_pillar_1_model.inc"
};

NJS_CNK_OBJECT quarters_pillar_1_object = {
#include "stg26/room_scenery/quarters_pillar_1_object.inc"
};

static NJS_TEXNAME pillar_2_textures[1] = {
#include "stg26/room_scenery/pillar_2_textures.inc"
};

NJS_TEXLIST quarters_pillar_2_texlist = {
#include "stg26/room_scenery/quarters_pillar_2_texlist.inc"
};

static Sint16 pillar_2_polygons[55] = {
#include "stg26/room_scenery/pillar_2_polygons.inc"
};

static Sint32 pillar_2_vertices[51] = {
#include "stg26/room_scenery/pillar_2_vertices.inc"
};

NJS_CNK_MODEL quarters_pillar_2_model = {
#include "stg26/room_scenery/quarters_pillar_2_model.inc"
};

NJS_CNK_OBJECT quarters_pillar_2_object = {
#include "stg26/room_scenery/quarters_pillar_2_object.inc"
};

static NJS_TEXNAME pillar_3_textures[1] = {
#include "stg26/room_scenery/pillar_3_textures.inc"
};

NJS_TEXLIST quarters_pillar_3_texlist = {
#include "stg26/room_scenery/quarters_pillar_3_texlist.inc"
};

static Sint16 pillar_3_polygons[55] = {
#include "stg26/room_scenery/pillar_3_polygons.inc"
};

static Sint32 pillar_3_vertices[51] = {
#include "stg26/room_scenery/pillar_3_vertices.inc"
};

NJS_CNK_MODEL quarters_pillar_3_model = {
#include "stg26/room_scenery/quarters_pillar_3_model.inc"
};

NJS_CNK_OBJECT quarters_pillar_3_object = {
#include "stg26/room_scenery/quarters_pillar_3_object.inc"
};

/* South room shadow volumes. */

static Sint16 south_shadow_4_polygons[353] = {
#include "stg26/room_scenery/south_shadow_4_polygons.inc"
};

struct quarters_shadow_vertices {
    Sint32 head[2];
    NJS_POINT3 points[60];
    Sint32 end;
};

static struct quarters_shadow_vertices south_shadow_4_vertices = {
#include "stg26/room_scenery/south_shadow_4_vertices.inc"
};

static NJS_CNK_MODEL south_shadow_4_model = {
#include "stg26/room_scenery/south_shadow_4_model.inc"
};

static NJS_CNK_OBJECT south_shadow_4 = {
#include "stg26/room_scenery/south_shadow_4.inc"
};

static Sint16 south_shadow_3_polygons[353] = {
#include "stg26/room_scenery/south_shadow_3_polygons.inc"
};

static struct quarters_shadow_vertices south_shadow_3_vertices = {
#include "stg26/room_scenery/south_shadow_3_vertices.inc"
};

static NJS_CNK_MODEL south_shadow_3_model = {
#include "stg26/room_scenery/south_shadow_3_model.inc"
};

static NJS_CNK_OBJECT south_shadow_3 = {
#include "stg26/room_scenery/south_shadow_3.inc"
};

static Sint16 south_shadow_2_polygons[353] = {
#include "stg26/room_scenery/south_shadow_2_polygons.inc"
};

static struct quarters_shadow_vertices south_shadow_2_vertices = {
#include "stg26/room_scenery/south_shadow_2_vertices.inc"
};

static NJS_CNK_MODEL south_shadow_2_model = {
#include "stg26/room_scenery/south_shadow_2_model.inc"
};

static NJS_CNK_OBJECT south_shadow_2 = {
#include "stg26/room_scenery/south_shadow_2.inc"
};

static Sint16 south_shadow_1_polygons[353] = {
#include "stg26/room_scenery/south_shadow_1_polygons.inc"
};

static struct quarters_shadow_vertices south_shadow_1_vertices = {
#include "stg26/room_scenery/south_shadow_1_vertices.inc"
};

static NJS_CNK_MODEL south_shadow_1_model = {
#include "stg26/room_scenery/south_shadow_1_model.inc"
};

static NJS_CNK_OBJECT south_shadow_1 = {
#include "stg26/room_scenery/south_shadow_1.inc"
};

NJS_CNK_OBJECT quarters_south_modifiers = {
#include "stg26/room_scenery/quarters_south_modifiers.inc"
};

/* East room shadow volumes. */
static Sint16 east_shadow_2_polygons[353] = {
#include "stg26/room_scenery/east_shadow_2_polygons.inc"
};

static struct quarters_shadow_vertices east_shadow_2_vertices = {
#include "stg26/room_scenery/east_shadow_2_vertices.inc"
};

static NJS_CNK_MODEL east_shadow_2_model = {
#include "stg26/room_scenery/east_shadow_2_model.inc"
};

static NJS_CNK_OBJECT east_shadow_2 = {
#include "stg26/room_scenery/east_shadow_2.inc"
};

static Sint16 east_shadow_1_polygons[353] = {
#include "stg26/room_scenery/east_shadow_1_polygons.inc"
};

static struct quarters_shadow_vertices east_shadow_1_vertices = {
#include "stg26/room_scenery/east_shadow_1_vertices.inc"
};

static NJS_CNK_MODEL east_shadow_1_model = {
#include "stg26/room_scenery/east_shadow_1_model.inc"
};

static NJS_CNK_OBJECT east_shadow_1 = {
#include "stg26/room_scenery/east_shadow_1.inc"
};

NJS_CNK_OBJECT quarters_east_modifiers = {
#include "stg26/room_scenery/quarters_east_modifiers.inc"
};

/* West room shadow volumes. */
static Sint16 west_shadow_2_polygons[353] = {
#include "stg26/room_scenery/west_shadow_2_polygons.inc"
};

static struct quarters_shadow_vertices west_shadow_2_vertices = {
#include "stg26/room_scenery/west_shadow_2_vertices.inc"
};

static NJS_CNK_MODEL west_shadow_2_model = {
#include "stg26/room_scenery/west_shadow_2_model.inc"
};

static NJS_CNK_OBJECT west_shadow_2 = {
#include "stg26/room_scenery/west_shadow_2.inc"
};

static Sint16 west_shadow_1_polygons[353] = {
#include "stg26/room_scenery/west_shadow_1_polygons.inc"
};

static struct quarters_shadow_vertices west_shadow_1_vertices = {
#include "stg26/room_scenery/west_shadow_1_vertices.inc"
};

static NJS_CNK_MODEL west_shadow_1_model = {
#include "stg26/room_scenery/west_shadow_1_model.inc"
};

static NJS_CNK_OBJECT west_shadow_1 = {
#include "stg26/room_scenery/west_shadow_1.inc"
};

NJS_CNK_OBJECT quarters_west_modifiers = {
#include "stg26/room_scenery/quarters_west_modifiers.inc"
};

/* Pool water and texture animation. */
static NJS_TEXNAME archive_textures[2] = {
#include "stg26/room_scenery/archive_textures.inc"
};

static NJS_TEXLIST archive_texlist = {
#include "stg26/room_scenery/archive_texlist.inc"
};

static NJS_TEXNAME water_surface_textures[1] = {
#include "stg26/room_scenery/water_surface_textures.inc"
};

NJS_TEXLIST quarters_water_surface_texlist = {
#include "stg26/room_scenery/quarters_water_surface_texlist.inc"
};

static Sint16 water_polygons[25] = {
#include "stg26/room_scenery/water_polygons.inc"
};

static Sint32 water_vertices[27] = {
#include "stg26/room_scenery/water_vertices.inc"
};

NJS_CNK_MODEL quarters_water_model[1] = {
#include "stg26/room_scenery/quarters_water_model.inc"
};

NJS_CNK_OBJECT quarters_water_object = {
#include "stg26/room_scenery/quarters_water_object.inc"
};

static NJS_TEXNAME water_textures[14] = {
#include "stg26/room_scenery/water_textures.inc"
};

NJS_TEXLIST quarters_water_texlist = {
#include "stg26/room_scenery/quarters_water_texlist.inc"
};

static NJS_TEXLIST *background_textures[7] = {
#include "stg26/room_scenery/background_textures.inc"
};

quarters_texture_binding *quarters_background_groups[2] = {
#include "stg26/room_scenery/quarters_background_groups.inc"
};
struct texture_pvm quarters_background_pvms[3] = {
#include "stg26/room_scenery/quarters_background_pvms.inc"
};

/* Kill area. */
static NJS_MATERIAL quarters_kill_area_materials[1] = {
#include "stg26/room_scenery/quarters_kill_area_materials.inc"
};

static Sint16 quarters_kill_area_faces[5] = {
#include "stg26/room_scenery/quarters_kill_area_faces.inc"
};

static NJS_MESHSET quarters_kill_area_meshsets[1] = {
#include "stg26/room_scenery/quarters_kill_area_meshsets.inc"
};

static NJS_POINT3 quarters_kill_area_points[4] = {
#include "stg26/room_scenery/quarters_kill_area_points.inc"
};

static NJS_VECTOR quarters_kill_area_normals[4] = {
#include "stg26/room_scenery/quarters_kill_area_normals.inc"
};

static NJS_MODEL quarters_kill_area_model = {
#include "stg26/room_scenery/quarters_kill_area_model.inc"
};

NJS_OBJECT quarters_kill_area = {
#include "stg26/room_scenery/quarters_kill_area.inc"
};

#pragma section
