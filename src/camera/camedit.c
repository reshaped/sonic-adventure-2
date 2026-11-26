/* the camera editor's fields: stepping a value by its digits, printing a
 * page's rows, and the BASE page */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "camera/camedit.h"
#include "camera/editor.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/pad.h"

/* a digit's step changes its value only while camedit_player's pad holds
 * one of FACE_BUTTONS */
#define FACE_BUTTONS (PDD_DGT_TB | PDD_DGT_TA | PDD_DGT_TY | PDD_DGT_TX)

/* the highest digit camedit_step_decimal and camedit_step_hex step, where
 * the ones are digit 0 */
#define DECIMAL_TOP_DIGIT 5
#define HEX_TOP_DIGIT 3

/* the color a print sets back after printing in YELLOW or GREEN */
#define WHITE 0xffffffff

/* the BASE page's rows, named for the field each steps. camedit_labels
 * names them MODE, LEVEL, COLLI, ADJUST and ENTRY in row order */
#define ROW_MODE 0
#define ROW_ENTRY 1
#define ROW_SHAPE 2
#define ROW_ADJUST 3
#define ROW_SHARED 4

/* the scl.z a shape that becomes a box takes */
#define BOX_DEPTH 10.0F

/* the editor's cursor: its row, or NO_ROW, and the digit a step changes.
 * the ones are digit 0, and a float's digits below 0 lie after the point.
 * no code writes row, and the steps only clamp digit, so both stay 0 */
#define NO_ROW (-1)
struct camedit_cursor {
    Sint32 row;
    Sint32 unk_04;
    Sint32 digit;
};

/* a page of the editor: the name camedit_print_page prints, and unk_04,
 * which no code reads. where a page's rows are known, unk_04 is their count.
 * it is 5 for BASE, and 3 for each page of rows X, Y and Z. it is 8 for the
 * two OTHER pages */
#define PAGES 9
struct camedit_page {
    const char *name;
    Sint32 unk_04;
};

static struct camedit_page camedit_pages[PAGES] = {
    {"BASE", 5},   {"COLPOS", 3},        {"COLANG", 3},
    {"COLSCL", 3}, {"CAMPOS", 3},        {"CAMANG", 3},
    {"TGTPOS", 3}, {"OTHER INTEGER", 8}, {"OTHER FLOAT", 8},
};

Sint32 camedit_player;
static struct camedit_cursor camedit_cursor;
/* a word of bss that no code in the game's images refers to. it lies between
 * camedit_cursor and camset.c's camset_last_put, and nothing shows which of
 * the two objects defines it */
static Sint32 dat_8c268bcc;

static void camedit_step_decimal(Sint32 *value, Sint32 min, Sint32 max);
static void camedit_print_edited_float(int x, int y, const char *label,
                                       Float value);
static void camedit_print_edited_hex(int x, int y, const char *label,
                                     Sint32 value);
static void camedit_print_edited_decimal(int x, int y, const char *label,
                                         Sint32 value);

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void camedit_step_float(Float *value, Float min, Float max);
void camedit_step_hex(Sint32 *value, Sint32 min, Sint32 max);
void camedit_print_page(int page);
/* print a page's rows from screen row 4, the cursor's row as an edited row.
 * camedit_print_angles prints ang in hexadecimal and camedit_print_point
 * prints point, each as rows X, Y and Z. camedit_print_floats and
 * camedit_print_decimals print count values, in rows numbered from 0 */
void camedit_print_angles(struct angles *ang);
void camedit_print_floats(Float *values, int count);
void camedit_print_decimals(Sint32 *values, int count);
void camedit_print_point(NJS_POINT3 *point);
void camedit_edit_base(void);
void camedit_apply_shape(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* while camedit_player's pad holds one of FACE_BUTTONS, step *value by 10
 * to the power of the cursor's digit. the digit is first clamped between 0
 * and DECIMAL_TOP_DIGIT. up adds the step, and down takes it away. *value
 * is then clamped between min and max */
static void camedit_step_decimal(Sint32 *value, Sint32 min, Sint32 max)
{
    if (player_pads[camedit_player]->on & FACE_BUTTONS) {
        if (camedit_cursor.digit < 0) {
            camedit_cursor.digit = 0;
        } else if (camedit_cursor.digit > DECIMAL_TOP_DIGIT) {
            camedit_cursor.digit = DECIMAL_TOP_DIGIT;
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KU) {
            *value += int_power(10, camedit_cursor.digit);
        } else if (pad_press_repeat[camedit_player] & PDD_DGT_KD) {
            *value -= int_power(10, camedit_cursor.digit);
        }
        if (*value < min) {
            *value = min;
        } else if (*value > max) {
            *value = max;
        }
    }
}

/* camedit_step_decimal for a float, by 10 to the power of any digit */
void camedit_step_float(Float *value, Float min, Float max)
{
    if (player_pads[camedit_player]->on & FACE_BUTTONS) {
        if (pad_press_repeat[camedit_player] & PDD_DGT_KU) {
            *value += powf(10.0F, camedit_cursor.digit);
        } else if (pad_press_repeat[camedit_player] & PDD_DGT_KD) {
            *value -= powf(10.0F, camedit_cursor.digit);
        }
        if (*value < min) {
            *value = min;
        } else if (*value > max) {
            *value = max;
        }
    }
}

/* camedit_step_decimal in hexadecimal, with HEX_TOP_DIGIT the top digit.
 * *value is cut to its low 16 bits before it is clamped */
void camedit_step_hex(Sint32 *value, Sint32 min, Sint32 max)
{
    if (player_pads[camedit_player]->on & FACE_BUTTONS) {
        if (camedit_cursor.digit < 0) {
            camedit_cursor.digit = 0;
        } else if (camedit_cursor.digit > HEX_TOP_DIGIT) {
            camedit_cursor.digit = HEX_TOP_DIGIT;
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KU) {
            *value += int_power(16, camedit_cursor.digit);
        } else if (pad_press_repeat[camedit_player] & PDD_DGT_KD) {
            *value -= int_power(16, camedit_cursor.digit);
        }
        *value = (Uint16)*value;
        if (*value < min) {
            *value = min;
        } else if (*value > max) {
            *value = max;
        }
    }
}

void camedit_print_page(int page)
{
    if (camedit_cursor.row == NO_ROW) {
        njPrintColor(YELLOW);
    }
    njPrintC(NJM_LOCATION(3, 3), camedit_pages[page].name);
    njPrintColor(WHITE);
}

/* print the row the cursor is on in yellow: label at column x of row y, then
 * value. the cursor's digit then prints again over its place, in green. a
 * negative value starts a column further left, for its sign */
static void camedit_print_edited_float(int x, int y, const char *label,
                                       Float value)
{
    Float step;
    int digit;

    njPrintColor(YELLOW);
    njPrintC(NJM_LOCATION(x, y), label);
    if (value >= 0.0F) {
        njPrintF(NJM_LOCATION(x + 4, y), value, 6);
    } else {
        njPrintF(NJM_LOCATION(x + 3, y), value, 6);
    }
    njPrintColor(GREEN);
    step = powf(10.0F, camedit_cursor.digit);
    /* @hack spelling: the digit worked out by statements of its own, and
     * kept in value as a float, where digit = (int)(value / step) % 10, with
     * digit printed, would do */
    digit = value / step;
    digit %= 10;
    value = digit;
    if (camedit_cursor.digit >= 0) {
        njPrintD(NJM_LOCATION(x - camedit_cursor.digit + 9, y), njAbs(value),
                 1);
    } else {
        njPrintD(NJM_LOCATION(x - camedit_cursor.digit + 10, y), njAbs(value),
                 1);
    }
    njPrintColor(WHITE);
}

/* camedit_print_edited_float for a hexadecimal value, which prints as 8
 * digits in one column whatever its sign */
static void camedit_print_edited_hex(int x, int y, const char *label,
                                     Sint32 value)
{
    Float step;
    int digit;

    njPrintColor(YELLOW);
    njPrintC(NJM_LOCATION(x, y), label);
    njPrintH(NJM_LOCATION(x + 8, y), value, 8);
    njPrintColor(GREEN);
    step = powf(16.0F, camedit_cursor.digit);
    /* @hack spelling: the digit worked out by statements of its own, where
     * digit = (int)(value / step) % 16 would do */
    digit = value / step;
    digit %= 16;
    /* @bug logic: the green digit is a digit of value's magnitude (FABS at
     * 0x8c0ad5ca), where the row prints value's two's complement (the JSR
     * at 0x8c0ad580). for a negative value the two disagree: for -16 at
     * digit 1, the row shows F and the green digit 1. only
     * camedit_print_angles calls this function, and no code in the game's
     * images calls camedit_print_angles */
    njPrintH(NJM_LOCATION(x - camedit_cursor.digit + 15, y), njAbs(digit), 1);
    njPrintColor(WHITE);
}

/* camedit_print_edited_float for a decimal value */
static void camedit_print_edited_decimal(int x, int y, const char *label,
                                         Sint32 value)
{
    Float step;

    njPrintColor(YELLOW);
    njPrintC(NJM_LOCATION(x, y), label);
    if (value >= 0) {
        njPrintD(NJM_LOCATION(x + 8, y), value, 5);
    } else {
        njPrintD(NJM_LOCATION(x + 7, y), value, 5);
    }
    njPrintColor(GREEN);
    step = powf(10.0F, camedit_cursor.digit);
    /* @hack spelling: the digit worked out in value itself, by statements of
     * its own, where value = (int)(value / step) % 10 would do */
    value /= step;
    value %= 10;
    njPrintD(NJM_LOCATION(x - camedit_cursor.digit + 12, y), njAbs(value), 1);
    njPrintColor(WHITE);
}

void camedit_print_angles(struct angles *ang)
{
    if (camedit_cursor.row == 0) {
        camedit_print_edited_hex(3, 4, "X:", ang->x);
    } else {
        njPrintC(NJM_LOCATION(3, 4), "X:");
        /* @hack type: the angle compared with 0.0F, as a float, here and in
         * the two tests below, not with 0 */
        /* @bug logic: prints a negative angle a column left of a positive
         * one, here and in the two tests below (FCMP/GT at 0x8c0ad730,
         * 0x8c0ad76a and 0x8c0ad7aa), though njPrintH writes no sign. the
         * cursor's row prints any angle at column 11 (ADD #8,R4 at
         * 0x8c0ad576), so a negative angle's digits move as the cursor
         * reaches its row. no code in the game's images calls
         * camedit_print_angles */
        if (ang->x >= 0.0F) {
            njPrintH(NJM_LOCATION(11, 4), ang->x, 8);
        } else {
            njPrintH(NJM_LOCATION(10, 4), ang->x, 8);
        }
    }
    if (camedit_cursor.row == 1) {
        camedit_print_edited_hex(3, 5, "Y:", ang->y);
    } else {
        njPrintC(NJM_LOCATION(3, 5), "Y:");
        if (ang->y >= 0.0F) {
            njPrintH(NJM_LOCATION(11, 5), ang->y, 8);
        } else {
            njPrintH(NJM_LOCATION(10, 5), ang->y, 8);
        }
    }
    if (camedit_cursor.row == 2) {
        camedit_print_edited_hex(3, 6, "Z:", ang->z);
    } else {
        njPrintC(NJM_LOCATION(3, 6), "Z:");
        if (ang->z >= 0.0F) {
            njPrintH(NJM_LOCATION(11, 6), ang->z, 8);
        } else {
            njPrintH(NJM_LOCATION(10, 6), ang->z, 8);
        }
    }
}

void camedit_print_floats(Float *values, int count)
{
    char label[16];
    int i;

    for (i = 0; i < count; i++) {
        sprintf(label, "%d:", i);
        if (camedit_cursor.row == i) {
            camedit_print_edited_float(3, i + 4, label, values[i]);
        } else {
            njPrintC(NJM_LOCATION(3, i + 4), label);
            if (values[i] >= 0.0F) {
                njPrintF(NJM_LOCATION(7, i + 4), values[i], 6);
            } else {
                njPrintF(NJM_LOCATION(6, i + 4), values[i], 6);
            }
        }
    }
}

void camedit_print_decimals(Sint32 *values, int count)
{
    char label[16];
    int i;

    for (i = 0; i < count; i++) {
        sprintf(label, "%d:", i);
        if (camedit_cursor.row == i) {
            camedit_print_edited_decimal(3, i + 4, label, values[i]);
        } else {
            njPrintC(NJM_LOCATION(3, i + 4), label);
            /* @hack type: the value compared with 0.0F, as a float, not with
             * 0 */
            if (values[i] >= 0.0F) {
                njPrintD(NJM_LOCATION(11, i + 4), values[i], 5);
            } else {
                njPrintD(NJM_LOCATION(10, i + 4), values[i], 5);
            }
        }
    }
}

void camedit_print_point(NJS_POINT3 *point)
{
    if (camedit_cursor.row == 0) {
        camedit_print_edited_float(3, 4, "X:", point->x);
    } else {
        njPrintC(NJM_LOCATION(3, 4), "X:");
        if (point->x >= 0.0F) {
            njPrintF(NJM_LOCATION(7, 4), point->x, 6);
        } else {
            njPrintF(NJM_LOCATION(6, 4), point->x, 6);
        }
    }
    if (camedit_cursor.row == 1) {
        camedit_print_edited_float(3, 5, "Y:", point->y);
    } else {
        njPrintC(NJM_LOCATION(3, 5), "Y:");
        if (point->y >= 0.0F) {
            njPrintF(NJM_LOCATION(7, 5), point->y, 6);
        } else {
            njPrintF(NJM_LOCATION(6, 5), point->y, 6);
        }
    }
    if (camedit_cursor.row == 2) {
        camedit_print_edited_float(3, 6, "Z:", point->z);
    } else {
        njPrintC(NJM_LOCATION(3, 6), "Z:");
        if (point->z >= 0.0F) {
            njPrintF(NJM_LOCATION(7, 6), point->z, 6);
        } else {
            njPrintF(NJM_LOCATION(6, 6), point->z, 6);
        }
    }
}

/* camedit_speed_names holds the camlin editor's speeds as its draw names
 * them. camedit_labels holds the BASE page's rows in row order, then COMMON,
 * PART 1P and PART 2P. no code reads either */
static const char camedit_speed_names[4][12] = {
    "NORMAL SPD",
    "HIGH SPD",
    "SUPER SPD",
    "MADNESS SPD",
};
static const char camedit_labels[8][8] = {
    "MODE", "LEVEL", "COLLI", "ADJUST", "ENTRY", "COMMON", "PART 1P", "PART 2P",
};

/* the BASE page's step: the pad steps the field the cursor's row names. that
 * is a member of the held zone, or camera_editor.shared for ROW_SHARED */
void camedit_edit_base(void)
{
    if (!(player_pads[camedit_player]->on & PAD_DIRECTIONS)) {
        return;
    }
    switch (camedit_cursor.row) {
    case ROW_MODE:
        if (pad_press_repeat[camedit_player] & PDD_DGT_KR) {
            camera_editor.held_zone.mode++;
            if (camera_editor.held_zone.mode >= CAMERA_MODES) {
                camera_editor.held_zone.mode = CAMERA_MODE_FOLLOW;
            }
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KL) {
            camera_editor.held_zone.mode--;
            if (camera_editor.held_zone.mode <= CAMERA_MODE_USER) {
                camera_editor.held_zone.mode = CAMERA_MODES - 1;
            }
        }
        break;
    case ROW_ENTRY:
        if (camedit_cursor.digit < 0) {
            camedit_cursor.digit = 0;
        }
        camedit_step_decimal(&camera_editor.held_zone.entry, 0,
                             CAMERA_STACK_DEPTH - 1);
        break;
    case ROW_SHAPE:
        if (pad_press_repeat[camedit_player] & PDD_DGT_KR) {
            camera_editor.held_zone.shape++;
            /* @hack spelling: the test as shape >= ZONE_BOX + 1, where shape
             * > ZONE_BOX would do */
            if (camera_editor.held_zone.shape >= ZONE_BOX + 1) {
                camera_editor.held_zone.shape = ZONE_SPHERE;
            }
            if (camera_editor.held_zone.shape == ZONE_BOX) {
                camera_editor.held_zone.scl.z = BOX_DEPTH;
            }
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KL) {
            camera_editor.held_zone.shape--;
            if (camera_editor.held_zone.shape < ZONE_SPHERE) {
                camera_editor.held_zone.shape = ZONE_BOX;
            }
            if (camera_editor.held_zone.shape == ZONE_BOX) {
                camera_editor.held_zone.scl.z = BOX_DEPTH;
            }
        }
        break;
    case ROW_ADJUST:
        if (pad_press_repeat[camedit_player] & PDD_DGT_KR) {
            camera_editor.held_zone.adjust++;
            if (camera_editor.held_zone.adjust >= CAMERA_ADJUSTS) {
                camera_editor.held_zone.adjust = CAMERA_ADJUST_NONE;
            }
            if (camera_editor.held_zone.adjust == CAMERA_ADJUST_USER) {
                camera_editor.held_zone.adjust++;
            }
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KL) {
            camera_editor.held_zone.adjust--;
            if (camera_editor.held_zone.adjust < CAMERA_ADJUST_NONE) {
                camera_editor.held_zone.adjust = CAMERA_ADJUSTS - 1;
            }
            if (camera_editor.held_zone.adjust == CAMERA_ADJUST_USER) {
                camera_editor.held_zone.adjust = CAMERA_ADJUST_NONE;
            }
        }
        break;
    case ROW_SHARED:
        if (pad_press_repeat[camedit_player] & PDD_DGT_KL) {
            camera_editor.shared = 1;
        }
        if (pad_press_repeat[camedit_player] & PDD_DGT_KR) {
            camera_editor.shared = 0;
        }
        break;
    }
}

/* set the held zone's fields that its shape fixes. a sphere keeps its
 * radius, scl.x, squared in scl.z, as its tests read it */
void camedit_apply_shape(void)
{
    switch (camera_editor.held_zone.shape) {
    case ZONE_SPHERE:
        camera_editor.held_zone.scl.y = camera_editor.held_zone.scl.x;
        camera_editor.held_zone.scl.z =
            camera_editor.held_zone.scl.x * camera_editor.held_zone.scl.x;
        camera_editor.held_zone.ang.x = 0;
        camera_editor.held_zone.ang.y = 0;
        camera_editor.held_zone.ang.z = 0;
        break;
    case ZONE_PLANE:
        camera_editor.held_zone.scl.z = 0.0F;
        camera_editor.held_zone.entry = 0;
        break;
    /* @hack redundant: a case for a box that does nothing */
    case ZONE_BOX:
        break;
    }
    camera_editor.held_zone.camera_ang.x = 0;
    camera_editor.held_zone.camera_ang.y = 0;
}

void camedit_update(void)
{
}

void camedit_init(void)
{
    char *byte;
    int i;

    camedit_player = CAMEDIT_NO_PLAYER;
    /* @hack spelling: the record cleared a byte at a time, down a count,
     * where memset(&camera_editor, 0, sizeof(camera_editor)) would do */
    byte = (char *)&camera_editor;
    for (i = sizeof(camera_editor); i != 0; i--) {
        *byte++ = 0;
    }
}

void camedit_draw(void)
{
}
