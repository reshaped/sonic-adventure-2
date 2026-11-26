/* The cloth and hook share the same strips in each wave frame. The low
 * flag gives the hook a diffuse material of its own before the hook texture. */
#ifndef SHARED_FLAG_FRAMES_H
#define SHARED_FLAG_FRAMES_H

#include <ninja.h>
#include <sg_xpt.h>
#include "draw/chunk_polygon.h"

enum { FLAG_FRAMES = 24 };

struct flag_strip {
    Sint16 length;
    struct part_uv vertices[4];
};

struct flag_hook_material {
    Uint16 head, words;
    struct part_color diffuse;
};

struct canyon_flag_polygons {
    Sint16 cloth_material[6];
    struct part_texture cloth_texture;
    struct part_strip_header cloth_strips;
    struct flag_strip cloth[13];
    struct part_texture hook_texture;
    struct part_strip_header hook_strips;
    struct flag_strip hook;
    Uint16 end;
};

struct flag_low_polygons {
    Sint16 cloth_material[6];
    struct part_texture cloth_texture;
    struct part_strip_header cloth_strips;
    struct flag_strip cloth[13];
    struct flag_hook_material hook_material;
    struct part_texture hook_texture;
    struct part_strip_header hook_strips;
    struct flag_strip hook;
    Uint16 end;
};

struct flag_frame {
    NJS_TEXNAME names[2];
    NJS_TEXLIST textures;
    struct canyon_flag_polygons polygons;
    Sint32 vertices[183];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT object;
};

struct flag_low_frame {
    NJS_TEXNAME names[2];
    NJS_TEXLIST textures;
    struct flag_low_polygons polygons;
    Sint32 vertices[183];
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT object;
};

#endif
