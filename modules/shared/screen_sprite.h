/* screen sprites: draws of sprites and of whole pictures on the screen.
 * OTHERPRINT.PRS, EMBLEMGET.PRS and CHAOSTGRACE.PRS each link this source */
#ifndef SHARED_SCREEN_SPRITE_H
#define SHARED_SCREEN_SPRITE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a sprite of one cell, drawn where pos.x and pos.y give, at priority pos.z.
 * the other fields are those of NJS_SPRITE and njDrawSprite2D's call */
struct screen_sprite {
    NJS_TEXLIST *texlist;
    NJS_TEXANIM *tanim;
    Int cell; /* the cell of tanim */
    NJS_POINT3 pos;
    Float sx, sy;
    Angle ang;
    Uint32 attr;
};

/* draw sprite's cell with njDrawSprite2D */
void screen_sprite_draw(struct screen_sprite *sprite);
/* move *value by step toward target, and not past it. it returns TRUE where
 * *value was not target, and FALSE, changing nothing, where it was. step must
 * not be negative */
Bool float_step_toward(Float *value, Float target, Float step);
/* draw the sprites that follow at level, a constant material of level in
 * each channel. a sprite takes it under NJD_SPRITE_COLOR. the call saves the
 * constant attributes with draw_attr_save, for screen_draw_end */
void screen_draw_begin(Float level);
/* end what screen_draw_begin began. the constant material goes back to 0 in
 * each channel. the constant attributes go back to what the begin saved */
void screen_draw_end(void);
/* draw a picture of 640 by 512 pixels from the screen's top left, at
 * priority pri. its textures are texid to texid + 7 of texlist. the first
 * four are squares of 256 pixels. texid and texid + 1 lie down the left, and
 * texid + 2 and texid + 3 beside them. the last four are squares of 128
 * pixels down the right edge, from the top. each draws with
 * NJD_SPRITE_COLOR and NJD_SPRITE_ALPHA. attr isn't read */
void screen_picture_draw(Int texid, NJS_TEXLIST *texlist, Float pri,
                         Uint32 attr);

#endif
