/* STORE box geometry. The blue and green lids have 20 strips and the red
 * lid has 21. All three finish with four side strips and a shadow strip. */
#ifndef SHARED_STORE_MESH_H
#define SHARED_STORE_MESH_H

#include <sg_xpt.h>

#include "draw/chunk_polygon.h"

struct store_strip_3 {
    Sint16 length;
    struct part_uv vertices[3];
};

struct store_strip_4 {
    Sint16 length;
    struct part_uv vertices[4];
};

struct store_strip_5 {
    Sint16 length;
    struct part_uv vertices[5];
};

struct store_strip_6 {
    Sint16 length;
    struct part_uv vertices[6];
};

struct store_box_side_and_shadow {
    struct part_texture side_texture;
    struct part_strip_header side_header;
    struct store_strip_4 sides[4];
    Uint16 side_padding;
    struct part_texture shadow_texture;
    struct part_strip_header shadow_header;
    struct store_strip_4 shadow;
    Uint16 end;
};

struct store_box_polygons {
    struct part_material_diffuse_ambient material;
    struct part_texture lid_texture;
    struct part_strip_header lid_header;
    struct store_strip_4 strips_1_4[4];
    struct store_strip_6 strips_5_6[2];
    struct store_strip_4 strip_7;
    struct store_strip_6 strip_8;
    struct store_strip_4 strips_9_11[3];
    struct store_strip_6 strip_12;
    struct store_strip_4 strips_13_18[6];
    struct store_strip_5 strip_19;
    struct store_strip_3 strip_20;
    Uint16 lid_padding;
    struct store_box_side_and_shadow side_and_shadow;
};

struct store_box_red_polygons {
    struct part_material_diffuse_ambient material;
    struct part_texture lid_texture;
    struct part_strip_header lid_header;
    struct store_strip_4 strips_1_4[4];
    struct store_strip_6 strips_5_6[2];
    struct store_strip_4 strips_7_12[6];
    struct store_strip_6 strip_13;
    struct store_strip_4 strips_14_19[6];
    struct store_strip_5 strip_20;
    struct store_strip_3 strip_21;
    struct store_box_side_and_shadow side_and_shadow;
};

#endif
