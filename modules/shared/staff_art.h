/* the staff roll's glyphs, and each story's photograph cues. staff_roll_load
 * measures each glyph's width from its cell. staff_photo_draw sets
 * staff_photo_cell's texture to the photograph's before each draw */
#ifndef SHARED_STAFF_ART_H
#define SHARED_STAFF_ART_H
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/staff_roll.h"

struct staff_glyph {
    char character;
    Sint32 cell;
    Float width;
};
/* from frame on, the roll shows photograph photo, or none where photo is -1 */
struct staff_cue {
    Sint32 frame, photo;
};

extern struct staff_glyph staff_glyphs[STAFF_GLYPHS];
extern const struct staff_cue *staff_photo_cues[2];
extern NJS_TEXANIM staff_photo_cell;

#endif
