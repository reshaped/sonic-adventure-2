/* the menus' draws: a list's rows, a row of sprites, and a backdrop that
 * covers the screen */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/vector.h"

/* the backdrop's textures. the first BIG_TILES are BIG_SIZE pixels a side,
 * and the rest SMALL_SIZE */
#define TILES 8
#define BIG_TILES 4
#define BIG_SIZE 256
#define SMALL_SIZE 128
#define TILE_VERTICES 4
/* the last u and v of a cell that spans its whole texture. a cell counts
 * 256 of each across a texture */
#define UV_END 255
/* the vertices' color without MENU_BACKDROP_TINT, opaque white */
#define UNTINTED 0xffffffff
/* menu_backdrop_textures_draw's flag that draws the backdrop translucent */
#define MENU_BACKDROP_TRANSLUCENT 0x2

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* menu_lerp for x alone, and for x and y */
void menu_lerp_x(const struct menu_lerp *lerp);
void menu_lerp_xy(const struct menu_lerp *lerp);
/* menu_backdrop_textures_draw at (0, 0, priority) */
void menu_backdrop_textures_draw_at(Int first, NJS_TEXLIST *texlist, Int flags,
                                    Float priority);
/* NOLINTEND(misc-use-internal-linkage) */

/* each texture's top left corner, from the backdrop's */
struct tile_corner {
    Sint16 x, y;
};

/* the colors of menu_backdrop_textures_draw's vertices under
 * MENU_BACKDROP_TINT, by game_state.theme */
static Uint32 tile_colors[MENU_THEMES] = {
    0xffffffff, 0xff19a0a0, 0xff3cd2ff, 0xff808080, 0xffff9000,
    0xffff5000, 0xff64b420, 0xffffa0f5, 0xffa00000, 0xff00d2b4,
    0xff0080ff, 0xffffc800, 0xffc0c0c0,
};
/* clang-format off */
static struct tile_corner tile_corners[TILES] = {
    {0, 0},
    {0, 256},
    {256, 0},
    {256, 256},
    {512, 0},
    {512, 128},
    {512, 256},
    {512, 384},
};
/* clang-format on */
/* the backdrop's quads, as menu_backdrop_textures_draw draws them. it sets
 * each vertex's z and color before the draw */
static NJS_TEXTURE_VTX tile_quads[TILES][TILE_VERTICES] = {
#include "advertise/menu_draw/tile_quads.inc"
};

void menu_lerp_x(const struct menu_lerp *lerp)
{
    lerp->out->x =
        ((lerp->time - lerp->from_time) * (lerp->to->x - lerp->from->x) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->x;
}

void menu_lerp_xy(const struct menu_lerp *lerp)
{
    lerp->out->x =
        ((lerp->time - lerp->from_time) * (lerp->to->x - lerp->from->x) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->x;
    lerp->out->y =
        ((lerp->time - lerp->from_time) * (lerp->to->y - lerp->from->y) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->y;
}

void menu_scroll_step(const struct menu_scroll *scroll)
{
    if (scroll->target == 0) {
        *scroll->top = 0;
        *scroll->cursor = 0;
    } else if (scroll->target < *scroll->top + *scroll->cursor) {
        --*scroll->cursor;
        if (*scroll->cursor < 0) {
            *scroll->cursor = 0;
            --*scroll->top;
        }
    } else if (scroll->target > *scroll->top + *scroll->cursor) {
        ++*scroll->cursor;
        if (*scroll->cursor >= scroll->shown) {
            *scroll->cursor = scroll->shown - 1;
            ++*scroll->top;
        }
    }
}

void menu_list_draw(const struct menu_list *list,
                    const struct menu_scroll *scroll, Float scale)
{
    NJS_POINT3 pos;
    Int language;
    Int row;
    Int i;

    pos = list->pos;
    for (i = 0; i < scroll->shown; i++) {
        row = *scroll->top + i;
        if (row >= list->count) {
            break;
        }
        /* @hack spelling: the language read in the test of the cursor's
         * row, where a statement of its own before the test would do */
        if (language = game_state.text_language, i == *scroll->cursor) {
            if (list->rows[row].lit >= 0) {
                /* @hack spelling: the offset times a comparison, here and
                 * for a dim row, where a conditional expression would do */
                list->draw(list->rows[row].lit +
                               (list->language_offset * (language > 0)),
                           scale, &pos, MENU_SPRITE_ATTR);
            }
            pos.z -= 1.0F;
            if (list->lit_back >= 0) {
                list->draw(list->lit_back, scale, &pos, MENU_SPRITE_ATTR);
            }
        } else {
            if (list->rows[row].dim >= 0) {
                list->draw(list->rows[row].dim +
                               (list->language_offset * (language > 0)),
                           1.0F, &pos, MENU_SPRITE_ATTR);
            }
            pos.z -= 1.0F;
            if (list->dim_back >= 0) {
                list->draw(list->dim_back, 1.0F, &pos, MENU_SPRITE_ATTR);
            }
        }
        pos.z = list->pos.z;
        vector_sum(&pos, &list->step, &pos);
    }
}

static void menu_backdrop_draw_at(Int first, NJS_TEXLIST *texlist,
                                  const NJS_POINT3 *pos, Int flags)
{
    /* @hack spelling: the locals declared kept, cell and sprite, where
     * another order would do */
    NJS_ARGB kept;
    NJS_TEXANIM cell;
    NJS_SPRITE sprite;
    Int i;

    for (i = 0; i < TILES; i++) {
        if (i < BIG_TILES) {
            cell.sx = BIG_SIZE;
            cell.sy = BIG_SIZE;
        } else {
            cell.sx = SMALL_SIZE;
            cell.sy = SMALL_SIZE;
        }
        cell.cx = 0;
        cell.cy = 0;
        cell.u1 = 0;
        cell.v1 = 0;
        cell.u2 = UV_END;
        cell.v2 = UV_END;
        cell.texid = first + i;
        sprite.tlist = texlist;
        sprite.tanim = &cell;
        sprite.ang = 0;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        sprite.p.x = tile_corners[i].x + pos->x;
        sprite.p.y = tile_corners[i].y + pos->y;
        if (flags & MENU_BACKDROP_TINT) {
            kept = _nj_constant_material_;
            constant_material_set(kept.a, menu_color.r, menu_color.g,
                                  menu_color.b);
        }
        njDrawSprite2D(&sprite, 0, pos->z, MENU_SPRITE_ATTR);
        if (flags & MENU_BACKDROP_TINT) {
            _nj_constant_material_ = kept;
        }
    }
}

void menu_backdrop_draw(Int first, NJS_TEXLIST *texlist, Float priority,
                        Int flags)
{
    NJS_POINT3 pos;

    pos.x = 0.0F;
    pos.y = 0.0F;
    pos.z = priority;
    menu_backdrop_draw_at(first, texlist, &pos, flags);
}

void menu_sprite_row_draw(const struct menu_sprite_row *row)
{
    NJS_POINT3 pos;
    /* @hack type: the counter as a Sint32, where an Int would do */
    Sint32 i;

    pos = *row->pos;
    for (i = 0; i < row->count; i++) {
        row->draw(row->first + i, 1.0F, &pos, MENU_SPRITE_ATTR);
#ifdef NONMATCHING
        /* this side repairs the read past tanim that the tags at
         * options_title_draw's and story_select_draw's calls describe. the
         * matching side reads the next cell's width after the last cell too,
         * for a pos that nothing reads again. this side stops there */
        if (i + 1 == row->count) {
            break;
        }
#endif
        /* each half width is rounded down to a whole pixel */
        /* NOLINTBEGIN(bugprone-integer-division) */
        pos.x += (row->tanim[row->first + i].sx / 2) +
                 (row->tanim[row->first + i + 1].sx / 2) + 1;
        /* NOLINTEND(bugprone-integer-division) */
    }
}

static void menu_backdrop_textures_draw(Int first, NJS_TEXLIST *texlist,
                                        const NJS_POINT3 *pos, Int flags)
{
    Int i;
    Int j;

    for (i = 0; i < TILES; i++) {
        for (j = 0; j < TILE_VERTICES; j++) {
            tile_quads[i][j].z = -1.0F / pos->z;
            if (flags & MENU_BACKDROP_TINT) {
                tile_quads[i][j].col = tile_colors[game_state.theme];
            } else {
                tile_quads[i][j].col = UNTINTED;
            }
        }
    }
    njSetTexture(texlist);
    for (i = 0; i < TILES; i++) {
        if (flags & MENU_BACKDROP_TRANSLUCENT) {
            njDrawTexture(tile_quads[i], TILE_VERTICES, first + i, TRUE);
        } else {
            njDrawTexture(tile_quads[i], TILE_VERTICES, first + i, FALSE);
        }
    }
}

void menu_backdrop_textures_draw_at(Int first, NJS_TEXLIST *texlist, Int flags,
                                    Float priority)
{
    NJS_POINT3 pos;

    pos.x = 0.0F;
    pos.y = 0.0F;
    pos.z = priority;
    menu_backdrop_textures_draw(first, texlist, &pos, flags);
}
