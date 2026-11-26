/* the versus select's panel. each step, the task moves the view toward
 * what versus_select_work asks for */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/versus_select_draw.h"
#include "draw/screen.h"
#include "player/player.h"
#include "shared/message_sprite.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"

#include "advertise/versus_select_draw/text.inc"

/* destroy the task, where versus_select_draw_create made one that this
 * hasn't destroyed. nothing calls versus_select_draw_end, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void versus_select_draw_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
/* a fade runs from 0 to 1 in FADE_STEPS steps */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)

/* the list of games' title, two cells side by side */
#define LIST_TITLE_CELL 17
#define SIDE_CELL 19 /* two cells, under TITLE_LEFT and under TITLE_RIGHT */
/* beside a lit row's character whose team has another look open */
#define LOOK_MARK_CELL 21
#define HERO_TITLE_CELL 26
#define DARK_TITLE_CELL 27
/* game n draws the cell GAME_LIT_CELL + n while lit, and GAME_DIM_CELL + n
 * while dim. a row is lit while the cursor is on it. a row of either list
 * draws BACK_LIT_CELL or BACK_DIM_CELL behind it */
#define GAME_LIT_CELL 7
#define GAME_DIM_CELL 12
#define BACK_LIT_CELL 28
#define BACK_DIM_CELL 29
#define DIGIT_CELL 30 /* digit n's cell is DIGIT_CELL + n */
#define STAGE_ARROW_CELL 84
/* the list of stages' title, three cells side by side */
#define STAGES_TITLE_CELL 85
/* stage n draws the cell STAGE_LIT_CELL + n while lit, and STAGE_DIM_CELL + n
 * while dim. the last stage draws the cell after its own while the work's
 * last_stage_open is 0. while its english_names is 1, each cell is
 * ENGLISH_CELLS further on */
#define STAGE_LIT_CELL 40
#define STAGE_DIM_CELL 51
#define ENGLISH_CELLS 22
/* the rows of characters' icons. a dim icon takes NORMAL_ICON_CELL's row. a
 * lit icon takes the row of its team's look. under THEME_COSTUME_H and
 * THEME_COSTUME_X every icon is H_ICON_CELL or X_ICON_CELL */
#define NORMAL_ICON_CELL 0
#define ALT_ICON_CELL 88
#define COSTUME_ICON_CELL 96
#define H_ICON_CELL 94
#define X_ICON_CELL 95
/* a character's place in each row of icons. only the normal row holds
 * ICON_NONE, which stands for no character */
enum icon {
    ICON_SHADOW,
    ICON_EGGMAN,
    ICON_ROUGE,
    ICON_SONIC,
    ICON_TAILS,
    ICON_KNUCKLES,
    ICON_NONE
};
/* no character, which each player shows on the random game's row. the last
 * stage's row shows it too while the work's last_stage_open is 0 */
#define NO_CHARACTER (-1)
/* the help lines, as message_sprite_draw's messages */
#define SIDES_HELP_MESSAGE 71
#define GAMES_HELP_MESSAGE 72
#define RANDOM_HELP_MESSAGE 73
#define STAGES_GAME_HELP_MESSAGE 74
#define STAGES_HELP_MESSAGE 75

#define BACKDROP_TEXTURE 0
#define BACKDROP_PRIORITY (-8000.0F)
#define FRONT_PRIORITY (-500.0F)
/* the hero team's title stands at TITLE_LEFT while the teams aren't
 * swapped, and at TITLE_RIGHT while they are. it moves TITLE_STEP a step
 * between them. the dark team's title stands opposite, at SCREEN_WIDTH - 1
 * less the hero's x. both titles' widths scale from 1 at TITLE_LEFT to -1
 * at TITLE_RIGHT. below 0, they draw under NJD_SPRITE_HFLIP */
#define TITLE_LEFT 200.0F
#define TITLE_RIGHT 440.0F
#define TITLE_STEP 20.0F
#define TITLE_HALF_MOVE ((TITLE_RIGHT - TITLE_LEFT) / 2)
#define TITLE_Y 180.0F
#define SIDES_Y 350.0F
#define HELP_X 0.0F
#define HELP_Y 400.0F

/* the list of games' title draws its two cells LIST_TITLE_STEP apart */
#define LIST_TITLE_X 80.0F
#define LIST_TITLE_Y 80.0F
#define LIST_TITLE_STEP 128.0F
#define LIST_PRIORITY (-400.0F)
/* game n's row stands GAME_Y + n * GAME_STEP down. as the view's list_fade
 * falls to 0, the rows close in on GATHER_Y */
#define GAME_Y 150
#define GAME_STEP 50
#define GATHER_Y 250.0F
#define GAME_BACK_BEHIND 1.0F /* a row's back, behind its cell */
/* player 0's character stands CHARACTER_X left of its row's middle, and
 * player 1's CHARACTER_X right of it. a lit row's characters draw 1 +
 * LIT_GROWTH times their size. each stands LIT_NEARER in front of what drew
 * before it. a look's mark stands MARK_X outside its character */
#define CHARACTER_X 200.0F
#define LIT_GROWTH 0.5F
#define LIT_NEARER 10.0F
#define MARK_X 60.0F
/* where player 0's and player 1's match wins draw. the draw caps the work's
 * wins at WINS_MAX. two digits draw DIGITS_STEP apart, each DIGIT_SCALE
 * times as tall. one digit draws ONE_DIGIT_X further right, DIGIT_SCALE
 * times as large */
#define WINS_LEFT_X 50.0F
#define WINS_RIGHT_X 490.0F
#define WINS_Y 400.0F
#define WINS_PRIORITY (-2000.0F)
#define WINS_MAX 99
#define DIGIT_SCALE 2.0F
#define DIGITS_STEP 80.0F
#define ONE_DIGIT_X 50.0F

/* the view's list_y slides SLIDE_STEP a step. it slides to STAGES_Y while
 * the work's stages.shown is 1, and to 0 otherwise. at a list_y of 0, the list
 * of stages stands below the screen, and its title at STAGES_TITLE_Y. the
 * window shows STAGE_ROWS_SHOWN rows. the first stands STAGE_STEP below
 * STAGES_ROW_Y, and each next one STAGE_STEP lower */
#define STAGES_Y (-SCREEN_HEIGHT)
#define SLIDE_STEP 16.0F
#define STAGES_TITLE_Y 560.0F
#define STAGES_ROW_Y 600.0F
#define STAGE_STEP 50.0F
#define STAGE_ROWS_SHOWN 4
/* the window's top while its last row shows VERSUS_SELECT_LAST_STAGE - 1,
 * where the cursor stops */
#define LAST_TOP (VERSUS_SELECT_LAST_STAGE - STAGE_ROWS_SHOWN)
#define STAGE_BACK_BEHIND 2.0F /* a row's back, behind its cell */
/* the arrows above and below the window: STAGE_ARROW_CELL turned up or
 * down */
#define ARROW_ABOVE_Y 125.0F
#define ARROW_BELOW_Y 360.0F
#define ARROW_UP NJM_DEG_ANG(90)
#define ARROW_DOWN NJM_DEG_ANG(-90)
#define TEXTURES 19
#define BACKDROP_TEXTURES 8
#define CELLS 102

/* the task's view of versus_select_work. title_x is the hero team's title's
 * x. list_y moves the games' rows and the whole list of stages. top is the
 * stage in the window's first row, and cursor the cursor's row in the
 * window. more_above and more_below are TRUE while the cursor can reach
 * stages above or below the window. the step sets them only while the
 * work's stages_help_shown is 1 */
struct versus_select_view {
    Float backdrop_fade;
    Float title_fade;
    Float title_x;
    Float list_fade;
    Float list_y;
    Sint32 top;
    Sint32 cursor;
    Bool more_above;
    Bool more_below;
};
static struct versus_select_view versus_select_view;

static struct versus_select_view view_start = {
    0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0, 0, 0, 0,
};
static struct task *versus_select_task = NULL;
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/versus_select_draw/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/versus_select_draw/texlist.inc"
};
static NJS_TEXNAME backdrop_texnames[BACKDROP_TEXTURES] = {
#include "advertise/versus_select_draw/backdrop_texnames.inc"
};
static NJS_TEXLIST backdrop_texlist = {
#include "advertise/versus_select_draw/backdrop_texlist.inc"
};
/* versus_select_draw_create sets each cell's cx and cy to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/versus_select_draw/cells.inc"
};

static void versus_select_sprite_draw_wide(Int cell, Float scale,
                                           NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void versus_select_sprite_draw_tall(Int cell, Float scale,
                                           NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = 1.0F;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void versus_select_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
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

static void versus_select_sprite_draw_turned(Int cell, Float scale,
                                             NJS_POINT3 *pos, Angle ang,
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

static Int versus_select_icon(Int player, Bool lit, Int character);

/* draw the list of games and each player's match wins */
static void versus_select_games_draw(void)
{
    NJS_POINT3 pos;
    Sint32 shown[2];
    NJS_POINT3 mark;
    Float fade;
    /* @hack type: the scale volatile, where a plain Float would do */
    volatile Float scale;
    Int i;
    Int j;
    Bool lit;
    Int team;
    Int attr;
    Int tens;
    Int ones;
    Sint32 *character;

    fade = versus_select_view.list_fade;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    pos.x = LIST_TITLE_X;
    pos.y = LIST_TITLE_Y;
    pos.z = LIST_PRIORITY;
    versus_select_sprite_draw(LIST_TITLE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.x += LIST_TITLE_STEP;
    versus_select_sprite_draw(LIST_TITLE_CELL + 1, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it draws no third
     * cell. the list title uses the two cells drawn above */
#else
    /* @bug memory: draws the cell whose number is the address of the local
     * character, so the draw reads far outside cells (0x8c50868a) */
    versus_select_sprite_draw((Int)&character, 1.0F, &pos, MENU_SPRITE_ATTR);
#endif
    for (i = 0; i < VERSUS_SELECT_GAMES; i++) {
        pos.x = SCREEN_WIDTH / 2;
        pos.y = (Float)((i * GAME_STEP) + GAME_Y) + versus_select_view.list_y;
        pos.y = ((pos.y - GATHER_Y) * fade) + GATHER_Y;
        pos.z = LIST_PRIORITY - i;
        if (versus_select_work.game == i) {
            lit = TRUE;
            versus_select_sprite_draw(i + GAME_LIT_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
            pos.z -= GAME_BACK_BEHIND;
            versus_select_sprite_draw(BACK_LIT_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
        } else {
            lit = FALSE;
            versus_select_sprite_draw(i + GAME_DIM_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
            pos.z -= GAME_BACK_BEHIND;
            versus_select_sprite_draw(BACK_DIM_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
        }
        if (i < VERSUS_SELECT_PAIRS) {
            if (versus_select_work.swapped) {
                shown[0] = versus_select_work.pairs[i][1];
                shown[1] = versus_select_work.pairs[i][0];
            } else {
                shown[0] = versus_select_work.pairs[i][0];
                shown[1] = versus_select_work.pairs[i][1];
            }
        } else if (i == VERSUS_SELECT_RANDOM_GAME) {
            shown[0] = NO_CHARACTER;
            shown[1] = NO_CHARACTER;
        } else {
            continue;
        }
        scale = (lit * LIT_GROWTH) + 1.0F;
        /* @hack redundant: a test of i and the scale that never runs */
        if (0) {
            if (i < 2 && scale) {
            }
        }
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                pos.x -= CHARACTER_X;
            } else {
                pos.x += CHARACTER_X * 2;
            }
            if (lit) {
                pos.z += LIT_NEARER;
            }
            /* @hack spelling: the character read through a pointer that the
             * call's argument sets, where shown[j] would do */
            versus_select_sprite_draw(
                versus_select_icon(j, lit, *(character = &shown[j])), scale,
                &pos, MENU_SPRITE_ATTR);
            if (*character >= 0) {
                team = versus_select_work.swapped ? j == 0 : j;
                if (versus_select_work.looks_open[team] && lit) {
                    mark = pos;
                    if (j) {
                        mark.x += MARK_X;
                        attr = MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP;
                    } else {
                        mark.x -= MARK_X;
                        attr = MENU_SPRITE_ATTR;
                    }
                    versus_select_sprite_draw(LOOK_MARK_CELL, 1.0F, &mark,
                                              attr);
                }
            }
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    for (j = 0; j < 2; j++) {
        /* @hack redundant: a test of the wins that never runs */
        if (0) {
            if (versus_select_work.wins[0] > WINS_MAX && j) {
            }
        }
        if (versus_select_work.wins[j] > WINS_MAX) {
            versus_select_work.wins[j] = WINS_MAX;
        }
        tens = versus_select_work.wins[j] / 10;
        ones = versus_select_work.wins[j] % 10;
        if (j) {
            pos.x = WINS_RIGHT_X;
            pos.y = WINS_Y;
        } else {
            pos.x = WINS_LEFT_X;
            pos.y = WINS_Y;
        }
        pos.z = WINS_PRIORITY;
        if (tens) {
            versus_select_sprite_draw_tall(tens + DIGIT_CELL, DIGIT_SCALE, &pos,
                                           MENU_SPRITE_ATTR);
            pos.x += DIGITS_STEP;
            versus_select_sprite_draw_tall(ones + DIGIT_CELL, DIGIT_SCALE, &pos,
                                           MENU_SPRITE_ATTR);
        } else {
            pos.x += ONE_DIGIT_X;
            versus_select_sprite_draw(ones + DIGIT_CELL, DIGIT_SCALE, &pos,
                                      MENU_SPRITE_ATTR);
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void versus_select_arrow_draw(Bool below)
{
    NJS_POINT3 pos;

    pos.x = SCREEN_WIDTH / 2;
    pos.z = LIST_PRIORITY;
    if (below) {
        pos.y = ARROW_BELOW_Y;
        versus_select_sprite_draw_turned(STAGE_ARROW_CELL, 1.0F, &pos,
                                         ARROW_DOWN,
                                         NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
    } else {
        pos.y = ARROW_ABOVE_Y;
        versus_select_sprite_draw_turned(STAGE_ARROW_CELL, 1.0F, &pos, ARROW_UP,
                                         NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
    }
}

static void versus_select_stages_draw(void)
{
    struct versus_select_work *work;
    NJS_POINT3 pos;
    /* @hack type: shown sized ten, where the two it holds would do */
    Sint32 shown[10];
    NJS_POINT3 mark;
    Float scale;
    Int base;
    Int row;
    Int j;
    Bool lit;
    Int team;
    Int attr;
    Sint8 last_open;
    Sint8 swapped;

    work = &versus_select_work;
    base = work->english_names * ENGLISH_CELLS;
    pos.x = SCREEN_WIDTH / 2;
    pos.y = versus_select_view.list_y + STAGES_TITLE_Y;
    pos.z = FRONT_PRIORITY;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    versus_select_sprite_draw(STAGES_TITLE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    /* each of the title's cells stands half the two cells' widths right of
     * the one before it. the half is rounded down to a whole pixel */
    /* NOLINTBEGIN(bugprone-integer-division) */
    pos.x += (Float)((cells[STAGES_TITLE_CELL].sx +
                      cells[STAGES_TITLE_CELL + 1].sx) /
                     2);
    versus_select_sprite_draw(STAGES_TITLE_CELL + 1, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    pos.x += (Float)((cells[STAGES_TITLE_CELL + 1].sx +
                      cells[STAGES_TITLE_CELL + 2].sx) /
                     2);
    /* NOLINTEND(bugprone-integer-division) */
    versus_select_sprite_draw(STAGES_TITLE_CELL + 2, 1.0F, &pos,
                              MENU_SPRITE_ATTR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    pos.y = versus_select_view.list_y + STAGES_ROW_Y;
    for (row = 0; row < STAGE_ROWS_SHOWN; row++) {
        pos.x = SCREEN_WIDTH / 2;
        pos.z = LIST_PRIORITY;
        pos.y += STAGE_STEP;
        last_open = work->last_stage_open;
        if (versus_select_view.top + row == work->stage) {
            if ((versus_select_view.top + row) == VERSUS_SELECT_LAST_STAGE) {
                if (last_open) {
                    versus_select_sprite_draw(versus_select_view.top + row +
                                                  base + STAGE_LIT_CELL,
                                              1.0F, &pos, MENU_SPRITE_ATTR);
                } else {
                    versus_select_sprite_draw(versus_select_view.top + row +
                                                  base + (STAGE_LIT_CELL + 1),
                                              1.0F, &pos, MENU_SPRITE_ATTR);
                }
            } else {
                versus_select_sprite_draw(versus_select_view.top + row + base +
                                              STAGE_LIT_CELL,
                                          1.0F, &pos, MENU_SPRITE_ATTR);
            }
            pos.z -= STAGE_BACK_BEHIND;
            versus_select_sprite_draw(BACK_LIT_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
            lit = TRUE;
        } else {
            if ((versus_select_view.top + row) == VERSUS_SELECT_LAST_STAGE) {
                if (last_open) {
                    versus_select_sprite_draw(versus_select_view.top + row +
                                                  base + STAGE_DIM_CELL,
                                              1.0F, &pos, MENU_SPRITE_ATTR);
                } else {
                    versus_select_sprite_draw(versus_select_view.top + row +
                                                  base + (STAGE_DIM_CELL + 1),
                                              1.0F, &pos, MENU_SPRITE_ATTR);
                }
            } else {
                versus_select_sprite_draw(versus_select_view.top + row + base +
                                              STAGE_DIM_CELL,
                                          1.0F, &pos, MENU_SPRITE_ATTR);
            }
            pos.z -= STAGE_BACK_BEHIND;
            versus_select_sprite_draw(BACK_DIM_CELL, 1.0F, &pos,
                                      MENU_SPRITE_ATTR);
            lit = FALSE;
        }
        swapped = work->swapped;
        if ((versus_select_view.top + row) < VERSUS_SELECT_LAST_STAGE) {
            if (swapped) {
                shown[0] = work->stage_pairs[versus_select_view.top + row][1];
                shown[1] = work->stage_pairs[versus_select_view.top + row][0];
            } else {
                shown[0] = work->stage_pairs[versus_select_view.top + row][0];
                shown[1] = work->stage_pairs[versus_select_view.top + row][1];
            }
        } else if (work->last_stage_open) {
            if (swapped) {
                shown[0] = work->stage_pairs[versus_select_view.top + row][1];
                shown[1] = work->stage_pairs[versus_select_view.top + row][0];
            } else {
                shown[0] = work->stage_pairs[versus_select_view.top + row][0];
                shown[1] = work->stage_pairs[versus_select_view.top + row][1];
            }
        } else {
            shown[0] = NO_CHARACTER;
            shown[1] = NO_CHARACTER;
        }
        scale = (lit * LIT_GROWTH) + 1.0F;
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                pos.x -= CHARACTER_X;
            } else {
                pos.x += CHARACTER_X * 2;
            }
            if (lit) {
                pos.z += LIT_NEARER;
            }
            versus_select_sprite_draw(versus_select_icon(j, lit, shown[j]),
                                      scale, &pos, MENU_SPRITE_ATTR);
            if (shown[j] >= 0) {
                team = work->swapped ? j == 0 : j;
                if (work->looks_open[team] && lit) {
                    mark = pos;
                    if (j) {
                        mark.x += MARK_X;
                        attr = MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP;
                    } else {
                        mark.x -= MARK_X;
                        attr = MENU_SPRITE_ATTR;
                    }
                    versus_select_sprite_draw(LOOK_MARK_CELL, 1.0F, &mark,
                                              attr);
                }
            }
        }
    }
}

/* the cell of character's icon for player, on a lit row or a dim one. a
 * character with no icon, as NO_CHARACTER, takes the normal row's
 * ICON_NONE */
static Int versus_select_icon(Int player, Bool lit, Int character)
{
    struct versus_select_work *work;
    Int team;

    switch (game_state.theme) {
    case THEME_COSTUME_X:
        return X_ICON_CELL;
    case THEME_COSTUME_H:
        return H_ICON_CELL;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    if (!lit) {
        switch (character) {
        case PLAYER_SONIC:
            return NORMAL_ICON_CELL + ICON_SONIC;
        case PLAYER_SHADOW:
            return NORMAL_ICON_CELL + ICON_SHADOW;
        case PLAYER_TAILS:
        case PLAYER_TAILS_MECH:
            return NORMAL_ICON_CELL + ICON_TAILS;
        case PLAYER_EGGMAN:
        case PLAYER_EGGMAN_MECH:
            return NORMAL_ICON_CELL + ICON_EGGMAN;
        case PLAYER_KNUCKLES:
            return NORMAL_ICON_CELL + ICON_KNUCKLES;
        case PLAYER_ROUGE:
            return NORMAL_ICON_CELL + ICON_ROUGE;
        }
        return NORMAL_ICON_CELL + ICON_NONE;
    }
    work = &versus_select_work;
    if (work->swapped) {
        team = player == 0;
    } else {
        team = player;
    }
    switch (work->looks[team]) {
    case VERSUS_SELECT_LOOK_ALT:
        switch (character) {
        case PLAYER_SONIC:
            return ALT_ICON_CELL + ICON_SONIC;
        case PLAYER_SHADOW:
            return ALT_ICON_CELL + ICON_SHADOW;
        case PLAYER_TAILS:
        case PLAYER_TAILS_MECH:
            return ALT_ICON_CELL + ICON_TAILS;
        case PLAYER_EGGMAN:
        case PLAYER_EGGMAN_MECH:
            return ALT_ICON_CELL + ICON_EGGMAN;
        case PLAYER_KNUCKLES:
            return ALT_ICON_CELL + ICON_KNUCKLES;
        case PLAYER_ROUGE:
            return ALT_ICON_CELL + ICON_ROUGE;
        }
        break;
    case VERSUS_SELECT_LOOK_COSTUME:
        switch (character) {
        case PLAYER_SONIC:
            return COSTUME_ICON_CELL + ICON_SONIC;
        case PLAYER_SHADOW:
            return COSTUME_ICON_CELL + ICON_SHADOW;
        case PLAYER_TAILS:
        case PLAYER_TAILS_MECH:
            return COSTUME_ICON_CELL + ICON_TAILS;
        case PLAYER_EGGMAN:
        case PLAYER_EGGMAN_MECH:
            return COSTUME_ICON_CELL + ICON_EGGMAN;
        case PLAYER_KNUCKLES:
            return COSTUME_ICON_CELL + ICON_KNUCKLES;
        case PLAYER_ROUGE:
            return COSTUME_ICON_CELL + ICON_ROUGE;
        }
        break;
    default:
        switch (character) {
        case PLAYER_SONIC:
            return NORMAL_ICON_CELL + ICON_SONIC;
        case PLAYER_SHADOW:
            return NORMAL_ICON_CELL + ICON_SHADOW;
        case PLAYER_TAILS:
        case PLAYER_TAILS_MECH:
            return NORMAL_ICON_CELL + ICON_TAILS;
        case PLAYER_EGGMAN:
        case PLAYER_EGGMAN_MECH:
            return NORMAL_ICON_CELL + ICON_EGGMAN;
        case PLAYER_KNUCKLES:
            return NORMAL_ICON_CELL + ICON_KNUCKLES;
        case PLAYER_ROUGE:
            return NORMAL_ICON_CELL + ICON_ROUGE;
        }
        break;
    }
    return NORMAL_ICON_CELL + ICON_NONE;
}

static void versus_select_draw_step(struct task *task)
{
    struct menu_scroll scroll;
    Bool *flag;

    if (versus_select_work.shown == FALSE) {
        versus_select_work.backdrop.shown = FALSE;
        versus_select_work.title.shown = FALSE;
        versus_select_work.list.shown = FALSE;
        versus_select_work.stages.shown = FALSE;
        versus_select_work.sides_shown = FALSE;
        versus_select_work.sides_help_shown = FALSE;
        versus_select_work.games_help_shown = FALSE;
        versus_select_work.stages_help_shown = FALSE;
    }
    versus_select_work.wins[0] = game_state.match_wins[0];
    versus_select_work.wins[1] = game_state.match_wins[1];
    versus_select_work.backdrop.moving = FALSE;
    if (versus_select_work.backdrop.shown) {
        if (menu_approach(&versus_select_view.backdrop_fade, 1.0F, FADE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&versus_select_view.backdrop_fade, 0.0F, FADE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    }
    versus_select_work.title.moving = FALSE;
    if (versus_select_work.title.shown) {
        if (menu_approach(&versus_select_view.title_fade, 1.0F, FADE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&versus_select_view.title_fade, 0.0F, FADE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    }
    if (!versus_select_work.swapped) {
        if (menu_approach(&versus_select_view.title_x, TITLE_LEFT,
                          TITLE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&versus_select_view.title_x, TITLE_RIGHT,
                          TITLE_STEP)) {
            versus_select_work.backdrop.moving = TRUE;
        }
    }
    versus_select_work.list.moving = FALSE;
    if (versus_select_work.list.shown) {
        if (menu_approach(&versus_select_view.list_fade, 1.0F, FADE_STEP)) {
            versus_select_work.list.moving = TRUE;
        }
    } else {
        if (menu_approach(&versus_select_view.list_fade, 0.0F, FADE_STEP)) {
            versus_select_work.list.moving = TRUE;
        }
    }
    versus_select_work.stages.moving = FALSE;
    if (versus_select_work.stages.shown) {
        if (menu_approach(&versus_select_view.list_y, STAGES_Y, SLIDE_STEP)) {
            versus_select_work.stages.moving = TRUE;
        }
    } else {
        if (menu_approach(&versus_select_view.list_y, 0.0F, SLIDE_STEP)) {
            versus_select_work.stages.moving = TRUE;
        }
    }
    scroll.top = &versus_select_view.top;
    scroll.cursor = &versus_select_view.cursor;
    scroll.target = versus_select_work.stage;
    scroll.shown = STAGE_ROWS_SHOWN;
    menu_scroll_step(&scroll);
    if (versus_select_work.stages_help_shown) {
        /* @hack spelling: each flag set through a pointer, where its own
         * name would do */
        flag = &versus_select_view.more_above;
        if (versus_select_view.top == 0) {
            *flag = FALSE;
        } else {
            *flag = TRUE;
        }
        flag = &versus_select_view.more_below;
        if (versus_select_view.top >= LAST_TOP) {
            *flag = FALSE;
        } else {
            *flag = TRUE;
        }
    }
    versus_select_work.moving =
        versus_select_work.backdrop.moving | versus_select_work.title.moving |
        versus_select_work.list.moving | versus_select_work.stages.moving;
}

static void versus_select_draw(struct task *task)
{
    NJS_POINT3 pos;
    Float scale;

    if (versus_select_view.backdrop_fade != 0.0F) {
        menu_fade_begin(versus_select_view.backdrop_fade);
        menu_backdrop_draw(BACKDROP_TEXTURE, &backdrop_texlist,
                           BACKDROP_PRIORITY, MENU_BACKDROP_TINT);
        menu_tint_end();
    }
    /* @hack spelling: the fade tested alone, where a compare with 0.0F
     * would do */
    if (versus_select_view.title_fade) {
        pos.x = versus_select_view.title_x;
        pos.y = TITLE_Y;
        pos.z = FRONT_PRIORITY;
        menu_fade_begin(versus_select_view.title_fade);
        scale = 1.0F - ((versus_select_view.title_x - TITLE_LEFT) *
                        (1.0F / TITLE_HALF_MOVE));
        if (scale < 0.0F) {
            versus_select_sprite_draw_wide(HERO_TITLE_CELL, scale, &pos,
                                           NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                                               NJD_SPRITE_HFLIP |
                                               NJD_SPRITE_COLOR);
        } else {
            versus_select_sprite_draw_wide(HERO_TITLE_CELL, scale, &pos,
                                           NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                                               NJD_SPRITE_COLOR);
        }
        /* @hack spelling: the 1 taken in a statement of its own, where one
         * expression would do */
        pos.x = SCREEN_WIDTH - versus_select_view.title_x;
        pos.x -= 1.0F;
        if (scale < 0.0F) {
            versus_select_sprite_draw_wide(DARK_TITLE_CELL, scale, &pos,
                                           NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                                               NJD_SPRITE_HFLIP |
                                               NJD_SPRITE_COLOR);
        } else {
            versus_select_sprite_draw_wide(DARK_TITLE_CELL, scale, &pos,
                                           NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE |
                                               NJD_SPRITE_COLOR);
        }
        menu_tint_end();
    }
    if (versus_select_work.sides_shown) {
        pos.x = TITLE_LEFT;
        pos.y = SIDES_Y;
        pos.z = FRONT_PRIORITY;
        versus_select_sprite_draw(SIDE_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
        pos.x = TITLE_RIGHT;
        versus_select_sprite_draw(SIDE_CELL + 1, 1.0F, &pos, NJD_SPRITE_ALPHA);
    }
    if (versus_select_work.sides_help_shown) {
        pos.x = HELP_X;
        pos.y = HELP_Y;
        pos.z = FRONT_PRIORITY;
        message_sprite_draw(SIDES_HELP_MESSAGE, &pos, 1.0F);
    }
    if (versus_select_view.list_fade != 0.0F) {
        menu_fade_begin(versus_select_view.list_fade);
        versus_select_games_draw();
        versus_select_stages_draw();
        menu_tint_end();
    }
    if (versus_select_work.games_help_shown) {
        pos.x = HELP_X;
        pos.y = HELP_Y;
        pos.z = FRONT_PRIORITY;
        if (versus_select_work.game == VERSUS_SELECT_RANDOM_GAME) {
            message_sprite_draw(RANDOM_HELP_MESSAGE, &pos, 1.0F);
        } else if (versus_select_work.game == VERSUS_SELECT_STAGES_GAME) {
            message_sprite_draw(STAGES_GAME_HELP_MESSAGE, &pos, 1.0F);
        } else {
            message_sprite_draw(GAMES_HELP_MESSAGE, &pos, 1.0F);
        }
    }
    if (versus_select_work.stages_help_shown) {
        pos.x = HELP_X;
        pos.y = HELP_Y;
        pos.z = FRONT_PRIORITY;
        message_sprite_draw(STAGES_HELP_MESSAGE, &pos, 1.0F);
        if (versus_select_view.more_above) {
            versus_select_arrow_draw(FALSE);
        }
        if (versus_select_view.more_below) {
            versus_select_arrow_draw(TRUE);
        }
    }
}

static void versus_select_draw_destroy(struct task *task)
{
    versus_select_release();
}

void versus_select_draw_create(void)
{
    struct task *task;
    Int i;

    task = task_create(TASK_WORK, TASK_LEVEL, versus_select_draw_step,
                       "twoPVSExec");
    task->draw = versus_select_draw;
    task->destructor = versus_select_draw_destroy;
    versus_select_task = task;
    /* @hack spelling: the clear in a do loop that runs once, and its
     * locals in the loop, where locals of the function's would do */
    do {
        Uint8 *byte = (Uint8 *)&versus_select_work;
        Int left;

        /* @hack spelling: a pointer beside a count of the bytes left tested
         * against 0, here and in versus_select_draw_reset, where byte[i] for
         * an unsigned i counting up would do */
        for (left = sizeof(versus_select_work); left != 0; left--) {
            *byte++ = 0;
        }
    } while (0);
    versus_select_draw_reset();
    for (i = 0; i < CELLS; i++) {
        cells[i].cx = cells[i].sx / 2;
        cells[i].cy = cells[i].sy / 2;
    }
}

void versus_select_draw_reset(void)
{
    /* @hack type: the kept flag volatile, where a plain Sint32 would do */
    volatile Sint32 loaded;
    Uint8 *byte;
    Int left;

    loaded = versus_select_work.loaded;
    byte = (Uint8 *)&versus_select_work;
    for (left = sizeof(versus_select_work); left != 0; left--) {
        *byte++ = 0;
    }
    versus_select_view = view_start;
    /* @hack type: the work through menu_works_array, where menu_works would
     * do */
    menu_works_array[0].work_04->pairs[0][0] = PLAYER_SONIC;
    menu_works_array[0].work_04->pairs[0][1] = PLAYER_SHADOW;
    menu_works_array[0].work_04->pairs[1][0] = PLAYER_TAILS;
    menu_works_array[0].work_04->pairs[1][1] = PLAYER_EGGMAN;
    menu_works_array[0].work_04->pairs[2][0] = PLAYER_KNUCKLES;
    menu_works_array[0].work_04->pairs[2][1] = PLAYER_ROUGE;
    menu_works_array[0].work_04->stage_pairs[0][0] = PLAYER_SONIC;
    menu_works_array[0].work_04->stage_pairs[0][1] = PLAYER_SHADOW;
    menu_works_array[0].work_04->stage_pairs[1][0] = PLAYER_SONIC;
    menu_works_array[0].work_04->stage_pairs[1][1] = PLAYER_SHADOW;
    menu_works_array[0].work_04->stage_pairs[2][0] = PLAYER_SONIC;
    menu_works_array[0].work_04->stage_pairs[2][1] = PLAYER_SHADOW;
    menu_works_array[0].work_04->stage_pairs[3][0] = PLAYER_KNUCKLES;
    menu_works_array[0].work_04->stage_pairs[3][1] = PLAYER_ROUGE;
    menu_works_array[0].work_04->stage_pairs[4][0] = PLAYER_KNUCKLES;
    menu_works_array[0].work_04->stage_pairs[4][1] = PLAYER_ROUGE;
    menu_works_array[0].work_04->stage_pairs[5][0] = PLAYER_KNUCKLES;
    menu_works_array[0].work_04->stage_pairs[5][1] = PLAYER_ROUGE;
    menu_works_array[0].work_04->stage_pairs[6][0] = PLAYER_TAILS;
    menu_works_array[0].work_04->stage_pairs[6][1] = PLAYER_EGGMAN;
    menu_works_array[0].work_04->stage_pairs[7][0] = PLAYER_TAILS;
    menu_works_array[0].work_04->stage_pairs[7][1] = PLAYER_EGGMAN;
    menu_works_array[0].work_04->stage_pairs[8][0] = PLAYER_TAILS;
    menu_works_array[0].work_04->stage_pairs[8][1] = PLAYER_EGGMAN;
    menu_works_array[0].work_04->stage_pairs[9][0] = PLAYER_SONIC;
    menu_works_array[0].work_04->stage_pairs[9][1] = PLAYER_SHADOW;
    versus_select_work.loaded = loaded;
}

void versus_select_draw_end(void)
{
    if (versus_select_task) {
        task_destroy(versus_select_task);
        versus_select_task = NULL;
    }
}

void versus_select_load(void)
{
    if (!versus_select_work.loaded) {
        texture_load_pvm(VERSUS_ARCHIVE, &texlist);
        texture_load_pvm(VERSUS_BACKDROP_ARCHIVE, &backdrop_texlist);
        versus_select_work.loaded = TRUE;
    }
}

void versus_select_release(void)
{
    if (versus_select_work.loaded == TRUE) {
        njReleaseTexture(&texlist);
        njReleaseTexture(&backdrop_texlist);
        versus_select_work.loaded = FALSE;
    }
}
