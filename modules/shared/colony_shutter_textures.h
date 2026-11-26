/* the far face's texture list, in the shutter of Lost Colony and Eternal Engine
 */
#ifndef SHARED_COLONY_SHUTTER_TEXTURES_H
#define SHARED_COLONY_SHUTTER_TEXTURES_H
#include <ninja.h>

/* the names that the far face's six textures share with the shutter's eleven.
 * both texture lists point at these same strings */
extern const char shutter_miu256_cg004[16];
extern const char shutter_miu256_cg005[16];
extern const char shutter_miu256_cg006[16];
extern const char shutter_miu32_cg001[12];
extern const char shutter_miu64_cg009[12];
extern const char shutter_miu64_cg019[12];
extern NJS_TEXLIST core_shutter_far_texlist;

#endif
