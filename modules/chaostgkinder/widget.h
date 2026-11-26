/* the kindergarten's menu widgets. a container destroys its children with
 * itself. it gives each key to its focused child before it takes the key
 * itself */
#ifndef CHAOSTGKINDER_WIDGET_H
#define CHAOSTGKINDER_WIDGET_H
#include <ninja.h>
#include <sg_xpt.h>

struct kinder_widget;
struct kinder_widget_desc;
typedef int kinder_notify(void *owner, struct kinder_widget *widget, int event);

struct kinder_widget_ops {
    void (*destroy)(struct kinder_widget *widget);
    void (*update)(struct kinder_widget *widget);
    void (*draw)(struct kinder_widget *widget, Sint32 x, Sint32 y, Sint32 z);
    Sint32 (*input)(struct kinder_widget *widget, Uint32 key);
    void (*focus)(struct kinder_widget *widget, Sint32 selected);
};

struct kinder_widget_desc {
    Sint32 kind, id;
    Sint16 x, y, z;
    Sint16 width, height;
    const void *settings;
    kinder_notify *notify;
};

struct kinder_widget {
    Sint32 kind, id;
    Float x, y, z, width, height;
    struct kinder_widget *parent;
    Sint32 visible;
    void *owner;
    kinder_notify *notify;
    void *state;
    const struct kinder_widget_ops *ops;
};

enum kinder_widget_kind {
    WIDGET_CUSTOM,
    WIDGET_CONTAINER,
    WIDGET_LABEL,
    WIDGET_TEXT_ENTRY,
    WIDGET_BUTTON,
    WIDGET_FRAME,
    WIDGET_LIST,
    WIDGET_TEXTURE,
    WIDGET_SCALED_TEXT,
    WIDGET_CAPTION
};

enum kinder_widget_event {
    WIDGET_ACCEPT = 0,
    WIDGET_CANCEL = 1,
    WIDGET_UP = 2,
    WIDGET_DOWN = 3,
    WIDGET_LEFT = 4,
    WIDGET_RIGHT = 5,
    WIDGET_TAB = 6,
    /* no panel or widget sends it. the book editor's panels take it with
     * WIDGET_UP, as they take WIDGET_TAB with WIDGET_DOWN */
    WIDGET_BACK_TAB = 7,
    WIDGET_TICK = 8,
    WIDGET_SELECTED = 10
};

struct kinder_rect {
    Sint8 texture, translucent;
    Sint16 x1, x2, y1, y2, z;
    Sint16 u1, u2, v1, v2;
    Uint32 color;
};

/* a rect's and a texture widget's u and v count KINDER_ATLAS_UNITS units
 * across a texture. KINDER_ATLAS_UV gives a count of them as a texture
 * coordinate */
#define KINDER_ATLAS_UNITS 256.0F
#define KINDER_ATLAS_UV(unit) ((unit) * (1.0F / KINDER_ATLAS_UNITS))
/* the depth a widget draws at on layer, the sum of its z and its parents' */
#define KINDER_WIDGET_DEPTH(layer) (1.1F + ((layer) * 0.001F))

struct kinder_label_desc {
    Sint32 message;
};

struct kinder_scaled_label_desc {
    Sint32 message;
    Float scale, height;
};

#define CAPTION_CENTERED 1
#define CAPTION_SCALED 2
struct kinder_caption_desc {
    Sint32 message;
    Float scale, height;
    Uint32 color, flags;
};

struct kinder_button_desc {
    Sint32 message;
    NJS_TEXLIST *texlist;
    const struct kinder_rect *focused;
    Sint32 focused_count;
    const struct kinder_rect *unfocused;
    Sint32 unfocused_count;
};

struct kinder_button {
    const Uint16 *text;
    Sint32 selected;
    NJS_TEXLIST *texlist;
    const struct kinder_rect *focused;
    Sint32 focused_count;
    const struct kinder_rect *unfocused;
    Sint32 unfocused_count;
};

struct kinder_container_desc {
    NJS_TEXLIST *texlist;
    const struct kinder_rect *background;
    Sint32 background_count;
};

/* TEXTURE_TILE tiles both axes, and TEXTURE_STRETCH_X stretches across x and
 * tiles y. the atlas counts 2 units to a screen pixel. a tile is then half
 * its u or v interval. a tiled axis needs an interval of 2 or more. a narrower
 * one makes the tile 0 pixels, and the draw never ends */
#define TEXTURE_TILE 0
#define TEXTURE_STRETCH_X 1
struct kinder_texture_desc {
    NJS_TEXLIST *texlist;
    Sint8 texture, translucent;
    Sint16 x, y, z, width, height;
    Sint16 u1, u2, v1, v2;
    Uint32 color;
    Uint8 mode;
};

struct kinder_list_item_desc {
    Sint32 key, message;
};

/* a list needs at least one item and a notify. it calls the notify unchecked
 * when an item is chosen. selected is the key of the item the cursor starts
 * on. with entries NULL, every item's key is 0 and its text string 0 */
struct kinder_list_desc {
    NJS_TEXLIST *texlist;
    const struct kinder_list_item_desc *entries;
    Sint32 count, selected;
};

struct kinder_entry_desc {
    Sint32 rows, capacity;
};

struct kinder_frame_desc {
    NJS_TEXLIST *texlist;
    const struct kinder_rect *focused;
    Sint32 focused_count;
    const struct kinder_rect *unfocused;
    Sint32 unfocused_count;
    Sint32 capacity;
};

struct kinder_custom_desc {
    struct kinder_widget *(*create)(const struct kinder_widget_desc *desc);
};

/* player 0's pad buttons as keys, which read_input gives a widget's input
 * after the keyboard's. their codes start at 192, past every usage the
 * widgets test */
enum kinder_pad_key {
    WIDGET_PAD_UP = 192,
    WIDGET_PAD_DOWN,
    WIDGET_PAD_LEFT,
    WIDGET_PAD_RIGHT,
    WIDGET_PAD_A,
    WIDGET_PAD_B,
    WIDGET_PAD_X,
    WIDGET_PAD_Y,
    WIDGET_PAD_L,
    WIDGET_PAD_R,
    WIDGET_PAD_START
};

extern NJS_TEXLIST kinder_widget_texlist;
void kinder_widgets_start(void);
void kinder_widgets_stop(void);
void kinder_widget_update(struct kinder_widget *widget);
void kinder_widget_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                        Sint32 z);
Sint32 kinder_widget_input(struct kinder_widget *widget, Uint32 key);
/* draw count rects, offset by x, y and z. a rect with a negative texture is
 * a solid color, and any other draws that texture of the current texlist.
 * the opaque textured rects draw first, then the translucent ones, then the
 * solid ones. the textured rects draw point sampled, and the call leaves
 * the filter bilinear */
void kinder_rects_draw(const struct kinder_rect *entries, Int count, Int x,
                       Int y, Int z);

struct kinder_widget *
kinder_custom_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_container_create(const struct kinder_widget_desc *desc);
struct kinder_widget *kinder_text_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_entry_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_button_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_frame_create(const struct kinder_widget_desc *desc);
struct kinder_widget *kinder_list_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_texture_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_scaled_text_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_caption_create(const struct kinder_widget_desc *desc);

/* labels, captions and buttons keep the pointer they are given, not a copy.
 * the text must stay live while the widget shows it */
void kinder_text_set(struct kinder_widget *widget, const Uint16 *text);
void kinder_scaled_text_set(struct kinder_widget *widget, const Uint16 *text);
void kinder_caption_set_text(struct kinder_widget *widget, const Uint16 *text);
void kinder_button_set_text(struct kinder_widget *widget, const Uint16 *text);
void kinder_button_set_texlist(struct kinder_widget *widget,
                               NJS_TEXLIST *texlist);
void kinder_entry_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z);
Sint32 kinder_entry_input(struct kinder_widget *widget, Uint32 key);
void kinder_entry_begin(struct kinder_widget *widget);
void kinder_entry_end(struct kinder_widget *widget);
Sint32 kinder_entry_editing(struct kinder_widget *widget);
void kinder_entry_set_limit(struct kinder_widget *widget, Sint32 limit);
void kinder_entry_resize(struct kinder_widget *widget, Sint32 capacity);
void kinder_entry_set_rows(struct kinder_widget *widget, Sint32 rows);
/* set each part whose ENTRY_ bit below is in mask to color, in ARGB 4444 */
void kinder_entry_set_color(struct kinder_widget *widget, Uint16 color,
                            Uint32 mask);
#define ENTRY_TEXT_COLOR 1
#define ENTRY_TEXT_BACKGROUND 2
#define ENTRY_KANA_COLOR 4
#define ENTRY_KANA_BACKGROUND 8
#define ENTRY_ROMAN_COLOR 16
#define ENTRY_ROMAN_BACKGROUND 32
#define ENTRY_FIELD_BACKGROUND 64
Sint32 kinder_entry_height(struct kinder_widget *widget);
Sint32 fun_0c9daf46(struct kinder_widget *widget);
Sint32 fun_0c9daf5e(struct kinder_widget *widget);
Uint16 *kinder_entry_text(struct kinder_widget *widget);
void kinder_entry_set_text(struct kinder_widget *widget, const Uint16 *text);
Uint16 *kinder_frame_text(struct kinder_widget *widget);
void kinder_frame_set_text(struct kinder_widget *widget, const Uint16 *text);
void kinder_frame_set_texlist(struct kinder_widget *widget,
                              NJS_TEXLIST *texlist);
Sint32 kinder_list_key(struct kinder_widget *widget);
void kinder_list_select(struct kinder_widget *widget, Sint32 key);
void kinder_list_set_item(struct kinder_widget *widget, Sint32 index,
                          Sint32 key, const Uint16 *text);
void kinder_list_set_texlist(struct kinder_widget *widget,
                             NJS_TEXLIST *texlist);
void kinder_widget_textures_load(void);
void kinder_widget_textures_release(void);
void kinder_widget_push(struct kinder_widget *widget);
void kinder_widget_remove(struct kinder_widget *widget);
struct kinder_widget *
kinder_widget_create(const struct kinder_widget_desc *desc);
struct kinder_widget *
kinder_widget_alloc(const struct kinder_widget_desc *desc);
void kinder_widget_free(struct kinder_widget *widget);
struct kinder_widget *kinder_widget_add(struct kinder_widget *parent,
                                        const struct kinder_widget_desc *desc);
/* kinder_widget_select focuses the child with that id and returns it, and
 * kinder_widget_find returns it. both return NULL for an id no child has.
 * kinder_widget_focus_at and kinder_widget_drop_at take an index below the
 * child count. a drop destroys the child */
struct kinder_widget *kinder_widget_select(struct kinder_widget *parent,
                                           Sint32 id);

void kinder_widget_focus(struct kinder_widget *parent,
                         struct kinder_widget *child);
void kinder_widget_focus_at(struct kinder_widget *parent, Sint32 index);
struct kinder_widget *kinder_widget_find(struct kinder_widget *parent,
                                         Sint32 id);
void kinder_widget_drop_at(struct kinder_widget *parent, Sint32 index);
void kinder_widget_drop(struct kinder_widget *parent, Sint32 id);

#endif
