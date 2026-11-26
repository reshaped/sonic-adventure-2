/* the super forms' effects in the Finalhazard's fight: the switch flash, the
 * glow, and each form's trail */
#ifndef BOSS_LAST2_SUPER_EFFECTS_H
#define BOSS_LAST2_SUPER_EFFECTS_H
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/texture_anim.h"
#include "draw/chunk.h"
extern NJS_TEXLIST super_effect_textures;

extern NJS_CNK_OBJECT super_flash;

extern NJS_CNK_MODEL super_glow_model;
extern NJS_CNK_OBJECT super_glow;
extern struct texture_anim super_glow_animation;
extern NJS_CNK_OBJECT super_sonic_trail;
extern struct texture_anim super_sonic_trail_animation;
extern NJS_CNK_OBJECT super_shadow_trail;
extern struct texture_anim super_shadow_trail_animation;

/* the chunks of the flash's polygon list, spelled out so that super.c can write
 * its texture. they are a material, the texture, and strips. each vertex of a
 * strip holds an index, a u and a v */
struct super_strip_uv {
    Uint16 index;
    Sint16 u, v;
};
struct super_texture_word {
    Uint16 head;
    Sint16 texture;
};
struct super_strip_head {
    Uint16 head, words, count;
};

/* the glow's vertex chunk: its two head words, then each vertex's position and
 * color. super.c recolors the vertices of super_glow_vertices whose colors in
 * super_glow_source are opaque. the new color brightens with the step count */
#define SUPER_GLOW_VERTICES 49
struct super_glow_vertices {
    Sint32 head[2];
    struct chunk_vertex_d8 vertices[SUPER_GLOW_VERTICES];
    Sint32 end;
};
extern struct super_glow_vertices super_glow_vertices;
extern struct super_glow_vertices super_glow_source;
struct super_flash_polygons {
    Sint16 material[6];
    struct super_texture_word texture;
    struct super_strip_head strips;
    Sint16 length1;
    struct super_strip_uv strip1[3];
    Sint16 length2;
    struct super_strip_uv strip2[10];
    Sint16 length3;
    struct super_strip_uv strip3[10];
    Sint16 length4;
    struct super_strip_uv strip4[10];
    Sint16 length5;
    struct super_strip_uv strip5[10];
    Sint16 length6;
    struct super_strip_uv strip6[5];
    Sint16 length7;
    struct super_strip_uv strip7[10];
    Sint16 length8;
    struct super_strip_uv strip8[10];
    Sint16 length9;
    struct super_strip_uv strip9[3];
    Sint16 length10;
    struct super_strip_uv strip10[10];
    Sint16 length11;
    struct super_strip_uv strip11[5];
    Sint16 length12;
    struct super_strip_uv strip12[10];
    Sint16 length13;
    struct super_strip_uv strip13[18];
    Uint16 end;
};
extern struct super_flash_polygons super_flash_polygons;
#ifdef NONMATCHING
/* the field by its own spelling: this side's objects don't define
 * super_flash_texture, which only symbols.txt places */
#define SUPER_FLASH_TEXTURE (super_flash_polygons.texture.texture)
#else
/* @hack type: the flash's texture under a second name, which symbols.txt places
 * at super_flash_polygons.texture.texture, where the field would do */
extern Sint16 super_flash_texture;
#define SUPER_FLASH_TEXTURE super_flash_texture
#endif
#endif
