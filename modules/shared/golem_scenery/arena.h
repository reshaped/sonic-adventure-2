/* The arena's illuminated band, scrolling sand and quicksand floor.
 * Each fight owns the writable meshes and texture lists used by its stage. */
#ifndef SHARED_GOLEM_SCENERY_ARENA_H
#define SHARED_GOLEM_SCENERY_ARENA_H

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/light.h"
#include "draw/texture_anim.h"
#include "player/player.h"
#include "system/texture.h"

static NJS_TEXNAME pulse_textures[1] = {
#include "shared/golem_scenery/arena/pulse_textures.inc"
};
static NJS_TEXLIST pulse_texlist = {
#include "shared/golem_scenery/arena/pulse_texlist.inc"
};

static Sint16 pulse_polygons[137] = {
#include "shared/golem_scenery/arena/pulse_polygons.inc"
};

static Sint32 pulse_vertices[163] = {
#include "shared/golem_scenery/arena/pulse_vertices.inc"
};

static NJS_CNK_MODEL pulse_model = {
#include "shared/golem_scenery/arena/pulse_model.inc"
};

static NJS_CNK_OBJECT pulse_object = {
#include "shared/golem_scenery/arena/pulse_object.inc"
};

/* One set of texture coordinates for each of the sand model's eight strips. */
static Sint16 sand_uvs[106] = {
#include "shared/golem_scenery/arena/sand_uvs.inc"
};
static struct texture_anim sand_scroll = {
#include "shared/golem_scenery/arena/sand_scroll.inc"
};

static NJS_TEXNAME sand_textures[1] = {
#include "shared/golem_scenery/arena/sand_textures.inc"
};
static NJS_TEXLIST sand_texlist = {
#include "shared/golem_scenery/arena/sand_texlist.inc"
};

static Sint16 sand_polygons[153] = {
#include "shared/golem_scenery/arena/sand_polygons.inc"
};

static Sint32 sand_vertices[103] = {
#include "shared/golem_scenery/arena/sand_vertices.inc"
};

static NJS_CNK_MODEL sand_model = {
#include "shared/golem_scenery/arena/sand_model.inc"
};

static NJS_CNK_OBJECT sand_object[1] = {
#include "shared/golem_scenery/arena/sand_object.inc"
};

/* texture_load_shared populates both model lists from this archive. */
static NJS_TEXNAME background_pvm_texnames[2] = {
#include "shared/golem_scenery/arena/background_pvm_texnames.inc"
};
static NJS_TEXLIST background_pvm_texlist = {
#include "shared/golem_scenery/arena/background_pvm_texlist.inc"
};
static NJS_TEXLIST *background_group[3] = {
#include "shared/golem_scenery/arena/background_group.inc"
};
static NJS_TEXLIST **background_texture_groups[2] = {
#include "shared/golem_scenery/arena/background_texture_groups.inc"
};
static struct texture_pvm background_pvms[2] = {
#include "shared/golem_scenery/arena/background_pvms.inc"
};

/* The quicksand sinks every character that enters the square floor. */
/* SHC requires scalar initializers for the material colors. */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL quicksand_material = {
#include "shared/golem_scenery/arena/quicksand_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
static Sint16 quicksand_meshes[5] = {
#include "shared/golem_scenery/arena/quicksand_meshes.inc"
};
static NJS_MESHSET quicksand_meshset = {
#include "shared/golem_scenery/arena/quicksand_meshset.inc"
};
static NJS_POINT3 quicksand_points[4] = {
#include "shared/golem_scenery/arena/quicksand_points.inc"
};
static NJS_VECTOR quicksand_normals[4] = {
#include "shared/golem_scenery/arena/quicksand_normals.inc"
};
static NJS_MODEL quicksand_model = {
#include "shared/golem_scenery/arena/quicksand_model.inc"
};
static NJS_OBJECT quicksand_object = {
#include "shared/golem_scenery/arena/quicksand_object.inc"
};
static struct kill_area golem_kill_areas[2] = {
#include "shared/golem_scenery/arena/golem_kill_areas.inc"
};
static struct light golem_lights[2] = {
#include "shared/golem_scenery/arena/golem_lights.inc"
};

#endif
