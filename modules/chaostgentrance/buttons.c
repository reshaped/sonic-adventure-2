#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/race.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"

enum button_mode { BUTTON_DELAY, BUTTON_SPRING, BUTTON_LEAVE };
/* set once the button has shown its message for the cursor on it */
#define BUTTON_HAS_MESSAGE 0x100

/* a button's state, laid over its task's work */
struct entrance_button {
    Sint8 mode;
    Sint8 item; /* the cursor's row that selects the button */
    Uint8 id;
    Uint8 label; /* the label's first cell on the language's sheet */
    Sint16 flags;
    Uint16 delay; /* the frames left before the button springs in */
    Sint32 icon; /* the icon's cell on the common sheet */
    Angle pulse, swing;
    Float x, y;
    /* a scale that squeezes the height as it grows. it springs to 1, and a
     * leaving button widens until it passes LEAVE_WIDTH */
    Float width;
    Float unk_20, width_speed, unk_28;
    struct collision_work *collision;
};

#define PULSE_RATE 0x500
#define SWING_RATE 0x800
#define START_WIDTH 0.3F
#define SPRING 0.5F
#define DAMPING 0.85F
#define LEAVE_GROWTH 0.14F
#define LEAVE_WIDTH 1.8F
/* under the cursor, the button's scale pulses PULSE_SIZE either side of
 * PULSE_MEAN. its icon swings SWING_ANGLE either side of upright */
#define PULSE_MEAN 1.05F
#define PULSE_SIZE 0.05F
#define SWING_ANGLE (20 * ANGLE_PER_DEGREE)
/* the offsets from the button's point to its frame, its icon and its
 * label. the button's scale scales each */
#define FRAME_LEFT 64.0F
#define FRAME_UP 12.0F
#define ICON_LEFT 34.0F
#define ICON_DOWN 4.0F
#define LABEL_LEFT 20.0F
#define LABEL_UP 9.0F

/* cells of the common sheet. the button under the cursor draws its lit
 * frame and LIT_BACK_CELL, and any other button DIM_CELL. the picker's lit
 * frame is PICKER_LIT_CELL, and another screen's LIT_CELL */
#define LIT_BACK_CELL 85
#define DIM_CELL 86
#define LIT_CELL 87
#define PICKER_LIT_CELL 88
#define BACK_ICON 93
#define CONFIRM_ICON 94

/* the frames each button waits before it springs in */
#define BACK_DELAY 120
#define CONFIRM_DELAY 110

static void button_step(struct task *task)
{
    struct entrance_button *button = (struct entrance_button *)task->work;
    const PDS_PERIPHERAL *pad;
    Sint32 request;

    switch (button->mode) {
    case BUTTON_DELAY:
        if (button->delay > 0) {
            button->delay--;
        } else {
            button->mode = BUTTON_SPRING;
        }
        entrance_menu->wait = TRUE;
        break;
    case BUTTON_SPRING:
        button->width += button->width_speed;
        button->width_speed += (1.0F - button->width) * SPRING;
        button->width_speed *= DAMPING;
        if (entrance_menu->cursor == button->item) {
            button->swing += SWING_RATE;
            button->pulse += PULSE_RATE;
            if (!(button->flags & BUTTON_HAS_MESSAGE)) {
                if (button->label == entrance_label_ids[CONFIRM_LABEL]) {
                    entrance_message_set(CONFIRM_MESSAGE);
                } else if (entrance_menu->task->work->mode >=
                           MENU_BUILD_COURSES) {
                    entrance_message_set(COURSE_BACK_MESSAGE);
                } else {
                    entrance_message_set(PICKER_BACK_MESSAGE);
                }
                button->flags |= BUTTON_HAS_MESSAGE;
            } else {
                /* @hack redundant: a test of player 0's A and B, which does
                 * nothing */
                if (((pad = player_pads[0])->press & PDD_DGT_TA) ||
                    (pad->press & PDD_DGT_TB)) {
                }
            }
        } else {
            if (button->flags & BUTTON_HAS_MESSAGE) {
                if (entrance_menu->task->work->mode >= MENU_BUILD_COURSES) {
                    if (garden_race_group() >= RACE_CHALLENGE) {
                        entrance_message_set(COURSES_MESSAGE);
                    } else {
                        entrance_message_set(LEVELS_MESSAGE);
                    }
                } else if (garden_race_group() != RACE_MULTI) {
                    entrance_message_set(PICK_MESSAGE);
                } else {
                    entrance_message_set(MULTI_PICK_MESSAGE);
                    entrance_message_append(MULTI_PICK_MESSAGE + 1);
                }
                button->flags &= ~BUTTON_HAS_MESSAGE;
            }
            button->swing = angle_approach(button->swing, 0, SWING_RATE);
            button->pulse = angle_approach(button->pulse, 0, PULSE_RATE);
        }
        break;
    case BUTTON_LEAVE:
        button->width += LEAVE_GROWTH;
        entrance_menu->wait = TRUE;
        if (button->width > LEAVE_WIDTH) {
            task_kill(task);
        }
        break;
    }
    if (button->mode < BUTTON_LEAVE) {
        request = entrance_menu->request;
        if (request == ENTRANCE_MENU_SELECT || request == ENTRANCE_MENU_BACK) {
            button->mode = BUTTON_LEAVE;
        }
    }
}

static void button_draw(struct task *task)
{
    struct entrance_button *button = (struct entrance_button *)task->work;
    NJS_SPRITE sprite;
    Float pulse;
    Float sx;
    Float sy;
    Int cell;

    if (button->delay > 0) {
        return;
    }
    pulse = PULSE_MEAN - (njCos(button->pulse) * PULSE_SIZE);
    sx = button->width;
    sy = 2.0F - sx;
    sx *= pulse;
    sy *= pulse;
    sprite.p.z = 0;
    sprite.ang = 0;
    sprite.sx = sx;
    sprite.sy = sy;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    sprite.p.x = button->x - (sx * FRAME_LEFT);
    sprite.p.y = button->y - (sy * FRAME_UP);
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    if (entrance_menu->cursor == button->item) {
        if (entrance_menu->screen == ENTRANCE_SELECT_CHAO) {
            cell = PICKER_LIT_CELL;
        } else {
            cell = LIT_CELL;
        }
        ENTRANCE_DRAW_CELL(sprite, cell, -11.0F);
        ENTRANCE_DRAW_CELL(sprite, LIT_BACK_CELL, -11.1F);
    } else {
        ENTRANCE_DRAW_CELL(sprite, DIM_CELL, -11.0F);
    }
    sprite.p.x = button->x - (sx * ICON_LEFT);
    sprite.p.y = button->y + (sy * ICON_DOWN);
    sprite.ang = njSin(button->swing) * SWING_ANGLE;
    cell = button->icon;
    ENTRANCE_DRAW_CELL(sprite, cell, -10.0F);
    if (game_state.text_language == TEXT_JAPANESE) {
        sprite.tlist = &entrance_japanese_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_japanese_cells;
    } else {
        sprite.tlist = &entrance_english_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_english_cells;
    }
    sprite.p.x = button->x - (sx * LABEL_LEFT);
    sprite.p.y = button->y - (sy * LABEL_UP);
    cell = button->label;
    ENTRANCE_DRAW_CELL(sprite, cell, -10.0F);
}

static void back_button_destroy(struct task *task)
{
    entrance_menu->back_button = NULL;
}

static void confirm_button_destroy(struct task *task)
{
    entrance_menu->confirm_button = NULL;
}

/* @hack spelling: a macro that makes the button in place, where a function
 * returning it for the caller to store would do */
#define CREATE_BUTTON(parent_task, px, py, row, member, destroy, label_index,  \
                      icon_cell, frames)                                       \
    do {                                                                       \
        struct task *child =                                                   \
            task_create_child(TASK_WORK, button_step, (parent_task));          \
        struct entrance_button *button;                                        \
        child->draw = button_draw;                                             \
        child->destructor = (destroy);                                         \
        button = (struct entrance_button *)child->work;                        \
        button->label = entrance_label_ids[(label_index)];                     \
        button->icon = (icon_cell);                                            \
        button->item = (row);                                                  \
        button->delay = (frames);                                              \
        button->x = (px);                                                      \
        button->y = (py);                                                      \
        button->pulse = 0;                                                     \
        button->width = START_WIDTH;                                           \
        entrance_menu->member = child;                                         \
    } while (0)

void entrance_back_button(struct task *parent, Float x, Float y, int item)
{
    if (!entrance_menu->back_button) {
        CREATE_BUTTON(parent, x, y, item, back_button, back_button_destroy,
                      BACK_LABEL, BACK_ICON, BACK_DELAY);
    }
}

void entrance_confirm_button(struct task *parent, Float x, Float y, int item)
{
    if (!entrance_menu->confirm_button) {
        CREATE_BUTTON(parent, x, y, item, confirm_button,
                      confirm_button_destroy, CONFIRM_LABEL, CONFIRM_ICON,
                      CONFIRM_DELAY);
    }
}
