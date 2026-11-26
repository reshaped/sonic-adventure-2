/* the kart race panel. it picks the title, the list's hint and the
 * none-found hint by menu_control.mode, and the backdrop by
 * kart_race_work.download_backdrop */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/kart_race_cells.h"
#include "advertise/kart_race_panel.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "camera/viewport.h"
#include "cart/kart_models.h"
#include "draw/chunk.h"
#include "draw/light.h"
#include "draw/screen.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/kart_race_panel/text.inc"

#define TASK_LEVEL 2
/* a fade runs from 0 to 1 in FADE_STEPS steps */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
/* a stat's bar is BAR_UNIT pixels long for each point. it grows or shrinks
 * BAR_STEP pixels a step, 100 pixels in FADE_STEPS steps */
#define BAR_UNIT 10.0F
#define BAR_STEP (100.0F / FADE_STEPS)
#define STATS 4
/* the sides of the stats' bars. LEFT_SIDE is the whole screen or the first
 * player's half, and RIGHT_SIDE the second player's half. a side's bars show
 * its player's pick */
#define LEFT_SIDE 0
#define RIGHT_SIDE 1
#define SIDES 2
#define BACKDROP_PRIORITY (-10000.0F)
/* the first textures of the download menu's backdrop and the kart race's,
 * in backdrop_texlist */
#define DOWNLOAD_BACKDROP 0
#define KART_BACKDROP 8
/* the textures of texlist, backdrop_texlist and kart_texlist */
#define TEXTURES 18
#define BACKDROP_TEXTURES 16
#define KART_TEXTURES 98

/* the cells of kart_race_cells. a title or a frame is a run of cells that
 * menu_sprite_row_draw draws side by side */
#define KART_TITLE_CELL 8
#define KART_TITLE_CELLS 3
#define STATS_CELL 11
#define BAR_CELL 18 /* a pixel wide, so a bar's sx is its length */
#define COURSE_LIT_BACK_CELL 20
#define COURSE_DIM_BACK_CELL 21
#define COURSES_TITLE_CELL 22
#define COURSES_TITLE_CELLS 3
#define LIST_TITLE_CELL 25
#define LIST_TITLE_CELLS 2
#define ARROW_CELL 27 /* an arrow up, or down under NJD_SPRITE_VFLIP */
#define TIME_FRAME_CELL 28
#define TIME_FRAME_CELLS 2
#define EMBLEM_FRAME_CELL 30
#define PLACE_FRAME_CELL 31
#define FIRST_PLACE_CELL 32 /* the label 1st, which 2nd and 3rd follow */
#define ROW_CELL 35
#define ROW_CELLS 3
/* a rule's left half, and its right half under NJD_SPRITE_HFLIP */
#define RULE_CELL 39
#define PICKED_CELL 40
/* the question's window and its answers, in Japanese. under every other
 * text language, the English window is ENGLISH_WINDOW cells on, and each
 * English answer ENGLISH_ANSWERS */
#define WINDOW_CELL 41
#define DOWNLOAD_TITLE_CELL 43
#define DOWNLOAD_TITLE_CELLS 4
#define YES_LIT_CELL 52
#define YES_DIM_CELL 53
#define NO_LIT_CELL 54
#define NO_DIM_CELL 55
#define ENGLISH_WINDOW 1
#define ENGLISH_ANSWERS 5
#define NONE_FOUND_CELL 56

/* the hints, system messages of message_tables */
#define HINT_DRIVER 76 /* "Select a driver." */
#define HINT_COURSE 77 /* "Select a course." */
#define HINT_READING 79 /* "Checking memory card." */
#define HINT_KART_LIST 80 /* "Select a download course." */
#define HINT_NO_COURSE 81 /* "No course file found." */
#define HINT_NO_DOWNLOAD 83 /* "No download file found." */
#define HINT_DOWNLOAD_LIST 84 /* "Select a download event." */
#define HINT_QUESTION 85 /* "Do you want to play this event?" */

/* the views the drivers and karts draw in: the whole screen, or a player's
 * half. view_areas holds the halves from VIEW_LEFT */
#define VIEW_WHOLE 0
#define VIEW_LEFT 1
#define VIEW_RIGHT 2
#define VIEWS 3

#define COURSE_ROWS (KART_RACE_DOWNLOADED + 1)
#define LIST_ROWS 3
#define TEXT_PRIORITY (-10.0F)
#define FRONT_PRIORITY (-50.0F)
/* a file's lines draw only once their part's fade passes these */
#define TEXT_FADE 0.99F
#define QUESTION_TEXT_FADE 0.98F
#define SCREEN_CENTER_X 320.0F
#define SCREEN_CENTER_Y 240.0F

/* the courses' title and the list's */
#define TITLE_X 240.0F
#define TITLE_Y 120.0F
#define TITLE_PRIORITY (-100.0F)
#define LIST_TITLE_X 298.0F
#define COURSES_TITLE_X 257.0F
/* the course list's first row, beside two players' drivers or below one's */
#define SPLIT_COURSES_X 330.0F
#define SPLIT_COURSES_Y 220.0F
#define COURSES_X 150.0F
#define COURSES_Y 180.0F
#define COURSES_PRIORITY (-20.0F)
#define COURSE_STEP 35.0F
/* a kart record's SAVE_BEST times draw a row each, from TIMES_Y down. a
 * row's place draws in its frame at PLACE_X. the time's frame draws
 * PLACE_TO_FRAME to the right, and the driver's icon FRAME_TO_ICON
 * further. the time itself draws at TIME_X. the rows step PLACE_STEP down,
 * but the times TIME_STEP */
#define TIMES_Y 280.0F
#define PLACE_X 360.0F
#define PLACE_TO_FRAME 100.0F
#define FRAME_TO_ICON 120.0F
#define PLACE_STEP 38.0F
#define TIME_X 420.0F
#define TIME_STEP 40.0F
#define TIME_PRIORITY 5.0F
#define COURSE_TIME_SCALE 0.7F
#define EMBLEM_PRIORITY 10.0F
/* the picked driver's icon draws at PICKED_SCALE, in PICKED_CELL's ring */
#define PICKED_SCALE 1.1F
/* the bars' first row, from the stats' frame, and the step to the next */
#define BARS_X 40.0F
#define BARS_RISE 40.0F
#define BAR_STEP_Y 21.0F
#define LIGHT_AMBIENT 0.6F

/* the parts' fades, each from 0 to 1, the stats' bars, and the list's
 * window */
struct kart_race_fades {
    Float backdrop;
    Float drivers;
    Float bars[SIDES][STATS]; /* each bar's length in pixels */
    Float courses;
    Float list;
    Float preview;
    Float question;
    Sint32 top; /* the list's entry the window starts at */
    Sint32 cursor; /* the cursor's row in the window */
};

/* a driver to pick */
struct kart_race_preview {
    Sint32 character;
    Sint32 icon;
    NJS_CNK_OBJECT *kart;
    Uint8 stats[STATS];
};

/* where a view draws the drivers' icons and the stats' frame */
struct kart_race_layout {
    NJS_POINT3 icons;
    NJS_POINT3 stats;
};

/* The visible download list and the individual panel fades. */
static struct download_entry *kart_race_list;
static struct kart_race_fades kart_race_fades;

/* destroy the task, where kart_race_panel_create made one that this hasn't
 * destroyed. nothing calls kart_race_panel_kill, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void kart_race_panel_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

static void kart_race_panel_destroy(struct task *task);

/* the hints' place. each hint's text opens with '\t', which
 * message_sprite_draw centers on the screen, so x goes unread */
static NJS_POINT3 hint_pos = {
#include "advertise/kart_race_panel/hint_pos.inc"
};
static struct kart_race_fades fades_start = {
    0.0F, 0.0F, {{0.0F}, {0.0F}}, 0.0F, 0.0F, 0.0F, 0.0F, 0, 0,
};
/* by character, its driver in previews. no code reads it by name: it lies
 * just before previews, so with a second driver of NO_DRIVER,
 * kart_race_panel_step reads the map's last word as that driver's stats */
static Sint32 drivers_by_character[PLAYER_EGGMAN_MECH + 1] = {0, 3, 1, 4,
                                                              2, 5, 1, 4};
KEPT static struct kart_race_preview previews[KART_RACE_DRIVERS] = {
#include "advertise/kart_race_panel/previews.inc"
};
/* by character, its driver's icon, for a kart record's time */
static Sint32 character_cells[PLAYER_EGGMAN_MECH + 1] = {12, 14, 13, 15,
                                                         16, 17, 13, 15};
static struct task *panel_task = NULL;
/* the question's window and the first line of the entry's text. each
 * next line draws line_step lower */
static Float window_x = 150.0F;
static Float window_y = 150.0F;
static Float text_x = 250.0F;
static Float text_y = 100.0F;
static Float line_step = 26.0F;
static Float last_line_x = 100.0F;
static Float last_line_y = 250.0F;
static Float yes_x = 200.0F;
static Float yes_y = 350.0F;
static Float no_step = 240.0F;
/* the best time's offset from where a fourth line would draw, and its
 * scale */
static Float time_dx = 8.0F;
static Float time_dy = 14.0F;
static Float time_scale = 0.8F;
/* the list's first row, and its frame's x. each row's text draws text_rise
 * above its frame */
static Float list_x = 200.0F;
static Float row_frame_x = 234.0F;
static Float list_y = 210.0F;
static Float row_step = 50.0F;
static Float text_rise = 13.0F;
static Float up_arrow_y = 172.0F;
static Float down_arrow_y = 350.0F;
static Float dat_8c567df0 = 240.0F;
static Float rule_left_x = 260.0F;
static Float rule_right_x = 385.0F;
static Float rule_y = 135.0F;
/* the menu's title's first cell */
static NJS_POINT3 title_pos = {
#include "advertise/kart_race_panel/title_pos.inc"
};
static Float dat_8c567e0c[8] = {
    320.0F, 120.0F, 20.0F, 10.0F, 10.0F, 10.0F, 10.0F, 10.0F,
};
static Float emblem_x = 520.0F;
static Float emblem_y = 230.0F;
static Float emblem_scale = 1.0F;
/* the course list's window, which never moves */
static Sint32 courses_top = 0;
/* clang-format off */
static struct menu_row course_rows[COURSE_ROWS] = {
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7},
};
/* VIEW_LEFT's and VIEW_RIGHT's: the whole screen, centered on a half */
static struct viewport_area view_areas[VIEWS - VIEW_LEFT] = {
    {{{0.0F, 0.0F}, {SCREEN_WIDTH, SCREEN_HEIGHT}}, {160.0F, SCREEN_CENTER_Y}, {0.0F, 0.0F}},
    {{{0.0F, 0.0F}, {SCREEN_WIDTH, SCREEN_HEIGHT}}, {480.0F, SCREEN_CENTER_Y}, {0.0F, 0.0F}},
};
/* clang-format on */
/* the preview's camera rises and backs off from where njInitCamera leaves
 * it, and looks back at the origin. the kart stands at y kart_drop, below
 * the origin */
static Float camera_rise = 20.0F;
static Float camera_back = 70.0F;
static Float kart_drop = -3.0F;
static Float dat_8c567ea8[2] = {10.0F, 10.0F};
static Float icon_step = 40.0F;
/* by view */
KEPT static struct kart_race_layout layouts[VIEWS] = {
#include "advertise/kart_race_panel/layouts.inc"
};
/* the list's window. the step sets its target to kart_race_work.entry */
static struct menu_scroll list_scroll = {
    &kart_race_fades.top,
    &kart_race_fades.cursor,
    0,
    LIST_ROWS,
};
/* kartRace.prs */
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/kart_race_panel/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/kart_race_panel/texlist.inc"
};
/* kartRaceBg.prs: the download menu's backdrop, then the kart race's */
static NJS_TEXNAME backdrop_texnames[BACKDROP_TEXTURES] = {
#include "advertise/kart_race_panel/backdrop_texnames.inc"
};
static NJS_TEXLIST backdrop_texlist = {
#include "advertise/kart_race_panel/backdrop_texlist.inc"
};
/* the floor a kart stands on in its preview, under kartrace_tex_blue. it is
 * a square 20 a side, its points a grid of 3 by 3 */
#define FLOOR_POINTS 9
/* two strips of six points. a strip is its count of points, then each
 * point's index, u and v. the 0 after the strips pads the chunk to a whole
 * word */
static Sint16 floor_polygons[51] = {
#include "advertise/kart_race_panel/floor_polygons.inc"
};
static Sint32 floor_vertices[57] = {
#include "advertise/kart_race_panel/floor_vertices.inc"
};
static NJS_CNK_MODEL floor_model = {
#include "advertise/kart_race_panel/floor_model.inc"
};
static NJS_CNK_OBJECT floor_object = {
#include "advertise/kart_race_panel/floor_object.inc"
};
/* cartData.pvm */
static NJS_TEXNAME kart_texnames[KART_TEXTURES] = {
#include "advertise/kart_race_panel/kart_texnames.inc"
};
static NJS_TEXLIST kart_texlist = {
#include "advertise/kart_race_panel/kart_texlist.inc"
};

static void kart_race_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                  Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = kart_race_cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void kart_race_bar_draw(Int cell, Float length, NJS_POINT3 *pos,
                               Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = kart_race_cells;
    sprite.sx = length;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void kart_race_question_draw(void)
{
    NJS_POINT3 pos;
    NJS_POINT3 time_pos;
    struct save_time time;
    /* @hack spelling: the English answers' offset as a comparison times the
     * offset, where a conditional expression would do */
    Int answers = (game_state.text_language > TEXT_JAPANESE) * ENGLISH_ANSWERS;

    menu_fade_begin(kart_race_fades.question);
    pos.x = window_x;
    pos.y = window_y;
    pos.z = FRONT_PRIORITY;
    kart_race_sprite_draw(
        (game_state.text_language > TEXT_JAPANESE ? ENGLISH_WINDOW : 0) +
            WINDOW_CELL,
        1.0F, &pos, MENU_SPRITE_ATTR);
    if (kart_race_fades.question > QUESTION_TEXT_FADE) {
        pos.x = text_x;
        pos.y = text_y;
        pos.z = FRONT_PRIORITY;
        message_sprite_draw_text(kart_race_list[kart_race_work.entry]
                                     .texts[game_state.text_language]
                                     .line1,
                                 &pos, 1.0F);
        pos.y += line_step;
        message_sprite_draw_text(kart_race_list[kart_race_work.entry]
                                     .texts[game_state.text_language]
                                     .line2,
                                 &pos, 1.0F);
        pos.y += line_step;
        message_sprite_draw_text(kart_race_list[kart_race_work.entry]
                                     .texts[game_state.text_language]
                                     .line3,
                                 &pos, 1.0F);
        pos.y += line_step;
        time_pos = pos;
        time_pos.x += time_dx;
        time_pos.y += time_dy;
        /* @hack spelling: the time copied a field at a time, where a copy of
         * the record would do */
        time.minute =
            save_data
                .download_best_times[kart_race_list[kart_race_work.entry].slot]
                .minute;
        time.second =
            save_data
                .download_best_times[kart_race_list[kart_race_work.entry].slot]
                .second;
        time.hundredths =
            save_data
                .download_best_times[kart_race_list[kart_race_work.entry].slot]
                .hundredths;
        share_data_time_draw(&time_pos, &time, time_scale);
        pos.x = last_line_x;
        pos.y = last_line_y;
        pos.z = FRONT_PRIORITY;
        message_sprite_draw_text(kart_race_list[kart_race_work.entry]
                                     .texts[game_state.text_language]
                                     .line4,
                                 &pos, 1.0F);
    }
    pos.x = yes_x;
    pos.y = yes_y;
    pos.z = FRONT_PRIORITY;
    if (kart_race_work.choice == KART_RACE_YES) {
        kart_race_sprite_draw(answers + YES_LIT_CELL, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    } else {
        kart_race_sprite_draw(answers + YES_DIM_CELL, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    }
    pos.x += no_step;
    pos.z = FRONT_PRIORITY;
    if (kart_race_work.choice == KART_RACE_NO) {
        kart_race_sprite_draw(answers + NO_LIT_CELL, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    } else {
        kart_race_sprite_draw(answers + NO_DIM_CELL, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    }
    message_sprite_draw(HINT_QUESTION, &hint_pos, 1.0F);
}

static void kart_race_list_rows_draw(void)
{
    NJS_POINT3 pos;
    struct menu_sprite_row frame;
    Int i;
    Int entry;

    pos.x = list_x;
    pos.y = list_y;
    frame.tanim = kart_race_cells;
    frame.first = ROW_CELL;
    frame.count = ROW_CELLS;
    frame.pos = &pos;
    frame.draw = kart_race_sprite_draw;
    for (i = 0; i < LIST_ROWS; i++) {
        entry = kart_race_fades.top + i;
        if (entry >= kart_race_work.entry_count) {
            return;
        }
        pos.x = 0.0F;
        pos.y -= text_rise;
        pos.z = TEXT_PRIORITY;
        if (kart_race_fades.list > TEXT_FADE) {
            message_sprite_draw_text(
                kart_race_list[entry].texts[game_state.text_language].line0,
                &pos, 1.0F);
        }
        pos.x = row_frame_x;
        pos.y += text_rise;
        pos.z -= 1.0F;
        menu_sprite_row_draw(&frame);
        if (i == kart_race_fades.cursor) {
            pos.z = FRONT_PRIORITY;
        } else {
            pos.z = 0.0F;
        }
        menu_sprite_row_draw(&frame);
        pos.z += 1.0F;
        pos.y += row_step;
    }
    pos.x = SCREEN_CENTER_X;
    pos.z = FRONT_PRIORITY;
    if (kart_race_fades.top != 0) {
        pos.y = up_arrow_y;
        kart_race_sprite_draw(ARROW_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    }
    if (kart_race_fades.top < kart_race_work.entry_count - LIST_ROWS) {
        pos.y = down_arrow_y;
        kart_race_sprite_draw(ARROW_CELL, 1.0F, &pos,
                              MENU_SPRITE_ATTR | NJD_SPRITE_VFLIP);
    }
}

static void kart_race_list_draw(void)
{
    struct menu_sprite_row title;
    NJS_POINT3 pos;
    NJS_POINT3 rule;

    menu_fade_begin(kart_race_fades.list);
    title.tanim = kart_race_cells;
    /* @hack redundant: stores to the title's first, count and x that later
     * stores replace */
    title.first = ROW_CELL;
    title.count = ROW_CELLS;
    title.pos = &pos;
    title.draw = kart_race_sprite_draw;
    pos.x = TITLE_X;
    pos.y = TITLE_Y;
    /* @hack spelling: the title's depth set and then 1 added, where
     * TITLE_PRIORITY + 1 would do */
    pos.z = TITLE_PRIORITY;
    rule.x = rule_left_x;
    rule.y = rule_y;
    rule.z = TEXT_PRIORITY;
    kart_race_sprite_draw(RULE_CELL, 1.0F, &rule, MENU_SPRITE_ATTR);
    rule.x = rule_right_x;
    kart_race_sprite_draw(RULE_CELL, 1.0F, &rule,
                          MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP);
    pos.x = LIST_TITLE_X;
    pos.z += 1.0F;
    title.first = LIST_TITLE_CELL;
    title.count = LIST_TITLE_CELLS;
    menu_sprite_row_draw(&title);
    kart_race_list_rows_draw();
}

/* the title adds its light to what lies behind it */
static void kart_race_title_draw(void)
{
    struct menu_sprite_row title;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    title.tanim = kart_race_cells;
    if (menu_control.mode == MENU_MODE_KART_RACE) {
        title.first = KART_TITLE_CELL;
        title.count = KART_TITLE_CELLS;
    } else {
        title.first = DOWNLOAD_TITLE_CELL;
        title.count = DOWNLOAD_TITLE_CELLS;
    }
    title.pos = &title_pos;
    title.draw = kart_race_sprite_draw;
    menu_sprite_row_draw(&title);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void kart_race_courses_draw(void)
{
    struct menu_sprite_row frame;
    NJS_POINT3 pos;
    Int i;

    menu_fade_begin(kart_race_fades.courses);
    frame.tanim = kart_race_cells;
    /* @hack redundant: stores to the frame's first and count and to x that
     * later stores replace */
    frame.first = ROW_CELL;
    frame.count = ROW_CELLS;
    frame.pos = &pos;
    frame.draw = kart_race_sprite_draw;
    pos.x = TITLE_X;
    pos.y = TITLE_Y;
    /* @hack spelling: the title's depth set and then 1 added, where
     * TITLE_PRIORITY + 1 would do */
    pos.z = TITLE_PRIORITY;
    /* @hack spelling: the rule's, the course list's and the times' locals
     * declared in blocks of their own, where the function's head would do */
    {
        NJS_POINT3 rule;

        rule.x = rule_left_x;
        rule.y = rule_y;
        rule.z = TEXT_PRIORITY;
        kart_race_sprite_draw(RULE_CELL, 1.0F, &rule, MENU_SPRITE_ATTR);
        rule.x = rule_right_x;
        kart_race_sprite_draw(RULE_CELL, 1.0F, &rule,
                              MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP);
    }
    pos.x = COURSES_TITLE_X;
    pos.z += 1.0F;
    frame.first = COURSES_TITLE_CELL;
    frame.count = COURSES_TITLE_CELLS;
    menu_sprite_row_draw(&frame);
    if (kart_race_work.split) {
        pos.x = SPLIT_COURSES_X;
        pos.y = SPLIT_COURSES_Y;
    } else {
        pos.x = COURSES_X;
        pos.y = COURSES_Y;
    }
    pos.z = COURSES_PRIORITY;
    {
        struct menu_scroll scroll;
        struct menu_list list;
        NJS_POINT3 step;

        step.x = 0.0F;
        step.y = COURSE_STEP;
        step.z = 0.0F;
        scroll.top = &courses_top;
        scroll.cursor = &kart_race_work.course;
        scroll.shown = COURSE_ROWS;
        list.rows = course_rows;
        list.count = COURSE_ROWS;
        list.lit_back = COURSE_LIT_BACK_CELL;
        list.dim_back = COURSE_DIM_BACK_CELL;
        list.language_offset = 0;
        list.pos = pos;
        list.step = step;
        list.draw = kart_race_sprite_draw;
        menu_list_draw(&list, &scroll, 1.0F);
    }
#ifdef NONMATCHING
    /* this side draws no times for a course outside save_data.kart_records. the
     * signed test below lets a negative course through */
    if (!SAVE_KART_COURSE_VALID(kart_race_work.course)) {
        return;
    }
#endif
    if (!kart_race_work.split && kart_race_work.course < KART_RACE_DOWNLOADED) {
        pos.y = TIMES_Y;
        for (i = 0; i < SAVE_BEST; i++) {
            struct save_time time;
            NJS_POINT3 time_pos;

            pos.x = PLACE_X;
            kart_race_sprite_draw(i + FIRST_PLACE_CELL, 1.0F, &pos,
                                  MENU_SPRITE_ATTR);
            pos.z -= 1.0F;
            kart_race_sprite_draw(PLACE_FRAME_CELL, 1.0F, &pos,
                                  MENU_SPRITE_ATTR);
            pos.x += PLACE_TO_FRAME;
            frame.first = TIME_FRAME_CELL;
            frame.count = TIME_FRAME_CELLS;
            frame.pos = &pos;
            menu_sprite_row_draw(&frame);
            pos.x += FRAME_TO_ICON;
            pos.z += 1.0F;
            kart_race_sprite_draw(
                character_cells[save_data.kart_records[kart_race_work.course]
                                    .best[i]
                                    .character],
                1.0F, &pos, MENU_SPRITE_ATTR);
            /* @hack spelling: the time copied a field at a time, where a copy
             * of the record would do */
            time.minute = save_data.kart_records[kart_race_work.course]
                              .best[i]
                              .time.minute;
            time.second = save_data.kart_records[kart_race_work.course]
                              .best[i]
                              .time.second;
            time.hundredths = save_data.kart_records[kart_race_work.course]
                                  .best[i]
                                  .time.hundredths;
            time_pos.x = TIME_X;
            time_pos.y = TIMES_Y + (i * TIME_STEP);
            /* @bug uninitialized: the times draw at a depth left on the
             * stack. time_pos.z is read before any store to it (0x8c513fb8).
             * its slot first holds rule.y, and each time adds TIME_PRIORITY,
             * so the three times draw at depths 140, 145 and 150 */
#ifdef NONMATCHING
            /* this side keeps the image's depths. it sets time_pos.z to what
             * the image's slot holds here: rule_y, the y the rule drew at,
             * raised by TIME_PRIORITY for each time drawn before */
            time_pos.z = rule_y + (i * TIME_PRIORITY);
#endif
            time_pos.z += TIME_PRIORITY;
            share_data_time_draw(&time_pos, &time, COURSE_TIME_SCALE);
            pos.y += PLACE_STEP;
        }
        pos.x = emblem_x;
        pos.y = emblem_y;
        pos.z = EMBLEM_PRIORITY;
        kart_race_sprite_draw(EMBLEM_FRAME_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
        pos.z += 1.0F;
        if (save_data.kart_records[kart_race_work.course].emblem) {
            share_data_sprite_draw(SHARE_DATA_EMBLEM_CELL, emblem_scale, &pos,
                                   MENU_SPRITE_ATTR);
        }
    }
}

static void kart_race_view_set(Int view)
{
    if (view != VIEW_WHOLE) {
        viewport_set_area(&view_areas[view - VIEW_LEFT]);
        viewport_set_screen(VIEWPORT_AREA);
    } else {
        viewport_set_whole();
    }
}

static void kart_race_preview_draw(Int view, Int driver)
{
    NJS_CAMERA camera;
    /* @hack redundant: twelve bytes that no code names, which size the
     * frame */
    Uint8 spare[12];
    NJS_POINT3 pos;

    draw_control_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    menu_fade_begin(kart_race_fades.preview);
    njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    njUnitMatrix(NULL);
    kart_race_view_set(view);
    njInitCamera(&camera);
    camera.py += camera_rise;
    camera.pz += camera_back;
    camera.vy = -camera.py;
    camera.vz = -camera.pz;
    njSetCamera(&camera);
    pos.x = 0.0F;
    pos.y = kart_drop;
    pos.z = 0.0F;
    njTranslateV(NULL, &pos);
    njRotateY(NULL, game_state.steps * ANGLE_PER_DEGREE);
    njSetTexture(&kart_texlist);
    simple_light_set_direction(0.0F, 0.0F, 1.0F);
    njCnkSetSimpleLightIntensity(1.0F, LIGHT_AMBIENT);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
    njCnkSimpleDrawObject(previews[driver].kart);
    njSetTexture(&texlist);
    njCnkSimpleDrawObject(&floor_object);
    kart_race_view_set(VIEW_WHOLE);
    menu_tint_end();
    draw_control_restore();
    draw_attr_restore();
}

static void kart_race_previews_draw(void)
{
    if (kart_race_work.split) {
        kart_race_preview_draw(VIEW_LEFT, kart_race_work.picks[0].driver);
        kart_race_preview_draw(VIEW_RIGHT, kart_race_work.picks[1].driver);
    } else {
        kart_race_preview_draw(VIEW_WHOLE, kart_race_work.picks[0].driver);
    }
}

static void kart_race_drivers_draw(Int view, Int driver)
{
    NJS_POINT3 pos;
    Int i;
    Int side;

    menu_fade_begin(kart_race_fades.drivers);
    pos = layouts[view].icons;
    for (i = 0; i < KART_RACE_DRIVERS; i++) {
        if (i == driver) {
            kart_race_sprite_draw(previews[i].icon, PICKED_SCALE, &pos,
                                  MENU_SPRITE_ATTR);
            pos.z += 1.0F;
            kart_race_sprite_draw(PICKED_CELL, PICKED_SCALE, &pos,
                                  MENU_SPRITE_ATTR);
            pos.z -= 1.0F;
        } else {
            kart_race_sprite_draw(previews[i].icon, 1.0F, &pos,
                                  MENU_SPRITE_ATTR);
        }
        pos.x += icon_step;
    }
    pos = layouts[view].stats;
    kart_race_sprite_draw(STATS_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    if (view >= VIEW_RIGHT) {
        side = RIGHT_SIDE;
    } else {
        side = LEFT_SIDE;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    pos.x += BARS_X;
    pos.y -= BARS_RISE;
    kart_race_bar_draw(BAR_CELL, kart_race_fades.bars[side][0], &pos,
                       MENU_SPRITE_ATTR);
    pos.y += BAR_STEP_Y;
    kart_race_bar_draw(BAR_CELL, kart_race_fades.bars[side][1], &pos,
                       MENU_SPRITE_ATTR);
    pos.y += BAR_STEP_Y;
    kart_race_bar_draw(BAR_CELL, kart_race_fades.bars[side][2], &pos,
                       MENU_SPRITE_ATTR);
    pos.y += BAR_STEP_Y;
    kart_race_bar_draw(BAR_CELL, kart_race_fades.bars[side][3], &pos,
                       MENU_SPRITE_ATTR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void kart_race_drivers_draw_all(void)
{
    if (kart_race_work.split) {
        kart_race_drivers_draw(VIEW_LEFT, kart_race_work.picks[0].driver);
        kart_race_drivers_draw(VIEW_RIGHT, kart_race_work.picks[1].driver);
    } else {
        kart_race_drivers_draw(VIEW_WHOLE, kart_race_work.picks[0].driver);
    }
}

static void kart_race_panel_step(struct task *task)
{
    Int i;

    kart_race_work.fading = FALSE;
    kart_race_work.backdrop.moving = FALSE;
    kart_race_work.drivers.moving = FALSE;
    kart_race_work.courses.moving = FALSE;
    kart_race_work.list.moving = FALSE;
    kart_race_work.question.moving = FALSE;
    if (!kart_race_work.shown) {
        kart_race_work.backdrop.shown = FALSE;
        kart_race_work.drivers.shown = FALSE;
        kart_race_work.preview_shown = FALSE;
        kart_race_work.courses.shown = FALSE;
        kart_race_work.list.shown = FALSE;
        kart_race_work.question.shown = FALSE;
        kart_race_work.hint_shown = FALSE;
    }
    if (kart_race_work.backdrop.shown) {
        if (menu_approach(&kart_race_fades.backdrop, 1.0F, FADE_STEP)) {
            kart_race_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&kart_race_fades.backdrop, 0.0F, FADE_STEP)) {
            kart_race_work.backdrop.moving = TRUE;
        }
    }
    if (kart_race_work.drivers.shown) {
#ifdef NONMATCHING
        /* for NO_DRIVER, the matching side reads the stats just before
         * previews, which are the last word of drivers_by_character. this side
         * reads that word by name, since GCC needn't place the map before
         * previews */
        const Uint8 *second_stats =
            kart_race_work.picks[1].driver < 0
                ? (const Uint8 *)&drivers_by_character[PLAYER_EGGMAN_MECH]
                : previews[kart_race_work.picks[1].driver].stats;
#define SECOND_DRIVER_STATS second_stats
#else
#define SECOND_DRIVER_STATS (previews[kart_race_work.picks[1].driver].stats)
#endif

        /* @hack spelling: the stats stored through menu_works, where the
         * work's own name would do */
        menu_works.work_13->picks[0].stat_0 =
            previews[kart_race_work.picks[0].driver].stats[0];
        menu_works.work_13->picks[0].stat_1 =
            previews[kart_race_work.picks[0].driver].stats[1];
        menu_works.work_13->picks[0].stat_2 =
            previews[kart_race_work.picks[0].driver].stats[2];
        menu_works.work_13->picks[0].stat_3 =
            previews[kart_race_work.picks[0].driver].stats[3];
        menu_works.work_13->picks[1].stat_0 = SECOND_DRIVER_STATS[0];
        menu_works.work_13->picks[1].stat_1 = SECOND_DRIVER_STATS[1];
        menu_works.work_13->picks[1].stat_2 = SECOND_DRIVER_STATS[2];
        menu_works.work_13->picks[1].stat_3 = SECOND_DRIVER_STATS[3];
#undef SECOND_DRIVER_STATS
        if (menu_approach(&kart_race_fades.drivers, 1.0F, FADE_STEP)) {
            kart_race_work.drivers.moving = TRUE;
        }
        for (i = 0; i < SIDES; i++) {
            kart_race_work.drivers.moving |= menu_approach(
                &kart_race_fades.bars[i][0],
                kart_race_work.picks[i].stat_0 * BAR_UNIT, BAR_STEP);
            kart_race_work.drivers.moving |= menu_approach(
                &kart_race_fades.bars[i][1],
                kart_race_work.picks[i].stat_1 * BAR_UNIT, BAR_STEP);
            kart_race_work.drivers.moving |= menu_approach(
                &kart_race_fades.bars[i][2],
                kart_race_work.picks[i].stat_2 * BAR_UNIT, BAR_STEP);
            kart_race_work.drivers.moving |= menu_approach(
                &kart_race_fades.bars[i][3],
                kart_race_work.picks[i].stat_3 * BAR_UNIT, BAR_STEP);
        }
    } else {
        if (menu_approach(&kart_race_fades.drivers, 0.0F, FADE_STEP)) {
            kart_race_work.drivers.moving = TRUE;
        }
    }
    if (kart_race_work.preview_shown) {
        if (menu_approach(&kart_race_fades.preview, 1.0F, FADE_STEP)) {
            kart_race_work.fading = TRUE;
        }
    } else {
        if (menu_approach(&kart_race_fades.preview, 0.0F, FADE_STEP)) {
            kart_race_work.fading = TRUE;
        }
    }
    if (kart_race_work.courses.shown) {
        if (menu_approach(&kart_race_fades.courses, 1.0F, FADE_STEP)) {
            kart_race_work.courses.moving = TRUE;
        }
    } else {
        if (menu_approach(&kart_race_fades.courses, 0.0F, FADE_STEP)) {
            kart_race_work.courses.moving = TRUE;
        }
    }
    if (kart_race_work.list.shown) {
        if (menu_approach(&kart_race_fades.list, 1.0F, FADE_STEP)) {
            kart_race_work.courses.moving = TRUE;
        }
        list_scroll.target = kart_race_work.entry;
        menu_scroll_step(&list_scroll);
    } else {
        if (menu_approach(&kart_race_fades.list, 0.0F, FADE_STEP)) {
            kart_race_work.courses.moving = TRUE;
        }
    }
    if (!kart_race_work.question.shown) {
        kart_race_work.none_found = FALSE;
    }
    if (kart_race_work.question.shown && !kart_race_work.none_found) {
        if (menu_approach(&kart_race_fades.question, 1.0F, FADE_STEP)) {
            kart_race_work.question.moving = TRUE;
        }
    } else {
        if (menu_approach(&kart_race_fades.question, 0.0F, FADE_STEP)) {
            kart_race_work.question.moving = TRUE;
        }
        kart_race_work.choice = KART_RACE_YES;
    }
    /* @hack spelling: fading read again in the sum of the flags, where an or
     * into it would do */
    kart_race_work.fading =
        kart_race_work.fading | kart_race_work.backdrop.moving |
        kart_race_work.drivers.moving | kart_race_work.courses.moving |
        kart_race_work.list.moving | kart_race_work.question.moving;
}

static void kart_race_panel_draw(struct task *task)
{
    if (kart_race_fades.backdrop != 0.0F) {
        menu_fade_begin(kart_race_fades.backdrop);
        if (kart_race_work.download_backdrop) {
            menu_backdrop_draw(DOWNLOAD_BACKDROP, &backdrop_texlist,
                               BACKDROP_PRIORITY, MENU_BACKDROP_TINT);
        } else {
            menu_backdrop_draw(KART_BACKDROP, &backdrop_texlist,
                               BACKDROP_PRIORITY, MENU_BACKDROP_TINT);
        }
        kart_race_title_draw();
    }
    /* @hack spelling: the drivers', preview's, courses', list's and
     * question's fades tested alone, where compares with 0.0F would do */
    if (kart_race_fades.drivers) {
        kart_race_drivers_draw_all();
        if (kart_race_work.hint_shown) {
            message_sprite_draw(HINT_DRIVER, &hint_pos, 1.0F);
        }
    }
    if (kart_race_fades.preview) {
        kart_race_previews_draw();
    }
    if (kart_race_fades.courses) {
        kart_race_courses_draw();
        if (kart_race_work.hint_shown && !kart_race_work.reading) {
            message_sprite_draw(HINT_COURSE, &hint_pos, 1.0F);
        }
    }
    if (kart_race_work.hint_shown && kart_race_work.reading) {
        message_sprite_draw(HINT_READING, &hint_pos, 1.0F);
    }
    if (kart_race_fades.list) {
        kart_race_list_draw();
        if (kart_race_work.hint_shown) {
            if (menu_control.mode == MENU_MODE_KART_RACE) {
                message_sprite_draw(HINT_KART_LIST, &hint_pos, 1.0F);
            } else {
                message_sprite_draw(HINT_DOWNLOAD_LIST, &hint_pos, 1.0F);
            }
        }
    }
    if (kart_race_fades.question) {
        kart_race_question_draw();
    }
    if (kart_race_work.none_found) {
        NJS_POINT3 pos = {SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXT_PRIORITY};

        kart_race_sprite_draw(NONE_FOUND_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
        if (menu_control.mode == MENU_MODE_KART_RACE) {
            message_sprite_draw(HINT_NO_COURSE, &hint_pos, 1.0F);
        } else {
            message_sprite_draw(HINT_NO_DOWNLOAD, &hint_pos, 1.0F);
        }
    }
}

static void kart_race_panel_destroy(struct task *task)
{
    kart_race_panel_release();
}

void kart_race_panel_create(void)
{
    struct task *task;
    Uint8 *byte;
    int left;
    Int i;

    task = task_create(TASK_WORK, TASK_LEVEL, kart_race_panel_step,
                       KART_PANEL_TASK_NAME);
    task->draw = kart_race_panel_draw;
    task->destructor = kart_race_panel_destroy;
    panel_task = task;
    byte = (Uint8 *)&kart_race_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(kart_race_work); left != 0; left--) {
        *byte++ = 0;
    }
    kart_race_panel_reset();
    for (i = 0; i < KART_RACE_CELLS; i++) {
        if (i != BAR_CELL) {
            kart_race_cells[i].cx = kart_race_cells[i].sx / 2;
            kart_race_cells[i].cy = kart_race_cells[i].sy / 2;
        }
    }
}

void kart_race_panel_reset(void)
{
    Sint32 loaded = kart_race_work.loaded;
    Uint8 *byte = (Uint8 *)&kart_race_work;
    int left;

    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(kart_race_work); left != 0; left--) {
        *byte++ = 0;
    }
    kart_race_fades = fades_start;
    kart_race_work.loaded = loaded;
}

void kart_race_panel_kill(void)
{
    if (panel_task) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

void kart_race_panel_load(void)
{
    if (!kart_race_work.loaded) {
        texture_load_pvm(KART_PANEL_ARCHIVE, &texlist);
        texture_load_pvm(KART_BACKDROP_ARCHIVE, &backdrop_texlist);
        njLoadTexturePvmFile(KART_MODEL_ARCHIVE, &kart_texlist);
        kart_race_work.loaded = TRUE;
    }
}

void kart_race_panel_release(void)
{
    if (kart_race_work.loaded) {
        njReleaseTexture(&texlist);
        njReleaseTexture(&backdrop_texlist);
        njReleaseTexture(&kart_texlist);
        kart_race_work.loaded = FALSE;
    }
}

void kart_race_panel_set_list(struct download_entry *entries, int count)
{
    kart_race_fades.top = 0;
    kart_race_fades.cursor = 0;
    kart_race_work.entry = 0;
    kart_race_work.entry_count = count;
    kart_race_list = entries;
}
