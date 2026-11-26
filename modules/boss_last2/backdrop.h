/* the models of the Finalhazard's backdrop, their texture lists, and the tables
 * texture_load_shared takes for them */
#ifndef BOSS_LAST2_BACKDROP_H
#define BOSS_LAST2_BACKDROP_H

#include <ninja.h>
#include "system/texture.h"

#ifdef NONMATCHING
/* whether the sky's first texture is named, which this side's stage tests
 * before it loads the sky */
Bool finalhazard_backdrop_available(void);
#endif
extern struct texture_pvm finalhazard_sky_pvms[];
extern NJS_TEXLIST **finalhazard_sky_groups[];
extern NJS_CNK_OBJECT finalhazard_earth;
extern NJS_TEXLIST finalhazard_earth_texlist;
/* @hack type: the planet's node declared as an array of one, where the node
 * alone would do */
extern NJS_CNK_OBJECT finalhazard_planet[1];
extern NJS_TEXLIST finalhazard_planet_texlist;
extern NJS_CNK_OBJECT finalhazard_sky;
extern NJS_TEXLIST finalhazard_sky_texlist;

#endif
