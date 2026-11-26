/* the fake title's draw: a task that fades and draws the backdrop and pages
 * as fake_title.h's works ask */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/fake_title.h"
#include "advertise/fake_title_draw.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "draw/lines.h"
#include "main.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define BACKDROP_TEXTURE 0
#define BACKDROP_PRIORITY (-10000.0F)
/* cells. a page's rows are the FAKE_TITLE_ROWS cells from page *
 * FAKE_TITLE_ROWS, but FAKE_TITLE_TENTO's are those from TENTO_CELL. the
 * wide cursor draws WIDE_CURSOR_CELL twice, the second mirrored to the
 * right of the first */
#define NARROW_CURSOR_CELL 12
#define WIDE_CURSOR_CELL 13
#define PROMPT_CELL 14
#define TENTO_CELL 16
/* the pages up to LAST_NARROW_PAGE have rows at most 196 pixels wide. they
 * draw at narrow_rows under the narrow cursor, and the later pages at
 * wide_rows under the wide cursor */
#define LAST_NARROW_PAGE FAKE_TITLE_LIST_1
/* a cursor draws this far up and left of its row */
#define CURSOR_OFFSET 8.0F
/* a row that can't be chosen draws at this level */
#define REFUSED_LEVEL 0.3F
#define SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR | NJD_SPRITE_ANGLE)

/* @hack type: a page's fade as a record of one Float, where a Float would
 * do */
struct page_fade {
    Float level;
};
/* the fades of the backdrop, of each page and of the cover, from 0 to 1 */
struct fake_title_fades {
    Float backdrop;
    struct page_fade pages[FAKE_TITLE_PAGES];
    Float cover;
};
/* the fades */
static struct fake_title_fades fake_title_fades;

static NJS_TEXANIM cells[19] = {
#include "advertise/fake_title_draw/cells.inc"
};
static struct fake_title_fades fades_start = {
    0.0F,
    {{0.0F}, {0.0F}, {0.0F}, {0.0F}, {0.0F}},
    0.0F,
};
static NJS_POINT3 prompt_pos = {
#include "advertise/fake_title_draw/prompt_pos.inc"
};
static NJS_POINT3 narrow_rows[FAKE_TITLE_ROWS] = {
#include "advertise/fake_title_draw/narrow_rows.inc"
};
static NJS_POINT3 wide_rows[FAKE_TITLE_ROWS] = {
#include "advertise/fake_title_draw/wide_rows.inc"
};
static Float dim_levels[2] = {0.5F, 0.3F};

static void fake_title_sprite_draw(Int cell, const NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &fake_title_texlist;
    sprite.tanim = cells;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* set the constant material for a row of page. a refused row is
 * REFUSED_LEVEL's gray at full alpha. any other takes the page's fade in
 * each channel */
static void fake_title_row_tint(Int page, Bool refused)
{
    if (refused) {
        constant_material_set(1.0F, REFUSED_LEVEL, REFUSED_LEVEL,
                              REFUSED_LEVEL);
    } else {
        constant_material_set(fake_title_fades.pages[page].level,
                              fake_title_fades.pages[page].level,
                              fake_title_fades.pages[page].level,
                              fake_title_fades.pages[page].level);
    }
}

static void fake_title_page_draw(Int page)
{
    if (page == FAKE_TITLE_TENTO) {
        fake_title_row_tint(page, fake_title_works.pages[page].refused[0]);
        fake_title_sprite_draw(TENTO_CELL, &wide_rows[0], SPRITE_ATTR);
        fake_title_row_tint(page, fake_title_works.pages[page].refused[1]);
        fake_title_sprite_draw(TENTO_CELL + 1, &wide_rows[1], SPRITE_ATTR);
        fake_title_row_tint(page, fake_title_works.pages[page].refused[2]);
        fake_title_sprite_draw(TENTO_CELL + 2, &wide_rows[2], SPRITE_ATTR);
    } else if (page <= LAST_NARROW_PAGE) {
        fake_title_sprite_draw(page * FAKE_TITLE_ROWS, &narrow_rows[0],
                               SPRITE_ATTR);
        fake_title_sprite_draw((page * FAKE_TITLE_ROWS) + 1, &narrow_rows[1],
                               SPRITE_ATTR);
        fake_title_sprite_draw((page * FAKE_TITLE_ROWS) + 2, &narrow_rows[2],
                               SPRITE_ATTR);
    } else {
        fake_title_sprite_draw(page * FAKE_TITLE_ROWS, &wide_rows[0],
                               SPRITE_ATTR);
        fake_title_sprite_draw((page * FAKE_TITLE_ROWS) + 1, &wide_rows[1],
                               SPRITE_ATTR);
        fake_title_sprite_draw((page * FAKE_TITLE_ROWS) + 2, &wide_rows[2],
                               SPRITE_ATTR);
    }
}

static void fake_title_cursor_draw(Int page)
{
    NJS_POINT3 pos;

    if (page == FAKE_TITLE_TENTO) {
        pos = wide_rows[fake_title_works.pages[page].row];
        pos.x -= CURSOR_OFFSET;
        pos.y -= CURSOR_OFFSET;
        pos.z += 1.0F;
        fake_title_sprite_draw(WIDE_CURSOR_CELL, &pos, NJD_SPRITE_ALPHA);
        pos.x += cells[WIDE_CURSOR_CELL].sx;
        fake_title_sprite_draw(WIDE_CURSOR_CELL, &pos,
                               NJD_SPRITE_ALPHA | NJD_SPRITE_HFLIP);
    } else if (page <= LAST_NARROW_PAGE) {
        pos = narrow_rows[fake_title_works.pages[page].row];
        pos.x -= CURSOR_OFFSET;
        pos.y -= CURSOR_OFFSET;
        pos.z += 1.0F;
        fake_title_sprite_draw(NARROW_CURSOR_CELL, &pos, NJD_SPRITE_ALPHA);
    } else {
        pos = wide_rows[fake_title_works.pages[page].row];
        pos.x -= CURSOR_OFFSET;
        pos.y -= CURSOR_OFFSET;
        pos.z += 1.0F;
        fake_title_sprite_draw(WIDE_CURSOR_CELL, &pos, NJD_SPRITE_ALPHA);
        pos.x += cells[WIDE_CURSOR_CELL].sx;
        fake_title_sprite_draw(WIDE_CURSOR_CELL, &pos,
                               NJD_SPRITE_ALPHA | NJD_SPRITE_HFLIP);
    }
}

static void fake_title_draw_step(struct task *task)
{
    Int i;

    fake_title_works.backdrop->fading = FALSE;
    if (!fake_title_works.backdrop->shown) {
        if (menu_approach(&fake_title_fades.backdrop, 0.0F, FADE_STEP)) {
            fake_title_works.backdrop->fading = TRUE;
        }
    } else if (!fake_title_works.backdrop->bright) {
        if (menu_approach(&fake_title_fades.backdrop,
                          dim_levels[fake_title_works.backdrop->dim],
                          FADE_STEP)) {
            fake_title_works.backdrop->fading = TRUE;
        }
    } else {
        if (menu_approach(&fake_title_fades.backdrop, 1.0F, FADE_STEP)) {
            fake_title_works.backdrop->fading = TRUE;
        }
    }
    if (fake_title_works.backdrop->fading) {
        fake_title_works.backdrop->prompt = FALSE;
    }
    for (i = 0; i < FAKE_TITLE_PAGES; ++i) {
        fake_title_works.pages[i].fading = FALSE;
        if (!fake_title_works.pages[i].shown) {
            fake_title_works.pages[i].cursor = FALSE;
            if (menu_approach(&fake_title_fades.pages[i].level, 0.0F,
                              FADE_STEP)) {
                fake_title_works.pages[i].fading = TRUE;
            }
        } else {
            if (menu_approach(&fake_title_fades.pages[i].level, 1.0F,
                              FADE_STEP)) {
                fake_title_works.pages[i].fading = TRUE;
            }
        }
        if (fake_title_works.pages[i].fading) {
            fake_title_works.pages[i].cursor = FALSE;
        }
    }
    fake_title_set.fading = FALSE;
    if (fake_title_set.shown) {
        if (menu_approach(&fake_title_fades.cover, 1.0F, FADE_STEP)) {
            fake_title_set.fading = TRUE;
        }
    } else {
        if (menu_approach(&fake_title_fades.cover, 0.0F, FADE_STEP)) {
            fake_title_set.fading = TRUE;
        }
    }
}

static void fake_title_draw(struct task *task)
{
    Int i;

    if (fake_title_fades.backdrop != 0.0F) {
        menu_fade_begin(fake_title_fades.backdrop);
        menu_backdrop_draw(BACKDROP_TEXTURE, &fake_title_texlist,
                           BACKDROP_PRIORITY, 0);
        menu_tint_end();
        if (fake_title_works.backdrop->prompt) {
            fake_title_sprite_draw(PROMPT_CELL, &prompt_pos, NJD_SPRITE_ALPHA);
        }
    }
    for (i = 0; i < FAKE_TITLE_PAGES; ++i) {
        /* @hack spelling: the fade tested alone, where a compare with 0.0F
         * would do */
        if (fake_title_fades.pages[i].level) {
            draw_attr_save();
            constant_material_set(fake_title_fades.pages[i].level,
                                  fake_title_fades.pages[i].level,
                                  fake_title_fades.pages[i].level,
                                  fake_title_fades.pages[i].level);
            fake_title_page_draw(i);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
        }
        if (fake_title_works.pages[i].cursor) {
            fake_title_cursor_draw(i);
        }
    }
    if (fake_title_fades.cover != 0.0F) {
        menu_fade_begin(fake_title_fades.cover);
        menu_tint_end();
    }
}

static void fake_title_draw_destroy(struct task *task)
{
    njReleaseTexture(&fake_title_texlist);
}

void fake_title_draw_create(void)
{
    struct task *task;
    Int i;

    task = task_create(TASK_WORK, TASK_LEVEL, fake_title_draw_step,
                       "niseTitleExec");
    task->draw = fake_title_draw;
    task->destructor = fake_title_draw_destroy;
    njGarbageTexture(texmemlist, TEXTURE_COUNT);
    texture_load_pvm("forcus", &fake_title_texlist);
    fake_title_works.backdrop->shown = FALSE;
    fake_title_works.backdrop->prompt = FALSE;
    for (i = 0; i < FAKE_TITLE_PAGES; ++i) {
        fake_title_works.pages[i].shown = FALSE;
        fake_title_works.pages[i].row = 0;
    }
    fake_title_fades = fades_start;
}
