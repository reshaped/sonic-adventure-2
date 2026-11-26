/* the story select's panels. each step, the task moves the view toward
 * what story_select_work asks for */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/story_select_draw.h"
#include "advertise/synopsis.h"
#include "camera/viewport.h"
#include "shared/message_sprite.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* destroy the task, where story_select_draw_create made one that this
 * hasn't destroyed. nothing calls story_select_draw_end, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void story_select_draw_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
/* a fade runs from 0 to 1 in FADE_STEPS steps */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)

/* the frame's top left corner. the other three corners draw it flipped */
#define FRAME_CELL 0
#define MARK_CELL 1 /* the cursor's mark on the list */
#define PANEL_CELL 4 /* story's panel is the cell PANEL_CELL + story */
/* the hero story's first chapter row. each story's rows follow the last's */
#define CHAPTER_CELL (PANEL_CELL + STORY_SELECT_STORIES)
/* the up arrow, and the down under NJD_SPRITE_VFLIP */
#define ARROW_CELL 41
#define TITLE_CELL 42 /* the title's first cell of TITLE_CELLS */
#define TITLE_CELLS 3
#define BACKDROP_TEXTURE 4
#define BACKDROP_PRIORITY (-10000.0F)

/* places' columns: where a panel stands while two panels show, while three
 * show, and once it is chosen */
#define PLACE_TWO 0
#define PLACE_THREE 1
#define PLACE_CHOSEN 2
#define PLACES 3
/* each step, a panel moves this share of what is left of its way */
#define MOVE_SHARE 0.1F
/* the frame's corners draw at these offsets from the place of the panel
 * it frames */
#define FRAME_LEFT (-6.0F)
#define FRAME_TOP (-6.0F)
#define FRAME_RIGHT 81.0F
#define FRAME_BOTTOM 121.0F
#define FRAME_PRIORITY (-9.0F)

/* a list's rows lie ROW_HEIGHT apart. its window shows ROWS_SHOWN of them,
 * and scrolls SCROLL_STEP pixels a step */
#define ROW_HEIGHT 80.0F
#define ROWS_SHOWN 3
#define SCROLL_STEP 8.0F
/* where the window's first row draws, and the cursor's mark while it is
 * on that row */
#define LIST_X 300.0F
#define LIST_Y 100.0F
#define LIST_PRIORITY (-1000.0F)
#define MARK_X 292.0F
#define MARK_Y 92.0F
#define MARK_PRIORITY (-999.0F)
/* a chapter's name draws NAME_X and NAME_Y from its row. a name of two
 * lines draws NAME_RAISE higher, to center it on the row */
#define NAME_X 90.0F
#define NAME_Y 20.0F
#define NAME_RAISE 16.0F
#define LINE_BREAK '\n'
/* a labeled panel draws the message LABEL_MESSAGE + story, LABEL_X and
 * LABEL_Y from the panel's top left corner */
#define LABEL_X 10.0F
#define LABEL_Y 250.0F
#define LABEL_PRIORITY (-5.0F)
#define LABEL_MESSAGE 55

#define ARROW_X 324.0F
#define ARROW_ABOVE_Y 60.0F
#define ARROW_BELOW_Y 330.0F
#define HELP_X 168.0F
#define HELP_Y 400.0F
#define FRONT_PRIORITY (-100.0F)
/* the help to choose a story, and the help to choose a chapter, as
 * indexes of help_messages */
#define HELP_STORIES 0
#define HELP_CHAPTERS 1
#define HELPS 2
#define TEXTURES 16
#define CELLS 45

/* a story's panel as the task draws it. top is the chapter in the window's
 * first row, and row the cursor's row in the window. scroll moves toward
 * top times ROW_HEIGHT, in pixels. more_above and more_below are TRUE while
 * chapters lie above or below the window */
struct panel_view {
    Float fade;
    Float list_fade;
    NJS_POINT3 pos;
    Sint32 top;
    Float scroll;
    Sint32 row;
    Bool more_above;
    Bool more_below;
};
/* the task's view of story_select_work */
struct story_select_view {
    Float backdrop_fade;
    struct panel_view panels[STORY_SELECT_STORIES];
};
/* the view */
static struct story_select_view story_select_view;

/* clang-format off */
static struct story_select_view view_start = {
    0.0F,
    {{0.0F, 0.0F, {0.0F, 0.0F, 0.0F}, 0, 0.0F, 0, 0, 0},
     {0.0F, 0.0F, {0.0F, 0.0F, 0.0F}, 0, 0.0F, 0, 0, 0},
     {0.0F, 0.0F, {0.0F, 0.0F, 0.0F}, 0, 0.0F, 0, 0, 0}},
};
/* clang-format on */
static struct task *story_select_task = NULL;
static Sint32 help_messages[HELPS] = {65, 66};
KEPT static NJS_POINT3 places[STORY_SELECT_STORIES][PLACES] = {
#include "advertise/story_select_draw/places.inc"
};
/* by story, each chapter's name, as a message */
static Sint32 hero_messages[STORY_SELECT_HERO_CHAPTERS] = {
#include "advertise/story_select_draw/hero_messages.inc"
};
static Sint32 dark_messages[STORY_SELECT_DARK_CHAPTERS] = {
#include "advertise/story_select_draw/dark_messages.inc"
};
static Sint32 last_messages[STORY_SELECT_LAST_CHAPTERS] = {
#include "advertise/story_select_draw/last_messages.inc"
};
KEPT static Sint32 *chapter_messages[STORY_SELECT_STORIES] = {
#include "advertise/story_select_draw/chapter_messages.inc"
};
/* the area a list of n chapters draws in is list_areas[n - 1]. a list of
 * ROWS_SHOWN or more takes the last */
static struct viewport_area list_areas[ROWS_SHOWN] = {
#include "advertise/story_select_draw/list_areas.inc"
};
/* by story, the cell of its first chapter's row. the other chapters' cells
 * follow in order */
static Sint32 first_chapter_cells[STORY_SELECT_STORIES] = {
    CHAPTER_CELL,
    CHAPTER_CELL + STORY_SELECT_HERO_CHAPTERS,
    CHAPTER_CELL + STORY_SELECT_HERO_CHAPTERS + STORY_SELECT_DARK_CHAPTERS,
};
/* where the middle of the title's first cell draws */
static NJS_POINT3 title_pos = {
#include "advertise/story_select_draw/title_pos.inc"
};
#include "advertise/story_select_draw/text.inc"

static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/story_select_draw/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/story_select_draw/texlist.inc"
};
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/story_select_draw/cells.inc"
};

static void story_select_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                     Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void story_select_arrow_draw(Bool below)
{
    NJS_POINT3 pos;

    pos.x = ARROW_X;
    pos.z = FRONT_PRIORITY;
    /* @hack spelling: the draw in each arm, where one after the if with
     * the attributes in a local would do */
    if (below) {
        pos.y = ARROW_BELOW_Y;
        story_select_sprite_draw(ARROW_CELL, 1.0F, &pos,
                                 NJD_SPRITE_VFLIP | NJD_SPRITE_ALPHA);
    } else {
        pos.y = ARROW_ABOVE_Y;
        story_select_sprite_draw(ARROW_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
    }
}

static void story_select_help_draw(Int help)
{
    NJS_POINT3 pos;

    pos.x = HELP_X;
    pos.y = HELP_Y;
    pos.z = FRONT_PRIORITY;
    message_sprite_draw(help_messages[help], &pos, 1.0F);
}

/* move the cursor of story's list a row toward chapter. the window moves a
 * row where the cursor would leave it. the cursor goes to the first chapter
 * at once. it goes to the last at once too, except in a list of
 * ROWS_SHOWN - 1 chapters. the scroll then steps toward the window's top.
 *
 * it returns TRUE where the scroll moved, and FALSE otherwise */
static Bool story_select_list_scroll(Int story, Int chapter, Int chapters)
{
    if (chapter == 0) {
        story_select_view.panels[story].top = 0;
        story_select_view.panels[story].row = 0;
    } else if (chapter == chapters - 1 && chapters != ROWS_SHOWN - 1) {
        story_select_view.panels[story].top = chapters - ROWS_SHOWN;
        story_select_view.panels[story].row = ROWS_SHOWN - 1;
    } else if (chapter < story_select_view.panels[story].row +
                             story_select_view.panels[story].top) {
        story_select_view.panels[story].row--;
        if (story_select_view.panels[story].row < 0) {
            story_select_view.panels[story].row = 0;
            story_select_view.panels[story].top--;
        }
    } else if (chapter > story_select_view.panels[story].row +
                             story_select_view.panels[story].top) {
        story_select_view.panels[story].row++;
        if (story_select_view.panels[story].row >= ROWS_SHOWN) {
            story_select_view.panels[story].row = ROWS_SHOWN - 1;
            story_select_view.panels[story].top++;
        }
    }
    return menu_approach(&story_select_view.panels[story].scroll,
                         story_select_view.panels[story].top * ROW_HEIGHT,
                         SCROLL_STEP);
}

static void story_select_panel_move(Int story, Bool last_shown, Bool chosen,
                                    NJS_POINT3 *pos)
{
    Int place;

    if (chosen) {
        place = PLACE_CHOSEN;
    } else if (last_shown) {
        place = PLACE_THREE;
    } else {
        place = PLACE_TWO;
    }
    menu_approach(
        &pos->x, places[story][place].x,
        _builtin_fabsf((places[story][place].x - pos->x) * MOVE_SHARE));
    menu_approach(
        &pos->y, places[story][place].y,
        _builtin_fabsf((places[story][place].y - pos->y) * MOVE_SHARE));
    pos->z = places[story][place].z;
}

/* frame story's place, among two panels or three, and draw the help to
 * choose a story */
static void story_select_frame_draw(Int story, Bool last_shown)
{
    NJS_POINT3 pos;
    Int place;

    if (last_shown) {
        place = PLACE_THREE;
    } else {
        place = PLACE_TWO;
    }
    pos.x = places[story][place].x + FRAME_LEFT;
    pos.y = places[story][place].y + FRAME_TOP;
    pos.z = FRAME_PRIORITY;
    story_select_sprite_draw(FRAME_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
    pos.x = places[story][place].x + FRAME_RIGHT;
    pos.y = places[story][place].y + FRAME_TOP;
    story_select_sprite_draw(FRAME_CELL, 1.0F, &pos,
                             NJD_SPRITE_HFLIP | NJD_SPRITE_ALPHA);
    pos.x = places[story][place].x + FRAME_LEFT;
    pos.y = places[story][place].y + FRAME_BOTTOM;
    story_select_sprite_draw(FRAME_CELL, 1.0F, &pos,
                             NJD_SPRITE_VFLIP | NJD_SPRITE_ALPHA);
    pos.x = places[story][place].x + FRAME_RIGHT;
    pos.y = places[story][place].y + FRAME_BOTTOM;
    story_select_sprite_draw(FRAME_CELL, 1.0F, &pos,
                             NJD_SPRITE_HFLIP | NJD_SPRITE_VFLIP |
                                 NJD_SPRITE_ALPHA);
    story_select_help_draw(HELP_STORIES);
}

/* draw chapter's name on its row at *row. a chapter past story's last
 * draws nothing */
static void story_select_chapter_name_draw(Int story, Int chapter,
                                           NJS_POINT3 *row)
{
    NJS_POINT3 pos;

    switch (story) {
    case STORY_HERO:
        if (chapter >= STORY_SELECT_HERO_CHAPTERS) {
            return;
        }
        break;
    case STORY_DARK:
        if (chapter >= STORY_SELECT_DARK_CHAPTERS) {
            return;
        }
        break;
    case STORY_LAST:
        if (chapter >= STORY_SELECT_LAST_CHAPTERS) {
            return;
        }
        break;
    }
    pos = *row;
    pos.x += NAME_X;
    pos.y += NAME_Y;
    pos.z = LIST_PRIORITY;
    /* @hack spelling: the draw in each arm, where one after the raise
     * would do */
    if (message_sprite_count(chapter_messages[story][chapter], LINE_BREAK)) {
        pos.y -= NAME_RAISE;
        message_sprite_draw(chapter_messages[story][chapter], &pos, 1.0F);
    } else {
        message_sprite_draw(chapter_messages[story][chapter], &pos, 1.0F);
    }
}

static void story_select_label_draw(Int story, Bool last_shown)
{
    NJS_POINT3 pos;

    /* @hack redundant: a test of last_shown with two empty arms */
    if (last_shown) {
    } else {
    }
    pos = story_select_view.panels[story].pos;
    pos.x += LABEL_X;
    pos.y += LABEL_Y;
    pos.z = LABEL_PRIORITY;
    message_sprite_draw(story + LABEL_MESSAGE, &pos, 1.0F);
}

/* keep the area for a list of chapters, then set Ninja's screen to part, a
 * VIEWPORT_ part. a list of 0 chapters keeps list_areas[-1], unchecked */
static void story_select_list_viewport(Int part, Int chapters)
{
    /* @hack spelling: the call in each arm, where one after the if would
     * do */
    if (chapters > ROWS_SHOWN - 1) {
        viewport_set_area(&list_areas[ROWS_SHOWN - 1]);
    } else {
        viewport_set_area(&list_areas[chapters - 1]);
    }
    viewport_set_screen(part);
}

static void story_select_draw_step(struct task *task)
{
    Bool last_shown;
    Int i;

    if (!story_select_work.shown) {
        story_select_work.backdrop.shown = FALSE;
        story_select_work.panels[STORY_HERO].shown = FALSE;
        story_select_work.panels[STORY_DARK].shown = FALSE;
        story_select_work.panels[STORY_LAST].shown = FALSE;
    }
    for (i = 0; i < STORY_SELECT_STORIES; i++) {
        if (!story_select_work.panels[i].shown) {
            story_select_work.panels[i].list.shown = FALSE;
        }
    }
    story_select_work.backdrop.moving = FALSE;
    if (story_select_work.backdrop.shown) {
        if (menu_approach(&story_select_view.backdrop_fade, 1.0F, FADE_STEP)) {
            story_select_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&story_select_view.backdrop_fade, 0.0F, FADE_STEP)) {
            story_select_work.backdrop.moving = TRUE;
        }
    }
    last_shown = story_select_work.panels[STORY_LAST].shown;
    for (i = 0; i < STORY_SELECT_STORIES; i++) {
        story_select_work.panels[i].fading = FALSE;
        if (story_select_work.panels[i].shown) {
            if (menu_approach(&story_select_view.panels[i].fade, 1.0F,
                              FADE_STEP)) {
                story_select_work.panels[i].fading = TRUE;
            }
        } else {
            if (menu_approach(&story_select_view.panels[i].fade, 0.0F,
                              FADE_STEP)) {
                story_select_work.panels[i].fading = TRUE;
            }
        }
        story_select_panel_move(i, last_shown,
                                story_select_work.panels[i].chosen,
                                &story_select_view.panels[i].pos);
        story_select_work.panels[i].list.moving = FALSE;
        if (story_select_work.panels[i].list.shown) {
            if (menu_approach(&story_select_view.panels[i].list_fade, 1.0F,
                              FADE_STEP)) {
                story_select_work.panels[i].list.moving = TRUE;
            }
        } else {
            if (menu_approach(&story_select_view.panels[i].list_fade, 0.0F,
                              FADE_STEP)) {
                story_select_work.panels[i].list.moving = TRUE;
            }
        }
        story_select_work.panels[i].list.moving |=
            story_select_list_scroll(i, story_select_work.panels[i].chapter,
                                     story_select_work.panels[i].chapters);
        if (story_select_view.panels[i].top == 0) {
            story_select_view.panels[i].more_above = FALSE;
        } else {
            story_select_view.panels[i].more_above = TRUE;
        }
        if (story_select_view.panels[i].top >=
            story_select_work.panels[i].chapters - ROWS_SHOWN) {
            story_select_view.panels[i].more_below = FALSE;
        } else {
            story_select_view.panels[i].more_below = TRUE;
        }
    }
    story_select_work.moving =
        story_select_work.backdrop.moving |
        story_select_work.panels[STORY_HERO].fading |
        story_select_work.panels[STORY_DARK].fading |
        story_select_work.panels[STORY_LAST].fading |
        story_select_work.panels[STORY_HERO].list.moving |
        story_select_work.panels[STORY_DARK].list.moving |
        story_select_work.panels[STORY_LAST].list.moving;
}

static void story_select_draw(struct task *task)
{
    Int i;

    if (story_select_view.backdrop_fade != 0.0F) {
        struct menu_sprite_row title;

        menu_fade_begin(story_select_view.backdrop_fade);
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                           MENU_BACKDROP_TINT);
        title.tanim = cells;
        title.first = TITLE_CELL;
        title.count = TITLE_CELLS;
        title.pos = &title_pos;
        title.draw = story_select_sprite_draw;
        /* @bug memory: the title's last cell is the last of cells, so
         * menu_sprite_row_draw reads the width of cells[CELLS], past their
         * end: the call at 0x8c5076f4 and the read at 0x8c500ac2. nothing
         * draws at the place that width gives */
        menu_sprite_row_draw(&title);
        menu_tint_end();
    }
    if (story_select_work.framed) {
        story_select_frame_draw(story_select_work.story,
                                story_select_work.panels[STORY_LAST].shown);
    }
    for (i = 0; i < STORY_SELECT_STORIES; i++) {
        NJS_POINT3 pos;

        /* @hack spelling: the fade tested alone, here and for the list
         * below, where a compare with 0.0F would do */
        if (story_select_view.panels[i].fade) {
            menu_fade_begin(story_select_view.panels[i].fade);
            story_select_sprite_draw(i + PANEL_CELL, 1.0F,
                                     &story_select_view.panels[i].pos,
                                     MENU_SPRITE_ATTR);
            menu_tint_end();
            /* @hack redundant: a second draw_attr_restore, after
             * menu_tint_end's */
            draw_attr_restore();
        }
        if (story_select_work.panels[i].labeled) {
            story_select_label_draw(i,
                                    story_select_work.panels[STORY_LAST].shown);
        }
        if (story_select_view.panels[i].list_fade) {
            /* while the scroll moves, a row past the window's edge can show.
             * so the list draws the row above the window and the row below
             * it too. at the list's top the window has no row above it, and
             * at its bottom none below. a chapter's name draws only once the
             * list's fade reaches 1 */
            pos.x = LIST_X;
            pos.y = LIST_Y;
            pos.z = LIST_PRIORITY;
            story_select_list_viewport(VIEWPORT_AREA,
                                       story_select_work.panels[i].chapters);
            menu_fade_begin(story_select_view.panels[i].list_fade);
            if (story_select_view.panels[i].top == 0) {
                pos.y += (story_select_view.panels[i].top * ROW_HEIGHT) -
                         story_select_view.panels[i].scroll;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             1,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 1, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             2,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 2, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             3,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 3, &pos);
                }
            } else if (story_select_view.panels[i].top ==
                       story_select_work.panels[i].chapters - ROWS_SHOWN) {
                pos.y += ((story_select_view.panels[i].top - 1) * ROW_HEIGHT) -
                         story_select_view.panels[i].scroll;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top -
                                             1,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top - 1, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             1,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 1, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             2,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 2, &pos);
                }
            } else {
                pos.y += ((story_select_view.panels[i].top - 1) * ROW_HEIGHT) -
                         story_select_view.panels[i].scroll;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top -
                                             1,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top - 1, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             1,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 1, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             2,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 2, &pos);
                }
                pos.y += ROW_HEIGHT;
                story_select_sprite_draw(first_chapter_cells[i] +
                                             story_select_view.panels[i].top +
                                             3,
                                         1.0F, &pos, MENU_SPRITE_ATTR);
                if (story_select_view.panels[i].list_fade == 1.0F) {
                    story_select_chapter_name_draw(
                        i, story_select_view.panels[i].top + 3, &pos);
                }
            }
            menu_tint_end();
            /* @bug memory: to give back the whole screen after each list,
             * the call passes 0 chapters, so it keeps list_areas[-1], the 32
             * bytes before the table, as the area (0x8c507c34, 0x8c507442) */
#ifdef NONMATCHING
            /* this side keeps the tag's result by defined means. it passes 1
             * chapter and keeps list_areas[0], where C leaves the read of
             * list_areas[-1] undefined. VIEWPORT_WHOLE reads no area, and each
             * list keeps its own area before it draws */
            story_select_list_viewport(VIEWPORT_WHOLE, 1);
#else
            story_select_list_viewport(VIEWPORT_WHOLE, 0);
#endif
            if (story_select_work.panels[i].marked) {
                pos.x = MARK_X;
                pos.y = (story_select_view.panels[i].row * ROW_HEIGHT) + MARK_Y;
                pos.z = MARK_PRIORITY;
                story_select_sprite_draw(MARK_CELL, 1.0F, &pos,
                                         NJD_SPRITE_ALPHA);
                story_select_help_draw(HELP_CHAPTERS);
                if (story_select_view.panels[i].more_above) {
                    story_select_arrow_draw(FALSE);
                }
                if (story_select_view.panels[i].more_below) {
                    story_select_arrow_draw(TRUE);
                }
            }
        }
    }
}

static void story_select_title_center(void)
{
    Int i;

    for (i = TITLE_CELL; i <= TITLE_CELL + TITLE_CELLS - 1; i++) {
        cells[i].cx = cells[i].sx / 2;
        cells[i].cy = cells[i].sy / 2;
    }
}

static void story_select_draw_destroy(struct task *task)
{
    story_select_release();
}

void story_select_draw_create(void)
{
    struct task *task;
    Uint8 *byte;
    Int left;

    task = task_create(TASK_WORK, TASK_LEVEL, story_select_draw_step,
                       "storyMainExec");
    task->draw = story_select_draw;
    task->destructor = story_select_draw_destroy;
    story_select_task = task;
    byte = (Uint8 *)&story_select_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, here and in story_select_draw_reset, where byte[i] for an
     * unsigned i counting up would do */
    for (left = sizeof(story_select_work); left != 0; left--) {
        *byte++ = 0;
    }
    story_select_draw_reset();
    story_select_title_center();
}

void story_select_draw_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    Int left;

    loaded = story_select_work.loaded;
    byte = (Uint8 *)&story_select_work;
    for (left = sizeof(story_select_work); left != 0; left--) {
        *byte++ = 0;
    }
    story_select_view = view_start;
    story_select_work.loaded = loaded;
}

void story_select_draw_end(void)
{
    if (story_select_task) {
        task_destroy(story_select_task);
        story_select_task = NULL;
    }
}

void story_select_load(void)
{
    if (!story_select_work.loaded) {
        texture_load_pvm(STORY_SELECT_ARCHIVE, &texlist);
        story_select_work.loaded = TRUE;
    }
}

void story_select_release(void)
{
    if (story_select_work.loaded) {
        njReleaseTexture(&texlist);
        story_select_work.loaded = FALSE;
    }
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}
