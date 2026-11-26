/* the shaft walls' texture names and the layouts of their poses. the inner
 * wall has sixteen poses, and the middle and outer walls ten each. no code
 * reads the poses */
#ifndef SHARED_SHAFT_WALL_ART_H
#define SHARED_SHAFT_WALL_ART_H
#include <ninja.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

extern const char shaft_wall_texture_301[12];
extern const char shaft_wall_texture_302[12];

struct shaft_inner_wall_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[34];
        Sint16 length2;
        struct part_uv strip2[34];
        Sint16 length3;
        struct part_uv strip3[34];
        Sint16 length4;
        struct part_uv strip4[34];
        Sint16 length5;
        struct part_uv strip5[34];
        Sint16 length6;
        struct part_uv strip6[34];
        Sint16 length7;
        struct part_uv strip7[34];
        Sint16 length8;
        struct part_uv strip8[34];
        Uint16 strip_pad1;
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[136];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

extern struct shaft_inner_wall_pose shaft_inner_wall_poses[16];

struct shaft_side_wall_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[10];
        Sint16 length2;
        struct part_uv strip2[10];
        Sint16 length3;
        struct part_uv strip3[10];
        Sint16 length4;
        struct part_uv strip4[10];
        Sint16 length5;
        struct part_uv strip5[10];
        Sint16 length6;
        struct part_uv strip6[10];
        Sint16 length7;
        struct part_uv strip7[10];
        Sint16 length8;
        struct part_uv strip8[10];
        Uint16 strip_pad1;
        struct part_texture texture2;
        struct part_strip_header strips2;
        Sint16 length9;
        struct part_uv strip9[8];
        Sint16 length10;
        struct part_uv strip10[8];
        Sint16 length11;
        struct part_uv strip11[8];
        Sint16 length12;
        struct part_uv strip12[8];
        Sint16 length13;
        struct part_uv strip13[8];
        Sint16 length14;
        struct part_uv strip14[8];
        Sint16 length15;
        struct part_uv strip15[8];
        Sint16 length16;
        struct part_uv strip16[8];
        Uint16 strip_pad2;
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[64];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};

extern struct shaft_side_wall_pose shaft_middle_wall_poses[10];

extern struct shaft_side_wall_pose shaft_outer_wall_poses[10];

#endif
