#include <ninja.h>
#include <sg_xpt.h>

#include "stg22/fixture_models.h"
#include "stg22/prop_textures.h"

static const char miu32_cg006[12] = {
#include "stg22/sign_textures/miu32_cg006.inc"
};
static const char miu64_cg022[12] = {
#include "stg22/sign_textures/miu64_cg022.inc"
};
static const char miu64_cg023[12] = {
#include "stg22/sign_textures/miu64_cg023.inc"
};
static const char miu64_cg024[12] = {
#include "stg22/sign_textures/miu64_cg024.inc"
};

struct gadget_sign_frame gadget_sign_frames[GADGET_LIGHT_FRAMES] = {
#include "stg22/sign_textures/gadget_sign_frames.inc"
};

/* The three illuminated sign shapes using the cycling texture slots. */

static Sint16 gadget_sign_b_node_4_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_b_node_4_polygons.inc"
};

static Sint32 gadget_sign_b_node_4_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_b_node_4_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_b_node_4_mesh = {
#include "stg22/sign_textures/gadget_sign_b_node_4_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_b_node_4 = {
#include "stg22/sign_textures/gadget_sign_b_node_4.inc"
};

static Sint16 gadget_sign_b_node_3_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_b_node_3_polygons.inc"
};

static Sint32 gadget_sign_b_node_3_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_b_node_3_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_b_node_3_mesh = {
#include "stg22/sign_textures/gadget_sign_b_node_3_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_b_node_3 = {
#include "stg22/sign_textures/gadget_sign_b_node_3.inc"
};

static Sint16 gadget_sign_b_node_2_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_b_node_2_polygons.inc"
};

static Sint32 gadget_sign_b_node_2_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_b_node_2_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_b_node_2_mesh = {
#include "stg22/sign_textures/gadget_sign_b_node_2_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_b_node_2 = {
#include "stg22/sign_textures/gadget_sign_b_node_2.inc"
};

static Sint16 gadget_sign_b_node_1_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_b_node_1_polygons.inc"
};

static Sint32 gadget_sign_b_node_1_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_b_node_1_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_b_node_1_mesh = {
#include "stg22/sign_textures/gadget_sign_b_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_b_node_1 = {
#include "stg22/sign_textures/gadget_sign_b_node_1.inc"
};

static Sint16 gadget_sign_b_polygons[91] = {
#include "stg22/sign_textures/gadget_sign_b_polygons.inc"
};

static Sint32 gadget_sign_b_vertices[99] = {
#include "stg22/sign_textures/gadget_sign_b_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_b_mesh = {
#include "stg22/sign_textures/gadget_sign_b_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_b = {
#include "stg22/sign_textures/gadget_sign_b.inc"
};

static NJS_TEXNAME gadget_sign_c_textures[2] = {
#include "stg22/sign_textures/gadget_sign_c_textures.inc"
};

static NJS_TEXLIST gadget_sign_c_texlist = {
#include "stg22/sign_textures/gadget_sign_c_texlist.inc"
};

static Sint16 gadget_sign_c_node_2_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_c_node_2_polygons.inc"
};

static Sint32 gadget_sign_c_node_2_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_c_node_2_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_c_node_2_mesh = {
#include "stg22/sign_textures/gadget_sign_c_node_2_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_c_node_2 = {
#include "stg22/sign_textures/gadget_sign_c_node_2.inc"
};

static Sint16 gadget_sign_c_node_1_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_c_node_1_polygons.inc"
};

static Sint32 gadget_sign_c_node_1_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_c_node_1_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_c_node_1_mesh = {
#include "stg22/sign_textures/gadget_sign_c_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_c_node_1 = {
#include "stg22/sign_textures/gadget_sign_c_node_1.inc"
};

static Sint16 gadget_sign_c_polygons[39] = {
#include "stg22/sign_textures/gadget_sign_c_polygons.inc"
};

static Sint32 gadget_sign_c_vertices[35] = {
#include "stg22/sign_textures/gadget_sign_c_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_c_mesh = {
#include "stg22/sign_textures/gadget_sign_c_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_c = {
#include "stg22/sign_textures/gadget_sign_c.inc"
};

static NJS_TEXNAME gadget_sign_d_textures[2] = {
#include "stg22/sign_textures/gadget_sign_d_textures.inc"
};

static NJS_TEXLIST gadget_sign_d_texlist = {
#include "stg22/sign_textures/gadget_sign_d_texlist.inc"
};

static Sint16 gadget_sign_d_node_2_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_d_node_2_polygons.inc"
};

static Sint32 gadget_sign_d_node_2_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_d_node_2_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_d_node_2_mesh = {
#include "stg22/sign_textures/gadget_sign_d_node_2_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_d_node_2 = {
#include "stg22/sign_textures/gadget_sign_d_node_2.inc"
};

static Sint16 gadget_sign_d_node_1_polygons[25] = {
#include "stg22/sign_textures/gadget_sign_d_node_1_polygons.inc"
};

static Sint32 gadget_sign_d_node_1_vertices[19] = {
#include "stg22/sign_textures/gadget_sign_d_node_1_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_d_node_1_mesh = {
#include "stg22/sign_textures/gadget_sign_d_node_1_mesh.inc"
};

static NJS_CNK_OBJECT gadget_sign_d_node_1 = {
#include "stg22/sign_textures/gadget_sign_d_node_1.inc"
};

static Sint16 gadget_sign_d_polygons[39] = {
#include "stg22/sign_textures/gadget_sign_d_polygons.inc"
};

static Sint32 gadget_sign_d_vertices[35] = {
#include "stg22/sign_textures/gadget_sign_d_vertices.inc"
};

static NJS_CNK_MODEL gadget_sign_d_mesh = {
#include "stg22/sign_textures/gadget_sign_d_mesh.inc"
};

NJS_CNK_OBJECT gadget_sign_d = {
#include "stg22/sign_textures/gadget_sign_d.inc"
};
