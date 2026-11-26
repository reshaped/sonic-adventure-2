#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/composition.h"
#include "chaostgkinder/font.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/widget.h"
#include "system/pad.h"
#include "system/task.h"

/* the frames after a push before the first window takes input */
#define PUSH_DELAY 6

struct widget_node {
    struct kinder_widget *widget;
    struct widget_node *next;
};

/* the widgets kinder_widget_push has pushed, the last one first. each frame
 * window_task updates and draws them all, and gives input to the first alone */
static struct widget_node *windows;
static Sint32 input_delay;
static struct task *window_task;
static void read_input(struct kinder_widget *widget);

void kinder_widget_push(struct kinder_widget *widget)
{
    struct widget_node *node = syCalloc(1, sizeof(*node));
    node->widget = widget;
    node->next = windows;
    windows = node;
    input_delay = PUSH_DELAY;
}

void kinder_widget_remove(struct kinder_widget *widget)
{
    struct widget_node *head = windows;
    struct widget_node *node;
    if (head) {
        if (head->widget == widget) {
            windows = head->next;
            syFree(head);
        } else {
            node = head;
            while (node->next) {
                if (node->next->widget == widget) {
                    struct widget_node *removed = node->next;
                    node->next = removed->next;
                    syFree(removed);
                    break;
                }
                node = node->next;
            }
        }
    }
}

static void windows_destroy(struct task *task)
{
}

static void windows_draw(struct task *task)
{
    struct widget_node *node;
    for (node = windows; node; node = node->next) {
        struct kinder_widget *widget = node->widget;
        widget->ops->draw(widget, 0, 0, 0);
    }
}

static void windows_step(struct task *task)
{
    struct widget_node *node = windows;
    struct widget_node *next;
    struct kinder_widget *widget;
    while (node) {
        widget = node->widget;
        next = node->next;
        widget->ops->update(widget);
        if (widget->notify) {
            widget->notify(widget->owner, widget, WIDGET_TICK);
        }
        node = next;
    }
    node = windows;
    if (node) {
        if (input_delay) {
            input_delay--;
        } else {
            read_input(node->widget);
        }
    }
}

static void windows_start(void)
{
    window_task = task_create(0, 2, windows_step, "KbdWin2ListExec");
    window_task->destructor = windows_destroy;
    window_task->draw = windows_draw;
    windows = NULL;
}

struct kinder_widget *
kinder_widget_create(const struct kinder_widget_desc *desc)
{
    struct kinder_widget *widget;
    switch (desc->kind) {
    case WIDGET_CUSTOM:
        widget = kinder_custom_create(desc);
        break;
    case WIDGET_CONTAINER:
        widget = kinder_container_create(desc);
        break;
    case WIDGET_LABEL:
        widget = kinder_text_create(desc);
        break;
    case WIDGET_TEXT_ENTRY:
        widget = kinder_entry_create(desc);
        break;
    case WIDGET_BUTTON:
        widget = kinder_button_create(desc);
        break;
    case WIDGET_FRAME:
        widget = kinder_frame_create(desc);
        break;
    case WIDGET_LIST:
        widget = kinder_list_create(desc);
        break;
    case WIDGET_TEXTURE:
        widget = kinder_texture_create(desc);
        break;
    case WIDGET_SCALED_TEXT:
        widget = kinder_scaled_text_create(desc);
        break;
    case WIDGET_CAPTION:
        widget = kinder_caption_create(desc);
        break;
        /* @bug uninitialized: returns widget unset for a kind no case names
         * (0x0c9d2e92) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: any other kind returns null */
    default:
        return NULL;
#endif
    }
    return widget;
}

void kinder_widgets_start(void)
{
    kinder_font_start();
    kinder_strings_load();
    kinder_keys_start();
    kinder_keyboard_start();
    kinder_kana_start();
    windows_start();
}

void kinder_widgets_stop(void)
{
    task_kill(window_task);
    kinder_kana_stop();
    kinder_keyboard_stop();
    kinder_keys_stop();
    kinder_strings_release();
    kinder_font_stop();
}

struct kinder_widget *kinder_widget_alloc(const struct kinder_widget_desc *desc)
{
    struct kinder_widget *widget;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((widget = syCalloc(1, sizeof(*widget))) != NULL) {
        widget->kind = desc->kind;
        widget->id = desc->id;
        widget->x = desc->x;
        widget->y = desc->y;
        widget->z = desc->z;
        widget->width = desc->width;
        widget->height = desc->height;
        widget->parent = NULL;
        widget->visible = TRUE;
        widget->owner = NULL;
        widget->notify = desc->notify;
        widget->state = NULL;
        widget->ops = NULL;
    }
    return widget;
}

void kinder_widget_free(struct kinder_widget *widget)
{
    syFree(widget);
}

void kinder_widget_update(struct kinder_widget *widget)
{
}

void kinder_widget_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                        Sint32 z)
{
}

Sint32 kinder_widget_input(struct kinder_widget *widget, Uint32 key)
{
    int event;
    kinder_notify *notify;

    switch (key) {
    case KBD_ENTER:
    case KBD_SPACE:
    case WIDGET_PAD_A:
        event = WIDGET_ACCEPT;
        break;
    case KBD_ESCAPE:
    case WIDGET_PAD_B:
        event = WIDGET_CANCEL;
        break;
    case KBD_TAB:
        event = WIDGET_TAB;
        break;
    case KBD_RIGHT:
    case WIDGET_PAD_RIGHT:
        event = WIDGET_RIGHT;
        break;
    case KBD_LEFT:
    case WIDGET_PAD_LEFT:
        event = WIDGET_LEFT;
        break;
    case KBD_DOWN:
    case WIDGET_PAD_DOWN:
        event = WIDGET_DOWN;
        break;
    case KBD_UP:
    case WIDGET_PAD_UP:
        event = WIDGET_UP;
        break;
    default:
        return FALSE;
    }
    /* @hack spelling: notify assigned inside the returned expression, where
     * an assignment before the return would do */
    return (notify = widget->notify) && notify(widget->owner, widget, event);
}

static void read_input(struct kinder_widget *widget)
{
    Uint16 key;
    kinder_keys_poll();
    /* @hack spelling: the comma that tests key after its assignment, where
     * testing the assignment would do */
    while ((key = kinder_keys_next()), key) {
        widget->ops->input(widget, key);
    }
#define PAD_INPUT(mask, key)                                                   \
    if (player_pads[0]->press & (mask)) {                                      \
        widget->ops->input(widget, (key));                                     \
    }
    PAD_INPUT(PDD_DGT_KU, WIDGET_PAD_UP)
    PAD_INPUT(PDD_DGT_KD, WIDGET_PAD_DOWN)
    PAD_INPUT(PDD_DGT_KL, WIDGET_PAD_LEFT)
    PAD_INPUT(PDD_DGT_KR, WIDGET_PAD_RIGHT)
    PAD_INPUT(PDD_DGT_TA, WIDGET_PAD_A)
    PAD_INPUT(PDD_DGT_TB, WIDGET_PAD_B)
    PAD_INPUT(PDD_DGT_TX, WIDGET_PAD_X)
    PAD_INPUT(PDD_DGT_TY, WIDGET_PAD_Y)
    PAD_INPUT(PDD_DGT_TL, WIDGET_PAD_L)
    PAD_INPUT(PDD_DGT_TR, WIDGET_PAD_R)
    PAD_INPUT(PDD_DGT_ST, WIDGET_PAD_START)
#undef PAD_INPUT
}

void kinder_rects_draw(const struct kinder_rect *entries, Int count, Int x,
                       Int y, Int z)
{
    const struct kinder_rect *entry;
    Int i;
    Bool started;

    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    {
        NJS_QUAD_TEXTURE quad;
        /* @hack spelling: one block holds both textured passes' quad, where
         * each pass could declare its own */
#define TEXTURED_PASS(trans, in_pass)                                          \
    entry = entries;                                                           \
    started = FALSE;                                                           \
    for (i = 0; i < count; i++) {                                              \
        if (entry->texture >= 0 && (in_pass)) {                                \
            if (!started) {                                                    \
                njQuadTextureStart(trans);                                     \
                started = TRUE;                                                \
            }                                                                  \
            quad.x1 = entry->x1 + x;                                           \
            quad.x2 = entry->x2 + x;                                           \
            quad.y1 = entry->y1 + y;                                           \
            quad.y2 = entry->y2 + y;                                           \
            quad.u1 = KINDER_ATLAS_UV(entry->u1);                              \
            quad.u2 = KINDER_ATLAS_UV(entry->u2);                              \
            quad.v1 = KINDER_ATLAS_UV(entry->v1);                              \
            quad.v2 = KINDER_ATLAS_UV(entry->v2);                              \
            njSetQuadTexture(entry->texture, entry->color);                    \
            njDrawQuadTexture(&quad, KINDER_WIDGET_DEPTH(entry->z + z));       \
        }                                                                      \
        entry++;                                                               \
    }                                                                          \
    if (started) {                                                             \
        njQuadTextureEnd();                                                    \
    }
        TEXTURED_PASS(FALSE, !entry->translucent);
        TEXTURED_PASS(TRUE, entry->translucent);
#undef TEXTURED_PASS
    }
    {
        NJS_POLYGON_VTX quad[4];
        entry = entries;
        for (i = 0; i < count; i++) {
            if (entry->texture < 0) {
                quad[0].x = quad[1].x = entry->x1 + x;
                quad[2].x = quad[3].x = entry->x2 + x;
                quad[0].y = quad[2].y = entry->y1 + y;
                quad[1].y = quad[3].y = entry->y2 + y;
                quad[0].z = quad[1].z = quad[2].z = quad[3].z =
                    KINDER_WIDGET_DEPTH(entry->z + z);
                quad[0].col = quad[1].col = quad[2].col = quad[3].col =
                    entry->color;
                njDrawPolygon(quad, 4, entry->translucent);
            }
            entry++;
        }
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}
