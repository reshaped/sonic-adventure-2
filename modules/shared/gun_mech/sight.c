/* the sight that mech_boss_sight.c draws over its target.
 * mech_boss_sight_motion moves its four sectors, and
 * mech_boss_sight_ring_motion the ring of two parts below them */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

enum effect_texture { TEX_LOCKON01 = 0, TEX_LOCKON02 = 1, TEX_LOCKON03 = 2 };

static Sint16 sector_4_polygons[205] = {
#include "shared/gun_mech/sight/sector_4_polygons.inc"
};

static Sint32 sector_4_vertices[363] = {
#include "shared/gun_mech/sight/sector_4_vertices.inc"
};

static NJS_CNK_MODEL sector_4_mesh = {
#include "shared/gun_mech/sight/sector_4_mesh.inc"
};

static NJS_CNK_OBJECT sector_4 = {
#include "shared/gun_mech/sight/sector_4.inc"
};

static Sint16 sector_3_polygons[25] = {
#include "shared/gun_mech/sight/sector_3_polygons.inc"
};

static Sint32 sector_3_vertices[27] = {
#include "shared/gun_mech/sight/sector_3_vertices.inc"
};

static NJS_CNK_MODEL sector_3_mesh = {
#include "shared/gun_mech/sight/sector_3_mesh.inc"
};

static NJS_CNK_OBJECT sector_3 = {
#include "shared/gun_mech/sight/sector_3.inc"
};

static Sint16 sector_2_polygons[25] = {
#include "shared/gun_mech/sight/sector_2_polygons.inc"
};

static Sint32 sector_2_vertices[27] = {
#include "shared/gun_mech/sight/sector_2_vertices.inc"
};

static NJS_CNK_MODEL sector_2_mesh = {
#include "shared/gun_mech/sight/sector_2_mesh.inc"
};

static NJS_CNK_OBJECT sector_2 = {
#include "shared/gun_mech/sight/sector_2.inc"
};

static Sint16 sector_1_polygons[25] = {
#include "shared/gun_mech/sight/sector_1_polygons.inc"
};

static Sint32 sector_1_vertices[27] = {
#include "shared/gun_mech/sight/sector_1_vertices.inc"
};

static NJS_CNK_MODEL sector_1_mesh = {
#include "shared/gun_mech/sight/sector_1_mesh.inc"
};

static NJS_CNK_OBJECT sector_1 = {
#include "shared/gun_mech/sight/sector_1.inc"
};

NJS_CNK_OBJECT mech_boss_sight_object = {
#include "shared/gun_mech/sight/mech_boss_sight_object.inc"
};

static Sint16 ring_inner_polygons[25] = {
#include "shared/gun_mech/sight/ring_inner_polygons.inc"
};

static Sint32 ring_inner_vertices[27] = {
#include "shared/gun_mech/sight/ring_inner_vertices.inc"
};

static NJS_CNK_MODEL ring_inner_mesh = {
#include "shared/gun_mech/sight/ring_inner_mesh.inc"
};

static NJS_CNK_OBJECT ring_inner = {
#include "shared/gun_mech/sight/ring_inner.inc"
};

static Sint16 sight_ring_polygons[25] = {
#include "shared/gun_mech/sight/sight_ring_polygons.inc"
};

static Sint32 sight_ring_vertices[27] = {
#include "shared/gun_mech/sight/sight_ring_vertices.inc"
};

static NJS_CNK_MODEL sight_ring_mesh = {
#include "shared/gun_mech/sight/sight_ring_mesh.inc"
};

NJS_CNK_OBJECT mech_boss_sight_ring_object = {
#include "shared/gun_mech/sight/mech_boss_sight_ring_object.inc"
};
