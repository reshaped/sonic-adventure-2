/* the options' panel. each wheel turns the short way round until its chosen
 * entry is at the front. an entry draws largest there */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_projection.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/options.h"
#include "advertise/options_draw.h"
#include "shared/message_sprite.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"

#include "advertise/options_draw/text.inc"

#define TASK_LEVEL 2
/* a fade runs from 0 to 1 in FADE_STEPS steps. the backdrop and the item
 * wheel fade to HALF_LEVEL while they aren't bright */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
#define HALF_LEVEL 0.5F
#define BACKDROP_TEXTURE 0
#define BACKDROP_PRIORITY (-10000.0F)
/* a list's row draws over LIT_BACK_CELL while the cursor is on it, and over
 * DIM_BACK_CELL otherwise. a language list's label draws over
 * LABEL_BACK_CELL. under Japanese text, the label is the cell
 * JAPANESE_LABEL_OFFSET on */
#define LIT_BACK_CELL 0
#define DIM_BACK_CELL 1
#define LABEL_BACK_CELL 2
#define VOICE_LABEL_CELL 25
#define TEXT_LABEL_CELL 26
#define JAPANESE_LABEL_OFFSET 2
/* where a label draws, and how far in front of its back its text draws */
#define LABEL_X 320.0F
#define LABEL_PRIORITY (-5.0F)
#define LABEL_LIFT 1.0F
/* an item's back is LIT_ITEM_CELL while the cursor rests on it, and
 * DIM_ITEM_CELL otherwise. its icon draws ICON_LIFT in front */
#define LIT_ITEM_CELL 29
#define DIM_ITEM_CELL 30
#define ICON_LIFT 0.1F
#define ITEM_HELP_POS {0.0F, 400.0F, -100.0F}
/* the left arrow, and the right under NJD_SPRITE_HFLIP */
#define ARROW_CELL 47
/* on the theme wheel, a lit theme n draws the cell LIT_THEME_CELL + n. a dim
 * one draws DIM_THEME_CELL + n */
#define LIT_THEME_CELL 61
#define DIM_THEME_CELL 48
#define TITLE_CELL 84
#define TITLE_CELLS 2
#define TITLE_X 100.0F
#define TITLE_Y 50.0F
#define TITLE_PRIORITY (-100.0F)
/* the sound list's rows are cells of Japanese text. under any other text
 * language, each row's cell is SOUND_LANGUAGE_OFFSET on */
#define SOUND_LANGUAGE_OFFSET 2
/* a wheel turns WHEEL_STEP degrees a step. its entries draw WHEEL_RISE
 * pixels above where its arc projects them. an entry's scale is
 * WHEEL_BACK_SCALE at the back, and grows by twice WHEEL_SCALE_SPAN to the
 * front. the larger an entry, the further in front it draws */
#define DEGREES_PER_TURN 360
#define WHEEL_STEP 3
#define WHEEL_RISE 180.0F
#define WHEEL_BACK_SCALE 0.8F
#define WHEEL_SCALE_SPAN 0.1F
#define WHEEL_PRIORITY_SCALE 100.0F
#define WHEEL_PRIORITY (-1000.0F)
#define LIST_ROWS 2 /* the rows of each list but the text list */
enum { ROW_STEREO, ROW_MONO };
enum { ROW_VIBRATION_ON, ROW_VIBRATION_OFF };
#define NO_PICTURE (-1) /* a theme with no picture of its own */
#define THEME_PICTURE_SCALE 2.0F
#define TEXTURES 41
#define THEME_TEXTURES 10
#define CELLS 86

/* an item on the item wheel: its icon's cell, and its help's message */
struct options_item_face {
    Sint32 icon;
    Sint32 help;
};
/* a wheel's entry: where it draws, with its priority in pos.z, and its
 * scale */
struct options_wheel_entry {
    NJS_POINT3 pos;
    Float scale;
};
/* the panel's view of options_work. each part has a fade from 0 to 1. each
 * wheel has a turn in degrees, and the places of its entries */
struct options_view {
    Float backdrop_fade;
    Float items_fade;
    Float sound_fade;
    Float vibration_fade;
    Float voice_fade;
    Float text_fade;
    Float themes_fade;
    Sint32 item_turn;
    Sint32 theme_turn;
    struct options_wheel_entry items[OPTIONS_ITEMS];
    /* an entry for each theme found, in the themes' order */
    struct options_wheel_entry themes[MENU_THEMES];
};

/* the view */
static struct options_view options_view;

static void options_draw_reset(void);

/* clang-format off */
static struct options_item_face item_faces[OPTIONS_ITEMS] = {
    {33, 22},
    {31, 26},
    {35, 24},
    {37, 23},
    {41, 25},
    {45, 27},
    {43, 28},
};
/* clang-format on */
static NJS_POINT3 dat_8c562c8c[12] = {
#include "advertise/options_draw/dat_8c562c8c.inc"
};
/* by theme: its picture's cell, drawn from theme_texlist, or NO_PICTURE */
static Sint32 theme_pictures[MENU_THEMES] = {
    NO_PICTURE, 74, 75, 76, NO_PICTURE, 77, 78, 79, 81, 80, 82, 83, NO_PICTURE,
};
/* the row each list's window starts at. nothing writes it, so each window
 * starts at its list's first row */
static Sint32 list_top = 0;
static struct options_view view_start = {0};
static struct task *panel_task = NULL;
static NJS_POINT3 left_arrow_pos = {
#include "advertise/options_draw/left_arrow_pos.inc"
};
static NJS_POINT3 right_arrow_pos = {
#include "advertise/options_draw/right_arrow_pos.inc"
};
static struct menu_arc item_arc = {-3.0F, 1.0F};
static struct menu_arc theme_arc = {-3.0F, 1.0F};
/* where a list's first row draws, and how far down each next row draws. the
 * text list's first row draws at text_list_y instead */
static Float list_x = 320.0F;
static Float list_y = 240.0F;
static Float text_list_y = 180.0F;
static Float list_z = -5.0F;
static Float row_step = 40.0F;
static NJS_POINT3 theme_picture_pos = {
#include "advertise/options_draw/theme_picture_pos.inc"
};
/* the lists' rows, indexed by their ROW_ names or by the languages' enums */
/* clang-format off */
static struct menu_row sound_rows[LIST_ROWS] = {
    {8, 19},
    {9, 20},
};
static struct menu_row vibration_rows[LIST_ROWS] = {
    {12, 23},
    {13, 24},
};
static struct menu_row voice_rows[LIST_ROWS] = {
    {4, 15},
    {3, 14},
};
static struct menu_row text_rows[TEXT_LANGUAGES] = {
    {4, 15},
    {3, 14},
    {6, 17},
    {7, 18},
    {5, 16},
};
/* clang-format on */
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/options_draw/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/options_draw/texlist.inc"
};
static NJS_TEXNAME theme_texnames[THEME_TEXTURES] = {
#include "advertise/options_draw/theme_texnames.inc"
};
static NJS_TEXLIST theme_texlist = {
#include "advertise/options_draw/theme_texlist.inc"
};
/* options_draw_create sets each cell's center, cx and cy, to its middle.
 * the theme pictures are cells of theme_texlist, and the rest of texlist */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/options_draw/cells.inc"
};

static void options_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void options_theme_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                      Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &theme_texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void options_arrows_draw(void)
{
    options_sprite_draw(ARROW_CELL, 1.0F, &left_arrow_pos, MENU_SPRITE_ATTR);
    options_sprite_draw(ARROW_CELL, 1.0F, &right_arrow_pos,
                        MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP);
}

static void options_title_draw(void)
{
    struct menu_sprite_row row;
    NJS_POINT3 pos;

    pos.x = TITLE_X;
    pos.y = TITLE_Y;
    pos.z = TITLE_PRIORITY;
    row.tanim = cells;
    row.first = TITLE_CELL;
    row.count = TITLE_CELLS;
    row.pos = &pos;
    row.draw = options_sprite_draw;
    /* @bug memory: the title's last cell is the last of cells, so
     * menu_sprite_row_draw reads the width of cells[CELLS], past their end:
     * the call at 0x8c50c6bc and the read at 0x8c500ac2. nothing draws at the
     * place that width gives */
    menu_sprite_row_draw(&row);
}

/* turn the item wheel a step toward options_work.item, and place its
 * entries. it returns TRUE where it turned */
static Bool options_item_wheel_turn(void)
{
    Bool moving;
    /* @hack type: the two distances as an array, where two Ints would do */
    Int dist[2];
    Int count;
    Int target;
    Int angle;
    Int i;

    if (options_work.tv_setting_shown) {
        count = OPTIONS_ITEMS;
    } else {
        count = OPTIONS_ITEM_TV_SETTING;
    }
    target =
        (DEGREES_PER_TURN - (options_work.item * (DEGREES_PER_TURN / count))) %
        DEGREES_PER_TURN;
    if (target == options_view.item_turn) {
        moving = FALSE;
    } else {
        dist[0] = (target - options_view.item_turn + DEGREES_PER_TURN) %
                  DEGREES_PER_TURN;
        dist[1] = (options_view.item_turn - target + DEGREES_PER_TURN) %
                  DEGREES_PER_TURN;
        if (dist[0] < dist[1]) {
            options_view.item_turn += WHEEL_STEP;
            dist[0] = (target - options_view.item_turn + DEGREES_PER_TURN) %
                      DEGREES_PER_TURN;
            dist[1] = (options_view.item_turn - target + DEGREES_PER_TURN) %
                      DEGREES_PER_TURN;
            if (dist[0] >= dist[1]) {
                options_view.item_turn = target;
            }
        } else {
            options_view.item_turn -= WHEEL_STEP;
            dist[0] = (target - options_view.item_turn + DEGREES_PER_TURN) %
                      DEGREES_PER_TURN;
            dist[1] = (options_view.item_turn - target + DEGREES_PER_TURN) %
                      DEGREES_PER_TURN;
            if (dist[0] <= dist[1]) {
                options_view.item_turn = target;
            }
        }
        moving = TRUE;
    }
    for (i = 0; i < count; i++) {
        angle = (options_view.item_turn + ((DEGREES_PER_TURN / count) * i)) %
                DEGREES_PER_TURN;
        menu_arc_point(&item_arc, &options_view.items[i].pos, angle);
        options_view.items[i].pos.y -= WHEEL_RISE;
        options_view.items[i].scale =
            ((njCos(angle * ANGLE_PER_DEGREE) + 1.0F) * WHEEL_SCALE_SPAN) +
            WHEEL_BACK_SCALE;
        options_view.items[i].pos.z =
            (options_view.items[i].scale * WHEEL_PRIORITY_SCALE) +
            WHEEL_PRIORITY;
    }
    return moving;
}

/* turn the theme wheel a step toward options_work.theme_row, among the
 * themes found, and place their entries. it sets options_work.themes_found
 * to their count, and returns TRUE where it turned */
static Bool options_theme_wheel_turn(void)
{
    Bool moving;
    /* @hack type: the two distances as an array, where two Ints would do */
    Int dist[2];
    Int count;
    Int target;
    Int angle;
    Int i;

    count = 0;
    for (i = 0; i < MENU_THEMES; i++) {
        if (options_work.theme_found[i]) {
            count++;
        }
    }
    target =
        (Int)((Float)DEGREES_PER_TURN -
              (options_work.theme_row * ((Float)DEGREES_PER_TURN / count))) %
        DEGREES_PER_TURN;
    if (target == options_view.theme_turn) {
        moving = FALSE;
    } else {
        dist[0] = (target - options_view.theme_turn + DEGREES_PER_TURN) %
                  DEGREES_PER_TURN;
        dist[1] = (options_view.theme_turn - target + DEGREES_PER_TURN) %
                  DEGREES_PER_TURN;
        if (dist[0] < dist[1]) {
            options_view.theme_turn += WHEEL_STEP;
            dist[0] =
                (target - options_view.theme_turn + (2 * DEGREES_PER_TURN)) %
                DEGREES_PER_TURN;
            dist[1] =
                (options_view.theme_turn - target + (2 * DEGREES_PER_TURN)) %
                DEGREES_PER_TURN;
            if (dist[0] >= dist[1]) {
                options_view.theme_turn = target;
            }
        } else {
            options_view.theme_turn -= WHEEL_STEP;
            dist[0] =
                (target - options_view.theme_turn + (2 * DEGREES_PER_TURN)) %
                DEGREES_PER_TURN;
            dist[1] =
                (options_view.theme_turn - target + (2 * DEGREES_PER_TURN)) %
                DEGREES_PER_TURN;
            if (dist[0] <= dist[1]) {
                options_view.theme_turn = target;
            }
        }
        moving = TRUE;
    }
    for (i = 0; i < count; i++) {
        angle = (Int)((i * ((Float)DEGREES_PER_TURN / count)) +
                      options_view.theme_turn) %
                DEGREES_PER_TURN;
        menu_arc_point(&theme_arc, &options_view.themes[i].pos, angle);
        options_view.themes[i].pos.y -= WHEEL_RISE;
        options_view.themes[i].scale =
            ((njCos(angle * ANGLE_PER_DEGREE) + 1.0F) * WHEEL_SCALE_SPAN) +
            WHEEL_BACK_SCALE;
        options_view.themes[i].pos.z =
            (options_view.themes[i].scale * WHEEL_PRIORITY_SCALE) +
            WHEEL_PRIORITY;
    }
    options_work.themes_found = count;
    return moving;
}

/* draw the theme wheel. the theme at options_work.theme_row draws lit while
 * options_work.themes isn't moving */
static void options_theme_wheel_draw(void)
{
    Int order[MENU_THEMES];
    NJS_POINT3 pos;
    Int count;
    Int entry;
    Int i;

    count = 0;
    for (i = 0; i < MENU_THEMES; i++) {
        if (options_work.theme_found[i]) {
            /* @hack redundant: the found themes listed in order, which nothing
             * reads */
            order[count++] = i;
        }
    }
    menu_fade_begin(options_view.themes_fade);
    entry = 0;
    for (i = 0; i < MENU_THEMES; i++) {
        if (!options_work.theme_found[i]) {
            continue;
        }
        pos = options_view.themes[entry].pos;
        if (!options_work.themes.moving && options_row_theme() == i) {
            options_sprite_draw(i + LIT_THEME_CELL,
                                options_view.themes[entry].scale, &pos,
                                MENU_SPRITE_ATTR);
        } else {
            options_sprite_draw(i + DIM_THEME_CELL,
                                options_view.themes[entry].scale, &pos,
                                MENU_SPRITE_ATTR);
        }
        /* @hack redundant: a step of pos's priority, which the next entry's
         * place overwrites */
        pos.z += ICON_LIFT;
        entry++;
    }
    menu_tint_end();
}

static void options_draw_step(struct task *task)
{
    options_work.moving = FALSE;
    if (!options_work.shown) {
        options_work.backdrop.shown = FALSE;
        options_work.sound.shown = FALSE;
        options_work.vibration.shown = FALSE;
        options_work.text.shown = FALSE;
        options_work.voice.shown = FALSE;
        options_work.themes.shown = FALSE;
        options_work.items.shown = FALSE;
    }
    options_work.backdrop.moving = FALSE;
    if (options_work.backdrop.shown) {
        if (options_work.backdrop.bright) {
            if (menu_approach(&options_view.backdrop_fade, 1.0F, FADE_STEP)) {
                options_work.backdrop.moving = TRUE;
            }
        } else {
            if (menu_approach(&options_view.backdrop_fade, HALF_LEVEL,
                              FADE_STEP)) {
                options_work.backdrop.moving = TRUE;
            }
        }
    } else {
        if (menu_approach(&options_view.backdrop_fade, 0.0F, FADE_STEP)) {
            options_work.backdrop.moving = TRUE;
        }
    }
    options_work.items.moving = FALSE;
    if (options_work.items.shown) {
        if (options_work.items.bright) {
            if (menu_approach(&options_view.items_fade, 1.0F, FADE_STEP)) {
                options_work.items.moving = TRUE;
            }
        } else {
            if (menu_approach(&options_view.items_fade, HALF_LEVEL,
                              FADE_STEP)) {
                options_work.items.moving = TRUE;
            }
        }
    } else {
        if (menu_approach(&options_view.items_fade, 0.0F, FADE_STEP)) {
            options_work.items.moving = TRUE;
        }
    }
    if (options_work.items.shown) {
        options_work.items.moving |= options_item_wheel_turn();
    }
    options_work.sound.moving = FALSE;
    if (options_work.sound.shown) {
        if (menu_approach(&options_view.sound_fade, 1.0F, FADE_STEP)) {
            options_work.sound.moving = TRUE;
        }
    } else {
        if (menu_approach(&options_view.sound_fade, 0.0F, FADE_STEP)) {
            options_work.sound.moving = TRUE;
        }
    }
    options_work.vibration.moving = FALSE;
    if (options_work.vibration.shown) {
        if (menu_approach(&options_view.vibration_fade, 1.0F, FADE_STEP)) {
            options_work.vibration.moving = TRUE;
        }
    } else {
        if (menu_approach(&options_view.vibration_fade, 0.0F, FADE_STEP)) {
            options_work.vibration.moving = TRUE;
        }
    }
    options_work.voice.moving = FALSE;
    if (options_work.voice.shown) {
        if (menu_approach(&options_view.voice_fade, 1.0F, FADE_STEP)) {
            options_work.voice.moving = TRUE;
        }
    } else {
        if (menu_approach(&options_view.voice_fade, 0.0F, FADE_STEP)) {
            options_work.voice.moving = TRUE;
        }
    }
    options_work.text.moving = FALSE;
    if (options_work.text.shown) {
        if (menu_approach(&options_view.text_fade, 1.0F, FADE_STEP)) {
            options_work.text.moving = TRUE;
        }
    } else {
        if (menu_approach(&options_view.text_fade, 0.0F, FADE_STEP)) {
            options_work.text.moving = TRUE;
        }
    }
    options_work.themes.moving = FALSE;
    if (options_work.themes.shown) {
        if (menu_approach(&options_view.themes_fade, 1.0F, FADE_STEP)) {
            options_work.themes.moving = TRUE;
        }
    } else {
        if (menu_approach(&options_view.themes_fade, 0.0F, FADE_STEP)) {
            options_work.themes.moving = TRUE;
        }
    }
    if (options_work.themes.shown) {
        options_work.themes.moving |= options_theme_wheel_turn();
    }
    /* @hack redundant: moving ORed in with the parts' moving, where the
     * parts' alone would do. the step set it FALSE at its start */
    options_work.moving =
        options_work.moving | options_work.backdrop.moving |
        options_work.sound.moving | options_work.vibration.moving |
        options_work.text.moving | options_work.voice.moving |
        options_work.themes.moving | options_work.items.moving;
}

static void options_draw(struct task *task)
{
    Sint32 text_cursor;
    Sint32 count;
    Sint32 sound_cursor;
    Sint32 vibration_cursor;
    Sint32 voice_cursor;
    Int i;

    if (options_view.backdrop_fade != 0.0F) {
        menu_fade_begin(options_view.backdrop_fade);
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                           MENU_BACKDROP_TINT);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        options_title_draw();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        if (theme_pictures[game_state.theme] >= 0) {
            menu_fade_begin(options_view.backdrop_fade * HALF_LEVEL);
            options_theme_sprite_draw(theme_pictures[game_state.theme],
                                      THEME_PICTURE_SCALE, &theme_picture_pos,
                                      MENU_SPRITE_ATTR);
        }
        menu_tint_end();
    }
    /* @hack spelling: the item wheel's and the theme wheel's fades tested
     * alone, where compares with 0.0F would do */
    if (options_view.items_fade) {
        if (options_work.tv_setting_shown) {
            count = OPTIONS_ITEMS;
        } else {
            count = OPTIONS_ITEM_TV_SETTING;
        }
        for (i = 0; i < count; i++) {
            NJS_POINT3 pos;

            pos = options_view.items[i].pos;
            if (i == OPTIONS_ITEM_VIBRATION) {
                if (options_work.vibration.bright) {
                    menu_fade_begin(options_view.items_fade);
                } else {
                    menu_fade_begin(options_view.items_fade * HALF_LEVEL);
                }
            } else {
                menu_fade_begin(options_view.items_fade);
            }
            if (options_work.items.bright && !options_work.items.moving &&
                options_work.item == i) {
                options_sprite_draw(LIT_ITEM_CELL, options_view.items[i].scale,
                                    &pos, MENU_SPRITE_ATTR);
                if (options_work.help_shown) {
                    NJS_POINT3 at = ITEM_HELP_POS;

                    message_sprite_draw(item_faces[i].help, &at, 1.0F);
                }
            } else {
                options_sprite_draw(DIM_ITEM_CELL, options_view.items[i].scale,
                                    &pos, MENU_SPRITE_ATTR);
            }
            pos.z += ICON_LIFT;
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            options_sprite_draw(item_faces[i].icon, options_view.items[i].scale,
                                &pos, MENU_SPRITE_ATTR);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        }
        menu_tint_end();
    }
    if (options_view.sound_fade != 0.0F) {
        struct menu_scroll scroll;
        struct menu_list list;
        NJS_POINT3 label;

        menu_fade_begin(options_view.sound_fade);
        /* @hack redundant: the label's place set as the other lists set
         * theirs, though this list draws no label */
        label.x = LABEL_X;
        label.y = list_y - row_step;
        label.z = LABEL_PRIORITY;
        label.z += LABEL_LIFT;
        scroll.top = &list_top;
        scroll.cursor = &sound_cursor;
        scroll.shown = LIST_ROWS;
        switch (game_state.mono) {
        case FALSE:
            scroll.target = ROW_STEREO;
            sound_cursor = ROW_STEREO;
            break;
        case TRUE:
            scroll.target = ROW_MONO;
            sound_cursor = ROW_MONO;
            break;
        }
        list.rows = sound_rows;
        list.count = LIST_ROWS;
        list.lit_back = LIT_BACK_CELL;
        list.dim_back = DIM_BACK_CELL;
        list.language_offset = SOUND_LANGUAGE_OFFSET;
        list.pos.x = list_x;
        list.pos.y = list_y;
        list.pos.z = list_z;
        list.step.x = 0.0F;
        list.step.y = row_step;
        list.step.z = 0.0F;
        list.draw = options_sprite_draw;
        menu_list_draw(&list, &scroll, 1.0F);
        menu_tint_end();
    }
    if (options_view.vibration_fade != 0.0F) {
        struct menu_scroll scroll;
        struct menu_list list;

        menu_fade_begin(options_view.vibration_fade);
        scroll.top = &list_top;
        scroll.cursor = &vibration_cursor;
        scroll.shown = LIST_ROWS;
        switch (game_state.vibration) {
        case TRUE:
            scroll.target = ROW_VIBRATION_ON;
            vibration_cursor = ROW_VIBRATION_ON;
            break;
        case FALSE:
            scroll.target = ROW_VIBRATION_OFF;
            vibration_cursor = ROW_VIBRATION_OFF;
            break;
        }
        list.rows = vibration_rows;
        list.count = LIST_ROWS;
        list.lit_back = LIT_BACK_CELL;
        list.dim_back = DIM_BACK_CELL;
        list.language_offset = 0;
        list.pos.x = list_x;
        list.pos.y = list_y;
        list.pos.z = list_z;
        list.step.x = 0.0F;
        list.step.y = row_step;
        list.step.z = 0.0F;
        list.draw = options_sprite_draw;
        menu_list_draw(&list, &scroll, 1.0F);
        menu_tint_end();
    }
    if (options_view.voice_fade != 0.0F) {
        struct menu_scroll scroll;
        struct menu_list list;
        NJS_POINT3 label;

        menu_fade_begin(options_view.voice_fade);
        label.x = LABEL_X;
        label.y = list_y - row_step;
        label.z = LABEL_PRIORITY;
        options_sprite_draw(LABEL_BACK_CELL, 1.0F, &label, MENU_SPRITE_ATTR);
        label.z += LABEL_LIFT;
        if (game_state.text_language) {
            options_sprite_draw(VOICE_LABEL_CELL, 1.0F, &label,
                                MENU_SPRITE_ATTR);
        } else {
            options_sprite_draw(VOICE_LABEL_CELL + JAPANESE_LABEL_OFFSET, 1.0F,
                                &label, MENU_SPRITE_ATTR);
        }
        scroll.top = &list_top;
        scroll.cursor = &voice_cursor;
        scroll.shown = LIST_ROWS;
        switch (game_state.voice_language) {
        case VOICE_JAPANESE:
            scroll.target = VOICE_JAPANESE;
            voice_cursor = VOICE_JAPANESE;
            break;
        case VOICE_ENGLISH:
            scroll.target = VOICE_ENGLISH;
            voice_cursor = VOICE_ENGLISH;
            break;
        }
        list.rows = voice_rows;
        list.count = LIST_ROWS;
        list.lit_back = LIT_BACK_CELL;
        list.dim_back = DIM_BACK_CELL;
        list.language_offset = 0;
        list.pos.x = list_x;
        list.pos.y = list_y;
        list.pos.z = list_z;
        list.step.x = 0.0F;
        list.step.y = row_step;
        list.step.z = 0.0F;
        list.draw = options_sprite_draw;
        menu_list_draw(&list, &scroll, 1.0F);
        options_arrows_draw();
        menu_tint_end();
    }
    if (options_view.text_fade != 0.0F) {
        struct menu_scroll scroll;
        struct menu_list list;
        NJS_POINT3 label;

        menu_fade_begin(options_view.text_fade);
        label.x = LABEL_X;
        label.y = text_list_y - row_step;
        label.z = LABEL_PRIORITY;
        options_sprite_draw(LABEL_BACK_CELL, 1.0F, &label, MENU_SPRITE_ATTR);
        label.z += LABEL_LIFT;
        if (game_state.text_language) {
            options_sprite_draw(TEXT_LABEL_CELL, 1.0F, &label,
                                MENU_SPRITE_ATTR);
        } else {
            options_sprite_draw(TEXT_LABEL_CELL + JAPANESE_LABEL_OFFSET, 1.0F,
                                &label, MENU_SPRITE_ATTR);
        }
        scroll.top = &list_top;
        scroll.cursor = &text_cursor;
        scroll.shown = TEXT_LANGUAGES;
        scroll.target = options_work.text_row;
        text_cursor = scroll.target;
        list.rows = text_rows;
        list.count = TEXT_LANGUAGES;
        list.lit_back = LIT_BACK_CELL;
        list.dim_back = DIM_BACK_CELL;
        list.language_offset = 0;
        list.pos.x = list_x;
        list.pos.y = text_list_y;
        list.pos.z = list_z;
        list.step.x = 0.0F;
        list.step.y = row_step;
        list.step.z = 0.0F;
        list.draw = options_sprite_draw;
        menu_list_draw(&list, &scroll, 1.0F);
        options_arrows_draw();
        menu_tint_end();
    }
    if (options_view.themes_fade) {
        options_theme_wheel_draw();
    }
}

static void options_draw_destroy(struct task *task)
{
    options_draw_release();
}

void options_draw_create(void)
{
    struct task *task;
    Int i;
    NJS_TEXANIM *cell;
    Uint8 *byte;
    Int left;

    task = task_create(TASK_WORK, TASK_LEVEL, options_draw_step, "optionsExec");
    task->draw = options_draw;
    task->destructor = options_draw_destroy;
    panel_task = task;
    byte = (Uint8 *)&options_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(options_work); left != 0; left--) {
        *byte++ = 0;
    }
    options_draw_reset();
    /* @hack spelling: the cells' address set in a loop that runs once,
     * where an assignment would do */
    do {
        cell = cells;
    } while (0);
    for (i = 0; i < CELLS; i++) {
        cell[i].cx = cell[i].sx / 2;
        cell[i].cy = cell[i].sy / 2;
    }
}

/* clear options_work but its loaded, and set the view to view_start, every
 * fade and turn 0 */
static void options_draw_reset(void)
{
    Int loaded;
    Uint8 *byte;
    Int left;

    loaded = options_work.loaded;
    byte = (Uint8 *)&options_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(options_work); left != 0; left--) {
        *byte++ = 0;
    }
    options_view = view_start;
    options_work.loaded = loaded;
}

/* destroy the task, where options_draw_create made one that this hasn't
 * destroyed. nothing calls options_draw_end, and no header declares it. it
 * is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void options_draw_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

void options_draw_end(void)
{
    if (panel_task != NULL) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

void options_draw_load(void)
{
    if (!options_work.loaded) {
        texture_load_pvm(OPTIONS_ARCHIVE, &texlist);
        texture_load_pvm(OPTIONS_THEME_ARCHIVE, &theme_texlist);
        options_work.loaded = TRUE;
    }
}

void options_draw_release(void)
{
    if (options_work.loaded) {
        njReleaseTexture(&texlist);
        njReleaseTexture(&theme_texlist);
        options_work.loaded = FALSE;
    }
}
