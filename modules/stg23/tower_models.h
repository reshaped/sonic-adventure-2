#ifndef STG23_TOWER_MODELS_H
#define STG23_TOWER_MODELS_H

#include <ninja.h>

extern NJS_OBJECT tower_collision;
extern NJS_CNK_MODEL tower_fragment_0;
extern NJS_CNK_MODEL tower_fragment_1;
extern NJS_CNK_MODEL tower_fragment_2;
extern NJS_CNK_MODEL tower_fragment_3;
extern NJS_CNK_MODEL tower_fragment_4;
extern NJS_CNK_MODEL tower_fragment_5;
/* @hack type: the motion declared as an array of one, where the motion alone
 * would do */
extern NJS_MOTION tower_motion[1];
extern NJS_CNK_OBJECT tower_platform;
extern NJS_CNK_MODEL tower_platform_model;
extern NJS_CNK_MODEL tower_support_model;

#endif
