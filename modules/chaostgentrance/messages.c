/* the menu's message bar, whose text scrolls from right to left along a band
 * at the screen's foot */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chao/messages.h"
#include "chaostgentrance/menu.h"
#include "draw/screen.h"
#include "draw/text_sprite.h"
#include "system/task.h"

#define MESSAGE_PARTS 8
/* entrance_message_append adds a part only while the bar holds fewer than
 * APPENDED_PARTS */
#define APPENDED_PARTS 4
#define TEXT_BUFFER 1024
/* the bar's modes. nothing enters MESSAGE_FADE_OUT, so the bar stays until
 * the menu's task destroys it */
enum message_mode {
    MESSAGE_DELAY,
    MESSAGE_FADE_IN,
    MESSAGE_IDLE,
    MESSAGE_BUILD,
    MESSAGE_SCROLL,
    MESSAGE_REPLACE,
    MESSAGE_FADE_OUT
};

struct message_part {
    Sint8 id, count;
};

struct message_bar {
    Sint8 mode, submode;
    Uint8 unk_02, alpha;
    Uint16 unk_04, delay;
    Sint8 current; /* the part shown, or -1 for none */
    Sint8 replaced; /* a set replaced the parts since the last build */
    Uint8 count; /* the parts held */
    Uint8 text_delay;
    struct message_part parts[MESSAGE_PARTS];
    Uint8 unk_1c[12];
    Float text_x;
    Sint32 unk_2c;
    Sint16 text_alpha, unk_32;
};

#define BAND_TOP 413
#define BAND_BOTTOM 445
#define BAND_DEPTH 0.1F
#define TEXT_TOP 417
#define OPAQUE 255
#define FADE_IN 7
#define FADE_OUT 8
#define TEXT_FADE 20
/* a built text starts at the screen's right edge, and shows TEXT_DELAY
 * frames later. it scrolls until its end is SCROLL_OFF pixels past the left
 * edge. the next part builds then */
#define TEXT_DELAY 10
#define SCROLL_OFF 30
#define SCROLL_SPEED 2

/* the text of the part shown */
static struct text_sprite *message_sprite = NULL;
/* while second_sprite is set, bar_draw draws the text again, second_width
 * pixels on */
static struct text_sprite *second_sprite = NULL;
static Float scroll_speed = SCROLL_SPEED;
static Uint16 text_width, text_height, second_width;

static void bar_draw(struct task *task)
{
    struct message_bar *bar = (struct message_bar *)task->work;
    NJS_POLYGON_VTX quad[4];
    NJS_POINT3 pos;
    Uint32 color;
    Uint32 band;
    Float scale;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    /* @hack redundant: band set to 0 before the alpha is or'ed in, where an
     * assignment of the alpha would do */
    band = 0;
    band |= (Uint32)bar->alpha << 24;
#define BAND_CORNER(i, px, py)                                                 \
    quad[i].x = (px);                                                          \
    quad[i].y = (py);                                                          \
    quad[i].z = BAND_DEPTH;                                                    \
    quad[i].col = band
    BAND_CORNER(0, 0, BAND_TOP);
    BAND_CORNER(1, 0, BAND_BOTTOM);
    BAND_CORNER(2, SCREEN_WIDTH, BAND_TOP);
    BAND_CORNER(3, SCREEN_WIDTH, BAND_BOTTOM);
#undef BAND_CORNER
    njDrawPolygon(quad, 4, TRUE);
    if (message_sprite && bar->current >= 0 && bar->text_delay == 0) {
        pos.x = bar->text_x;
        pos.y = TEXT_TOP;
        scale = 1;
        color = ((Uint32)bar->text_alpha << 24) | 0xffffff;
        text_sprite_draw_text(message_sprite, pos.x, pos.y, scale, scale,
                              &color);
        if (second_sprite) {
            text_sprite_draw_text(message_sprite,
                                  pos.x + (second_width * scale), pos.y, scale,
                                  scale, &color);
        }
    }
}

static void bar_destroy(struct task *task)
{
    if (message_sprite) {
        text_sprite_destroy(message_sprite);
    }
    message_sprite = NULL;
    entrance_menu->message = NULL;
}

static void bar_step(struct task *task)
{
    struct message_bar *bar = (struct message_bar *)task->work;
    char text[TEXT_BUFFER];
    /* @hack type: Sint8 locals, where ints would do */
    Sint8 index;
    Sint8 count;
    switch (bar->mode) {
    case MESSAGE_DELAY:
        if (bar->delay-- == 0) {
            bar->mode = MESSAGE_FADE_IN;
            bar->submode = 0;
        }
        break;
    case MESSAGE_FADE_IN:
        if (bar->alpha < OPAQUE - FADE_IN) {
            bar->alpha += FADE_IN;
            entrance_menu->wait = TRUE;
        } else {
            bar->alpha = OPAQUE;
            bar->mode = MESSAGE_IDLE;
            bar->submode = 0;
        }
        break;
    case MESSAGE_IDLE:
        if (bar->current >= 0) {
            bar->mode = MESSAGE_BUILD;
            bar->submode = 0;
        }
        break;
    case MESSAGE_BUILD:
        if (bar->current < 0) {
            if (message_sprite) {
                text_sprite_destroy(message_sprite);
                message_sprite = NULL;
            }
            bar->mode = MESSAGE_IDLE;
            bar->submode = 0;
        } else {
            if (bar->current < bar->count - 1) {
                if (!bar->replaced) {
                    bar->current++;
                }
            } else {
                bar->current = 0;
            }
            if (message_sprite) {
                text_sprite_destroy(message_sprite);
            }
            count = bar->parts[bar->current].count;
            text[0] = '\0';
            for (index = 0; index < count; index++) {
                strcat(text,
                       garden_message(bar->parts[bar->current].id + index));
            }
            message_sprite =
                garden_message_sprite(text, &text_width, &text_height);
            bar->text_x = SCREEN_WIDTH;
            bar->text_alpha = OPAQUE;
            bar->text_delay = TEXT_DELAY;
            bar->replaced = FALSE;
            bar->mode = MESSAGE_SCROLL;
            bar->submode = 0;
        }
        break;
    case MESSAGE_SCROLL:
        if (bar->text_delay > 0) {
            bar->text_delay--;
        } else if (bar->text_x < -SCROLL_OFF - text_width) {
            bar->mode = MESSAGE_BUILD;
            bar->submode = 0;
        }
        if (bar->replaced) {
            bar->mode = MESSAGE_REPLACE;
            bar->submode = 0;
        }
        break;
    case MESSAGE_REPLACE:
        bar->text_alpha -= TEXT_FADE;
        if (bar->text_alpha < 0) {
            bar->text_alpha = 0;
            bar->mode = MESSAGE_BUILD;
            bar->submode = 0;
        }
        break;
    case MESSAGE_FADE_OUT:
        bar->text_alpha -= TEXT_FADE;
        if (bar->text_alpha < 0) {
            bar->text_alpha = 0;
        }
        /* @hack spelling: > FADE_OUT - 1, where >= FADE_OUT would do */
        if (bar->alpha > FADE_OUT - 1) {
            bar->alpha -= FADE_OUT;
            entrance_menu->wait = TRUE;
        } else {
            task_kill(task);
        }
        break;
    }
    bar->text_x -= scroll_speed;
    /* @hack redundant: an empty test for a leave request */
    if (bar->mode < MESSAGE_FADE_OUT &&
        entrance_menu->request == ENTRANCE_MENU_LEAVE) {
    }
}

void entrance_message_create(struct task *parent, Sint16 delay)
{
    struct task *task;
    struct message_bar *bar;
    if (!entrance_menu->message) {
        task = task_create_child(0, bar_step, parent);
        task->draw = bar_draw;
        task->destructor = bar_destroy;
        bar = syCalloc(1, sizeof(*bar));
        task->work = (struct task_work *)bar;
        /* @hack type: the delay set through the task's work, where
         * bar->delay would do */
        task->work->timer = delay;
        bar->current = -1;
        /* @hack spelling: the first 4 parts set a field at a time, where a
         * loop would do */
        bar->parts[0].id = -1;
        bar->parts[1].id = -1;
        bar->parts[2].id = -1;
        bar->parts[3].id = -1;
        bar->parts[0].count = 1;
        bar->parts[1].count = 1;
        bar->parts[2].count = 1;
        bar->parts[3].count = 1;
        entrance_menu->message = task;
    }
}

/* @hack spelling: the sets share a macro and the appends another, where
 * entrance_message_set and entrance_message_append could call their _group
 * twins */
#define SET_PART(first_id, length)                                             \
    do {                                                                       \
        struct message_bar *bar =                                              \
            (struct message_bar *)entrance_menu->message->work;                \
        if ((first_id) < 0) {                                                  \
            bar->current = -1;                                                 \
        } else {                                                               \
            bar->current = 0;                                                  \
        }                                                                      \
        bar->replaced = TRUE;                                                  \
        bar->parts[0].id = (first_id);                                         \
        bar->parts[0].count = (length);                                        \
        bar->count = 1;                                                        \
        scroll_speed = SCROLL_SPEED;                                           \
    } while (0)

#define APPEND_PART(first_id, length, limit)                                   \
    do {                                                                       \
        struct message_bar *bar =                                              \
            (struct message_bar *)entrance_menu->message->work;                \
        /* @hack spelling: > limit - 1, where >= limit would do */             \
        if (bar->count > (limit) - 1) {                                        \
            return;                                                            \
        }                                                                      \
        bar->parts[bar->count].id = (first_id);                                \
        bar->parts[bar->count].count = (length);                               \
        bar->count++;                                                          \
    } while (0)

void entrance_message_set(int id)
{
    SET_PART((Sint8)id, 1);
}

void entrance_message_append(Sint8 id)
{
    APPEND_PART(id, 1, APPENDED_PARTS);
}

void entrance_message_set_group(Sint8 id, Sint8 count)
{
    SET_PART(id, count);
}

void entrance_message_append_group(Sint8 id, Sint8 count)
{
    APPEND_PART(id, count, MESSAGE_PARTS);
}

void entrance_message_speed(Float speed)
{
    scroll_speed = speed;
}
