/* the beginner and jewel screens' rows */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chao/race.h"
#include "chao/race_record.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/rows.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"

#include "chaostgentrance/slide.h"

/* the jewel screen offers the courses past its first JEWEL_GATE once a
 * level of each is won */
#define JEWEL_GATE 4

#define LIST_TOP 168.0F
#define HEADING_Y 140.0F
#define NAMES_X 112.0F
#define BEGINNER_LEVELS_X 208.0F
#define JEWEL_LEVELS_X 192.0F
/* a course's levels stand BEGINNER_STEP or JEWEL_STEP apart, from
 * BEGINNER_FIRST or JEWEL_FIRST right of its row's x */
#define BEGINNER_STEP 32.0F
#define BEGINNER_FIRST 24.0F
#define JEWEL_STEP 29.0F
#define JEWEL_FIRST 22.0F
#define NAMES_DELAY 45
#define LEVELS_DELAY 41
#define LOCKED_SCALE 0.92F
/* each heading's x and delay */
#define BEGINNER_HEADING_X 266.0F
#define JEWEL_HEADING_X 272.0F
#define COURSES_HEADING_DELAY 39
#define LEVELS_HEADING_DELAY 37

/* cells of the common sheet. a won level shows WON_MARK, and the next level
 * and those past it LEVEL_MARK. LEVEL_CURSOR marks the level under the
 * cursor. the levels' numbers run from LEVEL_NUMBERS, level 0's, on */
#define WON_MARK 38
#define LEVEL_MARK 39
#define LEVEL_CURSOR 40
#define LEVEL_NUMBERS 41
/* a name's row ends in NAME_END_CELL, or LIT_NAME_END_CELL under the cursor.
 * each name here ending in _CELLS begins a run of three */
#define LIT_NAME_END_CELL 59
#define LIT_BEGINNER_LEVEL_CELLS 60
#define LIT_JEWEL_LEVEL_CELLS 63
#define NAME_END_CELL 69
#define BEGINNER_LEVEL_CELLS 70
#define JEWEL_LEVEL_CELLS 73
#define JEWEL_HEADING 108
#define COURSES_HEADING 109
#define BEGINNER_HEADING 110

static void course_name_step(struct task *task)
{
    struct entrance_row *row = ROW(task);
    switch (row->mode) {
    case ROW_WAIT:
        ROW_WAIT_FOR(row, ROW_ENTER);
        break;
    case ROW_ENTER:
        ROW_SLIDE_IN(row, row->has_message = TRUE);
        break;
    case ROW_SHOWN:
        if (entrance_menu->cursor == row->row) {
            if (!row->has_message) {
                entrance_message_set(COURSE_MESSAGE(row->course));
                entrance_message_append(LEVELS_MESSAGE);
                row->has_message = TRUE;
            }
        } else {
            row->has_message = FALSE;
        }
        break;
    case ROW_WAIT_TO_LEAVE:
        ROW_WAIT_FOR(row, ROW_LEAVE);
        break;
    case ROW_LEAVE:
        ROW_SLIDE_OUT(task, row);
        break;
    }
    ROW_CHECK_REQUEST(row);
}

static void course_name_draw(struct task *task)
{
    struct entrance_row *row = ROW(task);
    NJS_SPRITE sprite;
    START_ROW_SPRITE(sprite, row);
    switch (garden_race_group()) {
    case RACE_BEGINNER:
        BEGINNER_COLOR(1.0F);
        break;
    case RACE_JEWEL:
        JEWEL_COLOR(1.0F);
        break;
    }
    /* once the screen takes input, the row under the cursor is lit */
    if (entrance_menu->task->work->mode >= MENU_COURSE_INPUT &&
        entrance_menu->cursor == row->row) {
        DRAW(LIT_NAME_CELLS, -12.5F);
        DRAW(LIT_NAME_CELLS + 1, -12.5F);
        DRAW(LIT_NAME_CELLS + 2, -12.5F);
        DRAW(LIT_NAME_END_CELL, -12.45F);
        LIT_COLOR(1.0F);
    }
    DRAW(NAME_CELLS, -11.5F);
    DRAW(NAME_CELLS + 1, -11.5F);
    DRAW(NAME_CELLS + 2, -11.5F);
    DRAW(NAME_END_CELL, -11.45F);
    constant_material_set(row->alpha, 1.0F, 1.0F, 1.0F);
    if (game_state.text_language == TEXT_JAPANESE) {
        sprite.tlist = &entrance_japanese_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_japanese_cells;
    } else {
        sprite.tlist = &entrance_english_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_english_cells;
    }
    DRAW(row->label, -11.4F);
    if (sprite.tanim[row->label + 1].sx < SECOND_CELL_WIDTH) {
        DRAW(row->label + 1, -11.4F);
    }
    CLEAR_COLOR();
}

static void course_name_destroy(struct task *task)
{
}

#define CREATE_NAME_ROW()                                                      \
    task = task_create_child(0, course_name_step, parent);                     \
    task->draw = course_name_draw;                                             \
    task->destructor = course_name_destroy;                                    \
    task->work = syCalloc(1, sizeof(*row));                                    \
    row = ROW(task);                                                           \
    row->x = NAMES_X;                                                          \
    row->y = (entrance_menu->rows * ROW_HEIGHT) + LIST_TOP;                    \
    row->delay = (entrance_menu->rows * ROW_DELAY) + NAMES_DELAY;              \
    row->slide = ROW_OUT;                                                      \
    row->course = course
void entrance_jewel_names(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    int open;
    int course;

    entrance_menu->rows = 0;
    open = 0;
    entrance_message_set(LEVELS_MESSAGE);
    entrance_message_append(COURSE_MESSAGE(RACE_BEGINNER_COURSES));
    /* @hack spelling: <= the last jewel course, where < the course after it
     * would do */
    for (course = RACE_BEGINNER_COURSES;
         course <= RACE_BEGINNER_COURSES + RACE_JEWEL_COURSES - 1; course++) {
        /* @hack spelling: the two tests nested, where && would do */
        if (course >= RACE_BEGINNER_COURSES + JEWEL_GATE) {
            if (open < JEWEL_GATE) {
                break;
            }
        }
        CREATE_NAME_ROW();
        /* @hack redundant: has_message set as the row is made, which its
         * arrival sets again */
        row->has_message = TRUE;
        row->column = 0;
        row->row = entrance_menu->rows;
        /* @hack spelling: a case for each course, where
         * entrance_label_ids[course + COURSE_LABELS] would do */
        switch (course) {
            NAME_CASE(4);
            NAME_CASE(5);
            NAME_CASE(6);
            NAME_CASE(7);
            NAME_CASE(8);
            NAME_CASE(9);
        }
        entrance_menu->rows++;
        if (garden_race_best_level(course) >= 0) {
            open++;
        }
    }
}

void entrance_beginner_names(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    int course;
    entrance_menu->rows = 0;
    entrance_message_set(LEVELS_MESSAGE);
    entrance_message_append(COURSE_MESSAGE(0));
    /* @hack spelling: <= RACE_BEGINNER_COURSES - 1, where
     * < RACE_BEGINNER_COURSES would do */
    for (course = 0; course <= RACE_BEGINNER_COURSES - 1; course++) {
        CREATE_NAME_ROW();
        row->column = 0;
        row->row = entrance_menu->rows;
        /* @hack spelling: a case for each course, where
         * entrance_label_ids[course + COURSE_LABELS] would do */
        switch (course) {
            NAME_CASE(0);
            NAME_CASE(1);
            NAME_CASE(2);
            NAME_CASE(3);
        }
        entrance_menu->rows++;
        /* @hack redundant: a test of the course's best level, which nothing
         * acts on */
        if (garden_race_best_level(course) >= RACE_BEGINNER_LEVELS - 1) {
        }
    }
}

static void level_row_step(struct task *task)
{
    struct entrance_row *row = ROW(task);
    switch (row->mode) {
    case ROW_WAIT:
        ROW_WAIT_FOR(row, ROW_ENTER);
        break;
    case ROW_ENTER:
        ROW_SLIDE_IN(row, (void)0);
        break;
    case ROW_SHOWN:
        break;
    case ROW_WAIT_TO_LEAVE:
        ROW_WAIT_FOR(row, ROW_LEAVE);
        break;
    case ROW_LEAVE:
        ROW_SLIDE_OUT(task, row);
        break;
    }
    ROW_CHECK_REQUEST(row);
}

/* next is the level after the best the course has won. a level below it
 * shows as won, next as open, and one past it as locked */
#define DRAW_LEVEL(level, next)                                                \
    if ((level) < (next)) {                                                    \
        RGB(255, 255, 255);                                                    \
        DRAW(WON_MARK, -11.3F);                                                \
        RGB(255, 0, 0);                                                        \
        DRAW((level) + LEVEL_NUMBERS, -11.2F);                                 \
    } else if ((level) == (next)) {                                            \
        RGB(255, 255, 255);                                                    \
        DRAW(LEVEL_MARK, -11.3F);                                              \
        RGB(23, 171, 234);                                                     \
        DRAW((level) + LEVEL_NUMBERS, -11.2F);                                 \
    } else {                                                                   \
        RGB(0, 0, 255);                                                        \
        DRAW((level) + LEVEL_NUMBERS, -11.2F);                                 \
        RGB(180, 180, 180);                                                    \
        sprite.sx = LOCKED_SCALE;                                              \
        sprite.sy = LOCKED_SCALE;                                              \
        DRAW(LEVEL_MARK, -11.3F);                                              \
        sprite.sx = 1.0F;                                                      \
        sprite.sy = 1.0F;                                                      \
    }                                                                          \
    CLEAR_COLOR()

static void jewel_level_draw(struct task *task)
{
    struct entrance_row *row = ROW(task);
    NJS_SPRITE sprite;
    /* @hack spelling: entrance_menu through menu in the first test, where
     * entrance_menu would do */
    struct entrance_menu *menu;
    int next;
    int level;
    START_ROW_SPRITE(sprite, row);
    JEWEL_COLOR(1.0F);
    menu = entrance_menu;
    if (menu->task->work->mode >= MENU_COURSE_INPUT &&
        menu->cursor == row->row) {
        DRAW(LIT_JEWEL_LEVEL_CELLS, -12.5F);
        DRAW(LIT_JEWEL_LEVEL_CELLS + 1, -12.5F);
        DRAW(LIT_JEWEL_LEVEL_CELLS + 2, -12.5F);
        LIT_COLOR(1.0F);
    }
    DRAW(JEWEL_LEVEL_CELLS, -11.5F);
    DRAW(JEWEL_LEVEL_CELLS + 1, -11.5F);
    DRAW(JEWEL_LEVEL_CELLS + 2, -11.5F);
    CLEAR_COLOR();
    next = garden_race_best_level(row->row + RACE_BEGINNER_COURSES) + 1;
    for (level = 0; level < RACE_JEWEL_LEVELS; level++) {
        sprite.p.x = row->x + row->slide + (level * JEWEL_STEP) + JEWEL_FIRST;
        DRAW_LEVEL(level, next);
        if (entrance_menu->task->work->mode >= MENU_COURSE_INPUT) {
            if (entrance_menu->cursor == row->row) {
                sprite.p.x = row->x + row->slide +
                             (entrance_menu->column * JEWEL_STEP) + JEWEL_FIRST;
                DRAW(LEVEL_CURSOR, -11.4F);
            }
        }
    }
    CLEAR_COLOR();
}

static void beginner_level_draw(struct task *task)
{
    struct entrance_row *row = ROW(task);
    NJS_SPRITE sprite;
    int next;
    int level;
    START_ROW_SPRITE(sprite, row);
    BEGINNER_COLOR(1.0F);
    if (entrance_menu->task->work->mode >= MENU_COURSE_INPUT) {
        if (entrance_menu->cursor == row->row) {
            DRAW(LIT_BEGINNER_LEVEL_CELLS, -12.5F);
            DRAW(LIT_BEGINNER_LEVEL_CELLS + 1, -12.5F);
            DRAW(LIT_BEGINNER_LEVEL_CELLS + 2, -12.5F);
            LIT_COLOR(1.0F);
        }
    }
    DRAW(BEGINNER_LEVEL_CELLS, -11.5F);
    DRAW(BEGINNER_LEVEL_CELLS + 1, -11.5F);
    DRAW(BEGINNER_LEVEL_CELLS + 2, -11.5F);
    CLEAR_COLOR();
    next = garden_race_best_level(row->row) + 1;
    for (level = 0; level < RACE_BEGINNER_LEVELS; level++) {
        sprite.p.x =
            row->x + row->slide + (BEGINNER_STEP * level) + BEGINNER_FIRST;
        DRAW_LEVEL(level, next);
        if (entrance_menu->task->work->mode >= MENU_COURSE_INPUT) {
            if (entrance_menu->cursor == row->row) {
                sprite.p.x = row->x + row->slide +
                             (BEGINNER_STEP * entrance_menu->column) +
                             BEGINNER_FIRST;
                DRAW(LEVEL_CURSOR, -11.4F);
            }
        }
    }
    CLEAR_COLOR();
}

static void level_row_destroy(struct task *task)
{
}

void entrance_jewel_levels(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    int course;
    int open;
    entrance_menu->rows = 0;
    open = 0;
    /* @hack spelling: <= the last jewel course, where < the course after it
     * would do */
    for (course = RACE_BEGINNER_COURSES;
         course <= RACE_BEGINNER_COURSES + RACE_JEWEL_COURSES - 1; course++) {
        /* @hack spelling: the two tests nested, where && would do */
        if (course >= RACE_BEGINNER_COURSES + JEWEL_GATE) {
            if (open < JEWEL_GATE) {
                break;
            }
        }
        task = task_create_child(0, level_row_step, parent);
        task->draw = jewel_level_draw;
        task->destructor = level_row_destroy;
        task->work = syCalloc(1, sizeof(*row));
        row = ROW(task);
        row->x = JEWEL_LEVELS_X;
        row->y = LIST_TOP + (ROW_HEIGHT * entrance_menu->rows);
        row->delay = (entrance_menu->rows * ROW_DELAY) + LEVELS_DELAY;
        row->slide = ROW_FAR_OUT;
        row->course = course;
        row->column = 0;
        row->row = entrance_menu->rows;
        entrance_menu->rows++;
        if (garden_race_best_level(course) >= 0) {
            open++;
        }
    }
}

void entrance_beginner_levels(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    int course;
    int completed;
    entrance_menu->rows = 0;
    completed = 0;
    /* @hack spelling: <= RACE_BEGINNER_COURSES - 1, where
     * < RACE_BEGINNER_COURSES would do */
    for (course = 0; course <= RACE_BEGINNER_COURSES - 1; course++) {
        task = task_create_child(0, level_row_step, parent);
        task->draw = beginner_level_draw;
        task->destructor = level_row_destroy;
        task->work = syCalloc(1, sizeof(*row));
        row = ROW(task);
        row->x = BEGINNER_LEVELS_X;
        row->y = LIST_TOP + (entrance_menu->rows * ROW_HEIGHT);
        row->delay = (entrance_menu->rows * ROW_DELAY) + LEVELS_DELAY;
        row->slide = ROW_FAR_OUT;
        row->course = course;
        row->column = 0;
        row->row = entrance_menu->rows;
        entrance_menu->rows++;
        /* @hack redundant: completed, the count of courses with every level
         * won, which nothing reads */
        if (garden_race_best_level(course) >= RACE_BEGINNER_LEVELS - 1) {
            completed++;
        }
    }
    (void)completed;
}

static void heading_step(struct task *task)
{
    struct entrance_row *row = ROW(task);
    switch (row->mode) {
    case ROW_WAIT:
        ROW_WAIT_FOR(row, ROW_ENTER);
        break;
    case ROW_ENTER:
        ROW_SLIDE_IN(row, (void)0);
        break;
    case ROW_SHOWN:
        break;
    case ROW_WAIT_TO_LEAVE:
        ROW_WAIT_FOR(row, ROW_LEAVE);
        break;
    case ROW_LEAVE:
        ROW_SLIDE_OUT(task, row);
        break;
    }
    ROW_CHECK_REQUEST(row);
}

static void heading_draw(struct task *task)
{
    struct entrance_row *row = ROW(task);
    NJS_SPRITE sprite;
    START_ROW_SPRITE(sprite, row);
    RGB(255, 255, 255);
    DRAW(row->cell, -11.4F);
    CLEAR_COLOR();
}

static void heading_destroy(struct task *task)
{
}

#define PLACE_HEADING(left, delay_frames, sprite_cell)                         \
    task = task_create_child(0, heading_step, parent);                         \
    task->draw = heading_draw;                                                 \
    task->destructor = heading_destroy;                                        \
    task->work = syCalloc(1, sizeof(*row));                                    \
    row = ROW(task);                                                           \
    row->x = (left);                                                           \
    row->y = HEADING_Y;                                                        \
    row->delay = (delay_frames);                                               \
    row->slide = ROW_FAR_OUT;                                                  \
    row->cell = (sprite_cell)

void entrance_course_heading(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    PLACE_HEADING(NAMES_X, COURSES_HEADING_DELAY, COURSES_HEADING);
}
void entrance_beginner_heading(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    PLACE_HEADING(BEGINNER_HEADING_X, LEVELS_HEADING_DELAY, BEGINNER_HEADING);
}
void entrance_jewel_heading(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    PLACE_HEADING(JEWEL_HEADING_X, LEVELS_HEADING_DELAY, JEWEL_HEADING);
}
