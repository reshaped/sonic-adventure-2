#include <ninja.h>

#include "shared/patrol_camera_tex.h"

static const char miu128_cg003[16] = {
#include "shared/camera_v_tex/miu128_cg003.inc"
};

static const char miu128_cg007[16] = {
#include "shared/camera_v_tex/miu128_cg007.inc"
};

static const char miu128_cg010[16] = {
#include "shared/camera_v_tex/miu128_cg010.inc"
};

const char camera_miu32_ee001[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu32_ee001";
const char camera_miu64_cg004[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu64_cg004";
const char camera_miu64_cg008[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu64_cg008";
const char camera_miu64_cg010[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu64_cg010";
const char camera_miu64_cg020[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu64_cg020";
const char camera_miu64_ee001[PATROL_CAMERA_TEXTURE_NAME_SIZE] = "miu64_ee001";

static NJS_TEXNAME textures[9] = {
#include "shared/camera_v_tex/textures.inc"
};

NJS_TEXLIST vertical_camera_texlist = {
#include "shared/camera_v_tex/vertical_camera_texlist.inc"
};

#include "shared/camera_art/vertical_camera_light.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/camera_art/engine_head_polygons.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/camera_art/vertical_camera.h"

NJS_CNK_OBJECT vertical_camera_object[1] = {
#include "shared/camera_v_tex/vertical_camera_object.inc"
};
