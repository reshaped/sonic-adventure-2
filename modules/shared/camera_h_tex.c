/* Horizontal patrol camera base, rotating head and projected light. */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/patrol_camera_tex.h"

static const char miu64_cg014[12] = {
#include "shared/camera_h_tex/miu64_cg014.inc"
};

static const char miu128_cg011[16] = {
#include "shared/camera_h_tex/miu128_cg011.inc"
};

static const char miu128_cg017[16] = {
#include "shared/camera_h_tex/miu128_cg017.inc"
};

static const char miu64_cg006[12] = {
#include "shared/camera_h_tex/miu64_cg006.inc"
};

static NJS_TEXNAME textures[10] = {
#include "shared/camera_h_tex/textures.inc"
};

NJS_TEXLIST horizontal_camera_texlist = {
#include "shared/camera_h_tex/horizontal_camera_texlist.inc"
};

static Sint16 horizontal_camera_light_polygons[25] = {
#include "shared/camera_h_tex/horizontal_camera_light_polygons.inc"
};

static Sint32 horizontal_camera_light_vertices[19] = {
#include "shared/camera_h_tex/horizontal_camera_light_vertices.inc"
};

static NJS_CNK_MODEL horizontal_camera_light_model = {
#include "shared/camera_h_tex/horizontal_camera_light_model.inc"
};

static NJS_CNK_OBJECT horizontal_camera_light = {
#include "shared/camera_h_tex/horizontal_camera_light.inc"
};

static Sint16 horizontal_camera_head_polygons[155] = {
#include "shared/camera_h_tex/horizontal_camera_head_polygons.inc"
};

static Sint32 horizontal_camera_head_vertices[107] = {
#include "shared/camera_h_tex/horizontal_camera_head_vertices.inc"
};

static NJS_CNK_MODEL horizontal_camera_head_model = {
#include "shared/camera_h_tex/horizontal_camera_head_model.inc"
};

static NJS_CNK_OBJECT horizontal_camera_head = {
#include "shared/camera_h_tex/horizontal_camera_head.inc"
};

static Sint16 horizontal_camera_polygons[277] = {
#include "shared/camera_h_tex/horizontal_camera_polygons.inc"
};

static Sint32 horizontal_camera_vertices[219] = {
#include "shared/camera_h_tex/horizontal_camera_vertices.inc"
};

static NJS_CNK_MODEL horizontal_camera_model = {
#include "shared/camera_h_tex/horizontal_camera_model.inc"
};

NJS_CNK_OBJECT horizontal_camera_object[1] = {
#include "shared/camera_h_tex/horizontal_camera_object.inc"
};
