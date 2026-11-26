/* the sound test's panel. each step, the task moves the view toward what
 * menu_sound_test_work asks for */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/sound_test_layout.h"
#include "advertise/sound_test_panel.h"
#include "camera/viewport.h"
#include "draw/light.h"
#include "shared/message_sprite.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"

#include "advertise/sound_test_panel/text.inc"

#define TASK_LEVEL 2
#define CELLS 129
/* the lists of songs: every list but list 0 */
#define SONG_LISTS (MENU_SOUND_TEST_LISTS - 1)
#define SONG_ROWS 64 /* the rows a list has room for */
#define NO_SONG (-1) /* a row's index in songs where the row has no song */
/* the rows that show at once, of list 0 and of a list of songs */
#define LIST_ROWS_SHOWN 6
#define SONG_ROWS_SHOWN 5
/* row n of list 0 names list n + 1. its cell is FIRST_LIST_CELL + n under
 * the cursor, and FIRST_DIM_LIST_CELL + n elsewhere. where list n + 1 has
 * no songs, the dim cell draws under the cursor too. under any text but
 * Japanese, each cell lies 12 on, the language_offset of
 * menu_sound_test_lists_list */
#define FIRST_LIST_CELL 0
#define FIRST_DIM_LIST_CELL 6
/* the up arrow, and the down under NJD_SPRITE_VFLIP */
#define ARROW_CELL 26
#define TITLE_CELL 27 /* the title's first cell of three */
/* the middle of the title's first cell */
#define TITLE_X 100.0F
#define TITLE_Y 50.0F
#define TITLE_PRIORITY (-100.0F)
#define BACKDROP_TEXTURE 3
#define BACKDROP_PRIORITY (-10000.0F)
#define ARROW_PRIORITY (-50.0F)
/* a fade runs from 0 to 1 in FADE_STEPS steps. the backdrop fades to
 * HALF_LEVEL while it isn't bright */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
#define HALF_LEVEL 0.5F
/* the disc slides along y, DISC_SLIDE_STEP a step, between DISC_HIDDEN_Y
 * and 0, where it shows. it draws only while its y is below DISC_DRAWN_Y */
#define DISC_SLIDE_STEP 1.0F
#define DISC_HIDDEN_Y 30.0F
#define DISC_DRAWN_Y 29.0F
/* while a song plays, the disc tilts to DISC_PLAYING_TILT about x, turns
 * back to 0 about y, and spins DISC_PLAYING_SPIN a step about z. while no
 * song plays, it tilts back to 0, turns DISC_IDLE_TURN a step about y, and
 * spins DISC_IDLE_SPIN a step about z. a tilt or a turn back moves
 * DISC_TILT_STEP a step */
#define DISC_PLAYING_TILT NJM_DEG_ANG(112.5)
#define DISC_TILT_STEP NJM_DEG_ANG(3.75)
#define DISC_PLAYING_SPIN NJM_DEG_ANG(10)
#define DISC_IDLE_TURN NJM_DEG_ANG(2)
#define DISC_IDLE_SPIN ((Float)NJM_DEG_ANG(3))
#define DISC_AMBIENT 0.6F
/* a list's rows lie ROW_SPACING times its fade apart, so they spread out as
 * it fades in */
#define ROW_SPACING 48.0F
#define CURSOR_SCALE 1.1F
/* the prompts: list 0's, and a list of songs' while no song plays */
#define LISTS_MESSAGE 40
#define SONGS_MESSAGE 41

/* a song: its number in music_names, the cells of its row, and the message
 * shown while it plays */
struct menu_sound_test_song {
    Sint32 music;
    struct menu_row row;
    Sint32 message;
};
/* a group of songs: count numbers of music_names */
struct menu_sound_test_group {
    const Sint32 *music;
    Sint32 count;
};

/* destroy the task, where menu_sound_test_panel_create made one that this
 * hasn't destroyed. nothing calls menu_sound_test_panel_kill, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void menu_sound_test_panel_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

static struct menu_sound_test_view menu_sound_test_view;
/* by list, the cells of its rows and each row's index in songs, or NO_SONG */
static struct menu_row menu_sound_test_rows[MENU_SOUND_TEST_LISTS][SONG_ROWS];
static Sint32 menu_sound_test_song_index[MENU_SOUND_TEST_LISTS][SONG_ROWS];

static void menu_sound_test_lists_fill(void);

/* menu_sound_test_panel_create sets each cell's cx and cy to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/sound_test_panel/cells.inc"
};
/* the songs, which menu_sound_test_lists_fill searches by music. an entry
 * with a negative music ends the table */
KEPT static struct menu_sound_test_song songs[98] = {
#include "advertise/sound_test_panel/songs.inc"
};
/* group_n holds the music numbers list n can hold, in its order */
static Sint32 group_1[15] = {
#include "advertise/sound_test_panel/group_1.inc"
};
static Sint32 group_2[36] = {
#include "advertise/sound_test_panel/group_2.inc"
};
static Sint32 group_3[14] = {
#include "advertise/sound_test_panel/group_3.inc"
};
static Sint32 group_4[13] = {
#include "advertise/sound_test_panel/group_4.inc"
};
static Sint32 group_5[6] = {
#include "advertise/sound_test_panel/group_5.inc"
};
static Sint32 group_6[7] = {
#include "advertise/sound_test_panel/group_6.inc"
};
KEPT static struct menu_sound_test_group groups[SONG_LISTS] = {
#include "advertise/sound_test_panel/groups.inc"
};

void menu_sound_test_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &menu_sound_test_texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* draw the title's three cells side by side, added to what lies under
 * them */
static void menu_sound_test_title_draw(void)
{
    NJS_POINT3 pos = {TITLE_X, TITLE_Y, TITLE_PRIORITY};

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    menu_sound_test_sprite_draw(TITLE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    /* each half width is rounded down to a whole pixel */
    /* NOLINTBEGIN(bugprone-integer-division) */
    pos.x += (cells[TITLE_CELL].sx / 2) + (cells[TITLE_CELL + 1].sx / 2);
    menu_sound_test_sprite_draw(TITLE_CELL + 1, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.x += (cells[TITLE_CELL + 1].sx / 2) + (cells[TITLE_CELL + 2].sx / 2);
    /* NOLINTEND(bugprone-integer-division) */
    menu_sound_test_sprite_draw(TITLE_CELL + 2, 1.0F, &pos, MENU_SPRITE_ATTR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* draw in the disc's area where on is TRUE, and on the whole screen
 * otherwise */
static void menu_sound_test_disc_viewport(Bool on)
{
    if (on) {
        viewport_set_area(&menu_sound_test_disc_area);
        viewport_set_screen(VIEWPORT_AREA);
    } else {
        viewport_set_whole();
    }
}

static void menu_sound_test_panel_step(struct task *task)
{
    struct menu_scroll *scroll;
    Int i;

    menu_sound_test_work.moving = FALSE;
    if (!menu_sound_test_work.active) {
        menu_sound_test_work.shown = FALSE;
        menu_sound_test_work.disc_shown = FALSE;
        for (i = 0; i < MENU_SOUND_TEST_LISTS; i++) {
            menu_sound_test_work.lists[i].shown = FALSE;
        }
        menu_sound_test_work.prompt = FALSE;
    }
    menu_sound_test_work.fading = FALSE;
    if (menu_sound_test_work.shown) {
        if (menu_sound_test_work.bright) {
            if (menu_approach(&menu_sound_test_view.backdrop_fade, 1.0F,
                              FADE_STEP)) {
                menu_sound_test_work.fading = TRUE;
            }
        } else if (menu_approach(&menu_sound_test_view.backdrop_fade,
                                 HALF_LEVEL, FADE_STEP)) {
            menu_sound_test_work.fading = TRUE;
        }
    } else if (menu_approach(&menu_sound_test_view.backdrop_fade, 0.0F,
                             FADE_STEP)) {
        menu_sound_test_work.fading = TRUE;
    }
    menu_sound_test_work.disc_moving = FALSE;
    if (menu_sound_test_work.disc_shown) {
        if (menu_approach(&menu_sound_test_view.disc_pos.y, 0.0F,
                          DISC_SLIDE_STEP)) {
            menu_sound_test_work.disc_moving = TRUE;
        }
        if (menu_sound_test_work.playing) {
            menu_approach_int(&menu_sound_test_view.disc_ang.x,
                              DISC_PLAYING_TILT, DISC_TILT_STEP);
            menu_sound_test_view.disc_ang.y = angle_approach(
                menu_sound_test_view.disc_ang.y, 0, DISC_TILT_STEP);
            menu_sound_test_view.disc_ang.z += DISC_PLAYING_SPIN;
        } else {
            menu_approach_int(&menu_sound_test_view.disc_ang.x, 0,
                              DISC_TILT_STEP);
            menu_sound_test_view.disc_ang.y += DISC_IDLE_TURN;
            /* @bug arithmetic: the idle disc stops spinning about z after
             * some four million steps. its angle sums in float. once the sum
             * passes INT32_MAX, the FTRC at 0x8c50f1fc, which turns it back
             * into an Angle, gives INT32_MAX each step */
            menu_sound_test_view.disc_ang.z += DISC_IDLE_SPIN;
        }
    } else if (menu_approach(&menu_sound_test_view.disc_pos.y, DISC_HIDDEN_Y,
                             DISC_SLIDE_STEP)) {
        menu_sound_test_work.disc_moving = TRUE;
    }

    scroll = &menu_sound_test_view.lists[0].scroll;
    /* @hack redundant: fading cleared, here and in the loop below, though
     * both arms of the if after it store it next */
    menu_sound_test_work.lists[0].fading = FALSE;
    if (menu_sound_test_work.lists[0].shown) {
        menu_sound_test_work.lists[0].fading =
            menu_approach(&menu_sound_test_view.lists[0].fade, 1.0F, FADE_STEP);
    } else {
        menu_sound_test_work.lists[0].fading =
            menu_approach(&menu_sound_test_view.lists[0].fade, 0.0F, FADE_STEP);
    }
    scroll->top = &menu_sound_test_view.lists[0].top;
    scroll->cursor = &menu_sound_test_view.lists[0].cursor;
    scroll->target = menu_sound_test_work.lists[0].row;
    scroll->shown = LIST_ROWS_SHOWN;
    menu_scroll_step(scroll);
    for (i = 1; i < MENU_SOUND_TEST_LISTS; i++) {
        scroll = &menu_sound_test_view.lists[i].scroll;
        menu_sound_test_work.lists[i].fading = FALSE;
        if (menu_sound_test_work.lists[i].shown) {
            menu_sound_test_work.lists[i].fading = menu_approach(
                &menu_sound_test_view.lists[i].fade, 1.0F, FADE_STEP);
        } else {
            menu_sound_test_work.lists[i].fading = menu_approach(
                &menu_sound_test_view.lists[i].fade, 0.0F, FADE_STEP);
        }
        scroll->top = &menu_sound_test_view.lists[i].top;
        scroll->cursor = &menu_sound_test_view.lists[i].cursor;
        scroll->target = menu_sound_test_work.lists[i].row;
        scroll->shown = SONG_ROWS_SHOWN;
        menu_scroll_step(scroll);
    }

    /* @hack spelling: moving = moving | ..., where moving |= ... would do */
    menu_sound_test_work.moving = menu_sound_test_work.moving |
                                  menu_sound_test_work.fading |
                                  menu_sound_test_work.disc_moving;
    for (i = 0; i < MENU_SOUND_TEST_LISTS; i++) {
        menu_sound_test_work.moving |= menu_sound_test_work.lists[i].fading;
    }
}

static void menu_sound_test_panel_draw(struct task *task)
{
    Int i;

    if (menu_sound_test_view.backdrop_fade != 0.0F) {
        menu_fade_begin(menu_sound_test_view.backdrop_fade);
        menu_backdrop_draw(BACKDROP_TEXTURE, &menu_sound_test_texlist,
                           BACKDROP_PRIORITY, MENU_BACKDROP_TINT);
        menu_sound_test_title_draw();
        menu_tint_end();
    }
    if (menu_sound_test_view.disc_pos.y < DISC_DRAWN_Y) {
        NJS_CAMERA camera;

        njUnitMatrix(NULL);
        menu_sound_test_disc_viewport(TRUE);
        njInitCamera(&camera);
        simple_light_set_direction(0.0F, 0.0F, 1.0F);
        njCnkSetSimpleLightIntensity(1.0F, DISC_AMBIENT);
        njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
        njTranslateV(NULL, &menu_sound_test_view.disc_pos);
        njRotateX(NULL, menu_sound_test_view.disc_ang.x);
        njRotateY(NULL, menu_sound_test_view.disc_ang.y);
        njRotateZ(NULL, menu_sound_test_view.disc_ang.z);
        njSetTexture(&menu_sound_test_texlist);
        njCnkSimpleDrawObject(&menu_sound_test_disc_object);
        menu_sound_test_disc_viewport(FALSE);
    }
    /* @hack spelling: the fade tested alone, here and in the loop below,
     * where a compare with 0.0F would do */
    if (menu_sound_test_view.lists[0].fade) {
        /* @hack type: j as a Uint32, where an Int would do */
        Uint32 j;
        struct menu_row rows[SONG_LISTS] = {
            {FIRST_LIST_CELL, FIRST_DIM_LIST_CELL},
            {FIRST_LIST_CELL + 1, FIRST_DIM_LIST_CELL + 1},
            {FIRST_LIST_CELL + 2, FIRST_DIM_LIST_CELL + 2},
            {FIRST_LIST_CELL + 3, FIRST_DIM_LIST_CELL + 3},
            {FIRST_LIST_CELL + 4, FIRST_DIM_LIST_CELL + 4},
            {FIRST_LIST_CELL + 5, FIRST_DIM_LIST_CELL + 5},
        };

        menu_sound_test_lists_list.rows = rows;
        for (j = 0; j < SONG_LISTS; j++) {
            /* @hack spelling: the work through menu_works, here and at each
             * use below, where menu_sound_test_work would do */
            if (menu_works.work_10->lists[j + 1].rows == 0) {
                rows[j].lit = rows[j].dim;
            }
        }
        menu_fade_begin(menu_sound_test_view.lists[0].fade);
        menu_sound_test_lists_list.step.y =
            menu_sound_test_view.lists[0].fade * ROW_SPACING;
        menu_list_draw(&menu_sound_test_lists_list,
                       &menu_sound_test_view.lists[0].scroll, CURSOR_SCALE);
        menu_tint_end();
        if (menu_sound_test_work.prompt) {
            message_sprite_draw(LISTS_MESSAGE, &menu_sound_test_message_pos,
                                1.0F);
        }
    }
    for (i = 1; i < MENU_SOUND_TEST_LISTS; i++) {
        if (menu_sound_test_view.lists[i].fade) {
            NJS_POINT3 pos;

            menu_fade_begin(menu_sound_test_view.lists[i].fade);
            menu_sound_test_songs_list.rows = menu_sound_test_rows[i];
            menu_sound_test_songs_list.count =
                menu_works.work_10->lists[i].rows;
            /* @hack spelling: the step's y through its second name, where
             * menu_sound_test_songs_list.step.y would do */
            menu_sound_test_songs_step_y =
                menu_sound_test_view.lists[i].fade * ROW_SPACING;
            menu_list_draw(&menu_sound_test_songs_list,
                           &menu_sound_test_view.lists[i].scroll, CURSOR_SCALE);
            if (menu_sound_test_view.lists[i].top) {
                pos.x = menu_sound_test_arrow_x;
                pos.y = menu_sound_test_arrow_up_y;
                pos.z = ARROW_PRIORITY;
                menu_sound_test_sprite_draw(ARROW_CELL, 1.0F, &pos,
                                            MENU_SPRITE_ATTR);
            }
            if (menu_sound_test_view.lists[i].top <
                menu_works.work_10->lists[i].rows - SONG_ROWS_SHOWN) {
                pos.x = menu_sound_test_arrow_x;
                pos.y = menu_sound_test_arrow_down_y;
                pos.z = ARROW_PRIORITY;
                menu_sound_test_sprite_draw(ARROW_CELL, 1.0F, &pos,
                                            MENU_SPRITE_ATTR |
                                                NJD_SPRITE_VFLIP);
            }
            menu_tint_end();
            if (menu_sound_test_work.prompt) {
                if (menu_sound_test_work.playing) {
                    message_sprite_draw(
                        songs[menu_sound_test_song_index
                                  [i][menu_sound_test_work.lists[i].row]]
                            .message,
                        &menu_sound_test_message_pos, 1.0F);
                } else {
                    message_sprite_draw(SONGS_MESSAGE,
                                        &menu_sound_test_message_pos, 1.0F);
                }
            }
        }
    }
    /* @hack redundant: a block no path reaches, which reads the step's y
     * again */
    if (0) {
        if (menu_sound_test_songs_step_y) {
        }
    }
}

static void menu_sound_test_lists_fill(void)
{
    Sint32 group;
    Sint32 row;
    Sint32 entry;
    Sint32 music;
    Sint32 out;
    Int count;
    Uint8 *byte;
    Sint32 empty;

    byte = (Uint8 *)menu_sound_test_rows;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, here and in each clear of the work below, where byte[i] for
     * an unsigned i counting up would do */
    for (count = sizeof(menu_sound_test_rows); count != 0; --count) {
        *byte++ = 0;
    }
    /* @hack type: the indices set through a byte pointer stepped by a
     * Sint32's size, where a Sint32 pointer would do */
    byte = (Uint8 *)menu_sound_test_song_index;
    /* @hack spelling: empty set beside count in the loop's start, where a
     * statement of its own would do */
    for (count = MENU_SOUND_TEST_LISTS * SONG_ROWS, empty = NO_SONG; count != 0;
         --count) {
        /* NOLINTBEGIN(misc-redundant-expression) */
        /* @hack redundant: a block no path reaches, which reads count twice
         * and empty */
        if (0) {
            if (count && count && empty) {
            }
        }
        /* NOLINTEND(misc-redundant-expression) */
        *(Sint32 *)byte = empty;
        byte += sizeof(Sint32);
    }
    for (group = 0; group < SONG_LISTS; ++group) {
        menu_works.work_10->lists[group + 1].rows = 0;
        for (row = 0; row < groups[group].count; ++row) {
            music = groups[group].music[row];
            if (music_heard(music_names[music])) {
                for (entry = 0; songs[entry].music >= 0; ++entry) {
                    if (songs[entry].music == music) {
                        menu_sound_test_song_index[group + 1][row] = entry;
                        menu_sound_test_rows[group + 1][row].lit =
                            songs[entry].row.lit;
                        menu_sound_test_rows[group + 1][row].dim =
                            songs[entry].row.dim;
                        ++menu_works.work_10->lists[group + 1].rows;
                        break;
                    }
                }
            }
        }
        out = 0;
        for (row = 0; row < SONG_ROWS; ++row) {
            if (menu_sound_test_song_index[group + 1][row] >= 0) {
                menu_sound_test_song_index[group + 1][out] =
                    menu_sound_test_song_index[group + 1][row];
                menu_sound_test_rows[group + 1][out] =
                    menu_sound_test_rows[group + 1][row];
                ++out;
            }
        }
    }
    /* @hack redundant: a block no path reaches, which reads group */
    if (0) {
        if (group) {
        }
    }
}

static void menu_sound_test_panel_destroy(struct task *task)
{
    menu_sound_test_panel_release();
}

void menu_sound_test_panel_create(void)
{
    struct task *task;
    Uint8 *byte;
    Int count;
    /* @hack spelling: the cells through cell, where their own name would
     * do */
    NJS_TEXANIM *cell;

    task = task_create(TASK_WORK, TASK_LEVEL, menu_sound_test_panel_step,
                       "soundTestExec");
    task->draw = menu_sound_test_panel_draw;
    task->destructor = menu_sound_test_panel_destroy;
    menu_sound_test_task = task;
    byte = (Uint8 *)&menu_sound_test_work;
    for (count = sizeof(menu_sound_test_work); count != 0; --count) {
        *byte++ = 0;
    }
    menu_sound_test_panel_reset();
    /* @hack redundant: a do loop that runs once */
    do {
        cell = cells;
    } while (0);
    for (count = 0; count < CELLS; ++count) {
        cell[count].cx = cell[count].sx / 2;
        cell[count].cy = cell[count].sy / 2;
    }
}

void menu_sound_test_panel_reset(void)
{
    Sint32 loaded = menu_sound_test_work.loaded;
    Int count;
    Uint8 *byte = (Uint8 *)&menu_sound_test_work;

    for (count = sizeof(menu_sound_test_work); count != 0; --count) {
        *byte++ = 0;
    }
    menu_sound_test_view = menu_sound_test_view_start;
    menu_works.work_10->lists[0].rows = SONG_LISTS;
    menu_sound_test_work.loaded = loaded;
}

void menu_sound_test_panel_kill(void)
{
    if (menu_sound_test_task) {
        task_destroy(menu_sound_test_task);
        menu_sound_test_task = NULL;
    }
}

void menu_sound_test_panel_load(void)
{
    if (!menu_sound_test_work.loaded) {
        texture_load_pvm(SOUND_TEST_ARCHIVE, &menu_sound_test_texlist);
        menu_sound_test_work.loaded = TRUE;
    }
}

void menu_sound_test_panel_release(void)
{
    if (!menu_sound_test_work.loaded) {
        return;
    }
    njReleaseTexture(&menu_sound_test_texlist);
    menu_sound_test_work.loaded = FALSE;
}

const char *menu_sound_test_song_name(Int list, Int row)
{
    return music_names[songs[menu_sound_test_song_index[list][row]].music];
}

void menu_sound_test_lists_build(void)
{
    menu_sound_test_lists_fill();
}
