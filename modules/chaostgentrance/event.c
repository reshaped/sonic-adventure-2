/* the group menu's buttons: one for each race group it offers, then the
 * exit. a tag at each button's left end follows the button on a spring of its
 * own */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chao/race.h"
#include "chao/race_state.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"

enum {
    EVENT_INIT,
    EVENT_DELAY,
    EVENT_SLIDE_IN,
    EVENT_SPRING,
    EVENT_WAIT,
    EVENT_ACTIVE,
    EVENT_EXIT_DELAY,
    EVENT_SLIDE_OUT,
    EVENT_FINISHED
};
/* the exit button's group, past every race group */
#define EVENT_EXIT 7
/* set once the button has shown its message for the cursor on it. a button
 * starts with it set, since creating it sets the bar to the screen's first
 * message */
#define EVENT_HAS_MESSAGE 0x100

/* a button waits at OFF_LEFT, ROW_STAGGER frames longer a row down. it
 * then slides in by SLIDE_SPEED a frame, to stop STOP_STEP further right a
 * row down */
#define FIRST_DELAY 30
#define ROW_STAGGER 10
#define OFF_LEFT (-10.0F)
#define SLIDE_SPEED 17.0F
#define STOP_X 376.0F
#define STOP_STEP 16.0F
#define TOP_Y 120.0F
#define BUTTON_HEIGHT 40.0F
#define EXIT_Y 390.0F
/* a button's width springs to 1 until its age passes SPRING_FRAMES. a
 * select or a back sends it out after half its delay less EXIT_LEAD */
#define WIDTH_KICK (-0.15F)
#define SPRING_FRAMES 60
#define WIDTH_SPRING 0.15F
#define WIDTH_DAMPING 0.86F
#define EXIT_LEAD 20
/* the tag springs toward a point TAG_LEFT left of the button and TAG_UP
 * above it. while the cursor is on the button, the point is LIT_TAG_LEFT
 * left and LIT_TAG_UP above. the button's scale on each axis scales each */
#define START_TAG_LEFT 266.0F
#define START_TAG_UP 8.0F
#define TAG_LEFT 262.0F
#define TAG_UP 7.0F
#define LIT_TAG_LEFT 266.0F
#define LIT_TAG_UP 9.0F
#define TAG_SPRING 0.37F
#define TAG_DAMPING 0.89F
/* while the cursor is on the button, the exit icon tilts and pulses, and
 * the tag glows. each turns back to 0 at SETTLE_RATE */
#define TILT_RATE 0x400
#define PULSE_RATE 0x600
#define GLOW_RATE 0x800
#define SETTLE_RATE 0x800
#define GLOW_GROWTH 0.2F
#define GLOW_ALPHA 0.7F
#define GLOW_SWING 0.3F
#define EXIT_ICON_LEFT 40.0F
#define TILT_ANGLE (5 * ANGLE_PER_DEGREE)
#define ICON_SCALE 0.88F
#define ICON_PULSE 0.1F

/* cells of the common sheet. LIT_CELLS and FRAME_CELLS each begin a run of
 * three */
#define TAG_LIT_CELL 16
#define LIT_CELLS 17
#define TAG_CELL 20
#define FRAME_CELLS 21
#define EXIT_ICON 135
#define GLOW_CELL 150

struct event_button {
    Sint8 mode, phase;
    Uint8 id, delay;
    Sint16 flags;
    Uint16 age;
    Sint32 row, label, group;
    Float x, y, width, stop, width_speed;
    Uint8 unk_28[8];
};

struct button_tag {
    Angle tilt, pulse;
    Float x, y, vx, vy;
    Angle glow;
};

/* the race groups, in the order the menu lists them */
/* clang-format off */
static Sint32 event_order[RACE_GROUPS + 1] = {
    RACE_BEGINNER, RACE_JEWEL, RACE_CHALLENGE, RACE_DARK_COURSES,
    RACE_HERO_COURSES, RACE_MULTI, -1};
/* clang-format on */

#define EVENT_CELL(cell, depth) ENTRANCE_DRAW_CELL(sprite, (cell), (depth))
#define DRAW_EVENT_FRAME()                                                     \
    EVENT_CELL(FRAME_CELLS, -11.5F);                                           \
    EVENT_CELL(FRAME_CELLS + 1, -11.5F);                                       \
    EVENT_CELL(FRAME_CELLS + 2, -11.5F)

static void event_button_draw(struct task *task)
{
    struct event_button *button = (struct event_button *)task->work;
    struct button_tag *tag = task->extra.p;
    NJS_SPRITE sprite;
    Float scale;
    int cell;

    sprite.p.x = button->x;
    sprite.p.y = button->y;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = button->width;
    sprite.sy = 2.0F - button->width;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (game_state.text_language == TEXT_JAPANESE) {
        sprite.tlist = &entrance_japanese_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_japanese_cells;
    } else {
        sprite.tlist = &entrance_english_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_english_cells;
    }
    cell = button->label;
    EVENT_CELL(cell, -10.0F);
    cell++;
    EVENT_CELL(cell, -10.0F);
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    if (button->mode >= EVENT_ACTIVE && entrance_menu->cursor == button->row) {
        RGB(23, 171, 234);
        DRAW_EVENT_FRAME();
        EVENT_CELL(LIT_CELLS, -11.6F);
        EVENT_CELL(LIT_CELLS + 1, -11.6F);
        EVENT_CELL(LIT_CELLS + 2, -11.6F);
        sprite.p.x = tag->x;
        sprite.p.y = tag->y;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        EVENT_CELL(TAG_CELL, -11.5F);
        EVENT_CELL(TAG_LIT_CELL, -11.6F);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        cell = GLOW_CELL;
        sprite.sx = sprite.sy = 1.0F + (njSin(tag->glow) * GLOW_GROWTH);
        constant_material_set(GLOW_ALPHA + (njSin(tag->glow) * GLOW_SWING),
                              23.0F / 255.0F, 171.0F / 255.0F, 234.0F / 255.0F);
        EVENT_CELL(cell, -11.7F);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    } else {
        if (button->group == EVENT_EXIT) {
            RGB(248, 201, 58);
        } else {
            RGB(239, 124, 112);
        }
        DRAW_EVENT_FRAME();
        sprite.p.x = tag->x;
        sprite.p.y = tag->y;
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        EVENT_CELL(TAG_CELL, -11.5F);
    }
    if (button->group == EVENT_EXIT) {
        cell = EXIT_ICON;
        sprite.p.x = button->x - (button->width * EXIT_ICON_LEFT);
        sprite.p.y = button->y;
        sprite.ang = (1.0F - njCos(tag->tilt)) * TILT_ANGLE;
        scale = ICON_SCALE + (njCos(tag->pulse) * ICON_PULSE);
        sprite.sx *= scale;
        sprite.sy *= scale;
        EVENT_CELL(cell, -11.4F);
    }
}

static void event_button_destroy(struct task *task)
{
}

#define BUTTON_MODE(next)                                                      \
    {                                                                          \
        button->mode = (next);                                                 \
        button->phase = 0;                                                     \
    }
static void event_button_step(struct task *task)
{
    struct event_button *button = (struct event_button *)task->work;
    struct button_tag *tag = task->extra.p;
    switch (button->mode) {
    case EVENT_INIT:
        tag->x = button->x - START_TAG_LEFT;
        tag->y = button->y - START_TAG_UP;
        BUTTON_MODE(EVENT_DELAY);
        break;
    case EVENT_DELAY:
        if (button->age++ > button->delay) {
            BUTTON_MODE(EVENT_SLIDE_IN);
            button->age = 0;
        }
        break;
    case EVENT_SLIDE_IN:
        button->x += SLIDE_SPEED;
        if (button->x > button->stop) {
            button->x = button->stop;
            button->width_speed = WIDTH_KICK;
            button->age = 0;
            BUTTON_MODE(EVENT_SPRING);
        }
        break;
    case EVENT_SPRING:
        button->width += button->width_speed;
        button->width_speed -= (button->width - 1.0F) * WIDTH_SPRING;
        button->width_speed *= WIDTH_DAMPING;
        if (button->age++ > SPRING_FRAMES) {
            button->width = 1.0F;
            BUTTON_MODE(EVENT_WAIT);
            button->age = 0;
        }
        break;
    case EVENT_WAIT:
        if (entrance_menu->task->work->mode >= MENU_GROUP_INPUT) {
            BUTTON_MODE(EVENT_ACTIVE);
        }
        break;
    case EVENT_ACTIVE:
        if (entrance_menu->cursor == button->row) {
            if (!(button->flags & EVENT_HAS_MESSAGE)) {
                if (button->group == EVENT_EXIT) {
                    if (game_state.chao_race_menu == CHAO_RACE_MENU_NONE) {
                        entrance_message_set(GARDEN_EXIT_MESSAGE);
                        entrance_message_append(GROUPS_MESSAGE);
                    } else {
                        entrance_message_set(GROUPS_MESSAGE);
                    }
                } else {
                    entrance_message_set(GROUP_MESSAGE(button->group));
                    entrance_message_append(GROUPS_MESSAGE);
                }
                button->flags |= EVENT_HAS_MESSAGE;
            }
            if (player_pads[0]->press & PDD_DGT_TA) {
                entrance_message_set(-1);
            }
        } else {
            button->flags &= ~EVENT_HAS_MESSAGE;
        }
        break;
    case EVENT_EXIT_DELAY:
        if (button->age++ > (button->delay / 2) - EXIT_LEAD) {
            BUTTON_MODE(EVENT_SLIDE_OUT);
        }
        break;
    case EVENT_SLIDE_OUT:
        button->x -= SLIDE_SPEED;
        if (button->x < OFF_LEFT) {
            BUTTON_MODE(EVENT_FINISHED);
        }
        break;
    case EVENT_FINISHED:
        task_kill(task);
        break;
    }
    if (button->mode >= EVENT_ACTIVE && entrance_menu->cursor == button->row) {
        tag->tilt += TILT_RATE;
        tag->pulse += PULSE_RATE;
        tag->pulse &= ANGLE_MASK;
        tag->glow += GLOW_RATE;
        tag->vx +=
            (button->x - (button->width * LIT_TAG_LEFT) - tag->x) * TAG_SPRING;
        tag->vy +=
            (button->y - ((2.0F - button->width) * LIT_TAG_UP) - tag->y) *
            TAG_SPRING;
    } else {
        tag->tilt = angle_approach(tag->tilt, 0, SETTLE_RATE);
        tag->pulse = angle_approach(tag->pulse, 0, SETTLE_RATE);
        tag->glow = angle_approach(tag->glow, 0, SETTLE_RATE);
        tag->vx +=
            (button->x - (button->width * TAG_LEFT) - tag->x) * TAG_SPRING;
        tag->vy += (button->y - ((2.0F - button->width) * TAG_UP) - tag->y) *
                   TAG_SPRING;
    }
    tag->vx *= TAG_DAMPING;
    tag->vy *= TAG_DAMPING;
    tag->x += tag->vx;
    tag->y += tag->vy;
    switch (button->mode) {
    case EVENT_WAIT:
    case EVENT_ACTIVE:
    case EVENT_FINISHED:
        break;
    default:
        entrance_menu->wait = TRUE;
        break;
    }
    if (button->mode < EVENT_EXIT_DELAY) {
        switch (entrance_menu->request) {
        case ENTRANCE_MENU_SELECT:
            if (entrance_menu->cursor == button->row) {
                garden_race_set_group(button->group);
            }
            /* fall through */
        case ENTRANCE_MENU_BACK:
            BUTTON_MODE(EVENT_EXIT_DELAY);
            break;
        }
    }
}

#define EVENT_LABEL(group_id, index)                                           \
    case (group_id):                                                           \
        button->label = entrance_label_ids[(index) + GROUP_LABELS];            \
        break
static void event_button_create(struct task *parent, int group, int row)
{
    struct task *task;
    struct event_button *button;
    task = task_create_child(TASK_WORK, event_button_step, parent);
    task->draw = event_button_draw;
    task->destructor = event_button_destroy;
    task->extra.p = syCalloc(1, sizeof(struct button_tag));
    button = (struct event_button *)task->work;
    switch (group) {
        EVENT_LABEL(RACE_BEGINNER, 0);
        EVENT_LABEL(RACE_JEWEL, 1);
        EVENT_LABEL(RACE_CHALLENGE, 2);
        EVENT_LABEL(RACE_DARK_COURSES, 3);
        EVENT_LABEL(RACE_HERO_COURSES, 4);
        EVENT_LABEL(RACE_MULTI, 5);
        EVENT_LABEL(EVENT_EXIT, 6);
    }
    button->row = row;
    button->delay = (row * ROW_STAGGER) + FIRST_DELAY;
    button->group = group;
    button->x = OFF_LEFT;
    button->stop = STOP_X + (row * STOP_STEP);
    button->y = TOP_Y + (row * BUTTON_HEIGHT);
    if (group == EVENT_EXIT) {
        button->y = EXIT_Y;
    }
    button->flags |= EVENT_HAS_MESSAGE;
    button->width = 1.0F;
    entrance_message_set(GROUPS_MESSAGE);
    if (game_state.chao_race_menu == CHAO_RACE_MENU_VERSUS) {
        entrance_message_append(GROUP_MESSAGE(RACE_MULTI));
    } else {
        entrance_message_append(GROUP_MESSAGE(RACE_BEGINNER));
    }
}

/* @hack spelling: a macro, expanded at each use, where a function would do */
#define CREATE_OPEN_GROUP()                                                    \
    /* @hack spelling: NEW before OPEN, where enum order would do */           \
    switch (garden_race_group_status(group)) {                                 \
    case RACE_GROUP_HIDDEN:                                                    \
        break;                                                                 \
    case RACE_GROUP_NEW:                                                       \
    case RACE_GROUP_OPEN:                                                      \
    case RACE_GROUP_VISITED:                                                   \
        event_button_create(parent, event_order[group], row);                  \
        row++;                                                                 \
        break;                                                                 \
    }

void entrance_event_buttons(struct task *parent)
{
    int group;
    int row = 0;
    switch (game_state.chao_race_menu) {
    case CHAO_RACE_MENU_NONE:
        for (group = RACE_BEGINNER; group <= RACE_MULTI; group++) {
            CREATE_OPEN_GROUP();
        }
        break;
    case CHAO_RACE_MENU_VERSUS:
        event_button_create(parent, RACE_MULTI, row);
        row++;
        break;
    case CHAO_RACE_MENU_EMBLEMS:
        for (group = RACE_BEGINNER; group <= RACE_HERO_COURSES; group++) {
            CREATE_OPEN_GROUP();
        }
        break;
    }
    event_button_create(parent, EVENT_EXIT, row);
    entrance_menu->cursor = 0;
}
