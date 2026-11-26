/* the stage map. each step, the task moves the view toward what
 * stage_map_work asks for */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "advertise/stage_map.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "main.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/stage_map/text.inc"

/* destroy the task, where stage_map_create made one that this hasn't
 * destroyed. nothing calls stage_map_destroy_task, and no header declares
 * it. it is external only because SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void stage_map_destroy_task(void);
/* NOLINTEND(misc-use-internal-linkage) */
/* TRUE where chao_characters[choice] has visited Chao World. only this
 * source calls stage_map_character_played, and no header declares it. a
 * static one builds other code than the image's, so it stays external */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Bool stage_map_character_played(Int choice);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2

/* a cell of the grid is CELL_SIZE pixels a side. page n scrolls the grid
 * and the backdrop n times PAGE_SCROLL left. LAST_PAGE scrolls them
 * LAST_PAGE_SCROLL, to the backdrop's right edge. only LAST_PAGE shows the
 * cells from LAST_PAGE_CELL on */
#define CELL_SIZE 60
#define CELL_MIDDLE (CELL_SIZE / 2)
#define PAGE_SCROLL 320
#define LAST_PAGE (STAGE_MAP_PAGES - 1)
#define BACKDROP_WIDTH 1920
#define LAST_PAGE_SCROLL (BACKDROP_WIDTH - (Int)SCREEN_WIDTH)
#define LAST_PAGE_CELL 24
/* the page turns as the cursor moves. a cell whose left edge lies left of
 * PAGE_BACK_X on the screen turns back a page. one right of PAGE_ON_X
 * turns on a page, up to LAST_TURNED_PAGE */
#define PAGE_BACK_X 60
#define PAGE_ON_X 520
#define LAST_TURNED_PAGE 2
/* where the cursor's cell isn't lit, the cursor goes to START_CELL, City
 * Escape's, and the page to START_PAGE. where that isn't lit either, they
 * go to OTHER_START_CELL, Iron Gate's, and OTHER_START_PAGE. once Iron
 * Gate's cell is lit, each step forces that cell's own page instead */
#define START_CELL 23
#define START_PAGE 0
#define OTHER_START_CELL 9
#define OTHER_START_PAGE 2
#define NO_CELL (-1) /* no cell found yet */

/* the backdrop's tiles. the last four are SMALL_SIZE pixels a side, and
 * the rest BIG_SIZE. a tile draws only where its left edge lies on the
 * screen or within BIG_SIZE of it */
#define TILES 18
#define BIG_SIZE 256
#define SMALL_SIZE 128
/* a whole texture's u2 and v2: its last texel, of 256 to a side */
#define UV_END 255

/* the panels move PANELS_STEP pixels a step, toward the quarter of the
 * screen away from the cursor's cell. a cell left of PANELS_SWAP_X on the
 * screen sends them right. a cell above PANELS_SWAP_Y sends them low */
#define PANELS_STEP 23.0F
#define PANELS_SWAP_X 340
#define PANELS_SWAP_Y 240
#define PANELS_LEFT 160.0F
#define PANELS_RIGHT 480.0F
#define PANELS_HIGH 140.0F
#define PANELS_LOW 300.0F
/* the grid and the backdrop scroll SCROLL_STEP pixels a step. the
 * backdrop's fade runs from 0 to 1 in FADE_STEPS steps */
#define SCROLL_STEP 23
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)

/* each part's priority. a sprite of greater priority draws in front. a part
 * drawn over another stands LAYER_NEARER in front of it */
#define BAND_PRIORITY (-1.3F)
#define BACKDROP_PRIORITY (-1000.0F)
#define GRID_PRIORITY (-100.0F)
#define PANEL_PRIORITY (-95.0F)
#define NAME_PRIORITY (-93.0F)
#define ICON_PRIORITY (-90.0F)
#define MARK_PRIORITY (-80.0F)
#define MESSAGE_PRIORITY (-50.0F) /* the bands' message */
#define LAYER_NEARER 1.0F

/* sprite cells of texlist. character_icons gives each character's icon */
#define GREEN_HILL_ICON 2
#define CHAO_ICON 7
/* the stages' names in Japanese are the NAMES cells from FIRST_NAME. those
 * in English follow, up to LAST_NAME */
#define FIRST_NAME 8
#define NAMES 33
#define LAST_NAME (FIRST_NAME + (2 * NAMES) - 1)
#define KART_NAME 16
#define KART_ROUGE_NAME 33
#define CHAO_NAME 40
/* mission n's icon is MISSION_ICON + n. a locked mission's, from mission 1
 * on, is LOCKED_ICON + n */
#define MISSION_ICON 74
#define LOCKED_ICON 78
#define NO_RANK_CELL 88
#define VALUE_CELL 89 /* the box a value draws over */
#define CURSOR_CELL 90
#define SUMMARY_CELL 91
/* the missions' panel, with two rows of records and with four */
#define MISSIONS_2_CELL 92
#define MISSIONS_4_CELL 93
/* the records' labels in Japanese. each label in English is LABELS cells
 * on */
#define TIME_LABEL 94
#define RINGS_LABEL 95
#define SCORE_LABEL 96
#define PLAYS_LABEL 97
#define LABELS 4
#define TEXTURES 15
#define SPRITE_CELLS 102
#define SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR)

/* the cursor and its cell draw at PULSE_SCALE, give or take PULSE_SWING.
 * the swing is a cosine whose angle gains PULSE_RATE degrees a step */
#define PULSE_SCALE 1.1F
#define PULSE_SWING 0.1F
#define PULSE_RATE 5
/* the pulse's scale on the step whose count is steps */
#define PULSE_AT(steps)                                                        \
    (PULSE_SCALE +                                                             \
     (PULSE_SWING * njCos((Angle)((steps) * PULSE_RATE * ANGLE_PER_DEGREE))))

/* the missions' icons lie ICON_STEP apart, from ICONS_X and ICONS_Y off
 * the panels' place. each rank draws RANK_Y below its icon */
#define ICON_STEP 50.0F
#define ICONS_X (-100.0F)
#define ICONS_Y (-10.0F)
#define RANK_Y 40.0F
/* Chao World's emblems lie in the icons' columns, EMBLEMS_ROW to a row. the
 * first row lies EMBLEMS_Y off the panels' place */
#define EMBLEMS_ROW 5
#define EMBLEMS_Y (-5.0F)
/* from the panels' place to the missions' top left corner */
#define MISSIONS_X (-128.0F)
#define MISSIONS_Y (-64.0F)
/* Chao World's choices of character. a choice whose character hasn't
 * visited draws at UNPLAYED_LEVEL */
#define CHAO_CHOICES 6
#define UNPLAYED_LEVEL 0.5F
/* the digits of each number the missions show */
#define DIGITS 7
/* a mission's records are best first */
#define BEST 0
/* each of Chao World's emblems has a slot, NO_RANK_CELL at SLOT_SCALE_Y of
 * its height. an emblem won draws CHAO_EMBLEM_FRONT in front of its slot,
 * at CHAO_EMBLEM_SCALE */
#define CHAO_EMBLEM_FRONT 10.0F
#define CHAO_EMBLEM_SCALE 0.8F
#define SLOT_SCALE_Y 0.5F

/* a band is a quad across the screen behind a message. its top and bottom
 * lie BAND_TOP and BAND_BOTTOM from band_y, the message's y */
#define BAND_CORNERS 4
#define BAND_TOP (-2.0F)
#define BAND_BOTTOM 30.0F

/* the texts of message_tables that the bands show */
#define MESSAGE_SELECT_STAGE 67
#define MESSAGE_SELECT_MISSION 68
#define MESSAGE_SELECT_CHARACTER 69

/* what save_data keeps of a cell's stage: a stage's records, the kart
 * race's, or Chao World's */
#define RECORDS_STAGE 0
#define RECORDS_KART 1
#define RECORDS_CHAO 2
/* Chao World's records in save_data: chao_emblems, and played right after
 * it */
struct chao_records {
    Sint8 emblems[SAVE_CHAO_EMBLEMS];
    Sint8 played[PLAYER_EGGMAN_MECH + 1];
};
/* a cell's records: stage under RECORDS_STAGE and RECORDS_KART, and chao
 * under RECORDS_CHAO */
struct cell_records {
    Sint32 kind;
    struct save_stage *stage;
    struct chao_records *chao;
};

/* the task's view of stage_map_work */
struct stage_map_view {
    Float backdrop_fade;
    Sint32 scroll; /* the grid's x and the backdrop's, 0 or less */
    NJS_POINT2 panels; /* the panels' place, the summary's middle */
    Sint32 unk_10;
    Bool lit[STAGE_MAP_CELLS]; /* by cell, TRUE where it is lit */
};
static struct stage_map_view stage_map_view;

/* a backdrop's tile: its top left corner on the backdrop */
struct tile {
    Sint16 x, y;
};

/* by stage, the cell of its name, or -1 for none */
static const Sint32 name_cells[SAVE_STAGES] = {
    -1, -1, -1, 12, 32, 13, 34, 15, 31, 10, 11, 24, 30, 8,  27, -1,
    9,  14, 25, -1, -1, 26, 22, 17, 20, 19, 28, 29, 18, -1, 23, 39,
    21, -1, 38, 38, 38, 38, 38, -1, 37, -1, -1, 36, 35, -1,
};

/* the y of the bands' message */
static Float band_y = 28.0F;
/* the view stage_map_reset sets: the backdrop dark, the grid at page 0 and
 * the panels low. no cell is lit */
static struct stage_map_view view_start = {0.0F, 0, {0.0F, PANELS_LOW}, 0, {0}};
static struct task *map_task = NULL;
static Float panel_level = 0.5F;
static Uint32 band_color = 0x600000ff; /* ARGB: a translucent blue */
/* clang-format off */
static struct tile tiles[TILES] = {
    {1024, 0},
    {768, 0},
    {512, 0},
    {256, 0},
    {0, 0},
    {1024, 256},
    {768, 256},
    {512, 256},
    {256, 256},
    {0, 256},
    {1664, 0},
    {1408, 0},
    {1664, 256},
    {1408, 256},
    {1280, 0},
    {1280, 128},
    {1280, 256},
    {1280, 384},
};
/* clang-format on */
struct stage_map_cell stage_map_cells[STAGE_MAP_CELLS] = {
    {STAGE_HIDDEN_BASE, PLAYER_TAILS_MECH, 19, 1},
    {STAGE_PYRAMID_CAVE, PLAYER_SONIC, 19, 4},
    {STAGE_DEATH_CHAMBER, PLAYER_KNUCKLES, 19, 5},
    {STAGE_EGG_QUARTERS, PLAYER_ROUGE, 18, 5},
    {STAGE_SAND_OCEAN, PLAYER_EGGMAN_MECH, 17, 1},
    {STAGE_DRY_LAGOON, PLAYER_ROUGE, 15, 2},
    {STAGE_WEAPONS_BED, PLAYER_EGGMAN_MECH, 15, 5},
    {STAGE_PRISON_LANE, PLAYER_TAILS_MECH, 11, 6},
    {STAGE_WILD_CANYON, PLAYER_KNUCKLES, 15, 1},
    {STAGE_IRON_GATE, PLAYER_EGGMAN_MECH, 11, 4},
    {STAGE_SECURITY_HALL, PLAYER_ROUGE, 11, 5},
    {STAGE_METAL_HARBOR, PLAYER_SONIC, 13, 6},
    {STAGE_SKY_RAIL, PLAYER_SHADOW, 13, 1},
    {STAGE_WHITE_JUNGLE, PLAYER_SHADOW, 10, 4},
    {STAGE_GREEN_FOREST, PLAYER_SONIC, 10, 5},
    {STAGE_PUMPKIN_HILL, PLAYER_KNUCKLES, 11, 1},
    {STAGE_AQUATIC_MINE, PLAYER_KNUCKLES, 9, 2},
    {STAGE_GREEN_HILL, PLAYER_SONIC, 8, 6},
    {STAGE_CART_1, PLAYER_ROUGE, 6, 5},
    {STAGE_RADICAL_HIGHWAY, PLAYER_SHADOW, 4, 4},
    {STAGE_CART_1, PLAYER_TAILS_MECH, 4, 6},
    {STAGE_MISSION_STREET, PLAYER_TAILS_MECH, 6, 3},
    {STAGE_CHAO, PLAYER_SONIC, 4, 1},
    {STAGE_CITY_ESCAPE, PLAYER_SONIC, 1, 4},
    {STAGE_CRAZY_GADGET, PLAYER_SONIC, 29, 3},
    {STAGE_ETERNAL_ENGINE, PLAYER_TAILS_MECH, 25, 2},
    {STAGE_COSMIC_WALL, PLAYER_EGGMAN_MECH, 24, 3},
    {STAGE_METEOR_HERD, PLAYER_KNUCKLES, 23, 4},
    {STAGE_LOST_COLONY, PLAYER_EGGMAN_MECH, 25, 1},
    {STAGE_RELAY_SONIC, PLAYER_SONIC, 26, 4},
    {STAGE_FINAL_CHASE, PLAYER_SHADOW, 28, 6},
    {STAGE_FINAL_RUSH, PLAYER_SONIC, 27, 5},
    {STAGE_MAD_SPACE, PLAYER_ROUGE, 24, 6},
};
/* by character, its icon's cell. no cell of the grid reads the entries
 * past PLAYER_EGGMAN_MECH's */
static Sint32 character_icons[] = {5, 4, 6, 0, 1, 3, 6, 0, -1, 5, 4};
/* the scale and the level of each lit cell but the cursor's */
static Float lit_scale = 0.7F;
static Float lit_level = 0.8F;
/* by rank, its cell */
static Sint32 rank_cells[SAVE_RANK_TOP + 1] = {
    NO_RANK_CELL, 87, 86, 85, 84, 83,
};
static Float emblem_scale = 1.0F;
/* the place of a stage's name, from the panels' */
static Float name_x = -110.0F;
static Float name_y = -50.0F;
/* from the first row of Chao World's emblems to the second */
static Float emblem_row_step = 35.0F;
static Float chao_name_x = -110.0F;
static Float chao_name_y = -50.0F;
/* the missions' rows. the first label lies label_x and label_y from the
 * panel's corner, and each next one label_step below it. a label's box
 * lies box_x to its right, and label_back, -box_x, takes x back to the
 * labels' column */
static Float label_x = 55.0F;
static Float label_y = 140.0F;
static Float label_back = -115.0F;
static Float label_step = 20.0F;
static Float box_x = 115.0F;
/* the values. the time lies value_x and time_y_4 from where the next label
 * would lie. under two rows, time_y_2 takes time_y_4's place. the first
 * number lies number_x right of the time and value_step below it. each
 * next one lies value_step lower */
static Float value_x = 68.0F;
static Float value_step = 20.0F;
static Float number_x = 97.0F;
static Float time_y_4 = -79.0F;
static Float time_y_2 = -39.0F;
static Float number_scale = 0.6F;
static Float time_scale = 0.6F;
/* the mark on the mission chosen, among the missions' icons */
static Float mark_x = -101.0F;
static Float mark_y = 29.0F;
static Float mark_step = 50.0F;
static Float mark_scale = 0.7F;
/* Chao World's missions. its emblems lie chao_emblems_y below the
 * summary's, and its choices lie chao_step apart in a row */
static Float chao_emblems_y = 50.0F;
static Float chao_x = -102.0F;
static Float chao_y = 0.0F;
static Float chao_step = 40.0F;
static Float chao_scale = 0.6F;
/* by Chao World's choice, its icon's cell and its character */
static Sint32 chao_icons[CHAO_CHOICES] = {5, 6, 1, 4, 0, 3};
static Sint32 chao_characters[CHAO_CHOICES] = {
    PLAYER_SONIC,  PLAYER_TAILS_MECH,  PLAYER_KNUCKLES,
    PLAYER_SHADOW, PLAYER_EGGMAN_MECH, PLAYER_ROUGE,
};
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/stage_map/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/stage_map/texlist.inc"
};
static NJS_TEXNAME backdrop_texnames[18] = {
#include "advertise/stage_map/backdrop_texnames.inc"
};
static NJS_TEXLIST backdrop_texlist = {
#include "advertise/stage_map/backdrop_texlist.inc"
};
/* stage_map_create sets each sprite cell's cx and cy, the point a sprite
 * draws at, to the cell's middle. the names and the missions' panels keep
 * theirs at 0, the top left corner */
static NJS_TEXANIM sprite_cells[SPRITE_CELLS] = {
#include "advertise/stage_map/sprite_cells.inc"
};

static void stage_map_sprite_draw(Int cell, Float scale, const NJS_POINT3 *pos,
                                  Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = sprite_cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* draw cell as stage_map_sprite_draw does, scaled by scale's x across and
 * by its y down */
static void stage_map_sprite_draw_xy(Int cell, const NJS_POINT3 *scale,
                                     const NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = sprite_cells;
    sprite.sx = scale->x;
    sprite.sy = scale->y;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* draw a translucent band across the screen, behind a message at band_y */
static void stage_map_band_draw(void)
{
    NJS_COLOR colors[BAND_CORNERS];
    NJS_POINT2 band_corners[BAND_CORNERS] = {{0.0F, BAND_TOP},
                                             {0.0F, BAND_BOTTOM},
                                             {SCREEN_WIDTH, BAND_BOTTOM},
                                             {SCREEN_WIDTH, BAND_TOP}};
    NJS_POINT2COL poly;

    band_corners[0].y += band_y;
    band_corners[1].y += band_y;
    band_corners[2].y += band_y;
    band_corners[3].y += band_y;
    colors[0].color = band_color;
    colors[1].color = band_color;
    colors[2].color = band_color;
    colors[3].color = band_color;
    poly.p = band_corners;
    poly.col = colors;
    poly.tex = NULL;
    poly.num = BAND_CORNERS;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njDrawPolygon2D(&poly, BAND_CORNERS, BAND_PRIORITY,
                    NJD_FILL | NJD_TRANSPARENT);
}

static void stage_map_backdrop_draw(Float scroll, Float level)
{
    NJS_TEXANIM cell;
    NJS_SPRITE sprite;
    Int i;

    draw_attr_save();
    constant_material_set(1.0F, level, level, level);
    /* @hack spelling: the tiles' loop ended on its last tile, where a test
     * against TILES would do */
    for (i = 0; i <= TILES - 1; i++) {
        if (tiles[i].x + scroll < -BIG_SIZE ||
            tiles[i].x + scroll > SCREEN_WIDTH + BIG_SIZE) {
            continue;
        }
        /* @hack spelling: a switch of the small tiles' numbers, where a test
         * of the first would do */
        switch (i) {
        case TILES - 4:
        case TILES - 3:
        case TILES - 2:
        case TILES - 1:
            cell.sx = SMALL_SIZE;
            cell.sy = SMALL_SIZE;
            break;
        default:
            cell.sx = BIG_SIZE;
            cell.sy = BIG_SIZE;
            break;
        }
        cell.cx = 0;
        cell.cy = 0;
        cell.u1 = 0;
        cell.v1 = 0;
        cell.u2 = UV_END;
        cell.v2 = UV_END;
        cell.texid = i;
        sprite.tlist = &backdrop_texlist;
        sprite.tanim = &cell;
        sprite.ang = 0;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        sprite.p.x = tiles[i].x + scroll;
        sprite.p.y = tiles[i].y;
        njDrawSprite2D(&sprite, 0, BACKDROP_PRIORITY, SPRITE_ATTR);
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

/* draw each lit cell as its character's icon. Green Hill's and Chao World's
 * cells draw icons of their own. while the work's grid.bright is 1, the
 * cursor draws on its cell, and both pulse */
static void stage_map_grid_draw(void)
{
    NJS_POINT3 pos;
    Int cell;
    Int i;

    for (i = 0; i < STAGE_MAP_CELLS; i++) {
        /* half of CELL_SIZE is a whole pixel */
        /* NOLINTBEGIN(bugprone-integer-division) */
        pos.x = (stage_map_cells[i].column * CELL_SIZE) +
                stage_map_view.scroll + CELL_MIDDLE;
        pos.y = (stage_map_cells[i].row * CELL_SIZE) + CELL_MIDDLE;
        /* NOLINTEND(bugprone-integer-division) */
        pos.z = GRID_PRIORITY;
        if (stage_map_cells[i].stage == STAGE_GREEN_HILL) {
            cell = GREEN_HILL_ICON;
        } else if (stage_map_cells[i].stage == STAGE_CHAO) {
            cell = CHAO_ICON;
        } else {
            cell = character_icons[stage_map_cells[i].character];
        }
        if (i == stage_map_work.cursor && stage_map_work.grid.bright != 0) {
            if (stage_map_view.lit[i]) {
                menu_fade_begin(1.0F);
                stage_map_sprite_draw(cell, PULSE_AT(game_state.steps), &pos,
                                      SPRITE_ATTR);
            }
            pos.z += LAYER_NEARER;
            /* @hack redundant: grid.bright tested again, inside the branch
             * its first test leads to */
            if (stage_map_work.grid.bright != 0) {
                menu_fade_begin(1.0F);
                stage_map_sprite_draw(CURSOR_CELL, PULSE_AT(game_state.steps),
                                      &pos, NJD_SPRITE_ALPHA);
            }
        } else if (stage_map_view.lit[i]) {
            menu_fade_begin(lit_level);
            stage_map_sprite_draw(cell, lit_scale, &pos, SPRITE_ATTR);
        }
    }
}

/* fill records with save_data's records of stage, cell's stage. the kart
 * race's two cells take their records of cart_stage by their characters */
static void stage_map_records(struct cell_records *records, Int stage, Int cell)
{
    switch (stage) {
    case STAGE_CART_1:
        records->kind = RECORDS_KART;
        if (stage_map_cells[cell].character == PLAYER_ROUGE) {
            records->stage = &save_data.cart_stage[SAVE_CART_ROUGE];
        } else {
            records->stage = &save_data.cart_stage[SAVE_CART_OTHERS];
        }
        break;
    case STAGE_CHAO:
        records->kind = RECORDS_CHAO;
        /* @hack type: Chao World's records laid over save_data.chao_emblems,
         * where reads of save_data.chao_emblems and save_data.played would
         * do */
        records->chao = (struct chao_records *)save_data.chao_emblems;
        break;
    default:
        records->kind = RECORDS_STAGE;
        records->stage = &save_data.stages[stage];
        break;
    }
}

/* draw the name of the cursor's stage, and each mission's icon, emblem
 * and rank */
static void stage_map_ranks_draw(struct cell_records *records)
{
    NJS_POINT3 pos;
    struct stage_map_cell *cell;
    Int name_offset;
    Int i;

    if (game_state.text_language != TEXT_JAPANESE) {
        name_offset = NAMES;
    } else {
        name_offset = 0;
    }
    for (i = 0; i < MISSIONS; i++) {
        pos.x = stage_map_view.panels.x + (i * ICON_STEP) + ICONS_X;
        pos.y = stage_map_view.panels.y + ICONS_Y;
        pos.z = ICON_PRIORITY;
        if (i == 0) {
            stage_map_sprite_draw(MISSION_ICON, 1.0F, &pos, NJD_SPRITE_ALPHA);
        } else if (records->stage->ranks[i - 1] == SAVE_RANK_NONE) {
            stage_map_sprite_draw(i + LOCKED_ICON, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
        } else {
            stage_map_sprite_draw(i + MISSION_ICON, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
        }
        if (records->stage->ranks[i] > SAVE_RANK_NONE) {
            pos.z += LAYER_NEARER;
            share_data_sprite_draw_tall(SHARE_DATA_EMBLEM_CELL, emblem_scale,
                                        &pos, NJD_SPRITE_ALPHA);
            pos.z -= LAYER_NEARER;
        }
        pos.y += RANK_Y;
        stage_map_sprite_draw(rank_cells[records->stage->ranks[i]], 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
    }
    pos.x = stage_map_view.panels.x + name_x;
    pos.y = stage_map_view.panels.y + name_y;
    pos.z = NAME_PRIORITY;
    cell = &stage_map_cells[stage_map_work.cursor];
    if (cell->stage != STAGE_CART_1) {
        stage_map_sprite_draw(name_cells[cell->stage] + name_offset, 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
    } else if (cell->character != PLAYER_ROUGE) {
        stage_map_sprite_draw(name_offset + KART_NAME, 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
    } else {
        stage_map_sprite_draw(name_offset + KART_ROUGE_NAME, 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
    }
}

/* draw Chao World's name, and its emblems in their slots, y below where
 * the summary draws them */
static void stage_map_chao_emblems_draw(struct cell_records *records, Float y)
{
    NJS_POINT3 pos;
    Int name_offset;
    Int i;

    if (game_state.text_language != TEXT_JAPANESE) {
        name_offset = NAMES;
    } else {
        name_offset = 0;
    }
    for (i = 0; i < SAVE_CHAO_EMBLEMS; i++) {
        pos.x =
            stage_map_view.panels.x + (i % EMBLEMS_ROW * ICON_STEP) + ICONS_X;
        /* i / EMBLEMS_ROW is the emblem's row, rounded down */
        /* NOLINTBEGIN(bugprone-integer-division) */
        pos.y = stage_map_view.panels.y + (i / EMBLEMS_ROW * emblem_row_step) +
                y + EMBLEMS_Y;
        /* NOLINTEND(bugprone-integer-division) */
        pos.z = ICON_PRIORITY;
        if (records->chao->emblems[i] != 0) {
            pos.z += CHAO_EMBLEM_FRONT;
            share_data_sprite_draw_tall(SHARE_DATA_EMBLEM_CELL,
                                        CHAO_EMBLEM_SCALE, &pos,
                                        NJD_SPRITE_ALPHA);
            pos.z -= CHAO_EMBLEM_FRONT;
        }
        /* @hack spelling: the slot's scale declared in a block of its own,
         * where the function's head would do */
        {
            NJS_POINT3 scale = {1.0F, SLOT_SCALE_Y, 0.0F};

            stage_map_sprite_draw_xy(NO_RANK_CELL, &scale, &pos,
                                     NJD_SPRITE_ALPHA);
        }
    }
    pos.x = stage_map_view.panels.x + chao_name_x;
    pos.y = stage_map_view.panels.y + chao_name_y;
    pos.z = NAME_PRIORITY;
    stage_map_sprite_draw(name_offset + CHAO_NAME, 1.0F, &pos,
                          NJD_SPRITE_ALPHA);
}

/* draw the summary of the cursor's cell. Green Hill's cell has none */
static void stage_map_summary_draw(void)
{
    struct cell_records records;
    NJS_POINT3 pos;

    if (stage_map_cells[stage_map_work.cursor].stage == STAGE_GREEN_HILL) {
        return;
    }
    stage_map_records(&records, stage_map_cells[stage_map_work.cursor].stage,
                      stage_map_work.cursor);
    pos.x = stage_map_view.panels.x;
    pos.y = stage_map_view.panels.y;
    pos.z = PANEL_PRIORITY;
    menu_fade_begin(panel_level);
    stage_map_sprite_draw(SUMMARY_CELL, 1.0F, &pos, SPRITE_ATTR);
    menu_tint_end();
    switch (records.kind) {
    case RECORDS_STAGE:
    case RECORDS_KART:
        stage_map_ranks_draw(&records);
        break;
    case RECORDS_CHAO:
        stage_map_chao_emblems_draw(&records, 0.0F);
        break;
    }
}

/* draw the missions of the cursor's cell. set the work's mission_count to
 * the count of choices they offer. Green Hill's cell has none */
static void stage_map_missions_draw(void)
{
    struct cell_records records;
    NJS_POINT3 pos;
    Int label_offset;
    Int i;

    if (stage_map_cells[stage_map_work.cursor].stage == STAGE_GREEN_HILL) {
        return;
    }
    stage_map_records(&records, stage_map_cells[stage_map_work.cursor].stage,
                      stage_map_work.cursor);
    if (game_state.text_language != TEXT_JAPANESE) {
        label_offset = LABELS;
    } else {
        label_offset = 0;
    }
    pos.x = stage_map_view.panels.x + MISSIONS_X;
    pos.y = stage_map_view.panels.y + MISSIONS_Y;
    pos.z = PANEL_PRIORITY;
    menu_fade_begin(panel_level);
    switch (records.kind) {
    case RECORDS_STAGE:
        switch (stage_map_work.mission) {
        case MISSION_FIRST:
        case MISSION_TIMED:
        case MISSION_HD:
            stage_map_sprite_draw(MISSIONS_4_CELL, 1.0F, &pos, SPRITE_ATTR);
            break;
        case MISSION_RINGS:
        case MISSION_CHAO:
            stage_map_sprite_draw(MISSIONS_2_CELL, 1.0F, &pos, SPRITE_ATTR);
            break;
        }
        break;
    case RECORDS_KART:
    default:
        stage_map_sprite_draw(MISSIONS_2_CELL, 1.0F, &pos, SPRITE_ATTR);
        break;
    }
    menu_tint_end();
    pos.z += LAYER_NEARER;
    pos.x += label_x;
    pos.y += label_y;
    switch (records.kind) {
    case RECORDS_STAGE:
        stage_map_work.mission_count = MISSIONS;
        stage_map_ranks_draw(&records);
        /* @hack spelling: the mark and the time declared in blocks of their
         * own, here and below, where the function's head would do */
        {
            NJS_POINT3 mark;

            mark.x = stage_map_view.panels.x + mark_x +
                     (mark_step * stage_map_work.mission);
            mark.y = stage_map_view.panels.y + mark_y;
            mark.z = MARK_PRIORITY;
            stage_map_sprite_draw(CURSOR_CELL, mark_scale, &mark,
                                  NJD_SPRITE_ALPHA);
        }
        switch (stage_map_work.mission) {
        case MISSION_FIRST:
        case MISSION_TIMED:
        case MISSION_HD:
            stage_map_sprite_draw(label_offset + TIME_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            stage_map_sprite_draw(label_offset + RINGS_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            stage_map_sprite_draw(label_offset + SCORE_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            stage_map_sprite_draw(label_offset + PLAYS_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            pos.z += LAYER_NEARER;
            pos.x += value_x;
            pos.y += time_y_4;
            menu_fade_begin(1.0F);
            {
                struct save_time time;

                /* @hack spelling: the time copied a field at a time, here and
                 * below, where a copy of the record would do */
                time.minute =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.minute;
                time.second =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.second;
                time.hundredths =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.hundredths;
                share_data_time_draw(&pos, &time, time_scale);
            }
            pos.x += number_x;
            pos.y += value_step;
            share_data_number_draw_right(
                &pos,
                records.stage->records[stage_map_work.mission][BEST].rings,
                DIGITS, number_scale);
            pos.y += value_step;
            share_data_number_draw_right(
                &pos,
                save_record_score(
                    &records.stage->records[stage_map_work.mission][BEST]),
                DIGITS, number_scale);
            pos.y += value_step;
            share_data_number_draw_right(
                &pos, records.stage->plays[stage_map_work.mission], DIGITS,
                number_scale);
            break;
        default:
            stage_map_sprite_draw(label_offset + TIME_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            stage_map_sprite_draw(label_offset + PLAYS_LABEL, 1.0F, &pos,
                                  NJD_SPRITE_ALPHA);
            pos.x += box_x;
            stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
            pos.x += label_back;
            pos.y += label_step;
            pos.z += LAYER_NEARER;
            pos.x += value_x;
            pos.y += time_y_2;
            menu_fade_begin(1.0F);
            {
                struct save_time time;

                time.minute =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.minute;
                time.second =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.second;
                time.hundredths =
                    records.stage->records[stage_map_work.mission][BEST]
                        .time.hundredths;
                share_data_time_draw(&pos, &time, time_scale);
            }
            pos.x += number_x;
            pos.y += value_step;
            share_data_number_draw_right(
                &pos, records.stage->plays[stage_map_work.mission], DIGITS,
                number_scale);
            break;
        }
        break;
    case RECORDS_KART:
        stage_map_work.mission_count = MISSIONS;
        stage_map_ranks_draw(&records);
        {
            NJS_POINT3 mark;

            mark.x = stage_map_view.panels.x + mark_x +
                     (mark_step * stage_map_work.mission);
            mark.y = stage_map_view.panels.y + mark_y;
            mark.z = MARK_PRIORITY;
            stage_map_sprite_draw(CURSOR_CELL, mark_scale, &mark,
                                  NJD_SPRITE_ALPHA);
        }
        stage_map_sprite_draw(label_offset + TIME_LABEL, 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
        pos.x += box_x;
        stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
        pos.x += label_back;
        pos.y += label_step;
        stage_map_sprite_draw(label_offset + PLAYS_LABEL, 1.0F, &pos,
                              NJD_SPRITE_ALPHA);
        pos.x += box_x;
        stage_map_sprite_draw(VALUE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
        pos.x += label_back;
        pos.y += label_step;
        pos.z += LAYER_NEARER;
        pos.x += value_x;
        pos.y += time_y_2;
        menu_fade_begin(1.0F);
        {
            struct save_time time;

            time.minute = records.stage->records[stage_map_work.mission][BEST]
                              .time.minute;
            time.second = records.stage->records[stage_map_work.mission][BEST]
                              .time.second;
            time.hundredths =
                records.stage->records[stage_map_work.mission][BEST]
                    .time.hundredths;
            share_data_time_draw(&pos, &time, time_scale);
        }
        pos.x += number_x;
        pos.y += value_step;
        share_data_number_draw_right(
            &pos, records.stage->plays[stage_map_work.mission], DIGITS,
            number_scale);
        break;
    case RECORDS_CHAO:
        stage_map_work.mission_count = CHAO_CHOICES;
        stage_map_chao_emblems_draw(&records, chao_emblems_y);
        for (i = 0; i < CHAO_CHOICES; i++) {
            pos.x = stage_map_view.panels.x + chao_x + (i * chao_step);
            pos.y = stage_map_view.panels.y + chao_y;
            pos.z = ICON_PRIORITY;
            if (stage_map_character_played(i)) {
                menu_fade_begin(1.0F);
            } else {
                menu_fade_begin(UNPLAYED_LEVEL);
            }
            stage_map_sprite_draw(chao_icons[i], chao_scale, &pos, SPRITE_ATTR);
            pos.z += LAYER_NEARER;
            if (stage_map_work.mission == i) {
                menu_fade_begin(1.0F);
                stage_map_sprite_draw(CURSOR_CELL, chao_scale, &pos,
                                      NJD_SPRITE_ALPHA);
            }
        }
        break;
    }
}

static void stage_map_step(struct task *task)
{
    struct cell_records records;
    Int i;

    if (!stage_map_work.shown) {
        stage_map_work.backdrop.shown = FALSE;
        stage_map_work.grid.shown = FALSE;
        stage_map_work.summary.shown = FALSE;
        stage_map_work.missions.shown = FALSE;
        stage_map_work.unk_44.shown = 0;
    }
    stage_map_work.moving = FALSE;
    if (stage_map_work.shown) {
        for (i = 0; i < STAGE_MAP_CELLS; i++) {
            stage_map_records(&records, stage_map_cells[i].stage, i);
            switch (records.kind) {
            case RECORDS_STAGE:
            case RECORDS_KART:
                if (records.stage->ranks[MISSION_FIRST]) {
                    stage_map_view.lit[i] = TRUE;
                } else {
                    stage_map_view.lit[i] = FALSE;
                }
                break;
            default:
                if (records.chao->played[PLAYER_SONIC] +
                    records.chao->played[PLAYER_SHADOW] +
                    records.chao->played[PLAYER_TAILS] +
                    records.chao->played[PLAYER_EGGMAN] +
                    records.chao->played[PLAYER_KNUCKLES] +
                    records.chao->played[PLAYER_ROUGE] +
                    records.chao->played[PLAYER_TAILS_MECH] +
                    records.chao->played[PLAYER_EGGMAN_MECH]) {
                    stage_map_view.lit[i] = TRUE;
                } else {
                    stage_map_view.lit[i] = FALSE;
                }
                break;
            }
        }
    }
    if (!stage_map_view.lit[stage_map_work.cursor]) {
        /* @hack spelling: the start cell's lit compared with TRUE, where its
         * test would do, as lit holds TRUE or FALSE */
        if (stage_map_view.lit[START_CELL] == TRUE) {
            stage_map_work.cursor = START_CELL;
            stage_map_work.page = START_PAGE;
        } else {
            stage_map_work.cursor = OTHER_START_CELL;
            stage_map_work.page = OTHER_START_PAGE;
        }
    } else {
        /* on these six stages' cells, each step forces the cell's page over
         * the one stage_map_cursor_move chose */
        switch (stage_map_cells[stage_map_work.cursor].stage) {
        case STAGE_CITY_ESCAPE:
        case STAGE_RADICAL_HIGHWAY:
        case STAGE_CHAO:
            stage_map_work.page = 0;
            break;
        case STAGE_IRON_GATE:
            stage_map_work.page = 1;
            break;
        /* @hack spelling: Dry Lagoon's case before Wild Canyon's, where the
         * stages' order would do */
        case STAGE_DRY_LAGOON:
        case STAGE_WILD_CANYON:
            stage_map_work.page = 2;
            break;
        }
    }
    stage_map_work.backdrop.moving = FALSE;
    if (stage_map_work.backdrop.shown) {
        if (menu_approach(&stage_map_view.backdrop_fade, 1.0F, FADE_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&stage_map_view.backdrop_fade, 0.0F, FADE_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
    }
    switch (stage_map_work.page) {
    case 0:
        if (menu_approach_int(&stage_map_view.scroll, 0, SCROLL_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
        break;
    case 1:
        if (menu_approach_int(&stage_map_view.scroll, -PAGE_SCROLL,
                              SCROLL_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
        break;
    case 2:
        if (menu_approach_int(&stage_map_view.scroll, -2 * PAGE_SCROLL,
                              SCROLL_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
        break;
    case LAST_PAGE:
        if (menu_approach_int(&stage_map_view.scroll, -LAST_PAGE_SCROLL,
                              SCROLL_STEP)) {
            stage_map_work.backdrop.moving = TRUE;
        }
        break;
    }
    stage_map_work.grid.moving = 0;
    stage_map_work.summary.moving = FALSE;
    if ((stage_map_cells[stage_map_work.cursor].column * CELL_SIZE) +
            stage_map_view.scroll <
        PANELS_SWAP_X) {
        if (menu_approach(&stage_map_view.panels.x, PANELS_RIGHT,
                          PANELS_STEP)) {
            stage_map_work.summary.moving = TRUE;
        }
    } else {
        if (menu_approach(&stage_map_view.panels.x, PANELS_LEFT, PANELS_STEP)) {
            stage_map_work.summary.moving = TRUE;
        }
    }
    if (stage_map_cells[stage_map_work.cursor].row * CELL_SIZE <
        PANELS_SWAP_Y) {
        if (menu_approach(&stage_map_view.panels.y, PANELS_LOW, PANELS_STEP)) {
            stage_map_work.summary.moving = TRUE;
        }
    } else {
        if (menu_approach(&stage_map_view.panels.y, PANELS_HIGH, PANELS_STEP)) {
            stage_map_work.summary.moving = TRUE;
        }
    }
    stage_map_work.missions.moving = 0;
    stage_map_work.unk_44.moving = 0;
    /* @hack spelling: moving ORed into itself, where |= would do */
    stage_map_work.moving =
        stage_map_work.moving | stage_map_work.backdrop.moving |
        stage_map_work.grid.moving | stage_map_work.summary.moving |
        stage_map_work.missions.moving;
}

static void stage_map_draw(struct task *task)
{
    NJS_POINT3 pos = {0.0F, 0.0F, MESSAGE_PRIORITY};

    pos.y = band_y;
    if (stage_map_view.backdrop_fade) {
        stage_map_backdrop_draw(stage_map_view.scroll,
                                stage_map_view.backdrop_fade);
    }
    if (stage_map_work.grid.shown) {
        stage_map_grid_draw();
        if (stage_map_work.grid.bright) {
            stage_map_band_draw();
            message_sprite_draw(MESSAGE_SELECT_STAGE, &pos, 1.0F);
        }
    }
    if (stage_map_work.summary.shown) {
        stage_map_summary_draw();
    }
    if (stage_map_work.missions.shown) {
        stage_map_missions_draw();
        if (stage_map_work.missions.bright) {
            stage_map_band_draw();
            if (stage_map_cells[stage_map_work.cursor].stage != STAGE_CHAO) {
                message_sprite_draw_set_band(MESSAGE_SELECT_MISSION, &pos,
                                             1.0F);
            } else {
                message_sprite_draw(MESSAGE_SELECT_CHARACTER, &pos, 1.0F);
            }
        }
    }
}

static void stage_map_destroy(struct task *task)
{
    stage_map_release();
}

void stage_map_create(void)
{
    struct task *task;
    Uint8 *byte;
    Int i;

    task = task_create(TASK_WORK, TASK_LEVEL, stage_map_step, "stageMapExec");
    task->draw = stage_map_draw;
    task->destructor = stage_map_destroy;
    map_task = task;
    byte = (Uint8 *)&stage_map_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, here and in stage_map_reset, where byte[i] for an unsigned
     * i counting up would do */
    for (i = sizeof(stage_map_work); i != 0; i--) {
        *byte++ = 0;
    }
    stage_map_reset();
    for (i = 0; i < SPRITE_CELLS; i++) {
        if (i < FIRST_NAME || i > LAST_NAME) {
            /* @hack spelling: a switch that passes over the missions' panels,
             * where a test would do */
            switch (i) {
            case MISSIONS_2_CELL:
            case MISSIONS_4_CELL:
                break;
            default:
                sprite_cells[i].cx = sprite_cells[i].sx / 2;
                sprite_cells[i].cy = sprite_cells[i].sy / 2;
                break;
            }
        }
    }
    for (i = 0; i < MISSIONS; i++) {
        save_data.stages[STAGE_RELAY_SONIC].plays[i] =
            save_data.stages[STAGE_RELAY_FIRST].plays[i];
    }
}

void stage_map_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    Int i;

    loaded = stage_map_work.loaded;
    byte = (Uint8 *)&stage_map_work;
    for (i = sizeof(stage_map_work); i != 0; i--) {
        *byte++ = 0;
    }
    stage_map_view = view_start;
    stage_map_work.loaded = loaded;
}

void stage_map_load(void)
{
    if (!stage_map_work.loaded) {
        texture_load_pvm(STAGE_MAP_ARCHIVE, &texlist);
        texture_load_pvm(STAGE_MAP_BACKDROP_ARCHIVE, &backdrop_texlist);
        stage_map_work.loaded = TRUE;
    }
}

void stage_map_release(void)
{
    if (stage_map_work.loaded) {
        njReleaseTexture(&texlist);
        njReleaseTexture(&backdrop_texlist);
        stage_map_work.loaded = FALSE;
    }
}

void stage_map_destroy_task(void)
{
    if (map_task) {
        task_destroy(map_task);
        map_task = NULL;
    }
}

Bool stage_map_cursor_move(Int direction)
{
    Int best;
    Int x;
    Int i;

    best = NO_CELL;
    switch (direction) {
    case STAGE_MAP_UP:
        for (i = 0; i < STAGE_MAP_CELLS; i++) {
            if (!stage_map_view.lit[i]) {
                continue;
            }
            if (stage_map_cells[i].column ==
                    stage_map_cells[stage_map_work.cursor].column &&
                stage_map_cells[i].row <
                    stage_map_cells[stage_map_work.cursor].row) {
                if (best != NO_CELL) {
                    if (stage_map_cells[i].row > stage_map_cells[best].row) {
                        best = i;
                    }
                } else {
                    best = i;
                }
            }
        }
        break;
    case STAGE_MAP_DOWN:
        for (i = 0; i < STAGE_MAP_CELLS; i++) {
            if (!stage_map_view.lit[i]) {
                continue;
            }
            if (stage_map_cells[i].column ==
                    stage_map_cells[stage_map_work.cursor].column &&
                stage_map_cells[i].row >
                    stage_map_cells[stage_map_work.cursor].row) {
                if (best != NO_CELL) {
                    if (stage_map_cells[i].row < stage_map_cells[best].row) {
                        best = i;
                    }
                } else {
                    best = i;
                }
            }
        }
        break;
    case STAGE_MAP_LEFT:
        for (i = 0; i < STAGE_MAP_CELLS; i++) {
            if (!stage_map_view.lit[i]) {
                continue;
            }
            /* @hack spelling: an else after the continue, here and in
             * STAGE_MAP_RIGHT's case, where an if of its own would do */
            if (stage_map_cells[i].column >=
                stage_map_cells[stage_map_work.cursor].column) {
                continue;
            } else if (best != NO_CELL) {
                if (stage_map_cells[i].column == stage_map_cells[best].column) {
                    if (_builtin_fabsf(
                            (Float)(stage_map_cells[i].row -
                                    stage_map_cells[stage_map_work.cursor]
                                        .row)) <
                        _builtin_fabsf(
                            (Float)(stage_map_cells[best].row -
                                    stage_map_cells[stage_map_work.cursor]
                                        .row))) {
                        best = i;
                    }
                } else if (stage_map_cells[i].column >
                           stage_map_cells[best].column) {
                    best = i;
                }
            } else {
                best = i;
            }
        }
        break;
    case STAGE_MAP_RIGHT:
        for (i = 0; i < STAGE_MAP_CELLS; i++) {
            if (!stage_map_view.lit[i]) {
                continue;
            }
            if (stage_map_cells[i].column <=
                stage_map_cells[stage_map_work.cursor].column) {
                continue;
            } else if (best != NO_CELL) {
                if (stage_map_cells[i].column == stage_map_cells[best].column) {
                    if (_builtin_fabsf(
                            (Float)(stage_map_cells[i].row -
                                    stage_map_cells[stage_map_work.cursor]
                                        .row)) <
                        _builtin_fabsf(
                            (Float)(stage_map_cells[best].row -
                                    stage_map_cells[stage_map_work.cursor]
                                        .row))) {
                        best = i;
                    }
                } else if (stage_map_cells[i].column <
                           stage_map_cells[best].column) {
                    best = i;
                }
            } else {
                best = i;
            }
        }
        break;
    }
    if (best >= 0) {
        stage_map_work.cursor = best;
        x = (stage_map_cells[stage_map_work.cursor].column * CELL_SIZE) +
            stage_map_view.scroll;
        if (stage_map_work.cursor >= LAST_PAGE_CELL) {
            stage_map_work.page = LAST_PAGE;
        } else if (x < PAGE_BACK_X) {
            stage_map_work.page--;
            if (stage_map_work.page < 0) {
                stage_map_work.page = 0;
            }
        } else if (x > PAGE_ON_X) {
            stage_map_work.page++;
            if (stage_map_work.page >= LAST_TURNED_PAGE) {
                stage_map_work.page = LAST_TURNED_PAGE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

Int stage_map_start(void)
{
    struct cell_records records;
    Int stage;
    Int result;

    stage_map_records(&records, stage_map_cells[stage_map_work.cursor].stage,
                      stage_map_work.cursor);
    stage = stage_map_cells[stage_map_work.cursor].stage;
    result = STAGE_MAP_LOCKED;
    if (stage == STAGE_RELAY_SONIC) {
        stage = STAGE_RELAY_FIRST;
    }
    switch (records.kind) {
    case RECORDS_STAGE:
        stage_set_current(stage);
        if (stage == STAGE_RELAY_FIRST) {
            stage_set_character(0, PLAYER_TAILS_MECH);
        } else {
            stage_set_character(
                0, stage_map_cells[stage_map_work.cursor].character);
        }
        if (stage == STAGE_GREEN_HILL) {
            game_state.mission = MISSION_FIRST;
        } else {
            game_state.mission = stage_map_work.mission;
        }
        game_state.split_screen = 0;
        game_state.two_players = 0;
        game_state.demo = 0;
        game_state.next_mode = GAME_MODE_STAGE;
        result = STAGE_MAP_STAGE;
        break;
    case RECORDS_KART:
        stage_set_current(stage);
        stage_set_character(0,
                            stage_map_cells[stage_map_work.cursor].character);
        game_state.split_screen = 0;
        game_state.two_players = 0;
        game_state.demo = 0;
        game_state.mission = stage_map_work.mission;
        game_state.next_mode = GAME_MODE_STAGE;
        result = STAGE_MAP_STAGE;
        break;
    case RECORDS_CHAO:
        if (stage_map_character_played(stage_map_work.mission)) {
            stage_set_current(STAGE_CHAO);
            if (chao_characters[stage_map_work.mission] == PLAYER_TAILS_MECH) {
                stage_set_character(0, PLAYER_TAILS);
            } else if (chao_characters[stage_map_work.mission] ==
                       PLAYER_EGGMAN_MECH) {
                stage_set_character(0, PLAYER_EGGMAN);
            } else {
                stage_set_character(0, chao_characters[stage_map_work.mission]);
            }
            game_state.split_screen = 0;
            game_state.two_players = 0;
            game_state.demo = 0;
            game_state.next_mode = GAME_MODE_STAGE;
            result = STAGE_MAP_CHAO;
        }
        break;
    }
    return result;
}

Int stage_map_mission_open(void)
{
    struct cell_records records;
    Int stage;
    Int mission;
    Int result;
    Int cart;

    stage_map_records(&records, stage_map_cells[stage_map_work.cursor].stage,
                      stage_map_work.cursor);
    stage = stage_map_cells[stage_map_work.cursor].stage;
    mission = stage_map_work.mission;
    result = STAGE_MAP_LOCKED;
    switch (records.kind) {
    case RECORDS_STAGE:
        if (mission == MISSION_FIRST ||
            save_data.stages[stage].ranks[mission - 1]) {
            result = STAGE_MAP_STAGE;
        }
        break;
    case RECORDS_KART:
        if (stage_map_cells[stage_map_work.cursor].character == PLAYER_ROUGE) {
            cart = SAVE_CART_ROUGE;
        } else {
            cart = SAVE_CART_OTHERS;
        }
        if (mission == MISSION_FIRST ||
            save_data.cart_stage[cart].ranks[mission - 1]) {
            result = STAGE_MAP_KART;
        }
        break;
    case RECORDS_CHAO:
        if (stage_map_character_played(mission)) {
            result = STAGE_MAP_CHAO;
        }
        break;
    }
    return result;
}

Bool stage_map_cell_lit(Int cell)
{
    return stage_map_view.lit[cell];
}

Bool stage_map_character_played(Int choice)
{
    return save_data.played[chao_characters[choice]];
}
