#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chaostgkinder/messages.h"
#include "chaostgkinder/service_menu.h"

#include "draw/lines.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

enum service_cell {
    BOX_NW,
    BOX_NE,
    BOX_SW,
    BOX_SE,
    BOX_FILL,
    BOX_ACCENT,
    BOX_MARK,
    FRAME_NW,
    FRAME_NE,
    FRAME_SW,
    FRAME_SE,
    FRAME_HORIZONTAL,
    FRAME_VERTICAL,
    FRAME_PINK_TILE,
    FRAME_BLUE_TILE,
    FRAME_GOLD_TILE,
    FRAME_EMBLEM,
    FRAME_ARROW,
    PAGE_NW,
    PAGE_N,
    PAGE_NE,
    PAGE_W,
    PAGE_CENTER,
    PAGE_E,
    PAGE_SW,
    PAGE_S,
    PAGE_SE,
    PAGE_FILL,
    SERVICE_CELLS
};

#define SERVICE_BLEND()                                                        \
    do {                                                                       \
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);    \
        njColorBlendingMode(NJD_DESTINATION_COLOR,                             \
                            NJD_COLOR_BLENDING_INVSRCALPHA);                   \
    } while (0)

#define SERVICE_SPRITE(sprite, px, py, scale)                                  \
    do {                                                                       \
        (sprite).p.x = (px);                                                   \
        (sprite).p.y = (py);                                                   \
        (sprite).p.z = 0.0F;                                                   \
        (sprite).sx = (scale);                                                 \
        (sprite).sy = (scale);                                                 \
        (sprite).ang = 0;                                                      \
        (sprite).tlist = &kinder_service_textures;                             \
        (sprite).tanim = kinder_service_cells;                                 \
    } while (0)

#define SERVICE_DRAW(sprite, cell, depth)                                      \
    njDrawSprite2D(&(sprite), (cell), (depth), (sprite).tanim[(cell)].attr)

static NJS_TEXANIM kinder_service_cells[SERVICE_CELLS] = {
#include "chaostgkinder/service_menu/kinder_service_cells.inc"
};

/* the bit of a button's work->flags that is set while it is selected */
#define BUTTON_SELECTED 1
/* a button the tracking selects pulses in size. scl.y is the pulse's
 * amplitude, from PULSE_AMPLITUDE. each step multiplies it by PULSE_DECAY
 * until it falls below PULSE_END. ang.x is the pulse's phase, and scl.x the
 * scale the button draws at */
#define PULSE_AMPLITUDE 0.1F
#define PULSE_DECAY 0.9F
#define PULSE_END 0.01F

/* a box's and a page's corner cells are CORNER pixels square. the strips
 * between them start a pixel inside the corners, at EDGE_INSET */
#define CORNER 16.0F
#define EDGE_INSET 15.0F

/* a frame's corner cells are FRAME_CORNER pixels square. its horizontal edges
 * start FRAME_HORIZONTAL_INSET in from its sides. its vertical edges start
 * FRAME_VERTICAL_INSET down from its top. the bottom and right edges stand
 * FRAME_FAR_EDGE in from the frame's bottom and right */
#define FRAME_CORNER 24.0F
#define FRAME_HORIZONTAL_INSET 16.0F
#define FRAME_VERTICAL_INSET 21.0F
#define FRAME_FAR_EDGE 5.0F
/* the gold frame's emblem stands EMBLEM_LEFT left of the frame's middle, and
 * EMBLEM_TOP below its top */
#define EMBLEM_LEFT 32.0F
#define EMBLEM_TOP 8.0F
/* the tiles that fill a frame, TILE pixels square, or GOLD_TILE in the gold
 * frame. the gold frame's rows start TILE_MARGIN in from its left. where a
 * row's last tile doesn't fit, it stops TILE_MARGIN short of the frame's
 * right edge */
#define TILE 8.0F
#define GOLD_TILE 64.0F
#define TILE_MARGIN 4.0F

/* a page's panel, and the rule PANEL_RULE pixels wide along the inside of
 * its edges */
#define PANEL_X 158
#define PANEL_Y 102
#define PANEL_WIDTH 424
#define PANEL_HEIGHT 198
#define PANEL_RULE 2
/* the tab above a page's panel has a cap at each end, a middle and a band
 * below. the middle starts TAB_MIDDLE_INSET in, over the caps */
#define TAB_Y 80
#define TAB_WIDTH 224
#define TAB_CAP 16
#define TAB_MIDDLE_INSET 12
#define TAB_BAND_DROP 15
#define TAB_BAND_HEIGHT 8
/* the tab's cells in the service atlas, in texels. its caps are TAB_CAP
 * square, from v TAB_V. its middle and band run from u TAB_MIDDLE_U to
 * TAB_MIDDLE_END */
#define TAB_LEFT_U 168
#define TAB_RIGHT_U 192
#define TAB_MIDDLE_U 176
#define TAB_MIDDLE_END 182
#define TAB_V 8

/* how far past its box's depth each part draws. a selected button's border
 * and a frame's tiles draw that far short of it */
#define CORNER_MARK_DEPTH 0.1F /* a panel's decoration at its corners */
#define MARK_DEPTH 0.5F /* a panel's decoration elsewhere */
#define BORDER_DEPTH 0.001F
#define CAPTION_DEPTH 0.02F
#define TILE_DEPTH 0.01F
#define PANEL_DEPTH 0.01F
#define RULE_DEPTH 0.02F /* a page's rule and its tab */

/* a selected button's border, SELECTED_BORDER pixels wide, and the colors
 * of a text button's caption */
#define SELECTED_BORDER 2
#define WHITE 0xffffffff
#define CAPTION_COLOR 0xff595959

void service_box_draw(const NJS_SPRITE *source, const struct service_box *box,
                      int border, Float depth)
{
    NJS_SPRITE sprite;
    Float bottom;
    Float x;
    Float y;
    Float width;
    Float height;
    Float sx;
    Float sy;
    Float left;
    Float center_x;
    Float center_y;
    if (!source) {
        SERVICE_SPRITE(sprite, 0, 0, 1.0F);
    } else {
        sprite = *source;
        if (!sprite.tlist) {
            sprite.tlist = &kinder_service_textures;
        }
        if (!sprite.tanim) {
            sprite.tanim = kinder_service_cells;
        }
    }
    sx = sprite.sx;
    sy = sprite.sy;
    if (sx == 1.0F && sy == 1.0F) {
        width = box->width + (Float)(border * 2);
        height = box->height + (Float)(border * 2);
        x = box->pos.x - border;
        y = box->pos.y - border;
    } else {
        center_x = box->pos.x + (box->width * 0.5F);
        center_y = box->pos.y + (box->height * 0.5F);
        width = box->width + (Float)(border * 2);
        height = box->height + (Float)(border * 2);
        x = center_x - (width * sx * 0.5F);
        y = center_y - (height * sy * 0.5F);
    }
    sprite.p.x = x;
    sprite.p.y = y;
    SERVICE_DRAW(sprite, BOX_NW, depth);
    sprite.p.x += (width - CORNER) * sx;
    SERVICE_DRAW(sprite, BOX_NE, depth);
    bottom = (height - CORNER) * sy;
    sprite.p.y += bottom;
    SERVICE_DRAW(sprite, BOX_SE, depth);
    sprite.p.x = x;
    SERVICE_DRAW(sprite, BOX_SW, depth);
    left = x + (EDGE_INSET * sx);
    sprite.p.x = left;
    sprite.p.y = y;
    sprite.sx = (width - (2 * EDGE_INSET)) * sx;
    sprite.sy = CORNER * sy;
    SERVICE_DRAW(sprite, BOX_FILL, depth);
    sprite.p.x = left;
    sprite.p.y = y + bottom;
    SERVICE_DRAW(sprite, BOX_FILL, depth);
    sprite.p.x = x;
    sprite.p.y = y + (EDGE_INSET * sy);
    sprite.sx = width * sx;
    sprite.sy = (height - (2 * EDGE_INSET)) * sy;
    if (sprite.sy > 0.0F) {
        SERVICE_DRAW(sprite, BOX_FILL, depth);
    }
}

void service_set_color(Uint32 argb)
{
    Float color[4];
    /* @hack type: i a Uint8, where an int would do */
    Uint8 i;
    for (i = 0; i < 4; i++) {
        int channel = (Uint8)(argb >> ((3 - i) * 8));
        color[i] = channel * (1.0F / 255.0F);
    }
    constant_material_set(color[0], color[1], color[2], color[3]);
}

static void service_panel_draw(struct task *task)
{
    struct service_box *box = task->extra.p;
    struct service_decoration *decoration = &box->detail.decoration;
    NJS_SPRITE sprite;
    int cell;
    /* @hack type: i a char, where an int would do */
    char i;
    Float depth;
    SERVICE_SPRITE(sprite, 0, 0, 1.0F);
    SERVICE_BLEND();
    service_set_color(box->color);
    service_box_draw(&sprite, box, 0, box->pos.z);
    if (box->flags & SERVICE_DECORATED) {
        cell = decoration->cell;
        if (sprite.tanim[cell].attr & NJD_SPRITE_ALPHA) {
            service_set_color(decoration->color);
        } else {
            constant_material_set(0, 0, 0, 0);
        }
        /* @hack redundant: decoration->scale multiplied by 1.0F in both arms,
         * where decoration->scale alone would do */
        if (box->flags & SERVICE_DECORATE_CORNERS) {
            depth = box->pos.z + CORNER_MARK_DEPTH;
            sprite.sx = 1.0F * decoration->scale;
            sprite.sy = 1.0F * decoration->scale;
            sprite.p.x = box->pos.x;
            sprite.p.y = box->pos.y;
            SERVICE_DRAW(sprite, cell, depth);
            sprite.p.x += box->width - sprite.tanim[cell].sx;
            SERVICE_DRAW(sprite, cell, depth);
            sprite.p.y += box->height - sprite.tanim[cell].sy;
            SERVICE_DRAW(sprite, cell, depth);
            sprite.p.x = box->pos.x;
            SERVICE_DRAW(sprite, cell, depth);
        } else {
            depth = box->pos.z + MARK_DEPTH;
            sprite.sx = 1.0F * decoration->scale;
            sprite.sy = 1.0F * decoration->scale;
            for (i = 0; i < 4; ++i) {
                if (i % 2 == 0) {
                    sprite.p.x = decoration->left;
                } else {
                    sprite.p.x = decoration->right;
                }
                if (i / 2 == 0) {
                    sprite.p.y = decoration->top;
                } else {
                    sprite.p.y = decoration->bottom;
                }
                SERVICE_DRAW(sprite, cell, depth);
            }
        }
        constant_material_set(0, 0, 0, 0);
    }
}

static void service_panel_destroy(struct task *task)
{
}
static void service_panel_step(struct task *task)
{
    if (game_state.time_stopped) {
        return;
    }
}
void service_panel_create(struct task *parent, const struct service_box *box,
                          const struct service_decoration *decoration)
{
    struct task *task;
    struct service_box *copy;
    struct service_decoration *destination;
    if (!parent) {
        return;
    }
    task = task_create_child(0, service_panel_step, parent);
    if (!task) {
        return;
    }
    task->draw = service_panel_draw;
    task->destructor = service_panel_destroy;
    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        task_destroy(task);
        return;
    }
    *copy = *box;
    task->extra.p = copy;
    destination = &copy->detail.decoration;
    *destination = *decoration;
}

static void service_button_step(struct task *task)
{
    struct task_work *work = task->work;
    struct service_box *box = task->extra.p;
    struct service_choice *choice = &box->detail.choice;
    if (game_state.time_stopped) {
        return;
    }
    if (!(work->flags & BUTTON_SELECTED)) {
        if ((box->flags & SERVICE_TRACK_SELECTION) &&
            work->id == *choice->selection) {
            work->scl.y = PULSE_AMPLITUDE;
            work->flags |= BUTTON_SELECTED;
        }
    } else {
        if (work->scl.y > 0.0F) {
            work->ang.x += EIGHTH_TURN;
            work->scl.x = 1.0F + (njSin(work->ang.x) * work->scl.y);
            work->scl.y *= PULSE_DECAY;
            if (work->scl.y < PULSE_END) {
                work->scl.y = 0;
                work->scl.x = 1;
                work->ang.x = 0;
            }
        }
        if (box->flags & SERVICE_TRACK_SELECTION) {
            if (work->id != *choice->selection) {
                work->ang.x = 0;
                work->scl.x = 1;
                work->flags &= ~BUTTON_SELECTED;
            }
        }
    }
}
static void service_button_draw(struct task *task)
{
    struct service_box *box = task->extra.p;
    struct task_work *work = task->work;
    struct service_choice *choice = &box->detail.choice;
    NJS_SPRITE sprite;
    SERVICE_SPRITE(sprite, 0, 0, work->scl.x);
    SERVICE_BLEND();
    if (!(work->flags & BUTTON_SELECTED)) {
        service_set_color(box->color);
        service_box_draw(&sprite, box, 0, box->pos.z);
    } else {
        service_set_color(choice->selected_color);
        service_box_draw(&sprite, box, 0, box->pos.z);
        service_set_color(WHITE);
        service_box_draw(&sprite, box, SELECTED_BORDER,
                         box->pos.z - BORDER_DEPTH);
    }
    constant_material_set(0, 0, 0, 0);
}
static void service_button_destroy(struct task *task)
{
}
struct task *service_button_create(struct task *parent, int id,
                                   const struct service_box *box,
                                   const struct service_choice *choice)
{
    struct task *task;
    struct task_work *work;
    struct service_box *copy;
    struct service_choice *selection;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, service_button_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = service_button_draw;
    task->destructor = service_button_destroy;
    work = task->work;
    work->id = (Uint8)id;
    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = copy;
    *copy = *box;
    if (choice) {
        selection = &copy->detail.choice;
        *selection = *choice;
        if (!selection->selection) {
            copy->flags &= ~SERVICE_TRACK_SELECTION;
        }
        if (copy->flags & SERVICE_TRACK_SELECTION) {
            if (*selection->selection == work->id) {
                work->flags |= BUTTON_SELECTED;
            }
        }
    }
    work->scl.x = 1.0F;
    return task;
}
void service_text_button(struct task *parent, int id,
                         const struct service_box *box,
                         const struct service_choice *choice, const char *text)
{
    struct kinder_message_label caption;
    service_button_create(parent, id, box, choice);
    caption.bank = 0;
    caption.flags = KINDER_MESSAGE_LABEL_TRACK | KINDER_MESSAGE_LABEL_CENTER |
                    KINDER_MESSAGE_LABEL_LITERAL;
    caption.id = id;
    caption.color = CAPTION_COLOR;
    caption.selected_color = WHITE;
    caption.message = 0;
    caption.selection = (const Uint8 *)choice->selection;
    caption.pos.x = box->pos.x + (box->width * 0.5F);
    caption.pos.y = box->pos.y + (box->height * 0.5F);
    caption.pos.z = box->pos.z + CAPTION_DEPTH;
    caption.width = box->width;
    caption.text = text;
    caption.scale = 1.0F;
    kinder_message_label_create(parent, &caption);
}

void service_button_select(struct task *task)
{
    struct task_work *work = task->work;
    work->flags |= BUTTON_SELECTED;
}
void service_button_deselect(struct task *task)
{
    struct task_work *work = task->work;
    work->ang.x = 0;
    work->scl.x = 1.0F;
    work->flags &= ~BUTTON_SELECTED;
}
static void service_frame_step(struct task *task)
{
    if (game_state.time_stopped) {
        return;
    }
}
static void service_frame_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct service_box *box = task->extra.p;
    NJS_SPRITE sprite;
    Float width;
    Float height;
    Float x;
    Float y;
    Float depth;
    Float top;
    Float left;
    Float tile_x;
    Float tile_y;
    Float tile_size;
    Float remaining;
    int cell;
    NJS_TEXANIM *tile;
    SERVICE_SPRITE(sprite, 0, 0, 1.0F);
    SERVICE_BLEND();
    switch (work->id) {
    case SERVICE_PINK:
        constant_material_set(1, 1, 29.0F / 255.0F, 167.0F / 255.0F);
        break;
    case SERVICE_BLUE:
        constant_material_set(1, 0, 80.0F / 255.0F, 167.0F / 255.0F);
        break;
    case SERVICE_GOLD:
        constant_material_set(1, 152.0F / 255.0F, 142.0F / 255.0F, 0);
        break;
    }
    width = box->width;
    height = box->height;
    x = box->pos.x;
    y = box->pos.y;
    depth = box->pos.z;
    sprite.p.x = x;
    sprite.p.y = y;
    SERVICE_DRAW(sprite, FRAME_NW, depth);
    sprite.p.x += width - FRAME_CORNER;
    SERVICE_DRAW(sprite, FRAME_NE, depth);
    sprite.p.y += height - FRAME_CORNER;
    SERVICE_DRAW(sprite, FRAME_SE, depth);
    sprite.p.x = x;
    SERVICE_DRAW(sprite, FRAME_SW, depth);
    sprite.p.x = x + FRAME_HORIZONTAL_INSET;
    sprite.p.y = y;
    sprite.sx = width - (2 * FRAME_HORIZONTAL_INSET);
    SERVICE_DRAW(sprite, FRAME_HORIZONTAL, depth);
    sprite.p.y = y + height - FRAME_FAR_EDGE;
    SERVICE_DRAW(sprite, FRAME_HORIZONTAL, depth);
    sprite.p.x = x;
    sprite.p.y = y + FRAME_VERTICAL_INSET;
    sprite.sx = 1;
    sprite.sy = height - (2 * FRAME_VERTICAL_INSET);
    cell = FRAME_VERTICAL;
    SERVICE_DRAW(sprite, cell, depth);
    sprite.p.x = x + width - FRAME_FAR_EDGE;
    SERVICE_DRAW(sprite, cell, depth);
    sprite.sx = sprite.sy = 1.0F;
    constant_material_set(0, 0, 0, 0);
    if (work->id == SERVICE_GOLD) {
        sprite.p.x = x + (width * 0.5F) - EMBLEM_LEFT;
        sprite.p.y = y + EMBLEM_TOP;
        cell = FRAME_EMBLEM;
        SERVICE_DRAW(sprite, cell, depth);
    }
    switch (work->id) {
    case SERVICE_PINK:
        cell = FRAME_PINK_TILE;
        break;
    case SERVICE_BLUE:
        cell = FRAME_BLUE_TILE;
        break;
    case SERVICE_GOLD:
        cell = FRAME_GOLD_TILE;
        break;
    }
    tile_x = x;
    tile_y = y;
    left = x;
    top = y;
    if (cell == FRAME_GOLD_TILE) {
        tile_size = GOLD_TILE;
        tile_x += TILE_MARGIN;
        top = y + TILE_MARGIN;
        left = x + TILE_MARGIN;
    } else {
        tile_size = TILE;
    }
    /* a frame of small tiles leaves out its four corner tiles */
    /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
    for (; tile_y < y + height; tile_y += tile_size, tile_x = left) {
        /* NOLINTNEXTLINE(bugprone-float-loop-counter) */
        for (; tile_x < x + width; tile_x += tile_size) {
            sprite.p.x = tile_x;
            sprite.p.y = tile_y;
            remaining = x + width - tile_x;
            tile = &sprite.tanim[cell];
            if (!(tile_size == TILE &&
                  (tile_x == left || tile_x + tile_size >= left + width) &&
                  (tile_y == top || tile_y + tile_size >= top + height))) {
                if (remaining < tile_size) {
                    tile->sx = (Sint16)remaining - TILE_MARGIN;
                    tile->u2 = tile->u1 + (Sint16)remaining - TILE_MARGIN;
                }
                SERVICE_DRAW(sprite, cell, depth - TILE_DEPTH);
                if (remaining < tile_size) {
                    tile->sx = tile_size;
                    tile->u2 = tile->u1 + tile_size;
                }
            }
        }
    }
    constant_material_set(0, 0, 0, 0);
}
static void service_frame_destroy(struct task *task)
{
}
struct task *service_frame_create(struct task *parent, Uint8 palette,
                                  const struct service_box *box)
{
    struct task *task;
    struct task_work *work;
    struct service_box *copy;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, service_frame_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = service_frame_draw;
    task->destructor = service_frame_destroy;
    work = task->work;
    work->id = palette;
    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = copy;
    *copy = *box;
    return task;
}
void service_arrow_draw(Float x, Float y, Float depth)
{
    NJS_SPRITE sprite;
    SERVICE_SPRITE(sprite, x, y, 1.0F);
    SERVICE_BLEND();
    SERVICE_DRAW(sprite, FRAME_ARROW, depth);
}
static void service_page_step(struct task *task)
{
    if (game_state.time_stopped) {
        return;
    }
}

/* @hack spelling: PAGE_RECT as a comma expression, where a do-while block
 * would do */
#define PAGE_RECT(sprite, px, py, w, h, depth)                                 \
    ((sprite).p.x = (px), (sprite).p.y = (py), (sprite).sx = (w),              \
     (sprite).sy = (h), SERVICE_DRAW(sprite, PAGE_FILL, depth))

static void service_page_draw(struct task *task)
{
    struct service_box *box = task->extra.p;
    NJS_SPRITE sprite;
    NJS_TEXANIM cap;
    Float width;
    Float height;
    Float x;
    Float y;
    Float depth;
    Float inner_x;
    Float inner_y;
    Float inner_width;
    Float inner_height;
    /* @hack redundant: this SERVICE_BLEND, which the one below repeats */
    SERVICE_BLEND();
    SERVICE_SPRITE(sprite, 0, 0, 1.0F);
    SERVICE_BLEND();
    width = box->width;
    height = box->height;
    x = box->pos.x;
    y = box->pos.y;
    depth = box->pos.z;

    sprite.p.x = x;
    sprite.p.y = y;
    SERVICE_DRAW(sprite, PAGE_NW, depth);
    sprite.p.x += width - CORNER;
    SERVICE_DRAW(sprite, PAGE_NE, depth);
    sprite.p.y += height - CORNER;
    SERVICE_DRAW(sprite, PAGE_SE, depth);
    sprite.p.x = x;
    SERVICE_DRAW(sprite, PAGE_SW, depth);
    sprite.p.x = inner_x = x + EDGE_INSET;
    sprite.p.y = y;
    sprite.sx = inner_width = width - (2 * EDGE_INSET);
    SERVICE_DRAW(sprite, PAGE_N, depth);
    sprite.p.y = y + height - EDGE_INSET;
    SERVICE_DRAW(sprite, PAGE_S, depth);
    sprite.p.x = x;
    sprite.p.y = inner_y = y + EDGE_INSET;
    sprite.sx = 1.0F;
    sprite.sy = inner_height = height - (2 * EDGE_INSET);
    SERVICE_DRAW(sprite, PAGE_W, depth);
    sprite.p.x = x + width - EDGE_INSET;
    SERVICE_DRAW(sprite, PAGE_E, depth);
    sprite.p.x = inner_x;
    sprite.p.y = inner_y;
    sprite.sx = inner_width;
    sprite.sy = inner_height;
    SERVICE_DRAW(sprite, PAGE_CENTER, depth);

    constant_material_set(1, 1, 1, 1);
    PAGE_RECT(sprite, PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_HEIGHT,
              depth + PANEL_DEPTH);
    constant_material_set(1, 249.0F / 255.0F, 109.0F / 255.0F, 28.0F / 255.0F);
    PAGE_RECT(sprite, PANEL_X, PANEL_Y, PANEL_WIDTH, PANEL_RULE,
              depth + RULE_DEPTH);
    PAGE_RECT(sprite, PANEL_X, PANEL_Y + PANEL_HEIGHT - PANEL_RULE, PANEL_WIDTH,
              PANEL_RULE, depth + RULE_DEPTH);
    PAGE_RECT(sprite, PANEL_X, PANEL_Y, PANEL_RULE, PANEL_HEIGHT,
              depth + RULE_DEPTH);
    PAGE_RECT(sprite, PANEL_X + PANEL_WIDTH - PANEL_RULE, PANEL_Y, PANEL_RULE,
              PANEL_HEIGHT, depth + RULE_DEPTH);

    cap = sprite.tanim[BOX_NW];
    cap.sx = TAB_CAP;
    cap.sy = TAB_CAP;
    cap.cx = 0;
    cap.cy = 0;
    cap.u1 = TAB_LEFT_U;
    cap.u2 = TAB_LEFT_U + TAB_CAP;
    cap.v1 = TAB_V;
    cap.v2 = TAB_V + TAB_CAP;
    sprite.tanim = &cap;
    sprite.p.x = PANEL_X;
    sprite.p.y = TAB_Y;
    sprite.sx = 1;
    sprite.sy = 1;
    SERVICE_DRAW(sprite, 0, depth + RULE_DEPTH);
    cap.u1 = TAB_RIGHT_U;
    cap.u2 = TAB_RIGHT_U + TAB_CAP;
    cap.v1 = TAB_V;
    cap.v2 = TAB_V + TAB_CAP;
    sprite.p.x = PANEL_X + TAB_WIDTH - TAB_CAP;
    SERVICE_DRAW(sprite, 0, depth + RULE_DEPTH);
    cap.sx = 1;
    cap.sy = TAB_CAP;
    cap.u1 = TAB_MIDDLE_U;
    cap.u2 = TAB_MIDDLE_END;
    sprite.p.x = PANEL_X + TAB_MIDDLE_INSET;
    sprite.sx = TAB_WIDTH - (2 * TAB_MIDDLE_INSET);
    SERVICE_DRAW(sprite, 0, depth + RULE_DEPTH);
    cap.sx = 1;
    cap.sy = TAB_BAND_HEIGHT;
    sprite.p.x = PANEL_X;
    sprite.p.y += TAB_BAND_DROP;
    sprite.sx = TAB_WIDTH;
    sprite.sy = 1;
    SERVICE_DRAW(sprite, 0, depth + RULE_DEPTH);
    constant_material_set(0, 0, 0, 0);
}

static void service_page_destroy(struct task *task)
{
}
struct task *service_page_create(struct task *parent,
                                 const struct service_box *box)
{
    struct task *task;
    struct service_box *copy;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, service_page_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = service_page_draw;
    task->destructor = service_page_destroy;
    copy = syCalloc(1, sizeof(*copy));
    if (!copy) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = copy;
    *copy = *box;
    return task;
}
