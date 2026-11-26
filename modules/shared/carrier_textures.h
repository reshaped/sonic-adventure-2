/* the carrier stages' textures: the PVMs and groups for texture_load_shared,
 * and the sky's and sea's names */
#ifndef SHARED_CARRIER_TEXTURES_H
#define SHARED_CARRIER_TEXTURES_H
#include <ninja.h>

#include "system/texture.h"

extern struct texture_pvm carrier_pvms[];
extern NJS_TEXLIST **carrier_texture_groups[];

#define CARRIER_SEA_FRAMES 20
/* the sea's frames. background_texlist takes them from carrier_background_pvm
 * by index, after the sky's two textures, so they keep the PVM's order */
extern const char carrier_sea_names[CARRIER_SEA_FRAMES][12];
extern const char carrier_sky_clouds[12];
extern const char carrier_sky_horizon[8];
extern const char carrier_background_pvm[8];
extern NJS_TEXLIST carrier_sky_texlist;

#endif
