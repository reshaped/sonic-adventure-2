/* parts of a chunk polygon list, as fields of the records that lay out one
 * model's list */
#ifndef DRAW_CHUNK_POLYGON_H
#define DRAW_CHUNK_POLYGON_H
#include <sg_xpt.h>

/* a polygon list is aligned to two bytes only. so a 32-bit color is two
 * Uint16s, low half first */
struct part_color {
    Uint16 low, high;
};
/* Ninja material chunks place a header and word count before the packed colors.
 */
struct part_material_diffuse {
    Uint16 head, words;
    struct part_color diffuse;
};
struct part_material_diffuse_ambient {
    Uint16 head, words;
    struct part_color diffuse, ambient;
};
struct part_material_diffuse_ambient_specular {
    Uint16 head, words;
    struct part_color diffuse, ambient, specular;
};
struct part_texture {
    Uint16 head, texture;
};
struct part_strip_header {
    Uint16 head, words, count;
};
struct part_uv {
    Uint16 index;
    Sint16 u, v;
};

#endif
