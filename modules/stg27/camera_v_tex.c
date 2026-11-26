/* Lost Colony uses eight material names on its vertical camera. */
#include <ninja.h>

#include "shared/patrol_camera_tex.h"

static const char miu128_cg007[16] = {
#include "stg27/camera_v_tex/miu128_cg007.inc"
};

static const char miu128_cg010[16] = {
#include "stg27/camera_v_tex/miu128_cg010.inc"
};

const char camera_miu32_ee001[12] = {
#include "stg27/camera_v_tex/camera_miu32_ee001.inc"
};

const char camera_miu64_cg004[12] = {
#include "stg27/camera_v_tex/camera_miu64_cg004.inc"
};

const char camera_miu64_cg008[12] = {
#include "stg27/camera_v_tex/camera_miu64_cg008.inc"
};

const char camera_miu64_cg010[12] = {
#include "stg27/camera_v_tex/camera_miu64_cg010.inc"
};

const char camera_miu64_cg020[12] = {
#include "stg27/camera_v_tex/camera_miu64_cg020.inc"
};

const char camera_miu64_ee001[12] = {
#include "stg27/camera_v_tex/camera_miu64_ee001.inc"
};

static NJS_TEXNAME textures[8] = {
#include "stg27/camera_v_tex/textures.inc"
};

NJS_TEXLIST vertical_camera_texlist = {
#include "stg27/camera_v_tex/vertical_camera_texlist.inc"
};

#include "shared/camera_art/vertical_camera_light.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/camera_art/colony_head_polygons.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/camera_art/vertical_camera.h"

NJS_CNK_OBJECT vertical_camera_object[1] = {
#include "stg27/camera_v_tex/vertical_camera_object.inc"
};
