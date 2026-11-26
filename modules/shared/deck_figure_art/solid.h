/* the enemy figure's solid, a Ninja basic model that players can stand on. the
 * figure's kind takes it for its collision */
#ifndef SHARED_DECK_FIGURE_ART_DECK_SOLID_H
#define SHARED_DECK_FIGURE_ART_DECK_SOLID_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME deck_solid_textures[3] = {
#include "shared/deck_figure_art/solid/deck_solid_textures.inc"
};

/* the solid's texture list, which no code reads */
static NJS_TEXLIST deck_solid_texlist = {
#include "shared/deck_figure_art/solid/deck_solid_texlist.inc"
};

/* each material's colors are written as packed words, without the braces of
 * their unions */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL deck_solid_materials[4] = {
#include "shared/deck_figure_art/solid/deck_solid_materials.inc"
};

static Sint16 deck_solid_fuselage_indices[52] = {
#include "shared/deck_figure_art/solid/deck_solid_fuselage_indices.inc"
};

static Sint16 deck_solid_nose_indices[17] = {
#include "shared/deck_figure_art/solid/deck_solid_nose_indices.inc"
};

static Sint16 deck_solid_hull_indices[68] = {
#include "shared/deck_figure_art/solid/deck_solid_hull_indices.inc"
};

static Sint16 deck_solid_wings_indices[18] = {
#include "shared/deck_figure_art/solid/deck_solid_wings_indices.inc"
};

static NJS_COLOR deck_solid_fuselage_uvs[42] = {
#include "shared/deck_figure_art/solid/deck_solid_fuselage_uvs.inc"
};

static NJS_COLOR deck_solid_nose_uvs[14] = {
#include "shared/deck_figure_art/solid/deck_solid_nose_uvs.inc"
};

static NJS_COLOR deck_solid_hull_uvs[53] = {
#include "shared/deck_figure_art/solid/deck_solid_hull_uvs.inc"
};

static NJS_COLOR deck_solid_wings_uvs[14] = {
#include "shared/deck_figure_art/solid/deck_solid_wings_uvs.inc"
};

/* NOLINTEND(clang-diagnostic-missing-braces) */

static NJS_MESHSET deck_solid_meshsets[4] = {
#include "shared/deck_figure_art/solid/deck_solid_meshsets.inc"
};

static NJS_POINT3 deck_solid_points[75] = {
#include "shared/deck_figure_art/solid/deck_solid_points.inc"
};

static NJS_VECTOR deck_solid_normals[75] = {
#include "shared/deck_figure_art/solid/deck_solid_normals.inc"
};

static NJS_MODEL deck_solid_model = {
#include "shared/deck_figure_art/solid/deck_solid_model.inc"
};

static NJS_OBJECT deck_enemy_solid = {
#include "shared/deck_figure_art/solid/deck_enemy_solid.inc"
};

#endif
