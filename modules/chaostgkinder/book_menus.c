#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chaostgkinder/book.h"

#include "chaostgkinder/form.h"
#include "chaostgkinder/keyboard.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/wide_text.h"
#include "chaostgkinder/widget.h"
#include "save/card.h"
#include "system/task.h"
#include "system/util.h"

enum editor_screen {
    EDITOR_START,
    EDITOR_HOME,
    EDITOR_ACTIONS,
    EDITOR_BOOK,
    EDITOR_MARKS,
    EDITOR_MARK,
    EDITOR_REMOVE_MARK,
    EDITOR_PAGES,
    EDITOR_PAGE,
    EDITOR_REMOVE_PAGE,
    EDITOR_SAVE,
    EDITOR_LOAD,
    EDITOR_EXPORT
};

/* book_editor_step's values. a screen makes its panel at STEP_OPEN, and
 * polls it at STEP_POLL. the screens of a mark and a page poll at STEP_EDIT
 * instead. they remove their mark or page at STEP_REMOVE */
enum editor_step { STEP_OPEN, STEP_POLL };
enum item_step { STEP_REMOVE = 1, STEP_EDIT };

/* a page control's id is its group | segment << 8 | index. index is the
 * line or link the control belongs to */
enum page_control_group {
    PAGE_LINE_BUTTON = 0x10000,
    PAGE_SEGMENT_BUTTON = 0x20000,
    PAGE_LINE_CODE = 0x30000,
    PAGE_LINE_TEXT = 0x40000,
    PAGE_LINE_MARK = 0x50000,
    PAGE_LINE_INSERT = 0x60000,
    PAGE_LINK_TEXT = 0x70000,
    PAGE_LINK_INSERT = 0x80000,
    PAGE_LINK_MARK = 0x90000,
    PAGE_LINK_BUTTON = 0xa0000,
    PAGE_LINK_TARGET = 0xb0000,
    PAGE_LINK_KIND = 0xc0000
};

#define PAGE_CONTROL(group, index, segment)                                    \
    (((segment) << 8) | (index) | (group))
/* a mark choice lists EDITOR_MARK_OPTIONS options of its own, then the
 * book's marks. a mark in a text is the code BOOK_TEXT_MARK << 8 | n. n is
 * the number of its option */
#define EDITOR_MARK_OPTIONS 4

/* the ids of each panel's controls, as the panels' descriptions below give
 * them */
enum meter_control { METER_VALUE };
enum home_control { HOME_NEW, HOME_BLANK, HOME_LOAD, HOME_EXIT };
enum actions_control {
    ACTIONS_EDIT,
    ACTIONS_SAVE,
    ACTIONS_EXPORT,
    ACTIONS_EXIT
};
enum property_control {
    PROPERTY_TITLE,
    PROPERTY_AUTHOR,
    PROPERTY_MARKS,
    PROPERTY_PAGES,
    PROPERTY_FIRST_PAGE,
    PROPERTY_SETTING
};
/* a mark's line n is the field MARK_ROW(n) | MARK_LINE and the button
 * MARK_ROW(n) | MARK_REMOVE_LINE. the lines stand ROW_PITCH apart from
 * LINES_TOP */
enum mark_control {
    MARK_TITLE,
    MARK_DELETE,
    MARK_CLOSE,
    MARK_ADD_LINE,
    MARK_LINE,
    MARK_REMOVE_LINE
};
#define ROW_SHIFT 8
#define MARK_ROW(n) ((n) << ROW_SHIFT)
#define LINES_TOP 64
enum removal_control { REMOVE_CONFIRM, REMOVE_CANCEL };
/* the mark and page lists' add button is LIST_ADD. the button of item n, a
 * mark or a page, is LIST_ITEM | n. the items stand ROW_PITCH apart from
 * LIST_TOP */
#define LIST_ADD 0
#define LIST_ITEM 0x100
#define LIST_TOP 80
#define ROW_PITCH 48
/* an id's low byte holds an item's number or a mark control. the byte above
 * it holds a mark line's row */
#define ITEM_MASK 0xffUL
#define LINE_ROW_MASK 0xff00UL
/* a list's buttons, and the pages list's add button, in panel pixels */
#define LIST_LEFT 16
#define LIST_DEPTH 0.001F
#define LIST_BUTTON_WIDTH 288
#define LIST_BUTTON_HEIGHT 32
#define LIST_ADD_TOP 16.0F

/* the text of the first-page choice's one item while the book has no page */
#define NO_PAGE_MESSAGE 0x270
/* the message of the pages list's add button, which new_book and add_mark
 * show too */
#define ADD_MESSAGE 6
/* the codes text holds before its 0 */
#define TEXT_LIMIT(text) (ARRAY_COUNT(text) - 1)

struct editor_form_node {
    struct kinder_form *form;
    struct editor_form_node *next;
};
/* a rule under a list's add button, in a FORM_RECT's settings. it draws in
 * color 0: form_rect_create reads only box, and leaves color unread */
struct editor_rule {
    struct form_box box;
    Uint32 color;
};

static form_notify meter_event, home_event, actions_event, properties_event,
    marks_event, mark_event, remove_mark_event, pages_event;

static enum editor_screen book_editor_screen = EDITOR_START;
static Sint32 book_editor_step = STEP_OPEN;
static struct book *editor_book = NULL;
/* the open panels, newest first, in the order the editor ticks and draws
 * them. a node doesn't own its panel */
static struct editor_form_node *editor_forms = NULL;

static Sint32 file_slot;
static struct book_mark *current_mark;
static Sint32 mark_index;

static struct book_page *editor_page;
static Sint32 editor_page_index;
static struct kinder_form *meter;
/* the codes of the meter's text, the setting choice's settings, and the
 * codes of each one's label */
#define METER_CODES 32
#define SETTING_CHOICES 6
#define SETTING_CODES 4
/* file_slot for a new book, which no file holds */
#define NO_FILE_SLOT (-1)
static Uint16 meter_text[METER_CODES];
static struct kinder_form *home;
static struct kinder_form *actions;
static Uint16 setting_labels[SETTING_CHOICES][SETTING_CODES];
static struct kinder_form *properties, *mark_list, *mark_form;
static Sint32 mark_lines;
static struct kinder_form *remove_prompt, *page_list;

static const Uint16 empty_filename = {
#include "chaostgkinder/book_menus/empty_filename.inc"
};
static const char book_save_filename[13] = {
#include "chaostgkinder/book_menus/book_save_filename.inc"
};
static const struct form_text_desc meter_value = {
#include "chaostgkinder/book_menus/meter_value.inc"
};
static const struct form_desc meter_controls[1] = {
#include "chaostgkinder/book_menus/meter_controls.inc"
};
static const struct form_panel_desc meter_panel = {
#include "chaostgkinder/book_menus/meter_panel.inc"
};
static const struct form_desc meter_desc = {
#include "chaostgkinder/book_menus/meter_desc.inc"
};
static const struct form_text_desc new_book = {
#include "chaostgkinder/book_menus/new_book.inc"
};
static const struct form_text_desc home_blank = {
#include "chaostgkinder/book_menus/home_blank.inc"
};
static const struct form_text_desc load_book = {
#include "chaostgkinder/book_menus/load_book.inc"
};
static const struct form_text_desc exit_home = {
#include "chaostgkinder/book_menus/exit_home.inc"
};
static const struct form_desc home_controls[4] = {
#include "chaostgkinder/book_menus/home_controls.inc"
};
static const struct form_panel_desc home_panel = {
#include "chaostgkinder/book_menus/home_panel.inc"
};
static const struct form_desc home_desc = {
#include "chaostgkinder/book_menus/home_desc.inc"
};
static const struct form_text_desc edit_book = {
#include "chaostgkinder/book_menus/edit_book.inc"
};
static const struct form_text_desc save_book = {
#include "chaostgkinder/book_menus/save_book.inc"
};
static const struct form_text_desc export_book = {
#include "chaostgkinder/book_menus/export_book.inc"
};
static const struct form_text_desc exit_actions = {
#include "chaostgkinder/book_menus/exit_actions.inc"
};
static const struct form_desc action_controls[4] = {
#include "chaostgkinder/book_menus/action_controls.inc"
};
static const struct form_panel_desc actions_panel = {
#include "chaostgkinder/book_menus/actions_panel.inc"
};
static const struct form_desc actions_desc = {
#include "chaostgkinder/book_menus/actions_desc.inc"
};
static const Sint8 book_settings[SETTING_CHOICES] = {
#include "chaostgkinder/book_menus/book_settings.inc"
};
static const struct form_field_desc title_field = {
#include "chaostgkinder/book_menus/title_field.inc"
};
static const struct form_field_desc author_field = {
#include "chaostgkinder/book_menus/author_field.inc"
};
static const struct form_text_desc edit_marks = {
#include "chaostgkinder/book_menus/edit_marks.inc"
};
static const struct form_text_desc edit_pages = {
#include "chaostgkinder/book_menus/edit_pages.inc"
};
static const struct form_choice_desc first_page_choice = {
#include "chaostgkinder/book_menus/first_page_choice.inc"
};
static const struct form_choice_desc setting_choice = {
#include "chaostgkinder/book_menus/setting_choice.inc"
};
static const struct form_text_desc title_label = {
#include "chaostgkinder/book_menus/title_label.inc"
};
static const struct form_text_desc author_label = {
#include "chaostgkinder/book_menus/author_label.inc"
};
static const struct form_text_desc first_page_label = {
#include "chaostgkinder/book_menus/first_page_label.inc"
};
static const struct form_text_desc setting_label = {
#include "chaostgkinder/book_menus/setting_label.inc"
};
static const struct form_desc properties_controls[8] = {
#include "chaostgkinder/book_menus/properties_controls.inc"
};
static const struct form_panel_desc properties_panel = {
#include "chaostgkinder/book_menus/properties_panel.inc"
};
static const struct form_desc properties_desc = {
#include "chaostgkinder/book_menus/properties_desc.inc"
};
static const struct form_text_desc add_mark = {
#include "chaostgkinder/book_menus/add_mark.inc"
};
static const struct editor_rule marks_rule = {
#include "chaostgkinder/book_menus/marks_rule.inc"
};
static const struct form_desc marks_controls[2] = {
#include "chaostgkinder/book_menus/marks_controls.inc"
};
static const struct form_panel_desc marks_panel = {
#include "chaostgkinder/book_menus/marks_panel.inc"
};
static const struct form_desc marks_desc = {
#include "chaostgkinder/book_menus/marks_desc.inc"
};
static const struct form_text_desc mark_title_label = {
#include "chaostgkinder/book_menus/mark_title_label.inc"
};
static const struct form_field_desc mark_title = {
#include "chaostgkinder/book_menus/mark_title.inc"
};
static const struct form_text_desc delete_mark = {
#include "chaostgkinder/book_menus/delete_mark.inc"
};
static const struct form_text_desc close_mark = {
#include "chaostgkinder/book_menus/close_mark.inc"
};
static const struct form_text_desc add_mark_line = {
#include "chaostgkinder/book_menus/add_mark_line.inc"
};
static const struct form_field_desc mark_line = {
#include "chaostgkinder/book_menus/mark_line.inc"
};
static const struct form_text_desc remove_line = {
#include "chaostgkinder/book_menus/remove_line.inc"
};
static const struct form_desc mark_controls[5] = {
#include "chaostgkinder/book_menus/mark_controls.inc"
};
static const struct form_panel_desc mark_panel = {
#include "chaostgkinder/book_menus/mark_panel.inc"
};
static const struct form_desc mark_desc = {
#include "chaostgkinder/book_menus/mark_desc.inc"
};
static const struct form_text_desc remove_mark_title = {
#include "chaostgkinder/book_menus/remove_mark_title.inc"
};
static const struct form_text_desc remove_mark_warning = {
#include "chaostgkinder/book_menus/remove_mark_warning.inc"
};
static const struct form_text_desc confirm_remove = {
#include "chaostgkinder/book_menus/confirm_remove.inc"
};
static const struct form_text_desc cancel_remove = {
#include "chaostgkinder/book_menus/cancel_remove.inc"
};
static const struct form_desc remove_controls[4] = {
#include "chaostgkinder/book_menus/remove_controls.inc"
};
static const struct form_panel_desc remove_panel = {
#include "chaostgkinder/book_menus/remove_panel.inc"
};
static const struct form_desc remove_desc = {
#include "chaostgkinder/book_menus/remove_desc.inc"
};
static const struct form_text_desc page_button = {
#include "chaostgkinder/book_menus/page_button.inc"
};
static const struct editor_rule pages_rule = {
#include "chaostgkinder/book_menus/pages_rule.inc"
};
static const struct form_desc pages_controls[1] = {
#include "chaostgkinder/book_menus/pages_controls.inc"
};
static const struct form_panel_desc pages_panel = {
#include "chaostgkinder/book_menus/pages_panel.inc"
};
static const struct form_desc pages_desc = {
#include "chaostgkinder/book_menus/pages_desc.inc"
};

static void editor_form_push(struct kinder_form *form)
{
    struct editor_form_node *node = syCalloc(1, sizeof(*node));
#ifdef NONMATCHING
    /* this side pushes no node where the heap has none or there is no form */
    if (!node || !form) {
        if (node) {
            syFree(node);
        }
        return;
    }
#endif
    node->form = form;
    node->next = editor_forms;
    editor_forms = node;
}

static void editor_form_remove(struct kinder_form *form)
{
    struct editor_form_node *node = editor_forms;
    struct editor_form_node *previous;
    if (!node) {
        return;
    }
    if (node->form == form) {
        editor_forms = node->next;
        syFree(node);
    } else {
        for (previous = node; previous->next; previous = previous->next) {
            if (previous->next->form == form) {
                struct editor_form_node *found = previous->next;
                previous->next = found->next;
                syFree(found);
                break;
            }
        }
    }
}

static void editor_forms_tick(void)
{
    struct editor_form_node *node;
    struct kinder_form *form;
    for (node = editor_forms; node; node = node->next) {
        form = node->form;
        form->ops->tick(form);
    }
}

static void editor_forms_draw(void)
{
    struct editor_form_node *node;
    struct kinder_form *form;
    for (node = editor_forms; node; node = node->next) {
        form = node->form;
        form->ops->draw(form, 0, 0, 0);
    }
}

static void editor_forms_clear(void)
{
    struct editor_form_node *node;
    struct editor_form_node *next;
    for (node = editor_forms; node; node = next) {
        next = node->next;
        syFree(node);
    }
    editor_forms = NULL;
}

static void meter_event(void *context, struct kinder_form *form, Sint32 event,
                        Uint32 key)
{
}

static void editor_meter_open(void)
{
    meter = kinder_form_create(NULL, &meter_desc);
#ifdef NONMATCHING
    /* this side opens no meter where kinder_form_create makes none */
    if (!meter) {
        return;
    }
#endif
    form_panel_hide_focus(meter);
    editor_form_push(meter);
}

static void editor_meter_close(void)
{
#ifdef NONMATCHING
    /* this side closes no meter that didn't open */
    if (!meter) {
        return;
    }
#endif
    editor_form_remove(meter);
    meter->ops->destroy(meter);
    meter = NULL;
}

static void editor_meter_update(void)
{
    Sint32 remaining = BOOK_EXPORT_BYTES;
    Uint16 *suffix;
    struct kinder_form *label;
    if (editor_book) {
        remaining -= book_export_size(editor_book);
    }
    kinder_int_to_text(meter_text, remaining);
    suffix = &meter_text[kinder_wide_length(meter_text)];
    /* @hack spelling: the codes of " bytes free" stored one at a time, where
     * unpacking the string would do */
    *suffix++ = ' ';
    *suffix++ = 'b';
    *suffix++ = 'y';
    *suffix++ = 't';
    *suffix++ = 'e';
    *suffix++ = 's';
    *suffix++ = ' ';
    *suffix++ = 'f';
    *suffix++ = 'r';
    *suffix++ = 'e';
    *suffix++ = 'e';
    *suffix = 0;
#ifdef NONMATCHING
    /* this side updates no meter that didn't open */
    if (!meter) {
        return;
    }
#endif
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((label = form_panel_find(meter, METER_VALUE)) != NULL) {
        form_label_set_text(label, meter_text);
    }
}

/* @hack spelling: CLOSE_PANEL's and GO_TO's two statements bare, where a
 * do-while around each pair would do */
#define CLOSE_PANEL(form)                                                      \
    editor_form_remove(form);                                                  \
    (form)->ops->destroy(form)
#define GO_TO(screen)                                                          \
    book_editor_screen = (screen);                                             \
    book_editor_step = STEP_OPEN
/* PREVIOUS_OF_FOUR and NEXT_OF_FOUR step id to its neighbor among ids 0 to
 * 3, wrapping */
#define PREVIOUS_OF_FOUR(id)                                                   \
    switch (id) {                                                              \
    case 0:                                                                    \
        (id) = 3;                                                              \
        break;                                                                 \
    case 1:                                                                    \
        (id) = 0;                                                              \
        break;                                                                 \
    case 2:                                                                    \
        (id) = 1;                                                              \
        break;                                                                 \
    case 3:                                                                    \
        (id) = 2;                                                              \
        break;                                                                 \
    }
#define NEXT_OF_FOUR(id)                                                       \
    switch (id) {                                                              \
    case 0:                                                                    \
        (id) = 1;                                                              \
        break;                                                                 \
    case 1:                                                                    \
        (id) = 2;                                                              \
        break;                                                                 \
    case 2:                                                                    \
        (id) = 3;                                                              \
        break;                                                                 \
    case 3:                                                                    \
        (id) = 0;                                                              \
        break;                                                                 \
    }

static void home_event(void *context, struct kinder_form *form, Sint32 event,
                       Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
        switch (key) {
        case HOME_NEW:
            editor_book = book_create();
#ifdef NONMATCHING
            /* this side stays home where book_create makes no book */
            if (!editor_book) {
                return;
            }
#endif
            file_slot = NO_FILE_SLOT;
            CLOSE_PANEL(form);
            GO_TO(EDITOR_BOOK);
            break;
        case HOME_BLANK:
            break;
        case HOME_LOAD:
            CLOSE_PANEL(form);
            GO_TO(EDITOR_LOAD);
            break;
        case HOME_EXIT:
            CLOSE_PANEL(form);
            kinder_request_room(KINDER_LIBRARY, 0);
            break;
        }
        break;
    case WIDGET_UP:
    case WIDGET_BACK_TAB:
        PREVIOUS_OF_FOUR(key);
        form_panel_focus_id(form, key);
        break;
    case WIDGET_DOWN:
    case WIDGET_TAB:
        NEXT_OF_FOUR(key);
        form_panel_focus_id(form, key);
        break;
    }
}

static void editor_home_update(void)
{
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((home = kinder_form_create(NULL, &home_desc)) != NULL) {
            editor_form_push(home);
            book_editor_step = STEP_POLL;
        }
        break;
    case STEP_POLL:
    default:
        kinder_form_poll(home);
        break;
    }
}

static void actions_event(void *context, struct kinder_form *form, Sint32 event,
                          Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
        switch (key) {
        default:
        case ACTIONS_EDIT:
            CLOSE_PANEL(actions);
            GO_TO(EDITOR_BOOK);
            break;
        case ACTIONS_SAVE:
            CLOSE_PANEL(actions);
            GO_TO(EDITOR_SAVE);
            break;
        case ACTIONS_EXPORT:
            CLOSE_PANEL(actions);
            GO_TO(EDITOR_EXPORT);
            break;
        case ACTIONS_EXIT:
            CLOSE_PANEL(actions);
            kinder_request_room(KINDER_LIBRARY, 0);
            break;
        }
        break;
    case WIDGET_CANCEL:
        CLOSE_PANEL(actions);
        GO_TO(EDITOR_BOOK);
        break;
    case WIDGET_UP:
    case WIDGET_BACK_TAB:
        PREVIOUS_OF_FOUR(key);
        form_panel_focus_id(form, key);
        break;
    case WIDGET_DOWN:
    case WIDGET_TAB:
        NEXT_OF_FOUR(key);
        form_panel_focus_id(form, key);
        break;
    }
}

static void editor_actions_update(void)
{
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((actions = kinder_form_create(NULL, &actions_desc)) != NULL) {
            book_editor_step = STEP_POLL;
            editor_form_push(actions);
        }
        break;
    case STEP_POLL:
    default:
        kinder_form_poll(actions);
        break;
    }
}

static void read_properties(void)
{
    struct kinder_form *control;
    control = form_panel_find(properties, PROPERTY_TITLE);
    kinder_wide_terminate(editor_book->title, form_field_text(control),
                          TEXT_LIMIT(editor_book->title));
    control = form_panel_find(properties, PROPERTY_AUTHOR);
    kinder_wide_terminate(editor_book->author, form_field_text(control),
                          TEXT_LIMIT(editor_book->author));
    control = form_panel_find(properties, PROPERTY_FIRST_PAGE);
    editor_book->first_page = form_choice_value(control);
    control = form_panel_find(properties, PROPERTY_SETTING);
    editor_book->setting = book_settings[form_choice_value(control)];
}

static void properties_event(void *context, struct kinder_form *form,
                             Sint32 event, Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
        switch (key) {
        case PROPERTY_MARKS:
            read_properties();
            CLOSE_PANEL(form);
            GO_TO(EDITOR_MARKS);
            break;
        case PROPERTY_PAGES:
            read_properties();
            CLOSE_PANEL(form);
            GO_TO(EDITOR_PAGES);
            break;
        }
        break;
    case WIDGET_CANCEL:
        read_properties();
        CLOSE_PANEL(form);
        GO_TO(EDITOR_ACTIONS);
        break;
    case WIDGET_UP:
    case WIDGET_BACK_TAB:
        switch (key) {
        case PROPERTY_TITLE:
            key = PROPERTY_SETTING;
            break;
        case PROPERTY_AUTHOR:
            key = PROPERTY_TITLE;
            break;
        case PROPERTY_MARKS:
            key = PROPERTY_AUTHOR;
            break;
        case PROPERTY_PAGES:
            key = PROPERTY_MARKS;
            break;
        case PROPERTY_FIRST_PAGE:
            key = PROPERTY_PAGES;
            break;
        case PROPERTY_SETTING:
            key = PROPERTY_FIRST_PAGE;
            break;
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_DOWN:
    case WIDGET_TAB:
        switch (key) {
        case PROPERTY_TITLE:
            key = PROPERTY_AUTHOR;
            break;
        case PROPERTY_AUTHOR:
            key = PROPERTY_MARKS;
            break;
        case PROPERTY_MARKS:
            key = PROPERTY_PAGES;
            break;
        case PROPERTY_PAGES:
            key = PROPERTY_FIRST_PAGE;
            break;
        case PROPERTY_FIRST_PAGE:
            key = PROPERTY_SETTING;
            break;
        case PROPERTY_SETTING:
            key = PROPERTY_TITLE;
            break;
        }
        form_panel_focus_id(form, key);
        break;
    }
}
static void editor_book_update(void)
{
    struct form_desc desc;
    struct form_choice_desc choice;
    struct kinder_form *control;
    Sint32 i;
    Uint32 selected;
    Sint32 option;
    Sint32 j;
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((properties = kinder_form_create(NULL, &properties_desc)) != NULL) {
            editor_form_push(properties);
            form_field_set_text(form_panel_find(properties, PROPERTY_TITLE),
                                editor_book->title);
            form_field_set_text(form_panel_find(properties, PROPERTY_AUTHOR),
                                editor_book->author);
            desc.kind = FORM_CHOICE;
            desc.settings = &choice;
            choice = first_page_choice;
            if (editor_book->page_count > 0) {
                choice.count = editor_book->page_count;
                control = form_panel_add(properties, &desc);
                for (i = 0; i < editor_book->page_count; i++) {
                    form_choice_set_item(control, i, i,
                                         editor_book->pages[i]->title);
                }
                /* @bug logic: the first-page choice opens on page 0, whatever
                 * first_page holds, and leaving the panel sets first_page to
                 * 0 unless the player picks again. form_choice_select at
                 * 0x0c9c33cc takes i, which the loop leaves at page_count, a
                 * value no item has. read_properties stores the choice at
                 * 0x0c9c30ba */
                form_choice_select(control, i);
            } else {
                choice.count = 1;
                control = form_panel_add(properties, &desc);
                form_choice_set_item(control, 0, 0,
                                     kinder_string(NO_PAGE_MESSAGE));
                form_choice_select(control, 0);
            }
            for (j = 0; (unsigned)j < sizeof(book_settings); j++) {
                kinder_int_to_text(setting_labels[j], book_settings[j]);
            }
            /* @hack redundant: desc's kind and settings set again, to the
             * values they hold */
            desc.kind = FORM_CHOICE;
            desc.settings = &choice;
            choice = setting_choice;
            choice.count = sizeof(book_settings);
            control = form_panel_add(properties, &desc);
#ifdef NONMATCHING
            /* this side starts selected at 0, so that a setting no option
             * matches selects the first */
            selected = 0;
#endif
            for (option = 0; (unsigned)option < sizeof(book_settings);
                 option++) {
                form_choice_set_item(control, option, option,
                                     setting_labels[option]);
                if (editor_book->setting >= book_settings[option]) {
                    selected = option;
                }
            }
            form_choice_select(control, selected);
            book_editor_step = STEP_POLL;
        }
        break;
    case STEP_POLL:
    default:
        kinder_form_poll(properties);
        break;
    }
}

static void marks_event(void *context, struct kinder_form *form, Sint32 event,
                        Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
        if (key == LIST_ADD) {
            mark_index = editor_book->mark_count;
            current_mark = book_add_mark(editor_book);
#ifdef NONMATCHING
            /* this side stays on the marks where book_add_mark makes no mark */
            if (!current_mark) {
                return;
            }
#endif
            /* @hack spelling: the goto into the other branch's tail, where
             * each branch's own CLOSE_PANEL and GO_TO would do */
            goto open;
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            mark_index = (Uint8)key;
            current_mark = editor_book->marks[mark_index];
        open:
            CLOSE_PANEL(form);
            GO_TO(EDITOR_MARK);
        }
        break;
    case WIDGET_CANCEL:
        CLOSE_PANEL(form);
        GO_TO(EDITOR_BOOK);
        break;
    case WIDGET_DOWN:
    case WIDGET_TAB:
        if (key == LIST_ADD) {
            key = LIST_ITEM;
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            key++;
            if ((key & ITEM_MASK) >= (Uint32)editor_book->mark_count) {
                key &= ~ITEM_MASK;
            }
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_UP:
    case WIDGET_BACK_TAB:
        /* @hack redundant: the chain's empty else */
        if (key == LIST_ADD) {
            key = (editor_book->mark_count - 1) | LIST_ITEM;
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            if ((Uint8)key) {
                key--;
            } else {
                key = LIST_ADD;
            }
        } else {
        }
        form_panel_focus_id(form, key);
        break;
    }
}

static void editor_marks_update(void)
{
    struct form_desc desc;
    struct form_text_desc button;
    struct kinder_form *control;
    Sint32 i;
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((mark_list = kinder_form_create(NULL, &marks_desc)) != NULL) {
            editor_form_push(mark_list);
            /* @hack spelling: the step advanced by ++, where = STEP_POLL
             * would do */
            book_editor_step++;
            desc.kind = FORM_BUTTON;
            desc.settings = &button;
            button.box.x = LIST_LEFT;
            button.box.z = LIST_DEPTH;
            button.box.width = LIST_BUTTON_WIDTH;
            button.box.height = LIST_BUTTON_HEIGHT;
#ifdef NONMATCHING
            /* this side starts the button's message at 0, which nothing else
             * sets */
            button.message = 0;
#endif
            for (i = 0; i < editor_book->mark_count; ++i) {
                button.box.y = (i * ROW_PITCH) + LIST_TOP;
                button.box.id = i | LIST_ITEM;
                control = form_panel_add(mark_list, &desc);
                form_button_set_text(control, editor_book->marks[i]->title);
            }
        }
        break;
    default:
        kinder_form_poll(mark_list);
        break;
    }
}

static Sint32 mark_is_valid(void)
{
    return 1;
}

static void read_mark_fields(void)
{
    struct kinder_form *control;
    Sint32 i;
#ifdef NONMATCHING
    /* build the new mark whole first, so a failed allocation keeps the old */
    struct book_mark *replacement = book_mark_create();
    if (!replacement) {
        return;
    }
    for (i = 0; i < mark_lines; i++) {
        replacement->lines[i] = book_mark_line_create();
        if (!replacement->lines[i]) {
            replacement->line_count = i;
            book_mark_destroy(replacement);
            return;
        }
    }
    book_mark_destroy(current_mark);
    editor_book->marks[mark_index] = current_mark = replacement;
#else
    book_mark_destroy(current_mark);
    editor_book->marks[mark_index] = current_mark = book_mark_create();
#endif
    control = form_panel_find(mark_form, MARK_TITLE);
    kinder_wide_terminate(current_mark->title, form_field_text(control),
                          TEXT_LIMIT(current_mark->title));
    current_mark->line_count = mark_lines;
    for (i = 0; i < mark_lines; i++) {
        control = form_panel_find(mark_form, MARK_ROW(i) | MARK_LINE);
#ifdef NONMATCHING
#else
        current_mark->lines[i] = book_mark_line_create();
#endif
        kinder_wide_terminate(current_mark->lines[i]->text,
                              form_field_text(control),
                              TEXT_LIMIT(current_mark->lines[i]->text));
    }
}

static void close_mark_editor(void)
{
    CLOSE_PANEL(mark_form);
    GO_TO(EDITOR_MARKS);
}

static void layout_mark(void)
{
    struct kinder_form *control;
    Float y = LINES_TOP;
    Sint32 i;
    for (i = 0; i < mark_lines; i++) {
        control = form_panel_find(mark_form, MARK_ROW(i) | MARK_LINE);
        control->box.y = y;
        control = form_panel_find(mark_form, MARK_ROW(i) | MARK_REMOVE_LINE);
        y += ROW_PITCH;
        control->box.y = (i * ROW_PITCH) + LINES_TOP;
    }
    control = form_panel_find(mark_form, MARK_ADD_LINE);
    control->box.y = y;
    y += ROW_PITCH;
    control = form_panel_find(mark_form, MARK_DELETE);
    control->box.y = y;
    control = form_panel_find(mark_form, MARK_CLOSE);
    control->box.y = y;
}

static void finish_mark(void)
{
    if (mark_is_valid()) {
        read_mark_fields();
        close_mark_editor();
    }
}

static void mark_event(void *context, struct kinder_form *form, Sint32 event,
                       Uint32 key)
{
    struct form_desc field_desc;
    struct form_field_desc field;
    struct form_desc button_desc;
    struct form_text_desc button;
    struct form_panel *panel;
    struct kinder_form *item;
    Uint32 row;
    Uint32 row_bits;
    Sint32 i;
    Sint32 kept;
    Sint32 next;
    switch (event) {
    case WIDGET_ACCEPT:
        switch ((Uint8)key) {
        case MARK_DELETE:
            GO_TO(EDITOR_REMOVE_MARK);
            break;
        case MARK_CLOSE:
            finish_mark();
            break;
        case MARK_ADD_LINE:
            /* @hack spelling: the empty branch and the else, where
             * if (mark_lines < BOOK_MARK_LINES) would do */
            if (mark_lines >= BOOK_MARK_LINES) {
            } else {
                field_desc.kind = FORM_FIELD;
                field_desc.settings = &field;
                field = mark_line;
                button_desc.kind = FORM_BUTTON;
                button_desc.settings = &button;
                button = remove_line;
                row = mark_lines++;
                field.box.id = MARK_ROW(row) | MARK_LINE;
                form_panel_add(form, &field_desc);
                button.box.id = MARK_ROW(row) | MARK_REMOVE_LINE;
                form_panel_add(form, &button_desc);
                layout_mark();
            }
            break;
        case MARK_REMOVE_LINE:
            row_bits = key & LINE_ROW_MASK;
            panel = mark_form->state;
            kept = 0;
            for (i = 0; i < panel->count; i++) {
                item = panel->children[i];
                switch ((Uint8)item->box.id) {
                case MARK_LINE:
                case MARK_REMOVE_LINE:
                    if ((item->box.id & LINE_ROW_MASK) == row_bits) {
                        item->ops->destroy(item);
                        item = NULL;
                    } else if (item->box.id > row_bits) {
                        item->box.id -= MARK_ROW(1);
                    }
                }
                /* @hack spelling: the empty branch and the else, where
                 * if (item) would do */
                if (!item) {
                } else {
                    panel->children[kept++] = item;
                }
            }
            panel->count = kept;
            mark_lines--;
            layout_mark();
            break;
        }
        break;
    case WIDGET_CANCEL:
        finish_mark();
        break;
    case WIDGET_DOWN:
        if (key == MARK_TITLE) {
            if (mark_lines > 0) {
                key = MARK_ROW(0) | MARK_LINE;
            } else {
                key = MARK_ADD_LINE;
            }
        } else if (key == MARK_DELETE || key == MARK_CLOSE) {
            key = MARK_TITLE;
        } else if (key == MARK_ADD_LINE) {
            key = MARK_CLOSE;
        } else if ((key & ITEM_MASK) == MARK_LINE ||
                   (key & ITEM_MASK) == MARK_REMOVE_LINE) {
            next = key >> ROW_SHIFT;
            next++;
            if (next < mark_lines) {
                key = MARK_ROW(next) | (key & ITEM_MASK);
            } else {
                key = MARK_ADD_LINE;
            }
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_UP:
        if (key == MARK_TITLE) {
            key = MARK_CLOSE;
        } else if (key == MARK_DELETE || key == MARK_CLOSE) {
            key = MARK_ADD_LINE;
        } else if (key == MARK_ADD_LINE) {
            if (mark_lines > 0) {
                key = MARK_ROW(mark_lines - 1) | MARK_LINE;
            } else {
                key = MARK_TITLE;
            }
        } else if ((key & ITEM_MASK) == MARK_LINE ||
                   (key & ITEM_MASK) == MARK_REMOVE_LINE) {
            if (key & ~ITEM_MASK) {
                key -= MARK_ROW(1);
            } else {
                key = MARK_TITLE;
            }
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_LEFT:
        if (key == MARK_CLOSE) {
            key = MARK_DELETE;
        } else if ((key & ITEM_MASK) == MARK_REMOVE_LINE) {
            key = (key & ~ITEM_MASK) | MARK_LINE;
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_RIGHT:
        if (key == MARK_DELETE) {
            key = MARK_CLOSE;
        } else if ((key & ITEM_MASK) == MARK_LINE) {
            key = (key & ~ITEM_MASK) | MARK_REMOVE_LINE;
        }
        form_panel_focus_id(form, key);
        break;
    }
}

static void editor_mark_update(void)
{
    struct form_desc field_desc;
    struct form_field_desc field;
    struct form_desc button_desc;
    struct form_text_desc button;
    struct kinder_form *control;
    Sint32 i;
    Sint32 row;
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((mark_form = kinder_form_create(NULL, &mark_desc)) != NULL) {
            editor_form_push(mark_form);
            book_editor_step = STEP_EDIT;
            form_field_set_text(form_panel_find(mark_form, MARK_TITLE),
                                current_mark->title);
            mark_lines = current_mark->line_count;
            field_desc.kind = FORM_FIELD;
            field_desc.settings = &field;
            field = mark_line;
            button_desc.kind = FORM_BUTTON;
            button_desc.settings = &button;
            button = remove_line;
            for (i = 0; i < current_mark->line_count; i++) {
                /* @hack spelling: the assignment inside the id's expression,
                 * where a statement of its own would do */
                field.box.id = (row = MARK_ROW(i)) | MARK_LINE;
                control = form_panel_add(mark_form, &field_desc);
                form_field_set_text(control, current_mark->lines[i]->text);
                button.box.id = row | MARK_REMOVE_LINE;
                form_panel_add(mark_form, &button_desc);
            }
            layout_mark();
        }
        break;
    case STEP_REMOVE:
        book_remove_mark(editor_book, mark_index);
        CLOSE_PANEL(mark_form);
        GO_TO(EDITOR_MARKS);
        break;
    case STEP_EDIT:
    default:
        kinder_form_poll(mark_form);
        break;
    }
}

static void remove_mark_event(void *context, struct kinder_form *form,
                              Sint32 event, Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        CLOSE_PANEL(form);
        book_editor_screen = EDITOR_MARK;
        book_editor_step = event == WIDGET_ACCEPT && key == REMOVE_CONFIRM
                               ? STEP_REMOVE
                               : STEP_EDIT;
        break;
    case WIDGET_RIGHT:
        form_panel_focus_id(form, REMOVE_CANCEL);
        break;
    case WIDGET_LEFT:
        form_panel_focus_id(form, REMOVE_CONFIRM);
        break;
    case WIDGET_TAB:
    case WIDGET_BACK_TAB:
        form_panel_focus_id(form, key == REMOVE_CANCEL ? REMOVE_CONFIRM
                                                       : REMOVE_CANCEL);
        break;
    }
}

static void editor_remove_mark_update(void)
{
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((remove_prompt = kinder_form_create(NULL, &remove_desc)) != NULL) {
            editor_form_push(remove_prompt);
            /* @hack spelling: the step advanced by ++, where = STEP_POLL
             * would do */
            book_editor_step++;
        }
        break;
    default:
        kinder_form_poll(remove_prompt);
        break;
    }
}

static void pages_event(void *context, struct kinder_form *form, Sint32 event,
                        Uint32 key)
{
    Uint32 index;
    switch (event) {
    case WIDGET_ACCEPT:
        if (key == LIST_ADD) {
            editor_page_index = editor_book->page_count;
            editor_page = book_add_page(editor_book);
#ifdef NONMATCHING
            /* this side stays on the pages where book_add_page makes no page */
            if (!editor_page) {
                return;
            }
#endif
            /* @hack spelling: the goto into the other branch's tail, where
             * each branch's own CLOSE_PANEL and GO_TO would do */
            goto open;
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            index = (Uint8)key;
            editor_page_index = index;
            editor_page = editor_book->pages[editor_page_index];
        open:
            CLOSE_PANEL(form);
            GO_TO(EDITOR_PAGE);
        }
        break;
    case WIDGET_CANCEL:
        CLOSE_PANEL(form);
        GO_TO(EDITOR_BOOK);
        break;
    case WIDGET_UP:
    case WIDGET_BACK_TAB:
        if (key == LIST_ADD) {
            if (editor_book->page_count > 0) {
                key = (editor_book->page_count - 1) | LIST_ITEM;
            }
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            index = (Uint8)key;
            if (index == 0) {
                key = LIST_ADD;
            } else {
                key = ((key & ITEM_MASK) - 1) | LIST_ITEM;
            }
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_DOWN:
    case WIDGET_TAB:
        if (key == LIST_ADD) {
            if (editor_book->page_count > 0) {
                key = LIST_ITEM;
            }
        } else if ((key & ~ITEM_MASK) == LIST_ITEM) {
            Sint32 next = ((Sint32)key & (Sint32)ITEM_MASK) + 1;
            if (next < editor_book->page_count) {
                key = next | LIST_ITEM;
            } else {
                key = LIST_ADD;
            }
        }
        form_panel_focus_id(form, key);
        break;
    }
}

static void editor_pages_update(void)
{
    struct form_desc desc;
    struct form_text_desc button;
    struct kinder_form *control;
    Sint32 i;
    switch (book_editor_step) {
    case STEP_OPEN:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((page_list = kinder_form_create(NULL, &pages_desc)) != NULL) {
            editor_form_push(page_list);
            book_editor_step = STEP_POLL;
            desc.kind = FORM_BUTTON;
            desc.settings = &button;
            button = page_button;
            button.box.y = LIST_ADD_TOP;
            button.box.id = LIST_ADD;
            button.message = ADD_MESSAGE;
            form_panel_add(page_list, &desc);
            form_panel_focus_id(page_list, LIST_ADD);
            button.message = 0;
            for (i = 0; i < editor_book->page_count; i++) {
                button.box.y = (i * ROW_PITCH) + LIST_TOP;
                button.box.id = i | LIST_ITEM;
                control = form_panel_add(page_list, &desc);
                form_button_set_text(control, editor_book->pages[i]->title);
            }
        }
        break;
    default:
        kinder_form_poll(page_list);
        break;
    }
}

static form_notify page_editor_event;

static const Uint8 page_link_kinds[10] = {
#include "chaostgkinder/book_menus/page_link_kinds.inc"
};
static const struct form_text_desc page_title_label = {
#include "chaostgkinder/book_menus/page_title_label.inc"
};
static const struct form_field_desc page_title_field = {
#include "chaostgkinder/book_menus/page_title_field.inc"
};

static const struct form_choice_entry ratings[5] = {
#include "chaostgkinder/book_menus/ratings.inc"
};
static const struct form_text_desc first_option_label = {
#include "chaostgkinder/book_menus/first_option_label.inc"
};
static const struct form_choice_desc first_option = {
#include "chaostgkinder/book_menus/first_option.inc"
};
static const struct form_text_desc second_option_label = {
#include "chaostgkinder/book_menus/second_option_label.inc"
};
static const struct form_choice_desc second_option = {
#include "chaostgkinder/book_menus/second_option.inc"
};
static const struct form_text_desc third_option_label = {
#include "chaostgkinder/book_menus/third_option_label.inc"
};
static const struct form_choice_desc third_option = {
#include "chaostgkinder/book_menus/third_option.inc"
};
static const struct form_text_desc fourth_option_label = {
#include "chaostgkinder/book_menus/fourth_option_label.inc"
};
static const struct form_choice_desc fourth_option = {
#include "chaostgkinder/book_menus/fourth_option.inc"
};
static const struct form_choice_entry fifth_values[10] = {
#include "chaostgkinder/book_menus/fifth_values.inc"
};
static const struct form_choice_desc fifth_option = {
#include "chaostgkinder/book_menus/fifth_option.inc"
};
static const struct form_text_desc page_line_button = {
#include "chaostgkinder/book_menus/page_line_button.inc"
};
static const struct form_text_desc page_segment_button = {
#include "chaostgkinder/book_menus/page_segment_button.inc"
};
static const struct form_text_desc page_line_insert = {
#include "chaostgkinder/book_menus/page_line_insert.inc"
};
static const struct form_choice_entry line_codes[23] = {
#include "chaostgkinder/book_menus/line_codes.inc"
};
static const struct form_choice_desc page_line_codes = {
#include "chaostgkinder/book_menus/page_line_codes.inc"
};
static const struct form_choice_desc page_line_marks = {
#include "chaostgkinder/book_menus/page_line_marks.inc"
};
static const struct form_field_desc page_line_field = {
#include "chaostgkinder/book_menus/page_line_field.inc"
};
static const struct form_text_desc add_line = {
#include "chaostgkinder/book_menus/add_line.inc"
};
static const struct form_choice_entry link_styles[6] = {
#include "chaostgkinder/book_menus/link_styles.inc"
};
static const struct form_choice_desc link_style = {
#include "chaostgkinder/book_menus/link_style.inc"
};
static const struct form_choice_desc link_target = {
#include "chaostgkinder/book_menus/link_target.inc"
};
static const struct form_choice_desc link_kind = {
#include "chaostgkinder/book_menus/link_kind.inc"
};
static const struct form_field_desc page_link_field = {
#include "chaostgkinder/book_menus/page_link_field.inc"
};
static const struct form_text_desc page_link_insert = {
#include "chaostgkinder/book_menus/page_link_insert.inc"
};
static const struct form_text_desc page_link_button = {
#include "chaostgkinder/book_menus/page_link_button.inc"
};
static const struct form_choice_desc page_link_marks = {
#include "chaostgkinder/book_menus/page_link_marks.inc"
};
static const struct form_text_desc remove_page = {
#include "chaostgkinder/book_menus/remove_page.inc"
};
static const struct form_text_desc close_page = {
#include "chaostgkinder/book_menus/close_page.inc"
};
static const struct form_desc page_controls[14] = {
#include "chaostgkinder/book_menus/page_controls.inc"
};
static const struct form_panel_desc page_panel = {
#include "chaostgkinder/book_menus/page_panel.inc"
};
static const struct form_desc page_editor_desc = {
#include "chaostgkinder/book_menus/page_editor_desc.inc"
};

static struct kinder_form *page_editor_form;
static Sint32 page_editor_lines;
static Sint32 page_line_segments[BOOK_PAGE_LINES];
static Sint32 page_link_style;
static Sint32 page_link_segments[BOOK_PAGE_LINES];
static Uint16 page_kind_labels[10][4];

#define PAGE_FIND(id) form_panel_find(page_editor_form, (id))

static void page_editor_layout(void)
{
    struct kinder_form *control;
    Float y;
    int line;
    int segment;
    int link;
    y = 184.0F;
    for (line = 0; line < page_editor_lines; line++) {
        PAGE_FIND(line | PAGE_LINE_BUTTON)->box.y = y;
        PAGE_FIND(line | PAGE_LINE_CODE)->box.y = y;
        for (segment = 0; segment < page_line_segments[line]; segment++) {
            PAGE_FIND(PAGE_CONTROL(PAGE_SEGMENT_BUTTON, line, segment))->box.y =
                y;
            PAGE_FIND(PAGE_CONTROL(PAGE_LINE_TEXT, line, segment))->box.y = y;
            PAGE_FIND(PAGE_CONTROL(PAGE_LINE_INSERT, line, segment))->box.y = y;
            if (segment != 0) {
                PAGE_FIND(PAGE_CONTROL(PAGE_LINE_MARK, line, segment))->box.y =
                    y;
            }
            y += PAGE_FIND(PAGE_CONTROL(PAGE_LINE_TEXT, line, segment))
                     ->box.height +
                 2.0F;
        }
        y += 14.0F;
    }
    PAGE_FIND(5)->box.y = y;
    y += 40;
    PAGE_FIND(6)->box.y = y;
    y += 40;
    switch (page_link_style) {
    default:
    case 2:
    case 3:
        for (link = 0; link < 2; link++) {
            control = PAGE_FIND(link | PAGE_LINK_TARGET);
            kinder_form_hide(control);
            control = PAGE_FIND(link | PAGE_LINK_KIND);
            kinder_form_hide(control);
        }
        break;
    case 0:
    case 4:
    case 5:
        control = PAGE_FIND(PAGE_LINK_TARGET);
        kinder_form_show(control);
        control->box.y = y;
        control = PAGE_FIND(PAGE_LINK_KIND);
        kinder_form_show(control);
        control->box.y = y;
        y += 40;
        control = PAGE_FIND(PAGE_LINK_TARGET | 1);
        kinder_form_hide(control);
        control = PAGE_FIND(PAGE_LINK_KIND | 1);
        kinder_form_hide(control);
        break;
    case 1:
        break;
    }
    switch (page_link_style) {
    default:
    case 2:
    case 3:
    case 0:
    case 4:
    case 5:
        for (line = 0; line < 2; line++) {
            for (segment = 0; segment < page_link_segments[line]; segment++) {
                if (segment != 0) {
                    control = PAGE_FIND(
                        PAGE_CONTROL(PAGE_LINK_BUTTON, line, segment));
                    kinder_form_hide(control);
                    control =
                        PAGE_FIND(PAGE_CONTROL(PAGE_LINK_MARK, line, segment));
                    kinder_form_hide(control);
                }
                control =
                    PAGE_FIND(PAGE_CONTROL(PAGE_LINK_INSERT, line, segment));
                kinder_form_hide(control);
                control =
                    PAGE_FIND(PAGE_CONTROL(PAGE_LINK_TEXT, line, segment));
                kinder_form_hide(control);
            }
        }
        break;
    case 1:
        for (line = 0; line < 2; line++) {
            control = PAGE_FIND(line | PAGE_LINK_TARGET);
            kinder_form_show(control);
            control->box.y = y;
            control = PAGE_FIND(line | PAGE_LINK_KIND);
            kinder_form_show(control);
            control->box.y = y;
            for (segment = 0; segment < page_link_segments[line]; segment++) {
                if (segment != 0) {
                    control = PAGE_FIND(
                        PAGE_CONTROL(PAGE_LINK_BUTTON, line, segment));
                    kinder_form_show(control);
                    control->box.y = y;
                    control =
                        PAGE_FIND(PAGE_CONTROL(PAGE_LINK_MARK, line, segment));
                    kinder_form_show(control);
                    control->box.y = y;
                }
                control =
                    PAGE_FIND(PAGE_CONTROL(PAGE_LINK_INSERT, line, segment));
                kinder_form_show(control);
                control->box.y = y;
                control =
                    PAGE_FIND(PAGE_CONTROL(PAGE_LINK_TEXT, line, segment));
                kinder_form_show(control);
                control->box.y = y;
                y += control->box.height + 4.0F;
            }
            y += 8.0F;
        }
        break;
    }
    control = PAGE_FIND(17);
    control->box.y = y;
    control = PAGE_FIND(18);
    control->box.y = y;
}

/* The panel copies these dynamic settings while the local form descriptor is
 * live. */
#define PAGE_SETTINGS(kind_id, type, source)                                   \
    struct form_desc desc;                                                     \
    struct type settings;                                                      \
    desc.kind = (kind_id);                                                     \
    desc.settings = &settings;                                                 \
    settings = (source)
#define PAGE_BUTTON(source, control_id)                                        \
    {                                                                          \
        PAGE_SETTINGS(FORM_BUTTON, form_text_desc, source);                    \
        settings.box.id = (control_id);                                        \
        form_panel_add(page_editor_form, &desc);                               \
    }

#define MARK_OPTIONS(control, index)                                           \
    form_choice_set_item((control), 0, 0, kinder_string(0x27f));               \
    form_choice_set_item((control), 1, 1, kinder_string(0x280));               \
    form_choice_set_item((control), 2, 2, kinder_string(0x281));               \
    form_choice_set_item((control), 3, 3, kinder_string(0x282));               \
    for ((index) = 0; (index) < editor_book->mark_count; (index)++) {          \
        form_choice_set_item((control), (index) + EDITOR_MARK_OPTIONS,         \
                             (index) + EDITOR_MARK_OPTIONS,                    \
                             editor_book->marks[index]->title);                \
    }

static void page_editor_populate(void)
{
    struct kinder_form *control;
    Uint16 *text;
    int code;
    int line;
    int part;
    int link;
    int mark;
    int option;
    int segment;
    int length;
    int selected;
    int best;
    page_link_style = editor_page->style;
    control = PAGE_FIND(6);
    form_choice_select(control, page_link_style);
    for (link = 0; link < 2; link++) {
#ifdef NONMATCHING
        Uint16 caption[15]; /* a full fixed-width caption plus its terminator */
        const Uint16 *end;
#else
        Uint16 caption[14];
#endif
        segment = 0;
        length = 0;
        {
            PAGE_SETTINGS(FORM_CHOICE, form_choice_desc, link_target);
            settings.count = editor_book->page_count;
            settings.box.id = link | PAGE_LINK_TARGET;
            control = form_panel_add(page_editor_form, &desc);
        }
        if (link == 0) {
            selected = editor_page->links[0].page;
        } else {
            selected = editor_page->links[1].page;
        }
        for (option = 0; option < editor_book->page_count; option++) {
            form_choice_set_item(control, option, option,
                                 editor_book->pages[option]->title);
            if (selected == option) {
                form_choice_select(control, option);
            }
        }
        {
            PAGE_SETTINGS(FORM_CHOICE, form_choice_desc, link_kind);
            settings.count = 10;
            settings.box.id = link | PAGE_LINK_KIND;
            control = form_panel_add(page_editor_form, &desc);
        }
        if (link == 0) {
            selected = editor_page->links[0].kind;
        } else {
            selected = editor_page->links[1].kind;
        }
#ifdef NONMATCHING
        /* this side starts best at 0, so that a link no option matches selects
         * the first */
        best = 0;
#endif
        for (option = 0; (unsigned)option < sizeof(page_link_kinds); option++) {
            form_choice_set_item(control, option, option,
                                 page_kind_labels[option]);
            if (selected >= page_link_kinds[option]) {
                best = option;
            }
        }
        form_choice_select(control, best);
        if (link == 0) {
            text = editor_page->links[0].text;
        } else {
            text = editor_page->links[1].text;
        }
#ifdef NONMATCHING
        /* the end of the link's text. past it, this side's read below takes
         * 0 */
        end = text + sizeof(editor_page->links[0].text) / sizeof(*text);
#endif
        for (;;) {
#ifdef NONMATCHING
            /* this side reads no code past the link's text. the matching side
             * reads on until a 0 or a marker */
            selected = text < end ? *text++ : 0;
#else
            selected = *text++;
#endif
            if (selected == 0 || (selected >> 8) == 0xff) {
                caption[length] = 0;
                length = 0;
                {
                    PAGE_SETTINGS(FORM_FIELD, form_field_desc, page_link_field);
                    settings.box.id =
                        PAGE_CONTROL(PAGE_LINK_TEXT, link, segment);
                    control = form_panel_add(page_editor_form, &desc);
                    form_field_set_text(control, caption);
                }
                PAGE_BUTTON(page_link_insert,
                            PAGE_CONTROL(PAGE_LINK_INSERT, link, segment));
                segment++;
                if (selected == 0) {
                    page_link_segments[link] = segment;
                    break;
                }
                {
                    PAGE_SETTINGS(FORM_CHOICE, form_choice_desc,
                                  page_link_marks);
                    settings.box.id =
                        PAGE_CONTROL(PAGE_LINK_MARK, link, segment);
                    settings.count =
                        editor_book->mark_count + EDITOR_MARK_OPTIONS;
                    control = form_panel_add(page_editor_form, &desc);
                }
                MARK_OPTIONS(control, mark);
                form_choice_select(control, (Uint8)selected);
                PAGE_BUTTON(page_link_button,
                            PAGE_CONTROL(PAGE_LINK_BUTTON, link, segment));
            } else {
                caption[length++] = selected;
            }
        }
    }
    control = PAGE_FIND(19);
    form_choice_select(control, editor_page->options[4]);
    control = PAGE_FIND(1);
    form_choice_select(control, editor_page->options[0]);
    control = PAGE_FIND(2);
    form_choice_select(control, editor_page->options[1]);
    control = PAGE_FIND(3);
    form_choice_select(control, editor_page->options[2]);
    control = PAGE_FIND(4);
    form_choice_select(control, editor_page->options[3]);
    page_editor_lines = editor_page->line_count;
    for (line = 0; line < page_editor_lines; line++) {
        {
            PAGE_SETTINGS(FORM_CHOICE, form_choice_desc, page_line_codes);
            settings.box.id = line | PAGE_LINE_CODE;
            settings.value = editor_page->lines[line]->code;
            form_panel_add(page_editor_form, &desc);
        }
        PAGE_BUTTON(page_line_button, line | PAGE_LINE_BUTTON);
        text = editor_page->lines[line]->text;
        part = 0;
        length = 0;
        {
#ifdef NONMATCHING
            /* this side's line text holds a terminator past 64 codes, and the
             * read stops at the line's end */
            Uint16 line_text[65];
            const Uint16 *end =
                text + sizeof(editor_page->lines[line]->text) / sizeof(*text);
#else
            Uint16 line_text[64];
#endif
            for (;;) {
#ifdef NONMATCHING
                /* this side reads no code past the line's text. the matching
                 * side reads on until a 0 or a marker */
                code = text < end ? *text++ : 0;
#else
                code = *text++;
#endif
                if (code == 0 || (code >> 8) == 0xff) {
                    line_text[length] = 0;
                    length = 0;
                    {
                        PAGE_SETTINGS(FORM_FIELD, form_field_desc,
                                      page_line_field);
                        settings.box.id =
                            PAGE_CONTROL(PAGE_LINE_TEXT, line, part);
                        control = form_panel_add(page_editor_form, &desc);
                        form_field_set_text(control, line_text);
                    }
                    PAGE_BUTTON(page_segment_button,
                                PAGE_CONTROL(PAGE_SEGMENT_BUTTON, line, part));
                    PAGE_BUTTON(page_line_insert,
                                PAGE_CONTROL(PAGE_LINE_INSERT, line, part));
                    part++;
                    if (code == 0) {
                        page_line_segments[line] = part;
                        break;
                    }
                    {
                        PAGE_SETTINGS(FORM_CHOICE, form_choice_desc,
                                      page_line_marks);
                        settings.box.id =
                            PAGE_CONTROL(PAGE_LINE_MARK, line, part);
                        settings.count =
                            editor_book->mark_count + EDITOR_MARK_OPTIONS;
                        settings.value = selected = (Uint8)code;
                        control = form_panel_add(page_editor_form, &desc);
                    }
                    MARK_OPTIONS(control, mark);
                    form_choice_select(control, selected);
                } else {
                    line_text[length++] = code;
                }
            }
        }
    }
    page_editor_layout();
}

static form_notify confirm_page_event;
static const struct form_text_desc question = {
#include "chaostgkinder/book_menus/question.inc"
};
static const struct form_text_desc warning = {
#include "chaostgkinder/book_menus/warning.inc"
};
static const struct form_text_desc yes = {
#include "chaostgkinder/book_menus/yes.inc"
};
static const struct form_text_desc no = {
#include "chaostgkinder/book_menus/no.inc"
};
static const struct form_desc confirm_controls[4] = {
#include "chaostgkinder/book_menus/confirm_controls.inc"
};
static const struct form_panel_desc confirm_panel = {
#include "chaostgkinder/book_menus/confirm_panel.inc"
};
static const struct form_desc confirm_desc = {
#include "chaostgkinder/book_menus/confirm_desc.inc"
};

/* the comments of the save's backup header, both "unit creation data" in
 * Shift JIS. vmu_comment, which the VMU shows, is half-width katakana.
 * bios_comment, which the Dreamcast's file manager shows, is full-width */
static const char vmu_comment[18] = {
#include "chaostgkinder/book_menus/vmu_comment.inc"
};
static const char bios_comment[34] = {
#include "chaostgkinder/book_menus/bios_comment.inc"
};
static const Uint16 icon_palette[16] = {
#include "chaostgkinder/book_menus/icon_palette.inc"
};
static const Uint8 icon_pixels[512] = {
#include "chaostgkinder/book_menus/icon_pixels.inc"
};
static const char export_filename[16] = {
#include "chaostgkinder/book_menus/export_filename.inc"
};

static Uint16 icon_count = {
#include "chaostgkinder/book_menus/icon_count.inc"
};
static Uint16 icon_interval = {
#include "chaostgkinder/book_menus/icon_interval.inc"
};
static void *cover_image = {
#include "chaostgkinder/book_menus/cover_image.inc"
};
static Uint16 cover_type = {
#include "chaostgkinder/book_menus/cover_type.inc"
};

static struct kinder_form *confirmation;
static Uint8 *save_buffer, *load_buffer, *export_buffer;

#define FIND(id) form_panel_find(page_editor_form, (id))
#define CHOICE(id) form_choice_value(FIND(id))
#define LINE_LIMIT 63
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: a caption leaves room
 * for the terminator */
#define CAPTION_LIMIT 13
#else
/* @bug memory: a 14-code caption leaves no room for the terminator written
 * by the page reader at 0x0c9c4fe0. */
#define CAPTION_LIMIT 14
#endif

static void capture_page(void)
{
    struct book_line *line;
    Uint16 *text;
    int i;
    int j;
    int length;
#ifdef NONMATCHING
    /* Prepare every row before replacing the live book's page. */
    struct book_page *replacement = book_page_create();
    if (!replacement) {
        return;
    }
    for (i = 0; i < page_editor_lines; i++) {
        replacement->lines[i] = book_line_create();
        if (!replacement->lines[i]) {
            replacement->line_count = i;
            book_page_destroy(replacement);
            return;
        }
    }
    book_page_destroy(editor_page);
    editor_book->pages[editor_page_index] = editor_page = replacement;
#else
    book_page_destroy(editor_page);
    editor_book->pages[editor_page_index] = editor_page = book_page_create();
#endif
    kinder_wide_terminate(editor_page->title, form_field_text(FIND(0)), 15);
    editor_page->options[0] = CHOICE(1);
    editor_page->options[1] = CHOICE(2);
    editor_page->options[2] = CHOICE(3);
    editor_page->options[3] = CHOICE(4);
    editor_page->options[4] = CHOICE(19);
    editor_page->line_count = page_editor_lines;
    for (i = 0; i < page_editor_lines; i++) {
#ifdef NONMATCHING
        /* this side fills the lines made with the replacement page above, which
         * fails before the old page goes. the matching side makes each line
         * here */
        line = editor_page->lines[i];
#else
        line = book_line_create();
#endif
        text = line->text;
        editor_page->lines[i] = line;
        line->code = CHOICE(i | PAGE_LINE_CODE);
        length = 0;
        for (j = 0; j < page_line_segments[i]; j++) {
            if (j > 0 && length < LINE_LIMIT) {
                text[length++] =
                    CHOICE(PAGE_CONTROL(PAGE_LINE_MARK, i, j)) | 0xff00;
            }
            if (length < LINE_LIMIT) {
                kinder_wide_copy_n(
                    &text[length],
                    form_field_text(FIND(PAGE_CONTROL(PAGE_LINE_TEXT, i, j))),
                    LINE_LIMIT - length);
                length += kinder_wide_length(&text[length]);
            }
        }
        text[length] = 0;
    }
    editor_page->style = CHOICE(6);
    editor_page->links[0].page = CHOICE(PAGE_LINK_TARGET);
    editor_page->links[0].kind = page_link_kinds[CHOICE(PAGE_LINK_KIND)];
    editor_page->links[1].page = CHOICE(PAGE_LINK_TARGET | 1);
    editor_page->links[1].kind = page_link_kinds[CHOICE(PAGE_LINK_KIND | 1)];
    for (i = 0; i < 2; i++) {
        length = 0;
        if (i == 0) {
            text = editor_page->links[0].text;
        } else {
            text = editor_page->links[1].text;
        }
        for (j = 0; j < page_link_segments[i]; j++) {
            if (j > 0 && length < CAPTION_LIMIT) {
                text[length++] =
                    CHOICE(PAGE_CONTROL(PAGE_LINK_MARK, i, j)) | 0xff00;
            }
            if (length < CAPTION_LIMIT) {
                kinder_wide_copy_n(
                    &text[length],
                    form_field_text(FIND(PAGE_CONTROL(PAGE_LINK_TEXT, i, j))),
                    CAPTION_LIMIT - length);
                length += kinder_wide_length(&text[length]);
            }
        }
        text[length] = 0;
    }
}

#define SETTINGS(kind_id, type, source)                                        \
    struct type settings;                                                      \
    desc.kind = (kind_id);                                                     \
    desc.settings = &settings;                                                 \
    settings = (source)
#define ADD_CONTROL(source, kind_id, type, control_id)                         \
    {                                                                          \
        SETTINGS(kind_id, type, source);                                       \
        settings.box.id = (control_id);                                        \
        form_panel_add(page_editor_form, &desc);                               \
    }
#define ADD_BUTTON(source, control_id)                                         \
    ADD_CONTROL(source, FORM_BUTTON, form_text_desc, control_id)

static void add_editor_line(Sint32 index)
{
    struct form_desc desc;
    {
        SETTINGS(FORM_CHOICE, form_choice_desc, page_line_codes);
        settings.box.id = index | PAGE_LINE_CODE;
        settings.value = 0;
        form_panel_add(page_editor_form, &desc);
    }
    ADD_CONTROL(page_line_field, FORM_FIELD, form_field_desc,
                index | PAGE_LINE_TEXT);
    ADD_BUTTON(page_line_button, index | PAGE_LINE_BUTTON);
    ADD_BUTTON(page_segment_button, index | PAGE_SEGMENT_BUTTON);
    ADD_BUTTON(page_line_insert, index | PAGE_LINE_INSERT);
    page_line_segments[index] = 1;
}

#define GROUP_MASK 0xffff0000
#define ROW_MASK 0xff00
#define INDEX_MASK 0xff
#define WITH_SEGMENT(key, row) (((key) & 0xffff00ff) | (row))
#define LINE_TYPES                                                             \
    case PAGE_LINE_BUTTON:                                                     \
    case PAGE_SEGMENT_BUTTON:                                                  \
    case PAGE_LINE_CODE:                                                       \
    case PAGE_LINE_TEXT:                                                       \
    case PAGE_LINE_MARK:                                                       \
    case PAGE_LINE_INSERT
#define LINE_SEGMENT_TYPES                                                     \
    case PAGE_SEGMENT_BUTTON:                                                  \
    case PAGE_LINE_TEXT:                                                       \
    case PAGE_LINE_MARK:                                                       \
    case PAGE_LINE_INSERT
#define LINK_SEGMENT_TYPES                                                     \
    case PAGE_LINK_TEXT:                                                       \
    case PAGE_LINK_INSERT:                                                     \
    case PAGE_LINK_MARK:                                                       \
    case PAGE_LINK_BUTTON

#define ADD_MARK_OPTIONS(control, index)                                       \
    form_choice_set_item((control), 0, 0, kinder_string(0x27f));               \
    form_choice_set_item((control), 1, 1, kinder_string(0x280));               \
    form_choice_set_item((control), 2, 2, kinder_string(0x281));               \
    form_choice_set_item((control), 3, 3, kinder_string(0x282));               \
    for ((index) = 0; (index) < editor_book->mark_count; (index)++) {          \
        form_choice_set_item((control), (index) + 4, (index) + 4,              \
                             editor_book->marks[index]->title);                \
    }

static void page_editor_event(void *context, struct kinder_form *form,
                              Sint32 event, Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
        switch (key & GROUP_MASK) {
        case 0:
            switch (key) {
            case 5:
                if (page_editor_lines < BOOK_PAGE_LINES) {
                    add_editor_line(page_editor_lines++);
                    page_editor_layout();
                } else {
                }
                break;
            case 17:
                book_editor_screen = EDITOR_REMOVE_PAGE;
                book_editor_step = 0;
                break;
            case 18:
                goto close;
            }
            break;
        case PAGE_LINE_BUTTON: {
            Sint32 index;
            struct kinder_form **controls;
            Sint32 count;
            Sint32 i;
            struct kinder_form *child;
#ifdef NONMATCHING
            /* this side adds no line past BOOK_PAGE_LINES */
            if (page_editor_lines >= BOOK_PAGE_LINES) {
                break;
            }
#endif
            index = (Uint8)key;
            controls = form_panel_children(page_editor_form);
            count = form_panel_count(page_editor_form);
            for (i = 0; i < count; i++) {
                child = controls[i];
                switch (child->box.id & GROUP_MASK) {
                LINE_TYPES:
                    if ((child->box.id & INDEX_MASK) >= (Uint32)index) {
                        child->box.id++;
                    }
                    break;
                }
            }
            for (i = page_editor_lines; i > index; i--) {
                page_line_segments[i] = page_line_segments[i - 1];
            }
            page_editor_lines++;
            add_editor_line(index);
            page_editor_layout();
        } break;
        case PAGE_SEGMENT_BUTTON: {
            struct form_panel *panel;
            Sint32 index;
            Sint32 row;
            Sint32 marker;
            panel = form->state;
            index = (Uint8)key;
            row = key & ROW_MASK;
            marker = row;
            if (marker == 0) {
                marker = 0x100;
            }
            if (page_line_segments[index] > 1) {
                Sint32 kept;
                Sint32 i;
                kept = 0;
                for (i = 0; i < panel->count; i++) {
                    struct kinder_form *child = panel->children[i];
                    if ((child->box.id & INDEX_MASK) == (Uint32)index) {
                        switch (child->box.id & GROUP_MASK) {
                        case PAGE_LINE_MARK:
                            if ((child->box.id & ROW_MASK) == (Uint32)marker) {
                                child->ops->destroy(child);
                                child = NULL;
                            } else if ((child->box.id & ROW_MASK) >
                                       (Uint32)marker) {
                                child->box.id -= 0x100;
                            }
                            break;
                        case PAGE_SEGMENT_BUTTON:
                        case PAGE_LINE_TEXT:
                        case PAGE_LINE_INSERT:
                            if ((child->box.id & ROW_MASK) == (Uint32)row) {
                                child->ops->destroy(child);
                                child = NULL;
                            } else if ((child->box.id & ROW_MASK) >
                                       (Uint32)row) {
                                child->box.id -= 0x100;
                            }
                            break;
                        }
                    }
                    if (child) {
                        panel->children[kept++] = child;
                    }
                }
                panel->count = kept;
#ifdef NONMATCHING
/* this side compares the row's segment, its bits from 8. the matching side
 * compares the row's id whole */
#define SEGMENT_NUMBER(row) ((row) >> 8)
#else
#define SEGMENT_NUMBER(row) (row)
#endif
                if (SEGMENT_NUMBER(row) == page_line_segments[index] - 1) {
                    form_panel_focus_id(form, (row - 0x100) | index |
                                                  PAGE_SEGMENT_BUTTON);
                } else {
                    form_panel_focus_id(form,
                                        index | row | PAGE_SEGMENT_BUTTON);
                }
#undef SEGMENT_NUMBER
                page_line_segments[index]--;
            } else {
                Sint32 kept;
                Sint32 i;
                kept = 0;
                for (i = 0; i < panel->count; i++) {
                    struct kinder_form *child = panel->children[i];
                    switch (child->box.id & GROUP_MASK) {
                    LINE_TYPES:
                        if ((child->box.id & INDEX_MASK) == (Uint32)index) {
                            child->ops->destroy(child);
                            child = NULL;
                        } else if ((child->box.id & INDEX_MASK) >
                                   (Uint32)index) {
                            child->box.id--;
                        }
                        break;
                    }
                    if (child) {
                        panel->children[kept++] = child;
                    }
                }
                panel->count = kept;
                if (index == page_editor_lines - 1) {
                    if (index == 0) {
                        form_panel_focus_id(form, 19);
                    } else {
                        form_panel_focus_id(form,
                                            (index - 1) | PAGE_SEGMENT_BUTTON);
                    }
                } else {
                    form_panel_focus_id(form, index | PAGE_SEGMENT_BUTTON);
                }
                page_editor_lines--;
                {
                    Sint32 j;
                    for (j = (Uint8)key; j < page_editor_lines; j++) {
                        page_line_segments[j] = (page_line_segments + 1)[j];
                    }
                }
            }
            page_editor_layout();
        } break;
        case PAGE_LINE_INSERT: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = (key & ROW_MASK) + 0x100;
            {
                struct kinder_form **controls;
                Sint32 count;
                Sint32 i;
                struct kinder_form *child;
                controls = form_panel_children(page_editor_form);
                count = form_panel_count(page_editor_form);
                for (i = 0; i < count; i++) {
                    child = controls[i];
                    switch (child->box.id & GROUP_MASK) {
                    LINE_SEGMENT_TYPES:
                        if ((child->box.id & INDEX_MASK) == (Uint32)index &&
                            (child->box.id & ROW_MASK) >= (Uint32)row) {
                            child->box.id += 0x100;
                        }
                        break;
                    }
                }
            }
            {
                struct form_desc desc;
                {
                    struct form_choice_desc settings;
                    struct kinder_form *control;
                    Sint32 i;
                    Sint32 mark;
                    desc.kind = FORM_CHOICE;
                    desc.settings = &settings;
                    settings = page_line_marks;
                    settings.box.id = index | row | PAGE_LINE_MARK;
                    settings.count = editor_book->mark_count + 4;
                    control = form_panel_add(page_editor_form, &desc);
                    ADD_MARK_OPTIONS(control, mark);
                }
                ADD_CONTROL(page_line_field, FORM_FIELD, form_field_desc,
                            index | row | PAGE_LINE_TEXT);
                ADD_BUTTON(page_segment_button,
                           index | row | PAGE_SEGMENT_BUTTON);
                ADD_BUTTON(page_line_insert, index | row | PAGE_LINE_INSERT);
            }
            page_line_segments[index]++;
            page_editor_layout();
        } break;
        case PAGE_LINK_INSERT: {
            Sint32 index;
            Sint32 row;
            struct kinder_form *control;
            index = (Uint8)key;
            row = (key & ROW_MASK) + 0x100;
            {
                struct kinder_form **controls;
                Sint32 count;
                Sint32 i;
                struct kinder_form *child;
                controls = form_panel_children(page_editor_form);
                count = form_panel_count(page_editor_form);
                for (i = 0; i < count; i++) {
                    child = controls[i];
                    switch (child->box.id & GROUP_MASK) {
                    LINK_SEGMENT_TYPES:
                        if ((child->box.id & INDEX_MASK) == (Uint32)index &&
                            (child->box.id & ROW_MASK) >= (Uint32)row) {
                            child->box.id += 0x100;
                        }
                        break;
                    }
                }
            }
            {
                struct form_desc desc;
                {
                    SETTINGS(FORM_FIELD, form_field_desc, page_link_field);
                    settings.box.id = index | row | PAGE_LINK_TEXT;
                    control = form_panel_add(page_editor_form, &desc);
                }
                {
                    SETTINGS(FORM_BUTTON, form_text_desc, page_link_insert);
                    settings.box.id = index | row | PAGE_LINK_INSERT;
                    control = form_panel_add(page_editor_form, &desc);
                }
                {
                    struct form_choice_desc settings;
                    Sint32 mark;
                    desc.kind = FORM_CHOICE;
                    desc.settings = &settings;
                    settings = page_link_marks;
                    settings.box.id = index | row | PAGE_LINK_MARK;
                    settings.count = editor_book->mark_count + 4;
                    control = form_panel_add(page_editor_form, &desc);
                    ADD_MARK_OPTIONS(control, mark);
                }
                {
                    SETTINGS(FORM_BUTTON, form_text_desc, page_link_button);
                    settings.box.id = index | row | PAGE_LINK_BUTTON;
                    control = form_panel_add(page_editor_form, &desc);
                }
            }
            page_link_segments[index]++;
        } break;
        case PAGE_LINK_BUTTON: {
            Sint32 index;
            Sint32 row;
            struct form_panel *panel;
            Sint32 count;
            Sint32 kept;
            Sint32 i;
            struct kinder_form *child;
            index = (Uint8)key;
            row = key & ROW_MASK;
            panel = form->state;
            /* @hack redundant: the link-removal path fetches the count
             * even though the loop reads the panel directly. */
            count = form_panel_count(form);
            (void)count;
            kept = 0;
            for (i = 0; i < panel->count; i++) {
                child = panel->children[i];
                if ((child->box.id & INDEX_MASK) == (Uint32)index) {
                    switch (child->box.id & GROUP_MASK) {
                    case PAGE_LINK_BUTTON:
                    case PAGE_LINK_MARK:
                    case PAGE_LINK_TEXT:
                    case PAGE_LINK_INSERT:
                        if ((child->box.id & ROW_MASK) == (Uint32)row) {
                            child->ops->destroy(child);
                            child = NULL;
#ifdef NONMATCHING
                            /* this side compares the row bits, as the delete
                             * above does. the matching side compares the index
                             * bits */
                        } else if ((child->box.id & ROW_MASK) > (Uint32)row) {
#else
                        } else if ((child->box.id & INDEX_MASK) > (Uint32)row) {
#endif
                            child->box.id -= 0x100;
                        }
                        break;
                    }
                }
                if (child) {
                    panel->children[kept++] = child;
                }
            }
            panel->count = kept;
            form_panel_focus_id(form, (row - 0x100) | index | PAGE_LINK_TEXT);
            page_link_segments[index]--;
            page_editor_layout();
        } break;
        }
        break;
    case WIDGET_CANCEL:
    close:
        capture_page();
        editor_form_remove(form);
        form->ops->destroy(form);
        book_editor_screen = EDITOR_PAGES;
        book_editor_step = 0;
        break;
    case WIDGET_UP: {
        Sint32 index;
        Sint32 row;
        switch (key & GROUP_MASK) {
        case 0:
            switch (key) {
            case 0:
                key = 18;
                break;
            case 1:
            case 3:
                key = 0;
                break;
            case 2:
                key = 1;
                break;
            case 4:
                key = 3;
                break;
            case 19:
                key = 2;
                break;
            case 5:
                if (page_editor_lines > 0) {
                    key = PAGE_CONTROL(
                        PAGE_LINE_TEXT, page_editor_lines - 1,
                        page_line_segments[page_editor_lines - 1] - 1);
                } else {
                    key = 19;
                }
                break;
            case 6:
                key = 5;
                break;
            case 17:
            case 18:
                switch (page_link_style) {
                case 0:
                case 4:
                case 5:
                default:
                    key = PAGE_LINK_TARGET;
                    break;
                case 1:
                    key = ((page_link_segments[1] - 1) << 8) |
                          (PAGE_LINK_TEXT | 1);
                    break;
                case 2:
                case 3:
                    key = 6;
                    break;
                }
                break;
            }
            break;
        case PAGE_LINE_TEXT:
        case PAGE_LINE_INSERT:
        case PAGE_SEGMENT_BUTTON:
            index = (Uint8)key;
            row = key & ROW_MASK;
            if (row == 0) {
                if (index == 0) {
                    key = 19;
                } else {
                    index--;
                    key = ((page_line_segments[index] - 1) << 8) |
                          (key & GROUP_MASK) | index;
                }
            } else {
                row -= 0x100;
                key = WITH_SEGMENT(key, row);
            }
            break;
        case PAGE_LINE_CODE:
            index = (Uint8)key;
            if (index == 0) {
                key = 19;
            } else {
                index--;
                if (page_line_segments[index] > 1) {
                    key = PAGE_CONTROL(PAGE_LINE_MARK, index,
                                       page_line_segments[index] - 1);
                } else {
                    key = index | PAGE_LINE_CODE;
                }
            }
            break;
        case PAGE_LINE_MARK:
            index = (Uint8)key;
            row = key & ROW_MASK;
            row -= 0x100;
            if (row == 0) {
                key = index | PAGE_LINE_CODE;
            } else {
                key = row | index | PAGE_LINE_MARK;
            }
            break;
        case PAGE_LINE_BUTTON:
            index = (Uint8)key;
            if (index == 0) {
                key = 19;
            } else {
                index--;
                key = index | PAGE_LINE_BUTTON;
            }
            break;
        case PAGE_LINK_TARGET:
            index = (Uint8)key;
            if (index == 0) {
                key = 6;
            } else if (page_link_segments[0] > 1) {
                key = ((page_link_segments[0] - 1) << 8) | PAGE_LINK_BUTTON;
            } else {
                key = PAGE_LINK_TARGET;
            }
            break;
        case PAGE_LINK_KIND:
            index = (Uint8)key;
            if (index == 0) {
                key = 6;
            } else if (page_link_segments[0] > 1) {
                key = ((page_link_segments[0] - 1) << 8) | PAGE_LINK_MARK;
            } else {
                key = PAGE_LINK_KIND;
            }
            break;
        case PAGE_LINK_TEXT:
        case PAGE_LINK_INSERT:
            index = (Uint8)key;
            row = key & ROW_MASK;
            if (row == 0) {
                if (index == 0) {
                    key = 6;
                } else {
                    index--;
                    row = (page_link_segments[0] - 1) << 8;
                    key = (key & GROUP_MASK) | row | index;
                }
            } else {
                row -= 0x100;
                key = WITH_SEGMENT(key, row);
            }
            break;
        case PAGE_LINK_MARK:
            index = (Uint8)key;
            row = key & ROW_MASK;
            row -= 0x100;
            if (row == 0) {
                key = index | PAGE_LINK_KIND;
            } else {
                key = WITH_SEGMENT(key, row);
            }
            break;
        case PAGE_LINK_BUTTON:
            index = (Uint8)key;
            row = key & ROW_MASK;
            row -= 0x100;
            if (row == 0) {
                key = index | PAGE_LINK_TARGET;
            } else {
                key = WITH_SEGMENT(key, row);
            }
            break;
        }
        form_panel_focus_id(form, key);
        break;
    }
    case WIDGET_DOWN:
        switch (key & GROUP_MASK) {
        case 0:
            switch (key) {
            case 0:
                key = 1;
                break;
            case 1:
                key = 2;
                break;
            case 3:
                key = 4;
                break;
            case 2:
            case 4:
                key = 19;
                break;
            case 19:
                if (page_editor_lines > 0) {
                    key = PAGE_LINE_TEXT;
                } else {
                    key = 5;
                }
                break;
            case 5:
                key = 6;
                break;
            case 6:
                switch (page_link_style) {
                case 2:
                case 3:
                    key = 18;
                    break;
                default:
                    key = PAGE_LINK_TARGET;
                    break;
                }
                break;
            case 17:
            case 18:
                key = 0;
                break;
            }
            break;
        case PAGE_LINE_TEXT:
        case PAGE_LINE_INSERT:
        case PAGE_SEGMENT_BUTTON: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = key & ROW_MASK;
            row += 0x100;
            if ((row >> 8) < page_line_segments[index]) {
                key = WITH_SEGMENT(key, row);
            } else if (++index < page_editor_lines) {
                key = (key & GROUP_MASK) | index;
            } else {
                key = 5;
            }
        } break;
        case PAGE_LINE_CODE: {
            Sint32 index = (Uint8)key;
            if (page_line_segments[index] > 1) {
                key = index | (PAGE_LINE_MARK | 0x100);
            } else if (++index < page_editor_lines) {
                key = index | PAGE_LINE_CODE;
            } else {
                key = 5;
            }
        } break;
        case PAGE_LINE_MARK: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = key & ROW_MASK;
            row += 0x100;
            if ((row >> 8) < page_line_segments[index]) {
                key = WITH_SEGMENT(key, row);
            } else if (++index < page_editor_lines) {
                key = index | PAGE_LINE_CODE;
            } else {
                key = 5;
            }
        } break;
        case PAGE_LINE_BUTTON:
            key++;
            if ((key & INDEX_MASK) == (Uint32)page_editor_lines) {
                key = 5;
            }
            break;
        case PAGE_LINK_TARGET:
            switch (page_link_style) {
            case 1: {
                Sint32 index = (Uint8)key;
                if (page_link_segments[index] > 1) {
                    key = index | (PAGE_LINK_BUTTON | 0x100);
                } else if (index == 0) {
                    key = PAGE_LINK_TARGET | 1;
                } else {
                    key = 18;
                }
            } break;
            default:
                key = 18;
                break;
            }
            break;
        case PAGE_LINK_KIND:
            switch (page_link_style) {
            case 1: {
                Sint32 index = (Uint8)key;
                if (page_link_segments[index] > 1) {
                    key = index | (PAGE_LINK_MARK | 0x100);
                } else if (index == 0) {
                    key = PAGE_LINK_KIND | 1;
                } else {
                    key = 18;
                }
            } break;
            default:
                key = 18;
                break;
            }
            break;
        case PAGE_LINK_TEXT:
        case PAGE_LINK_INSERT: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = key & ROW_MASK;
            if (row + 0x100 == (page_link_segments[index] << 8)) {
                if (index == 0) {
                    key = (key & GROUP_MASK) | 1;
                } else {
                    key = 18;
                }
            } else {
                key += 0x100;
            }
        } break;
        case PAGE_LINK_MARK: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = key & ROW_MASK;
            if (row + 0x100 == (page_link_segments[index] << 8)) {
                if (index == 0) {
                    key = PAGE_LINK_KIND | 1;
                } else {
                    key = 18;
                }
            } else {
                key += 0x100;
            }
        } break;
        case PAGE_LINK_BUTTON: {
            Sint32 index;
            Sint32 row;
            index = (Uint8)key;
            row = key & ROW_MASK;
            if (row + 0x100 == (page_link_segments[index] << 8)) {
                if (index == 0) {
                    key = PAGE_LINK_TARGET | 1;
                } else {
                    key = 18;
                }
            } else {
                key += 0x100;
            }
            break;
        }
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_LEFT:
        switch (key & GROUP_MASK) {
        case 0:
            switch (key) {
            case 1:
                key = 3;
                break;
            case 2:
                key = 4;
                break;
            case 3:
                key = 1;
                break;
            case 4:
                key = 2;
                break;
            case 17:
                key = 18;
                break;
            case 18:
                key = 17;
                break;
            }
            break;
        case PAGE_LINE_BUTTON:
            key = (Uint8)key | PAGE_LINE_INSERT;
            break;
        case PAGE_SEGMENT_BUTTON:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINE_INSERT;
            } else {
                key = (Uint8)key | PAGE_LINE_BUTTON;
            }
            break;
        case PAGE_LINE_CODE:
        case PAGE_LINE_MARK:
            key = (Uint16)key | PAGE_SEGMENT_BUTTON;
            break;
        case PAGE_LINE_TEXT:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINE_MARK;
            } else {
                key = (Uint8)key | PAGE_LINE_CODE;
            }
            break;
        case PAGE_LINE_INSERT:
            key = (Uint16)key | PAGE_LINE_TEXT;
            break;
        case PAGE_LINK_TARGET:
            key = (Uint8)key | PAGE_LINK_INSERT;
            break;
        case PAGE_LINK_KIND:
            key = (Uint8)key | PAGE_LINK_TARGET;
            break;
        case PAGE_LINK_TEXT:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINK_MARK;
            } else {
                key = (Uint8)key | PAGE_LINK_KIND;
            }
            break;
        case PAGE_LINK_INSERT:
            key = (Uint16)key | PAGE_LINK_TEXT;
            break;
        case PAGE_LINK_BUTTON:
            key = (Uint16)key | PAGE_LINK_INSERT;
            break;
        case PAGE_LINK_MARK:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINK_BUTTON;
            } else {
                key = (Uint8)key | PAGE_LINK_KIND;
            }
            break;
        }
        form_panel_focus_id(form, key);
        break;
    case WIDGET_RIGHT:
        switch (key & GROUP_MASK) {
        case 0:
            switch (key) {
            case 1:
                key = 3;
                break;
            case 2:
                key = 4;
                break;
            case 3:
                key = 1;
                break;
            case 4:
                key = 2;
                break;
            case 17:
                key = 18;
                break;
            case 18:
                key = 17;
                break;
            }
            break;
        case PAGE_LINE_BUTTON:
            key = (Uint16)key | PAGE_SEGMENT_BUTTON;
            break;
        case PAGE_SEGMENT_BUTTON:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINE_MARK;
            } else {
                key = (Uint16)key | PAGE_LINE_CODE;
            }
            break;
        case PAGE_LINE_CODE:
        case PAGE_LINE_MARK:
            key = (Uint16)key | PAGE_LINE_TEXT;
            break;
        case PAGE_LINE_TEXT:
            key = (Uint16)key | PAGE_LINE_INSERT;
            break;
        case PAGE_LINE_INSERT:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_SEGMENT_BUTTON;
            } else {
                key = (Uint16)key | PAGE_LINE_BUTTON;
            }
            break;
        case PAGE_LINK_TARGET:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINK_MARK;
            } else {
                key = (Uint8)key | PAGE_LINK_KIND;
            }
            break;
        case PAGE_LINK_KIND:
            switch (page_link_style) {
            case 1:
                key = (Uint16)key | PAGE_LINK_TEXT;
                break;
            default:
                key = PAGE_LINK_TARGET;
                break;
            }
            break;
        case PAGE_LINK_MARK:
            key = (Uint16)key | PAGE_LINK_TEXT;
            break;
        case PAGE_LINK_TEXT:
            key = (Uint16)key | PAGE_LINK_INSERT;
            break;
        case PAGE_LINK_INSERT:
            if (key & ROW_MASK) {
                key = (Uint16)key | PAGE_LINK_BUTTON;
            } else {
                key = (Uint8)key | PAGE_LINK_TARGET;
            }
            break;
        case PAGE_LINK_BUTTON:
            key = (Uint16)key | PAGE_LINK_MARK;
            break;
        }
        form_panel_focus_id(form, key);
        break;
    case 9:
        if (key == 6) {
            struct kinder_form *choice = form_panel_find(form, key);
            page_link_style = form_choice_value(choice);
        }
        /* fall through to rearrange the controls for the new link style */
    case WIDGET_TICK:
        page_editor_layout();
        break;
    }
}

static void edit_page(void)
{
    struct form_desc desc;
    struct form_field_desc field;
    struct kinder_form *title;
    int i;
    switch (book_editor_step) {
    case 0:
        if ((page_editor_form = kinder_form_create(NULL, &page_editor_desc)) !=
            NULL) {
            editor_form_push(page_editor_form);
            book_editor_step = 2;
            desc.kind = FORM_FIELD;
            desc.settings = &field;
            field = page_title_field;
            title = form_panel_add(page_editor_form, &desc);
            form_field_set_text(title, editor_page->title);
            form_panel_focus_id(page_editor_form, 0);
            for (i = 0; (unsigned)i < sizeof(page_link_kinds); i++) {
                kinder_int_to_text(page_kind_labels[i], page_link_kinds[i]);
            }
            page_editor_populate();
        }
        break;
    case 1:
        book_remove_page(editor_book, editor_page_index);
        editor_form_remove(page_editor_form);
        page_editor_form->ops->destroy(page_editor_form);
        book_editor_screen = EDITOR_PAGES;
        book_editor_step = 0;
        break;
    case 2:
    default:
        kinder_form_poll(page_editor_form);
        break;
    }
}

static void confirm_page_event(void *context, struct kinder_form *form,
                               Sint32 event, Uint32 key)
{
    switch (event) {
    case WIDGET_ACCEPT:
    case WIDGET_CANCEL:
        editor_form_remove(form);
        form->ops->destroy(form);
        book_editor_screen = EDITOR_PAGE;
        book_editor_step = event == WIDGET_ACCEPT && key == 0 ? 1 : 2;
        break;
    case WIDGET_RIGHT:
        form_panel_focus_id(form, 1);
        break;
    case WIDGET_LEFT:
        form_panel_focus_id(form, 0);
        break;
    case WIDGET_TAB:
    case 7:
        form_panel_focus_id(form, key == 1 ? 0 : 1);
        break;
    }
}

static void confirm_page(void)
{
    switch (book_editor_step) {
    case 0:
        if ((confirmation = kinder_form_create(NULL, &confirm_desc)) != NULL) {
            editor_form_push(confirmation);
            book_editor_step++;
        }
        break;
    default:
        kinder_form_poll(confirmation);
        break;
    }
}

static void book_backup_header(BUS_BACKUPFILEHEADER *header, void *data,
                               Uint32 size)
{
    memcpy(header->vms_comment, vmu_comment, sizeof(header->vms_comment));
    memcpy(header->btr_comment, bios_comment, sizeof(header->btr_comment));
    memcpy(header->game_name, card_game_name, sizeof(header->game_name));
    header->icon_palette = (void *)icon_palette;
    header->icon_data = (void *)icon_pixels;
    header->icon_num = icon_count;
    header->icon_speed = icon_interval;
    header->visual_data = cover_image;
    header->visual_type = cover_type;
    header->reserved = 0;
    header->save_data = data;
    header->save_size = size;
}

static void save_to_vmu(void)
{
    BUS_BACKUPFILEHEADER header;
    Sint32 size;
    switch (book_editor_step) {
    case 0:
        size = book_save_size(editor_book);
        save_buffer = syCalloc(size, 1);
#ifdef NONMATCHING
        /* this side goes back to the actions where the heap has no save buffer.
         * the matching side writes through the null */
        if (!save_buffer) {
            book_editor_screen = EDITOR_ACTIONS;
            book_editor_step = 0;
            return;
        }
#endif
        book_write_save(editor_book, save_buffer);
        book_backup_header(&header, save_buffer, size);
        card_save_file(0, book_save_filename, &header, 0);
        book_editor_step = 1;
        break;
    case 1:
    default:
        if (buStat(0) == 0) {
            syFree(save_buffer);
            book_editor_screen = EDITOR_ACTIONS;
            book_editor_step = 0;
        }
        break;
    }
}

static void load_from_vmu(void)
{
#ifdef DREAMCAST
    /* @hack redundant: the VMU command reserves five unused words. */
    Sint32 unused[5];
#endif
    switch (book_editor_step) {
    case 0:
        load_buffer = syCalloc(BOOK_SAVE_BUFFER_SIZE, 1);
#ifdef NONMATCHING
        /* this side goes back home where the heap has no load buffer, which the
         * matching side reads into */
        if (!load_buffer) {
            book_editor_screen = EDITOR_HOME;
            book_editor_step = 0;
            return;
        }
#endif
#ifdef NONMATCHING
        /* this side frees the buffer and goes back home where the load can't
         * start */
        if (card_load_file(0, book_save_filename, load_buffer,
                           BOOK_SAVE_BUFFER_SIZE) != 0) {
            syFree(load_buffer);
            book_editor_screen = EDITOR_HOME;
            return;
        }
#else
        card_load_file(0, book_save_filename, load_buffer,
                       BOOK_SAVE_BUFFER_SIZE);
#endif
        book_editor_step = 1;
        break;
    case 1:
    default:
        if (buStat(0) == 0) {
#ifdef NONMATCHING
            /* this side frees the buffer and returns home where the load read
             * nothing, or more than it holds */
            if (card_load_result(0) <= 0 ||
                card_load_result(0) > BOOK_SAVE_BUFFER_SIZE) {
                syFree(load_buffer);
                book_editor_screen = EDITOR_HOME;
                book_editor_step = 0;
                return;
            }
#endif
            editor_book = book_create();
#ifdef NONMATCHING
            /* this side frees the buffer and returns home where it can't make
             * the book or read its save */
            if (!editor_book || !book_read_save(editor_book, load_buffer)) {
                if (editor_book) {
                    book_destroy(editor_book);
                    editor_book = NULL;
                }
                syFree(load_buffer);
                book_editor_screen = EDITOR_HOME;
                book_editor_step = 0;
                return;
            }
#else
            book_read_save(editor_book, load_buffer);
#endif
            book_editor_screen = EDITOR_BOOK;
            book_editor_step = 0;
            syFree(load_buffer);
        }
        break;
    }
}

static void export_to_vmu(void)
{
#ifdef DREAMCAST
    /* @hack redundant: the VMU command reserves five unused words. */
    Sint32 unused[5];
#endif
    switch (book_editor_step) {
    case 0:
        export_buffer = syCalloc(0x2000, 1);
#ifdef NONMATCHING
        /* this side goes back to the actions where the heap has no export
         * buffer. it does the same where the export would pass the buffer's
         * first half */
        if (!export_buffer || book_export_size(editor_book) > 0x1000) {
            if (export_buffer) {
                syFree(export_buffer);
            }
            book_editor_screen = EDITOR_ACTIONS;
            book_editor_step = 0;
            return;
        }
#endif
        book_export(editor_book, export_buffer);
        memcpy(export_buffer + 0x1000, export_buffer, 0x1000);
        card_rewrite_exec_file(0, export_filename, export_buffer, 10, 16);
        book_editor_step = 1;
        break;
    case 1:
        if (buStat(0) == 0) {
            syFree(export_buffer);
            export_buffer = syCalloc(0x200, 1);
#ifdef NONMATCHING
            /* this side goes back to the actions where the heap has no buffer
             * for the file's rewrite */
            if (!export_buffer) {
                book_editor_screen = EDITOR_ACTIONS;
                book_editor_step = 0;
                return;
            }
#endif
            card_rewrite_exec_file(0, export_filename, export_buffer, 9, 1);
            book_editor_step = 2;
        }
        break;
    case 2:
        if (buStat(0) == 0) {
            syFree(export_buffer);
            book_editor_screen = EDITOR_ACTIONS;
            book_editor_step = 0;
        }
        break;
    }
}

static void book_editor_destroy(struct task *task)
{
    editor_meter_close();
    editor_forms_clear();
    if (editor_book) {
        book_destroy(editor_book);
        editor_book = NULL;
    }
    book_editor_screen = EDITOR_START;
    book_editor_step = 0;
}

static void book_editor_draw(struct task *task)
{
    editor_forms_draw();
}

void kinder_editor_init(struct task *task)
{
    if (task->destructor == NULL) {
        task->draw = book_editor_draw;
        task->destructor = book_editor_destroy;
    }
    switch (book_editor_screen) {
    case EDITOR_START:
        editor_meter_open();
        book_editor_screen = EDITOR_HOME;
        book_editor_step = 0;
        break;
    case EDITOR_HOME:
        editor_home_update();
        break;
    case EDITOR_ACTIONS:
        editor_actions_update();
        break;
    case EDITOR_BOOK:
        editor_book_update();
        break;
    case EDITOR_MARKS:
        editor_marks_update();
        break;
    case EDITOR_MARK:
        editor_mark_update();
        break;
    case EDITOR_REMOVE_MARK:
        editor_remove_mark_update();
        break;
    case EDITOR_PAGES:
        editor_pages_update();
        break;
    case EDITOR_PAGE:
        edit_page();
        break;
    case EDITOR_REMOVE_PAGE:
        confirm_page();
        break;
    case EDITOR_SAVE:
        save_to_vmu();
        break;
    case EDITOR_LOAD:
        load_from_vmu();
        break;
    case EDITOR_EXPORT:
        export_to_vmu();
        break;
    }
    editor_meter_update();
    editor_forms_tick();
}
