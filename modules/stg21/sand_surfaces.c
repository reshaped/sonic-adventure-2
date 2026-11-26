/* Sand Ocean's sand: each surface is a model of colored vertices and a
 * texture_anim that scrolls its texture. each anim's uvs list holds one key,
 * laid out as texture_anim.h says */

#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "stg21/background_art.h"
#include "stg21/sand_surfaces/text.inc"

struct sand_start_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[825];
};

static struct sand_start_surface_data sand_start_surface = {
#include "stg21/sand_surfaces/sand_start_surface.inc"
};

static Sint32 ocean_sand_start_vertices[463] = {
#include "stg21/sand_surfaces/ocean_sand_start_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_start_model = {
#include "stg21/sand_surfaces/ocean_sand_start_model.inc"
};

NJS_CNK_OBJECT ocean_sand_start[1] = {
#include "stg21/sand_surfaces/ocean_sand_start.inc"
};

static Sint16 ocean_sand_start_uvs[596] = {
#include "stg21/sand_surfaces/ocean_sand_start_uvs.inc"
};

struct texture_anim ocean_sand_start_anim = {
#include "stg21/sand_surfaces/ocean_sand_start_anim.inc"
};

struct sand_2200_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[75];
};

static struct sand_2200_surface_data sand_2200_surface = {
#include "stg21/sand_surfaces/sand_2200_surface.inc"
};

static Sint32 ocean_sand_2200_vertices[63] = {
#include "stg21/sand_surfaces/ocean_sand_2200_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_2200_model = {
#include "stg21/sand_surfaces/ocean_sand_2200_model.inc"
};

NJS_CNK_OBJECT ocean_sand_2200[1] = {
#include "stg21/sand_surfaces/ocean_sand_2200.inc"
};

static Sint16 ocean_sand_2200_uvs[48] = {
#include "stg21/sand_surfaces/ocean_sand_2200_uvs.inc"
};

struct texture_anim ocean_sand_2200_anim = {
#include "stg21/sand_surfaces/ocean_sand_2200_anim.inc"
};

struct sand_3970_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[75];
};

static struct sand_3970_surface_data sand_3970_surface = {
#include "stg21/sand_surfaces/sand_3970_surface.inc"
};

static Sint32 ocean_sand_3970_vertices[63] = {
#include "stg21/sand_surfaces/ocean_sand_3970_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_3970_model = {
#include "stg21/sand_surfaces/ocean_sand_3970_model.inc"
};

NJS_CNK_OBJECT ocean_sand_3970[1] = {
#include "stg21/sand_surfaces/ocean_sand_3970.inc"
};

static Sint16 ocean_sand_3970_uvs[48] = {
#include "stg21/sand_surfaces/ocean_sand_3970_uvs.inc"
};

struct texture_anim ocean_sand_3970_anim = {
#include "stg21/sand_surfaces/ocean_sand_3970_anim.inc"
};

struct sand_6830_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[113];
};

static struct sand_6830_surface_data sand_6830_surface = {
#include "stg21/sand_surfaces/sand_6830_surface.inc"
};

static Sint32 ocean_sand_6830_vertices[87] = {
#include "stg21/sand_surfaces/ocean_sand_6830_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_6830_model = {
#include "stg21/sand_surfaces/ocean_sand_6830_model.inc"
};

NJS_CNK_OBJECT ocean_sand_6830[1] = {
#include "stg21/sand_surfaces/ocean_sand_6830.inc"
};

static Sint16 ocean_sand_6830_uvs[76] = {
#include "stg21/sand_surfaces/ocean_sand_6830_uvs.inc"
};

struct texture_anim ocean_sand_6830_anim = {
#include "stg21/sand_surfaces/ocean_sand_6830_anim.inc"
};

struct sand_4250_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[261];
};

static struct sand_4250_surface_data sand_4250_surface = {
#include "stg21/sand_surfaces/sand_4250_surface.inc"
};

static Sint32 ocean_sand_4250_vertices[159] = {
#include "stg21/sand_surfaces/ocean_sand_4250_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_4250_model = {
#include "stg21/sand_surfaces/ocean_sand_4250_model.inc"
};

NJS_CNK_OBJECT ocean_sand_4250[1] = {
#include "stg21/sand_surfaces/ocean_sand_4250.inc"
};

static Sint16 ocean_sand_4250_uvs[182] = {
#include "stg21/sand_surfaces/ocean_sand_4250_uvs.inc"
};

struct texture_anim ocean_sand_4250_anim = {
#include "stg21/sand_surfaces/ocean_sand_4250_anim.inc"
};

struct sand_3500_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[1091];
};

static struct sand_3500_surface_data sand_3500_surface = {
#include "stg21/sand_surfaces/sand_3500_surface.inc"
};

static Sint32 ocean_sand_3500_vertices[583] = {
#include "stg21/sand_surfaces/ocean_sand_3500_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_3500_model = {
#include "stg21/sand_surfaces/ocean_sand_3500_model.inc"
};

NJS_CNK_OBJECT ocean_sand_3500[1] = {
#include "stg21/sand_surfaces/ocean_sand_3500.inc"
};

static Sint16 ocean_sand_3500_uvs[788] = {
#include "stg21/sand_surfaces/ocean_sand_3500_uvs.inc"
};

struct texture_anim ocean_sand_3500_anim = {
#include "stg21/sand_surfaces/ocean_sand_3500_anim.inc"
};

struct sand_end_surface_data {
    NJS_TEXNAME texture;
    NJS_TEXLIST texlist;
    Sint16 polygons[75];
};

static struct sand_end_surface_data sand_end_surface = {
#include "stg21/sand_surfaces/sand_end_surface.inc"
};

static Sint32 ocean_sand_end_vertices[51] = {
#include "stg21/sand_surfaces/ocean_sand_end_vertices.inc"
};

static NJS_CNK_MODEL ocean_sand_end_model = {
#include "stg21/sand_surfaces/ocean_sand_end_model.inc"
};

NJS_CNK_OBJECT ocean_sand_end[1] = {
#include "stg21/sand_surfaces/ocean_sand_end.inc"
};

static Sint16 ocean_sand_end_uvs[48] = {
#include "stg21/sand_surfaces/ocean_sand_end_uvs.inc"
};

struct texture_anim ocean_sand_end_anim = {
#include "stg21/sand_surfaces/ocean_sand_end_anim.inc"
};
