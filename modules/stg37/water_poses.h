/* the poses of Rouge's water: the surface's, the rotor water's, the bubbles'
 * and the splashes' models. no source includes this header */
#ifndef STG37_WATER_POSES_H
#define STG37_WATER_POSES_H
#include <ninja.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

extern NJS_CNK_OBJECT surface_pose_0;
extern NJS_CNK_OBJECT surface_pose_1;
extern NJS_CNK_OBJECT surface_pose_2;
extern NJS_CNK_OBJECT surface_pose_3;
extern NJS_CNK_OBJECT surface_pose_4;
extern NJS_CNK_OBJECT surface_pose_5;
extern NJS_CNK_OBJECT surface_pose_6;
extern NJS_CNK_OBJECT surface_pose_7;
extern NJS_CNK_OBJECT surface_pose_8;
extern NJS_CNK_OBJECT surface_pose_9;
extern NJS_CNK_OBJECT surface_pose_10;
extern NJS_CNK_OBJECT surface_pose_11;
extern NJS_CNK_OBJECT surface_pose_12;
extern NJS_CNK_OBJECT surface_pose_13;
extern NJS_CNK_OBJECT surface_pose_14;
extern NJS_CNK_OBJECT surface_pose_15;
extern NJS_CNK_OBJECT surface_pose_16;
extern NJS_CNK_OBJECT surface_pose_17;
extern NJS_CNK_OBJECT surface_pose_18;
extern NJS_CNK_OBJECT surface_pose_19;
extern NJS_CNK_OBJECT surface_pose_20;
extern NJS_CNK_OBJECT surface_pose_21;
extern NJS_CNK_OBJECT surface_pose_22;
extern NJS_CNK_OBJECT surface_pose_23;
extern NJS_CNK_OBJECT surface_pose_24;
extern NJS_CNK_OBJECT surface_pose_25;
extern NJS_CNK_OBJECT surface_pose_26;
extern NJS_CNK_OBJECT surface_pose_27;
extern NJS_CNK_OBJECT surface_pose_28;
extern NJS_CNK_OBJECT surface_pose_29;
extern NJS_CNK_OBJECT surface_pose_30;
extern NJS_CNK_OBJECT surface_pose_31;

struct rotor_water_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[3];
        Sint16 length2;
        struct part_uv strip2[6];
        Sint16 length3;
        struct part_uv strip3[4];
        Sint16 length4;
        struct part_uv strip4[5];
        Sint16 length5;
        struct part_uv strip5[13];
        Sint16 length6;
        struct part_uv strip6[7];
        Sint16 length7;
        struct part_uv strip7[5];
        Sint16 length8;
        struct part_uv strip8[11];
        Uint16 strip_pad1;
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[30];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};
extern struct rotor_water_pose rotor_water_poses[20];

struct bubble_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[4];
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[4];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};
extern struct bubble_pose bubble_poses[60];

struct second_splash_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[6];
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[6];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};
extern struct second_splash_pose second_splash_poses[15];

struct first_splash_pose {
    struct {
        struct {
            Uint16 head, words;
            struct part_color diffuse, ambient;
        } material1;
        struct part_texture texture1;
        struct part_strip_header strips1;
        Sint16 length1;
        struct part_uv strip1[8];
        Sint16 length2;
        struct part_uv strip2[8];
        Uint16 strip_pad1;
        Uint16 end;
    } polygons;
    struct {
        Sint32 head[2];
        struct chunk_vertex_d8 vertices[16];
        Sint32 end;
    } vertices;
    NJS_CNK_MODEL mesh;
    NJS_CNK_OBJECT node;
};
extern struct first_splash_pose first_splash_poses[36];

extern NJS_CNK_MODEL water_splash_pose_0;
extern NJS_CNK_MODEL water_splash_pose_1;
extern NJS_CNK_MODEL water_splash_pose_2;
extern NJS_CNK_MODEL water_splash_pose_3;
extern NJS_CNK_MODEL water_splash_pose_4;
extern NJS_CNK_MODEL water_splash_pose_5;
extern NJS_CNK_MODEL water_splash_pose_6;
extern NJS_CNK_MODEL water_splash_pose_7;
extern NJS_CNK_MODEL water_splash_pose_8;
extern NJS_CNK_MODEL water_splash_pose_9;
extern NJS_CNK_MODEL water_splash_pose_10;
extern NJS_CNK_MODEL water_splash_pose_11;
extern NJS_CNK_MODEL water_splash_pose_12;
extern NJS_CNK_MODEL water_splash_pose_13;
extern NJS_CNK_MODEL water_splash_pose_14;
extern NJS_CNK_MODEL water_splash_pose_15;

#endif
