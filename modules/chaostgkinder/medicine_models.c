/* the six medicines the checkup shows: red, green and lavender capsules,
 * and white, cyan and peach tablets. each model has polygons and vertices of
 * its own */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/medicine_models.h"
#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

struct medicine_diffuse {
    Uint16 head, words;
    struct part_color diffuse;
};

struct medicine_capsule_polygons {
    Sint16 front[8];
    struct part_texture environment;
    struct part_strip_header front_strips;
    Sint16 front1[1 + 5], front2[1 + 5], front3[1 + 8], front4[1 + 5];
    Sint16 front5[1 + 3], front6[1 + 17], front7[1 + 8], front8[1 + 7];
    Uint16 front_fill;
    struct medicine_diffuse back;
    struct part_strip_header back_strips;
    Sint16 back1[1 + 8], back2[1 + 5], back3[1 + 4], back4[1 + 6];
    Sint16 back5[1 + 3], back6[1 + 7], back7[1 + 17], back8[1 + 8];
    Uint16 back_fill, end;
};

struct medicine_capsule_vertices {
    Uint32 head, count;
    struct chunk_vertex_vn vertices[44];
    Uint32 end;
};

struct medicine_tablet_polygons {
    Sint16 material[6];
    struct part_texture face_texture;
    struct part_strip_header face_strips;
    Sint16 face1_count;
    struct part_uv face1[18];
    Sint16 face2_count;
    struct part_uv face2[18];
    Sint16 face3_count;
    struct part_uv face3[18];
    Sint16 face4_count;
    struct part_uv face4[18];
    Uint16 face_fill;
    struct part_texture rim_texture;
    struct part_strip_header rim_strips;
    Sint16 rim1_count;
    struct part_uv rim1[3];
    Sint16 rim2_count;
    struct part_uv rim2[3];
    Sint16 rim3_count;
    struct part_uv rim3[3];
    Sint16 rim4_count;
    struct part_uv rim4[5];
    Sint16 rim5_count;
    struct part_uv rim5[4];
    Sint16 rim6_count;
    struct part_uv rim6[4];
    Sint16 rim7_count;
    struct part_uv rim7[4];
    Uint16 end;
};

struct medicine_tablet_vertices {
    Uint32 head, count;
    struct chunk_vertex_vn vertices[40];
    Uint32 end;
};

static struct medicine_capsule_polygons red_capsule_polygons = {
#include "chaostgkinder/medicine_models/red_capsule_polygons.inc"
};

static struct medicine_capsule_vertices red_capsule_vertices = {
#include "chaostgkinder/medicine_models/red_capsule_vertices.inc"
};

static NJS_CNK_MODEL red_capsule_mesh = {
#include "chaostgkinder/medicine_models/red_capsule_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_1 = {
#include "chaostgkinder/medicine_models/medicine_model_1.inc"
};

static struct medicine_capsule_polygons green_capsule_polygons = {
#include "chaostgkinder/medicine_models/green_capsule_polygons.inc"
};

static struct medicine_capsule_vertices green_capsule_vertices = {
#include "chaostgkinder/medicine_models/green_capsule_vertices.inc"
};

static NJS_CNK_MODEL green_capsule_mesh = {
#include "chaostgkinder/medicine_models/green_capsule_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_2 = {
#include "chaostgkinder/medicine_models/medicine_model_2.inc"
};

static struct medicine_capsule_polygons lavender_capsule_polygons = {
#include "chaostgkinder/medicine_models/lavender_capsule_polygons.inc"
};

static struct medicine_capsule_vertices lavender_capsule_vertices = {
#include "chaostgkinder/medicine_models/lavender_capsule_vertices.inc"
};

static NJS_CNK_MODEL lavender_capsule_mesh = {
#include "chaostgkinder/medicine_models/lavender_capsule_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_3 = {
#include "chaostgkinder/medicine_models/medicine_model_3.inc"
};

static struct medicine_tablet_polygons white_tablet_polygons = {
#include "chaostgkinder/medicine_models/white_tablet_polygons.inc"
};

static struct medicine_tablet_vertices white_tablet_vertices = {
#include "chaostgkinder/medicine_models/white_tablet_vertices.inc"
};

static NJS_CNK_MODEL white_tablet_mesh = {
#include "chaostgkinder/medicine_models/white_tablet_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_4 = {
#include "chaostgkinder/medicine_models/medicine_model_4.inc"
};

static struct medicine_tablet_polygons cyan_tablet_polygons = {
#include "chaostgkinder/medicine_models/cyan_tablet_polygons.inc"
};

static struct medicine_tablet_vertices cyan_tablet_vertices = {
#include "chaostgkinder/medicine_models/cyan_tablet_vertices.inc"
};

static NJS_CNK_MODEL cyan_tablet_mesh = {
#include "chaostgkinder/medicine_models/cyan_tablet_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_5 = {
#include "chaostgkinder/medicine_models/medicine_model_5.inc"
};

static struct medicine_tablet_polygons peach_tablet_polygons = {
#include "chaostgkinder/medicine_models/peach_tablet_polygons.inc"
};

static struct medicine_tablet_vertices peach_tablet_vertices = {
#include "chaostgkinder/medicine_models/peach_tablet_vertices.inc"
};

static NJS_CNK_MODEL peach_tablet_mesh = {
#include "chaostgkinder/medicine_models/peach_tablet_mesh.inc"
};

NJS_CNK_OBJECT medicine_model_6 = {
#include "chaostgkinder/medicine_models/medicine_model_6.inc"
};
