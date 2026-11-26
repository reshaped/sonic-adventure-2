/* the course screens' rows. the beginner and jewel screens give each course
 * two rows: its name and its levels */
#ifndef CHAOSTGENTRANCE_ROWS_H
#define CHAOSTGENTRANCE_ROWS_H
#include <sg_xpt.h>
struct task;

/* a list's rows stand ROW_HEIGHT apart, and each enters ROW_DELAY frames
 * after the one above */
#define ROW_HEIGHT 40.0F
#define ROW_DELAY 4
/* a row's slide out of sight, where it starts and where slide.h's
 * ROW_SLIDE_OUT stops it. a row that starts at ROW_FAR_OUT arrives later */
#define ROW_OUT (-380.0F)
#define ROW_FAR_OUT (-410.0F)
/* cells of the common sheet. NAME_CELLS begins the three of a name's row,
 * and LIT_NAME_CELLS the three that light it */
#define NAME_CELLS 66
#define LIT_NAME_CELLS 82
/* the cell after a name's first is its second only where it is narrower
 * than SECOND_CELL_WIDTH */
#define SECOND_CELL_WIDTH 128

/* a row's work, which slide.h's macros move */
struct entrance_row {
    Sint8 mode, phase;
    Uint8 kind, age;
    Sint16 flags;
    /* the row waits delay + 1 frames to enter, and delay + 1 to leave */
    Uint16 delay;
    /* the first cell of the course's name, on the language's sheet */
    Sint32 label;
    Uint16 unk_0c, course;
    Uint16 column, row; /* the menu's column and cursor that select it */
    Float x, y;
    Float slide; /* the offset from x, which entering brings to 0 */
    Float alpha; /* the alpha of the white the name draws in */
    Uint8 unk_24[12];
    Sint32 cell; /* a heading's cell on the common sheet */
    /* set once a name's row has shown its message for the cursor on it. the
     * row arrives with has_message set, since the screen opens with the first
     * course's message */
    Bool has_message;
};

/* each makes the beginner or jewel screen's names, or their levels, as
 * children of parent. each makes a row for every course the screen offers,
 * and counts its rows in entrance_menu->rows. the jewel screen offers its
 * last two courses once each of its first four has a won level */
void entrance_jewel_names(struct task *parent);
void entrance_beginner_names(struct task *parent);
void entrance_jewel_levels(struct task *parent);
void entrance_beginner_levels(struct task *parent);
/* the heading above the names, and those above the beginner and jewel
 * levels */
void entrance_course_heading(struct task *parent);
void entrance_beginner_heading(struct task *parent);
void entrance_jewel_heading(struct task *parent);
#endif
