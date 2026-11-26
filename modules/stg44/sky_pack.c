#include <ninja.h>

#include "shared/space_sky_textures.h"
#include "stg44/sky.h"
#include "stg44/sky_textures.h"

/* the texture list of bgtex44, the background's PVM. the sky's own lists share
 * the names this file exports. a sized array pads its name, so the name after
 * it starts on a 4-byte boundary */
const char sky_earth_texture[12] = {
#include "stg44/sky_pack/sky_earth_texture.inc"
};
const char mad_space_et_64_3[8] = {
#include "stg44/sky_pack/mad_space_et_64_3.inc"
};
const char mad_space_meteor_128_3[12] = {
#include "stg44/sky_pack/mad_space_meteor_128_3.inc"
};
const char mad_space_sky_128_17[12] = {
#include "stg44/sky_pack/mad_space_sky_128_17.inc"
};
static const char sky_128_3[12] = {
#include "stg44/sky_pack/sky_128_3.inc"
};
static const char sky_128_9[12] = {
#include "stg44/sky_pack/sky_128_9.inc"
};
static const char sky_256_1[12] = {
#include "stg44/sky_pack/sky_256_1.inc"
};
const char mad_space_sky_32_4[12] = {
#include "stg44/sky_pack/mad_space_sky_32_4.inc"
};
const char mad_space_sky_32_7[12] = {
#include "stg44/sky_pack/mad_space_sky_32_7.inc"
};
static const char sky_512_1[12] = {
#include "stg44/sky_pack/sky_512_1.inc"
};
const char mad_space_sky_512_2[12] = {
#include "stg44/sky_pack/mad_space_sky_512_2.inc"
};
static const char sky_64_18[12] = {
#include "stg44/sky_pack/sky_64_18.inc"
};
const char mad_space_sky_64_19[12] = {
#include "stg44/sky_pack/mad_space_sky_64_19.inc"
};
const char sky_stars_20_texture[12] = {
#include "stg44/sky_pack/sky_stars_20_texture.inc"
};
static const char sky_64_9[12] = {
#include "stg44/sky_pack/sky_64_9.inc"
};
const char mad_space_nebula_128[12] = {
#include "stg44/sky_pack/mad_space_nebula_128.inc"
};
const char sky_orbit_texture[12] = {
#include "stg44/sky_pack/sky_orbit_texture.inc"
};
const char sky_cloud_texture[9] = {
#include "stg44/sky_pack/sky_cloud_texture.inc"
};

static NJS_TEXNAME textures[18] = {
#include "stg44/sky_pack/textures.inc"
};
NJS_TEXLIST mad_space_sky_texlist = {
#include "stg44/sky_pack/mad_space_sky_texlist.inc"
};
