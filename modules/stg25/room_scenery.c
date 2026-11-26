/* Death Chamber's rooms: the center and its moving parts, the halls and
 * walls, and the three pools. the background's texture lists and the kill
 * area follow */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/fog.h"
#include "stg25/death_chamber.h"
#include "stg25/room_scenery.h"
#include "system/texture.h"

typedef NJS_TEXLIST *chamber_texture_binding;

static const char column_material[16] = {
#include "stg25/room_scenery/column_material.inc"
};
static const char mechanism_material[16] = {
#include "stg25/room_scenery/mechanism_material.inc"
};
static const char water_frame_01[16] = {
#include "stg25/room_scenery/water_frame_01.inc"
};
static const char water_frame_02[16] = {
#include "stg25/room_scenery/water_frame_02.inc"
};
static const char water_frame_03[16] = {
#include "stg25/room_scenery/water_frame_03.inc"
};
static const char water_frame_04[16] = {
#include "stg25/room_scenery/water_frame_04.inc"
};
static const char water_frame_05[16] = {
#include "stg25/room_scenery/water_frame_05.inc"
};
static const char water_frame_06[16] = {
#include "stg25/room_scenery/water_frame_06.inc"
};
static const char water_frame_07[16] = {
#include "stg25/room_scenery/water_frame_07.inc"
};
static const char water_frame_08[16] = {
#include "stg25/room_scenery/water_frame_08.inc"
};
static const char water_frame_09[16] = {
#include "stg25/room_scenery/water_frame_09.inc"
};
static const char water_frame_10[16] = {
#include "stg25/room_scenery/water_frame_10.inc"
};
static const char water_frame_11[16] = {
#include "stg25/room_scenery/water_frame_11.inc"
};
static const char water_frame_12[16] = {
#include "stg25/room_scenery/water_frame_12.inc"
};
static const char water_frame_13[16] = {
#include "stg25/room_scenery/water_frame_13.inc"
};
static const char water_frame_14[16] = {
#include "stg25/room_scenery/water_frame_14.inc"
};
static const char background_archive[8] = {
#include "stg25/room_scenery/background_archive.inc"
};

#pragma section ATA
static NJS_TEXNAME center_top_textures[1] = {
#include "stg25/room_scenery/center_top_textures.inc"
};

NJS_TEXLIST death_chamber_center_texlist = {
#include "stg25/room_scenery/death_chamber_center_texlist.inc"
};

static Sint16 center_top_polygons[123] = {
#include "stg25/room_scenery/center_top_polygons.inc"
};

static Sint32 center_top_vertices[131] = {
#include "stg25/room_scenery/center_top_vertices.inc"
};

NJS_CNK_MODEL death_chamber_center_top_model = {
#include "stg25/room_scenery/death_chamber_center_top_model.inc"
};

static NJS_CNK_OBJECT center_top_object = {
#include "stg25/room_scenery/center_top_object.inc"
};

static NJS_TEXNAME center_middle_textures[1] = {
#include "stg25/room_scenery/center_middle_textures.inc"
};

static NJS_TEXLIST center_middle_texlist = {
#include "stg25/room_scenery/center_middle_texlist.inc"
};

static Sint16 center_middle_polygons[151] = {
#include "stg25/room_scenery/center_middle_polygons.inc"
};

static Sint32 center_middle_vertices[131] = {
#include "stg25/room_scenery/center_middle_vertices.inc"
};

NJS_CNK_MODEL death_chamber_center_middle_model = {
#include "stg25/room_scenery/death_chamber_center_middle_model.inc"
};

static NJS_CNK_OBJECT center_middle_object = {
#include "stg25/room_scenery/center_middle_object.inc"
};

static NJS_TEXNAME center_base_textures[1] = {
#include "stg25/room_scenery/center_base_textures.inc"
};

static NJS_TEXLIST center_base_texlist = {
#include "stg25/room_scenery/center_base_texlist.inc"
};

static Sint16 center_base_polygons[139] = {
#include "stg25/room_scenery/center_base_polygons.inc"
};

static Sint32 center_base_vertices[163] = {
#include "stg25/room_scenery/center_base_vertices.inc"
};

NJS_CNK_MODEL death_chamber_center_base_model = {
#include "stg25/room_scenery/death_chamber_center_base_model.inc"
};

static NJS_CNK_OBJECT center_base_object = {
#include "stg25/room_scenery/center_base_object.inc"
};
static NJS_TEXNAME central_hall_textures[1] = {
#include "stg25/room_scenery/central_hall_textures.inc"
};

NJS_TEXLIST death_chamber_hall_texlist = {
#include "stg25/room_scenery/death_chamber_hall_texlist.inc"
};

static Sint16 hall_polygons[51] = {
#include "stg25/room_scenery/hall_polygons.inc"
};

static Sint32 hall_vertices[35] = {
#include "stg25/room_scenery/hall_vertices.inc"
};

NJS_CNK_MODEL death_chamber_hall_model = {
#include "stg25/room_scenery/death_chamber_hall_model.inc"
};

static NJS_CNK_OBJECT hall_object = {
#include "stg25/room_scenery/hall_object.inc"
};

static NJS_TEXNAME side_wall_textures[1] = {
#include "stg25/room_scenery/side_wall_textures.inc"
};

NJS_TEXLIST death_chamber_wall_texlist = {
#include "stg25/room_scenery/death_chamber_wall_texlist.inc"
};

static Sint16 wall_polygons[129] = {
#include "stg25/room_scenery/wall_polygons.inc"
};

static Sint32 wall_vertices[131] = {
#include "stg25/room_scenery/wall_vertices.inc"
};

NJS_CNK_MODEL death_chamber_wall_model = {
#include "stg25/room_scenery/death_chamber_wall_model.inc"
};

static NJS_CNK_OBJECT wall_object = {
#include "stg25/room_scenery/wall_object.inc"
};

static NJS_TEXNAME north_wall_textures[1] = {
#include "stg25/room_scenery/north_wall_textures.inc"
};

NJS_TEXLIST death_chamber_north_texlist = {
#include "stg25/room_scenery/death_chamber_north_texlist.inc"
};

static Sint16 north_polygons[51] = {
#include "stg25/room_scenery/north_polygons.inc"
};

static Sint32 north_vertices[35] = {
#include "stg25/room_scenery/north_vertices.inc"
};

NJS_CNK_MODEL death_chamber_north_model = {
#include "stg25/room_scenery/death_chamber_north_model.inc"
};

static NJS_CNK_OBJECT north_object = {
#include "stg25/room_scenery/north_object.inc"
};

static NJS_TEXNAME east_north_wall_textures[1] = {
#include "stg25/room_scenery/east_north_wall_textures.inc"
};

NJS_TEXLIST death_chamber_east_north_texlist = {
#include "stg25/room_scenery/death_chamber_east_north_texlist.inc"
};

static Sint16 east_north_polygons[43] = {
#include "stg25/room_scenery/east_north_polygons.inc"
};

static Sint32 east_north_vertices[35] = {
#include "stg25/room_scenery/east_north_vertices.inc"
};

NJS_CNK_MODEL death_chamber_east_north_model = {
#include "stg25/room_scenery/death_chamber_east_north_model.inc"
};

static NJS_CNK_OBJECT east_north_object = {
#include "stg25/room_scenery/east_north_object.inc"
};

static NJS_TEXNAME east_south_wall_textures[1] = {
#include "stg25/room_scenery/east_south_wall_textures.inc"
};

NJS_TEXLIST death_chamber_east_south_texlist = {
#include "stg25/room_scenery/death_chamber_east_south_texlist.inc"
};

static Sint16 east_south_polygons[43] = {
#include "stg25/room_scenery/east_south_polygons.inc"
};

static Sint32 east_south_vertices[35] = {
#include "stg25/room_scenery/east_south_vertices.inc"
};

NJS_CNK_MODEL death_chamber_east_south_model = {
#include "stg25/room_scenery/death_chamber_east_south_model.inc"
};

static NJS_CNK_OBJECT east_south_object = {
#include "stg25/room_scenery/east_south_object.inc"
};
static NJS_TEXNAME center_mechanism_textures[1] = {
#include "stg25/room_scenery/center_mechanism_textures.inc"
};

NJS_TEXLIST death_chamber_center_object_texlist = {
#include "stg25/room_scenery/death_chamber_center_object_texlist.inc"
};

static Sint16 center_polygons[2515] = {
#include "stg25/room_scenery/center_polygons.inc"
};

static Sint32 center_vertices[1283] = {
#include "stg25/room_scenery/center_vertices.inc"
};

static NJS_CNK_MODEL center_model = {
#include "stg25/room_scenery/center_model.inc"
};

NJS_CNK_OBJECT death_chamber_center_object = {
#include "stg25/room_scenery/death_chamber_center_object.inc"
};

static NJS_MKEY_F mechanism_position_keys[57] = {
#include "stg25/room_scenery/mechanism_position_keys.inc"
};

static NJS_MDATA2 mechanism_tracks[1] = {
#include "stg25/room_scenery/mechanism_tracks.inc"
};

NJS_MOTION death_chamber_center_motion[1] = {
#include "stg25/room_scenery/death_chamber_center_motion.inc"
};
static NJS_TEXNAME rotor_textures[1] = {
#include "stg25/room_scenery/rotor_textures.inc"
};

static NJS_TEXLIST rotor_texlist = {
#include "stg25/room_scenery/rotor_texlist.inc"
};

static Sint16 spinner_polygons[577] = {
#include "stg25/room_scenery/spinner_polygons.inc"
};

static Sint32 spinner_vertices[515] = {
#include "stg25/room_scenery/spinner_vertices.inc"
};

NJS_CNK_MODEL death_chamber_spinner_model = {
#include "stg25/room_scenery/death_chamber_spinner_model.inc"
};

static NJS_CNK_OBJECT chamber_room_spinner_object = {
#include "stg25/room_scenery/chamber_room_spinner_object.inc"
};
static NJS_TEXNAME background_archive_textures[1] = {
#include "stg25/room_scenery/background_archive_textures.inc"
};

static NJS_TEXLIST background_archive_texlist = {
#include "stg25/room_scenery/background_archive_texlist.inc"
};

static NJS_TEXNAME center_surface_textures[1] = {
#include "stg25/room_scenery/center_surface_textures.inc"
};

static NJS_TEXLIST center_surface_texlist = {
#include "stg25/room_scenery/center_surface_texlist.inc"
};

static Sint16 center_water_polygons[25] = {
#include "stg25/room_scenery/center_water_polygons.inc"
};

static Sint32 center_water_vertices[27] = {
#include "stg25/room_scenery/center_water_vertices.inc"
};

NJS_CNK_MODEL death_chamber_center_water[1] = {
#include "stg25/room_scenery/death_chamber_center_water.inc"
};

static NJS_CNK_OBJECT center_water_object = {
#include "stg25/room_scenery/center_water_object.inc"
};

static NJS_TEXNAME west_surface_textures[1] = {
#include "stg25/room_scenery/west_surface_textures.inc"
};

static NJS_TEXLIST west_surface_texlist = {
#include "stg25/room_scenery/west_surface_texlist.inc"
};

static Sint16 south_west_water_polygons[25] = {
#include "stg25/room_scenery/south_west_water_polygons.inc"
};

static Sint32 south_west_water_vertices[27] = {
#include "stg25/room_scenery/south_west_water_vertices.inc"
};

NJS_CNK_MODEL death_chamber_south_west_water[1] = {
#include "stg25/room_scenery/death_chamber_south_west_water.inc"
};

static NJS_CNK_OBJECT south_west_water_object = {
#include "stg25/room_scenery/south_west_water_object.inc"
};

static NJS_TEXNAME east_surface_textures[1] = {
#include "stg25/room_scenery/east_surface_textures.inc"
};

static NJS_TEXLIST east_surface_texlist = {
#include "stg25/room_scenery/east_surface_texlist.inc"
};

static Sint16 south_east_water_polygons[25] = {
#include "stg25/room_scenery/south_east_water_polygons.inc"
};

static Sint32 south_east_water_vertices[27] = {
#include "stg25/room_scenery/south_east_water_vertices.inc"
};

NJS_CNK_MODEL death_chamber_south_east_water[1] = {
#include "stg25/room_scenery/death_chamber_south_east_water.inc"
};

static NJS_CNK_OBJECT south_east_water_object = {
#include "stg25/room_scenery/south_east_water_object.inc"
};

static NJS_TEXNAME water_frames_textures[14] = {
#include "stg25/room_scenery/water_frames_textures.inc"
};

NJS_TEXLIST death_chamber_water_texlist = {
#include "stg25/room_scenery/death_chamber_water_texlist.inc"
};
/* bgtex25, the PVM that texture_load_shared loads for the background, and
 * the background's texture lists. each list takes its textures from the PVM
 * by name */
static NJS_TEXLIST *background_texlists[11] = {
#include "stg25/room_scenery/background_texlists.inc"
};

chamber_texture_binding *death_chamber_background_texture_groups[2] = {
#include "stg25/room_scenery/death_chamber_background_texture_groups.inc"
};

struct texture_pvm death_chamber_background_pvms[3] = {
#include "stg25/room_scenery/death_chamber_background_pvms.inc"
};
/* the object of Death Chamber's one kill area */
static NJS_MATERIAL death_chamber_kill_area_materials[1] = {
#include "stg25/room_scenery/death_chamber_kill_area_materials.inc"
};

static Sint16 death_chamber_kill_area_faces[5] = {
#include "stg25/room_scenery/death_chamber_kill_area_faces.inc"
};

static NJS_MESHSET death_chamber_kill_area_meshsets[1] = {
#include "stg25/room_scenery/death_chamber_kill_area_meshsets.inc"
};

static NJS_POINT3 death_chamber_kill_area_points[4] = {
#include "stg25/room_scenery/death_chamber_kill_area_points.inc"
};

static NJS_VECTOR death_chamber_kill_area_normals[4] = {
#include "stg25/room_scenery/death_chamber_kill_area_normals.inc"
};

static NJS_MODEL death_chamber_kill_area_model = {
#include "stg25/room_scenery/death_chamber_kill_area_model.inc"
};

NJS_OBJECT death_chamber_kill_area = {
#include "stg25/room_scenery/death_chamber_kill_area.inc"
};
#pragma section

/* the fogs the stage reads from stg25_fog.bin and stg25_fogB.bin. the
 * background sets the first while the camera is below the water. it sets
 * the second while the camera is above */
struct fog_setting death_chamber_under_water_fog;
struct fog_setting death_chamber_above_water_fog;
