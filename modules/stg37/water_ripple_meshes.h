/* the layouts of the cover's two ripples, a record to a frame. a frame is a
 * quad of 4 vertices or a strip of 6. no source includes this header */
#ifndef STG37_WATER_RIPPLE_MESHES_H
#define STG37_WATER_RIPPLE_MESHES_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

struct ripple_quad {
    struct {
        Sint16 material[6];
        struct part_texture texture;
        struct part_strip_header strips;
        Sint16 length;
        struct part_uv uv[4];
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[4];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

struct ripple_strip {
    struct {
        Sint16 material[6];
        struct part_texture texture;
        struct part_strip_header strips;
        Sint16 length;
        struct part_uv uv[6];
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[6];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

struct cover_ripple_a_bank {
    struct ripple_quad frame_59;
    struct ripple_quad frame_58;
    struct ripple_quad frame_57;
    struct ripple_quad frame_56;
    struct ripple_quad frame_55;
    struct ripple_quad frame_54;
    struct ripple_quad frame_53;
    struct ripple_quad frame_52;
    struct ripple_quad frame_51;
    struct ripple_quad frame_50;
    struct ripple_quad frame_49;
    struct ripple_quad frame_48;
    struct ripple_quad frame_47;
    struct ripple_quad frame_45;
    struct ripple_quad frame_46;
    struct ripple_quad frame_44;
    struct ripple_strip frame_43;
    struct ripple_strip frame_42;
    struct ripple_strip frame_41;
    struct ripple_strip frame_40;
    struct ripple_strip frame_39;
    struct ripple_strip frame_38;
    struct ripple_strip frame_37;
    struct ripple_strip frame_36;
    struct ripple_strip frame_35;
    struct ripple_strip frame_34;
    struct ripple_strip frame_33;
    struct ripple_strip frame_32;
    struct ripple_strip frame_31;
    struct ripple_strip frame_30;
    struct ripple_strip frame_29;
    struct ripple_strip frame_28;
    struct ripple_strip frame_27;
    struct ripple_strip frame_26;
    struct ripple_strip frame_25;
    struct ripple_strip frame_24;
    struct ripple_strip frame_23;
    struct ripple_strip frame_22;
    struct ripple_strip frame_21;
    struct ripple_strip frame_20;
    struct ripple_strip frame_19;
    struct ripple_strip frame_18;
    struct ripple_strip frame_17;
    struct ripple_strip frame_16;
    struct ripple_strip frame_15;
    struct ripple_quad frame_14;
    struct ripple_quad frame_13;
    struct ripple_quad frame_12;
    struct ripple_quad frame_11;
    struct ripple_quad frame_10;
    struct ripple_quad frame_9;
    struct ripple_quad frame_8;
    struct ripple_quad frame_7;
    struct ripple_quad frame_6;
    struct ripple_quad frame_5;
    struct ripple_quad frame_4;
    struct ripple_quad frame_3;
    struct ripple_quad frame_2;
    struct ripple_quad frame_1;
    struct ripple_quad frame_0;
    NJS_CNK_OBJECT root;
};
extern struct cover_ripple_a_bank cover_ripple_a;

struct cover_ripple_b_bank {
    struct ripple_quad frame_59;
    struct ripple_quad frame_58;
    struct ripple_quad frame_57;
    struct ripple_quad frame_56;
    struct ripple_quad frame_55;
    struct ripple_quad frame_54;
    struct ripple_quad frame_53;
    struct ripple_quad frame_52;
    struct ripple_quad frame_51;
    struct ripple_quad frame_50;
    struct ripple_quad frame_49;
    struct ripple_quad frame_48;
    struct ripple_quad frame_47;
    struct ripple_quad frame_46;
    struct ripple_quad frame_45;
    struct ripple_quad frame_44;
    struct ripple_strip frame_43;
    struct ripple_strip frame_42;
    struct ripple_strip frame_41;
    struct ripple_strip frame_40;
    struct ripple_strip frame_39;
    struct ripple_strip frame_38;
    struct ripple_strip frame_37;
    struct ripple_strip frame_36;
    struct ripple_strip frame_35;
    struct ripple_strip frame_34;
    struct ripple_strip frame_33;
    struct ripple_strip frame_32;
    struct ripple_strip frame_31;
    struct ripple_strip frame_30;
    struct ripple_strip frame_29;
    struct ripple_strip frame_28;
    struct ripple_strip frame_27;
    struct ripple_strip frame_26;
    struct ripple_strip frame_25;
    struct ripple_strip frame_24;
    struct ripple_strip frame_23;
    struct ripple_strip frame_22;
    struct ripple_strip frame_21;
    struct ripple_strip frame_20;
    struct ripple_strip frame_19;
    struct ripple_strip frame_18;
    struct ripple_strip frame_17;
    struct ripple_strip frame_16;
    struct ripple_strip frame_15;
    struct ripple_quad frame_14;
    struct ripple_quad frame_13;
    struct ripple_quad frame_12;
    struct ripple_quad frame_11;
    struct ripple_quad frame_10;
    struct ripple_quad frame_9;
    struct ripple_quad frame_8;
    struct ripple_quad frame_7;
    struct ripple_quad frame_6;
    struct ripple_quad frame_5;
    struct ripple_quad frame_4;
    struct ripple_quad frame_3;
    struct ripple_quad frame_2;
    struct ripple_quad frame_1;
    struct ripple_quad frame_0;
    NJS_CNK_OBJECT root;
};
extern struct cover_ripple_b_bank cover_ripple_b;

#endif
