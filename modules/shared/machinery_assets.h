/* the fan and the two machinery models that Death Chamber and Egg Quarters each
 * define for themselves. the curve's and the straight's texture lists are
 * separate and both refer to the same three names */
#ifndef SHARED_MACHINERY_ASSETS_H
#define SHARED_MACHINERY_ASSETS_H
#include <ninja.h>

enum machinery_kind { MACHINERY_CURVE, MACHINERY_STRAIGHT, MACHINERY_KINDS };

extern NJS_TEXLIST fan_texlist;
extern NJS_CNK_MODEL fan_model;
extern NJS_TEXLIST machinery_curve_texlist;
extern NJS_TEXLIST machinery_straight_texlist;
extern NJS_CNK_MODEL machinery_curve_model;
extern NJS_CNK_MODEL machinery_straight_model;

extern const char machinery_texture_128a[20];
extern const char machinery_texture_64a[20];
extern const char machinery_texture_64b[18];

#endif
