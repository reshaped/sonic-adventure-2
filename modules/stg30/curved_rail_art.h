#ifndef STG30_CURVED_RAIL_ART_H
#define STG30_CURVED_RAIL_ART_H
#include <ninja.h>

#include "draw/texture_anim.h"
#include "stage/path.h"

extern const char curve_et64_3[8];
extern const char curve_miu128_fr003[16];
extern const char curve_miu256_fr002[16];
extern const char curve_miu32_fr001[12];
extern const char curve_miu64_fr004[12];
extern const char curve_miu64_fr007[12];
extern const char curve_space64_1[10];
extern const char curve_miu64_ee002[12];

extern NJS_TEXLIST curved_rail_texlist;
extern NJS_TEXLIST curved_rail_b_texlist;
extern NJS_CNK_OBJECT curved_rail_object;
extern NJS_CNK_OBJECT curved_rail_b_object;
extern NJS_OBJECT curved_rail_collision;
/* @hack type: the second collision object declared as an array of one,
 * where a single object would do */
extern NJS_OBJECT curved_rail_b_collision[1];
extern struct texture_anim curved_rail_anim;
extern struct path curved_rail_path;

#endif
