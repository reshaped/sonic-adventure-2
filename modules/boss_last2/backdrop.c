/* the Finalhazard's backdrop: the ARK, which finalhazard_sky holds, and two
 * models of Earth. no code reads the orbit's model */
#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last2/backdrop.h"
#include "boss_last2/stars.h"
#include "system/texture.h"

#include "boss_last2/backdrop/text.inc"

/* The first PVM texture name also identifies whether this backdrop is present.
 */
static const char first_space_texture[5] = {
#include "boss_last2/backdrop/first_space_texture.inc"
};

#ifdef NONMATCHING
/* whether first_space_texture names a texture. the disc-free build leaves it
 * zeros, and this side's stage then loads no sky */
Bool finalhazard_backdrop_available(void)
{
    return first_space_texture[0] != '\0';
}
#endif

static NJS_TEXNAME space_textures[32] = {
#include "boss_last2/backdrop/space_textures.inc"
};

static NJS_TEXLIST space_texlist = {
    space_textures, sizeof(space_textures) / sizeof(space_textures[0])};

static NJS_TEXNAME orbit_textures[1] = {
#include "boss_last2/backdrop/orbit_textures.inc"
};

static NJS_TEXLIST orbit_texlist = {
    orbit_textures, sizeof(orbit_textures) / sizeof(orbit_textures[0])};

static Sint16 sky_orbit_64_polygons[833] = {
#include "boss_last2/backdrop/sky_orbit_64_polygons.inc"
};

static Sint32 sky_orbit_64_vertices[459] = {
#include "boss_last2/backdrop/sky_orbit_64_vertices.inc"
};

static NJS_CNK_MODEL sky_orbit_64_mesh[1] = {
#include "boss_last2/backdrop/sky_orbit_64_mesh.inc"
};

static NJS_CNK_OBJECT orbit = {
#include "boss_last2/backdrop/orbit.inc"
};

static NJS_TEXNAME earth_textures[1] = {
#include "boss_last2/backdrop/earth_textures.inc"
};

NJS_TEXLIST finalhazard_earth_texlist = {
    earth_textures, sizeof(earth_textures) / sizeof(earth_textures[0])};

static Sint16 earth_polygons[833] = {
#include "boss_last2/backdrop/earth_polygons.inc"
};

static Sint32 earth_vertices[459] = {
#include "boss_last2/backdrop/earth_vertices.inc"
};

static NJS_CNK_MODEL earth_mesh[1] = {
#include "boss_last2/backdrop/earth_mesh.inc"
};

NJS_CNK_OBJECT finalhazard_earth = {
#include "boss_last2/backdrop/finalhazard_earth.inc"
};

static NJS_TEXNAME planet_textures[2] = {
#include "boss_last2/backdrop/planet_textures.inc"
};

NJS_TEXLIST finalhazard_planet_texlist = {
    planet_textures, sizeof(planet_textures) / sizeof(planet_textures[0])};

static Sint16 sky_earth_polygons[1285] = {
#include "boss_last2/backdrop/sky_earth_polygons.inc"
};

static Sint32 sky_earth_vertices[863] = {
#include "boss_last2/backdrop/sky_earth_vertices.inc"
};

static NJS_CNK_MODEL sky_earth_mesh[1] = {
#include "boss_last2/backdrop/sky_earth_mesh.inc"
};

NJS_CNK_OBJECT finalhazard_planet[1] = {
#include "boss_last2/backdrop/finalhazard_planet.inc"
};

static NJS_TEXNAME ark_textures[10] = {
#include "boss_last2/backdrop/ark_textures.inc"
};

NJS_TEXLIST finalhazard_sky_texlist = {
    ark_textures, sizeof(ark_textures) / sizeof(ark_textures[0])};

static Sint16 ark_polygons[1505] = {
#include "boss_last2/backdrop/ark_polygons.inc"
};

static Sint32 ark_vertices[947] = {
#include "boss_last2/backdrop/ark_vertices.inc"
};

static NJS_CNK_MODEL ark_mesh[1] = {
#include "boss_last2/backdrop/ark_mesh.inc"
};

NJS_CNK_OBJECT finalhazard_sky = {
#include "boss_last2/backdrop/finalhazard_sky.inc"
};

static NJS_TEXLIST *layers[] = {
    &finalhazard_planet_texlist, &finalhazard_earth_texlist, &orbit_texlist,
    &finalhazard_sky_texlist,    &finalhazard_star_texlist,  NULL};

NJS_TEXLIST **finalhazard_sky_groups[] = {layers, NULL};

struct texture_pvm finalhazard_sky_pvms[] = {
    {BACKGROUND_ARCHIVE, &space_texlist}, {NULL, NULL}};
