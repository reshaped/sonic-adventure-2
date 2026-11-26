#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"

#include "stg31/scenery_land.h"
#include "stg31/sky/mesh_types.h"
#include "stg31/sky.h"
#include "system/texture.h"

static const char greenhill128_5[16] = {
#include "stg31/sky/greenhill128_5.inc"
};
static const char greenhill256_1[16] = {
#include "stg31/sky/greenhill256_1.inc"
};
static const char greenhill64_2[16] = {
#include "stg31/sky/greenhill64_2.inc"
};
static const char greenhill8_1[16] = {
#include "stg31/sky/greenhill8_1.inc"
};
static const char greenhillwt128_0[20] = {
#include "stg31/sky/greenhillwt128_0.inc"
};
static const char greenhillwt128_1[20] = {
#include "stg31/sky/greenhillwt128_1.inc"
};
static const char greenhillwt128_2[20] = {
#include "stg31/sky/greenhillwt128_2.inc"
};
static const char greenhillwt128_3[20] = {
#include "stg31/sky/greenhillwt128_3.inc"
};
static const char greenhillwt128_4[20] = {
#include "stg31/sky/greenhillwt128_4.inc"
};

static const char archive[8] = {
#include "stg31/sky/archive.inc"
};

static NJS_TEXNAME background_names[9] = {
#include "stg31/sky/background_names.inc"
};

static NJS_TEXLIST background_textures = {
#include "stg31/sky/background_textures.inc"
};

static NJS_TEXNAME horizon_names[3] = {
#include "stg31/sky/horizon_names.inc"
};
NJS_TEXLIST green_hill_horizon_texlist = {
#include "stg31/sky/green_hill_horizon_texlist.inc"
};

static Sint16 horizon_polygons[1045] = {
#include "stg31/sky/horizon_polygons.inc"
};
static struct green_hill_horizon_vertex_list horizon_vertices = {
#include "stg31/sky/horizon_vertices.inc"
};
static NJS_CNK_MODEL horizon_model = {
#include "stg31/sky/horizon_model.inc"
};

NJS_CNK_OBJECT green_hill_horizon[1] = {
#include "stg31/sky/green_hill_horizon.inc"
};

static NJS_TEXNAME horizon_scroll_names[1] = {
#include "stg31/sky/horizon_scroll_names.inc"
};
NJS_TEXLIST green_hill_horizon_scroll_texlist = {
#include "stg31/sky/green_hill_horizon_scroll_texlist.inc"
};

static Sint16 horizon_scroll_polygons[183] = {
#include "stg31/sky/horizon_scroll_polygons.inc"
};
static Sint32 horizon_scroll_vertices[131] = {
#include "stg31/sky/horizon_scroll_vertices.inc"
};
static NJS_CNK_MODEL horizon_scroll_model = {
#include "stg31/sky/horizon_scroll_model.inc"
};

NJS_CNK_OBJECT green_hill_horizon_scroll[1] = {
#include "stg31/sky/green_hill_horizon_scroll.inc"
};

static Sint16 horizon_scroll_uvs[132] = {
#include "stg31/sky/horizon_scroll_uvs.inc"
};

struct texture_anim green_hill_horizon_scroll_uv = {
#include "stg31/sky/green_hill_horizon_scroll_uv.inc"
};

static NJS_TEXNAME sky_north_names[1] = {
#include "stg31/sky/sky_north_names.inc"
};
NJS_TEXLIST green_hill_sky_north_texlist = {
#include "stg31/sky/green_hill_sky_north_texlist.inc"
};

static Sint16 sky_north_polygons[133] = {
#include "stg31/sky/sky_north_polygons.inc"
};
static Sint32 sky_north_vertices[155] = {
#include "stg31/sky/sky_north_vertices.inc"
};
static NJS_CNK_MODEL sky_north_model = {
#include "stg31/sky/sky_north_model.inc"
};

NJS_CNK_OBJECT green_hill_sky_north[1] = {
#include "stg31/sky/green_hill_sky_north.inc"
};

static NJS_TEXNAME sky_east_names[1] = {
#include "stg31/sky/sky_east_names.inc"
};
NJS_TEXLIST green_hill_sky_east_texlist = {
#include "stg31/sky/green_hill_sky_east_texlist.inc"
};

static Sint16 sky_east_polygons[133] = {
#include "stg31/sky/sky_east_polygons.inc"
};
static Sint32 sky_east_vertices[155] = {
#include "stg31/sky/sky_east_vertices.inc"
};
static NJS_CNK_MODEL sky_east_model = {
#include "stg31/sky/sky_east_model.inc"
};

NJS_CNK_OBJECT green_hill_sky_east[1] = {
#include "stg31/sky/green_hill_sky_east.inc"
};

static NJS_TEXNAME sky_south_names[1] = {
#include "stg31/sky/sky_south_names.inc"
};
NJS_TEXLIST green_hill_sky_south_texlist = {
#include "stg31/sky/green_hill_sky_south_texlist.inc"
};

static Sint16 sky_south_polygons[133] = {
#include "stg31/sky/sky_south_polygons.inc"
};
static Sint32 sky_south_vertices[155] = {
#include "stg31/sky/sky_south_vertices.inc"
};
static NJS_CNK_MODEL sky_south_model = {
#include "stg31/sky/sky_south_model.inc"
};

NJS_CNK_OBJECT green_hill_sky_south[1] = {
#include "stg31/sky/green_hill_sky_south.inc"
};

static NJS_TEXNAME sky_west_names[1] = {
#include "stg31/sky/sky_west_names.inc"
};
NJS_TEXLIST green_hill_sky_west_texlist = {
#include "stg31/sky/green_hill_sky_west_texlist.inc"
};

static Sint16 sky_west_polygons[133] = {
#include "stg31/sky/sky_west_polygons.inc"
};
static Sint32 sky_west_vertices[155] = {
#include "stg31/sky/sky_west_vertices.inc"
};
static NJS_CNK_MODEL sky_west_model = {
#include "stg31/sky/sky_west_model.inc"
};

NJS_CNK_OBJECT green_hill_sky_west[1] = {
#include "stg31/sky/green_hill_sky_west.inc"
};

static NJS_TEXNAME lower_1_names[1] = {
#include "stg31/sky/lower_1_names.inc"
};
NJS_TEXLIST green_hill_lower_1_texlist = {
#include "stg31/sky/green_hill_lower_1_texlist.inc"
};

static Sint16 lower_1_polygons[85] = {
#include "stg31/sky/lower_1_polygons.inc"
};
static Sint32 lower_1_vertices[67] = {
#include "stg31/sky/lower_1_vertices.inc"
};
static NJS_CNK_MODEL lower_1_model = {
#include "stg31/sky/lower_1_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_1[1] = {
#include "stg31/sky/green_hill_lower_1.inc"
};

static NJS_TEXNAME lower_2_names[1] = {
#include "stg31/sky/lower_2_names.inc"
};
NJS_TEXLIST green_hill_lower_2_texlist = {
#include "stg31/sky/green_hill_lower_2_texlist.inc"
};

static Sint16 lower_2_polygons[85] = {
#include "stg31/sky/lower_2_polygons.inc"
};
static Sint32 lower_2_vertices[67] = {
#include "stg31/sky/lower_2_vertices.inc"
};
static NJS_CNK_MODEL lower_2_model = {
#include "stg31/sky/lower_2_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_2[1] = {
#include "stg31/sky/green_hill_lower_2.inc"
};

static NJS_TEXNAME lower_3_names[1] = {
#include "stg31/sky/lower_3_names.inc"
};
NJS_TEXLIST green_hill_lower_3_texlist = {
#include "stg31/sky/green_hill_lower_3_texlist.inc"
};

static Sint16 lower_3_polygons[85] = {
#include "stg31/sky/lower_3_polygons.inc"
};
static Sint32 lower_3_vertices[67] = {
#include "stg31/sky/lower_3_vertices.inc"
};
static NJS_CNK_MODEL lower_3_model = {
#include "stg31/sky/lower_3_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_3[1] = {
#include "stg31/sky/green_hill_lower_3.inc"
};

static NJS_TEXNAME lower_4_names[1] = {
#include "stg31/sky/lower_4_names.inc"
};
NJS_TEXLIST green_hill_lower_4_texlist = {
#include "stg31/sky/green_hill_lower_4_texlist.inc"
};

static Sint16 lower_4_polygons[85] = {
#include "stg31/sky/lower_4_polygons.inc"
};
static Sint32 lower_4_vertices[67] = {
#include "stg31/sky/lower_4_vertices.inc"
};
static NJS_CNK_MODEL lower_4_model = {
#include "stg31/sky/lower_4_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_4[1] = {
#include "stg31/sky/green_hill_lower_4.inc"
};

static NJS_TEXNAME lower_5_names[1] = {
#include "stg31/sky/lower_5_names.inc"
};
NJS_TEXLIST green_hill_lower_5_texlist = {
#include "stg31/sky/green_hill_lower_5_texlist.inc"
};

static Sint16 lower_5_polygons[85] = {
#include "stg31/sky/lower_5_polygons.inc"
};
static Sint32 lower_5_vertices[67] = {
#include "stg31/sky/lower_5_vertices.inc"
};
static NJS_CNK_MODEL lower_5_model = {
#include "stg31/sky/lower_5_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_5[1] = {
#include "stg31/sky/green_hill_lower_5.inc"
};

static NJS_TEXNAME lower_6_names[1] = {
#include "stg31/sky/lower_6_names.inc"
};
NJS_TEXLIST green_hill_lower_6_texlist = {
#include "stg31/sky/green_hill_lower_6_texlist.inc"
};

static Sint16 lower_6_polygons[85] = {
#include "stg31/sky/lower_6_polygons.inc"
};
static Sint32 lower_6_vertices[67] = {
#include "stg31/sky/lower_6_vertices.inc"
};
static NJS_CNK_MODEL lower_6_model = {
#include "stg31/sky/lower_6_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_6[1] = {
#include "stg31/sky/green_hill_lower_6.inc"
};

static NJS_TEXNAME lower_7_names[1] = {
#include "stg31/sky/lower_7_names.inc"
};
NJS_TEXLIST green_hill_lower_7_texlist = {
#include "stg31/sky/green_hill_lower_7_texlist.inc"
};

static Sint16 lower_7_polygons[85] = {
#include "stg31/sky/lower_7_polygons.inc"
};
static Sint32 lower_7_vertices[67] = {
#include "stg31/sky/lower_7_vertices.inc"
};
static NJS_CNK_MODEL lower_7_model = {
#include "stg31/sky/lower_7_model.inc"
};

NJS_CNK_OBJECT green_hill_lower_7[1] = {
#include "stg31/sky/green_hill_lower_7.inc"
};

static NJS_TEXLIST *sky_lists[16] = {
#include "stg31/sky/sky_lists.inc"
};

NJS_TEXLIST **green_hill_background_groups[2] = {
#include "stg31/sky/green_hill_background_groups.inc"
};

struct texture_pvm green_hill_background_pvms[2] = {
#include "stg31/sky/green_hill_background_pvms.inc"
};
