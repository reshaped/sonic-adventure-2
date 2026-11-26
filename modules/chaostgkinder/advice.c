#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chaostgkinder/advice.h"
#include "chaostgkinder/messages.h"
#include "chaostgkinder/service_menu.h"
#include "chaostgkinder/services.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"

struct advice_menu {
    Sint8 state, step, cursor, selected, greeting;
    Uint16 timer;
    struct task *task;
};

static const struct kinder_message_label button_label = {
#include "chaostgkinder/advice/button_label.inc"
};
static const char exit_text[8] = {
#include "chaostgkinder/advice/exit_text.inc"
};

static struct advice_menu *menu = NULL;
static struct service_decoration menu_frame = {
#include "chaostgkinder/advice/menu_frame.inc"
};
static struct service_box menu_box = {
#include "chaostgkinder/advice/menu_box.inc"
};
static struct service_box first_button = {
#include "chaostgkinder/advice/first_button.inc"
};
static struct service_choice button_colors = {
#include "chaostgkinder/advice/button_colors.inc"
};
static int choice_message[5] = {
#include "chaostgkinder/advice/choice_message.inc"
};

enum advice_step {
    ADVICE_OPEN,
    ADVICE_GREET,
    ADVICE_CHOOSE,
    ADVICE_WAIT_LIST,
    ADVICE_FAREWELL,
    ADVICE_FINISHED
};

enum topic_list_mode {
    LIST_OPEN,
    LIST_GREETING,
    LIST_SHOW,
    LIST_WAIT_LABELS,
    LIST_CHOOSE,
    LIST_SCROLL,
    LIST_READ_PAGE,
    LIST_CLOSING,
    LIST_DONE
};

enum topic_page_mode {
    PAGE_OPEN,
    PAGE_WAIT_TEXT,
    PAGE_CHOOSE,
    PAGE_TURN,
    PAGE_CLOSING,
    PAGE_DONE
};

enum page_button { PAGE_EXIT, PAGE_NEXT, PAGE_BACK };

enum advice_choice { CHOICE_TOPICS, CHOICE_EXIT, CHOICES };

/* the messages of KINDER_WHOLE_BANK the menu shows */
enum advice_prompt {
    EXIT_ROW_MESSAGE = 53,
    GREETING_MESSAGE = 55,
    SECOND_GREETING_MESSAGE = 57,
    LIST_GREETING_MESSAGE = 61,
    TOPICS_MESSAGE = 63
};

/* the list shows TOPIC_ROWS of its TOPICS topics at a time. topics 0 to
 * LAST_TOPIC have pages, and EXIT_TOPIC, the last, closes the list */
enum {
    TOPIC_ROWS = 5,
    LAST_TOPIC = 18,
    EXIT_TOPIC = 19,
    TOPICS = EXIT_TOPIC + 1
};

/* title and page are messages of KINDER_SERVICE_BANK. part, where not NULL, is
 * a number that follows the title */
struct advice_topic {
    Sint16 title;
    const char *part;
    Sint16 page;
};

struct topic_list {
    Uint8 cursor;
    Sint8 top, highlighted, selected, reading;
    struct task *labels[TOPIC_ROWS];
    struct task *page;
};

struct topic_page {
    Sint8 button;
    int index, last;
    struct task *title, *body;
};

/* the page's task work, which lays reading over task_work past its timer.
 * reading points at its list's reading. the page sets it at each of its
 * steps until it is done. the list clears it at each of its own steps. a
 * list step that finds it clear closes the page */
struct topic_page_work {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    Sint8 *reading;
};

static const struct kinder_message_label row_label = {
#include "chaostgkinder/advice/row_label.inc"
};
static const struct kinder_message_label title_label = {
#include "chaostgkinder/advice/title_label.inc"
};
static const char back_text[8] = {
#include "chaostgkinder/advice/back_text.inc"
};
static const char next_text[8] = {
#include "chaostgkinder/advice/next_text.inc"
};
static const char part_1[4] = {
#include "chaostgkinder/advice/part_1.inc"
};
static const char part_2[4] = {
#include "chaostgkinder/advice/part_2.inc"
};
static const char part_3[4] = {
#include "chaostgkinder/advice/part_3.inc"
};
static const char part_4[2] = {
#include "chaostgkinder/advice/part_4.inc"
};

static struct advice_topic topics[TOPICS] = {
#include "chaostgkinder/advice/topics.inc"
};
static struct service_decoration list_frame = {
#include "chaostgkinder/advice/list_frame.inc"
};
static struct service_box list_box = {
#include "chaostgkinder/advice/list_box.inc"
};
static struct service_box first_row = {
#include "chaostgkinder/advice/first_row.inc"
};
static struct service_choice row_colors = {
#include "chaostgkinder/advice/row_colors.inc"
};
static struct service_box page_box = {
#include "chaostgkinder/advice/page_box.inc"
};

static struct kinder_message_entry advice_messages[103] = {
#include "chaostgkinder/advice/advice_messages.inc"
};

static struct task *topic_list_create(struct task *parent);
static struct task *topic_page_create(struct task *parent, Sint8 *reading,
                                      int index, int last);

#define ADVANCE_BUTTONS (PDD_DGT_TA | PDD_DGT_TB | PDD_DGT_TX | PDD_DGT_TY)

/* the sound of a topic's page opening from the list */
#define PAGE_SOUND 0x1004

/* the frames each wait lasts. a button skips a greeting's GREETING_FRAMES.
 * the menu pauses for PAUSE_FRAMES before it leaves. the list waits
 * STEP_FRAMES at a scroll and once a page closes */
#define GREETING_FRAMES 180
#define PAUSE_FRAMES 60
#define LIST_DELAY_FRAMES 60
#define LIST_GREETING_FRAMES 120
#define LABELS_FRAMES 40
#define CLOSE_LIST_FRAMES 30
#define STEP_FRAMES 5
#define PAGE_OPEN_FRAMES 20
#define PAGE_TEXT_FRAMES 10
#define PAGE_KEY_FRAMES 5
#define PAGE_CURSOR_FRAMES 2
/* a selection that matches no button */
#define NO_SELECTION (-1)
/* a page's title and body, and its buttons, in screen pixels */
#define PAGE_LEFT 168.0F
#define PAGE_DEPTH (-1.5F)
#define TITLE_TOP 80.0F
#define TITLE_WIDTH 200.0F
#define BODY_TOP 107.0F
#define BODY_WIDTH 410.0F
#define BODY_COLOR 0xff000000
#define BUTTON_TOP 314.0F
#define BUTTON_DEPTH (-1.3F)
#define BUTTON_WIDTH 88.0F
#define BUTTON_HEIGHT 28.0F
#define EXIT_LEFT 496.0F
#define BACK_LEFT 160.0F
#define NEXT_LEFT 280.0F
#define BUTTON_SELECTED_COLOR 0xfff96d1c

static void advice_step(struct task *task)
{
    struct service_box box;
    struct service_choice choice;
    struct kinder_message_label label;
    Sint8 i;

    /* @hack redundant: the read of task->work, which nothing uses */
    (void)task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (menu->step) {
    case ADVICE_OPEN:
        kinder_dialog_create(task, 0, 0);
        service_panel_create(task, &menu_box, &menu_frame);
        box = first_button;
        choice = button_colors;
        menu->cursor = menu->selected = NO_SELECTION;
        choice.selection = &menu->selected;
        for (i = 0; i < (Sint8)CHOICES; i++) {
            service_button_create(task, i, &box, &choice);
            label = button_label;
            switch (i) {
            case CHOICE_TOPICS:
                label.message = TOPICS_MESSAGE;
                break;
            case CHOICE_EXIT:
                label.message = 0;
                label.flags |= KINDER_MESSAGE_LABEL_LITERAL;
                label.text = exit_text;
                break;
            }
            label.id = i;
            label.selection = (const Uint8 *)&menu->selected;
            label.pos.x = box.pos.x + (box.width * 0.5F);
            label.pos.y = box.pos.y + (box.height * 0.5F);
            label.pos.z = box.pos.z + SERVICE_LABEL_DEPTH;
            label.width = box.width - SERVICE_LABEL_INSET;
            kinder_message_label_create(task, &label);
            box.pos.y += box.height + SERVICE_ROW_GAP;
        }
        kinder_dialog_set(GREETING_MESSAGE, KINDER_WHOLE_BANK);
        kinder_dialog_show_arrow();
        menu->timer = GREETING_FRAMES;
        menu->greeting = 0;
        menu->step = ADVICE_GREET;
        break;
    case ADVICE_GREET:
        if (menu->timer > 0) {
            menu->timer--;
        }
        if ((pad_press_repeat[0] & ADVANCE_BUTTONS) || menu->timer == 0) {
            menu->greeting++;
            if (menu->greeting == 1) {
                menu->timer = GREETING_FRAMES;
                kinder_dialog_set(SECOND_GREETING_MESSAGE, KINDER_WHOLE_BANK);
                kinder_dialog_show_arrow();
            } else {
                menu->cursor = menu->selected = CHOICE_TOPICS;
                menu->greeting = 0;
                kinder_dialog_set(choice_message[menu->selected],
                                  KINDER_WHOLE_BANK);
                menu->step = ADVICE_CHOOSE;
            }
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
        }
        break;
    case ADVICE_CHOOSE:
        if (menu->cursor < CHOICE_EXIT && (pad_press_repeat[0] & PDD_DGT_KD)) {
            menu->cursor++;
            menu->selected = menu->cursor;
            kinder_dialog_set(choice_message[menu->selected],
                              KINDER_WHOLE_BANK);
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (menu->cursor > CHOICE_TOPICS &&
            (pad_press_repeat[0] & PDD_DGT_KU)) {
            menu->cursor--;
            menu->selected = menu->cursor;
            kinder_dialog_set(choice_message[menu->selected],
                              KINDER_WHOLE_BANK);
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_TB) {
            if (menu->selected == CHOICE_EXIT) {
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
                menu->timer = PAUSE_FRAMES;
                menu->state = KINDER_ADVICE_LEAVING;
                sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
                menu->step = ADVICE_FAREWELL;
                return;
            } else {
                sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
                menu->selected = CHOICE_EXIT;
                kinder_dialog_set(choice_message[menu->selected],
                                  KINDER_WHOLE_BANK);
            }
        }
        if (pad_press_repeat[0] & PDD_DGT_TA) {
            switch (menu->selected) {
            case CHOICE_EXIT:
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
                menu->timer = PAUSE_FRAMES;
                menu->state = KINDER_ADVICE_LEAVING;
                sound_play(SERVICE_LEAVE_SOUND, NULL, 0, 0);
                menu->step = ADVICE_FAREWELL;
                break;
            case CHOICE_TOPICS:
                kinder_dialog_set(KINDER_NO_MESSAGE, KINDER_WHOLE_BANK);
                menu->timer = PAUSE_FRAMES;
                menu->state = KINDER_ADVICE_LIST_OPEN;
                topic_list_create(task);
                sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                menu->step = ADVICE_WAIT_LIST;
                break;
            }
            return;
        }
        break;
    case ADVICE_WAIT_LIST:
        if (kinder_advice_state() == KINDER_ADVICE_LIST_CLOSED) {
            kinder_dialog_set(choice_message[menu->selected],
                              KINDER_WHOLE_BANK);
            sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
            menu->step = ADVICE_CHOOSE;
        } else if (kinder_advice_state() >= KINDER_ADVICE_ABSENT) {
            menu->state = KINDER_ADVICE_UNREACHED;
            menu->timer = PAUSE_FRAMES;
        }
        break;
    case ADVICE_FAREWELL:
        if (menu->timer > 0) {
            menu->timer--;
        }
        if ((pad_press_repeat[0] & PDD_DGT_TA) || menu->timer == 0) {
            menu->step = ADVICE_FINISHED;
            kinder_advice_set_state(KINDER_ADVICE_LEAVING);
        }
        break;
    case ADVICE_FINISHED:
        break;
    }
}

static void advice_destroy(struct task *task)
{
    task_destroy_children(task);
    menu->task = NULL;
    syFree(menu);
    menu = NULL;
    kinder_message_table(NULL, KINDER_SERVICE_BANK);
}

struct task *kinder_advice_create(struct task *parent)
{
    struct task *task;
#ifdef NONMATCHING
    /* the topics end in the exit row, which has no page */
    if (topics[EXIT_TOPIC].page != KINDER_NO_MESSAGE ||
        !kinder_script_bank(KINDER_WHOLE_BANK) ||
        !kinder_string_bank(KINDER_WHOLE_BANK) ||
        !kinder_string_bank(KINDER_SERVICE_BANK)) {
        return NULL;
    }
#endif
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, advice_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = advice_destroy;
    menu = syCalloc(1, sizeof(*menu));
    if (!menu) {
        task_destroy(task);
        return NULL;
    }
    menu->task = task;
    menu->state = KINDER_ADVICE_OPEN;
    kinder_message_table(advice_messages, KINDER_SERVICE_BANK);
    return task;
}

void kinder_advice_close(void)
{
    if (!menu) {
        return;
    }
    task_kill(menu->task);
}

Sint8 kinder_advice_state(void)
{
    if (!menu) {
        return KINDER_ADVICE_ABSENT;
    }
    return menu->state;
}

void kinder_advice_set_state(Sint8 state)
{
    if (menu != NULL) {
        menu->state = state;
    }
}

static void topic_list_labels(struct task *task)
{
    struct kinder_message_label label;
    struct service_box box;
    struct topic_list *list;
    Sint8 index;
    int length;
    const char *title;
    char *text;
    Sint8 i;
    Sint8 top;

    /* @hack redundant: the read of task->work, which nothing uses */
    (void)task->work;
    list = task->extra.p;
    label = row_label;
    box = first_row;
    label.selection = (const Uint8 *)&list->highlighted;
    label.pos.x = box.pos.x + (box.width * 0.5F);
    for (i = 0; i < (Sint8)TOPIC_ROWS; i++) {
        label.id = i;
        top = list->top;
        index = i + top;
        label.pos.y = box.pos.y + (box.height * 0.5F);
        label.pos.z = box.pos.z + SERVICE_LABEL_DEPTH;
        label.width = box.width - SERVICE_LABEL_INSET;
        if (index != EXIT_TOPIC) {
            title =
                kinder_message_text(topics[index].title, KINDER_SERVICE_BANK);
        } else {
            title = kinder_message_text(EXIT_ROW_MESSAGE, KINDER_WHOLE_BANK);
        }
        if (topics[index].part) {
            length = strlen(title) + 2;
        } else {
            length = strlen(title);
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it makes room
         * for the terminator */
        text = syCalloc(1, length + 1);
#else
        /* @bug memory: a title without a part overruns its buffer by one
         * byte. syCalloc at 0x0c9ccf5a takes strlen(title) bytes, and
         * strncat at 0x0c9ccf6a writes the terminator past them */
        text = syCalloc(1, length);
#endif
        if (!text) {
            break;
        }
        strncat(text, title, length);
        if (topics[index].part) {
            strncat(text, topics[index].part, length);
        }
        label.text = text;
        list->labels[i] = kinder_message_label_fade_in(task, &label, 0);
        syFree(text);
        box.pos.y += box.height + SERVICE_ROW_GAP;
    }
}

#ifdef NONMATCHING
/* the plain form the hack tag names: no continue */
#define ROW_START()
#else
#define ROW_START()                                                            \
    /* @hack redundant: a continue that no row takes, which FADE_ROWS's loop   \
     * and LIST_SHOW's loop over the rows open with */                         \
    if (0) {                                                                   \
        continue;                                                              \
    }
#endif

#define FADE_ROWS(list, i)                                                     \
    for ((i) = 0; (i) < (Sint8)TOPIC_ROWS; (i)++) {                            \
        ROW_START();                                                           \
        kinder_message_label_fade_out((list)->labels[i]);                      \
        (list)->labels[i] = NULL;                                              \
    }

static void topic_list_step(struct task *task)
{
    struct service_box box;
    struct service_choice choice;
    struct task_work *work = task->work;
    struct topic_list *list = task->extra.p;
    Sint8 i;

    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case LIST_OPEN:
        work->timer = LIST_DELAY_FRAMES;
        work->mode = LIST_GREETING;
        work->submode = 0;
        break;
    case LIST_GREETING:
        if (work->timer > 0) {
            work->timer--;
        } else {
            kinder_dialog_set(LIST_GREETING_MESSAGE, KINDER_WHOLE_BANK);
            kinder_dialog_show_arrow();
            work->timer = LIST_GREETING_FRAMES;
            work->mode = LIST_SHOW;
            work->submode = 0;
        }
        break;
    case LIST_SHOW:
        if (work->timer > 0) {
            work->timer--;
        }
        if (work->timer == 0 || (pad_press_repeat[0] & ADVANCE_BUTTONS)) {
            service_panel_create(task, &list_box, &list_frame);
            box = first_row;
            choice = row_colors;
            choice.selection = &list->highlighted;
            for (i = 0; i < (Sint8)TOPIC_ROWS; i++) {
                ROW_START();
                service_button_create(task, i, &box, &choice);
                box.pos.y += box.height + SERVICE_ROW_GAP;
            }
            topic_list_labels(task);
#ifdef NONMATCHING
            /* this side keeps the tag's result, as kinder_dialog_hide_arrow
             * reads no argument. it calls the function by its own type, since a
             * call through another function type is undefined C */
            work->timer = LABELS_FRAMES;
            kinder_dialog_hide_arrow();
#else
            {
                int row = list->highlighted;
                work->timer = LABELS_FRAMES;
                /* @bug mismatched-call: the list passes the highlighted row
                 * to kinder_dialog_hide_arrow at 0x0c9cd16e, which takes no
                 * argument */
                ((void (*)(int))kinder_dialog_hide_arrow)(row);
            }
#endif
            sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
            work->mode = LIST_WAIT_LABELS;
            work->submode = 0;
        }
        break;
    case LIST_WAIT_LABELS:
        list->highlighted = NO_SELECTION;
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->mode = LIST_CHOOSE;
            work->submode = 0;
        }
        break;
    case LIST_CHOOSE:
        if (pad_press_repeat[0] & PDD_DGT_TB) {
            if (list->selected == EXIT_TOPIC) {
                work->timer = CLOSE_LIST_FRAMES;
                work->mode = LIST_CLOSING;
                work->submode = 0;
                return;
            }
            list->top = EXIT_TOPIC - TOPIC_ROWS + 1;
            list->cursor = TOPIC_ROWS - 1;
            FADE_ROWS(list, i);
            work->mode = LIST_SCROLL;
            work->submode = 0;
            work->timer = STEP_FRAMES;
            sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_TA) {
            if (list->selected == EXIT_TOPIC) {
                work->timer = CLOSE_LIST_FRAMES;
                work->mode = LIST_CLOSING;
                work->submode = 0;
                return;
            }
            work->timer = 0;
            list->page = topic_page_create(task, &list->reading, list->selected,
                                           LAST_TOPIC);
            sound_play(PAGE_SOUND, NULL, 0, 0);
            work->mode = LIST_READ_PAGE;
            work->submode = 0;
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_KU) {
            if (list->cursor > 0) {
                sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
                list->cursor--;
            } else if (list->top > 0) {
                list->top--;
                FADE_ROWS(list, i);
                work->timer = STEP_FRAMES;
                sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
                work->mode = LIST_SCROLL;
                work->submode = 0;
                return;
            }
        }
        if (pad_press_repeat[0] & PDD_DGT_KD) {
            if (list->cursor < TOPIC_ROWS - 1) {
                sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
                list->cursor++;
            } else if (list->top + TOPIC_ROWS - 1 < EXIT_TOPIC) {
                list->top++;
                work->timer = STEP_FRAMES;
                FADE_ROWS(list, i);
                sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
                work->mode = LIST_SCROLL;
                work->submode = 0;
                return;
            }
        }
        list->highlighted = list->cursor;
        list->selected = list->top + list->highlighted;
        break;
    case LIST_SCROLL:
        list->highlighted = NO_SELECTION;
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->timer = STEP_FRAMES;
            topic_list_labels(task);
            work->mode = LIST_WAIT_LABELS;
            work->submode = 0;
        }
        break;
    case LIST_READ_PAGE:
        if (work->timer > 0) {
            work->timer--;
        } else if (list->reading != 1) {
            if (list->page) {
                task_kill(list->page);
                list->page = NULL;
            }
            work->timer = STEP_FRAMES;
            work->mode = LIST_CHOOSE;
            work->submode = 0;
            return;
        }
        break;
    case LIST_CLOSING:
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->mode = LIST_DONE;
            work->submode = 0;
        }
        break;
    case LIST_DONE:
        kinder_advice_set_state(KINDER_ADVICE_LIST_CLOSED);
        task_kill(task);
        return;
    }
    if (work->mode != LIST_DONE) {
        kinder_advice_set_state(KINDER_ADVICE_LIST_OPEN);
    }
    list->reading = 0;
}

static void topic_list_draw(struct task *task)
{
}

static void topic_list_destroy(struct task *task)
{
    task_destroy_children(task);
}

static struct task *topic_list_create(struct task *parent)
{
    struct task *task;
    struct topic_list *list;

    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, topic_list_step, parent);
    if (!task) {
        return NULL;
    }
    task->draw = topic_list_draw;
    task->destructor = topic_list_destroy;
    list = syCalloc(1, sizeof(*list));
    if (!list) {
        task_destroy(task);
        /* @bug memory: where the allocation fails, task_destroy frees the task
         * at 0x0c9cd4ea, and the store to task->extra at 0x0c9cd4f0 writes
         * into it */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns before the
         * store */
        return NULL;
#endif
    }
    task->extra.p = list;
    return task;
}

static void topic_page_step(struct task *task)
{
    struct kinder_message_label label;
    struct topic_page_work *work = (struct topic_page_work *)task->work;
    struct topic_page *page;
    const char *title;
    char *text;
    int length;
    int index;
    Sint8 *reading = work->reading;

    page = task->extra.p;
    if (game_state.time_stopped) {
        return;
    }
    if (work->mode < PAGE_DONE) {
        *reading = 1;
    }
    switch (work->mode) {
    case PAGE_OPEN:
        index = page->index;
        label = title_label;
        label.pos.x = PAGE_LEFT;
        label.pos.y = TITLE_TOP;
        label.pos.z = PAGE_DEPTH;
        label.width = TITLE_WIDTH;
        title = kinder_message_text(topics[index].title, KINDER_SERVICE_BANK);
        if (topics[index].part) {
            length = strlen(title) + 2;
        } else {
            length = strlen(title);
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side: it makes room
         * for the terminator */
        text = syCalloc(1, length + 1);
#else
        /* @bug memory: a title without a part overruns its buffer by one
         * byte. syCalloc at 0x0c9cd650 takes strlen(title) bytes, and strcat
         * at 0x0c9cd65e writes the terminator past them */
        text = syCalloc(1, length);
#endif
        if (text) {
            strcat(text, title);
            if (topics[index].part) {
                strcat(text, topics[index].part);
            }
            label.text = text;
            page->title = kinder_message_label_fade_in(task, &label, 0);
            syFree(text);
        }
        label.bank = KINDER_SERVICE_BANK;
        label.flags = KINDER_MESSAGE_LABEL_FIT;
        label.pos.x = PAGE_LEFT;
        label.pos.y = BODY_TOP;
        label.pos.z = PAGE_DEPTH;
        label.color = BODY_COLOR;
        label.scale = 1.0F;
        label.width = BODY_WIDTH;
        label.message = topics[index].page;
        label.text = NULL;
        page->body = kinder_message_label_page(task, &label);
        if (page->body && page->title) {
            work->timer = PAGE_TEXT_FRAMES;
            work->mode = PAGE_WAIT_TEXT;
            work->submode = 0;
            break;
        } else {
            work->mode = PAGE_DONE;
            work->submode = 0;
            return;
        }
    case PAGE_WAIT_TEXT:
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->mode = PAGE_CHOOSE;
            work->submode = 0;
        }
        break;
    case PAGE_CHOOSE:
        if (work->timer > 0) {
            work->timer--;
            break;
        }
        if (pad_press_repeat[0] & PDD_DGT_TB) {
            if (page->button == PAGE_EXIT) {
                kinder_message_label_fade_out(page->title);
                page->title = NULL;
                task_kill(page->body);
                page->body = NULL;
                work->timer = PAGE_TEXT_FRAMES;
                work->mode = PAGE_CLOSING;
                work->submode = 0;
            } else {
                page->button = PAGE_EXIT;
            }
            work->timer = PAGE_KEY_FRAMES;
            sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_TA) {
            switch (page->button) {
            case PAGE_EXIT:
                kinder_message_label_fade_out(page->title);
                page->title = NULL;
                task_kill(page->body);
                page->body = NULL;
                work->timer = PAGE_TEXT_FRAMES;
                sound_play(SERVICE_BACK_SOUND, NULL, 0, 0);
                work->mode = PAGE_CLOSING;
                work->submode = 0;
                break;
            case PAGE_NEXT:
                if (page->index < page->last) {
                    page->index++;
                    kinder_message_label_fade_out(page->title);
                    page->title = NULL;
                    task_kill(page->body);
                    page->body = NULL;
                    work->timer = PAGE_TEXT_FRAMES;
                    sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                    work->mode = PAGE_TURN;
                    work->submode = 0;
                }
                break;
            case PAGE_BACK:
                if (page->index > 0) {
                    page->index--;
                    kinder_message_label_fade_out(page->title);
                    page->title = NULL;
                    task_kill(page->body);
                    page->body = NULL;
                    work->timer = PAGE_TEXT_FRAMES;
                    sound_play(SERVICE_ACCEPT_SOUND, NULL, 0, 0);
                    work->mode = PAGE_TURN;
                    work->submode = 0;
                }
            }
            return;
        }
        if ((pad_press_repeat[0] & PDD_DGT_KR) && page->button > PAGE_EXIT) {
            work->timer = PAGE_CURSOR_FRAMES;
            page->button--;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        if ((pad_press_repeat[0] & PDD_DGT_KL) && page->button < PAGE_BACK) {
            work->timer = PAGE_CURSOR_FRAMES;
            page->button++;
            sound_play(SERVICE_CURSOR_SOUND, NULL, 0, 0);
            return;
        }
        break;
    case PAGE_TURN:
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->mode = PAGE_OPEN;
            work->submode = 0;
        }
        break;
    case PAGE_CLOSING:
        if (work->timer > 0) {
            work->timer--;
        } else {
            work->mode = PAGE_DONE;
            work->submode = 0;
        }
        break;
    case PAGE_DONE:
        break;
    }
}

static void topic_page_destroy(struct task *task)
{
    task_destroy_children(task);
    kinder_message_texture_load();
}

static struct task *topic_page_create(struct task *parent, Sint8 *reading,
                                      int index, int last)
{
    struct service_box box;
    struct service_choice choice;
    struct task *task;
    struct topic_page_work *work;
    struct topic_page *page;

    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, topic_page_step, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = topic_page_destroy;
    work = (struct topic_page_work *)task->work;
    page = syCalloc(1, sizeof(*page));
    if (!page) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = page;
    service_page_create(task, &page_box);
    work->reading = reading;
    work->timer = PAGE_OPEN_FRAMES;
    page->index = index;
    page->last = last;
    *reading = 1;
    page->button = PAGE_EXIT;
    box.pos.x = EXIT_LEFT;
    box.pos.y = BUTTON_TOP;
    box.pos.z = BUTTON_DEPTH;
    box.color = 0;
    box.width = BUTTON_WIDTH;
    box.height = BUTTON_HEIGHT;
    box.flags = SERVICE_TRACK_SELECTION;
    choice.selected_color = BUTTON_SELECTED_COLOR;
    choice.selection = &page->button;
    service_text_button(task, PAGE_EXIT, &box, &choice, exit_text);
    box.pos.x = BACK_LEFT;
    service_text_button(task, PAGE_BACK, &box, &choice, back_text);
    box.pos.x = NEXT_LEFT;
    service_text_button(task, PAGE_NEXT, &box, &choice, next_text);
    return task;
}
