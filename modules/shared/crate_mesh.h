/* Wood and iron crate mesh records, including their alternate materials. */
#ifndef SHARED_CRATE_MESH_H
#define SHARED_CRATE_MESH_H

#include <sg_xpt.h>

#include "draw/chunk_polygon.h"

struct crate_polygon_list {
    Sint16 material1[8];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    struct part_texture texture2;
    struct part_strip_header strips2;
    Sint16 length2;
    struct part_uv strip2[4];
    Sint16 length3;
    struct part_uv strip3[4];
    Sint16 length4;
    struct part_uv strip4[4];
    Sint16 length5;
    struct part_uv strip5[4];
    Uint16 strip_pad2;
    Uint16 end;
};

struct wood_box_piece1_polygon_list {
    Sint16 material1[8];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[3];
    Sint16 length2;
    struct part_uv strip2[3];
    Sint16 length3;
    struct part_uv strip3[5];
    Sint16 length4;
    struct part_uv strip4[3];
    Sint16 length5;
    struct part_uv strip5[3];
    Sint16 length6;
    struct part_uv strip6[3];
    Sint16 length7;
    struct part_uv strip7[3];
    Sint16 length8;
    struct part_uv strip8[3];
    Sint16 length9;
    struct part_uv strip9[3];
    Sint16 length10;
    struct part_uv strip10[3];
    Sint16 length11;
    struct part_uv strip11[3];
    Sint16 length12;
    struct part_uv strip12[3];
    Sint16 length13;
    struct part_uv strip13[3];
    Sint16 length14;
    struct part_uv strip14[3];
    Sint16 length15;
    struct part_uv strip15[3];
    Sint16 length16;
    struct part_uv strip16[4];
    Sint16 length17;
    struct part_uv strip17[4];
    Sint16 length18;
    struct part_uv strip18[6];
    Uint16 end;
};

struct wood_box_piece2_polygon_list {
    Sint16 material1[6];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[3];
    Sint16 length2;
    struct part_uv strip2[3];
    Sint16 length3;
    struct part_uv strip3[3];
    Sint16 length4;
    struct part_uv strip4[5];
    Sint16 length5;
    struct part_uv strip5[3];
    Sint16 length6;
    struct part_uv strip6[3];
    Sint16 length7;
    struct part_uv strip7[4];
    Sint16 length8;
    struct part_uv strip8[4];
    Sint16 length9;
    struct part_uv strip9[4];
    Sint16 length10;
    struct part_uv strip10[4];
    Uint16 strip_pad1;
    Uint16 end;
};

struct wood_box_piece3_polygon_list {
    Sint16 material1[6];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    Sint16 length2;
    struct part_uv strip2[6];
    Sint16 length3;
    struct part_uv strip3[4];
    Sint16 length4;
    struct part_uv strip4[3];
    Sint16 length5;
    struct part_uv strip5[3];
    Sint16 length6;
    struct part_uv strip6[5];
    Sint16 length7;
    struct part_uv strip7[3];
    Sint16 length8;
    struct part_uv strip8[3];
    Sint16 length9;
    struct part_uv strip9[3];
    Sint16 length10;
    struct part_uv strip10[3];
    Sint16 length11;
    struct part_uv strip11[3];
    Sint16 length12;
    struct part_uv strip12[3];
    Sint16 length13;
    struct part_uv strip13[3];
    Sint16 length14;
    struct part_uv strip14[3];
    Sint16 length15;
    struct part_uv strip15[3];
    Sint16 length16;
    struct part_uv strip16[4];
    Uint16 strip_pad1;
    Uint16 end;
};

struct iron_box_piece1_polygon_list {
    Sint16 material1[8];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[8];
    Sint16 length2;
    struct part_uv strip2[4];
    Sint16 length3;
    struct part_uv strip3[6];
    Sint16 length4;
    struct part_uv strip4[6];
    Sint16 length5;
    struct part_uv strip5[3];
    Sint16 length6;
    struct part_uv strip6[3];
    Sint16 length7;
    struct part_uv strip7[6];
    Sint16 length8;
    struct part_uv strip8[6];
    Uint16 strip_pad1;
    Uint16 end;
};

struct iron_box_piece2_polygon_list {
    Sint16 material1[8];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[7];
    Sint16 length2;
    struct part_uv strip2[3];
    Uint16 strip_pad1;
    struct part_texture texture2;
    struct part_strip_header strips2;
    Sint16 length3;
    struct part_uv strip3[4];
    Sint16 length4;
    struct part_uv strip4[5];
    Sint16 length5;
    struct part_uv strip5[3];
    Sint16 length6;
    struct part_uv strip6[9];
    Sint16 length7;
    struct part_uv strip7[3];
    Sint16 length8;
    struct part_uv strip8[4];
    Sint16 length9;
    struct part_uv strip9[4];
    Sint16 length10;
    struct part_uv strip10[8];
    Uint16 strip_pad2;
    Uint16 end;
};

struct iron_box_piece3_polygon_list {
    Sint16 material1[8];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[3];
    Sint16 length2;
    struct part_uv strip2[4];
    Sint16 length3;
    struct part_uv strip3[9];
    Sint16 length4;
    struct part_uv strip4[3];
    Sint16 length5;
    struct part_uv strip5[6];
    Sint16 length6;
    struct part_uv strip6[6];
    Sint16 length7;
    struct part_uv strip7[6];
    Sint16 length8;
    struct part_uv strip8[7];
    Uint16 strip_pad1;
    Uint16 end;
};

#endif
