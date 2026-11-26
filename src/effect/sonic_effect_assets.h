/* s_efftex_texlist takes S_EFFTEX's or AMY_EFFTEX's textures, and
 * sh_efftex_texlist SH_EFFTEX's. flame_ring_draw animates and draws the flame
 * ring */
#ifndef EFFECT_SONIC_EFFECT_ASSETS_H
#define EFFECT_SONIC_EFFECT_ASSETS_H
#include <ninja.h>

#include "draw/texture_anim.h"

#define SPEED_EFFECT_TEXTURES 39
extern NJS_TEXNAME s_efftex_texname[SPEED_EFFECT_TEXTURES];
extern NJS_TEXLIST s_efftex_texlist;
extern NJS_TEXNAME sh_efftex_texname[SPEED_EFFECT_TEXTURES];
extern NJS_TEXLIST sh_efftex_texlist;

extern struct texture_anim flame_ring_anim;
extern NJS_CNK_MODEL flame_ring_model;
extern NJS_CNK_OBJECT flame_ring_object;

#endif
