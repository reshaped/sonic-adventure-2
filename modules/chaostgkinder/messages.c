#include <machine.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>
#include "chao/messages.h"
#include "chaostgkinder/message_data.h"
#include "chaostgkinder/messages.h"
#include "chaostgkinder/service_menu.h"
#include "draw/screen.h"
#include "draw/text_sprite.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"

#define DIALOG_PAGES 4
/* the bytes of a string file's path, its terminator included */
#define PATH_SIZE 256
#define MAX_PAGE_WIDTH 512
/* the widest character pitch a compact page tries, and its line pitch */
#define WIDEST_PITCH 2
#define LINE_PITCH 2
#define LABEL_SELECTED 0x100
#define LABEL_TRACK 0x200
#define RGB_MASK 0xffffffU
/* a texture's far edge, as njDrawPolygon2D's texture coordinates count */
#define TEXTURE_EDGE 256UL

/* the dialog's band, its page's top left corner and its arrow. the arrow
 * blinks every ARROW_BLINK frames */
#define BAND_TOP 368.0F
#define BAND_BOTTOM 438.0F
#define BAND_DEPTH 0.1F
#define TEXT_LEFT 20.0F
#define TEXT_TOP 373.0F
#define ARROW_X 580
#define ARROW_Y 404
#define ARROW_DEPTH 0.4F
#define ARROW_BLINK 30
/* the frames a page waits before its text shows, and its text's fade per
 * frame as it turns */
#define TEXT_DELAY 10
#define TURN_STEP 20
/* the band's fade in per frame as the dialog opens, and its fade out as it
 * closes */
#define OPEN_STEP 7
#define CLOSE_STEP 8
#define OPAQUE 255

enum dialog_mode {
    DIALOG_DELAY,
    DIALOG_OPEN,
    DIALOG_IDLE,
    DIALOG_LOAD,
    DIALOG_READ,
    DIALOG_TURN,
    DIALOG_CLOSE
};

struct dialog_work {
    Sint8 mode, submode;
    Uint8 reserved_02, alpha;
    Uint16 reserved_04, timer;
#ifdef NONMATCHING
    /* this side declares page signed, so the tests of page < 0 below hold for
     * the dialog's -1. the matching side's Uint8 reads that -1 as 255 */
    Sint8 page;
#else
    Uint8 page;
#endif
    Uint8 advance, count, wait;
    Sint16 messages[DIALOG_PAGES], banks[DIALOG_PAGES];
    Uint8 reserved_1c[12];
    Float x;
    Uint8 reserved_2c[4];
    Sint16 text_alpha;
    Uint8 reserved_32[2];
    Bool show_arrow, use_text;
    Uint8 reserved_3c[4];
    int style;
    char *text;
};

struct message_label_work {
    Sint8 mode, submode;
    Uint8 id, selection_offset;
    Sint16 flags;
    Uint8 reserved_06[6];
    const Uint8 *selection;
    struct text_sprite *sprite;
    NJS_POINT3 pos;
    Float scale;
};
/* a label's size on the screen and its colors, in its task's motion record */
struct message_label_style {
    Float width, height;
    Uint8 reserved_08[16];
    Uint32 color, selected_color;
    Sint32 alpha;
};

static Uint16 dialog_width, dialog_height;
#define DIALOG_WORK(task) ((struct dialog_work *)(task)->work)
#define LABEL_WORK(task) ((struct message_label_work *)(task)->work)

#ifdef NONMATCHING
#else
#pragma inline(kinder_string_bank, kinder_script_bank)
#endif

Sint32 *kinder_messages_load(const char *name, Sint8 bank)
{
    char path[PATH_SIZE];
    Sint32 *strings;
    Sint32 **slot;
    kinder_messages_release(bank);
#ifdef NONMATCHING
    /* this side loads nothing for a name too long for path with its suffix. it
     * copies the name by strcpy, the hack's plain form */
    if (strlen(name) > sizeof(path) - sizeof("_j.prs")) {
        return NULL;
    }
    strcpy(path, name);
#else
    /* @hack spelling: _builtin_strcpy, where strcpy would do */
    _builtin_strcpy(path, name);
#endif
    strcat(path, kinder_message_suffixes[game_state.text_language]);
    strcat(path, ".prs");
    strings = loader_read_prs_alloc(path);
    if (!strings) {
        return NULL;
    }
    if (bank == KINDER_SERVICE_BANK) {
        slot = &kinder_service_strings;
    } else {
        slot = &kinder_room_strings;
    }
    *slot = strings;
    return strings;
}
void kinder_messages_release(Sint8 bank)
{
    /* @hack redundant: the test of each script before it is cleared, where
     * clearing it would do */
    if (bank == KINDER_SERVICE_BANK) {
        if (kinder_service_strings) {
            syFree(kinder_service_strings);
            kinder_service_strings = NULL;
        }
        if (kinder_service_script) {
            kinder_service_script = NULL;
        }
    } else {
        if (kinder_room_strings) {
            syFree(kinder_room_strings);
            kinder_room_strings = NULL;
        }
        if (kinder_room_script) {
            kinder_room_script = NULL;
        }
    }
}

char *kinder_message_text(int message, Sint8 bank)
{
    Sint32 *strings = kinder_string_bank(bank);
    struct kinder_message_entry *entries = kinder_script_bank(bank);
    int text;
    if (!entries) {
        return NULL;
    }
    text = entries[message].text;
    if (!strings) {
        return NULL;
    }
    return (char *)strings + strings[text];
}
int kinder_message_text_index(int message, Sint8 bank)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: the read of the bank's strings, which nothing uses */
    Sint32 *strings = kinder_string_bank(bank);
#endif
    struct kinder_message_entry *entries = kinder_script_bank(bank);
    /* @hack spelling: the entry through a local, where entries[message].text
     * would do */
    struct kinder_message_entry *entry;
    if (!entries) {
        return -1;
    }
    entry = &entries[message];
    return entry->text;
}
int kinder_message_next_line(int message, Sint8 bank)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: the read of the bank's strings, which nothing uses */
    Sint32 *strings = kinder_string_bank(bank);
#endif
    struct kinder_message_entry *entries = kinder_script_bank(bank);
    int next;
    if (!entries) {
        return KINDER_NO_MESSAGE;
    }
    next = entries[message].next;
    if (next == KINDER_MESSAGE_CONTINUE) {
        return message + 1;
    } else if (next == KINDER_MESSAGE_PAGE) {
        return KINDER_NO_MESSAGE;
    }
    if (next == KINDER_MESSAGE_STOP) {
        return KINDER_NO_MESSAGE;
    }
    return next;
}
int kinder_message_next_page(int message, Sint8 bank)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: the read of the bank's strings, which nothing uses */
    Sint32 *strings = kinder_string_bank(bank);
#endif
    struct kinder_message_entry *entries = kinder_script_bank(bank);
    int next;
    if (entries) {
        next = entries[message].next;
        if (next != KINDER_MESSAGE_STOP) {
            while (next == KINDER_MESSAGE_CONTINUE) {
                message++;
                next = entries[message].next;
            }
            if (next == KINDER_MESSAGE_PAGE) {
                return message + 1;
            }
        }
    }
    return KINDER_NO_MESSAGE;
}
static struct text_sprite *message_sprite(const char *text, Uint16 *width,
                                          Uint16 *height)
{
    struct text_sprite *sprite;
    Uint16 measured_width;
    Uint16 measured_height;
    sprite = text_sprite_create(text, game_state.text_language);
    if (!sprite) {
        return NULL;
    }
    if (!width || !height) {
        text_sprite_measure(sprite, text, game_state.text_language,
                            &measured_width, &measured_height);
    } else {
        text_sprite_measure(sprite, text, game_state.text_language, width,
                            height);
    }
    return sprite;
}
static struct text_sprite *compact_message_sprite(const char *text,
                                                  Uint16 *width, Uint16 *height)
{
    struct text_sprite *sprite;
    Uint16 trial_width;
    Uint16 trial_height;
    Uint16 measured_width;
    Uint16 measured_height;
    struct text_sprite probe;
    Sint16 pitch;
    for (pitch = WIDEST_PITCH; pitch >= 0; pitch--) {
        probe.pitch_x = pitch;
        probe.pitch_y = pitch;
        text_sprite_measure(&probe, text, game_state.text_language,
                            &trial_width, &trial_height);
        if (trial_width < MAX_PAGE_WIDTH) {
            break;
        }
    }
    sprite = text_sprite_create_styled(text, game_state.text_language,
                                       TEXT_WHITE, pitch, LINE_PITCH);
    if (!sprite) {
        return NULL;
    }
    if (!width || !height) {
        text_sprite_measure(sprite, text, game_state.text_language,
                            &measured_width, &measured_height);
    } else {
        text_sprite_measure(sprite, text, game_state.text_language, width,
                            height);
    }
    return sprite;
}
Sint32 *kinder_string_bank(Sint8 bank)
{
    if (bank == KINDER_SERVICE_BANK) {
        return kinder_service_strings;
    } else {
        return kinder_room_strings;
    }
}
struct kinder_message_entry *kinder_script_bank(Sint8 bank)
{
    if (bank == KINDER_SERVICE_BANK) {
        return kinder_service_script;
    } else {
        return kinder_room_script;
    }
}
void kinder_message_table(struct kinder_message_entry *entries, Sint8 bank)
{
    if (bank == KINDER_SERVICE_BANK) {
        kinder_service_script = entries;
    } else {
        kinder_room_script = entries;
    }
}

#define PAGE_FUNCTION message_page
#define PAGE_FACTORY message_sprite
#include "chaostgkinder/message_page_impl.h"

void kinder_message_reset(void)
{
#ifdef NONMATCHING
    /* a first line that leads back to itself marks an empty script */
    if (kinder_default_script[0].next == 0) {
        kinder_room_script = NULL;
        return;
    }
#endif
    kinder_room_script = kinder_default_script;
}

static void dialog_step(struct task *task)
{
    struct dialog_work *work = DIALOG_WORK(task);
    switch (work->mode) {
    case DIALOG_DELAY:
        if (work->timer-- == 0) {
            work->mode = DIALOG_OPEN;
            work->submode = 0;
        }
        break;
    case DIALOG_OPEN:
        if (work->alpha < OPAQUE - OPEN_STEP) {
            work->alpha += OPEN_STEP;
        } else {
            work->alpha = OPAQUE;
            work->mode = DIALOG_IDLE;
            work->submode = 0;
        }
        break;
    case DIALOG_IDLE:
        /* @bug logic: an idle dialog tries to load a page every frame. page
         * is unsigned in dialog_step at 0x0c9ca4b0, so this test always
         * holds, and -1 reads as 255 */
        if (work->page >= 0) {
            work->mode = DIALOG_LOAD;
            work->submode = 0;
        }
        break;
    case DIALOG_LOAD:
        work->alpha = 255;
        if (work->page < 0) {
            if (kinder_dialog_sprite) {
                text_sprite_destroy(kinder_dialog_sprite);
                kinder_dialog_sprite = NULL;
            }
            work->show_arrow = FALSE;
            work->mode = DIALOG_IDLE;
            work->submode = 0;
        } else {
            if (work->page < work->count - 1) {
                if (!work->advance) {
                    work->page++;
                }
            } else {
                work->page = 0;
            }
            if (kinder_dialog_sprite) {
                text_sprite_destroy(kinder_dialog_sprite);
                kinder_dialog_sprite = NULL;
            }
            if (!work->use_text) {
                kinder_dialog_sprite = message_page(
                    work->messages[work->page], work->banks[work->page],
                    &dialog_width, &dialog_height);
            } else {
                kinder_dialog_sprite =
                    message_sprite(work->text, &dialog_width, &dialog_height);
                work->use_text = FALSE;
                syFree(work->text);
                work->text = NULL;
            }
            if (!kinder_dialog_sprite) {
                work->mode = DIALOG_IDLE;
                work->submode = 0;
                return;
            }
            work->x = TEXT_LEFT;
            work->text_alpha = 255;
            work->wait = TEXT_DELAY;
            work->advance = 0;
            work->mode = DIALOG_READ;
            work->submode = 0;
        }
        break;
    case DIALOG_READ:
        if (work->wait > 0) {
            work->wait--;
        }
        if (work->advance) {
            work->mode = DIALOG_TURN;
            work->submode = 0;
        }
        break;
    case DIALOG_TURN:
        work->text_alpha -= TURN_STEP;
        if (work->text_alpha < 0) {
            work->text_alpha = 0;
            work->mode = DIALOG_LOAD;
            work->submode = 0;
        }
        break;
    case DIALOG_CLOSE:
        work->show_arrow = FALSE;
        if (work->alpha > CLOSE_STEP - 1) {
            work->alpha -= CLOSE_STEP;
        } else {
            task_kill(task);
        }
        break;
    }
}

static void dialog_draw(struct task *task)
{
    struct dialog_work *work = DIALOG_WORK(task);
    NJS_POLYGON_VTX band[4];
    NJS_POINT3 pos;
    Uint32 color;
    Uint32 backdrop;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack redundant: the reads of wait and show_arrow, which nothing
     * uses */
    (void)(work->wait && work->show_arrow);
    backdrop = 0;
    backdrop |= (Uint32)work->alpha << 24;
#define BAND_VERTEX(i, px, py)                                                 \
    band[i].x = px;                                                            \
    band[i].y = py;                                                            \
    band[i].z = BAND_DEPTH;                                                    \
    band[i].col = backdrop
    BAND_VERTEX(0, 0.0F, BAND_TOP);
    BAND_VERTEX(1, 0.0F, BAND_BOTTOM);
    BAND_VERTEX(2, SCREEN_WIDTH, BAND_TOP);
    BAND_VERTEX(3, SCREEN_WIDTH, BAND_BOTTOM);
#undef BAND_VERTEX
    njDrawPolygon(band, 4, TRUE);
    if (kinder_dialog_sprite && !work->wait) {
        pos.x = work->x;
        pos.y = TEXT_TOP;
        color = ((Uint32)work->text_alpha << 24) | RGB_MASK;
        text_sprite_draw_text(kinder_dialog_sprite, pos.x, pos.y, 1, 1, &color);
        if (work->text_alpha == 255 && work->show_arrow == TRUE) {
            if (game_state.steps % ARROW_BLINK < ARROW_BLINK / 2) {
                service_arrow_draw(ARROW_X, ARROW_Y, ARROW_DEPTH);
            }
        }
    }
}

static void dialog_destroy(struct task *task)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
    if (kinder_dialog_sprite) {
        text_sprite_destroy(kinder_dialog_sprite);
    }
    kinder_dialog_sprite = NULL;
    if (work->text) {
        syFree(work->text);
        work->text = NULL;
    }
    kinder_dialog = NULL;
}

void kinder_dialog_create(struct task *parent, Sint16 delay, int style)
{
    struct task *task;
    struct dialog_work *work;
    if (kinder_dialog) {
        return;
    }
    if (!parent) {
        return;
    }
    task = task_create_child(0, dialog_step, parent);
#ifdef NONMATCHING
    /* this side makes no dialog where task_create_child fails. the matching
     * side writes through the null */
    if (!task) {
        return;
    }
#endif
    task->draw = dialog_draw;
    task->destructor = dialog_destroy;
    work = syCalloc(1, sizeof(*work));
    if (!work) {
        /* @bug memory: reads through a NULL kinder_dialog where the work
         * can't be allocated. kinder_dialog_create at 0x0c9ca826 destroys
         * task with dialog_destroy still its destructor, which reads
         * kinder_dialog's work */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the destroy runs no
         * destructor */
        task->destructor = NULL;
#endif
        task_destroy(task);
        return;
    }
    task->work = (struct task_work *)work;
    /* @hack spelling: the delay through task->work, where work would do */
    task->work->timer = delay;
    work->page = -1;
    work->messages[0] = KINDER_NO_MESSAGE;
    work->messages[1] = KINDER_NO_MESSAGE;
    work->messages[2] = KINDER_NO_MESSAGE;
    work->messages[3] = KINDER_NO_MESSAGE;
    work->style = style;
    work->mode = DIALOG_IDLE;
    kinder_dialog = task;
}
void kinder_dialog_set(Sint16 message, Sint8 bank)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
    if (message < 0) {
        work->page = -1;
    } else {
        work->page = 0;
    }
    work->advance = 1;
    work->messages[0] = message;
    work->banks[0] = bank;
    work->count = 1;
    work->messages[1] = KINDER_NO_MESSAGE;
    work->messages[2] = KINDER_NO_MESSAGE;
    work->messages[3] = KINDER_NO_MESSAGE;
    work->show_arrow = FALSE;
}
void kinder_dialog_append(Sint16 message, Sint8 bank)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
    if (work->count > DIALOG_PAGES - 1) {
        return;
    }
    work->messages[work->count] = message;
    work->banks[work->count] = bank;
    work->count++;
    work->show_arrow = FALSE;
}
void kinder_dialog_show_arrow(void)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
    work->show_arrow = TRUE;
}
void kinder_dialog_hide_arrow(void)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
    work->show_arrow = FALSE;
}
void kinder_dialog_set_text(const char *text, size_t length)
{
    struct dialog_work *work = DIALOG_WORK(kinder_dialog);
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. the copy takes the
     * text's length and its terminator, and the old text is freed */
    char *copy;
    if (length == (size_t)-1) {
        return;
    }
    copy = syCalloc(1, length + 1);
    if (!copy) {
        return;
    }
    if (work->text) {
        syFree(work->text);
    }
    work->text = copy;
    memcpy(copy, text, length);
#else
    /* @bug memory: writes past the copy's block for a text longer than 3
     * characters. kinder_dialog_set_text at 0x0c9ca9ba allocates
     * sizeof(text), a pointer's size */
    work->text = syCalloc(1, sizeof(text));
#endif
    if (work->text) {
#ifdef NONMATCHING
        /* this side's repair above has copied the text already */
#else
        /* @hack spelling: _builtin_strcpy, where strcpy would do */
        _builtin_strcpy(work->text, text);
#endif
        work->use_text = TRUE;
        work->page = 0;
        work->advance = 1;
        work->messages[0] = 0;
        work->banks[0] = -1;
        work->count = 1;
        work->messages[1] = KINDER_NO_MESSAGE;
        work->messages[2] = KINDER_NO_MESSAGE;
        work->messages[3] = KINDER_NO_MESSAGE;
        work->show_arrow = FALSE;
    }
}
static void label_sprite_draw(struct text_sprite *sprite, Float x, Float y,
                              Float sx, Float sy, Float depth,
                              const Uint32 *color);

static void message_label_step(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    const Uint8 *selection;
    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & LABEL_TRACK) {
        selection = work->selection;
        if ((work->flags & LABEL_SELECTED) && *selection != work->id) {
            work->flags &= ~LABEL_SELECTED;
        } else if (!(work->flags & LABEL_SELECTED) && *selection == work->id) {
            work->flags |= LABEL_SELECTED;
        }
    }
}
/* @hack spelling: the color through tint, where choosing color itself would
 * do */
#define DRAW_LABEL()                                                           \
    struct message_label_work *work = LABEL_WORK(task);                        \
    struct message_label_style *style = task->motion.p;                        \
    struct text_sprite *sprite = work->sprite;                                 \
    Uint32 tint;                                                               \
    Uint32 color;                                                              \
    if (!(work->flags & LABEL_SELECTED))                                       \
        tint = style->color;                                                   \
    else                                                                       \
        tint = style->selected_color;                                          \
    color = tint;                                                              \
    label_sprite_draw(sprite, work->pos.x, work->pos.y, work->scale,           \
                      work->scale, work->pos.z, &color)
static void message_label_draw(struct task *task)
{
    DRAW_LABEL();
}
static void message_label_destroy(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    struct text_sprite *sprite = work->sprite;
    /* @bug memory: passes text_sprite_destroy a NULL sprite where making the
     * label's sprite failed, and it reads through it. message_label_destroy
     * at 0x0c9caada tests nothing, where fading_label_destroy at 0x0c9caf86
     * tests sprite */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it destroys only a sprite that
     * was made */
    if (sprite)
#endif
        text_sprite_destroy(sprite);
    work->sprite = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
    syFree(task->work);
    task->work = NULL;
}
/* the level of a label made without a parent */
#define LABEL_LEVEL 2
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side: task takes the root
 * task_create makes */
#define CREATE_ROOT(task, step, name)                                          \
    ((task) = task_create(TASK_WORK | TASK_MOTION, LABEL_LEVEL, step, name))
#else
/* @bug uninitialized: drops the root task task_create makes, then tests task,
 * which nothing set, at 0x0c9cab28, 0x0c9cafde and 0x0c9cb420 */
#define CREATE_ROOT(task, step, name)                                          \
    task_create(TASK_WORK | TASK_MOTION, LABEL_LEVEL, step, name)
#endif

#define CREATE_LABEL(step, name, draw_fn, destroy_fn)                          \
    if (!parent) {                                                             \
        CREATE_ROOT(task, step, name);                                         \
    } else {                                                                   \
        task = task_create_child(TASK_WORK | TASK_MOTION, step, parent);       \
    }                                                                          \
    if (!task)                                                                 \
        return NULL;                                                           \
    task->draw = draw_fn;                                                      \
    task->destructor = destroy_fn

#define CREATE_SPRITE()                                                        \
    if (!(caption->flags & KINDER_MESSAGE_LABEL_LITERAL)) {                    \
        sprite =                                                               \
            message_page(caption->message, caption->bank, &width, &height);    \
    } else {                                                                   \
        sprite = garden_message_sprite(caption->text, &width, &height);        \
    }                                                                          \
    if (!sprite) {                                                             \
        task_destroy(task);                                                    \
        return NULL;                                                           \
    }                                                                          \
    work = LABEL_WORK(task);                                                   \
    style = task->motion.p;                                                    \
    work->sprite = sprite

#define TRACK_LABEL()                                                          \
    work->flags |= LABEL_TRACK;                                                \
    work->selection = caption->selection;                                      \
    work->id = caption->id

#define FIT_LABEL()                                                            \
    if (caption->width > (Float)width)                                         \
        work->scale = 1.0F;                                                    \
    else                                                                       \
        work->scale = caption->width / (Float)width

#define SCALE_LABEL()                                                          \
    if (caption->flags & KINDER_MESSAGE_LABEL_FIT) {                           \
        FIT_LABEL();                                                           \
        if (caption->scale != 0.0F && caption->scale != 1.0F) {                \
            work->scale *= caption->scale;                                     \
        }                                                                      \
    } else {                                                                   \
        work->scale = caption->scale;                                          \
    }

/* how far below pos a centered label's center sits, in pixels */
#define CENTER_DROP 2.0F
#define POSITION_LABEL()                                                       \
    if (caption->flags & KINDER_MESSAGE_LABEL_CENTER) {                        \
        work->pos.x = caption->pos.x - ((Float)width * work->scale * 0.5F);    \
        work->pos.y = caption->pos.y - ((Float)height * work->scale * 0.5F) +  \
                      CENTER_DROP;                                             \
        work->pos.z = caption->pos.z;                                          \
    } else {                                                                   \
        work->pos.x = caption->pos.x;                                          \
        work->pos.y = caption->pos.y;                                          \
        work->pos.z = caption->pos.z;                                          \
    }

struct task *
kinder_message_label_create(struct task *parent,
                            const struct kinder_message_label *caption)
{
    struct task *task;
    struct text_sprite *sprite;
    struct message_label_work *work;
    struct message_label_style *style;
    Uint16 width;
    Uint16 height;
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    CREATE_LABEL(message_label_step, "AL_KinderPMessageExec",
                 message_label_draw, message_label_destroy);
    CREATE_SPRITE();
    if (caption->flags & KINDER_MESSAGE_LABEL_TRACK) {
        TRACK_LABEL();
    }
    SCALE_LABEL();
    POSITION_LABEL();
    style->color = caption->color;
    style->selected_color = caption->selected_color;
    style->width = (Float)width * work->scale;
    style->height = (Float)height * work->scale;
    return task;
}
void kinder_message_label_size(struct task *task, Uint16 *width, Uint16 *height)
{
    struct text_sprite *sprite;
    struct message_label_style *style;
    *width = 0;
    *height = 0;
    if (task->work && task->motion.p) {
        sprite = LABEL_WORK(task)->sprite;
        style = task->motion.p;
        if (sprite) {
            *width = style->width;
            *height = style->height;
        }
    }
}
static void label_sprite_draw(struct text_sprite *sprite, Float x, Float y,
                              Float sx, Float sy, Float depth,
                              const Uint32 *color)
{
    Float x0 = x;
    Float y0 = y;
    Float x1 = x0 + (sprite->texture_width * sx);
    Float y1 = y0 + (sprite->texture_height * sy);
    if (sprite->texlist.textures) {
        NJS_COLOR colors[4];
        /* each word packs a corner's u in its low half and its v in the
         * high half. each runs from 0 to TEXTURE_EDGE. the words initialize
         * each union's first member, color */
        /* NOLINTBEGIN(clang-diagnostic-missing-braces) */
        NJS_COLOR corners[4] = {0, TEXTURE_EDGE << 16,
                                (TEXTURE_EDGE << 16) | TEXTURE_EDGE,
                                TEXTURE_EDGE};
        /* NOLINTEND(clang-diagnostic-missing-braces) */
        NJS_POINT2 points[4];
        NJS_POINT2COL polygon;
        points[0].x = points[1].x = x0;
        points[2].x = points[3].x = x1;
        points[0].y = points[3].y = y0;
        points[1].y = points[2].y = y1;
        colors[0].color = colors[1].color = colors[2].color = colors[3].color =
            color ? *color : sprite->color.color;
        polygon.p = points;
        polygon.col = colors;
        polygon.tex = corners;
        polygon.num = 4;
        njSetTexture(&sprite->texlist);
        njSetTextureNum(0);
        if (depth > -1.0F) {
            depth = -1.1F;
        }
        njDrawPolygon2D(&polygon, 4, depth,
                        NJD_FILL | NJD_TRANSPARENT | NJD_USE_TEXTURE);
    }
}

#define FADE_COLOR(color, alpha)                                               \
    (((color) & RGB_MASK) + ((Uint32)(alpha) << 24))
enum fade_mode { FADE_IN, FADE_HELD, FADE_OUT };
#define FADE_STEP 96
static void fading_label_step(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    struct message_label_style *style = task->motion.p;
    const Uint8 *selection;
    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & LABEL_TRACK) {
        selection = work->selection;
        if ((work->flags & LABEL_SELECTED) &&
            *selection != work->id + work->selection_offset) {
            work->flags &= ~LABEL_SELECTED;
        } else if (!(work->flags & LABEL_SELECTED) &&
                   *selection == work->id + work->selection_offset) {
            work->flags |= LABEL_SELECTED;
        }
    }
    switch (work->mode) {
    case FADE_IN:
        style->alpha += FADE_STEP;
        if (style->alpha > 255) {
            style->alpha = 255;
            style->color = FADE_COLOR(style->color, 255);
            style->selected_color = FADE_COLOR(style->selected_color, 255);
            work->mode = FADE_HELD;
            work->submode = 0;
        } else {
            style->color = FADE_COLOR(style->color, style->alpha);
            style->selected_color =
                FADE_COLOR(style->selected_color, style->alpha);
        }
        break;
    case FADE_HELD:
        break;
    case FADE_OUT:
        style->alpha -= FADE_STEP;
        if (style->alpha > 0) {
            style->color = FADE_COLOR(style->color, style->alpha);
            style->selected_color =
                FADE_COLOR(style->selected_color, style->alpha);
        } else {
            style->alpha = 0;
            style->color = style->color & RGB_MASK;
            style->selected_color = style->selected_color & RGB_MASK;
            task_kill(task);
        }
        break;
    }
}
static void fading_label_draw(struct task *task)
{
    DRAW_LABEL();
}
static void fading_label_destroy(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    struct text_sprite *sprite = work->sprite;
    if (sprite) {
        text_sprite_destroy(sprite);
        work->sprite = NULL;
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    syFree(task->work);
    task->work = NULL;
}
struct task *
kinder_message_label_fade_in(struct task *parent,
                             const struct kinder_message_label *caption,
                             Uint8 selection_offset)
{
    struct task *task;
    struct text_sprite *sprite;
    struct message_label_work *work;
    struct message_label_style *style;
    Uint16 width;
    Uint16 height;
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    CREATE_LABEL(fading_label_step, "AL_KinderPSMessageExec", fading_label_draw,
                 fading_label_destroy);
    CREATE_SPRITE();
    if (caption->flags & KINDER_MESSAGE_LABEL_TRACK) {
        TRACK_LABEL();
        work->selection_offset = selection_offset;
    }
    if (caption->flags & KINDER_MESSAGE_LABEL_FIT) {
        FIT_LABEL();
    } else {
        work->scale = caption->scale;
    }
    POSITION_LABEL();
    style->color = caption->color & RGB_MASK;
    style->selected_color = caption->selected_color & RGB_MASK;
    style->alpha = 0;
    return task;
}
void kinder_message_label_fade_out(struct task *caption)
{
    struct message_label_work *work = LABEL_WORK(caption);
    work->mode = FADE_OUT;
    work->submode = 0;
}

int kinder_message_insert(const char *text, const char *insert, char **result)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: a text without a
     * '<' gives no result */
    const char *marker = strchr(text, '<');
    size_t prefix;
    size_t suffix;
    size_t inserted;
    *result = NULL;
    if (!marker || !marker[1]) {
        return FALSE;
    }
    prefix = marker - text;
    suffix = strlen(marker + 2);
    inserted = strlen(insert);
    if (inserted > (size_t)-1 - prefix - suffix - 1) {
        return FALSE;
    }
    *result = syCalloc(1, prefix + inserted + suffix + 1);
    if (!*result) {
        return FALSE;
    }
    memcpy(*result, text, prefix);
    memcpy(*result + prefix, insert, inserted);
    memcpy(*result + prefix + inserted, marker + 2, suffix + 1);
#else
    /* @bug memory: reads past text's end where it holds no '<'.
     * kinder_message_insert at 0x0c9cb154 scans for the marker and tests for
     * nothing else */
    int length;
    const char *cursor;
    length = strlen(text) - 2;
    length += strlen(insert) + 1;
    *result = syCalloc(1, length);
    if (!*result) {
        return FALSE;
    }
    cursor = text;
    while (*cursor != '<') {
        strncat(*result, cursor, 1);
        cursor++;
    }
    strcat(*result, insert);
    cursor += 2;
    strcat(*result, cursor);
#endif
    return TRUE;
}

void kinder_message_texture_load(void)
{
    Uint16 width;
    Uint16 height;
    if (kinder_shared_sprite) {
        text_sprite_destroy(kinder_shared_sprite);
        kinder_shared_sprite = NULL;
    }
    kinder_shared_sprite =
        message_sprite(kinder_placeholder_text, &width, &height);
}
void kinder_message_texture_release(void)
{
    /* @hack spelling: the sprite through a local, where kinder_shared_sprite
     * would do */
    struct text_sprite *sprite = kinder_shared_sprite;
    if (sprite != NULL) {
        text_sprite_destroy(sprite);
        kinder_shared_sprite = NULL;
    }
}
static void shared_label_draw(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    struct message_label_style *style = task->motion.p;
    /* @hack spelling: the color through tint, where style->color would do */
    Uint32 tint = style->color;
    Uint32 color = tint;
    label_sprite_draw(kinder_shared_sprite, work->pos.x, work->pos.y,
                      work->scale, work->scale, work->pos.z, &color);
}
static void shared_label_destroy(struct task *task)
{
    struct message_label_work *work = LABEL_WORK(task);
    Uint16 width;
    Uint16 height;
    kinder_message_texture_release();
    kinder_shared_sprite =
        message_sprite(kinder_placeholder_text, &width, &height);
    work->sprite = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
    syFree(task->work);
    task->work = NULL;
}
#define PAGE_FUNCTION compact_page
#define PAGE_FACTORY compact_message_sprite
#include "chaostgkinder/message_page_impl.h"

struct task *
kinder_message_label_page(struct task *parent,
                          const struct kinder_message_label *caption)
{
    struct task *task;
    struct text_sprite *sprite;
    struct message_label_work *work;
    struct message_label_style *style;
    Uint16 width;
    Uint16 height;
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    CREATE_LABEL(message_label_step, "AL_KinderPMessageExec", shared_label_draw,
                 shared_label_destroy);
    if (kinder_shared_sprite) {
        text_sprite_destroy(kinder_shared_sprite);
        kinder_shared_sprite = NULL;
    }
    sprite = compact_page(caption->message, caption->bank, &width, &height);
    if (!sprite) {
        task_destroy(task);
        return NULL;
    }
    kinder_shared_sprite = sprite;
    work = LABEL_WORK(task);
    style = task->motion.p;
    work->sprite = kinder_shared_sprite;
    SCALE_LABEL();
    POSITION_LABEL();
    style->color = caption->color;
    style->selected_color = caption->selected_color;
    style->width = (Float)width * work->scale;
    style->height = (Float)height * work->scale;
    return task;
}
