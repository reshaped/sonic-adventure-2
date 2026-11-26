/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last2/super_effects.h"
#include "boss_last2/texture_names.h"
#include "draw/texture_anim.h"

const char finalhazard_super_effects_filenames[146] = {
#include "boss_last2/super_effects/finalhazard_super_effects_filenames.inc"
};

/* BOSS_LAST2.PRS's texture lists, chunk models and UV scrolls, from the
 * disc. its D lies between stage.c and super.c */
static NJS_TEXNAME effect_texture_names[15] = {
#include "boss_last2/super_effects/effect_texture_names.inc"
};
NJS_TEXLIST super_effect_textures = {
#include "boss_last2/super_effects/super_effect_textures.inc"
};
struct super_flash_polygons super_flash_polygons = {
#include "boss_last2/super_effects/super_flash_polygons.inc"
};
static Sint32 flash_vertices[297] = {
#include "boss_last2/super_effects/flash_vertices.inc"
};
static NJS_CNK_MODEL flash_model = {
#include "boss_last2/super_effects/flash_model.inc"
};
NJS_CNK_OBJECT super_flash = {
#include "boss_last2/super_effects/super_flash.inc"
};
static Sint16 glow_polygons[361] = {
#include "boss_last2/super_effects/glow_polygons.inc"
};
struct super_glow_vertices super_glow_vertices = {
#include "boss_last2/super_effects/super_glow_vertices.inc"
};
NJS_CNK_MODEL super_glow_model = {
#include "boss_last2/super_effects/super_glow_model.inc"
};
NJS_CNK_OBJECT super_glow = {
#include "boss_last2/super_effects/super_glow.inc"
};
static Sint16 glow_scroll[250] = {
#include "boss_last2/super_effects/glow_scroll.inc"
};
struct texture_anim super_glow_animation = {
#include "boss_last2/super_effects/super_glow_animation.inc"
};
static Sint16 marker_polygons[15] = {
#include "boss_last2/super_effects/marker_polygons.inc"
};
static Sint32 marker_vertices[27] = {
#include "boss_last2/super_effects/marker_vertices.inc"
};
static NJS_CNK_MODEL marker_model = {
#include "boss_last2/super_effects/marker_model.inc"
};
static NJS_CNK_OBJECT unused_marker = {
#include "boss_last2/super_effects/unused_marker.inc"
};
static Sint16 sonic_trail_polygons[321] = {
#include "boss_last2/super_effects/sonic_trail_polygons.inc"
};
static Sint32 sonic_trail_vertices[259] = {
#include "boss_last2/super_effects/sonic_trail_vertices.inc"
};
static NJS_CNK_MODEL sonic_trail_model = {
#include "boss_last2/super_effects/sonic_trail_model.inc"
};
NJS_CNK_OBJECT super_sonic_trail = {
#include "boss_last2/super_effects/super_sonic_trail.inc"
};
static Sint16 sonic_trail_scroll[212] = {
#include "boss_last2/super_effects/sonic_trail_scroll.inc"
};
struct texture_anim super_sonic_trail_animation = {
#include "boss_last2/super_effects/super_sonic_trail_animation.inc"
};
static Sint16 shadow_trail_polygons[321] = {
#include "boss_last2/super_effects/shadow_trail_polygons.inc"
};
static Sint32 shadow_trail_vertices[259] = {
#include "boss_last2/super_effects/shadow_trail_vertices.inc"
};
static NJS_CNK_MODEL shadow_trail_model = {
#include "boss_last2/super_effects/shadow_trail_model.inc"
};
NJS_CNK_OBJECT super_shadow_trail = {
#include "boss_last2/super_effects/super_shadow_trail.inc"
};
static Sint16 shadow_trail_scroll[212] = {
#include "boss_last2/super_effects/shadow_trail_scroll.inc"
};
struct texture_anim super_shadow_trail_animation = {
#include "boss_last2/super_effects/super_shadow_trail_animation.inc"
};
struct super_glow_vertices super_glow_source = {
#include "boss_last2/super_effects/super_glow_source.inc"
};
