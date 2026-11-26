/* the grid of sea below the course and the dome of sky above it */
#ifndef CART_BACKGROUND_ART_H
#define CART_BACKGROUND_ART_H
#include <ninja.h>

extern const char cart_floor_texture_filenames[];
/* @hack type: the sea grid's node declared as an array, where the node alone
 * would do */
extern NJS_CNK_OBJECT cart_floor[];
extern NJS_CNK_OBJECT cart_sky;
extern NJS_TEXLIST cart_floor_textures, cart_sky_textures;
#endif
