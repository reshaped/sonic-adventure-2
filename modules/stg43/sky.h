/* Cosmic Wall's sky: its four models and their builds. texture_load_shared
 * fills the models' texture lists from background_pvms, which names bgtex43 */
#ifndef STG43_SKY_H
#define STG43_SKY_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/common_object.h"
#include "system/texture.h"

extern Sint32 background_built;
extern struct model_build background_builds[4];
extern NJS_TEXLIST **background_texture_groups[];
extern struct texture_pvm background_pvms[];

extern NJS_TEXLIST cosmic_wall_sky_texlist;
extern NJS_CNK_OBJECT background_0_object;
extern NJS_CNK_OBJECT background_1_object;
extern NJS_CNK_OBJECT background_2_object;
extern NJS_CNK_OBJECT background_3_object;

#endif
