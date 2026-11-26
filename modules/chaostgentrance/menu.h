/* the race entrance's menu, and the parts it makes as its task's children */
#ifndef CHAOSTGENTRANCE_MENU_H
#define CHAOSTGENTRANCE_MENU_H
#include <sg_xpt.h>
#include "chao/garden_save.h"
#include "chao/race_record.h"

struct task;

/* the menu task's work->mode. no mode enters MENU_UNUSED_8. a course screen
 * takes its input in MENU_MULTI_INPUT, MENU_LEVEL_INPUT or MENU_GRID_INPUT,
 * by its race group */
enum entrance_menu_mode {
    MENU_INIT,
    MENU_BUILD_GROUPS,
    MENU_WAIT_GROUPS,
    MENU_GROUP_INPUT,
    MENU_CLOSE_GROUPS,
    MENU_BUILD_PICKER,
    MENU_SCAN_DRIVE,
    MENU_READ_DRIVE,
    MENU_UNUSED_8,
    MENU_WAIT_PICKER,
    MENU_PICKER_INPUT,
    MENU_CLOSE_PICKER,
    MENU_WAIT_PICKER_CLOSE,
    MENU_BUILD_COURSES,
    MENU_WAIT_COURSES,
    MENU_MULTI_INPUT,
    MENU_LEVEL_INPUT,
    MENU_GRID_INPUT,
    MENU_CLOSE_COURSES,
    MENU_WAIT_COURSES_CLOSE,
    MENU_BOARD_ELEVATOR
};
/* a mode from MENU_BUILD_COURSES on is past the picker. a mode from
 * MENU_COURSE_INPUT on is past the wait for a course screen's parts */
#define MENU_COURSE_INPUT MENU_MULTI_INPUT

enum entrance_screen {
    ENTRANCE_SELECT_GROUP,
    ENTRANCE_SELECT_CHAO,
    ENTRANCE_SELECT_COURSE
};

/* what the menu asks of its parts. on a select or a back, the screen's
 * parts leave. on a VMS change, the picker's parts leave, and the menu then
 * scans the drives again. on a leave, set as the racers board the elevator,
 * the title and the dimmer leave */
enum entrance_menu_request {
    ENTRANCE_MENU_IDLE,
    ENTRANCE_MENU_SELECT,
    ENTRANCE_MENU_BACK,
    ENTRANCE_MENU_VMS_CHANGED,
    ENTRANCE_MENU_LEAVE
};

/* the picker's page turn this frame. L turns to the previous page, and R
 * to the next */
enum entrance_turn {
    ENTRANCE_NO_TURN,
    ENTRANCE_TURN_PREVIOUS,
    ENTRANCE_TURN_NEXT
};

/* the picker's slots stand SLOT_ROWS to a column, in SLOT_COLUMNS columns.
 * the rows below them hold multiplayer's confirm and back buttons, or
 * another group's back button */
#define SLOT_ROWS 2
#define SLOT_COLUMNS 4
#define CONFIRM_ROW SLOT_ROWS
#define MULTI_BACK_ROW (SLOT_ROWS + 1)
#define BACK_ROW SLOT_ROWS

/* the challenge, dark and hero screens' grid holds GRID_COLUMNS levels a
 * row. the level under the cursor is cursor * GRID_COLUMNS + column */
#define GRID_COLUMNS 4
/* the challenge grid holds FIRST_CHALLENGES levels until the last of them is
 * won, and RACE_CHALLENGE_LEVELS from then on */
#define FIRST_CHALLENGES 8

/* the Chao and the names the menu places in the world stand at z PREVIEW_Z.
 * a pixel of an offset on the screen is WORLD_PER_PIXEL in the world. a Chao
 * shown at y PREVIEW_ORIGIN_Y on the screen stands at a height of
 * PREVIEW_HEIGHT */
#define PREVIEW_Z (-60.0F)
#define WORLD_PER_PIXEL 0.06F
#define PREVIEW_ORIGIN_Y 222.0F
#define PREVIEW_HEIGHT 10.0F
/* stop a Chao the menu shows from colliding, moving, running its behavior
 * and springing its body. the caller includes chao/work.h */
#define FREEZE_CHAO(actor)                                                     \
    CHAO_WORK(actor)->flags &= ~CHAO_COLLIDES;                                 \
    CHAO_WORK(actor)->flags &= ~CHAO_MOVES;                                    \
    CHAO_WORK(actor)->flags &= ~CHAO_RUNS_BEHAVIOR;                            \
    CHAO_WORK(actor)->flags &= ~CHAO_BODY_SPRINGS

/* the multiplayer screen's columns, its beginner courses' on the left and
 * its jewel courses' on the right */
enum { BEGINNER_COLUMN, JEWEL_COLUMN };

struct entrance_choice {
    Bool selected;
};

/* the menu's state, which its parts read.
 *
 * each frame, the menu's step runs before its parts' steps. a part sets
 * wait in each frame that it is still moving. a mode that waits for the
 * parts clears wait after it reads it. so the mode moves on in the frame
 * after one in which no part set wait */
struct entrance_menu {
    Bool wait;
    Sint32 request; /* an enum entrance_menu_request */
    enum entrance_screen screen;
    /* the group screen's exit button's row, below its groups */
    Sint32 exit_row;
    /* the beginner or jewel screen's rows, a course each. its back button is
     * on the row after them */
    Sint32 rows;
    /* the picker's pages, a row of entrance_roster each. pages counts the
     * pages the picker offers, and page is the row of the page shown */
    Sint32 pages, page;
    Sint32 levels; /* the beginner or jewel screen's levels, a column each */
    /* the multiplayer screen's courses, a row each: beginner on the left, and
     * jewel on the right */
    Sint32 left_rows, right_rows;
    enum entrance_turn turn;
    Sint32 unk_2c;
    /* the selection's column and row on the screen. old_column and
     * old_cursor hold them as they were before this frame's input. a part
     * that finds a pair unequal knows the selection moved */
    Sint32 column, cursor, old_column, old_cursor;
    Sint32 back; /* the course screen's back button's row */
    Sint32 unk_44;
    /* the picker's Chao, a slot each. on the course screen, [0] is the
     * chosen Chao and [1] the record holder */
    struct task *preview[RACE_ENTRANTS];
    /* choices[row][i] says whether the picker chose entrance_roster[row][i] */
    struct entrance_choice choices[CHAO_GARDENS + 1][RACE_ENTRANTS];
    struct task *task;
    /* the parts the menu holds at most one of, each NULL while there is
     * none */
    struct task *message;
    struct task *dimmer;
    struct task *title, *caption;
    struct task *back_button, *confirm_button;
    /* whether the picker offers each row of entrance_roster as a page */
    Bool available[CHAO_GARDENS + 1];
};

extern struct entrance_menu *entrance_menu;
/* make the menu's task and entrance_menu, if entrance_menu is NULL. once the
 * last level of every beginner course is won, it opens the jewel and
 * challenge groups. while the Hero and Dark Gardens are open, it then opens
 * the dark and hero groups too. the task's destructor destroys its children,
 * frees entrance_menu and sets it to NULL */
void entrance_menu_create(void);
/* each makes its part as a child of parent. a part kept in a field of
 * entrance_menu is made only while that field is NULL. its destructor clears
 * the field */
void entrance_chao_picker(struct task *parent);
void entrance_course_preview(struct task *parent);
/* the group screen's buttons: a button for each open group the way in
 * offers, then the exit. through the lobby, under CHAO_RACE_MENU_NONE, the
 * screen offers every group. under CHAO_RACE_MENU_VERSUS it offers
 * RACE_MULTI alone, and under CHAO_RACE_MENU_EMBLEMS every group but
 * RACE_MULTI */
void entrance_event_buttons(struct task *parent);
/* the title and the caption wait delay + 1 frames before they enter */
void entrance_title_create(struct task *parent, Sint16 delay);
void entrance_caption_create(struct task *parent, Sint16 delay);
void entrance_dimmer_create(struct task *parent);
/* a button at x, y on the screen, which the cursor selects on row item */
void entrance_back_button(struct task *parent, Float x, Float y, int item);
void entrance_confirm_button(struct task *parent, Float x, Float y, int item);

/* the message bar across the screen's foot. it waits delay frames, then
 * fades in. no request sends it out, so it lasts as long as the menu. the
 * bar shows its parts in turn, and after the last the first again. a part is
 * the text of count messages from garden_message(id) on, joined. the calls
 * below that take an id require the bar */
void entrance_message_create(struct task *parent, Sint16 delay);
/* the messages the menu shows, by garden_message id. GROUP_MESSAGE takes an
 * enum garden_race_group, and COURSE_MESSAGE a course's event */
#define GROUPS_MESSAGE 20 /* the group screen's */
#define GROUP_MESSAGE(group) ((group) + 21)
#define GARDEN_EXIT_MESSAGE 27 /* the exit's, to the Neutral Garden */
#define PICK_MESSAGE 29
/* multiplayer's picker shows MULTI_PICK_MESSAGE, then MULTI_PICK_MESSAGE + 1 */
#define MULTI_PICK_MESSAGE 30
#define PICKER_BACK_MESSAGE 32
#define CONFIRM_MESSAGE 33
/* COURSES_MESSAGE is the course screens' message, and LEVELS_MESSAGE the
 * beginner and jewel screens' in its place */
#define COURSES_MESSAGE 35
#define LEVELS_MESSAGE 36
#define COURSE_BACK_MESSAGE 37
#define COURSE_MESSAGE(course) ((course) + 38)
/* make id's message the bar's only part. the text shown, if any, fades out
 * first. an id below 0 leaves the bar empty. a set ends what
 * entrance_message_speed set */
void entrance_message_set(int id);
/* add id's message as a part, unless the bar holds 4 parts already */
void entrance_message_append(Sint8 id);
/* as entrance_message_set, with a part of count messages */
void entrance_message_set_group(Sint8 id, Sint8 count);
/* add a part of count messages, unless the bar holds 8 parts already */
void entrance_message_append_group(Sint8 id, Sint8 count);
/* scroll the text speed pixels a frame, until the next set */
void entrance_message_speed(Float speed);
#endif
