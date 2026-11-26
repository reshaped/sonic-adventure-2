/* the kindergarten's forms: a panel and the labels, fields, buttons,
 * rectangles and choices in it. a panel gives each key to its focused child.
 * an accept, cancel, tab or arrow key the child doesn't take reaches the
 * panel's notify as an event */
#ifndef CHAOSTGKINDER_FORM_H
#define CHAOSTGKINDER_FORM_H
#include <sg_xpt.h>

struct kinder_form;

enum form_kind {
    FORM_PANEL,
    FORM_LABEL,
    FORM_FIELD,
    FORM_BUTTON,
    FORM_RECT,
    FORM_CHOICE
};

struct form_box {
    Float x, y, z;
    Float width, height;
    Uint32 id;
};

struct form_field_desc {
    struct form_box box;
    Sint32 rows, capacity;
};

struct form_text_desc {
    struct form_box box;
    Sint32 message;
};

struct form_desc {
    Sint32 kind;
    const void *settings;
};

typedef void form_notify(void *context, struct kinder_form *panel, Sint32 event,
                         Uint32 id);

/* scroll_x and scroll_y follow the focused child. each tick scrolls just far
 * enough to show it with four pixels to spare */
struct form_panel {
    Sint32 capacity, count;
    struct kinder_form **children;
    Sint32 focus;
    Float scroll_x, scroll_y;
    Sint32 show_focus;
    void *context;
    form_notify *notify;
};

struct form_panel_desc {
    struct form_box box;
    Uint32 focus_id;
    Sint32 count;
    const struct form_desc *children;
    form_notify *notify;
};

struct form_choice_entry {
    Sint32 value, message;
};

struct form_choice_desc {
    struct form_box box;
    Sint32 count;
    const struct form_choice_entry *items;
    Sint32 value;
};

struct form_ops {
    void (*destroy)(struct kinder_form *form);
    void (*tick)(struct kinder_form *form);
    void (*draw)(struct kinder_form *form, Float x, Float y, Float z);
    Sint32 (*input)(struct kinder_form *form, Sint32 key);
};

struct kinder_form {
    struct form_box box;
    struct kinder_form *parent;
    Sint32 kind;
    Sint32 visible;
    void *state;
    const struct form_ops *ops;
};

void kinder_forms_start(void);
void kinder_forms_stop(void);
void kinder_form_position(struct kinder_form *form, Float x, Float y, Float z);
void kinder_form_size(struct kinder_form *form, Float width, Float height);
struct kinder_form *kinder_form_create(struct kinder_form *parent,
                                       const struct form_desc *desc);
void kinder_form_poll(struct kinder_form *form);
void kinder_form_show(struct kinder_form *form);
void kinder_form_hide(struct kinder_form *form);
struct kinder_form *form_panel_create(struct kinder_form *parent,
                                      const struct form_panel_desc *desc);
struct kinder_form *form_label_create(struct kinder_form *parent,
                                      const struct form_text_desc *desc);
struct kinder_form *form_field_create(struct kinder_form *parent,
                                      const struct form_field_desc *desc);
struct kinder_form *form_button_create(struct kinder_form *parent,
                                       const struct form_text_desc *desc);
struct kinder_form *form_rect_create(struct kinder_form *parent,
                                     const struct form_box *box);
struct kinder_form *form_choice_create(struct kinder_form *parent,
                                       const struct form_choice_desc *desc);
void form_label_set_text(struct kinder_form *form, const Uint16 *text);
void form_button_set_text(struct kinder_form *form, const Uint16 *text);
/* a panel destroys its children with itself. form_panel_add returns the
 * child it adds, or NULL. form_panel_find returns NULL for an id no child
 * has. form_panel_index_of returns -1 for a form not among them.
 * form_panel_at and form_panel_focus_index take an index below the count */
struct kinder_form *form_panel_add(struct kinder_form *form,
                                   const struct form_desc *desc);
Sint32 form_panel_index_of(struct kinder_form *form, struct kinder_form *child);
void form_panel_notify(struct kinder_form *form, struct kinder_form *child,
                       Sint32 event);
void form_panel_set_callback(struct kinder_form *form, form_notify *notify,
                             void *context);
void form_panel_focus_index(struct kinder_form *form, Sint32 index);
void form_panel_focus_id(struct kinder_form *form, Uint32 id);
struct kinder_form *form_panel_at(struct kinder_form *form, Sint32 index);
struct kinder_form *form_panel_find(struct kinder_form *form, Uint32 id);
Sint32 form_panel_count(struct kinder_form *form);
struct kinder_form **form_panel_children(struct kinder_form *form);
void form_panel_hide_focus(struct kinder_form *form);
Sint32 form_choice_value(struct kinder_form *form);
void form_choice_select(struct kinder_form *form, Sint32 value);
void form_choice_set_item(struct kinder_form *form, Sint32 index, Sint32 value,
                          const Uint16 *text);
void form_field_set_text(struct kinder_form *form, const Uint16 *text);
void form_field_set_limit(struct kinder_form *form, Sint32 limit);
void form_field_resize(struct kinder_form *form, Sint32 capacity);
void form_field_set_rows(struct kinder_form *form, Sint32 rows);
/* set each part whose FIELD_ bit below is in mask to color, in ARGB 4444 */
void form_field_set_color(struct kinder_form *form, Uint16 color, Uint32 mask);
#define FIELD_TEXT_COLOR 1
#define FIELD_TEXT_BACKGROUND 2
#define FIELD_KANA_COLOR 4
#define FIELD_KANA_BACKGROUND 8
#define FIELD_ROMAN_COLOR 16
#define FIELD_ROMAN_BACKGROUND 32
#define FIELD_BACKGROUND 64
void form_field_end(struct kinder_form *form);
void form_field_begin(struct kinder_form *form);
Float form_field_height(struct kinder_form *form);
Sint32 fun_0c9d1b90(struct kinder_form *form);
Sint32 fun_0c9d1ba8(struct kinder_form *form);
Sint32 form_field_mode(struct kinder_form *form);
Uint16 *form_field_text(struct kinder_form *form);
#endif
