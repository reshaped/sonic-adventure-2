/* the course screens of the race groups. multiplayer's screen lists the
 * beginner courses on the left. on the right it lists the open jewel
 * courses, once the jewel group has been visited */
#include <ninja.h>
#include "chao/race.h"
#include "chao/race_state.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/options.h"
#include "chaostgentrance/rows.h"
#include "chaostgentrance/slide.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"

static void multiplayer_row_step(struct task *task)
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

static void multiplayer_row_draw(struct task *task)
{
    struct entrance_row *row = ROW(task);
    NJS_SPRITE sprite;
    struct entrance_menu *menu;
    START_ROW_SPRITE(sprite, row);
    MULTI_COLOR(1.0F);
    /* once the screen takes input, the row under the cursor is lit in the
     * column selected */
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((menu = entrance_menu)->task->work->mode >= MENU_COURSE_INPUT &&
        menu->cursor == row->row) {
        if ((menu->column == BEGINNER_COLUMN &&
             row->course < RACE_BEGINNER_COURSES) ||
            (menu->column == JEWEL_COLUMN &&
             row->course >= RACE_BEGINNER_COURSES)) {
            DRAW(LIT_NAME_CELLS, -12.5F);
            DRAW(LIT_NAME_CELLS + 1, -12.5F);
            DRAW(LIT_NAME_CELLS + 2, -12.5F);
            LIT_COLOR(1.0F);
        }
    }
    DRAW(NAME_CELLS, -11.5F);
    DRAW(NAME_CELLS + 1, -11.5F);
    DRAW(NAME_CELLS + 2, -11.5F);
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

static void multiplayer_row_destroy(struct task *task)
{
}

/* each column's rows stand from LIST_TOP down */
#define LIST_TOP 150.0F
#define BEGINNER_X 112.0F
#define JEWEL_X 288.0F
#define BEGINNER_DELAY 55
#define JEWEL_DELAY 45
#define PLACE_MULTIPLAYER_ROW(side, count_field, left, delay_frames)           \
    task = task_create_child(TASK_WORK, multiplayer_row_step, parent);         \
    task->draw = multiplayer_row_draw;                                         \
    task->destructor = multiplayer_row_destroy;                                \
    row = ROW(task);                                                           \
    row->x = (left);                                                           \
    row->y = (entrance_menu->count_field * ROW_HEIGHT) + LIST_TOP;             \
    row->delay = (entrance_menu->count_field * ROW_DELAY) + (delay_frames);    \
    row->slide = ROW_OUT;                                                      \
    row->course = course;                                                      \
    row->column = (side);                                                      \
    row->row = entrance_menu->count_field

static void create_multiplayer_list(struct task *parent)
{
    struct task *task;
    struct entrance_row *row;
    int course;
    entrance_message_set(COURSES_MESSAGE);
    entrance_menu->left_rows = 0;
    entrance_menu->right_rows = 0;
    /* @hack spelling: <= RACE_BEGINNER_COURSES - 1, where
     * < RACE_BEGINNER_COURSES would do */
    for (course = 0; course <= RACE_BEGINNER_COURSES - 1; course++) {
        PLACE_MULTIPLAYER_ROW(BEGINNER_COLUMN, left_rows, BEGINNER_X,
                              BEGINNER_DELAY);
        /* @hack spelling: a case for each course, where
         * entrance_label_ids[course + COURSE_LABELS] would do */
        switch (course) {
            NAME_CASE(0);
            NAME_CASE(1);
            NAME_CASE(2);
            NAME_CASE(3);
        }
        entrance_menu->left_rows++;
    }
    if (garden_race_group_status(RACE_JEWEL) == RACE_GROUP_VISITED) {
        /* @hack spelling: <= the last jewel course, where < the course after
         * it would do */
        for (course = RACE_BEGINNER_COURSES;
             course <= RACE_BEGINNER_COURSES + RACE_JEWEL_COURSES - 1;
             course++) {
            if (garden_race_unlocked(course) >= 0) {
                PLACE_MULTIPLAYER_ROW(JEWEL_COLUMN, right_rows, JEWEL_X,
                                      JEWEL_DELAY);
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
                entrance_menu->right_rows++;
            }
        }
    }
}

void entrance_beginner_courses(struct task *parent)
{
    entrance_beginner_heading(parent);
    entrance_course_heading(parent);
    entrance_beginner_names(parent);
    entrance_beginner_levels(parent);
    entrance_menu->levels = RACE_BEGINNER_LEVELS;
}

void entrance_jewel_courses(struct task *parent)
{
    entrance_jewel_heading(parent);
    entrance_course_heading(parent);
    entrance_jewel_names(parent);
    entrance_jewel_levels(parent);
    entrance_menu->levels = RACE_JEWEL_LEVELS;
}

void entrance_challenge_courses(struct task *parent)
{
    entrance_event_info(parent);
    entrance_event_tiles(parent);
}

void entrance_dark_courses(struct task *parent)
{
    entrance_event_info(parent);
    entrance_event_tiles(parent);
}

void entrance_hero_courses(struct task *parent)
{
    entrance_event_info(parent);
    entrance_event_tiles(parent);
}

void entrance_multiplayer_courses(struct task *parent)
{
    create_multiplayer_list(parent);
}
