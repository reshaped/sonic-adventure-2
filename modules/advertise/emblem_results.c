/* the emblem results' screen, the task emblemResultExec. it also holds the
 * world ranking's pages, places and dialogs, which nothing in the program
 * shows */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/emblem_results.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/vector.h"

#include "advertise/emblem_results/text.inc"

/* destroy the task, where emblem_results_create made one that this hasn't
 * destroyed. nothing calls emblem_results_kill, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void emblem_results_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
#define TEXTURES 32
/* a fade runs from 0 to 1 in FADE_STEPS steps */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
#define BACKDROP_PRIORITY (-10000.0F)
#define PANEL_PRIORITY (-500.0F)
/* the x of the screen's middle */
#define SCREEN_MIDDLE 320.0F
/* where the time window and the totals' window draw */
#define TIME_WINDOW_X 527.0F
#define TIME_WINDOW_Y 66.0F
#define TOTALS_X 444.0F
#define TOTALS_Y 121.0F

/* the first of the backdrop's eight textures, w_ranking_bg0 */
#define BACKDROP_TEXTURE 24
/* indexes of the array cells, which holds CELLS. a line's plate is the
 * LINE_PLATE_CELLS from CELL_LINE_PLATE. the title is the TITLE_CELLS from
 * CELL_TITLE, or from CELL_RANKING_TITLE under the work's ranking */
#define CELLS 257
#define CELL_RANKING_TITLE 71
#define CELL_RANKING_LABEL 75
#define CELL_TOTALS_LABEL 76
/* a place's suffix: for 1, 2, 3 and any other place, and Japanese's for
 * every place */
#define CELL_SUFFIX_1 77
#define CELL_SUFFIX_2 78
#define CELL_SUFFIX_3 79
#define CELL_SUFFIX_N 80
#define CELL_SEPARATOR 81 /* between the count of emblems and SAVE_EMBLEMS */
#define CELL_SUFFIX_JAPANESE 82
/* a suffix other than Japanese's draws this many pixels lower */
#define SUFFIX_DROP 2.0F
#define CELL_EMBLEMS_LABEL 83
#define CELL_TIME_WINDOW 135
#define CELL_ARROW 136
/* the medals of places 1, 2 and 3 */
#define CELL_MEDAL_2 137
#define CELL_MEDAL_1 138
#define CELL_MEDAL_3 139
#define CELL_LINE_PLATE 140
#define LINE_PLATE_CELLS 2
#define CELL_PAGE_PLATE 142
#define CELL_TITLE 148
#define TITLE_CELLS 4
#define CELL_BOX 152
/* the name of a Chao, kart or boss line while it is locked */
#define CELL_LOCKED_NAME 171
#define CELL_TOTALS_WINDOW 186
#define CELL_CROSS 253
#define CELL_DIAL_UP 254
/* the dialog's plate, a cell PLATE_SIZE pixels tall. the plate draws
 * PLATE_WIDTH_SCALE times as wide as the cell. it draws one
 * dialog_line_height taller than the dialog's lines of text, and PLATE_DEPTH
 * behind them */
#define CELL_DIALOG_PLATE 255
#define PLATE_SIZE 256.0F
#define PLATE_WIDTH_SCALE 2.0F
#define PLATE_DEPTH 2.0F
#define CELL_DIALOG_ARROW 256
#define DIALOG_PRIORITY (-1.0F)

/* the lines the window shows at once */
#define SHOWN_LINES 6
/* the digits share_data_number_draw_right gives a place and a count of
 * emblems */
#define PLACE_DIGITS 6
#define RANK_NUMBER_SCALE 0.7F
#define EMBLEMS_DIGITS 3
/* the work's time and save_data.emblem_time count steps */
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define STEPS_PER_MINUTE (SECONDS_PER_MINUTE * STEPS_PER_SECOND)
#define STEPS_PER_HOUR (MINUTES_PER_HOUR * STEPS_PER_MINUTE)
#define HUNDREDTHS_PER_SECOND 100
/* the most the totals' time draws. a time of more than MOST_TWO_DIGIT_HOURS
 * hours draws its hours in three digits */
#define MOST_HOURS 999
#define MOST_MINUTES (MINUTES_PER_HOUR - 1)
#define MOST_SECONDS (SECONDS_PER_MINUTE - 1)
#define MOST_TWO_DIGIT_HOURS 99
/* the world ranking's two pages whose lines draw places */
#define RANKING_PLACES_1 5
#define RANKING_PLACES_2 6

/* dialogs of message_boxes. DIALOG_DIALING turns the dial-up icon,
 * ICON_RISE pixels above the dialog's top, SPIN_STEP degrees on each draw */
#define DIALOG_DIALING 1
#define DIALOG_UNK_3 3
#define ICON_RISE 100.0F
#define SPIN_STEP 6

/* the lines' messages, numbers of message_tables. a stage's line takes its
 * character's */
#define MESSAGE_SONIC 540
#define MESSAGE_TAILS 541
#define MESSAGE_KNUCKLES 542
#define MESSAGE_SHADOW 543
#define MESSAGE_EGGMAN 544
#define MESSAGE_ROUGE 545
#define MESSAGE_SONIC_TOP 546
#define MESSAGE_TAILS_TOP 547
#define MESSAGE_KNUCKLES_TOP 548
#define MESSAGE_SHADOW_TOP 549
#define MESSAGE_EGGMAN_TOP 550
#define MESSAGE_ROUGE_TOP 551
#define MESSAGE_STORY 552
#define MESSAGE_RELAY 553
#define MESSAGE_CHAO 554
#define MESSAGE_KART 555
#define MESSAGE_BOSS 556

/* the indexes of emblem_results_view's levels: a fade for each panel but the
 * dialog */
enum {
    FADE_TITLE,
    FADE_PAGE_NAME,
    FADE_TIME_WINDOW,
    FADE_TOTALS,
    FADE_LINES,
    FADE_BOXES,
    FADE_RANKS,
    FADE_UNK_D4,
    FADES
};

/* a line's kind, which says what its emblems are and what choosing it
 * plays */
enum {
    LINE_NONE = -1, /* a line that never shows */
    LINE_STAGE, /* a stage's missions, each won once it has a rank */
    /* a character's award for SAVE_RANK_TOP in every mission of the
     * character's stages */
    LINE_SONIC_TOP,
    LINE_KNUCKLES_TOP,
    LINE_TAILS_TOP,
    LINE_SHADOW_TOP,
    LINE_ROUGE_TOP,
    LINE_EGGMAN_TOP,
    LINE_HERO_STORY, /* the story's award */
    LINE_DARK_STORY,
    LINE_LAST_STORY,
    LINE_CHAO, /* save_data.chao_emblems */
    LINE_KART, /* the kart records' emblems */
    LINE_BOSS /* the SAVE_BOSS_AWARDS awards from SAVE_AWARD_FIRST_BOSS */
};
#define NO_CHARACTER (-1)
/* a name's cells, by language: Japanese, and every other */
#define NAME_LANGUAGES 2

/* a line of a page. box j holds its emblem while won[j] isn't 0 */
struct emblem_results_line {
    Sint32 stage;
    Sint32 character; /* who plays the line, or NO_CHARACTER */
    Sint32 names[NAME_LANGUAGES]; /* the name's cell */
    Sint32 kind;
    Sint32 boxes;
    /* room for the Chao line's boxes, the most a line has */
    Sint8 won[SAVE_CHAO_EMBLEMS];
};
struct emblem_results_page {
    struct emblem_results_line *lines;
    Sint32 count;
};
/* the rows the view keeps for each page */
#define PAGE_LINES 32

/* the screen's fades and its window's scroll. rows holds the cells of each
 * shown line's name, by language and page */
struct emblem_results_view {
    Float levels[FADES];
    Sint32 top;
    Sint32 cursor;
    struct menu_row rows[NAME_LANGUAGES][EMBLEM_RESULTS_PAGES][PAGE_LINES];
};

/* a window's lines for the boxes' and the ranks' draws, placed as a
 * struct menu_list places its rows */
struct line_list {
    const struct emblem_results_line *lines;
    Sint32 count;
    NJS_POINT3 pos;
    NJS_POINT3 step;
    menu_sprite_drawer draw;
};

/* a dialog: its messages, one under another on a plate */
#define BOX_MESSAGES 5
#define BOX_CHOICES 3
struct message_box {
    Sint32 messages[BOX_MESSAGES]; /* ended by one below 0 */
    Sint32 arrow; /* 0 for none */
    Float y; /* the dialog's top */
    /* the arrow's x from the screen's middle, by language and choice */
    Float arrow_x[TEXT_LANGUAGES][BOX_CHOICES];
};

/* Emblem pages show up to 24 lines. World-ranking pages have room for 48. */
#define EMBLEMS_ROOM 24
#define RANKING_ROOM 48
#define TEXT_ROOM 1024
static struct emblem_results_line emblem_results_hero_shown[EMBLEMS_ROOM];
static struct emblem_results_line emblem_results_dark_shown[EMBLEMS_ROOM];
static struct emblem_results_line emblem_results_last_shown[EMBLEMS_ROOM];
static struct emblem_results_line emblem_results_others_shown[EMBLEMS_ROOM];
static struct emblem_results_line emblem_results_ranking_shown_4[RANKING_ROOM];
static struct emblem_results_line emblem_results_ranking_shown_5[RANKING_ROOM];
static struct emblem_results_line emblem_results_ranking_shown_6[RANKING_ROOM];
static struct emblem_results_line emblem_results_ranking_shown_7[RANKING_ROOM];
static struct emblem_results_view emblem_results_view;
static char emblem_results_text[TEXT_ROOM]; /* a dialog message's text */
static Sint32 emblem_results_spin; /* the dial-up icon's turn, in degrees */

/* the stages of the lines of RANKING_PLACES_1 and RANKING_PLACES_2, in
 * order, ended by -1 */
/* clang-format off */
static Sint32 ranking_stages[] = {
    STAGE_CITY_ESCAPE, STAGE_METAL_HARBOR, STAGE_GREEN_FOREST,
        STAGE_PYRAMID_CAVE, STAGE_CRAZY_GADGET, STAGE_FINAL_RUSH,
    STAGE_PRISON_LANE, STAGE_MISSION_STREET, STAGE_CART_1, STAGE_HIDDEN_BASE,
        STAGE_ETERNAL_ENGINE,
    STAGE_WILD_CANYON, STAGE_PUMPKIN_HILL, STAGE_AQUATIC_MINE,
        STAGE_DEATH_CHAMBER, STAGE_METEOR_HERD,
    STAGE_RADICAL_HIGHWAY, STAGE_WHITE_JUNGLE, STAGE_SKY_RAIL,
        STAGE_FINAL_CHASE,
    STAGE_IRON_GATE, STAGE_SAND_OCEAN, STAGE_LOST_COLONY, STAGE_WEAPONS_BED,
        STAGE_COSMIC_WALL,
    STAGE_DRY_LAGOON, STAGE_EGG_QUARTERS, STAGE_SECURITY_HALL, STAGE_CART_1,
        STAGE_MAD_SPACE,
    STAGE_RELAY_SONIC, -1,
};
/* clang-format on */
/* where the work's message draws */
static NJS_POINT3 message_pos = {0.0F, 400.0F, -12.0F};
/* each page's lines shown, which emblem_results_gather fills */
static struct emblem_results_page shown_pages[EMBLEM_RESULTS_PAGES] = {
    {emblem_results_hero_shown, 0},      {emblem_results_dark_shown, 0},
    {emblem_results_last_shown, 0},      {emblem_results_others_shown, 0},
    {emblem_results_ranking_shown_4, 0}, {emblem_results_ranking_shown_5, 0},
    {emblem_results_ranking_shown_6, 0}, {emblem_results_ranking_shown_7, 0},
};
/* the view emblem_results_reset sets: every fade at 0, and the window and
 * its cursor at the first line */
static struct emblem_results_view view_start = {{0.0F}, 0, 0, {{{{0, 0}}}}};
static NJS_POINT3 title_pos = {100.0F, 66.0F, -100.0F};
/* the task emblem_results_create made, or NULL */
static struct task *results_task = NULL;
/* the cell of each page's name, by language */
static Sint32 page_names[NAME_LANGUAGES][EMBLEM_RESULTS_PAGES] = {
    {153, 154, 155, 156, 84, 85, 86, 87},
    {157, 158, 159, 160, 106, 107, 108, 109},
};
/* where the page's name draws. its right arrow lies right_arrow_x from it,
 * and its left arrow left_arrow_x from the right one */
static Float page_name_x = 180.0F;
static Float page_name_y = 105.0F;
static Float right_arrow_x = 120.0F;
static Float left_arrow_x = -238.0F;
/* where the cursor's line draws its character's icon, from the line */
static Float icon_x = -160.0F;
static Float icon_y = 0.0F;
/* each character's icon's cell */
static Sint32 character_icons[PLAYER_EGGMAN_MECH + 1] = {
    176, 172, 177, 173, 178, 174, 177, 173,
};
/* the spacing and scales of a line's boxes and their emblems. a line of more
 * than MISSIONS boxes draws them by the small_ values */
static Float box_step = 38.0F;
static Float box_emblem_scale = 0.7F;
static Float small_box_step = 18.0F;
static Float small_box_emblem_scale = 0.35F;
static Float small_box_scale = 0.5F;
/* on the ranks' panel, a place's number lies rank_number_x from its medal.
 * its suffix lies rank_suffix_x from the number */
static Float rank_number_x = 120.0F;
static Float rank_suffix_x = 30.0F;
/* where the window's first line draws, and the step to the next. the
 * cursor's line draws at lit_line_scale */
static Float lines_x = 245.0F;
static Float lines_y = 176.0F;
static Float lines_z = -500.0F;
static Float line_step_x = 0.0F;
static Float line_step_y = 40.0F;
static Float line_step_z = 0.0F;
static Float lit_line_scale = 1.2F;
/* the time window's time, from the window */
static Float time_x = -33.0F;
static Float time_y = 3.0F;
static Float time_scale = 0.7F;
/* where the totals' parts draw. the label is placed from the totals' window,
 * and each other part from the one drawn before it */
static Float totals_label_x = -80.0F;
static Float totals_label_y = -10.0F;
static Float ranking_label_x = 10.0F;
static Float ranking_label_y = 25.0F;
static Float emblems_label_x = -8.0F;
static Float emblems_label_y = 25.0F;
static Float emblems_x = 110.0F;
static Float emblems_y = -22.0F;
static Float slash_x = 30.0F;
static Float slash_y = 3.0F;
static Float all_emblems_x = 55.0F;
static Float all_emblems_y = 0.0F;
static Float emblem_time_x = -112.0F;
static Float emblem_time_y = 22.0F;
static Float ranking_time_x = 65.0F;
static Float ranking_time_y = 0.0F;
/* the ranking's place: its suffix and its number, each from the ranking's
 * time */
static Float place_suffix_x = 110.0F;
static Float place_suffix_y = -26.0F;
static Float place_x = 82.0F;
static Float place_y = -25.0F;
/* the scale each of the totals' numbers draws at. no code reads
 * dat_8c5ac7a0 */
static Float emblems_scale = 1.2F;
static Float all_emblems_scale = 0.8F;
static Float ranking_time_scale = 0.7F;
static Float dat_8c5ac7a0 = 1.0F;
static Float place_scale = 1.0F;
/* each line's boxes, from the line */
static Float boxes_x = 150.0F;
static Float boxes_y = 0.0F;
/* the dialogs, by emblem_results_work.dialog_box */
/* clang-format off */
static struct message_box message_boxes[] = {
    {{225, 226, 228, -1, -1}, 2, 80.0F,
     {{-35.0F, 35.0F, 0.0F}, {-35.0F, 35.0F, 0.0F}, {-35.0F, 35.0F, 0.0F},
      {-35.0F, 35.0F, 0.0F}, {-35.0F, 35.0F, 0.0F}}},
    {{232, 233, -1, -1, -1}, 1, 300.0F, {{0.0F}}},
    {{234, 235, 236, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{252, -1, -1, -1, -1}, 0, 250.0F, {{0.0F}}},
    {{-1, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{255, 256, -1, -1, -1}, 1, 100.0F, {{0.0F}}},
    {{257, 258, -1, -1, -1}, 2, 100.0F, {{0.0F}}},
    {{259, 260, -1, -1, -1}, 2, 100.0F, {{0.0F}}},
    {{261, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{262, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{263, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{266, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{263, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{267, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{310, 311, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{312, 313, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{314, 315, 316, 317, -1}, 2, 100.0F, {{0.0F}}},
    {{237, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{239, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{245, 246, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{239, 242, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{239, 244, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{247, -1, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{248, 249, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
    {{253, 254, -1, -1, -1}, 0, 100.0F, {{0.0F}}},
};
/* clang-format on */
static Float dialog_line_height = 28.0F;
static Float dialog_arrow_x = 0.0F;
static Float dialog_text_y = 0.0F;
static Uint32 dialog_text_color = 0xff000000;
static NJS_POINT3 cross_pos = {320.0F, 200.0F, -20.0F};
/* the stages emblem_results_short_of_top_find searches for each character's
 * top line, in order, each ended by -1 */
/* clang-format off */
static Sint32 sonic_stages[] = {
    STAGE_CITY_ESCAPE, STAGE_METAL_HARBOR, STAGE_GREEN_FOREST,
    STAGE_PYRAMID_CAVE, STAGE_CRAZY_GADGET, STAGE_FINAL_RUSH, -1,
};
static Sint32 knuckles_stages[] = {
    STAGE_WILD_CANYON, STAGE_PUMPKIN_HILL, STAGE_AQUATIC_MINE,
    STAGE_DEATH_CHAMBER, STAGE_METEOR_HERD, -1,
};
static Sint32 tails_stages[] = {
    STAGE_PRISON_LANE, STAGE_MISSION_STREET, STAGE_CART_1, STAGE_HIDDEN_BASE,
    STAGE_ETERNAL_ENGINE, -1,
};
static Sint32 shadow_stages[] = {
    STAGE_RADICAL_HIGHWAY, STAGE_WHITE_JUNGLE, STAGE_SKY_RAIL,
    STAGE_FINAL_CHASE, -1,
};
static Sint32 rouge_stages[] = {
    STAGE_DRY_LAGOON, STAGE_EGG_QUARTERS, STAGE_SECURITY_HALL, STAGE_CART_1,
    STAGE_MAD_SPACE, -1,
};
static Sint32 eggman_stages[] = {
    STAGE_IRON_GATE, STAGE_SAND_OCEAN, STAGE_LOST_COLONY, STAGE_WEAPONS_BED,
    STAGE_COSMIC_WALL, -1,
};
/* clang-format on */
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/emblem_results/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/emblem_results/texlist.inc"
};
/* emblem_results_create sets each cell's center, cx and cy, to its
 * middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/emblem_results/cells.inc"
};

static struct emblem_results_line hero_lines[20] = {
#include "advertise/emblem_results/hero_lines.inc"
};
static struct emblem_results_line dark_lines[18] = {
#include "advertise/emblem_results/dark_lines.inc"
};
static struct emblem_results_line last_lines[2] = {
#include "advertise/emblem_results/last_lines.inc"
};
static struct emblem_results_line others_lines[3] = {
#include "advertise/emblem_results/others_lines.inc"
};
static struct emblem_results_line ranking_lines_4[1] = {
#include "advertise/emblem_results/ranking_lines_4.inc"
};
static struct emblem_results_line ranking_lines_5[31] = {
#include "advertise/emblem_results/ranking_lines_5.inc"
};
static struct emblem_results_line ranking_lines_6[31] = {
#include "advertise/emblem_results/ranking_lines_6.inc"
};
static struct emblem_results_line ranking_lines_7[1] = {
#include "advertise/emblem_results/ranking_lines_7.inc"
};
/* by page, every line it has, which emblem_results_gather picks from */
KEPT static struct emblem_results_page pages[EMBLEM_RESULTS_PAGES] = {
#include "advertise/emblem_results/pages.inc"
};

static void emblem_results_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
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

static void emblem_results_sprite_draw_stretched(Int cell,
                                                 const NJS_POINT2 *scale,
                                                 const NJS_POINT3 *pos,
                                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale->x;
    sprite.sy = scale->y;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* ang turns the cell only where attr takes NJD_SPRITE_ANGLE */
static void emblem_results_sprite_draw_turned(Int cell, Float scale,
                                              const NJS_POINT3 *pos, Angle ang,
                                              Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = ang;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void emblem_results_page_name_draw(void)
{
    NJS_POINT3 pos;

    pos.x = page_name_x;
    pos.y = page_name_y;
    pos.z = PANEL_PRIORITY;
    emblem_results_sprite_draw(CELL_PAGE_PLATE, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.z += 1.0F;
    emblem_results_sprite_draw(
        page_names[game_state.text_language > 0][emblem_results_work.page],
        1.0F, &pos, MENU_SPRITE_ATTR);
    pos.x += right_arrow_x;
    emblem_results_sprite_draw_turned(CELL_ARROW, 1.0F, &pos, QUARTER_TURN,
                                      MENU_SPRITE_ATTR | NJD_SPRITE_ANGLE);
    pos.x += left_arrow_x;
    emblem_results_sprite_draw_turned(CELL_ARROW, 1.0F, &pos, -QUARTER_TURN,
                                      MENU_SPRITE_ATTR | NJD_SPRITE_ANGLE);
}

/* draw list's window as menu_list_draw does, each row on a plate. the
 * cursor's row draws at scale. it draws the icon of lines[row]'s character,
 * where it has one and isn't locked. list's lit_back and dim_back go
 * unread */
static void emblem_results_lines_draw(const struct menu_list *list,
                                      const struct menu_scroll *scroll,
                                      Float scale,
                                      const struct emblem_results_line *lines)
{
    NJS_POINT3 pos;
    NJS_POINT3 icon_pos;
    struct menu_sprite_row plate;
    Int row;
    Int i;

    pos = list->pos;
    for (i = 0; i < scroll->shown; i++) {
        row = *scroll->top + i;
        if (row >= list->count) {
            break;
        }
        menu_tint_begin(emblem_results_view.levels[FADE_LINES], 0.0F, 0.0F,
                        0.0F);
        if (i == *scroll->cursor) {
            if (list->rows[row].lit >= 0) {
                list->draw(list->rows[row].lit +
                               (list->language_offset *
                                (game_state.text_language > 0)),
                           scale, &pos, MENU_SPRITE_ATTR);
            }
        } else if (list->rows[row].dim >= 0) {
            list->draw(list->rows[row].dim + (list->language_offset *
                                              (game_state.text_language > 0)),
                       1.0F, &pos, MENU_SPRITE_ATTR);
        }
        menu_fade_begin(emblem_results_view.levels[FADE_LINES]);
        pos.z -= 1.0F;
        plate.tanim = cells;
        plate.first = CELL_LINE_PLATE;
        plate.count = LINE_PLATE_CELLS;
        plate.pos = &pos;
        plate.draw = emblem_results_sprite_draw;
        menu_sprite_row_draw(&plate);
        pos.z = list->pos.z;
        icon_pos = pos;
        if (i == *scroll->cursor) {
            icon_pos.x += icon_x;
            icon_pos.y += icon_y;
            if (list->rows[row].lit != CELL_LOCKED_NAME &&
                lines[row].character >= 0) {
                emblem_results_sprite_draw(
                    character_icons[lines[row].character], 1.0F, &icon_pos,
                    MENU_SPRITE_ATTR);
            }
        }
        vector_sum(&pos, &list->step, &pos);
    }
}

static void emblem_results_boxes_draw(const struct line_list *list,
                                      const struct menu_scroll *scroll)
{
    NJS_POINT3 pos;
    NJS_POINT3 box;
    Int row;
    Int i;
    Int j;

    pos = list->pos;
    for (i = 0; i < scroll->shown; i++) {
        row = *scroll->top + i;
        if (row >= list->count) {
            return;
        }
        box = pos;
        for (j = 0; j < list->lines[row].boxes; j++) {
            if (list->lines[row].boxes > MISSIONS) {
                list->draw(CELL_BOX, small_box_scale, &box, MENU_SPRITE_ATTR);
                box.z += 1.0F;
                if (list->lines[row].won[j]) {
                    share_data_sprite_draw(SHARE_DATA_EMBLEM_CELL,
                                           small_box_emblem_scale, &box,
                                           MENU_SPRITE_ATTR);
                }
                box.z -= 1.0F;
                box.x += small_box_step;
            } else {
                list->draw(CELL_BOX, 1.0F, &box, MENU_SPRITE_ATTR);
                box.z += 1.0F;
                if (list->lines[row].won[j]) {
                    share_data_sprite_draw(SHARE_DATA_EMBLEM_CELL,
                                           box_emblem_scale, &box,
                                           MENU_SPRITE_ATTR);
                }
                box.z -= 1.0F;
                box.x += box_step;
            }
        }
        vector_sum(&pos, &list->step, &pos);
    }
}

/* draw each line's place on the world ranking's pages of places. nothing
 * shows the ranks' panel, so this never runs */
static void emblem_results_ranks_draw(const struct line_list *list,
                                      const struct menu_scroll *scroll)
{
    NJS_POINT3 pos;
    NJS_POINT3 box;
    Int row;
    Int i;
    Int place;

#ifdef NONMATCHING
    /* this side draws no place, where the image reads place unset */
    place = 0;
#endif
    pos = list->pos;
    /* @hack spelling: the page tested by a switch, where an if would do */
    switch (emblem_results_work.page) {
    case RANKING_PLACES_1:
    case RANKING_PLACES_2:
        break;
    default:
        return;
    }
    for (i = 0; i < scroll->shown; i++) {
        row = *scroll->top + i;
        if (row >= list->count) {
            return;
        }
        box = pos;
        /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
        if (place == 0) {
            continue;
        }
        menu_fade_begin(emblem_results_view.levels[FADE_RANKS]);
        switch (place) {
        case 1:
            list->draw(CELL_MEDAL_1, 1.0F, &box, MENU_SPRITE_ATTR);
            break;
        case 2:
            list->draw(CELL_MEDAL_2, 1.0F, &box, MENU_SPRITE_ATTR);
            break;
        case 3:
            list->draw(CELL_MEDAL_3, 1.0F, &box, MENU_SPRITE_ATTR);
            break;
        }
        box.x += rank_number_x;
        menu_tint_begin(emblem_results_view.levels[FADE_RANKS], 0.0F, 0.0F,
                        0.0F);
        share_data_number_draw_right(&box, place, PLACE_DIGITS,
                                     RANK_NUMBER_SCALE);
        box.x += rank_suffix_x;
        if (game_state.text_language != TEXT_JAPANESE) {
            switch (place) {
            case 1:
                list->draw(CELL_SUFFIX_1, 1.0F, &box, MENU_SPRITE_ATTR);
                break;
            case 2:
                list->draw(CELL_SUFFIX_2, 1.0F, &box, MENU_SPRITE_ATTR);
                break;
            case 3:
                list->draw(CELL_SUFFIX_3, 1.0F, &box, MENU_SPRITE_ATTR);
                break;
            default:
                list->draw(CELL_SUFFIX_N, 1.0F, &box, MENU_SPRITE_ATTR);
                break;
            }
        } else {
            list->draw(CELL_SUFFIX_JAPANESE, 1.0F, &box, MENU_SPRITE_ATTR);
        }
        /* NOLINTEND(clang-diagnostic-uninitialized) */
        vector_sum(&pos, &list->step, &pos);
    }
}

/* the lines' panel moves the window's cursor a row toward the work's line.
 * the boxes' and the ranks' panels draw by the scroll it leaves */
static void emblem_results_lines_panel_draw(void)
{
    struct menu_scroll scroll;
    struct menu_list list;

    scroll.top = &emblem_results_view.top;
    scroll.cursor = &emblem_results_view.cursor;
    scroll.target = emblem_results_work.line;
    scroll.shown = SHOWN_LINES;
    menu_scroll_step(&scroll);
    list.rows =
        emblem_results_view
            .rows[game_state.text_language > 0][emblem_results_work.page];
    list.count = shown_pages[emblem_results_work.page].count;
    list.language_offset = 0;
    list.pos.x = lines_x;
    list.pos.y = lines_y;
    list.pos.z = lines_z;
    list.step.x = line_step_x;
    list.step.y = line_step_y;
    list.step.z = line_step_z;
    list.draw = emblem_results_sprite_draw;
    emblem_results_lines_draw(&list, &scroll, lit_line_scale,
                              shown_pages[emblem_results_work.page].lines);
    menu_fade_begin(emblem_results_view.levels[FADE_LINES]);
}

static void emblem_results_time_window_draw(void)
{
    NJS_POINT3 pos = {TIME_WINDOW_X, TIME_WINDOW_Y, PANEL_PRIORITY};
    struct save_time time;

    emblem_results_sprite_draw(CELL_TIME_WINDOW, 1.0F, &pos, MENU_SPRITE_ATTR);
    time.minute = emblem_results_work.time / STEPS_PER_MINUTE;
    time.second =
        emblem_results_work.time / STEPS_PER_SECOND % SECONDS_PER_MINUTE;
    time.hundredths = emblem_results_work.time % STEPS_PER_SECOND *
                      HUNDREDTHS_PER_SECOND / STEPS_PER_SECOND;
    pos.x += time_x;
    pos.y += time_y;
    pos.z += 1.0F;
    menu_tint_begin(emblem_results_view.levels[FADE_TIME_WINDOW], 0.0F, 0.0F,
                    0.0F);
    share_data_time_draw_short(&pos, &time, time_scale);
}

/* the totals: the count of emblems out of SAVE_EMBLEMS, and
 * save_data.emblem_time. a time of more than MOST_HOURS hours draws as
 * MOST_HOURS hours, MOST_MINUTES minutes and MOST_SECONDS seconds. under the
 * work's ranking, the totals are the ranking's time and its place,
 * save_data.ranking_place */
static void emblem_results_totals_draw(void)
{
    NJS_POINT3 pos = {TOTALS_X, TOTALS_Y, PANEL_PRIORITY};
    NJS_POINT3 place_pos;
    struct save_time time;
    struct share_data_clock clock;

    emblem_results_sprite_draw(CELL_TOTALS_WINDOW, 1.0F, &pos,
                               MENU_SPRITE_ATTR);
    pos.x += totals_label_x;
    pos.y += totals_label_y;
    pos.z += 1.0F;
    emblem_results_sprite_draw(CELL_TOTALS_LABEL, 1.0F, &pos, MENU_SPRITE_ATTR);
    if (emblem_results_work.ranking) {
        pos.x += ranking_label_x;
        pos.y += ranking_label_y;
        emblem_results_sprite_draw(CELL_RANKING_LABEL, 1.0F, &pos,
                                   MENU_SPRITE_ATTR);
        pos.x += ranking_time_x;
        pos.y += ranking_time_y;
        /* time is unset here: only the else branch below sets it. nothing
         * sets the work's ranking to anything but 0, so this never runs */
        /* NOLINTNEXTLINE(clang-diagnostic-uninitialized-const-pointer) */
        share_data_time_draw_alt(&pos, &time, ranking_time_scale);
        place_pos = pos;
        if (game_state.text_language != TEXT_JAPANESE) {
            pos.x += place_suffix_x;
            pos.y += place_suffix_y + SUFFIX_DROP;
            switch (save_data.ranking_place) {
            case 1:
                emblem_results_sprite_draw(CELL_SUFFIX_1, 1.0F, &pos,
                                           MENU_SPRITE_ATTR);
                break;
            case 2:
                emblem_results_sprite_draw(CELL_SUFFIX_2, 1.0F, &pos,
                                           MENU_SPRITE_ATTR);
                break;
            case 3:
                emblem_results_sprite_draw(CELL_SUFFIX_3, 1.0F, &pos,
                                           MENU_SPRITE_ATTR);
                break;
            default:
                emblem_results_sprite_draw(CELL_SUFFIX_N, 1.0F, &pos,
                                           MENU_SPRITE_ATTR);
                break;
            }
        } else {
            pos.x += place_suffix_x;
            pos.y += place_suffix_y;
            emblem_results_sprite_draw(CELL_SUFFIX_JAPANESE, 1.0F, &pos,
                                       MENU_SPRITE_ATTR);
        }
        place_pos.x += place_x;
        place_pos.y += place_y;
        share_data_number_draw_right(&place_pos, save_data.ranking_place,
                                     PLACE_DIGITS, place_scale);
    } else {
        pos.x += emblems_label_x;
        pos.y += emblems_label_y;
        emblem_results_sprite_draw(CELL_EMBLEMS_LABEL, 1.0F, &pos,
                                   MENU_SPRITE_ATTR);
        pos.x += emblems_x;
        pos.y += emblems_y;
        share_data_number_draw_right(&pos, save_data.emblems, EMBLEMS_DIGITS,
                                     emblems_scale);
        pos.x += slash_x;
        pos.y += slash_y;
        emblem_results_sprite_draw(CELL_SEPARATOR, 1.0F, &pos,
                                   MENU_SPRITE_ATTR);
        pos.x += all_emblems_x;
        pos.y += all_emblems_y;
        share_data_number_draw_right(&pos, SAVE_EMBLEMS, EMBLEMS_DIGITS,
                                     all_emblems_scale);
        pos.x += emblem_time_x;
        pos.y += emblem_time_y;
        clock.hour = save_data.emblem_time / STEPS_PER_HOUR;
        clock.minute =
            save_data.emblem_time / STEPS_PER_MINUTE % MINUTES_PER_HOUR;
        clock.second =
            save_data.emblem_time / STEPS_PER_SECOND % SECONDS_PER_MINUTE;
        if (clock.hour > MOST_HOURS) {
            clock.hour = MOST_HOURS;
            clock.minute = MOST_MINUTES;
            clock.second = MOST_SECONDS;
            /* @bug logic: a time of more than MOST_HOURS hours draws twice,
             * here and again below (the calls at 0x8c518ad6 and
             * 0x8c518aea) */
            share_data_clock_draw(&pos, &clock, time_scale);
        }
        if (clock.hour > MOST_TWO_DIGIT_HOURS) {
            share_data_clock_draw(&pos, &clock, time_scale);
        } else {
            /* share_data_time_draw draws each of a save_time's fields in two
             * places. these hold the clock's hours, minutes and seconds */
            time.minute = clock.hour;
            time.second = clock.minute;
            time.hundredths = clock.second;
            share_data_time_draw(&pos, &time, time_scale);
        }
    }
}

static void emblem_results_boxes_panel_draw(void)
{
    struct menu_scroll scroll;
    struct line_list list;

    scroll.top = &emblem_results_view.top;
    scroll.cursor = &emblem_results_view.cursor;
    scroll.target = emblem_results_work.line;
    scroll.shown = SHOWN_LINES;
    list.lines = shown_pages[emblem_results_work.page].lines;
    list.count = shown_pages[emblem_results_work.page].count;
    list.pos.x = lines_x + boxes_x;
    list.pos.y = lines_y + boxes_y;
    list.pos.z = lines_z + 1.0F;
    list.step.x = line_step_x;
    list.step.y = line_step_y;
    list.step.z = line_step_z;
    list.draw = emblem_results_sprite_draw;
    emblem_results_boxes_draw(&list, &scroll);
}

static void emblem_results_ranks_panel_draw(void)
{
    struct menu_scroll scroll;
    struct line_list list;

    scroll.top = &emblem_results_view.top;
    scroll.cursor = &emblem_results_view.cursor;
    scroll.target = emblem_results_work.line;
    scroll.shown = SHOWN_LINES;
    list.lines = shown_pages[emblem_results_work.page].lines;
    list.count = shown_pages[emblem_results_work.page].count;
    list.pos.x = lines_x + boxes_x;
    list.pos.y = lines_y + boxes_y;
    list.pos.z = lines_z + 1.0F;
    list.step.x = line_step_x;
    list.step.y = line_step_y;
    list.step.z = line_step_z;
    list.draw = emblem_results_sprite_draw;
    emblem_results_ranks_draw(&list, &scroll);
}

static Int emblem_results_line_feeds_count(const char *text)
{
    Int feeds;

    feeds = 0;
    for (; *text; text++) {
        if (*text == '\n') {
            feeds++;
        }
    }
    return feeds;
}

/* copy message's text into out, ended by a 0, and return the arg after the
 * last one used. each ++ in the text copies the work's dialog_args[arg] in
 * its place, and takes the next arg. a NULL dialog_args[arg] copies nothing.
 * game_state.text_language must be below TEXT_LANGUAGES, unchecked: text is
 * read unset otherwise */
static Int emblem_results_message_format(Int message, Int arg, char *out)
{
    char *text;
    char *from;

#ifdef NONMATCHING
    /* GCC can't see that every path sets text: game_state.text_language is
     * below TEXT_LANGUAGES, as this function requires */
    text = "";
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        text = message_tables.japanese[message];
        break;
    case TEXT_ENGLISH:
        text = message_tables.english[message];
        break;
    case TEXT_FRENCH:
        text = message_tables.french[message];
        break;
    case TEXT_SPANISH:
        text = message_tables.spanish[message];
        break;
    case TEXT_GERMAN:
        text = message_tables.german[message];
        break;
    }
    while (*text != 0) {
        /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
        if (*text == '+' && text[1] == '+') {
            from = emblem_results_work.dialog_args[arg++];
            if (from != 0) {
                while (*from != 0) {
                    *out = *from;
                    from++;
                    out++;
                }
            }
            text += 2;
        } else {
            *out = *text;
            out++;
            text++;
        }
    }
    *out = 0;
    return arg;
}

/* draw box with its first line's top left at origin's x and y. on its last
 * line, its arrow marks the choice numbered column */
static void emblem_results_message_box_draw(const struct message_box *box,
                                            Int column,
                                            const NJS_POINT3 *origin)
{
    NJS_POINT3 pos;
    NJS_POINT2 scale;
    Int lines;
    Int arg;
    Int i;

    /* @hack spelling: i set before the for, where its first clause would
     * do */
    i = 0;
    lines = 0;
    arg = 0;
    for (; box->messages[i] >= 0; i++) {
        pos = *origin;
        pos.y += (lines * dialog_line_height) + dialog_text_y;
        arg = emblem_results_message_format(box->messages[i], arg,
                                            emblem_results_text);
        message_sprite_draw_text_color(emblem_results_text, &pos, 1.0F,
                                       &dialog_text_color);
        lines += emblem_results_line_feeds_count(emblem_results_text) + 1;
    }
    /* the loop above copied origin into pos, z and all. a box of no messages
     * leaves pos.z unset, but nothing shows a dialog */
    pos.x = origin->x + SCREEN_MIDDLE;
    pos.y = origin->y + (lines * dialog_line_height * 0.5F) + dialog_text_y;
    pos.z -= PLATE_DEPTH;
    scale.x = PLATE_WIDTH_SCALE;
    scale.y = (lines + 1) * dialog_line_height * (1.0F / PLATE_SIZE);
    emblem_results_sprite_draw_stretched(CELL_DIALOG_PLATE, &scale, &pos,
                                         NJD_SPRITE_ALPHA);
    if (box->arrow != 0) {
        pos.x = box->arrow_x[game_state.text_language][column] +
                dialog_arrow_x + SCREEN_MIDDLE;
        pos.y =
            origin->y + ((lines - 0.5F) * dialog_line_height) + dialog_text_y;
        pos.z += 1.0F;
        emblem_results_sprite_draw(CELL_DIALOG_ARROW, 1.0F, &pos,
                                   NJD_SPRITE_ALPHA);
    }
}

static void emblem_results_dialog_draw(void)
{
    NJS_POINT3 pos;

    emblem_results_spin += SPIN_STEP;
    pos.x = 0.0F;
    pos.y = message_boxes[emblem_results_work.dialog_box].y;
    pos.z = DIALOG_PRIORITY;
    switch (emblem_results_work.dialog_box) {
    case 0:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        emblem_results_message_box_draw(
            &message_boxes[emblem_results_work.dialog_box],
            emblem_results_work.dialog_column, &pos);
        break;
    case DIALOG_DIALING:
        emblem_results_message_box_draw(
            &message_boxes[emblem_results_work.dialog_box],
            emblem_results_work.dialog_column, &pos);
        pos.x = SCREEN_MIDDLE;
        pos.y -= ICON_RISE;
        emblem_results_sprite_draw_turned(
            CELL_DIAL_UP, 1.0F, &pos,
            (Angle)(emblem_results_spin * ANGLE_PER_DEGREE),
            NJD_SPRITE_ANGLE | NJD_SPRITE_ALPHA);
        break;
    case DIALOG_UNK_3:
        emblem_results_message_box_draw(
            &message_boxes[emblem_results_work.dialog_box],
            emblem_results_work.dialog_column, &pos);
        /* @hack redundant: the icon's place set for DIALOG_UNK_3, which
         * draws no icon */
        pos.x = SCREEN_MIDDLE;
        pos.y -= ICON_RISE;
        break;
    }
}

/* fade each panel in while it is shown, and out while it isn't. the dialog
 * has no fade. while the work isn't open, every panel and the message hide
 * first */
static void emblem_results_step(struct task *task)
{
    Bool (*approach)(Float *value, Float target, Float step);
    struct emblem_results_view *view;
    Bool moved;
    Float full;
    Float zero;
    /* @hack redundant: step as a register Float, where a Float would do */
    register Float step;

    emblem_results_work.fading = FALSE;
    emblem_results_work.title.moving = FALSE;
    emblem_results_work.page_name.moving = FALSE;
    emblem_results_work.time_window.moving = FALSE;
    emblem_results_work.totals.moving = FALSE;
    emblem_results_work.lines.moving = FALSE;
    emblem_results_work.boxes.moving = FALSE;
    emblem_results_work.ranks.moving = FALSE;
    emblem_results_work.dialog.moving = FALSE;
    emblem_results_work.unk_d4.moving = FALSE;
    if (emblem_results_work.open == FALSE) {
        emblem_results_work.title.shown = FALSE;
        emblem_results_work.page_name.shown = FALSE;
        emblem_results_work.time_window.shown = FALSE;
        emblem_results_work.totals.shown = FALSE;
        emblem_results_work.lines.shown = FALSE;
        emblem_results_work.boxes.shown = FALSE;
        emblem_results_work.ranks.shown = FALSE;
        emblem_results_work.dialog.shown = FALSE;
        emblem_results_work.unk_d4.shown = FALSE;
        emblem_results_work.message = EMBLEM_RESULTS_NO_MESSAGE;
    }
    /* @hack spelling: the 1.0F in a local, where the constant at each call
     * would do */
    full = 1.0F;
    /* @hack spelling: the TRUE each fade stores in a local, where the
     * constant would do */
    moved = TRUE;
    /* @hack spelling: the view through a local pointer, where its own name
     * would do */
    view = &emblem_results_view;
    /* @hack spelling: the 0.0F in a local, where the constant at each call
     * would do */
    /* @hack spelling: zero set before approach, where any order would do */
    zero = 0.0F;
    /* @hack spelling: the call through a local, where menu_approach at each
     * call would do */
    approach = menu_approach;
    /* @hack spelling: the step in a local, where FADE_STEP at each call
     * would do */
    step = FADE_STEP;
    if (emblem_results_work.title.shown) {
        if (approach(&view->levels[FADE_TITLE], full, step)) {
            emblem_results_work.title.moving = moved;
        }
    } else if (approach(&view->levels[FADE_TITLE], zero, step)) {
        emblem_results_work.title.moving = moved;
    }
    if (emblem_results_work.page_name.shown) {
        if (approach(&view->levels[FADE_PAGE_NAME], full, step)) {
            emblem_results_work.page_name.moving = moved;
        }
    } else if (approach(&view->levels[FADE_PAGE_NAME], zero, step)) {
        emblem_results_work.page_name.moving = moved;
    }
    if (emblem_results_work.time_window.shown) {
        if (approach(&view->levels[FADE_TIME_WINDOW], full, step)) {
            emblem_results_work.time_window.moving = moved;
        }
    } else if (approach(&view->levels[FADE_TIME_WINDOW], zero, step)) {
        emblem_results_work.time_window.moving = moved;
    }
    if (emblem_results_work.totals.shown) {
        if (approach(&view->levels[FADE_TOTALS], full, step)) {
            emblem_results_work.totals.moving = moved;
        }
    } else if (approach(&view->levels[FADE_TOTALS], zero, step)) {
        emblem_results_work.totals.moving = moved;
    }
    if (emblem_results_work.lines.shown) {
        if (approach(&view->levels[FADE_LINES], full, step)) {
            emblem_results_work.lines.moving = moved;
        }
        emblem_results_work.line_count =
            shown_pages[emblem_results_work.page].count;
    } else if (approach(&view->levels[FADE_LINES], zero, step)) {
        emblem_results_work.lines.moving = moved;
    }
    if (emblem_results_work.boxes.shown) {
        if (approach(&view->levels[FADE_BOXES], full, step)) {
            emblem_results_work.boxes.moving = moved;
        }
    } else if (approach(&view->levels[FADE_BOXES], zero, step)) {
        emblem_results_work.boxes.moving = moved;
    }
    if (emblem_results_work.ranks.shown) {
        if (approach(&view->levels[FADE_RANKS], full, step)) {
            emblem_results_work.ranks.moving = moved;
        }
    } else if (approach(&view->levels[FADE_RANKS], zero, step)) {
        emblem_results_work.ranks.moving = moved;
    }
    if (emblem_results_work.unk_d4.shown) {
        if (approach(&view->levels[FADE_UNK_D4], full, step)) {
            emblem_results_work.unk_d4.moving = moved;
        }
    } else if (approach(&view->levels[FADE_UNK_D4], zero, step)) {
        emblem_results_work.unk_d4.moving = moved;
    }
    /* @hack spelling: fading read again, first, and the title's last, where
     * the panels alone in their order would do */
    emblem_results_work.fading =
        emblem_results_work.fading | emblem_results_work.page_name.moving |
        emblem_results_work.time_window.moving |
        emblem_results_work.totals.moving | emblem_results_work.lines.moving |
        emblem_results_work.boxes.moving | emblem_results_work.ranks.moving |
        emblem_results_work.dialog.moving | emblem_results_work.unk_d4.moving |
        emblem_results_work.title.moving;
}

/* draw each panel whose fade isn't 0, at that fade. the title's cells add
 * to the backdrop under them. the message and the dialog draw last,
 * unfaded */
static void emblem_results_draw(struct task *task)
{
    struct menu_sprite_row title;
    Float level;
    Int message;

    /* @hack spelling: each level read inside its test throughout
     * emblem_results_draw, where a statement of its own before the test
     * would do */
    if ((level = emblem_results_view.levels[FADE_TITLE]) != 0.0F) {
        menu_fade_begin(level);
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                           MENU_BACKDROP_TINT);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        title.tanim = cells;
        if (emblem_results_work.ranking) {
            title.first = CELL_RANKING_TITLE;
        } else {
            title.first = CELL_TITLE;
        }
        title.count = TITLE_CELLS;
        title.pos = &title_pos;
        title.draw = emblem_results_sprite_draw;
        menu_sprite_row_draw(&title);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        if (emblem_results_work.crossed) {
            emblem_results_sprite_draw(CELL_CROSS, 1.0F, &cross_pos,
                                       MENU_SPRITE_ATTR);
        }
    }
    if ((level = emblem_results_view.levels[FADE_PAGE_NAME]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_page_name_draw();
    }
    if ((level = emblem_results_view.levels[FADE_TIME_WINDOW]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_time_window_draw();
    }
    if ((level = emblem_results_view.levels[FADE_TOTALS]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_totals_draw();
    }
    if ((level = emblem_results_view.levels[FADE_LINES]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_lines_panel_draw();
    }
    if ((level = emblem_results_view.levels[FADE_BOXES]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_boxes_panel_draw();
    }
    if ((level = emblem_results_view.levels[FADE_RANKS]) != 0.0F) {
        menu_fade_begin(level);
        emblem_results_ranks_panel_draw();
    }
    /* @hack spelling: the message through a local, where the work's field
     * would do */
    message = emblem_results_work.message;
    if (message >= 0) {
        message_sprite_draw(message, &message_pos, 1.0F);
    }
    if (emblem_results_work.dialog.shown) {
        emblem_results_dialog_draw();
    }
}

static void emblem_results_destroy(struct task *task)
{
    emblem_results_release();
}

void emblem_results_create(void)
{
    struct task *task;
    NJS_TEXANIM *cell;
    NJS_TEXANIM *end;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, emblem_results_step,
                       "emblemResultExec");
    task->draw = emblem_results_draw;
    task->destructor = emblem_results_destroy;
    results_task = task;
    byte = (Uint8 *)&emblem_results_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(emblem_results_work); left != 0; left--) {
        *byte++ = 0;
    }
    /* @hack spelling: end set to the first cell and moved past the last in
     * a statement of its own, where cells + CELLS would do */
    cell = end = cells;
    end += CELLS;
    for (; cell < end; cell++) {
        cell->cx = cell->sx / 2;
        cell->cy = cell->sy / 2;
    }
    emblem_results_reset();
}

void emblem_results_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = emblem_results_work.loaded;
    byte = (Uint8 *)&emblem_results_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(emblem_results_work); left != 0; left--) {
        *byte++ = 0;
    }
    emblem_results_view = view_start;
    emblem_results_gather();
    emblem_results_work.loaded = loaded;
}

void emblem_results_kill(void)
{
    if (results_task) {
        task_destroy(results_task);
        results_task = NULL;
    }
}

void emblem_results_load(void)
{
    if (!emblem_results_work.loaded) {
        texture_load_pvm(EMBLEM_RESULTS_ARCHIVE, &texlist);
        emblem_results_work.loaded = TRUE;
    }
}

void emblem_results_release(void)
{
    /* @hack spelling: an early return, where an if around the release would
     * do */
    if (!emblem_results_work.loaded) {
        return;
    }
    njReleaseTexture(&texlist);
    emblem_results_work.loaded = FALSE;
}

void emblem_results_gather(void)
{
    Int page;
    Int i;
    Int j;
    Int cart;
    Sint8 *byte;
    int left;

    for (page = 0; page < EMBLEM_RESULTS_PAGES; page++) {
        shown_pages[page].count = 0;
        for (i = 0; i < pages[page].count; i++) {
            byte = pages[page].lines[i].won;
            /* @hack spelling: a pointer beside a count of the bytes left
             * tested against 0, where won[j] for j counting up would do */
            for (left = SAVE_CHAO_EMBLEMS; left != 0; left--) {
                *byte++ = 0;
            }
            if (pages[page].lines[i].kind == LINE_STAGE) {
                if (pages[page].lines[i].stage == STAGE_CART_1) {
                    if (pages[page].lines[i].character == PLAYER_ROUGE) {
                        cart = SAVE_CART_ROUGE;
                    } else {
                        cart = SAVE_CART_OTHERS;
                    }
                    if (save_data.cart_stage[cart].ranks[MISSION_FIRST]) {
                        shown_pages[page].lines[shown_pages[page].count] =
                            pages[page].lines[i];
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .lit = pages[page].lines[i].names[0];
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .dim = pages[page].lines[i].names[0];
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .lit = pages[page].lines[i].names[1];
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .dim = pages[page].lines[i].names[1];
                        for (j = 0; j < MISSIONS; j++) {
                            shown_pages[page]
                                .lines[shown_pages[page].count]
                                .won[j] = save_data.cart_stage[cart].ranks[j];
                        }
                        shown_pages[page].count++;
                    }
                } else if (save_data.stages[pages[page].lines[i].stage]
                               .ranks[MISSION_FIRST]) {
                    shown_pages[page].lines[shown_pages[page].count] =
                        pages[page].lines[i];
                    emblem_results_view.rows[0][page][shown_pages[page].count]
                        .lit = pages[page].lines[i].names[0];
                    emblem_results_view.rows[0][page][shown_pages[page].count]
                        .dim = pages[page].lines[i].names[0];
                    emblem_results_view.rows[1][page][shown_pages[page].count]
                        .lit = pages[page].lines[i].names[1];
                    emblem_results_view.rows[1][page][shown_pages[page].count]
                        .dim = pages[page].lines[i].names[1];
                    for (j = 0; j < MISSIONS; j++) {
                        shown_pages[page]
                            .lines[shown_pages[page].count]
                            .won[j] =
                            save_data.stages[pages[page].lines[i].stage]
                                .ranks[j];
                    }
                    shown_pages[page].count++;
                }
            } else if (pages[page].lines[i].kind >= 0) {
                shown_pages[page].lines[shown_pages[page].count] =
                    pages[page].lines[i];
                emblem_results_view.rows[0][page][shown_pages[page].count].lit =
                    pages[page].lines[i].names[0];
                emblem_results_view.rows[0][page][shown_pages[page].count].dim =
                    pages[page].lines[i].names[0];
                emblem_results_view.rows[1][page][shown_pages[page].count].lit =
                    pages[page].lines[i].names[1];
                emblem_results_view.rows[1][page][shown_pages[page].count].dim =
                    pages[page].lines[i].names[1];
                switch (pages[page].lines[i].kind) {
                case LINE_SONIC_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_SONIC_TOP];
                    break;
                case LINE_KNUCKLES_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_KNUCKLES_TOP];
                    break;
                case LINE_TAILS_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_TAILS_TOP];
                    break;
                case LINE_SHADOW_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_SHADOW_TOP];
                    break;
                case LINE_ROUGE_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_ROUGE_TOP];
                    break;
                case LINE_EGGMAN_TOP:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_EGGMAN_TOP];
                    break;
                case LINE_HERO_STORY:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_HERO_STORY];
                    break;
                case LINE_DARK_STORY:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_DARK_STORY];
                    break;
                case LINE_LAST_STORY:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_LAST_STORY];
                    break;
                case LINE_CHAO:
                    for (j = 0; j < SAVE_CHAO_EMBLEMS; j++) {
                        shown_pages[page]
                            .lines[shown_pages[page].count]
                            .won[j] = save_data.chao_emblems[j];
                    }
                    if (!SAVE_CHAO_ENTERED) {
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                    }
                    break;
                case LINE_KART:
                    for (j = 0; j < SAVE_KART_RECORDS; j++) {
                        shown_pages[page]
                            .lines[shown_pages[page].count]
                            .won[j] = save_data.kart_records[j].emblem;
                    }
                    if (!save_data.cart_stage[SAVE_CART_OTHERS]
                             .ranks[MISSION_FIRST] &&
                        !save_data.cart_stage[SAVE_CART_ROUGE]
                             .ranks[MISSION_FIRST]) {
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                    }
                    break;
                case LINE_BOSS:
                    shown_pages[page].lines[shown_pages[page].count].won[0] =
                        save_data.awards[SAVE_AWARD_FIRST_BOSS];
                    shown_pages[page].lines[shown_pages[page].count].won[1] =
                        save_data.awards[SAVE_AWARD_FIRST_BOSS + 1];
                    shown_pages[page].lines[shown_pages[page].count].won[2] =
                        save_data.awards[SAVE_AWARD_FIRST_BOSS + 2];
                    /* @hack type: the two awards as Sint8, where their own
                     * Uint8 would do */
                    /* @hack spelling: the two awards joined by |, where &&
                     * of their tests would do */
                    if (!((Sint8)save_data.awards[SAVE_AWARD_HERO_STORY] |
                          (Sint8)save_data.awards[SAVE_AWARD_DARK_STORY])) {
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[0][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .lit = CELL_LOCKED_NAME;
                        emblem_results_view
                            .rows[1][page][shown_pages[page].count]
                            .dim = CELL_LOCKED_NAME;
                    }
                    break;
                }
                shown_pages[page].count++;
            }
        }
    }
}

/* the index of the first of ranks' count ranks that is SAVE_RANK_NONE, or
 * -1 where none is */
static Int emblem_results_first_unranked(const Sint8 *ranks, Int count)
{
    Int i;

    for (i = 0; i < count; i++) {
        if (ranks[i] == SAVE_RANK_NONE) {
            return i;
        }
    }
    return -1;
}

/* find the lowest mission short of SAVE_RANK_TOP in the played stages of
 * stages. it returns FALSE where none is, and otherwise TRUE. *mission is
 * then the mission, and *stage the first of stages short in it.
 * STAGE_CART_1 reads character's record of cart_stage. stages must hold a
 * stage before the -1 that ends it, unchecked */
static Bool emblem_results_short_of_top_find(Int character,
                                             const Sint32 *stages, Int *stage,
                                             Int *mission)
{
    Int cart;
    Int i;
    Int j;

    for (j = 0; j < MISSIONS; j++) {
        /* @hack spelling: the walk of stages as a while (1) with its test
         * at the end, where a do-while over i would do */
        i = 0;
        while (1) {
            if (stages[i] == STAGE_CART_1) {
                if (character == PLAYER_ROUGE) {
                    cart = SAVE_CART_ROUGE;
                } else {
                    cart = SAVE_CART_OTHERS;
                }
                if (save_data.cart_stage[cart].plays[MISSION_FIRST] &&
                    save_data.cart_stage[cart].ranks[j] != SAVE_RANK_TOP) {
                    *stage = stages[i];
                    *mission = j;
                    return TRUE;
                }
            } else if (save_data.stages[stages[i]].plays[MISSION_FIRST] &&
                       save_data.stages[stages[i]].ranks[j] != SAVE_RANK_TOP) {
                *stage = stages[i];
                *mission = j;
                return TRUE;
            }
            i++;
            if (stages[i] < 0) {
                break;
            }
        }
    }
    return FALSE;
}

enum emblem_results_choice emblem_results_choose(void)
{
    struct emblem_results_line *lines;
    Int mission;
    Int stage;
    Int cart;

    lines = shown_pages[emblem_results_work.page].lines;
    stage = lines[emblem_results_work.line].stage;
    switch (lines[emblem_results_work.line].kind) {
    case LINE_STAGE:
        if (stage == STAGE_CART_1) {
            if (lines[emblem_results_work.line].character == PLAYER_ROUGE) {
                cart = SAVE_CART_ROUGE;
            } else {
                cart = SAVE_CART_OTHERS;
            }
            if ((mission = emblem_results_first_unranked(
                     save_data.cart_stage[cart].ranks, MISSIONS)) < 0) {
                break;
            }
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(STAGE_CART_1);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        if ((mission = emblem_results_first_unranked(
                 save_data.stages[stage].ranks, MISSIONS)) < 0) {
            break;
        }
        game_state.split_screen = 0;
        game_state.two_players = 0;
        game_state.demo = 0;
        game_state.mission = mission;
        game_state.next_mode = GAME_MODE_STAGE;
        if (stage == STAGE_RELAY_SONIC) {
            stage_set_current(STAGE_RELAY_FIRST);
            stage_set_character(0, PLAYER_TAILS_MECH);
        } else {
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
        }
        return EMBLEM_RESULTS_PLAY_STAGE;
    case LINE_SONIC_TOP:
        if (emblem_results_short_of_top_find(PLAYER_SONIC, sonic_stages, &stage,
                                             &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_KNUCKLES_TOP:
        if (emblem_results_short_of_top_find(PLAYER_KNUCKLES, knuckles_stages,
                                             &stage, &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_TAILS_TOP:
        if (emblem_results_short_of_top_find(PLAYER_TAILS_MECH, tails_stages,
                                             &stage, &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_SHADOW_TOP:
        if (emblem_results_short_of_top_find(PLAYER_SHADOW, shadow_stages,
                                             &stage, &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_ROUGE_TOP:
        if (emblem_results_short_of_top_find(PLAYER_ROUGE, rouge_stages, &stage,
                                             &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_EGGMAN_TOP:
        if (emblem_results_short_of_top_find(PLAYER_EGGMAN_MECH, eggman_stages,
                                             &stage, &mission)) {
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.mission = mission;
            game_state.next_mode = GAME_MODE_STAGE;
            stage_set_current(stage);
            stage_set_character(0, lines[emblem_results_work.line].character);
            return EMBLEM_RESULTS_PLAY_STAGE;
        }
        break;
    case LINE_HERO_STORY:
        if (!save_data.awards[SAVE_AWARD_HERO_STORY]) {
            return EMBLEM_RESULTS_PLAY_STORY;
        }
        /* fall through */
    case LINE_DARK_STORY:
        if (!save_data.awards[SAVE_AWARD_DARK_STORY]) {
            return EMBLEM_RESULTS_PLAY_STORY;
        }
        /* fall through */
    case LINE_LAST_STORY:
        if (!save_data.awards[SAVE_AWARD_LAST_STORY]) {
            return EMBLEM_RESULTS_PLAY_STORY;
        }
        break;
    case LINE_CHAO:
        if (SAVE_CHAO_ENTERED) {
            return EMBLEM_RESULTS_PLAY_CHAO;
        }
        break;
    case LINE_KART:
        if (save_data.cart_stage[SAVE_CART_OTHERS].plays[MISSION_FIRST] ||
            save_data.cart_stage[SAVE_CART_ROUGE].plays[MISSION_FIRST]) {
            /* @hack spelling: the two emblems' tests nested, the first
             * against 0, where !first || !second would do */
            /* @bug logic: with the first two kart emblems won and the
             * third not, the kart line doesn't start the kart races. it
             * tests two of the three emblems it shows (the reads at
             * 0x8c51a2be and 0x8c51a2ca) */
            if (save_data.kart_records[0].emblem != 0) {
                if (!save_data.kart_records[1].emblem) {
                    return EMBLEM_RESULTS_PLAY_KART;
                }
            } else {
                return EMBLEM_RESULTS_PLAY_KART;
            }
        }
        break;
    case LINE_BOSS:
        /* @hack type: the five awards as Sint8, where their own Uint8 would
         * do */
        /* @hack spelling: the tests joined by |, where || would do */
        if ((Sint8)save_data.awards[SAVE_AWARD_HERO_STORY] |
            (Sint8)save_data.awards[SAVE_AWARD_DARK_STORY]) {
            if (((Sint8)save_data.awards[SAVE_AWARD_FIRST_BOSS] == 0) |
                ((Sint8)save_data.awards[SAVE_AWARD_FIRST_BOSS + 1] == 0) |
                ((Sint8)save_data.awards[SAVE_AWARD_FIRST_BOSS + 2] == 0)) {
                return EMBLEM_RESULTS_PLAY_BOSS;
            }
        }
        break;
    }
    return EMBLEM_RESULTS_NOTHING;
}

void emblem_results_pick_message(void)
{
    struct emblem_results_line *lines;

    lines = shown_pages[emblem_results_work.page].lines;
    if (lines[emblem_results_work.line].stage == STAGE_RELAY_SONIC) {
        emblem_results_work.message = MESSAGE_RELAY;
        return;
    }
    switch (lines[emblem_results_work.line].kind) {
    case LINE_SONIC_TOP:
        emblem_results_work.message = MESSAGE_SONIC_TOP;
        return;
    case LINE_TAILS_TOP:
        emblem_results_work.message = MESSAGE_TAILS_TOP;
        return;
    case LINE_KNUCKLES_TOP:
        emblem_results_work.message = MESSAGE_KNUCKLES_TOP;
        return;
    case LINE_SHADOW_TOP:
        emblem_results_work.message = MESSAGE_SHADOW_TOP;
        return;
    case LINE_EGGMAN_TOP:
        emblem_results_work.message = MESSAGE_EGGMAN_TOP;
        return;
    case LINE_ROUGE_TOP:
        emblem_results_work.message = MESSAGE_ROUGE_TOP;
        return;
    case LINE_HERO_STORY:
    case LINE_DARK_STORY:
    case LINE_LAST_STORY:
        emblem_results_work.message = MESSAGE_STORY;
        return;
    case LINE_CHAO:
        if (!SAVE_CHAO_ENTERED) {
            emblem_results_work.message = EMBLEM_RESULTS_NO_MESSAGE;
        } else {
            emblem_results_work.message = MESSAGE_CHAO;
        }
        return;
    case LINE_KART:
        if (!save_data.cart_stage[SAVE_CART_OTHERS].ranks[MISSION_FIRST] &&
            !save_data.cart_stage[SAVE_CART_ROUGE].ranks[MISSION_FIRST]) {
            emblem_results_work.message = EMBLEM_RESULTS_NO_MESSAGE;
        } else {
            emblem_results_work.message = MESSAGE_KART;
        }
        return;
    case LINE_BOSS:
        /* @hack type: the two awards as Sint8, where their own Uint8 would
         * do */
        /* @hack spelling: the two awards joined by |, where && of their
         * tests would do */
        if (!((Sint8)save_data.awards[SAVE_AWARD_HERO_STORY] |
              (Sint8)save_data.awards[SAVE_AWARD_DARK_STORY])) {
            emblem_results_work.message = EMBLEM_RESULTS_NO_MESSAGE;
        } else {
            emblem_results_work.message = MESSAGE_BOSS;
        }
        return;
    default:
        switch (lines[emblem_results_work.line].character) {
        case PLAYER_SONIC:
            emblem_results_work.message = MESSAGE_SONIC;
            return;
        case PLAYER_TAILS_MECH:
            emblem_results_work.message = MESSAGE_TAILS;
            return;
        case PLAYER_KNUCKLES:
            emblem_results_work.message = MESSAGE_KNUCKLES;
            return;
        case PLAYER_SHADOW:
            emblem_results_work.message = MESSAGE_SHADOW;
            return;
        case PLAYER_EGGMAN_MECH:
            emblem_results_work.message = MESSAGE_EGGMAN;
            return;
        case PLAYER_ROUGE:
            emblem_results_work.message = MESSAGE_ROUGE;
            return;
        default:
            emblem_results_work.message = EMBLEM_RESULTS_NO_MESSAGE;
            return;
        }
    }
}
