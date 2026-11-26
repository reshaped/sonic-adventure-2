/* the file select's task, fileSelectExec, which shows file_select.h's
 * work */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/file_select.h"
#include "advertise/menu_draw.h"
#include "advertise/menu_projection.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/share_data.h"
#include "draw/lines.h"
#include "player/player.h"
#include "save/card.h"
#include "save/save.h"
#include "shared/message_sprite.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/util.h"

/* destroy the task, where file_select_create made one that this hasn't
 * destroyed. nothing calls file_select_kill, and no header declares it. it
 * is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void file_select_kill(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
#define TEXTURES 12
/* the drives swing SWING_STEP degrees a step. at SWUNG_OUT they are out,
 * and at SWUNG_IN they are in */
#define SWUNG_OUT 0
#define SWUNG_IN 180
#define SWING_STEP 6
/* the drives' circle, as menu_panel_point takes it: DRIVE_RADIUS about the y
 * axis, moved DRIVE_DEPTH along z */
#define DRIVE_DEPTH (-100.0F)
#define DRIVE_RADIUS 50.0F
/* the files slide SLIDE_STEP pixels a step to their x. a drive of ports A
 * and B puts them at FILES_RIGHT_X. a drive from FIRST_LEFT_DRIVE, port C's
 * first, puts them at FILES_LEFT_X */
#define SLIDE_STEP 21
#define FILES_RIGHT_X 480
#define FILES_LEFT_X 160
#define FIRST_LEFT_DRIVE 4
/* the files the window shows at once */
#define SHOWN_FILES 2
/* the unlit drives' rings dim to DIM_LEVEL while the cursor is on the files
 * or the dialog shows. their level moves FADE_STEP a step, as a fade from 0
 * to 1 in FADE_STEPS steps does */
#define DIM_LEVEL 0.5F
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
/* the blink of the drive under the cursor moves BLINK_STEP degrees a
 * step */
#define BLINK_STEP 4
/* the lit drive's icon grows by up to PULSE of its size with the blink */
#define PULSE 0.1F

/* indexes of the array cells, which holds CELLS. a text's cell is its
 * Japanese one. the other languages' cell lies OTHER_TEXT from it, earlier
 * in cells */
#define OTHER_TEXT (-13)
/* the other languages' new row: NEW_FILE_CELL with room, and NO_ROOM_CELL
 * without */
#define NEW_FILE_CELL 4
#define NO_ROOM_CELL 5
/* the labels of the free blocks and of a file's slot */
#define BLOCKS_LABEL_CELL 13
#define FILE_LABEL_CELL 14
/* Japanese's new row is two cells: NEW_FILE_JAPANESE_CELL and
 * NEW_FILE_TAIL_CELL with room, and NO_ROOM_JAPANESE_CELL and NO_ROOM_TAIL_CELL
 * without */
#define NEW_FILE_JAPANESE_CELL 17
#define NO_ROOM_JAPANESE_CELL 18
#define NO_ROOM_TAIL_CELL 26
#define NEW_FILE_TAIL_CELL 27
/* DIALOG_TEXT_CELL heads the dialog, over its choices.
 * FILE_SELECT_DIALOG_CONFIRM shows the first two of them */
#define DIALOG_TEXT_CELL 19
#define FIRST_CHOICE_CELL 15
#define SECOND_CHOICE_CELL 16
#define THIRD_CHOICE_CELL 20
/* the labels of a file's stage and of its time */
#define STAGE_LABEL_CELL 21
#define TIME_LABEL_CELL 22
/* a frame's corner, and a piece of its edge */
#define CORNER_CELL 24
#define EDGE_CELL 25
/* the meter of the free blocks: its back and its fill. each is a pixel
 * wide, so its scale across is its length */
#define METER_CELL 35
#define METER_FILL_CELL 36
#define BAND_CELL 37 /* the band behind each of a file's three values */
/* the plate behind the files: its top, middle and bottom */
#define PLATE_TOP_CELL 38
#define PLATE_CELL 39
#define PLATE_BOTTOM_CELL 40
/* a drive's ring, lit and dim. a device's icon is a cell of device_icons,
 * and its dim icon the cell after */
#define LIT_RING_CELL 41
#define DIM_RING_CELL 42
#define DIALOG_CELL 49
/* the stage names of the other languages lie OTHER_STAGE_NAMES from
 * Japanese's */
#define OTHER_STAGE_NAMES 33
#define NO_STAGE_NAME (-1)
/* STAGE_CART_1 has two names: Rouge's, and every other character's */
#define KART_NAME_CELL 58
#define ROUGE_KART_NAME_CELL 75
#define CELLS 116

/* the plate's level, and an unlit frame's */
#define PLATE_LEVEL 0.8F
#define FRAME_DIM_LEVEL 0.5F
/* the draws' priorities, from the back */
#define DRIVE_PRIORITY (-9.0F)
#define PLATE_PRIORITY (-8.0F)
/* the free blocks' label and meter, and the new row */
#define LABEL_PRIORITY (-7.0F)
/* the files, the meter's fill and the count of free blocks */
#define FRONT_PRIORITY (-6.0F)
/* the plate's cells stand PLATE_STEP apart down. the files' rows stand
 * ROW_STEP apart, and the lines of a row LINE_STEP. the file dialog's
 * choices stand CHOICE_STEP apart across, and FILE_SELECT_DIALOG_CONFIRM's
 * CONFIRM_STEP */
#define PLATE_STEP 62.0F
#define ROW_STEP 100.0F
#define LINE_STEP 16.0F
#define CHOICE_STEP 100
#define CONFIRM_STEP 120.0F
/* the free blocks' label and a file's label stand LABEL_SHIFT left of the
 * files' x. the free blocks' label and meter draw at BLOCKS_Y. each row's
 * frame starts FRAME_SHIFT left of the files' x, the first row's at
 * FRAME_Y */
#define LABEL_SHIFT 90
#define BLOCKS_Y 120.0F
#define FRAME_SHIFT 128.0F
#define FRAME_Y 146.0F
/* the draws' scales */
#define DIGITS_SCALE 0.75F /* the free blocks' and the slots' digits */
#define VALUE_SCALE 0.5F /* a file's other values */
#define NAME_SCALE 0.55F /* a stage's name */
#define DIALOG_SCALE 1.2F
/* the meter draws METER_WIDTH pixels across, which METER_BLOCKS fill */
#define METER_WIDTH 100.0F
#define METER_BLOCKS 200
/* share_data_number_draw's digits. the free blocks take three places, with
 * their leading zeros blank. a file's slot takes two, with its leading zero
 * drawn. the emblems take a place for each digit */
#define BLOCKS_DIGITS 3
#define SLOT_DIGITS (-2)
#define EMBLEM_DIGITS 0
/* the most hours a file's play time draws. a longer time draws as
 * MOST_HOURS hours, MOST_MINUTES minutes and as many seconds. a time of more
 * than MOST_TWO_DIGIT_HOURS hours draws its hours in three digits */
#define MOST_HOURS 999
#define MOST_MINUTES 59
#define MOST_TWO_DIGIT_HOURS 99

/* a frame of frame_sizes: a file's, the new row's and the dialog's
 * cursor */
#define FILE_FRAME 0
#define NEW_FRAME 1
#define CHOICE_FRAME 2
/* the steps between a frame's cells. CORNER_STEP runs from a corner to its
 * first edge, and EDGE_STEP from each edge to the next. the corner after the
 * last edge lies LAST_EDGE_STEP past where a next edge would */
#define CORNER_STEP 9.0F
#define EDGE_STEP 7.0F
#define LAST_EDGE_STEP 2.0F

/* what the task draws, which its step moves toward what the work asks */
struct file_select_view {
    Sint32 swing; /* the drives' swing, in degrees */
    Float level; /* the level of the unlit drives' rings, from DIM_LEVEL to 1 */
    NJS_POINT3 drive_pos[CARD_DRIVES];
    Sint32 blink; /* the pulse of the drive under the cursor, in degrees */
    Sint32 files_x;
    /* the files' window, as menu_scroll_step moves it */
    Sint32 top;
    Sint32 cursor;
};
static struct file_select_view file_select_view;
/* The drive that blinked on the previous draw. */
static Sint32 file_select_blink_drive;

/* a drive's place once it has swung in, and the roll of its circle */
struct drive_place {
    NJS_POINT3 pos;
    Angle roll;
};
/* a frame's size, in edge cells across and down */
struct frame_size {
    Sint16 across;
    Sint16 down;
};

/* by character, its portrait's cell */
static const Sint32 portraits[CHARACTER_KINDS] = {32, 29, 33, 30, 34, 31,
                                                  33, 30, 32, 32, 29};
/* by stage below SAVE_STAGES, its name's cell, or NO_STAGE_NAME */
/* clang-format off */
static const Sint32 stage_names[SAVE_STAGES] = {
    NO_STAGE_NAME, NO_STAGE_NAME, NO_STAGE_NAME, 54, 74, 55, 76, 57,
    73, 52, 53, 66, 72, 50, 69, NO_STAGE_NAME,
    51, 56, 67, NO_STAGE_NAME, NO_STAGE_NAME, 68, 64, 59,
    62, 61, 70, 71, 60, NO_STAGE_NAME, 65, 81,
    63, NO_STAGE_NAME, 80, 80, 80, 80, 80, NO_STAGE_NAME,
    79, NO_STAGE_NAME, NO_STAGE_NAME, 78, 77, NO_STAGE_NAME,
};
/* clang-format on */
/* the dialog's middle */
static const NJS_POINT3 dialog_pos = {
#include "advertise/file_select_draw/dialog_pos.inc"
};

/* where the work's message draws */
static NJS_POINT3 message_pos = {
#include "advertise/file_select_draw/message_pos.inc"
};
/* the view as file_select_create starts it, with the drives swung out. the
 * 480 is the blink's, in degrees: files_x starts at 0 */
/* clang-format off */
static struct file_select_view view_start = {
    SWUNG_OUT, 1.0F, {{0.0F, 0.0F, 0.0F}}, 480, 0, 0, 0,
};
/* clang-format on */
/* file_select_create's task, or NULL before it and after file_select_kill */
static struct task *draw_task = NULL;
/* by enum file_select_device, its icon's cell, or 0 for no icon */
static Sint32 device_icons[] = {0, 43, 47, 45};
/* by drive. a port's two drives stand one above the other */
KEPT static struct drive_place drive_places[CARD_DRIVES] = {
#include "advertise/file_select_draw/drive_places.inc"
};
/* by FILE_FRAME, NEW_FRAME and CHOICE_FRAME */
/* clang-format off */
static struct frame_size frame_sizes[] = {
    {33, 11},
    {33,  6},
    {12,  4},
};
/* clang-format on */
/* a file's three bands. band_x and band_y place the first from the stage
 * label's place. band_step steps to the next, and the two scales size each
 * band */
static Float band_x = 85.0F;
static Float band_y = 0.0F;
static Float band_step = 16.0F;
static Float band_scale_x = 0.65F;
static Float band_scale_y = 0.6F;
static Float stage_name_x = 5.0F;
static Float dat_8c5672ec = 0.0F;
static Float dat_8c5672f0 = 0.0F;
/* the cursor's x on FILE_SELECT_DIALOG_CONFIRM's second choice, from its
 * first's */
static Float second_choice_x = 123.0F;
#include "advertise/file_select_draw/text.inc"

static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/file_select_draw/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/file_select_draw/texlist.inc"
};
/* file_select_create sets each cell's center, cx and cy, to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/file_select_draw/cells.inc"
};

static void file_select_sprite_draw(Int cell, Float scale,
                                    const NJS_POINT3 *pos, Int attr)
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

/* draw cell scaled by scale's x across and its y down */
static void file_select_sprite_draw_scaled(Int cell, const NJS_POINT3 *scale,
                                           const NJS_POINT3 *pos, Int attr)
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

static void file_select_sprite_draw_wide(Int cell, Float width,
                                         const NJS_POINT3 *pos, Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = width;
    sprite.sy = 1.0F;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* the turn by ang shows only under NJD_SPRITE_ANGLE */
static void file_select_sprite_draw_turned(Int cell, Float scale,
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

static void file_select_drives_place(void)
{
    struct menu_panel panel;
    Int i;

    for (i = 0; i < CARD_DRIVES; i++) {
        panel.pos = drive_places[i].pos;
        panel.roll = drive_places[i].roll;
        panel.depth = DRIVE_DEPTH;
        panel.radius = DRIVE_RADIUS;
        menu_panel_point(&panel, &file_select_view.drive_pos[i],
                         file_select_view.swing);
    }
}

/* draw frame of frame_sizes, its top left corner's cell at pos */
static void file_select_frame_draw(Int frame, const NJS_POINT3 *pos)
{
    NJS_POINT3 edge;
    Int i;

    edge = *pos;
    file_select_sprite_draw(CORNER_CELL, 1.0F, &edge, MENU_SPRITE_ATTR);
    edge.x += CORNER_STEP;
    for (i = 0; i < frame_sizes[frame].across; i++) {
        file_select_sprite_draw(EDGE_CELL, 1.0F, &edge, MENU_SPRITE_ATTR);
        edge.x += EDGE_STEP;
    }
    edge.x += LAST_EDGE_STEP;
    file_select_sprite_draw(CORNER_CELL, 1.0F, &edge,
                            MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP);
    edge.y += CORNER_STEP;
    for (i = 0; i < frame_sizes[frame].down; i++) {
        file_select_sprite_draw_turned(EDGE_CELL, 1.0F, &edge, -QUARTER_TURN,
                                       NJD_SPRITE_ANGLE | MENU_SPRITE_ATTR);
        edge.y += EDGE_STEP;
    }
    edge = *pos;
    edge.y += CORNER_STEP;
    for (i = 0; i < frame_sizes[frame].down; i++) {
        file_select_sprite_draw_turned(EDGE_CELL, 1.0F, &edge, QUARTER_TURN,
                                       NJD_SPRITE_ANGLE | MENU_SPRITE_ATTR);
        edge.y += EDGE_STEP;
    }
    edge.y += LAST_EDGE_STEP;
    file_select_sprite_draw(CORNER_CELL, 1.0F, &edge,
                            MENU_SPRITE_ATTR | NJD_SPRITE_VFLIP);
    edge.x += CORNER_STEP;
    for (i = 0; i < frame_sizes[frame].across; i++) {
        file_select_sprite_draw(EDGE_CELL, 1.0F, &edge,
                                MENU_SPRITE_ATTR | NJD_SPRITE_VFLIP);
        edge.x += EDGE_STEP;
    }
    edge.x += LAST_EDGE_STEP;
    file_select_sprite_draw(CORNER_CELL, 1.0F, &edge,
                            MENU_SPRITE_ATTR | NJD_SPRITE_HFLIP |
                                NJD_SPRITE_VFLIP);
}

/* draw the plate behind the files, and the drive's free blocks */
static void file_select_blocks_draw(void)
{
    NJS_POINT3 pos;
    Int text_offset;

    pos.x = file_select_view.files_x - 5;
    pos.y = 124.0F;
    pos.z = PLATE_PRIORITY;
    menu_fade_begin(PLATE_LEVEL);
    file_select_sprite_draw(PLATE_TOP_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.y += PLATE_STEP;
    file_select_sprite_draw(PLATE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.y += PLATE_STEP;
    file_select_sprite_draw(PLATE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.y += PLATE_STEP;
    file_select_sprite_draw(PLATE_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    pos.y += PLATE_STEP;
    file_select_sprite_draw(PLATE_BOTTOM_CELL, 1.0F, &pos, MENU_SPRITE_ATTR);
    if (game_state.text_language == TEXT_JAPANESE) {
        text_offset = 0;
    } else {
        text_offset = OTHER_TEXT;
    }
    pos.x = file_select_view.files_x - LABEL_SHIFT;
    pos.y = BLOCKS_Y;
    pos.z = LABEL_PRIORITY;
    menu_fade_begin(1.0F);
    file_select_sprite_draw(text_offset + BLOCKS_LABEL_CELL, 1.0F, &pos,
                            MENU_SPRITE_ATTR);
    pos.x = file_select_view.files_x + 10;
    pos.y = BLOCKS_Y;
    pos.z = LABEL_PRIORITY;
    file_select_sprite_draw_wide(METER_CELL, METER_WIDTH, &pos,
                                 MENU_SPRITE_ATTR);
    pos.y += 1.0F;
    pos.z = FRONT_PRIORITY;
    if (file_select_work.free_blocks > METER_BLOCKS) {
        file_select_sprite_draw_wide(METER_FILL_CELL, METER_WIDTH, &pos,
                                     MENU_SPRITE_ATTR);
    } else {
        file_select_sprite_draw_wide(METER_FILL_CELL,
                                     file_select_work.free_blocks *
                                         (METER_WIDTH / METER_BLOCKS),
                                     &pos, MENU_SPRITE_ATTR);
    }
    pos.x = file_select_view.files_x - 40;
    share_data_number_draw(&pos, file_select_work.free_blocks, BLOCKS_DIGITS,
                           DIGITS_SCALE);
}

/* draw the files in the window. the frame of the file under the cursor is
 * lit while the cursor is on the files */
static void file_select_files_draw(void)
{
    NJS_POINT3 pos;
    NJS_POINT3 band_pos;
    NJS_POINT3 band_scale;
    Int text_offset;
    Int name_offset;
    Int name;
    Int file;
    Int i;

    if (game_state.text_language == TEXT_JAPANESE) {
        text_offset = 0;
    } else {
        text_offset = OTHER_TEXT;
    }
    for (i = 0; i < SHOWN_FILES; i++) {
        /* @hack spelling: the top read, and i added in a statement of its
         * own, where top + i would do */
        file = file_select_view.top;
        file += i;
        if (file >= file_select_work.file_count) {
            break;
        }
        menu_fade_begin(1.0F);
        pos.x = file_select_view.files_x - LABEL_SHIFT;
        pos.y = (i * ROW_STEP) + 160.0F;
        pos.z = FRONT_PRIORITY;
        file_select_sprite_draw(text_offset + FILE_LABEL_CELL, 1.0F, &pos,
                                MENU_SPRITE_ATTR);
        pos.x += 50.0F;
        share_data_number_draw(&pos, file_select_work.files[file].slot,
                               SLOT_DIGITS, DIGITS_SCALE);
        /* @hack spelling: two moves of x, where one of -50 would do */
        pos.x += 20.0F;
        pos.x -= 70.0F;
        pos.y += 40.0F;
        file_select_sprite_draw(
            portraits[file_select_work.files[file].character], 1.0F, &pos,
            MENU_SPRITE_ATTR);
        pos.x += 56.0F;
        pos.y -= LINE_STEP;
        band_pos = pos;
        file_select_sprite_draw(text_offset + STAGE_LABEL_CELL, 1.0F, &pos,
                                MENU_SPRITE_ATTR);
        pos.y += LINE_STEP;
        file_select_sprite_draw(text_offset + TIME_LABEL_CELL, 1.0F, &pos,
                                MENU_SPRITE_ATTR);
        pos.y += LINE_STEP;
        share_data_sprite_draw(SHARE_DATA_EMBLEM_CELL, VALUE_SCALE, &pos,
                               MENU_SPRITE_ATTR);
        band_pos.x += band_x;
        band_pos.y += band_y;
        band_pos.z -= 1.0F;
        band_scale.x = band_scale_x;
        band_scale.y = band_scale_y;
        file_select_sprite_draw_scaled(BAND_CELL, &band_scale, &band_pos,
                                       MENU_SPRITE_ATTR);
        band_pos.y += band_step;
        file_select_sprite_draw_scaled(BAND_CELL, &band_scale, &band_pos,
                                       MENU_SPRITE_ATTR);
        band_pos.y += band_step;
        file_select_sprite_draw_scaled(BAND_CELL, &band_scale, &band_pos,
                                       MENU_SPRITE_ATTR);
        pos.x += 80.0F;
        pos.y -= 2 * LINE_STEP;
        if (game_state.text_language) {
            name_offset = OTHER_STAGE_NAMES;
        } else {
            name_offset = 0;
        }
        if (file_select_work.files[file].stage < SAVE_STAGES) {
            name = stage_names[file_select_work.files[file].stage];
        } else if (file_select_work.files[file].stage == STAGE_CART_1) {
            if (file_select_work.files[file].character == PLAYER_ROUGE) {
                name = ROUGE_KART_NAME_CELL;
            } else {
                name = KART_NAME_CELL;
            }
        } else {
            name = NO_STAGE_NAME;
        }
        /* @hack spelling: the draw in the else of an empty test, where a test
         * of name >= 0 would do */
        if (name < 0) {
        } else {
            NJS_POINT3 name_pos;

            name_pos = pos;
            name_pos.x += stage_name_x;
            file_select_sprite_draw(name + name_offset, NAME_SCALE, &name_pos,
                                    MENU_SPRITE_ATTR);
        }
        pos.x -= 32.0F;
        pos.y += LINE_STEP;
        {
            struct save_time short_time;
            struct share_data_clock long_time;

            if (file_select_work.files[file].time.hour > MOST_HOURS) {
                long_time.hour = MOST_HOURS;
                /* @hack type: the minutes' MOST_MINUTES through a Sint8 into
                 * the seconds, where MOST_MINUTES stored in each would do */
                long_time.second = (Sint8)(long_time.minute = MOST_MINUTES);
                share_data_clock_draw(&pos, &long_time, VALUE_SCALE);
            } else if (file_select_work.files[file].time.hour >
                       MOST_TWO_DIGIT_HOURS) {
                long_time.hour = file_select_work.files[file].time.hour;
                long_time.minute = file_select_work.files[file].time.minute;
                long_time.second = file_select_work.files[file].time.second;
                share_data_clock_draw(&pos, &long_time, VALUE_SCALE);
            } else {
                /* share_data_time_draw draws each of a save_time's fields
                 * in two places. these hold the clock's hours, minutes and
                 * seconds */
                short_time.minute = file_select_work.files[file].time.hour;
                short_time.second = file_select_work.files[file].time.minute;
                short_time.hundredths =
                    file_select_work.files[file].time.second;
                share_data_time_draw(&pos, &short_time, VALUE_SCALE);
            }
        }
        pos.y += LINE_STEP;
        share_data_number_draw(&pos, file_select_work.files[file].emblems,
                               EMBLEM_DIGITS, VALUE_SCALE);
        pos.x = file_select_view.files_x - FRAME_SHIFT;
        pos.y = (i * ROW_STEP) + FRAME_Y;
        if (i == file_select_work.file - file_select_view.top &&
            file_select_work.choosing_file && file_select_work.on_new == 0) {
            menu_fade_begin(1.0F);
        } else {
            menu_fade_begin(FRAME_DIM_LEVEL);
        }
        file_select_frame_draw(FILE_FRAME, &pos);
    }
}

/* draw the new row, which says that the drive lacks the room while the
 * work's room is 0. its frame is lit while the cursor is on it */
static void file_select_new_row_draw(void)
{
    NJS_POINT3 pos;
    Float rows_y;
    Int count;
    Int rows;

    count = file_select_work.file_count;
    if (count < SHOWN_FILES + 1) {
        rows = count;
    } else {
        rows = SHOWN_FILES;
    }
    /* @hack spelling: rows_y set inside the sum, where a statement of its
     * own would do */
    pos.y = (rows_y = rows * ROW_STEP) + 172.0F;
    pos.z = LABEL_PRIORITY;
    if (game_state.text_language != TEXT_JAPANESE) {
        pos.x = file_select_view.files_x;
        if (file_select_work.room) {
            file_select_sprite_draw(NEW_FILE_CELL, 1.0F, &pos,
                                    NJD_SPRITE_ALPHA);
        } else {
            file_select_sprite_draw(NO_ROOM_CELL, 1.0F, &pos, NJD_SPRITE_ALPHA);
        }
    } else {
        pos.x = file_select_view.files_x - 15.0F;
        /* the cells' half widths truncate to whole pixels */
        /* NOLINTBEGIN(bugprone-integer-division) */
        if (file_select_work.room) {
            file_select_sprite_draw(NEW_FILE_JAPANESE_CELL, 1.0F, &pos,
                                    NJD_SPRITE_ALPHA);
            pos.x += (cells[NEW_FILE_JAPANESE_CELL].sx / 2) +
                     (cells[NEW_FILE_TAIL_CELL].sx / 2);
            file_select_sprite_draw(NEW_FILE_TAIL_CELL, 1.0F, &pos,
                                    NJD_SPRITE_ALPHA);
        } else {
            file_select_sprite_draw(NO_ROOM_JAPANESE_CELL, 1.0F, &pos,
                                    NJD_SPRITE_ALPHA);
            pos.x += (cells[NO_ROOM_JAPANESE_CELL].sx / 2) +
                     (cells[NO_ROOM_TAIL_CELL].sx / 2);
            file_select_sprite_draw(NO_ROOM_TAIL_CELL, 1.0F, &pos,
                                    NJD_SPRITE_ALPHA);
        }
        /* NOLINTEND(bugprone-integer-division) */
    }
    pos.x = file_select_view.files_x - FRAME_SHIFT;
    pos.y = rows_y + FRAME_Y;
    if (file_select_work.on_new != 0 && file_select_work.choosing_file) {
        menu_fade_begin(1.0F);
    } else {
        menu_fade_begin(FRAME_DIM_LEVEL);
    }
    file_select_frame_draw(NEW_FRAME, &pos);
}

/* draw a drive at pos: its ring, and over it the device's icon unless icon
 * is 0. scale scales both, and is each channel of the constant material. a
 * lit drive's icon adds to what lies behind it, and pulses with the blink.
 * an unlit drive's ring takes file_select_view.level too */
static void file_select_drive_draw(Int icon, Float scale, const NJS_POINT3 *pos,
                                   Bool lit)
{
    NJS_POINT3 at;

    draw_attr_save();
    constant_material_set(scale, scale, scale, scale);
    at = *pos;
    at.z = DRIVE_PRIORITY;
    if (icon != 0) {
        if (lit) {
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            file_select_sprite_draw(
                icon,
                scale * (_builtin_fabsf(njCos((Angle)(file_select_view.blink *
                                                      ANGLE_PER_DEGREE)) *
                                        PULSE) +
                         1.0F),
                &at, MENU_SPRITE_ATTR);
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
        } else {
            file_select_sprite_draw(icon + 1, scale, &at, MENU_SPRITE_ATTR);
        }
    }
    at.z -= 1.0F;
    if (lit) {
        file_select_sprite_draw(LIT_RING_CELL, scale, &at, MENU_SPRITE_ATTR);
    } else {
        menu_fade_begin(file_select_view.level * scale);
        file_select_sprite_draw(DIM_RING_CELL, scale, &at, MENU_SPRITE_ATTR);
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

/* draw the dialog, and its cursor on the choice */
static void file_select_dialog_draw(void)
{
    NJS_POINT3 pos;
    Int text_offset;

    pos = dialog_pos;
    menu_fade_begin(1.0F);
    file_select_sprite_draw(DIALOG_CELL, DIALOG_SCALE, &pos, NJD_SPRITE_ALPHA);
    if (game_state.text_language) {
        text_offset = OTHER_TEXT;
    } else {
        text_offset = 0;
    }
    pos.z += 1.0F;
    if (file_select_work.dialog == FILE_SELECT_DIALOG_FILE) {
        NJS_POINT3 text_pos;

        text_pos = pos;
        /* @hack spelling: y added -30, where 30 taken from it */
        text_pos.y += -30.0F;
        file_select_sprite_draw(text_offset + DIALOG_TEXT_CELL, 1.0F, &text_pos,
                                NJD_SPRITE_ALPHA);
        /* @hack spelling: x added -100, where 100 taken from it */
        pos.x += -100.0F;
        pos.y += 20.0F;
        file_select_sprite_draw(text_offset + FIRST_CHOICE_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        pos.x += CHOICE_STEP;
        file_select_sprite_draw(text_offset + SECOND_CHOICE_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        pos.x += CHOICE_STEP;
        file_select_sprite_draw(text_offset + THIRD_CHOICE_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        pos.x += (file_select_work.choice * CHOICE_STEP) - 245;
        /* @hack spelling: y added -20, where 20 taken from it */
        pos.y += -20.0F;
    } else {
        /* @hack spelling: y added -30, where 30 taken from it */
        pos.y += -30.0F;
        file_select_sprite_draw(text_offset + DIALOG_TEXT_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        pos.y += 40.0F;
        /* @hack spelling: x added -60, where 60 taken from it */
        pos.x += -60.0F;
        file_select_sprite_draw(text_offset + FIRST_CHOICE_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        pos.x += CONFIRM_STEP;
        file_select_sprite_draw(text_offset + SECOND_CHOICE_CELL, 1.0F, &pos,
                                NJD_SPRITE_ALPHA);
        if (file_select_work.choice) {
            pos.x += second_choice_x - 170.0F;
            /* @hack spelling: y added -20, where 20 taken from it */
            pos.y += -20.0F;
        } else {
            /* @hack spelling: x added -170, where 170 taken from it */
            pos.x += -170.0F;
            /* @hack spelling: y added -20, where 20 taken from it */
            pos.y += -20.0F;
        }
    }
    file_select_frame_draw(CHOICE_FRAME, &pos);
}

/* move the view toward what the work asks, and say in the work what
 * moved */
static void file_select_task_step(struct task *task)
{
    struct menu_scroll scroll;

    file_select_work.moving = FALSE;
    if (!file_select_work.open) {
        file_select_work.drives_shown = FALSE;
        file_select_work.files_shown = FALSE;
        file_select_work.dialog_shown = FALSE;
    }
    file_select_work.drives_moving = FALSE;
    if (file_select_work.drives_shown) {
        if (menu_approach_int(&file_select_view.swing, SWUNG_IN, SWING_STEP)) {
            file_select_work.drives_moving = TRUE;
        }
    } else {
        if (menu_approach_int(&file_select_view.swing, SWUNG_OUT, SWING_STEP)) {
            file_select_work.drives_moving = TRUE;
        }
    }
    file_select_drives_place();
    file_select_work.files_moving = FALSE;
    if (file_select_work.files_shown) {
        if (file_select_work.drive < FIRST_LEFT_DRIVE) {
            if (menu_approach_int(&file_select_view.files_x, FILES_RIGHT_X,
                                  SLIDE_STEP)) {
                file_select_work.files_moving = TRUE;
            }
        } else {
            if (menu_approach_int(&file_select_view.files_x, FILES_LEFT_X,
                                  SLIDE_STEP)) {
                file_select_work.files_moving = TRUE;
            }
        }
        scroll.top = &file_select_view.top;
        scroll.cursor = &file_select_view.cursor;
        scroll.target = file_select_work.file;
        scroll.shown = SHOWN_FILES;
        menu_scroll_step(&scroll);
    }
    if (file_select_work.choosing_file || file_select_work.dialog_shown) {
        if (menu_approach(&file_select_view.level, DIM_LEVEL, FADE_STEP)) {
            file_select_work.files_moving = TRUE;
        }
    } else {
        if (menu_approach(&file_select_view.level, 1.0F, FADE_STEP)) {
            file_select_work.files_moving = TRUE;
        }
    }
    file_select_work.unk_100 = 0;
    /* @hack redundant: moving read into its own sum, though the step set it
     * to 0 */
    file_select_work.moving = file_select_work.moving |
                              file_select_work.drives_moving |
                              file_select_work.files_moving;
}

static void file_select_task_draw(struct task *task)
{
    Float scale;
    Int i;

    if (file_select_view.swing != SWUNG_OUT) {
        scale = file_select_view.swing * (1.0F / SWUNG_IN);
        if (file_select_blink_drive == file_select_work.drive) {
            file_select_view.blink += BLINK_STEP;
        } else {
            file_select_view.blink = 0;
            file_select_blink_drive = file_select_work.drive;
        }
        for (i = 0; i < CARD_DRIVES; i++) {
            /* @hack spelling: drive_lit times a comparison, where a
             * conditional expression would do */
            file_select_drive_draw(device_icons[file_select_work.devices[i]],
                                   scale, &file_select_view.drive_pos[i],
                                   file_select_work.drive_lit *
                                       (file_select_work.drive == i));
        }
    }
    if (file_select_work.files_shown) {
        file_select_blocks_draw();
        file_select_files_draw();
        file_select_new_row_draw();
        if (file_select_work.message >= 0) {
            message_sprite_draw(file_select_work.message, &message_pos, 1.0F);
        }
    }
    if (file_select_work.dialog_shown) {
        file_select_dialog_draw();
    }
}

static void file_select_task_destroy(struct task *task)
{
    file_select_release();
}

void file_select_create(void)
{
    struct task *task;
    Int i;

    task = task_create(TASK_WORK, TASK_LEVEL, file_select_task_step,
                       "fileSelectExec");
    task->draw = file_select_task_draw;
    task->destructor = file_select_task_destroy;
    draw_task = task;
    /* @hack spelling: the clear in a do-while of one pass, where a block
     * would do */
    do {
        Uint8 *byte = (Uint8 *)&file_select_work;
        int left;

        /* @hack spelling: a pointer beside a count of the bytes left tested
         * against 0, where byte[i] for an unsigned i counting up would do */
        for (left = sizeof(file_select_work); left != 0; left--) {
            *byte++ = 0;
        }
    } while (0);
    file_select_view = view_start;
    for (i = 0; i < CELLS; i++) {
        /* @bug logic: the test of the meter's two cells always holds, so
         * they take their middles as the rest do (CMP/EQ #35 at 0x8c512382,
         * then CMP/EQ #36 at 0x8c512388) */
        if (i != METER_CELL || i != METER_FILL_CELL) {
            cells[i].cx = cells[i].sx / 2;
            cells[i].cy = cells[i].sy / 2;
        }
    }
}

void file_select_kill(void)
{
    if (draw_task != NULL) {
        task_destroy(draw_task);
        draw_task = NULL;
    }
}

void file_select_load(void)
{
    if (!file_select_work.loaded) {
        texture_load_pvm(FILE_SELECT_ARCHIVE, &texlist);
        file_select_work.loaded = TRUE;
    }
}

void file_select_release(void)
{
    if (file_select_work.loaded) {
        njReleaseTexture(&texlist);
        file_select_work.loaded = FALSE;
    }
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}
