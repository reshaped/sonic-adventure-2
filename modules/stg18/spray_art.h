/* Each of the 32 spray frames has its own writable mesh and two-entry texture
 * list. */
#ifndef STG18_SPRAY_ART_H
#define STG18_SPRAY_ART_H
#include <ninja.h>

#include "draw/chunk_polygon.h"

struct spray_polygons_layout_1 {
    Sint16 material[6];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    struct part_texture texture2;
    struct part_strip_header strips2;
    Sint16 length2;
    struct part_uv strip2[18];
    Uint16 end;
};

struct spray_polygons_layout_2 {
    Sint16 material[6];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    Sint16 length2;
    struct part_uv strip2[4];
    Uint16 strip_pad1;
    struct part_texture texture2;
    struct part_strip_header strips2;
    Sint16 length3;
    struct part_uv strip3[18];
    Sint16 length4;
    struct part_uv strip4[18];
    Uint16 strip_pad2;
    Uint16 end;
};

struct spray_polygons_layout_3 {
    Sint16 material[6];
    struct part_texture texture1;
    struct part_strip_header strips1;
    Sint16 length1;
    struct part_uv strip1[4];
    Uint16 end;
};

static NJS_TEXNAME spray_texlist_0_names[2] = {
#include "stg18/spray_art/spray_texlist_0_names.inc"
};

static NJS_TEXLIST spray_texlist_0 = {
#include "stg18/spray_art/spray_texlist_0.inc"
};

static struct spray_polygons_layout_1 spray_object_0_polygons = {
#include "stg18/spray_art/spray_object_0_polygons.inc"
};

static Sint32 spray_object_0_vertices[83] = {
#include "stg18/spray_art/spray_object_0_vertices.inc"
};

static NJS_CNK_MODEL spray_model_0 = {
#include "stg18/spray_art/spray_model_0.inc"
};

static NJS_CNK_OBJECT spray_object_0 = {
#include "stg18/spray_art/spray_object_0.inc"
};

static NJS_TEXNAME spray_texlist_1_names[2] = {
#include "stg18/spray_art/spray_texlist_1_names.inc"
};

static NJS_TEXLIST spray_texlist_1 = {
#include "stg18/spray_art/spray_texlist_1.inc"
};

static struct spray_polygons_layout_1 spray_object_1_polygons = {
#include "stg18/spray_art/spray_object_1_polygons.inc"
};

static Sint32 spray_object_1_vertices[83] = {
#include "stg18/spray_art/spray_object_1_vertices.inc"
};

static NJS_CNK_MODEL spray_model_1 = {
#include "stg18/spray_art/spray_model_1.inc"
};

static NJS_CNK_OBJECT spray_object_1 = {
#include "stg18/spray_art/spray_object_1.inc"
};

static NJS_TEXNAME spray_texlist_2_names[2] = {
#include "stg18/spray_art/spray_texlist_2_names.inc"
};

static NJS_TEXLIST spray_texlist_2 = {
#include "stg18/spray_art/spray_texlist_2.inc"
};

static struct spray_polygons_layout_1 spray_object_2_polygons = {
#include "stg18/spray_art/spray_object_2_polygons.inc"
};

static Sint32 spray_object_2_vertices[83] = {
#include "stg18/spray_art/spray_object_2_vertices.inc"
};

static NJS_CNK_MODEL spray_model_2 = {
#include "stg18/spray_art/spray_model_2.inc"
};

static NJS_CNK_OBJECT spray_object_2 = {
#include "stg18/spray_art/spray_object_2.inc"
};

static NJS_TEXNAME spray_texlist_3_names[2] = {
#include "stg18/spray_art/spray_texlist_3_names.inc"
};

static NJS_TEXLIST spray_texlist_3 = {
#include "stg18/spray_art/spray_texlist_3.inc"
};

static struct spray_polygons_layout_1 spray_object_3_polygons = {
#include "stg18/spray_art/spray_object_3_polygons.inc"
};

static Sint32 spray_object_3_vertices[83] = {
#include "stg18/spray_art/spray_object_3_vertices.inc"
};

static NJS_CNK_MODEL spray_model_3 = {
#include "stg18/spray_art/spray_model_3.inc"
};

static NJS_CNK_OBJECT spray_object_3 = {
#include "stg18/spray_art/spray_object_3.inc"
};

static NJS_TEXNAME spray_texlist_4_names[2] = {
#include "stg18/spray_art/spray_texlist_4_names.inc"
};

static NJS_TEXLIST spray_texlist_4 = {
#include "stg18/spray_art/spray_texlist_4.inc"
};

static struct spray_polygons_layout_2 spray_object_4_polygons = {
#include "stg18/spray_art/spray_object_4_polygons.inc"
};

static Sint32 spray_object_4_vertices[131] = {
#include "stg18/spray_art/spray_object_4_vertices.inc"
};

static NJS_CNK_MODEL spray_model_4 = {
#include "stg18/spray_art/spray_model_4.inc"
};

static NJS_CNK_OBJECT spray_object_4 = {
#include "stg18/spray_art/spray_object_4.inc"
};

static NJS_TEXNAME spray_texlist_5_names[2] = {
#include "stg18/spray_art/spray_texlist_5_names.inc"
};

static NJS_TEXLIST spray_texlist_5 = {
#include "stg18/spray_art/spray_texlist_5.inc"
};

static struct spray_polygons_layout_2 spray_object_5_polygons = {
#include "stg18/spray_art/spray_object_5_polygons.inc"
};

static Sint32 spray_object_5_vertices[131] = {
#include "stg18/spray_art/spray_object_5_vertices.inc"
};

static NJS_CNK_MODEL spray_model_5 = {
#include "stg18/spray_art/spray_model_5.inc"
};

static NJS_CNK_OBJECT spray_object_5 = {
#include "stg18/spray_art/spray_object_5.inc"
};

static NJS_TEXNAME spray_texlist_6_names[2] = {
#include "stg18/spray_art/spray_texlist_6_names.inc"
};

static NJS_TEXLIST spray_texlist_6 = {
#include "stg18/spray_art/spray_texlist_6.inc"
};

static struct spray_polygons_layout_2 spray_object_6_polygons = {
#include "stg18/spray_art/spray_object_6_polygons.inc"
};

static Sint32 spray_object_6_vertices[131] = {
#include "stg18/spray_art/spray_object_6_vertices.inc"
};

static NJS_CNK_MODEL spray_model_6 = {
#include "stg18/spray_art/spray_model_6.inc"
};

static NJS_CNK_OBJECT spray_object_6 = {
#include "stg18/spray_art/spray_object_6.inc"
};

static NJS_TEXNAME spray_texlist_7_names[2] = {
#include "stg18/spray_art/spray_texlist_7_names.inc"
};

static NJS_TEXLIST spray_texlist_7 = {
#include "stg18/spray_art/spray_texlist_7.inc"
};

static struct spray_polygons_layout_2 spray_object_7_polygons = {
#include "stg18/spray_art/spray_object_7_polygons.inc"
};

static Sint32 spray_object_7_vertices[131] = {
#include "stg18/spray_art/spray_object_7_vertices.inc"
};

static NJS_CNK_MODEL spray_model_7 = {
#include "stg18/spray_art/spray_model_7.inc"
};

static NJS_CNK_OBJECT spray_object_7 = {
#include "stg18/spray_art/spray_object_7.inc"
};

static NJS_TEXNAME spray_texlist_8_names[2] = {
#include "stg18/spray_art/spray_texlist_8_names.inc"
};

static NJS_TEXLIST spray_texlist_8 = {
#include "stg18/spray_art/spray_texlist_8.inc"
};

static struct spray_polygons_layout_2 spray_object_8_polygons = {
#include "stg18/spray_art/spray_object_8_polygons.inc"
};

static Sint32 spray_object_8_vertices[131] = {
#include "stg18/spray_art/spray_object_8_vertices.inc"
};

static NJS_CNK_MODEL spray_model_8 = {
#include "stg18/spray_art/spray_model_8.inc"
};

static NJS_CNK_OBJECT spray_object_8 = {
#include "stg18/spray_art/spray_object_8.inc"
};

static NJS_TEXNAME spray_texlist_9_names[2] = {
#include "stg18/spray_art/spray_texlist_9_names.inc"
};

static NJS_TEXLIST spray_texlist_9 = {
#include "stg18/spray_art/spray_texlist_9.inc"
};

static struct spray_polygons_layout_2 spray_object_9_polygons = {
#include "stg18/spray_art/spray_object_9_polygons.inc"
};

static Sint32 spray_object_9_vertices[131] = {
#include "stg18/spray_art/spray_object_9_vertices.inc"
};

static NJS_CNK_MODEL spray_model_9 = {
#include "stg18/spray_art/spray_model_9.inc"
};

static NJS_CNK_OBJECT spray_object_9 = {
#include "stg18/spray_art/spray_object_9.inc"
};

static NJS_TEXNAME spray_texlist_10_names[2] = {
#include "stg18/spray_art/spray_texlist_10_names.inc"
};

static NJS_TEXLIST spray_texlist_10 = {
#include "stg18/spray_art/spray_texlist_10.inc"
};

static struct spray_polygons_layout_2 spray_object_10_polygons = {
#include "stg18/spray_art/spray_object_10_polygons.inc"
};

static Sint32 spray_object_10_vertices[131] = {
#include "stg18/spray_art/spray_object_10_vertices.inc"
};

static NJS_CNK_MODEL spray_model_10 = {
#include "stg18/spray_art/spray_model_10.inc"
};

static NJS_CNK_OBJECT spray_object_10 = {
#include "stg18/spray_art/spray_object_10.inc"
};

static NJS_TEXNAME spray_texlist_11_names[2] = {
#include "stg18/spray_art/spray_texlist_11_names.inc"
};

static NJS_TEXLIST spray_texlist_11 = {
#include "stg18/spray_art/spray_texlist_11.inc"
};

static struct spray_polygons_layout_2 spray_object_11_polygons = {
#include "stg18/spray_art/spray_object_11_polygons.inc"
};

static Sint32 spray_object_11_vertices[131] = {
#include "stg18/spray_art/spray_object_11_vertices.inc"
};

static NJS_CNK_MODEL spray_model_11 = {
#include "stg18/spray_art/spray_model_11.inc"
};

static NJS_CNK_OBJECT spray_object_11 = {
#include "stg18/spray_art/spray_object_11.inc"
};

static NJS_TEXNAME spray_texlist_12_names[2] = {
#include "stg18/spray_art/spray_texlist_12_names.inc"
};

static NJS_TEXLIST spray_texlist_12 = {
#include "stg18/spray_art/spray_texlist_12.inc"
};

static struct spray_polygons_layout_2 spray_object_12_polygons = {
#include "stg18/spray_art/spray_object_12_polygons.inc"
};

static Sint32 spray_object_12_vertices[131] = {
#include "stg18/spray_art/spray_object_12_vertices.inc"
};

static NJS_CNK_MODEL spray_model_12 = {
#include "stg18/spray_art/spray_model_12.inc"
};

static NJS_CNK_OBJECT spray_object_12 = {
#include "stg18/spray_art/spray_object_12.inc"
};

static NJS_TEXNAME spray_texlist_13_names[2] = {
#include "stg18/spray_art/spray_texlist_13_names.inc"
};

static NJS_TEXLIST spray_texlist_13 = {
#include "stg18/spray_art/spray_texlist_13.inc"
};

static struct spray_polygons_layout_2 spray_object_13_polygons = {
#include "stg18/spray_art/spray_object_13_polygons.inc"
};

static Sint32 spray_object_13_vertices[131] = {
#include "stg18/spray_art/spray_object_13_vertices.inc"
};

static NJS_CNK_MODEL spray_model_13 = {
#include "stg18/spray_art/spray_model_13.inc"
};

static NJS_CNK_OBJECT spray_object_13 = {
#include "stg18/spray_art/spray_object_13.inc"
};

static NJS_TEXNAME spray_texlist_14_names[2] = {
#include "stg18/spray_art/spray_texlist_14_names.inc"
};

static NJS_TEXLIST spray_texlist_14 = {
#include "stg18/spray_art/spray_texlist_14.inc"
};

static struct spray_polygons_layout_2 spray_object_14_polygons = {
#include "stg18/spray_art/spray_object_14_polygons.inc"
};

static Sint32 spray_object_14_vertices[131] = {
#include "stg18/spray_art/spray_object_14_vertices.inc"
};

static NJS_CNK_MODEL spray_model_14 = {
#include "stg18/spray_art/spray_model_14.inc"
};

static NJS_CNK_OBJECT spray_object_14 = {
#include "stg18/spray_art/spray_object_14.inc"
};

static NJS_TEXNAME spray_texlist_15_names[2] = {
#include "stg18/spray_art/spray_texlist_15_names.inc"
};

static NJS_TEXLIST spray_texlist_15 = {
#include "stg18/spray_art/spray_texlist_15.inc"
};

static struct spray_polygons_layout_2 spray_object_15_polygons = {
#include "stg18/spray_art/spray_object_15_polygons.inc"
};

static Sint32 spray_object_15_vertices[131] = {
#include "stg18/spray_art/spray_object_15_vertices.inc"
};

static NJS_CNK_MODEL spray_model_15 = {
#include "stg18/spray_art/spray_model_15.inc"
};

static NJS_CNK_OBJECT spray_object_15 = {
#include "stg18/spray_art/spray_object_15.inc"
};

static NJS_TEXNAME spray_texlist_16_names[2] = {
#include "stg18/spray_art/spray_texlist_16_names.inc"
};

static NJS_TEXLIST spray_texlist_16 = {
#include "stg18/spray_art/spray_texlist_16.inc"
};

static struct spray_polygons_layout_2 spray_object_16_polygons = {
#include "stg18/spray_art/spray_object_16_polygons.inc"
};

static Sint32 spray_object_16_vertices[131] = {
#include "stg18/spray_art/spray_object_16_vertices.inc"
};

static NJS_CNK_MODEL spray_model_16 = {
#include "stg18/spray_art/spray_model_16.inc"
};

static NJS_CNK_OBJECT spray_object_16 = {
#include "stg18/spray_art/spray_object_16.inc"
};

static NJS_TEXNAME spray_texlist_17_names[2] = {
#include "stg18/spray_art/spray_texlist_17_names.inc"
};

static NJS_TEXLIST spray_texlist_17 = {
#include "stg18/spray_art/spray_texlist_17.inc"
};

static struct spray_polygons_layout_2 spray_object_17_polygons = {
#include "stg18/spray_art/spray_object_17_polygons.inc"
};

static Sint32 spray_object_17_vertices[131] = {
#include "stg18/spray_art/spray_object_17_vertices.inc"
};

static NJS_CNK_MODEL spray_model_17 = {
#include "stg18/spray_art/spray_model_17.inc"
};

static NJS_CNK_OBJECT spray_object_17 = {
#include "stg18/spray_art/spray_object_17.inc"
};

static NJS_TEXNAME spray_texlist_18_names[2] = {
#include "stg18/spray_art/spray_texlist_18_names.inc"
};

static NJS_TEXLIST spray_texlist_18 = {
#include "stg18/spray_art/spray_texlist_18.inc"
};

static struct spray_polygons_layout_2 spray_object_18_polygons = {
#include "stg18/spray_art/spray_object_18_polygons.inc"
};

static Sint32 spray_object_18_vertices[131] = {
#include "stg18/spray_art/spray_object_18_vertices.inc"
};

static NJS_CNK_MODEL spray_model_18 = {
#include "stg18/spray_art/spray_model_18.inc"
};

static NJS_CNK_OBJECT spray_object_18 = {
#include "stg18/spray_art/spray_object_18.inc"
};

static NJS_TEXNAME spray_texlist_19_names[2] = {
#include "stg18/spray_art/spray_texlist_19_names.inc"
};

static NJS_TEXLIST spray_texlist_19 = {
#include "stg18/spray_art/spray_texlist_19.inc"
};

static struct spray_polygons_layout_2 spray_object_19_polygons = {
#include "stg18/spray_art/spray_object_19_polygons.inc"
};

static Sint32 spray_object_19_vertices[131] = {
#include "stg18/spray_art/spray_object_19_vertices.inc"
};

static NJS_CNK_MODEL spray_model_19 = {
#include "stg18/spray_art/spray_model_19.inc"
};

static NJS_CNK_OBJECT spray_object_19 = {
#include "stg18/spray_art/spray_object_19.inc"
};

static NJS_TEXNAME spray_texlist_20_names[2] = {
#include "stg18/spray_art/spray_texlist_20_names.inc"
};

static NJS_TEXLIST spray_texlist_20 = {
#include "stg18/spray_art/spray_texlist_20.inc"
};

static struct spray_polygons_layout_2 spray_object_20_polygons = {
#include "stg18/spray_art/spray_object_20_polygons.inc"
};

static Sint32 spray_object_20_vertices[131] = {
#include "stg18/spray_art/spray_object_20_vertices.inc"
};

static NJS_CNK_MODEL spray_model_20 = {
#include "stg18/spray_art/spray_model_20.inc"
};

static NJS_CNK_OBJECT spray_object_20 = {
#include "stg18/spray_art/spray_object_20.inc"
};

static NJS_TEXNAME spray_texlist_21_names[2] = {
#include "stg18/spray_art/spray_texlist_21_names.inc"
};

static NJS_TEXLIST spray_texlist_21 = {
#include "stg18/spray_art/spray_texlist_21.inc"
};

static struct spray_polygons_layout_2 spray_object_21_polygons = {
#include "stg18/spray_art/spray_object_21_polygons.inc"
};

static Sint32 spray_object_21_vertices[131] = {
#include "stg18/spray_art/spray_object_21_vertices.inc"
};

static NJS_CNK_MODEL spray_model_21 = {
#include "stg18/spray_art/spray_model_21.inc"
};

static NJS_CNK_OBJECT spray_object_21 = {
#include "stg18/spray_art/spray_object_21.inc"
};

static NJS_TEXNAME spray_texlist_22_names[2] = {
#include "stg18/spray_art/spray_texlist_22_names.inc"
};

static NJS_TEXLIST spray_texlist_22 = {
#include "stg18/spray_art/spray_texlist_22.inc"
};

static struct spray_polygons_layout_2 spray_object_22_polygons = {
#include "stg18/spray_art/spray_object_22_polygons.inc"
};

static Sint32 spray_object_22_vertices[131] = {
#include "stg18/spray_art/spray_object_22_vertices.inc"
};

static NJS_CNK_MODEL spray_model_22 = {
#include "stg18/spray_art/spray_model_22.inc"
};

static NJS_CNK_OBJECT spray_object_22 = {
#include "stg18/spray_art/spray_object_22.inc"
};

static NJS_TEXNAME spray_texlist_23_names[2] = {
#include "stg18/spray_art/spray_texlist_23_names.inc"
};

static NJS_TEXLIST spray_texlist_23 = {
#include "stg18/spray_art/spray_texlist_23.inc"
};

static struct spray_polygons_layout_2 spray_object_23_polygons = {
#include "stg18/spray_art/spray_object_23_polygons.inc"
};

static Sint32 spray_object_23_vertices[131] = {
#include "stg18/spray_art/spray_object_23_vertices.inc"
};

static NJS_CNK_MODEL spray_model_23 = {
#include "stg18/spray_art/spray_model_23.inc"
};

static NJS_CNK_OBJECT spray_object_23 = {
#include "stg18/spray_art/spray_object_23.inc"
};

static NJS_TEXNAME spray_texlist_24_names[2] = {
#include "stg18/spray_art/spray_texlist_24_names.inc"
};

static NJS_TEXLIST spray_texlist_24 = {
#include "stg18/spray_art/spray_texlist_24.inc"
};

static struct spray_polygons_layout_2 spray_object_24_polygons = {
#include "stg18/spray_art/spray_object_24_polygons.inc"
};

static Sint32 spray_object_24_vertices[131] = {
#include "stg18/spray_art/spray_object_24_vertices.inc"
};

static NJS_CNK_MODEL spray_model_24 = {
#include "stg18/spray_art/spray_model_24.inc"
};

static NJS_CNK_OBJECT spray_object_24 = {
#include "stg18/spray_art/spray_object_24.inc"
};

static NJS_TEXNAME spray_texlist_25_names[2] = {
#include "stg18/spray_art/spray_texlist_25_names.inc"
};

static NJS_TEXLIST spray_texlist_25 = {
#include "stg18/spray_art/spray_texlist_25.inc"
};

static struct spray_polygons_layout_2 spray_object_25_polygons = {
#include "stg18/spray_art/spray_object_25_polygons.inc"
};

static Sint32 spray_object_25_vertices[131] = {
#include "stg18/spray_art/spray_object_25_vertices.inc"
};

static NJS_CNK_MODEL spray_model_25 = {
#include "stg18/spray_art/spray_model_25.inc"
};

static NJS_CNK_OBJECT spray_object_25 = {
#include "stg18/spray_art/spray_object_25.inc"
};

static NJS_TEXNAME spray_texlist_26_names[2] = {
#include "stg18/spray_art/spray_texlist_26_names.inc"
};

static NJS_TEXLIST spray_texlist_26 = {
#include "stg18/spray_art/spray_texlist_26.inc"
};

static struct spray_polygons_layout_3 spray_object_26_polygons = {
#include "stg18/spray_art/spray_object_26_polygons.inc"
};

static Sint32 spray_object_26_vertices[19] = {
#include "stg18/spray_art/spray_object_26_vertices.inc"
};

static NJS_CNK_MODEL spray_model_26 = {
#include "stg18/spray_art/spray_model_26.inc"
};

static NJS_CNK_OBJECT spray_object_26 = {
#include "stg18/spray_art/spray_object_26.inc"
};

static NJS_TEXNAME spray_texlist_27_names[2] = {
#include "stg18/spray_art/spray_texlist_27_names.inc"
};

static NJS_TEXLIST spray_texlist_27 = {
#include "stg18/spray_art/spray_texlist_27.inc"
};

static struct spray_polygons_layout_3 spray_object_27_polygons = {
#include "stg18/spray_art/spray_object_27_polygons.inc"
};

static Sint32 spray_object_27_vertices[19] = {
#include "stg18/spray_art/spray_object_27_vertices.inc"
};

static NJS_CNK_MODEL spray_model_27 = {
#include "stg18/spray_art/spray_model_27.inc"
};

static NJS_CNK_OBJECT spray_object_27 = {
#include "stg18/spray_art/spray_object_27.inc"
};

static NJS_TEXNAME spray_texlist_28_names[2] = {
#include "stg18/spray_art/spray_texlist_28_names.inc"
};

static NJS_TEXLIST spray_texlist_28 = {
#include "stg18/spray_art/spray_texlist_28.inc"
};

static struct spray_polygons_layout_3 spray_object_28_polygons = {
#include "stg18/spray_art/spray_object_28_polygons.inc"
};

static Sint32 spray_object_28_vertices[19] = {
#include "stg18/spray_art/spray_object_28_vertices.inc"
};

static NJS_CNK_MODEL spray_model_28 = {
#include "stg18/spray_art/spray_model_28.inc"
};

static NJS_CNK_OBJECT spray_object_28 = {
#include "stg18/spray_art/spray_object_28.inc"
};

static NJS_TEXNAME spray_texlist_29_names[2] = {
#include "stg18/spray_art/spray_texlist_29_names.inc"
};

static NJS_TEXLIST spray_texlist_29 = {
#include "stg18/spray_art/spray_texlist_29.inc"
};

static struct spray_polygons_layout_3 spray_object_29_polygons = {
#include "stg18/spray_art/spray_object_29_polygons.inc"
};

static Sint32 spray_object_29_vertices[19] = {
#include "stg18/spray_art/spray_object_29_vertices.inc"
};

static NJS_CNK_MODEL spray_model_29 = {
#include "stg18/spray_art/spray_model_29.inc"
};

static NJS_CNK_OBJECT spray_object_29 = {
#include "stg18/spray_art/spray_object_29.inc"
};

static NJS_TEXNAME spray_texlist_30_names[2] = {
#include "stg18/spray_art/spray_texlist_30_names.inc"
};

static NJS_TEXLIST spray_texlist_30 = {
#include "stg18/spray_art/spray_texlist_30.inc"
};

static struct spray_polygons_layout_3 spray_object_30_polygons = {
#include "stg18/spray_art/spray_object_30_polygons.inc"
};

static Sint32 spray_object_30_vertices[19] = {
#include "stg18/spray_art/spray_object_30_vertices.inc"
};

static NJS_CNK_MODEL spray_model_30 = {
#include "stg18/spray_art/spray_model_30.inc"
};

static NJS_CNK_OBJECT spray_object_30 = {
#include "stg18/spray_art/spray_object_30.inc"
};

static NJS_TEXNAME spray_texlist_31_names[2] = {
#include "stg18/spray_art/spray_texlist_31_names.inc"
};

static NJS_TEXLIST spray_texlist_31 = {
#include "stg18/spray_art/spray_texlist_31.inc"
};

static struct spray_polygons_layout_3 spray_object_31_polygons = {
#include "stg18/spray_art/spray_object_31_polygons.inc"
};

static Sint32 spray_object_31_vertices[19] = {
#include "stg18/spray_art/spray_object_31_vertices.inc"
};

static NJS_CNK_MODEL spray_model_31 = {
#include "stg18/spray_art/spray_model_31.inc"
};

static NJS_CNK_OBJECT spray_object_31 = {
#include "stg18/spray_art/spray_object_31.inc"
};

#endif
