/* screen sprites: draws of sprites and of whole pictures on the screen.
 * OTHERPRINT.PRS, EMBLEMGET.PRS and CHAOSTGRACE.PRS each link this source */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "shared/screen_sprite.h"
#include "system/util.h"

/* a picture's textures. the first PICTURE_LARGE_TILES are squares of
 * LARGE_TILE pixels, and the rest squares of SMALL_TILE */
#define PICTURE_TILES 8
#define PICTURE_LARGE_TILES 4
#define LARGE_TILE 256
#define SMALL_TILE 128
/* the last u and v of a cell that takes its whole texture */
#define WHOLE_TEXTURE 255

/* where each of a picture's textures draws, in pixels from the picture's top
 * left. nothing writes the table. it isn't const: the image holds it in
 * section D, among the initialized data. SHC puts a const table in
 * section C */
static struct {
    Sint16 x, y;
} picture_tiles[PICTURE_TILES] = {
    {0, 0},
    {0, LARGE_TILE},
    {LARGE_TILE, 0},
    {LARGE_TILE, LARGE_TILE},
    {2 * LARGE_TILE, 0},
    {2 * LARGE_TILE, SMALL_TILE},
    {2 * LARGE_TILE, 2 * SMALL_TILE},
    {2 * LARGE_TILE, 3 * SMALL_TILE},
};

void screen_sprite_draw(struct screen_sprite *sprite)
{
    NJS_SPRITE drawn;

    drawn.tlist = sprite->texlist;
    drawn.tanim = sprite->tanim;
    drawn.sx = sprite->sx;
    drawn.sy = sprite->sy;
    drawn.p.x = sprite->pos.x;
    drawn.p.y = sprite->pos.y;
    drawn.ang = sprite->ang;
    njDrawSprite2D(&drawn, sprite->cell, sprite->pos.z, sprite->attr);
}

Bool float_step_toward(Float *value, Float target, Float step)
{
    if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
        return TRUE;
    } else if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
        return TRUE;
    }
    return FALSE;
}

/* int_step_toward and screen_draw_begin_color aren't static, though nothing
 * calls them and no header declares them. SHC leaves out a static function
 * that nothing calls, and the image holds both */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* do as float_step_toward does, on a Sint32 */
Bool int_step_toward(Sint32 *value, Sint32 target, Sint32 step)
{
    if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
        return TRUE;
    } else if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
        return TRUE;
    }
    return FALSE;
}
/* NOLINTEND(misc-use-internal-linkage) */

void screen_draw_begin(Float level)
{
    draw_attr_save();
    constant_material_set(level, level, level, level);
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* screen_draw_begin, with each channel of the constant material its own */
void screen_draw_begin_color(Float alpha, Float red, Float green, Float blue)
{
    draw_attr_save();
    constant_material_set(alpha, red, green, blue);
}
/* NOLINTEND(misc-use-internal-linkage) */

void screen_draw_end(void)
{
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

/* screen_picture_draw at pos, whose z is the priority */
static void screen_picture_draw_at(Int texid, NJS_TEXLIST *texlist,
                                   NJS_POINT3 *pos, Uint32 attr)
{
    /* @hack redundant: unused, a local of 16 bytes that nothing reads */
    Uint8 unused[16];
    NJS_TEXANIM cell;
    NJS_SPRITE sprite;
    Int i;

    for (i = 0; i < PICTURE_TILES; i++) {
        if (i < PICTURE_LARGE_TILES) {
            cell.sx = LARGE_TILE;
            cell.sy = LARGE_TILE;
        } else {
            cell.sx = SMALL_TILE;
            cell.sy = SMALL_TILE;
        }
        cell.cx = 0;
        cell.cy = 0;
        cell.u1 = 0;
        cell.v1 = 0;
        cell.u2 = WHOLE_TEXTURE;
        cell.v2 = WHOLE_TEXTURE;
        cell.texid = texid + i;
        sprite.tlist = texlist;
        sprite.tanim = &cell;
        sprite.ang = 0;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        sprite.p.x = picture_tiles[i].x + pos->x;
        sprite.p.y = picture_tiles[i].y + pos->y;
        njDrawSprite2D(&sprite, 0, pos->z, NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA);
    }
}

void screen_picture_draw(Int texid, NJS_TEXLIST *texlist, Float pri,
                         Uint32 attr)
{
    NJS_POINT3 pos;

    pos.x = 0.0F;
    pos.y = 0.0F;
    pos.z = pri;
    screen_picture_draw_at(texid, texlist, &pos, attr);
}
