/* The lower and upper BLOCK2 portals each own four texture slots. Both
 * banks borrow the names exported by the lower portal's material member. */
#ifndef SHARED_LIFT_TEXTURES_H
#define SHARED_LIFT_TEXTURES_H
#include <ninja.h>

extern const char core_lift_cg001[16];
extern const char core_lift_cg022[16];
extern const char core_lift_cc002[16];
extern const char core_lift_poison[15];

static NJS_TEXNAME textures[4] = {
#include "shared/lift_textures/textures.inc"
};

#endif
