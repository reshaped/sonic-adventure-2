/* the sky's models, each with its texture list: two pyramids, two star
 * fields and the moon. bgtex21 is the PVM that fills the lists. the background
 * task draws the models. no code reads the objects that hold them */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg21/sky/text.inc"

#include "stg21/background_art.h"

#include "system/texture.h"

struct path *sand_ocean_paths[] = {NULL};

static NJS_TEXNAME ocean_first_pyramid_texlist_names[1] = {
#include "stg21/sky/ocean_first_pyramid_texlist_names.inc"
};

NJS_TEXLIST ocean_first_pyramid_texlist = {
#include "stg21/sky/ocean_first_pyramid_texlist.inc"
};

static Sint16 ocean_first_pyramid_polygons[25] = {
#include "stg21/sky/ocean_first_pyramid_polygons.inc"
};

static Sint32 ocean_first_pyramid_vertices[19] = {
#include "stg21/sky/ocean_first_pyramid_vertices.inc"
};

NJS_CNK_MODEL ocean_first_pyramid = {
#include "stg21/sky/ocean_first_pyramid.inc"
};

NJS_CNK_OBJECT ocean_first_pyramid_object = {
#include "stg21/sky/ocean_first_pyramid_object.inc"
};

static NJS_TEXNAME ocean_second_pyramid_texlist_names[1] = {
#include "stg21/sky/ocean_second_pyramid_texlist_names.inc"
};

NJS_TEXLIST ocean_second_pyramid_texlist = {
#include "stg21/sky/ocean_second_pyramid_texlist.inc"
};

static Sint16 ocean_second_pyramid_polygons[35] = {
#include "stg21/sky/ocean_second_pyramid_polygons.inc"
};

static Sint32 ocean_second_pyramid_vertices[31] = {
#include "stg21/sky/ocean_second_pyramid_vertices.inc"
};

NJS_CNK_MODEL ocean_second_pyramid = {
#include "stg21/sky/ocean_second_pyramid.inc"
};

NJS_CNK_OBJECT ocean_second_pyramid_object = {
#include "stg21/sky/ocean_second_pyramid_object.inc"
};

static NJS_TEXNAME ocean_first_stars_texlist_names[1] = {
#include "stg21/sky/ocean_first_stars_texlist_names.inc"
};

NJS_TEXLIST ocean_first_stars_texlist = {
#include "stg21/sky/ocean_first_stars_texlist.inc"
};

static Sint16 ocean_first_stars_polygons[317] = {
#include "stg21/sky/ocean_first_stars_polygons.inc"
};

static Sint32 ocean_first_stars_vertices[195] = {
#include "stg21/sky/ocean_first_stars_vertices.inc"
};

NJS_CNK_MODEL ocean_first_stars = {
#include "stg21/sky/ocean_first_stars.inc"
};

NJS_CNK_OBJECT ocean_first_stars_object = {
#include "stg21/sky/ocean_first_stars_object.inc"
};

static NJS_TEXNAME ocean_second_stars_texlist_names[1] = {
#include "stg21/sky/ocean_second_stars_texlist_names.inc"
};

NJS_TEXLIST ocean_second_stars_texlist = {
#include "stg21/sky/ocean_second_stars_texlist.inc"
};

static Sint16 ocean_second_stars_polygons[115] = {
#include "stg21/sky/ocean_second_stars_polygons.inc"
};

static Sint32 ocean_second_stars_vertices[131] = {
#include "stg21/sky/ocean_second_stars_vertices.inc"
};

NJS_CNK_MODEL ocean_second_stars = {
#include "stg21/sky/ocean_second_stars.inc"
};

NJS_CNK_OBJECT ocean_second_stars_object = {
#include "stg21/sky/ocean_second_stars_object.inc"
};

static NJS_TEXNAME ocean_moon_texlist_names[1] = {
#include "stg21/sky/ocean_moon_texlist_names.inc"
};

NJS_TEXLIST ocean_moon_texlist = {
#include "stg21/sky/ocean_moon_texlist.inc"
};

static Sint16 ocean_moon_polygons[25] = {
#include "stg21/sky/ocean_moon_polygons.inc"
};

static Sint32 ocean_moon_vertices[19] = {
#include "stg21/sky/ocean_moon_vertices.inc"
};

NJS_CNK_MODEL ocean_moon = {
#include "stg21/sky/ocean_moon.inc"
};

NJS_CNK_OBJECT ocean_moon_object = {
#include "stg21/sky/ocean_moon_object.inc"
};

static NJS_TEXNAME background_textures_names[5] = {
#include "stg21/sky/background_textures_names.inc"
};

static NJS_TEXLIST background_textures = {
#include "stg21/sky/background_textures.inc"
};

typedef NJS_TEXLIST *ocean_texture_list;

static NJS_TEXLIST *sky_materials[6] = {
#include "stg21/sky/sky_materials.inc"
};

ocean_texture_list *ocean_background_groups[2] = {
#include "stg21/sky/ocean_background_groups.inc"
};

struct texture_pvm ocean_background_pvms[2] = {
#include "stg21/sky/ocean_background_pvms.inc"
};
