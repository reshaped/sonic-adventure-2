/* the Chao picker. a page shows a row of entrance_roster, entry i in slot
 * i. slot i stands in column i / SLOT_ROWS and row i % SLOT_ROWS. on the
 * memory cards' page, a column is a connector and its rows the connector's
 * two sockets */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include "chao/garden_save.h"
#include "chao/race_record.h"
#include "draw/lines.h"
#include "chao/garden.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/race.h"
#include "chao/residents.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chaostgentrance/fixture_art.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/roster.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"

/* a slot of the page shown. it shows its entry's Chao, and on the memory
 * cards' page its drive's model too */
struct picker_slot {
    Sint8 mode, phase;
    Uint8 unk_02, age;
    /* the slot waits delay + 1 frames to enter, and delay + 1 to leave */
    Uint16 unk_04, delay;
    /* @hack type: column and row unsigned, where Sint32 would do */
    Uint32 column, row;
    Sint32 index; /* its entry in the page's row of entrance_roster */
    Float x, y, unk_1c, scale;
    Uint8 unk_24[12];
};

/* a panel beside the tabs, for L, or for R where right is set. it pulses
 * while its button is held */
struct picker_panel {
    Sint8 mode, phase;
    Uint8 unk_02, age;
    Uint16 unk_04, delay;
    Uint8 unk_08[8];
    Bool right;
    Float x, y, width, alpha, scale;
    Uint8 unk_28[8];
};

/* a page's tab. label is the first of its name's two cells. list is its
 * page's place among the pages offered, from 0 */
struct page_tab {
    Sint8 mode, phase;
    Uint8 unk_02, age;
    Uint16 unk_04, delay;
    Sint32 label;
    Uint16 place, list;
    Uint8 unk_10[4];
    Float x, y, unk_1c, alpha;
    /* the tab's scale on x, which multiplies scale. it is 1 for the shown
     * page's tab, and SIDE_TAB_WIDTH for a side tab */
    Float width;
    Float scale;
    Uint8 unk_2c[4];
};

enum {
    SLOT_WAIT,
    SLOT_ENTER,
    SLOT_IDLE,
    SLOT_HIGHLIGHT,
    SLOT_SELECTED,
    SLOT_EXIT_DELAY,
    SLOT_LEAVE,
    SLOT_DONE
};
enum {
    PANEL_WAIT,
    PANEL_ENTER,
    PANEL_OPEN,
    PANEL_IDLE,
    PANEL_UNHIDE,
    PANEL_HIDE,
    PANEL_PULSE,
    PANEL_STRETCH,
    PANEL_SETTLE,
    PANEL_EXIT_DELAY,
    PANEL_LEAVE,
    PANEL_DONE
};
enum {
    TAB_WAIT,
    TAB_ENTER,
    TAB_IDLE,
    TAB_SHRINK,
    TAB_APPEAR,
    TAB_DELAY,
    TAB_GROW,
    TAB_FROM_LEFT,
    TAB_TO_RIGHT,
    TAB_FROM_RIGHT,
    TAB_TO_LEFT,
    TAB_EXIT_DELAY,
    TAB_LEAVE,
    TAB_DONE
};
/* a tab's place. the shown page's tab is at the center, and its neighbors'
 * at either side. a fourth page's tab is hidden */
enum { TAB_LEFT, TAB_CENTER, TAB_RIGHT, TAB_HIDDEN };

/* the slots stand SLOT_WIDTH and SLOT_HEIGHT apart, from SLOT_LEFT,
 * SLOT_TOP. the cursor stands left of its slot, at CURSOR_X in column 0 */
#define SLOT_LEFT 166.0F
#define SLOT_TOP 174.0F
#define SLOT_WIDTH 104.0F
#define SLOT_HEIGHT 96.0F
#define CURSOR_X 115.0F
/* a slot's delay is SLOT_DELAY, SLOT_STAGGER more a column on. the top row
 * enters from the left, and the bottom row from the right */
#define SLOT_DELAY 60
#define SLOT_STAGGER 10
/* a slot grows and shrinks by SLOT_GROWTH a frame. its Chao comes once its
 * scale passes CHAO_SCALE, and goes once its scale falls below it. the Chao
 * of the first TILTED_SLOTS slots pitch by SLOT_TILT */
#define SLOT_GROWTH 0.08F
#define CHAO_SCALE 0.5F
#define TILTED_SLOTS 4
#define SLOT_TILT 0x400
/* the frames a slot's Chao blends into its next motion */
#define MOTION_BLEND 10
/* a slot's x less ORIGIN_X, times WORLD_PER_PIXEL, is its Chao's x in the
 * world */
#define ORIGIN_X 320.0F
/* a drive's model stands as a slot's Chao would, by scales and offsets of
 * its own. it draws at HIGHLIGHT_SCALE in the slot under the cursor, and at
 * CARD_SCALE in any other */
#define HIGHLIGHT_SCALE 1.3F
#define CARD_SCALE 1.05F
static Float card_x_scale = 0.064F;
static Float card_y_scale = 0.065F;
static Float card_height = 12.5F;
static Float card_depth = -62.0F;

#define TAB_Y 328.0F
#define LEFT_TAB_X 88.0F
#define CENTER_TAB_X 320.0F
#define RIGHT_TAB_X 552.0F
#define SIDE_TAB_WIDTH 0.6F
/* a tab slides TAB_SPEED pixels a frame. its width changes by WIDEN_STEP a
 * frame, its scale by TAB_GROWTH and its alpha by TAB_FADE */
#define TAB_SPEED 10.0F
#define WIDEN_STEP 0.025F
#define TAB_GROWTH 0.1F
#define TAB_FADE 0.07F
#define APPEAR_DELAY 5
#define LEFT_TAB_DELAY 90
#define CENTER_TAB_DELAY 100
#define RIGHT_TAB_DELAY 110
#define LEFT_PANEL_X 204.0F
#define RIGHT_PANEL_X 436.0F
/* a panel opens from START_PANEL_WIDTH to PANEL_WIDTH by WIDTH_STEP a
 * frame. a pulse grows its scale to PULSE_SCALE by PULSE_GROWTH a frame. it
 * then widens to STRETCHED_WIDTH while its scale falls by PANEL_SHRINKAGE a
 * frame */
#define START_PANEL_WIDTH 24.0F
#define PANEL_WIDTH 38.0F
#define STRETCHED_WIDTH 50.0F
#define WIDTH_STEP 2.0F
#define PULSE_SCALE 1.5F
#define PULSE_GROWTH 0.1F
#define PANEL_SHRINKAGE 0.05F
#define PANEL_FADE 0.07F
#define HIDE_FADE 0.05F
#define PANEL_DELAY 120
/* a panel's middle cell stands EDGE_WIDTH in from the panel's point. it
 * stretches to the panel's width less EDGES_WIDTH */
#define EDGE_WIDTH 10.0F
#define EDGES_WIDTH 20.0F
/* the cursor waits CURSOR_DELAY + 1 frames to show. it pulses CURSOR_PULSE
 * either side of its scale, CURSOR_RATE a frame */
#define CURSOR_DELAY 90
#define CURSOR_PULSE 0.15F
#define CURSOR_RATE 0x800
#define CURSOR_ANGLE (HALF_TURN + QUARTER_TURN)
#define CURSOR_PHASE(cursor) ((cursor)->ang.x)
/* the cursor and the panels wait EXIT_DELAY + 1 frames to leave */
#define EXIT_DELAY 60
/* the buttons stand at BUTTON_X. multiplayer's confirm stands at CONFIRM_Y
 * and its back at MULTI_BACK_Y. another group's back stands at BACK_Y */
#define BUTTON_X 320.0F
#define CONFIRM_Y 367.0F
#define MULTI_BACK_Y 395.0F
#define BACK_Y 383.0F

/* cells of the common sheet. a panel draws the three from LEFT_PANEL_CELLS
 * or RIGHT_PANEL_CELLS. a tab draws the three from TAB_CELLS, and the shown
 * page's tab the three from CENTER_TAB_CELLS too */
#define LEFT_PANEL_CELLS 10
#define RIGHT_PANEL_CELLS 13
#define CENTER_TAB_CELLS 76
#define TAB_CELLS 79
#define SLOT_CELL 133
#define CURSOR_CELL 154

#define PICK_MODE(widget, next)                                                \
    {                                                                          \
        (widget)->mode = (next);                                               \
        (widget)->phase = 0;                                                   \
    }
#define PICK_EXIT_REQUEST()                                                    \
    (entrance_menu->request == ENTRANCE_MENU_SELECT ||                         \
     entrance_menu->request == ENTRANCE_MENU_BACK ||                           \
     entrance_menu->request == ENTRANCE_MENU_VMS_CHANGED)
#define SLOT_CHAO(slot) entrance_menu->preview[(slot)->index]
#define SLOT_CHOICE(slot)                                                      \
    entrance_menu->choices[entrance_menu->page][(slot)->index].selected
#define REMOVE_SLOT_CHAO(slot)                                                 \
    if (SLOT_CHAO(slot)) {                                                     \
        task_kill(SLOT_CHAO(slot));                                            \
        SLOT_CHAO(slot) = NULL;                                                \
    }
#define PICK_CELL(cell, depth) ENTRANCE_DRAW_CELL(sprite, (cell), (depth))
#define PICK_BLEND()                                                           \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA)

static void picker_slot_step(struct task *task)
{
    struct picker_slot *slot = (struct picker_slot *)task->work;
    struct task *chao = SLOT_CHAO(slot);
    struct task *preview;
    struct chao_save *save;
    NJS_POINT3 position;
    switch (slot->mode) {
    case SLOT_WAIT:
        if (slot->age++ > slot->delay) {
            slot->age = 0;
            PICK_MODE(slot, SLOT_ENTER);
        }
        break;
    case SLOT_ENTER:
        if (slot->scale < 1.0F) {
            slot->scale += SLOT_GROWTH;
        } else {
            slot->scale = 1.0F;
        }
        if (slot->scale > CHAO_SCALE) {
            if (entrance_roster_ready) {
                PICK_MODE(slot, SLOT_IDLE);
                if (!chao) {
                    save = entrance_roster[entrance_menu->page][slot->index];
                    if (save) {
                        if (save->record.head[0].type > CHAO_EGG) {
                            position.x = (slot->x - ORIGIN_X) * WORLD_PER_PIXEL;
                            position.y =
                                PREVIEW_HEIGHT - ((slot->y - PREVIEW_ORIGIN_Y) *
                                                  WORLD_PER_PIXEL);
                            position.z = PREVIEW_Z;
                            preview = garden_spawn_chao(
                                entrance_roster[entrance_menu->page]
                                               [slot->index],
                                &position, 0);
                            SLOT_CHAO(slot) = preview;
#ifdef NONMATCHING
                            /* this side breaks where garden_spawn_chao makes no
                             * preview. the matching side freezes the null */
                            if (!preview) {
                                break;
                            }
#endif
                            FREEZE_CHAO(preview);
                            if (slot->index < TILTED_SLOTS) {
                                preview->work->ang.x = SLOT_TILT;
                            }
                            chao_link_motion_frames(
                                preview, CHAO_ANIM_GENKI_SUWARI, MOTION_BLEND);
                        }
                    }
                }
            }
        }
        break;
    case SLOT_IDLE:
        if (SLOT_CHOICE(slot)) {
            if (chao) {
                chao_link_motion_frames(chao, CHAO_ANIM_BANZAI_JUMP,
                                        MOTION_BLEND);
            }
            PICK_MODE(slot, SLOT_SELECTED);
        }
        if (entrance_menu->cursor == (Sint32)slot->row &&
            entrance_menu->column == (Sint32)slot->column) {
            if (chao) {
                chao_link_motion_frames(chao, CHAO_ANIM_HAND_DANCE_UP,
                                        MOTION_BLEND);
                if (chao->child) {
                    task_destroy_children(chao);
                }
                garden_create_name_label(chao);
            }
            PICK_MODE(slot, SLOT_HIGHLIGHT);
        }
        break;
    case SLOT_HIGHLIGHT:
        if (SLOT_CHOICE(slot)) {
            if (chao) {
                chao_link_motion_frames(chao, CHAO_ANIM_BANZAI_JUMP,
                                        MOTION_BLEND);
            }
            PICK_MODE(slot, SLOT_SELECTED);
        }
        if (entrance_menu->cursor != (Sint32)slot->row ||
            entrance_menu->column != (Sint32)slot->column) {
            if (chao) {
                if (chao->child) {
                    task_destroy_children(chao);
                }
                chao_link_motion_frames(chao, CHAO_ANIM_GENKI_SUWARI,
                                        MOTION_BLEND);
            }
            PICK_MODE(slot, SLOT_IDLE);
        }
        break;
    case SLOT_SELECTED:
        if (!SLOT_CHOICE(slot)) {
            if (chao) {
                chao_link_motion_frames(chao, CHAO_ANIM_GENKI_SUWARI,
                                        MOTION_BLEND);
            }
            PICK_MODE(slot, SLOT_IDLE);
        }
        break;
    case SLOT_EXIT_DELAY:
        if (slot->age++ > slot->delay) {
            PICK_MODE(slot, SLOT_LEAVE);
        }
        break;
    case SLOT_LEAVE:
        if (slot->scale > 0.0F) {
            slot->scale -= SLOT_GROWTH;
        } else {
            PICK_MODE(slot, SLOT_DONE);
        }
        if (slot->scale < CHAO_SCALE) {
            REMOVE_SLOT_CHAO(slot);
        }
        break;
    case SLOT_DONE:
        task_kill(task);
        break;
    }
    switch (slot->mode) {
    case SLOT_IDLE:
    case SLOT_HIGHLIGHT:
    case SLOT_SELECTED:
        /* L and R may turn the page, so the slot enters again with the
         * page's Chao */
        if (player_pads[0]->press & PDD_DGT_TL ||
            player_pads[0]->press & PDD_DGT_TR) {
            REMOVE_SLOT_CHAO(slot);
            PICK_MODE(slot, SLOT_ENTER);
        }
        break;
    default:
        entrance_menu->wait = TRUE;
        break;
    }
    if (slot->mode < SLOT_EXIT_DELAY) {
        if (PICK_EXIT_REQUEST()) {
            PICK_MODE(slot, SLOT_EXIT_DELAY);
        }
    }
}

static void picker_slot_draw(struct task *task)
{
    struct picker_slot *slot = (struct picker_slot *)task->work;
    NJS_SPRITE sprite;
    NJS_POINT3 position;
    Float scale;
    sprite.p.x = slot->x;
    sprite.p.y = slot->y;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    PICK_BLEND();
    if (SLOT_CHOICE(slot)) {
        TINT(slot->scale, 181, 228, 114);
    } else {
        TINT(slot->scale, 102, 102, 102);
    }
    PICK_CELL(SLOT_CELL, -55.5F);
    if (slot->scale > CHAO_SCALE && entrance_menu->page == ENTRANCE_CARD_ROW) {
        position.x = (slot->x - ORIGIN_X) * card_x_scale;
        position.y =
            card_height - ((slot->y - PREVIEW_ORIGIN_Y) * card_y_scale);
        position.z = card_depth;
        CLEAR_COLOR();
        njSetTexture(&entrance_vms_texlist);
        njPushMatrixEx();
        njTranslateEx(&position);
        if (slot->mode == SLOT_HIGHLIGHT) {
            scale = HIGHLIGHT_SCALE;
        } else {
            scale = CARD_SCALE;
        }
        njScale(NULL, scale, scale, scale);
        switch (entrance_drive_kind[slot->index]) {
        case ENTRANCE_DRIVE_CARD:
            njCnkEasyDrawObject(&entrance_vms_model);
            break;
        case ENTRANCE_DRIVE_MULTI_BANK:
            njCnkEasyDrawObject(&entrance_multibank_model);
            break;
        }
        njPopMatrixEx();
    }
}

static void picker_slot_destroy(struct task *task)
{
    (void)task;
}
static void picker_slot_create(struct task *parent, int index)
{
    struct task *task;
    struct picker_slot *slot;
    task = task_create_child(0, picker_slot_step, parent);
    task->draw = picker_slot_draw;
    task->destructor = picker_slot_destroy;
    task->work = syCalloc(1, sizeof(*slot));
    slot = (struct picker_slot *)task->work;
    slot->index = index;
    slot->column = index / SLOT_ROWS;
    slot->row = index % SLOT_ROWS;
    if (slot->row == 0) {
        slot->delay = (slot->column * SLOT_STAGGER) + SLOT_DELAY;
    } else {
        slot->delay =
            ((SLOT_COLUMNS - 1 - slot->column) * SLOT_STAGGER) + SLOT_DELAY;
    }
    slot->x = (slot->column * SLOT_WIDTH) + SLOT_LEFT;
    slot->y = (slot->row * SLOT_HEIGHT) + SLOT_TOP;
}

static void picker_cursor_draw(struct task *task)
{
    struct task_work *cursor = task->work;
    NJS_SPRITE sprite;
    Float scale;
    /* @hack spelling: > SLOT_ROWS - 1, where >= SLOT_ROWS would do */
    if (entrance_menu->cursor > SLOT_ROWS - 1) {
        return;
    }
    scale = 1.0F + (njSin(CURSOR_PHASE(cursor)) * CURSOR_PULSE);
    sprite.p.x = (entrance_menu->column * SLOT_WIDTH) + CURSOR_X;
    sprite.p.y = (entrance_menu->cursor * SLOT_HEIGHT) + SLOT_TOP;
    sprite.p.z = 0.0F;
    sprite.ang = CURSOR_ANGLE;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.tlist = &chao_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    PICK_BLEND();
    PICK_CELL(CURSOR_CELL, -11.5F);
}
enum {
    CURSOR_WAIT,
    CURSOR_ACTIVE,
    CURSOR_EXIT_DELAY,
    CURSOR_LEAVE,
    CURSOR_DONE
};
#define CURSOR_MODE(next)                                                      \
    {                                                                          \
        cursor->mode = (next);                                                 \
        cursor->submode = 0;                                                   \
    }
static void picker_cursor_step(struct task *task)
{
    struct task_work *cursor = task->work;
    switch (cursor->mode) {
    case CURSOR_WAIT:
        if (cursor->timer++ > CURSOR_DELAY) {
            task->draw = picker_cursor_draw;
            cursor->timer = 0;
            CURSOR_MODE(CURSOR_ACTIVE);
        }
        break;
    case CURSOR_ACTIVE:
        CURSOR_PHASE(cursor) += CURSOR_RATE;
        break;
    case CURSOR_EXIT_DELAY:
        if (cursor->timer++ > EXIT_DELAY) {
            CURSOR_MODE(CURSOR_LEAVE);
        }
        break;
    case CURSOR_LEAVE:
        CURSOR_MODE(CURSOR_DONE);
        break;
    case CURSOR_DONE:
        task_kill(task);
        break;
    }
    switch (cursor->mode) {
    case CURSOR_ACTIVE:
        break;
    default:
        entrance_menu->wait = TRUE;
        break;
    }
    if (cursor->mode < CURSOR_EXIT_DELAY) {
        if (PICK_EXIT_REQUEST()) {
            CURSOR_MODE(CURSOR_EXIT_DELAY);
        }
    }
}
static void picker_cursor_destroy(struct task *task)
{
    (void)task;
}
/* @hack redundant: picker_cursor_create's own copy, beside the one SHC
 * inlines into entrance_chao_picker, where a static function would do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void picker_cursor_create(struct task *parent)
{
    struct task *task =
        task_create_child(TASK_WORK, picker_cursor_step, parent);
    task->destructor = picker_cursor_destroy;
}

static void picker_panel_draw(struct task *task)
{
    struct picker_panel *panel = (struct picker_panel *)task->work;
    NJS_SPRITE sprite;
    if (panel->mode <= PANEL_WAIT) {
        return;
    }
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = panel->scale;
    sprite.sy = panel->scale;
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    PICK_BLEND();
    constant_material_set(panel->alpha, 1.0F, 1.0F, 1.0F);
    if (!panel->right) {
        sprite.p.x = panel->x;
        sprite.p.y = panel->y;
        PICK_CELL(LEFT_PANEL_CELLS + 2, -10.0F);
        sprite.p.x = panel->x - panel->width;
        PICK_CELL(LEFT_PANEL_CELLS, -10.0F);
        sprite.p.x = panel->x - EDGE_WIDTH;
        sprite.sx = panel->width - EDGES_WIDTH;
        PICK_CELL(LEFT_PANEL_CELLS + 1, -11.0F);
    } else {
        sprite.p.x = panel->x;
        sprite.p.y = panel->y;
        PICK_CELL(RIGHT_PANEL_CELLS + 2, -10.0F);
        sprite.p.x = panel->x + panel->width;
        PICK_CELL(RIGHT_PANEL_CELLS, -10.0F);
        sprite.p.x = panel->x + EDGE_WIDTH;
        sprite.sx = panel->width - EDGES_WIDTH;
        PICK_CELL(RIGHT_PANEL_CELLS + 1, -11.0F);
    }
}

/* where the pages don't wrap, a panel hides on the page at its end */
#define HIDE_END_PANEL(right_end)                                              \
    if (entrance_menu->pages <= CHAO_GARDENS) {                                \
        if (!panel->right) {                                                   \
            if (entrance_menu->page == ENTRANCE_CARD_ROW) {                    \
                PICK_MODE(panel, PANEL_HIDE);                                  \
            }                                                                  \
        } else if (right_end) {                                                \
            PICK_MODE(panel, PANEL_HIDE);                                      \
        }                                                                      \
    }
#define PANEL_SHRINK()                                                         \
    if (panel->scale > 1.0F) {                                                 \
        panel->scale -= PANEL_SHRINKAGE;                                       \
    } else {                                                                   \
        panel->scale = 1.0F;                                                   \
    }

static void picker_panel_step(struct task *task)
{
    struct picker_panel *panel = (struct picker_panel *)task->work;
    switch (panel->mode) {
    case PANEL_WAIT:
        if (panel->age++ > panel->delay) {
            panel->age = 0;
            PICK_MODE(panel, PANEL_ENTER);
        }
        break;
    case PANEL_ENTER:
        if (panel->alpha < 1.0F) {
            panel->alpha += PANEL_FADE;
        } else {
            panel->alpha = 1.0F;
            PICK_MODE(panel, PANEL_OPEN);
        }
        break;
    case PANEL_OPEN:
        if (panel->width < PANEL_WIDTH) {
            panel->width += WIDTH_STEP;
        } else {
            panel->width = PANEL_WIDTH;
            PICK_MODE(panel, PANEL_IDLE);
            HIDE_END_PANEL(entrance_menu->page == entrance_menu->pages - 1);
        }
        break;
    case PANEL_IDLE:
        break;
    case PANEL_HIDE:
        if (panel->alpha > 0.0F) {
            panel->alpha -= HIDE_FADE;
        } else {
            panel->alpha = 0.0F;
        }
        if (!panel->right) {
            if (entrance_menu->page != ENTRANCE_CARD_ROW) {
                PICK_MODE(panel, PANEL_UNHIDE);
            }
        } else if (entrance_menu->page < entrance_menu->pages - 1) {
            PICK_MODE(panel, PANEL_UNHIDE);
        }
        break;
    case PANEL_UNHIDE:
        if (panel->alpha < 1.0F) {
            panel->alpha += HIDE_FADE;
        } else {
            panel->alpha = 1.0F;
            PICK_MODE(panel, PANEL_IDLE);
        }
        break;
    case PANEL_PULSE:
        if (panel->scale < PULSE_SCALE) {
            panel->scale += PULSE_GROWTH;
        } else {
            panel->scale = PULSE_SCALE;
            PICK_MODE(panel, PANEL_STRETCH);
        }
        break;
    case PANEL_STRETCH:
        PANEL_SHRINK();
        if (panel->width < STRETCHED_WIDTH) {
            panel->width += WIDTH_STEP;
        } else {
            PICK_MODE(panel, PANEL_SETTLE);
        }
        break;
    case PANEL_SETTLE:
        PANEL_SHRINK();
        if (panel->width > PANEL_WIDTH) {
            panel->width -= 1.0F;
        } else {
            panel->width = PANEL_WIDTH;
            PICK_MODE(panel, PANEL_IDLE);
            HIDE_END_PANEL(entrance_menu->page >= entrance_menu->pages - 1);
        }
        break;
    case PANEL_EXIT_DELAY:
        if (panel->age++ > EXIT_DELAY) {
            PICK_MODE(panel, PANEL_LEAVE);
        }
        break;
    case PANEL_LEAVE:
        if (panel->alpha > 0.0F) {
            panel->alpha -= HIDE_FADE;
        } else {
            panel->alpha = 0.0F;
            PICK_MODE(panel, PANEL_DONE);
        }
        break;
    case PANEL_DONE:
        task_kill(task);
        break;
    }
    switch (panel->mode) {
    case PANEL_IDLE:
        if (!panel->right) {
            if (player_pads[0]->on & PDD_DGT_TL) {
                PICK_MODE(panel, PANEL_PULSE);
            }
        } else if (player_pads[0]->on & PDD_DGT_TR) {
            PICK_MODE(panel, PANEL_PULSE);
        }
        break;
    }
    if (panel->mode < PANEL_EXIT_DELAY) {
        if (PICK_EXIT_REQUEST()) {
            PICK_MODE(panel, PANEL_EXIT_DELAY);
        }
    }
}
static void picker_panel_destroy(struct task *task)
{
    (void)task;
}
static void picker_panel_create(struct task *parent, Float x, Float y,
                                Bool right)
{
    struct task *task;
    struct picker_panel *panel;
    task = task_create_child(0, picker_panel_step, parent);
    task->draw = picker_panel_draw;
    task->destructor = picker_panel_destroy;
    task->work = syCalloc(1, sizeof(*panel));
    panel = (struct picker_panel *)task->work;
    panel->right = right;
    panel->x = x;
    panel->y = y;
    panel->width = START_PANEL_WIDTH;
    panel->scale = 1.0F;
    panel->alpha = 0.0F;
    panel->delay = PANEL_DELAY;
}

#define TURN_TAB(from, to, animation)                                          \
    case (from):                                                               \
        tab->place = (to);                                                     \
        PICK_MODE(tab, animation);                                             \
        break
#define WIDEN_TAB()                                                            \
    if (tab->width < 1.0F) {                                                   \
        tab->width += WIDEN_STEP;                                              \
    } else {                                                                   \
        tab->width = 1.0F;                                                     \
    }
#define NARROW_TAB()                                                           \
    if (tab->width > SIDE_TAB_WIDTH) {                                         \
        tab->width -= WIDEN_STEP;                                              \
    } else {                                                                   \
        tab->width = SIDE_TAB_WIDTH;                                           \
    }
#define SLIDE_TAB(condition, advance, stop)                                    \
    if (condition) {                                                           \
        advance;                                                               \
    } else {                                                                   \
        tab->x = (stop);                                                       \
        PICK_MODE(tab, TAB_IDLE);                                              \
    }
static void page_tab_step(struct task *task)
{
    struct page_tab *tab = (struct page_tab *)task->work;
    switch (tab->mode) {
    case TAB_WAIT:
        if (tab->age++ > tab->delay) {
            tab->age = 0;
            PICK_MODE(tab, TAB_ENTER);
        }
        break;
    case TAB_ENTER:
        if (tab->alpha < 1.0F) {
            tab->alpha += TAB_FADE;
        } else {
            tab->alpha = 1.0F;
            PICK_MODE(tab, TAB_IDLE);
        }
        break;
    case TAB_IDLE:
        if (entrance_menu->turn == ENTRANCE_TURN_NEXT) {
            switch (tab->place) {
                TURN_TAB(TAB_LEFT, TAB_HIDDEN, TAB_SHRINK);
                TURN_TAB(TAB_CENTER, TAB_LEFT, TAB_TO_LEFT);
                TURN_TAB(TAB_RIGHT, TAB_CENTER, TAB_FROM_RIGHT);
                TURN_TAB(TAB_HIDDEN, TAB_RIGHT, TAB_APPEAR);
            }
            entrance_menu->wait = TRUE;
        } else if (entrance_menu->turn == ENTRANCE_TURN_PREVIOUS) {
            switch (tab->place) {
                TURN_TAB(TAB_LEFT, TAB_CENTER, TAB_FROM_LEFT);
                TURN_TAB(TAB_CENTER, TAB_RIGHT, TAB_TO_RIGHT);
                TURN_TAB(TAB_RIGHT, TAB_HIDDEN, TAB_SHRINK);
                TURN_TAB(TAB_HIDDEN, TAB_LEFT, TAB_APPEAR);
            }
            entrance_menu->wait = TRUE;
        }
        break;
    case TAB_SHRINK:
        if (tab->scale > 0.0F) {
            tab->scale -= TAB_GROWTH;
        } else {
            tab->scale = 0.0F;
            PICK_MODE(tab, TAB_IDLE);
        }
        break;
    case TAB_APPEAR:
        if (tab->place == TAB_LEFT) {
            tab->x = LEFT_TAB_X;
        } else {
            tab->x = RIGHT_TAB_X;
        }
        tab->age = 0;
        PICK_MODE(tab, TAB_DELAY);
        /* fall through */
    case TAB_DELAY:
        if (tab->age++ > APPEAR_DELAY) {
            PICK_MODE(tab, TAB_GROW);
        }
        break;
    case TAB_GROW:
        if (tab->scale < 1.0F) {
            tab->scale += TAB_GROWTH;
        } else {
            tab->scale = 1.0F;
            PICK_MODE(tab, TAB_IDLE);
        }
        break;
    case TAB_FROM_LEFT:
        WIDEN_TAB();
        SLIDE_TAB(tab->x < CENTER_TAB_X, tab->x += TAB_SPEED, CENTER_TAB_X);
        break;
    case TAB_TO_RIGHT:
        NARROW_TAB();
        SLIDE_TAB(tab->x < RIGHT_TAB_X, tab->x += TAB_SPEED, RIGHT_TAB_X);
        break;
    case TAB_FROM_RIGHT:
        WIDEN_TAB();
        SLIDE_TAB(tab->x > CENTER_TAB_X, tab->x -= TAB_SPEED, CENTER_TAB_X);
        break;
    case TAB_TO_LEFT:
        NARROW_TAB();
        SLIDE_TAB(tab->x > LEFT_TAB_X, tab->x -= TAB_SPEED, LEFT_TAB_X);
        break;
    case TAB_EXIT_DELAY:
        if (tab->age++ > tab->delay) {
            tab->age = 0;
            PICK_MODE(tab, TAB_LEAVE);
        }
        break;
    case TAB_LEAVE:
        if (tab->alpha > 0.0F) {
            tab->alpha -= TAB_FADE;
        } else {
            tab->alpha = 0.0F;
            PICK_MODE(tab, TAB_DONE);
        }
        break;
    case TAB_DONE:
        task_kill(task);
        break;
    }
    switch (tab->mode) {
    case TAB_IDLE:
        break;
    default:
        entrance_menu->wait = TRUE;
        break;
    }
    if (tab->mode < TAB_EXIT_DELAY) {
        if (PICK_EXIT_REQUEST()) {
            tab->age = 0;
            PICK_MODE(tab, TAB_EXIT_DELAY);
        }
    }
}
static void page_tab_draw(struct task *task)
{
    struct page_tab *tab = (struct page_tab *)task->work;
    NJS_SPRITE sprite;
    /* @hack spelling: the label's cells through cell, where tab->label would
     * do */
    Int cell;
    sprite.p.x = tab->x;
    sprite.p.y = tab->y;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = tab->width * tab->scale;
    sprite.sy = tab->scale;
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    PICK_BLEND();
    if (tab->place == TAB_CENTER) {
        TINT(tab->alpha, 255, 255, 255);
        PICK_CELL(CENTER_TAB_CELLS, -31.6F);
        PICK_CELL(CENTER_TAB_CELLS + 1, -31.6F);
        PICK_CELL(CENTER_TAB_CELLS + 2, -31.6F);
        TINT(tab->alpha, 181, 228, 114);
    } else {
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            BEGINNER_COLOR(tab->alpha);
            break;
        case RACE_JEWEL:
            JEWEL_COLOR(tab->alpha);
            break;
        case RACE_CHALLENGE:
            CHALLENGE_COLOR(tab->alpha);
            break;
        case RACE_DARK_COURSES:
            DARK_COURSES_COLOR(tab->alpha);
            break;
        case RACE_HERO_COURSES:
            HERO_COURSES_COLOR(tab->alpha);
            break;
        case RACE_MULTI:
            MULTI_COLOR(tab->alpha);
            break;
        }
    }
    PICK_CELL(TAB_CELLS, -31.5F);
    PICK_CELL(TAB_CELLS + 1, -31.5F);
    PICK_CELL(TAB_CELLS + 2, -31.5F);
    TINT(tab->alpha, 255, 255, 255);
    if (game_state.text_language == TEXT_JAPANESE) {
        sprite.tlist = &entrance_japanese_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_japanese_cells;
    } else {
        sprite.tlist = &entrance_english_chars;
        sprite.tanim = (NJS_TEXANIM *)entrance_english_cells;
    }
    cell = tab->label;
    PICK_CELL(cell, -31.4F);
    cell = tab->label + 1;
    PICK_CELL(cell, -31.4F);
}
static void page_tab_destroy(struct task *task)
{
    (void)task;
}

#define PLACE_TAB(which, tab_x, tab_width, tab_scale, opacity, frames)         \
    tab->x = (tab_x);                                                          \
    tab->y = TAB_Y;                                                            \
    tab->width = (tab_width);                                                  \
    tab->scale = (tab_scale);                                                  \
    tab->alpha = (opacity);                                                    \
    tab->place = (which);                                                      \
    tab->delay = (frames)
#define PLACE_LEFT()                                                           \
    PLACE_TAB(TAB_LEFT, LEFT_TAB_X, SIDE_TAB_WIDTH, 1.0F, 0.0F, LEFT_TAB_DELAY)
#define PLACE_CENTER()                                                         \
    PLACE_TAB(TAB_CENTER, CENTER_TAB_X, 1.0F, 1.0F, 0.0F, CENTER_TAB_DELAY)
#define PLACE_RIGHT()                                                          \
    PLACE_TAB(TAB_RIGHT, RIGHT_TAB_X, SIDE_TAB_WIDTH, 1.0F, 0.0F,              \
              RIGHT_TAB_DELAY)
#define PLACE_HIDDEN()                                                         \
    PLACE_TAB(TAB_HIDDEN, 0.0F, SIDE_TAB_WIDTH, 0.0F, 1.0F, LEFT_TAB_DELAY)

static void page_tabs_create(struct task *parent)
{
    struct task *task;
    struct page_tab *tab;
    int page;
    entrance_menu->pages = 0;
    for (page = 0; page < CHAO_GARDENS + 1; page++) {
        if (entrance_menu->available[page]) {
            entrance_menu->pages++;
            task = task_create_child(0, page_tab_step, parent);
            task->draw = page_tab_draw;
            task->destructor = page_tab_destroy;
            task->work = syCalloc(1, sizeof(*tab));
            tab = (struct page_tab *)task->work;
            tab->list = entrance_menu->pages - 1;
            /* the picker opens on multiplayer's first page, the memory
             * cards', or on another group's second. that page's tab stands
             * at the center, the next page's at the right and the one after
             * hidden. the page before the shown one has its tab at the left.
             * that page is another group's first, or multiplayer's fourth,
             * which L wraps to from the first page */
            if (garden_race_group() == RACE_MULTI) {
                /* @hack spelling: the fourth page's case first, where the
                 * pages' order would do */
                switch (entrance_menu->pages) {
                case 4:
                    PLACE_LEFT();
                    break;
                case 1:
                    PLACE_CENTER();
                    break;
                case 2:
                    PLACE_RIGHT();
                    break;
                case 3:
                    PLACE_HIDDEN();
                    break;
                }
            } else {
                switch (entrance_menu->pages) {
                case 1:
                    PLACE_LEFT();
                    break;
                case 2:
                    PLACE_CENTER();
                    break;
                case 3:
                    PLACE_RIGHT();
                    break;
                case 4:
                    PLACE_HIDDEN();
                    break;
                }
            }
            /* @hack spelling: a case for each page, where
             * entrance_label_ids[page + PAGE_LABELS] would do */
            switch (page) {
            case 0:
                tab->label = entrance_label_ids[PAGE_LABELS];
                break;
            case 1:
                tab->label = entrance_label_ids[PAGE_LABELS + 1];
                break;
            case 2:
                tab->label = entrance_label_ids[PAGE_LABELS + 2];
                break;
            case 3:
                tab->label = entrance_label_ids[PAGE_LABELS + 3];
                break;
            }
        }
    }
}

void entrance_chao_picker(struct task *parent)
{
    int slot;
    int page;
    if (garden_race_group() != RACE_MULTI) {
        entrance_message_set(PICK_MESSAGE);
    } else {
        entrance_message_set(MULTI_PICK_MESSAGE);
        entrance_message_append(MULTI_PICK_MESSAGE + 1);
    }
    for (page = 0; page < CHAO_GARDENS + 1; page++) {
        for (slot = 0; slot < RACE_ENTRANTS; slot++) {
            entrance_menu->choices[page][slot].selected = FALSE;
        }
    }
    for (slot = 0; slot < RACE_ENTRANTS; slot++) {
        picker_slot_create(parent, slot);
    }
    picker_cursor_create(parent);
    picker_panel_create(parent, LEFT_PANEL_X, TAB_Y, FALSE);
    picker_panel_create(parent, RIGHT_PANEL_X, TAB_Y, TRUE);
    page_tabs_create(parent);
    if (garden_race_group() == RACE_MULTI) {
        entrance_confirm_button(parent, BUTTON_X, CONFIRM_Y, CONFIRM_ROW);
        entrance_back_button(parent, BUTTON_X, MULTI_BACK_Y, MULTI_BACK_ROW);
    } else {
        entrance_back_button(parent, BUTTON_X, BACK_Y, BACK_ROW);
    }
    entrance_menu->cursor = 0;
}
