/* Pyramid Cave's background: its wall and lamps and their texture lists.
 * bgtex28, the PVM that fills the lists, and the stage's fog follow */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/fog.h"
#include "stg28/cave_scenery.h"
#include "system/texture.h"

static const char backdrop_material_1[12] = {
#include "stg28/cave_scenery/backdrop_material_1.inc"
};
static const char backdrop_material_2[12] = {
#include "stg28/cave_scenery/backdrop_material_2.inc"
};
static const char backdrop_material_3[12] = {
#include "stg28/cave_scenery/backdrop_material_3.inc"
};
static const char lamp_material[12] = {
#include "stg28/cave_scenery/lamp_material.inc"
};
static const char archive_name[8] = {
#include "stg28/cave_scenery/archive_name.inc"
};

#pragma section ATA
static NJS_TEXNAME pyramid_cave_wall_textures[3] = {
#include "stg28/cave_scenery/pyramid_cave_wall_textures.inc"
};

NJS_TEXLIST pyramid_cave_wall_texlist = {
#include "stg28/cave_scenery/pyramid_cave_wall_texlist.inc"
};

static Sint16 wall_polygons[543] = {
#include "stg28/cave_scenery/wall_polygons.inc"
};

static Sint32 wall_vertices[387] = {
#include "stg28/cave_scenery/wall_vertices.inc"
};

NJS_CNK_MODEL pyramid_cave_wall_model = {
#include "stg28/cave_scenery/pyramid_cave_wall_model.inc"
};

static NJS_CNK_OBJECT wall_object = {
#include "stg28/cave_scenery/wall_object.inc"
};
static NJS_TEXNAME pyramid_cave_lamp_textures[1] = {
#include "stg28/cave_scenery/pyramid_cave_lamp_textures.inc"
};

NJS_TEXLIST pyramid_cave_lamp_texlist = {
#include "stg28/cave_scenery/pyramid_cave_lamp_texlist.inc"
};

static Sint16 lamp_polygons[39] = {
#include "stg28/cave_scenery/lamp_polygons.inc"
};

static Sint32 lamp_vertices[35] = {
#include "stg28/cave_scenery/lamp_vertices.inc"
};

NJS_CNK_MODEL pyramid_cave_lamp_model[1] = {
#include "stg28/cave_scenery/pyramid_cave_lamp_model.inc"
};

static NJS_CNK_OBJECT lamp_object = {
#include "stg28/cave_scenery/lamp_object.inc"
};
static NJS_TEXNAME background_textures[4] = {
#include "stg28/cave_scenery/background_textures.inc"
};

static NJS_TEXLIST background_texlist = {
#include "stg28/cave_scenery/background_texlist.inc"
};

static NJS_TEXLIST *background_lists[3] = {
#include "stg28/cave_scenery/background_lists.inc"
};

NJS_TEXLIST **pyramid_cave_background_texture_groups[2] = {
#include "stg28/cave_scenery/pyramid_cave_background_texture_groups.inc"
};

struct texture_pvm pyramid_cave_background_pvms[2] = {
#include "stg28/cave_scenery/pyramid_cave_background_pvms.inc"
};
#pragma section

struct fog_setting pyramid_cave_fog;
