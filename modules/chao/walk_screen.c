/* the walk menu's screen pieces, each a child task of the menu's */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "camera/camera.h"
#include "chao/egg.h"
#include "chao/garden.h"
#include "chao/life.h"
#include "chao/messages.h"
#include "chao/motion.h"
#include "chao/orchard_art.h"
#include "chao/residents.h"
#include "chao/transfer.h"
#include "chao/walk_art.h"
#include "chao/walk_card.h"
#include "chao/walk_layout.h"
#include "chao/walk_screen.h"
#include "chao/work.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "draw/text_sprite.h"
#include "save/card.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"
#include "system/vector.h"

void walk_load_textures(void)
{
    if (game_state.text_language == TEXT_JAPANESE) {
        texture_load_pvm("al_osamenu_tex", &walk_textures_jp);
        garden_walk_texture_list = &walk_textures_jp;
    } else {
        texture_load_pvm("al_osamenu_tex_e", &walk_textures_intl);
        garden_walk_texture_list = &walk_textures_intl;
    }
}

void walk_release_textures(void)
{
    njReleaseTexture(garden_walk_texture_list);
    garden_walk_texture_list = NULL;
}

#define WALK_SET_PHASE(work, next)                                             \
    (work)->mode = (next);                                                     \
    (work)->submode = 0

#define TITLE_HIDDEN_X 600.0F
#define TITLE_SHRUNK 0.75F
/* the title's text is the first TITLE_CELLS cells, or TITLE_CELLS_JAPANESE
 * in Japanese */
#define TITLE_CELLS 2
#define TITLE_CELLS_JAPANESE 3
/* the back sound of list 0, the main executable's */
#define BACK_SOUND 0x8009
enum title_phase {
    TITLE_INIT,
    TITLE_DELAY,
    TITLE_SLIDE_IN,
    TITLE_GROW,
    TITLE_FADE_IN,
    TITLE_OPEN,
    TITLE_CLOSE_DELAY,
    TITLE_FADE_OUT,
    TITLE_SHRINK,
    TITLE_SLIDE_OUT,
    TITLE_DONE
};

/* the title's work. scale is that of cell 4, and alpha that of cell 15 */
struct walk_title {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    struct angles unk_08;
    Float x, unk_18[2];
    Float unk_20, scale, alpha;
};
static void step_title(struct task *task)
{
    struct walk_title *title = (struct walk_title *)task->work;
    switch (title->mode) {
    case TITLE_INIT:
        title->x = TITLE_HIDDEN_X;
        title->scale = TITLE_SHRUNK;
        title->alpha = 0.0F;
        if (chao_current_area() == CHAO_AREA_DARK) {
            garden_walk_menu->rotation = HALF_TURN;
        } else {
            garden_walk_menu->rotation = 0;
        }
        WALK_SET_PHASE(title, TITLE_DELAY);
        break;
    case TITLE_DELAY:
        if (title->tick++ > title->timer) {
            WALK_SET_PHASE(title, TITLE_SLIDE_IN);
            title->tick = 0;
        }
        break;
    case TITLE_SLIDE_IN:
        title->x = (title->x * 0.95F) - 2.0F;
        if (title->x < 0.0F) {
            title->x = 0.0F;
            WALK_SET_PHASE(title, TITLE_GROW);
        }
        break;
    case TITLE_GROW:
        if (title->scale < 1.0F) {
            title->scale += 0.015F;
        } else {
            title->scale = 1.0F;
            WALK_SET_PHASE(title, TITLE_FADE_IN);
        }
        break;
    case TITLE_FADE_IN:
        if (title->alpha < 1.0F) {
            title->alpha += 0.04F;
        } else {
            title->alpha = 1.0F;
            WALK_SET_PHASE(title, TITLE_OPEN);
        }
        break;
    case TITLE_OPEN:
        break;
    case TITLE_CLOSE_DELAY:
        if (title->tick++ > title->timer) {
            WALK_SET_PHASE(title, TITLE_FADE_OUT);
        }
        break;
    case TITLE_FADE_OUT:
        if (title->alpha > 0.0F) {
            title->alpha -= 0.08F;
        } else {
            title->alpha = 0.0F;
            WALK_SET_PHASE(title, TITLE_SHRINK);
        }
        break;
    case TITLE_SHRINK:
        if (title->scale > TITLE_SHRUNK) {
            title->scale -= 0.02F;
        } else {
            title->scale = TITLE_SHRUNK;
            WALK_SET_PHASE(title, TITLE_SLIDE_OUT);
            sound_play(BACK_SOUND, NULL, 0, 0);
        }
        break;
    case TITLE_SLIDE_OUT:
        title->x += (title->x * 0.05F) + 5.0F;
        if (title->x > TITLE_HIDDEN_X) {
            WALK_SET_PHASE(title, TITLE_DONE);
        }
        break;
    case TITLE_DONE:
        task_kill(task);
        break;
    }
    switch (title->mode) {
    case TITLE_INIT:
    case TITLE_SLIDE_IN:
    case TITLE_GROW:
    case TITLE_CLOSE_DELAY:
    case TITLE_FADE_OUT:
    case TITLE_SHRINK:
    case TITLE_SLIDE_OUT:
    case TITLE_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (title->mode < TITLE_CLOSE_DELAY &&
        walk_menu_mode() == WALK_MENU_CLOSING) {
        WALK_SET_PHASE(title, TITLE_CLOSE_DELAY);
    }
}

#define WALK_SPRITE(sprite, left, top)                                         \
    (sprite).p.x = (left);                                                     \
    (sprite).p.y = (top);                                                      \
    (sprite).p.z = 0.0F;                                                       \
    (sprite).ang = 0;                                                          \
    (sprite).sx = 1.0F;                                                        \
    (sprite).sy = 1.0F;                                                        \
    (sprite).tlist = garden_walk_texture_list;                                 \
    (sprite).tanim = walk_cells
#define WALK_BLEND()                                                           \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA)
#define DRAW_CELL(sprite, cell, z)                                             \
    njDrawSprite2D(&(sprite), (cell), (z), (sprite).tanim[cell].attr)
#define WALK_WHITE(alpha) constant_material_set((alpha), 1.0F, 1.0F, 1.0F)
#define WALK_CLEAR_COLOR() constant_material_set(0.0F, 0.0F, 0.0F, 0.0F)

static void draw_title(struct task *task)
{
    struct walk_title *title = (struct walk_title *)task->work;
    NJS_SPRITE sprite;
    int i;
    Sint16 count;
    NJS_TEXANIM *cell;

    WALK_SPRITE(sprite, title->x + 0.0F, 0.0F);
    WALK_BLEND();
    i = 0;
    cell = sprite.tanim;
    if (game_state.text_language == TEXT_JAPANESE) {
        count = TITLE_CELLS_JAPANESE;
    } else {
        count = TITLE_CELLS;
    }
    do {
        njDrawSprite2D(&sprite, i, -10.0F, cell->attr);
        i++;
        cell = &sprite.tanim[i];
    } while (i < count);
    WALK_WHITE(title->alpha);
    DRAW_CELL(sprite, 15, -10.0F);
    WALK_WHITE(1.0F);
    DRAW_CELL(sprite, 5, -11.5F);
    constant_material_set(1.0F, 65.0F / 255.0F, 188.0F / 255.0F,
                          158.0F / 255.0F);
    DRAW_CELL(sprite, 6, -10.2F);
    sprite.p.x =
        title->x + ((1.0F - ((title->scale - TITLE_SHRUNK) * 4.0F)) * 32.0F);
    sprite.p.y = (1.0F - ((title->scale - TITLE_SHRUNK) * 4.0F)) * 16.0F;
    sprite.sx = title->scale;
    sprite.sy = title->scale;
    DRAW_CELL(sprite, 4, -10.1F);
    WALK_WHITE(1.0F);
    DRAW_CELL(sprite, 3, -10.25F);
    WALK_CLEAR_COLOR();
}
static void destroy_title(struct task *task)
{
    garden_walk_menu->title = NULL;
}
void walk_create_title(struct task *parent, Sint16 delay)
{
    struct task *task;
    if (!garden_walk_menu->title && parent) {
        task = task_create_child(TASK_WORK, step_title, parent);
#ifdef NONMATCHING
        /* this side makes no title where task_create_child fails. the matching
         * side writes through the null */
        if (!task) {
            return;
        }
#endif
        task->draw = draw_title;
        task->destructor = destroy_title;
        task->work->timer = delay;
        garden_walk_menu->title = task;
    }
}

enum frame_phase {
    FRAME_INIT,
    FRAME_DELAY,
    FRAME_FADE_IN,
    FRAME_OPEN,
    FRAME_FADE_OUT,
    FRAME_DONE
};
/* a frame's work */
struct walk_frame {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    struct angles unk_08;
    NJS_POINT3 unk_14;
    Float alpha, unk_24[2];
};
static void step_frame(struct task *task)
{
    struct walk_frame *frame = (struct walk_frame *)task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (frame->mode) {
    case FRAME_INIT:
        frame->alpha = 0.0F;
        WALK_SET_PHASE(frame, FRAME_DELAY);
        break;
    case FRAME_DELAY:
        if (frame->timer > 0) {
            frame->timer--;
        } else {
            WALK_SET_PHASE(frame, FRAME_FADE_IN);
        }
        break;
    case FRAME_FADE_IN:
        frame->alpha += 0.05F;
        if (frame->alpha >= 1.0F) {
            frame->alpha = 1.0F;
            WALK_SET_PHASE(frame, FRAME_OPEN);
        }
        break;
    case FRAME_OPEN:
        break;
    case FRAME_FADE_OUT:
        frame->alpha -= 0.05F;
        if (frame->alpha <= 0.0F) {
            frame->alpha = 0.0F;
            WALK_SET_PHASE(frame, FRAME_DONE);
        }
        break;
    case FRAME_DONE:
        task_kill(task);
        break;
    }
    switch (frame->mode) {
    case FRAME_INIT:
    case FRAME_DELAY:
    case FRAME_FADE_IN:
    case FRAME_FADE_OUT:
    case FRAME_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (frame->mode < FRAME_FADE_OUT && walk_menu_mode() == WALK_MENU_CLOSING) {
        WALK_SET_PHASE(frame, FRAME_FADE_OUT);
    }
}
#define DRAW_CELL_AT(sprite, left, top, cell, attr_of)                         \
    (sprite).p.x = (left);                                                     \
    (sprite).p.y = (top);                                                      \
    njDrawSprite2D(&(sprite), (cell), -10.0F, (sprite).tanim[attr_of].attr)
static void draw_send_frame(struct task *task)
{
    struct walk_frame *frame = (struct walk_frame *)task->work;
    NJS_SPRITE sprite;
    int i;
    NJS_TEXANIM *border;
    WALK_SPRITE(sprite, 0.0F, 0.0F);
    WALK_BLEND();
    WALK_WHITE(frame->alpha);
    border = &sprite.tanim[16];
    for (i = 16; i <= 21; i++) {
        njDrawSprite2D(&sprite, i, -10.0F, border->attr);
    }
    DRAW_CELL_AT(sprite, 46.0F, 164.0F, 22, 23);
    DRAW_CELL_AT(sprite, 54.0F, 294.0F, 22, 23);
    DRAW_CELL_AT(sprite, 122.0F, 365.0F, 23, 23);
    DRAW_CELL_AT(sprite, 245.0F, 365.0F, 23, 23);
    DRAW_CELL_AT(sprite, 373.0F, 365.0F, 23, 23);
    DRAW_CELL_AT(sprite, 501.0F, 365.0F, 23, 23);
    DRAW_CELL_AT(sprite, 501.0F, 365.0F, 23, 23);
    DRAW_CELL_AT(sprite, 25.0F, 381.0F, 51, 51);
    sprite.p.x = 96.0F;
    DRAW_CELL(sprite, 50, -10.0F);
    sprite.p.x = 89.0F;
    sprite.sx = 0.5F;
    DRAW_CELL(sprite, 46, -10.2F);
    sprite.p.x = -7.0F;
    sprite.sx = 2.0F;
    DRAW_CELL(sprite, 46, -10.2F);
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.p.x = 534.0F;
    DRAW_CELL(sprite, 52, -10.0F);
    sprite.p.x = 504.0F;
    DRAW_CELL(sprite, 47, -10.0F);
    sprite.p.x = 526.0F;
    sprite.sx = 0.5F;
    DRAW_CELL(sprite, 49, -10.2F);
    sprite.p.x = 620.0F;
    sprite.sx = 2.0F;
    DRAW_CELL(sprite, 49, -10.2F);
    WALK_CLEAR_COLOR();
}
static void destroy_frame(struct task *task)
{
    garden_walk_menu->frame = NULL;
}

#define CREATE_WALK_FRAME(draw_frame)                                          \
    struct task *task;                                                         \
    struct task_work *work;                                                    \
    if (garden_walk_menu->frame) {                                             \
        return NULL;                                                           \
    }                                                                          \
    if (!parent) {                                                             \
        return NULL;                                                           \
    }                                                                          \
    task = task_create_child(TASK_WORK, step_frame, parent);                   \
    if (!task) {                                                               \
        return NULL;                                                           \
    }                                                                          \
    task->draw = (draw_frame);                                                 \
    task->destructor = destroy_frame;                                          \
    work = task->work;                                                         \
    work->timer = delay;                                                       \
    garden_walk_menu->frame = task;                                            \
    return task

struct task *walk_create_send_frame(struct task *parent, Sint16 delay)
{
    CREATE_WALK_FRAME(draw_send_frame);
}
static void draw_receive_frame(struct task *task)
{
    struct walk_frame *frame;
    NJS_SPRITE sprite;
    frame = (struct walk_frame *)task->work;
    WALK_SPRITE(sprite, 0.0F, 0.0F);
    WALK_BLEND();
    WALK_WHITE(frame->alpha);
    DRAW_CELL_AT(sprite, 541.0F, 349.0F, 51, 51);
    sprite.p.x = 512.0F;
    njDrawSprite2D(&sprite, 45, -10.0F, sprite.tanim[50].attr);
    sprite.p.x = 534.0F;
    sprite.sx = 0.5F;
    DRAW_CELL(sprite, 46, -10.2F);
    sprite.p.x = 600.0F;
    sprite.sx = 3.0F;
    DRAW_CELL(sprite, 46, -10.2F);
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    DRAW_CELL_AT(sprite, 534.0F, 381.0F, 52, 52);
    sprite.p.x = 504.0F;
    DRAW_CELL(sprite, 47, -10.0F);
    sprite.p.x = 526.0F;
    sprite.sx = 0.5F;
    DRAW_CELL(sprite, 49, -10.2F);
    sprite.p.x = 600.0F;
    sprite.sx = 3.0F;
    DRAW_CELL(sprite, 49, -10.2F);
    WALK_CLEAR_COLOR();
}
struct task *walk_create_receive_frame(struct task *parent, Sint16 delay)
{
    CREATE_WALK_FRAME(draw_receive_frame);
}

/* the bar's lines, each a text sprite. the count is a Sint8, as the loops
 * over the lines are */
#define BAR_LINES ((Sint8)2)
/* the bar scrolls its text from SCROLL_RIGHT to past SCROLL_LEFT, both off
 * screen. it fills its other line once the last one's end scrolls inside
 * SCROLL_RIGHT. it frees a line whose end has passed SCROLL_LEFT */
enum { SCROLL_RIGHT = 670, SCROLL_LEFT = -30 };
/* a line holds at most LINE_WIDTH pixels of text, broken only between
 * links */
#define LINE_WIDTH 1024
#define SCROLL_SPEED 2.5F
/* the steps the bar waits, its text hidden, before it scrolls a message in */
#define TEXT_HOLD 10
#define BAR_TOP 413.0F
#define BAR_BOTTOM 445.0F
#define TEXT_TOP 417.0F
/* a line of the bar. limit is the limit it was filled with, which nothing
 * reads */
struct walk_line {
    struct text_sprite *sprite;
    Sint16 limit, x;
    Uint16 width;
};
/* the bar's work. current indexes the message showing in messages, or is
 * -1. restart has the bar drop its text and show current anew. scroll is
 * the x the lines are laid out from, and width how far they reach.
 * last_line is the line filled last, and last_link its last link */
struct walk_bar {
    Sint8 mode, submode, unk_02;
    Uint8 alpha;
    Uint16 unk_04, timer;
    Sint8 current, restart;
    Uint8 count, hold;
    Sint16 messages[WALK_BAR_MESSAGES];
    Uint32 unk_14[3];
    Float scroll;
    Uint32 unk_24;
    Sint16 text_alpha, unk_2a[3], width;
    Sint8 last_line;
    Uint8 last_link;
    struct walk_line lines[BAR_LINES];
};
#define BAR(task) ((struct walk_bar *)(task)->work)
static Uint16 measured_text_height;

Uint8 walk_message_text_id(int index)
{
    return walk_message_links[index].text;
}
static int next_text_link(int index)
{
    int next;
#ifdef NONMATCHING
    /* this side ends the text at an index outside walk_message_links */
    if (index < 0 || index >= WALK_TEXT_LINKS) {
        return WALK_END_TEXT;
    }
#endif
    next = walk_message_links[index].next;
    if (next == WALK_NEXT_TEXT) {
        next = index + 1;
    }
    return next;
}
static struct text_sprite *create_walk_text(const char *text, Uint16 *width,
                                            Uint16 *height)
{
    struct text_sprite *sprite;
    Uint16 measured_width;
    Uint16 measured_height;
    sprite =
        text_sprite_create_styled(text, game_state.text_language, -1, 1, 4);
#ifdef NONMATCHING
    /* this side returns null where text_sprite_create_styled makes no sprite.
     * the matching side measures through the null */
    if (!sprite) {
        return NULL;
    }
#endif
    if (!width || !height) {
        text_sprite_measure(sprite, text, game_state.text_language,
                            &measured_width, &measured_height);
    } else {
        text_sprite_measure(sprite, text, game_state.text_language, width,
                            height);
    }
    return sprite;
}
static void clear_message_lines(struct walk_line *line)
{
    Sint8 i;
    for (i = 0; i < BAR_LINES; i++) {
        if (line->sprite) {
            text_sprite_destroy(line->sprite);
            line->sprite = NULL;
#ifdef NONMATCHING
            /* this side repairs the bug tagged in the matching side. it steps
             * to the next line below the if, for every line */
#else
            /* @bug memory: leaves the second line's sprite where the first
             * line has none. destroy_message_bar then leaks it as it frees
             * the bar, at 0x0c51fe20. the loop steps to the second line only
             * past a sprite, at 0x0c51f6fa */
            line++;
#endif
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged above in the matching side. the loop
         * steps to the next line whether or not the line held a sprite */
        line++;
#endif
    }
}
/* fill the bar's line which with as many texts as fit from link start. the
 * line starts at the bar's width. return its sprite, or null where start is
 * WALK_END_TEXT or memory runs out. x goes unread, and limit is only kept */
static struct text_sprite *fill_message_line(struct task *task, Sint8 start,
                                             Sint16 x, Sint16 limit,
                                             Sint8 which)
{
    struct walk_bar *bar;
    struct walk_line *line;
    Sint8 index;
    Sint8 last;
    int next;
    int length;
    int size;
    int done;
    const char *text;
    char *shown;
    char *candidate;
    Uint16 width;
    Uint16 height;
    struct text_sprite measure;
    bar = BAR(task);
    line = &bar->lines[which];
    size = 0;
    done = FALSE;
    if (start < 0) {
        return NULL;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: a test that the Uint8 text id is below 0, which never
     * holds */
    if (walk_message_links[start].text < 0) {
        return NULL;
    }
#endif
    index = start;
    do {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((length = strlen(garden_message(walk_message_links[index].text))) ==
            0) {
            break;
        }
        size += length;
        next = walk_message_links[index].next;
        if (next == WALK_END_TEXT) {
            done = TRUE;
        } else if (next == WALK_NEXT_TEXT) {
            index++;
        } else {
            index = next;
        }
    } while (!done);
    size++;
    shown = syCalloc(1, size);
    candidate = syCalloc(1, size);
    if (!shown || !candidate) {
        if (shown) {
            syFree(shown);
        }
        if (candidate) {
            syFree(candidate);
        }
        return NULL;
    }
    last = WALK_END_TEXT;
    index = start;
    for (;;) {
        text = garden_message(walk_message_links[index].text);
#ifdef NONMATCHING
        /* the size above counts only up to the first empty text. the copy
         * has to stop there too */
        if (!*text) {
            break;
        }
#endif
        strcat(candidate, text);
        measure.pitch_x = 1;
        measure.pitch_y = 4;
        text_sprite_measure(&measure, candidate, game_state.text_language,
                            &width, &height);
        if (width > LINE_WIDTH) {
            index = last;
            break;
        }
        strcat(shown, text);
        last = index;
        next = walk_message_links[index].next;
        if (next == WALK_END_TEXT) {
            break;
        } else if (next == WALK_NEXT_TEXT) {
            index++;
        } else {
            index = next;
        }
    }
    if (line->sprite) {
        text_sprite_destroy(line->sprite);
        line->sprite = NULL;
    }
    line->sprite = create_walk_text(shown, &line->width, &measured_text_height);
#ifdef NONMATCHING
    /* this side frees both texts and returns null where the line's sprite can't
     * be made */
    if (!line->sprite) {
        syFree(shown);
        syFree(candidate);
        return NULL;
    }
#endif
    line->limit = limit;
    line->x = bar->width;
    bar->width += line->width + 1;
    bar->last_link = index;
    bar->last_line = which;
    syFree(shown);
    syFree(candidate);
    return line->sprite;
}

enum bar_phase {
    BAR_DELAY,
    BAR_FADE_IN,
    BAR_IDLE,
    BAR_LOAD,
    BAR_SCROLL,
    BAR_CHANGE_MESSAGE,
    BAR_FADE_OUT
};
static void step_message_bar(struct task *task)
{
    struct walk_bar *bar;
    struct walk_menu *menu;
    struct walk_line *line;
    Sint8 i;
    Sint16 right;
    int other;
    bar = BAR(task);
    /* @hack spelling: the menu's load inside the switch's expression, where
     * a statement of its own would do */
    switch (menu = garden_walk_menu, bar->mode) {
    case BAR_DELAY:
        if (bar->timer-- == 0) {
            WALK_SET_PHASE(bar, BAR_FADE_IN);
        }
        break;
    case BAR_FADE_IN:
        if (bar->alpha < 248) {
            bar->alpha += 7;
            garden_walk_menu->busy = TRUE;
        } else {
            bar->alpha = 255;
            WALK_SET_PHASE(bar, BAR_IDLE);
        }
        break;
    case BAR_IDLE:
        if (bar->current >= 0) {
            WALK_SET_PHASE(bar, BAR_LOAD);
        }
        break;
    case BAR_LOAD:
        if (bar->current < 0) {
            clear_message_lines(bar->lines);
            WALK_SET_PHASE(bar, BAR_IDLE);
        } else {
            if (bar->current < bar->count - 1) {
                if (!bar->restart) {
                    bar->current++;
                }
            } else if (menu->state < MENU_RESULT) {
                bar->current = 0;
            } else if (menu->state == MENU_RESULT && menu->timer > 0) {
                bar->current = 0;
            } else {
                bar->current = -1;
                WALK_SET_PHASE(bar, BAR_FADE_OUT);
                return;
            }
            if (bar->messages[bar->current] < 0) {
                WALK_SET_PHASE(bar, BAR_IDLE);
                return;
            }
            bar->width = 0;
            bar->last_line = 0;
            clear_message_lines(bar->lines);
            if (fill_message_line(task, bar->messages[bar->current], bar->width,
                                  SCROLL_RIGHT, 0)) {
                bar->scroll = SCROLL_RIGHT;
                bar->text_alpha = 255;
                bar->hold = TEXT_HOLD;
                bar->restart = FALSE;
                WALK_SET_PHASE(bar, BAR_SCROLL);
            }
        }
        break;
    case BAR_SCROLL:
        if (bar->hold > 0) {
            bar->hold--;
        } else {
            bar->scroll -= SCROLL_SPEED;
            line = &bar->lines[bar->last_line];
            right = bar->scroll + line->x + line->width;
            if (line->sprite && right < SCROLL_RIGHT) {
                if (bar->last_line == 0) {
                    other = 1;
                } else {
                    other = 0;
                }
                fill_message_line(task, next_text_link(bar->last_link),
                                  bar->width, bar->width + line->width, other);
            }
            line = bar->lines;
            for (i = 0; i < BAR_LINES; i++, line++) {
                if (line->sprite) {
                    if (bar->scroll + line->x + line->width < SCROLL_LEFT) {
                        text_sprite_destroy(line->sprite);
                        line->sprite = NULL;
                    }
                }
            }
            if (bar->scroll < -bar->width + SCROLL_LEFT) {
                WALK_SET_PHASE(bar, BAR_LOAD);
                if (garden_walk_menu->state >= MENU_RESULT &&
#ifdef NONMATCHING
                    /* this side reads no message past the bar's
                     * WALK_BAR_MESSAGES */
                    bar->current + 1 < WALK_BAR_MESSAGES &&
#endif
                    bar->messages[bar->current + 1] != -1) {
                    return;
                }
            }
        }
        if (garden_walk_menu->state >= MENU_RESULT &&
            (pad_press_repeat[0] & PDD_DGT_TA)) {
            WALK_SET_PHASE(bar, BAR_FADE_OUT);
            return;
        }
        if (bar->restart) {
            WALK_SET_PHASE(bar, BAR_CHANGE_MESSAGE);
        }
        break;
    case BAR_CHANGE_MESSAGE:
        bar->scroll -= SCROLL_SPEED;
        bar->text_alpha -= 20;
        if (bar->text_alpha < 0) {
            bar->text_alpha = 0;
            WALK_SET_PHASE(bar, BAR_LOAD);
        }
        break;
    case BAR_FADE_OUT:
        if (bar->alpha > 7) {
            bar->alpha -= 8;
            garden_walk_menu->busy = TRUE;
        } else {
            task_kill(task);
            return;
        }
        break;
    }
    if (garden_walk_menu->state >= MENU_RESULT &&
        (bar->mode == BAR_SCROLL || bar->mode >= BAR_CHANGE_MESSAGE)) {
        walk_set_menu_busy(TRUE);
    } else if (walk_menu_mode() == WALK_MENU_CLOSING) {
        WALK_SET_PHASE(bar, BAR_FADE_OUT);
    }
}

#define BAR_VERTEX(point, left, top, color)                                    \
    (point).x = (left);                                                        \
    (point).y = (top);                                                         \
    (point).z = 0.1F;                                                          \
    (point).col = (color)
static void draw_message_bar(struct task *task)
{
    struct walk_bar *bar;
    struct walk_line *line;
    Sint8 i;
    Uint32 color;
    Uint32 background;
    Uint32 white;
    NJS_POLYGON_VTX vertices[4];
    NJS_POINT3 position;
    bar = BAR(task);
    WALK_BLEND();
    background = 0;
    background |= (Uint32)bar->alpha << 24;
    BAR_VERTEX(vertices[0], 0.0F, BAR_TOP, background);
    BAR_VERTEX(vertices[1], 0.0F, BAR_BOTTOM, background);
    BAR_VERTEX(vertices[2], SCREEN_WIDTH, BAR_TOP, background);
    BAR_VERTEX(vertices[3], SCREEN_WIDTH, BAR_BOTTOM, background);
    njDrawPolygon(vertices, 4, TRUE);
    if (!bar->hold) {
#ifdef NONMATCHING
#else
        /* @hack redundant: a store of scroll to position.x, which each line's
         * own store replaces */
        position.x = bar->scroll;
#endif
        position.y = TEXT_TOP;
        white = 0x00ffffff;
        for (i = 0; i < BAR_LINES; i++) {
            line = &bar->lines[i];
            if (line->sprite) {
                position.x = line->x + bar->scroll;
                color = ((Uint32)bar->text_alpha << 24) | white;
                text_sprite_draw_text(line->sprite, position.x, position.y,
                                      1.0F, 1.0F, &color);
            }
        }
    }
}
static void destroy_message_bar(struct task *task)
{
    struct walk_bar *bar = BAR(task);
    clear_message_lines(bar->lines);
    syFree(bar);
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: it writes nothing
     * through the freed bar */
#else
    /* @bug memory: clears the collision field of task_work in the bar it has
     * just freed. the free is at 0x0c51fe20, and the store at 0x0c51fe28 */
    task->work->collision = NULL;
#endif
    task->work = NULL;
    garden_walk_menu->bar = NULL;
}
void walk_create_message_bar(struct task *parent, Sint16 delay)
{
    struct task *task;
    struct walk_bar *bar;
    if (!garden_walk_menu->bar && parent) {
        task = task_create_child(0, step_message_bar, parent);
#ifdef NONMATCHING
        /* this side makes no bar where task_create_child fails. the matching
         * side writes through the null */
        if (!task) {
            return;
        }
#endif
        task->draw = draw_message_bar;
        task->destructor = destroy_message_bar;
        bar = syCalloc(1, sizeof(struct walk_bar));
#ifdef NONMATCHING
        /* this side destroys the task without its destructor where the heap has
         * no bar. the destructor would read the bar */
        if (!bar) {
            task->destructor = NULL;
            task_destroy(task);
            return;
        }
#endif
        task->work = (struct task_work *)bar;
        task->work->timer = delay;
        bar->current = -1;
        bar->messages[0] = -1;
        bar->messages[1] = -1;
        bar->messages[2] = -1;
        bar->messages[3] = -1;
        garden_walk_menu->bar = task;
    }
}
void garden_walk_message(Sint16 id)
{
    struct walk_bar *bar;
    if (garden_walk_menu && garden_walk_menu->bar) {
        bar = BAR(garden_walk_menu->bar);
        if (id < 0) {
            bar->current = -1;
        } else {
            bar->current = 0;
        }
        bar->restart = TRUE;
        bar->messages[0] = id;
        bar->count = 1;
        bar->messages[1] = -1;
        bar->messages[2] = -1;
        bar->messages[3] = -1;
    }
}
static void append_message(Sint16 id)
{
    struct walk_bar *bar;
    if (garden_walk_menu && garden_walk_menu->bar) {
        bar = BAR(garden_walk_menu->bar);
        if (bar->count > WALK_BAR_MESSAGES - 1) {
            return;
        }
        bar->messages[bar->count] = id;
        bar->count++;
    }
}
void garden_walk_queue_message(Sint16 id)
{
    if (garden_walk_menu && garden_walk_menu->bar) {
#ifdef NONMATCHING
#else
        /* @hack redundant: an empty test of id against the first message */
        struct walk_bar *bar = BAR(garden_walk_menu->bar);
        if (id == bar->messages[0]) {
        }
#endif
        append_message(id);
    }
}
void garden_walk_change_message(Sint16 id)
{
    struct walk_bar *bar;
    if (garden_walk_menu && garden_walk_menu->bar) {
        bar = BAR(garden_walk_menu->bar);
        if (id != bar->messages[0]) {
            garden_walk_message(id);
        }
    }
}
void fun_0c51ffb2(void) /* NOLINT(misc-use-internal-linkage) */
{
}
void walk_remember_message(void)
{
    struct walk_bar *bar = BAR(garden_walk_menu->bar);
    garden_walk_menu->message = bar->messages[bar->current];
}

static Bool cell_is_checked(Sint8 index)
{
    struct task *task = garden_walk_menu->cells[index];
    struct task_work *work;
    if (task) {
        work = task->work;
        if (work->flags & CELL_CHECKED) {
            return TRUE;
        }
    }
    return FALSE;
}
enum highlight_phase {
    HIGHLIGHT_INIT,
    HIGHLIGHT_DELAY,
    HIGHLIGHT_FADE_IN,
    HIGHLIGHT_OPEN,
    HIGHLIGHT_CLOSE_DELAY,
    HIGHLIGHT_FADE_OUT,
    HIGHLIGHT_DONE
};
/* the top row's highlights open left to right, HIGHLIGHT_STAGGER steps
 * apart. the bottom row's open right to left at the same time */
#define HIGHLIGHT_STAGGER 10
/* the walk_cells sprites of a content cell's highlight and of a drive
 * cell's */
#define CONTENT_HIGHLIGHT_CELL 14
#define DRIVE_HIGHLIGHT_CELL 13
/* a highlight's work. id is its grid cell, at column and row. cell is the
 * walk_cells sprite it draws at x and y */
struct walk_highlight {
    Sint8 mode, submode;
    Uint8 id, cell;
    Sint16 flags;
    Uint16 timer;
    Sint32 column, row, unk_10;
    Float x, y, unk_1c;
    Float unk_20[2], alpha;
};
static void step_cell_highlight(struct task *task)
{
    struct walk_highlight *highlight = (struct walk_highlight *)task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (highlight->mode) {
    case HIGHLIGHT_INIT:
        WALK_SET_PHASE(highlight, HIGHLIGHT_DELAY);
        break;
    case HIGHLIGHT_DELAY:
        if (highlight->timer > 0) {
            highlight->timer--;
        } else {
            WALK_SET_PHASE(highlight, HIGHLIGHT_FADE_IN);
        }
        break;
    case HIGHLIGHT_FADE_IN:
        if (highlight->alpha < 1.0F) {
            highlight->alpha += 0.08F;
        } else {
            highlight->alpha = 1.0F;
            WALK_SET_PHASE(highlight, HIGHLIGHT_OPEN);
        }
        break;
    case HIGHLIGHT_OPEN:
        break;
    case HIGHLIGHT_CLOSE_DELAY:
        if (highlight->timer > 0) {
            highlight->timer--;
        } else {
            WALK_SET_PHASE(highlight, HIGHLIGHT_FADE_OUT);
        }
        break;
    case HIGHLIGHT_FADE_OUT:
        if (highlight->alpha > 0.0F) {
            highlight->alpha -= 0.08F;
        } else {
            highlight->alpha = 0.0F;
            WALK_SET_PHASE(highlight, HIGHLIGHT_DONE);
        }
        break;
    case HIGHLIGHT_DONE:
        task_kill(task);
        return;
    }
    switch (highlight->mode) {
    case HIGHLIGHT_INIT:
    case HIGHLIGHT_DELAY:
    case HIGHLIGHT_FADE_IN:
    case HIGHLIGHT_CLOSE_DELAY:
    case HIGHLIGHT_FADE_OUT:
    case HIGHLIGHT_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (highlight->mode < HIGHLIGHT_CLOSE_DELAY &&
        walk_menu_mode() == WALK_MENU_CLOSING) {
        WALK_SET_PHASE(highlight, HIGHLIGHT_CLOSE_DELAY);
    }
}
static void draw_cell_highlight(struct task *task)
{
    struct walk_highlight *highlight = (struct walk_highlight *)task->work;
    NJS_SPRITE sprite;
    WALK_SPRITE(sprite, highlight->x + 0.0F, highlight->y + 0.0F);
    WALK_BLEND();
    if (garden_walk_texture_list) {
        /* @hack spelling: the comparison with TRUE, where the Bool alone
         * would do */
        if (cell_is_checked(highlight->id) == TRUE) {
            constant_material_set(highlight->alpha, 250.0F / 255.0F,
                                  168.0F / 255.0F, 5.0F / 255.0F);
        } else {
            constant_material_set(highlight->alpha, 128.0F / 255.0F,
                                  128.0F / 255.0F, 128.0F / 255.0F);
        }
        DRAW_CELL(sprite, highlight->cell, -30.0F);
        WALK_CLEAR_COLOR();
    }
}
static void destroy_cell_highlight(struct task *task)
{
}
struct task *walk_create_cell_highlight(struct task *parent, Sint8 id,
                                        Sint8 cells)
{
    struct task *task;
    struct walk_highlight *highlight;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_cell_highlight, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_cell_highlight;
    task->destructor = destroy_cell_highlight;
    highlight = (struct walk_highlight *)task->work;
    highlight->id = id;
    if (cells == WALK_CONTENT_CELLS) {
        highlight->column = id / WALK_GRID_ROWS;
        highlight->row = id % WALK_GRID_ROWS;
        highlight->timer =
            (highlight->row == 0
                 ? (Sint16)highlight->column
                 : (WALK_CONTENT_COLUMNS - 1) - (Sint16)highlight->column) *
            HIGHLIGHT_STAGGER;
        highlight->x = (highlight->column * 98.0F) + 97.0F;
        highlight->y = (highlight->row * 80.0F) + 116.0F;
        highlight->cell = CONTENT_HIGHLIGHT_CELL;
    } else {
        highlight->column = id / WALK_GRID_ROWS;
        highlight->row = id % WALK_GRID_ROWS;
        highlight->timer =
            (highlight->row == 0
                 ? (Sint16)highlight->column
                 : (WALK_DRIVE_COLUMNS - 1) - (Sint16)highlight->column) *
            HIGHLIGHT_STAGGER;
        highlight->x = (highlight->column * 128.0F) + 110.0F;
        highlight->y = (highlight->row * 127.0F) + 146.0F;
        highlight->cell = DRIVE_HIGHLIGHT_CELL;
    }
    highlight->alpha = 0.0F;
    return task;
}

/* the work of a model shown in front of the camera: a drive's card or an
 * item. id is the drive or the item, column and row its cell, and position
 * where it stands */
struct walk_model {
    Sint8 mode, submode;
    Uint8 id, delay;
    Sint16 flags;
    Uint16 timer;
    Sint32 column;
    Angle yaw;
    Sint32 row;
    NJS_POINT3 position;
};
/* the screen's middle, which the placements count their pixels from */
#define SCREEN_HALF_WIDTH (SCREEN_WIDTH / 2)
#define SCREEN_HALF_HEIGHT (SCREEN_HEIGHT / 2)
/* the yaw that turns a frame's -z axis along the camera's direction */
#define WALK_CAMERA_YAW(direction)                                             \
    ((Angle)(atan2f((direction).x, (direction).z) * ANGLE_PER_RADIAN) +        \
     HALF_TURN)
/* carry point into the world from the frame at eye, turned by yaw and moved
 * by offset. each placement's offset is its depth along -z. a point at z 0
 * then stands that depth ahead of the camera */
#define CARRY_TO_WORLD(eye, yaw, offset, point)                                \
    matrix_push_unit();                                                        \
    njTranslateEx(&(eye));                                                     \
    njRotateY(NULL, (yaw));                                                    \
    njTranslateEx(&(offset));                                                  \
    njCalcPoint(NULL, &(point), &(point));                                     \
    njPopMatrixEx()
enum drive_frame_phase {
    DRIVE_FRAME_INIT,
    DRIVE_FRAME_DELAY,
    DRIVE_FRAME_PLACE,
    DRIVE_FRAME_OPEN,
    DRIVE_FRAME_CLOSE,
    DRIVE_FRAME_DONE
};
static void step_drive_frame(struct task *owner)
{
    /* @hack spelling: task, a copy of owner, where the parameter itself would
     * do */
    struct task *task = owner;
    struct walk_model *frame = (struct walk_model *)task->work;
    NJS_POINT3 base;
    NJS_POINT3 position;
    NJS_POINT3 eye;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;

    if (game_state.time_stopped) {
        return;
    }
    switch (frame->mode) {
    case DRIVE_FRAME_INIT:
        WALK_SET_PHASE(frame, DRIVE_FRAME_DELAY);
        break;
    case DRIVE_FRAME_DELAY:
        frame->timer = frame->delay;
        WALK_SET_PHASE(frame, DRIVE_FRAME_PLACE);
        break;
    case DRIVE_FRAME_PLACE:
        if (frame->timer > 0) {
            frame->timer--;
        } else {
            /* @hack redundant: the copy of walk_origins' entry, whose x and y
             * the placement sets and whose z nothing reads */
            base = walk_origins[WALK_DRIVE_ORIGIN];
            eye = camera_views[0]->pos;
            direction = camera_views[0]->dir;
            offset.x = offset.y = 0.0F;
            offset.z = -walk_drive_depth;
            base.x = (walk_drive_left + (walk_drive_row_shift * frame->row) -
                      SCREEN_HALF_WIDTH) *
                     walk_drive_units_x;
            base.y = (walk_drive_top - SCREEN_HALF_HEIGHT) * walk_drive_units_x;
            position.x = base.x + (frame->column * walk_drive_column_spacing *
                                   walk_drive_units_x);
            position.y = base.y + (frame->row * walk_drive_row_spacing *
                                   walk_drive_units_y);
            position.z = 0.0F;
            yaw = WALK_CAMERA_YAW(direction);
            CARRY_TO_WORLD(eye, yaw, offset, position);
            frame->position = position;
            frame->yaw = yaw;
            WALK_SET_PHASE(frame, DRIVE_FRAME_OPEN);
        }
        break;
    case DRIVE_FRAME_OPEN:
        /* @hack redundant: the if (0) block below, a second placement that
         * never runs, where the case would be empty */
        if (0) {
            eye = camera_views[0]->pos;
            offset.x = offset.y = 0.0F;
            offset.z = 0.0F;
            position.x =
                (walk_drive_left - SCREEN_HALF_WIDTH) * walk_drive_units_x;
            position.y =
                (walk_drive_top - SCREEN_HALF_HEIGHT) * walk_drive_units_x;
            position.z = 0.0F;
            yaw = WALK_CAMERA_YAW(eye);
            matrix_push_unit();
            njTranslateEx(&eye);
            njRotateY(NULL, yaw);
            njTranslateEx(&offset);
            njPopMatrixEx();
            frame->position = position;
            frame->yaw = yaw;
        }
        break;
    case DRIVE_FRAME_CLOSE:
        if (frame->timer > 0) {
            frame->timer--;
        } else {
            WALK_SET_PHASE(frame, DRIVE_FRAME_DONE);
        }
        break;
    case DRIVE_FRAME_DONE:
        task_kill(task);
        return;
    }
    switch (frame->mode) {
    case DRIVE_FRAME_INIT:
    case DRIVE_FRAME_PLACE:
    case DRIVE_FRAME_CLOSE:
    case DRIVE_FRAME_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: an empty test of the mode against DRIVE_FRAME_OPEN */
    if (frame->mode >= DRIVE_FRAME_OPEN) {
    } else {
    }
#endif
    /* no code sets the menu's mode to 1, so only WALK_MENU_CLOSING closes the
     * frame */
    if (frame->mode < DRIVE_FRAME_CLOSE &&
        (walk_menu_mode() == WALK_MENU_CLOSING || walk_menu_mode() == 1)) {
        frame->timer = frame->delay;
        WALK_SET_PHASE(frame, DRIVE_FRAME_CLOSE);
    }
}
static void draw_drive_frame(struct task *task)
{
    struct walk_model *frame = (struct walk_model *)task->work;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    if (frame->flags < 0) {
#else
    /* @hack spelling: bit 16 of the sign-extended flags, where flags < 0
     * would do */
    if (frame->flags & 0x10000) {
#endif
        return;
    }
    WALK_BLEND();
    njPushMatrixEx();
    njSetTexture(&egg_texlist);
    njTranslateEx(&frame->position);
    njRotateY(NULL, walk_drive_yaw + garden_walk_menu->rotation);
    njScale(NULL, walk_drive_scale, walk_drive_scale, 0.1F);
    if (garden_walk_drive_type(frame->id) == WALK_DRIVE_CARD) {
        njCnkEasyDrawObject(&walk_vmu_model);
    } else if (garden_walk_drive_type(frame->id) == WALK_DRIVE_MULTI_BANK) {
        njCnkEasyDrawObject(&walk_multi_vmu_model);
    }
    njPopMatrixEx();
}
static void destroy_drive_frame(struct task *task)
{
}
struct task *walk_create_drive_frame(struct task *parent, Sint8 id, int delay)
{
    struct task *task;
    struct walk_model *frame;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_drive_frame, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_drive_frame;
    task->destructor = destroy_drive_frame;
    frame = (struct walk_model *)task->work;
    frame->id = id;
    frame->column = id / WALK_GRID_ROWS;
    frame->row = id % WALK_GRID_ROWS;
    frame->position.x = frame->position.y = frame->position.z = 0.0F;
    frame->delay = delay;
    return task;
}

/* a preview's work. actor is its Chao or egg, and egg is set where
 * walk_create_chao_preview's actor is an egg */
struct walk_preview {
    Sint8 mode, submode;
    Uint8 id, egg;
    Sint16 flags;
    Uint16 timer;
    struct task *actor;
};
/* the world units per screen pixel, the value of every units_x and units_y
 * of walk_layout.h. WALK_CARD_PLACEMENT, walk_create_egg_preview and
 * walk_create_item_preview take the constant for their bases instead */
#define UNITS_PER_PIXEL 0.0316F
/* the sizes step_chao_preview gives an egg, which walk_egg_scale and
 * walk_content_egg_scale also hold */
#define CARD_EGG_SIZE 0.4F
#define CONTENT_EGG_SIZE 0.55F
#define WALK_CARD_PLACEMENT(base, position, column, row)                       \
    (base).x = (walk_card_chao_left - SCREEN_HALF_WIDTH) * UNITS_PER_PIXEL;    \
    (base).y = (walk_card_chao_top - SCREEN_HALF_HEIGHT) * UNITS_PER_PIXEL;    \
    (position).x = (base).x + ((column) * walk_card_chao_units_x *             \
                               walk_card_chao_column_spacing);                 \
    (position).y = (base).y + ((row) * walk_card_chao_units_y *                \
                               walk_card_chao_row_spacing)
/* a preview's placement, for the code in the previews' steps that never
 * runs */
#define PREVIEW_PLACEMENT()                                                    \
    base = walk_origins[WALK_CHAO_ORIGIN];                                     \
    eye = camera_views[0]->pos;                                                \
    direction = camera_views[0]->dir;                                          \
    offset.x = offset.y = 0.0F;                                                \
    if (preview->egg) {                                                        \
        offset.z = -walk_card_chao_depth;                                      \
        WALK_CARD_PLACEMENT(base, position, column, row);                      \
        position.z = 0.0F;                                                     \
        yaw = WALK_CAMERA_YAW(direction);                                      \
    } else {                                                                   \
        offset.z = -walk_content_chao_depth;                                   \
        position.x = (walk_content_chao_left - SCREEN_HALF_WIDTH) *            \
                     walk_content_chao_units_x;                                \
        position.y = (walk_content_chao_top - SCREEN_HALF_HEIGHT) *            \
                     walk_content_chao_units_y;                                \
        position.z = 0.0F;                                                     \
        yaw = WALK_CAMERA_YAW(direction);                                      \
    }                                                                          \
    CARRY_TO_WORLD(eye, yaw, offset, position);                                \
    chao->work->pos = position

static void step_chao_preview(struct task *task)
{
    struct walk_preview *preview = (struct walk_preview *)task->work;
    struct task *chao = preview->actor;
    struct egg_state *egg;
#ifdef NONMATCHING
#else
    NJS_POINT3 position;
    NJS_POINT3 base;
    NJS_POINT3 eye;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;
    Sint8 id;
    int column;
    int row;
#endif
    if (chao && preview->egg) {
        egg = chao->extra.p;
        if (egg) {
            if (preview->id < WALK_CONTENT_PREVIEW) {
                egg->size = CARD_EGG_SIZE;
            } else {
                egg->size = CONTENT_EGG_SIZE;
            }
        }
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: the if (0) block below, a placement that never runs,
     * and the locals declared for it alone */
    if (0) {
        id = preview->id;
        column = id / WALK_GRID_ROWS;
        row = id % WALK_GRID_ROWS;
        chao->work->ang.y = walk_card_chao_yaw + garden_walk_menu->rotation +
                            (column * walk_card_chao_column_turn);
        PREVIEW_PLACEMENT();
        preview->id = id;
    }
#endif
}
static void destroy_chao_preview(struct task *task)
{
    struct walk_preview *preview = (struct walk_preview *)task->work;
    struct task *chao = preview->actor;
    task_destroy_children(task);
    if (chao) {
        task_kill(chao);
        preview->actor = NULL;
    }
}

/* @hack spelling: the five bits cleared one at a time, where one mask would
 * do */
#define FREEZE_PREVIEW(actor)                                                  \
    ((struct chao_work *)(actor)->work)->flags &= ~CHAO_COLLIDES;              \
    ((struct chao_work *)(actor)->work)->flags &= ~CHAO_MOVES;                 \
    ((struct chao_work *)(actor)->work)->flags &= ~CHAO_RUNS_BEHAVIOR;         \
    ((struct chao_work *)(actor)->work)->flags &= ~CHAO_BODY_SPRINGS;          \
    ((struct chao_work *)(actor)->work)->flags &= ~CHAO_AGES

struct task *walk_create_chao_preview(struct task *parent, int column, int row,
                                      struct garden_vms_chao *saved, Sint8 id)
{
    struct task *task;
    struct walk_preview *preview;
    struct task *chao;
    NJS_POINT3 position;
    NJS_POINT3 base;
    NJS_POINT3 eye;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;
    struct garden_vms_chao *record;
#ifdef NONMATCHING
    /* this side returns null without a saved Chao, which the matching side
     * reads */
    if (!saved) {
        return NULL;
    }
#endif
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_chao_preview, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = destroy_chao_preview;
    preview = (struct walk_preview *)task->work;
#ifdef NONMATCHING
    /* this side starts position at the origin, which the matching side passes
     * to garden_spawn_chao unset */
    position.x = position.y = position.z = 0.0F;
#endif
    chao = garden_spawn_chao(&saved->saved, &position, walk_card_chao_yaw);
    if (chao) {
        FREEZE_PREVIEW(chao);
        /* @hack spelling: record, a copy of saved, where saved itself would
         * do */
        record = saved;
        /* @hack redundant: the test for CHAO_NONE, which never holds here.
         * garden_spawn_chao spawns nothing for an empty record */
        if (record->saved.record.live.parameters.type == CHAO_NONE ||
            record->saved.record.live.parameters.type == CHAO_EGG) {
            preview->egg = TRUE;
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged in the matching side. it passes the
         * preview, which holds its Chao before its motion is set */
        preview->actor = chao;
        if (id == garden_walk_menu->cursor) {
            walk_set_preview_motion(task, WALK_PREVIEW_FOCUS, 1);
        } else {
            walk_set_preview_motion(task, WALK_PREVIEW_IDLE, 1);
        }
#else
        /* @bug mismatched-call: passes the Chao's task to
         * walk_set_preview_motion, which takes the preview's, at 0x0c520914.
         * the Chao's work holds 0 where a preview's holds its Chao, so no
         * motion takes */
        if (id == garden_walk_menu->cursor) {
            walk_set_preview_motion(chao, WALK_PREVIEW_FOCUS, 1);
        } else {
            walk_set_preview_motion(chao, WALK_PREVIEW_IDLE, 1);
        }
#endif
        chao->work->ang.y = walk_card_chao_yaw + garden_walk_menu->rotation +
                            (column * walk_card_chao_column_turn);
        preview->actor = chao;
    } else {
        preview->actor = NULL;
        task_destroy(task);
        return NULL;
    }
    /* @hack redundant: the copy of walk_origins' entry, whose x and y the
     * placement sets and whose z nothing reads */
    base = walk_origins[WALK_CHAO_ORIGIN];
    eye = camera_views[0]->pos;
    direction = camera_views[0]->dir;
    offset.x = offset.y = 0.0F;
    if (id < WALK_CONTENT_PREVIEW) {
        offset.z = -walk_card_chao_depth;
        WALK_CARD_PLACEMENT(base, position, column, row);
        position.z = 0.0F;
        yaw = WALK_CAMERA_YAW(direction);
    } else {
        offset.z = -walk_content_chao_depth;
        position.x = (walk_content_chao_left - SCREEN_HALF_WIDTH) *
                     walk_content_chao_units_x;
        if (preview->egg) {
            position.y = (walk_content_chao_top + walk_content_egg_top -
                          SCREEN_HALF_HEIGHT) *
                         walk_content_chao_units_y;
        } else {
            position.y = (walk_content_chao_top - SCREEN_HALF_HEIGHT) *
                         walk_content_chao_units_y;
        }
        position.z = 0.0F;
        yaw = WALK_CAMERA_YAW(direction);
    }
    CARRY_TO_WORLD(eye, yaw, offset, position);
    chao->work->pos = position;
    preview->id = id;
    return task;
}
struct task *walk_preview_chao(struct task *task)
{
    struct walk_preview *preview;
    /* no preview's step is its destructor, so the last test never holds */
    if (!task || !task->work || task->step == task->destructor) {
        return NULL;
    }
    preview = (struct walk_preview *)task->work;
    return preview->actor;
}
void walk_set_preview_motion(struct task *task, int motion, int frames)
{
    struct task *chao = walk_preview_chao(task);
    /* @hack spelling: an empty branch for no Chao, where chao && !egg would
     * do */
    if (!chao) {
    } else if (!((struct walk_preview *)task->work)->egg) {
        chao_link_motion_frames(chao, motion, frames);
    }
}
static void step_egg_preview(struct task *task)
{
    struct walk_preview *preview = (struct walk_preview *)task->work;
    struct task *chao = preview->actor;
    struct egg_state *egg;
#ifdef NONMATCHING
#else
    NJS_POINT3 position;
    NJS_POINT3 base;
    NJS_POINT3 eye;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;
    int column;
    int row;
#endif
    /* @hack redundant: the test of preview->actor, which chao already
     * holds */
    if (chao && preview->actor) {
        egg = chao->extra.p;
        if (egg) {
            if (preview->id < WALK_CONTENT_PREVIEW) {
                egg->size = walk_egg_scale;
            } else {
                egg->size = walk_content_egg_scale;
            }
        }
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: the if (0) block below, a placement that never runs,
     * and the locals declared for it alone */
    if (0) {
        column = preview->id / WALK_GRID_ROWS;
        row = preview->id % WALK_GRID_ROWS;
        PREVIEW_PLACEMENT();
        preview->egg = FALSE;
    }
#endif
}
static void destroy_egg_preview(struct task *task)
{
    struct walk_preview *preview = (struct walk_preview *)task->work;
    struct task *chao = preview->actor;
    if (chao) {
        task_kill(chao);
        preview->actor = NULL;
    }
}
struct task *walk_create_egg_preview(struct task *parent, int column, int row,
                                     struct garden_vms_chao *saved, Uint8 id)
{
    struct task *task;
    struct walk_preview *preview;
    struct task *chao;
    struct egg_state *egg;
    NJS_POINT3 position;
    NJS_POINT3 base;
    NJS_POINT3 eye;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_egg_preview, parent);
    if (!task) {
        return NULL;
    }
    task->destructor = destroy_egg_preview;
    preview = (struct walk_preview *)task->work;
    preview->id = id;
    /* @hack redundant: the copy of walk_origins' entry, whose x and y the
     * placement sets and whose z nothing reads */
    base = walk_origins[WALK_EGG_ORIGIN];
    eye = camera_views[0]->pos;
    direction = camera_views[0]->dir;
    offset.x = offset.y = 0.0F;
    if (preview->id < WALK_CONTENT_PREVIEW) {
        offset.z = -walk_egg_depth;
        base.x = (walk_card_chao_left + walk_egg_left - SCREEN_HALF_WIDTH) *
                 UNITS_PER_PIXEL;
        base.y = (walk_card_chao_top + walk_egg_top - SCREEN_HALF_HEIGHT) *
                 UNITS_PER_PIXEL;
        position.x = base.x + (column * walk_card_chao_units_x *
                               walk_egg_column_spacing);
        position.y =
            base.y + (row * walk_card_chao_units_y * walk_egg_row_spacing);
        position.z = 0.0F;
        yaw = WALK_CAMERA_YAW(direction);
    } else {
        offset.z = -walk_content_chao_depth;
        position.x = (walk_content_chao_left + walk_content_egg_left -
                      SCREEN_HALF_WIDTH) *
                     walk_content_chao_units_x;
        position.y = (walk_content_chao_row_spacing + walk_content_chao_top +
                      walk_content_egg_top - SCREEN_HALF_HEIGHT) *
                     walk_content_chao_units_y;
        position.z = 0.0F;
        yaw = WALK_CAMERA_YAW(direction);
    }
    CARRY_TO_WORLD(eye, yaw, offset, position);
    chao = NULL;
    if (saved) {
        chao = garden_spawn_chao(&saved->saved, &position, 0);
    }
    if (chao) {
        FREEZE_PREVIEW(chao);
        egg = chao->extra.p;
        if (!egg) {
#ifdef NONMATCHING
            /* this side ends the preview where the egg has no extra. the
             * matching side spins there for good */
            task_kill(chao);
            task_destroy(task);
            return NULL;
#else
            for (;;) {
            }
#endif
        }
        if (preview->id < WALK_CONTENT_PREVIEW) {
            egg->size = walk_egg_scale;
        } else {
            egg->size = walk_content_egg_scale;
        }
        preview->actor = chao;
    } else {
        preview->actor = NULL;
        task_destroy(task);
        return NULL;
    }
    return task;
}

/* column and row point at garden_walk_menu's */
struct walk_cursor {
    Sint8 grid;
    Sint32 *column, *row;
};
#define CURSOR_CELL 12
enum cursor_phase {
    CURSOR_INIT,
    CURSOR_DELAY,
    CURSOR_GROW,
    CURSOR_PULSE,
    CURSOR_CLOSE_DELAY,
    CURSOR_SHRINK,
    CURSOR_DONE
};
/* the cursor's work. tick is 0, so CURSOR_DELAY has nothing to wait */
struct walk_cursor_work {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    Angle phase, unk_0c[2];
    NJS_POINT3 unk_14;
    Float scale, unk_24[2];
};
static void step_cursor(struct task *task)
{
    struct walk_cursor_work *pulse = (struct walk_cursor_work *)task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (pulse->mode) {
    case CURSOR_INIT:
        pulse->timer = pulse->tick;
        WALK_SET_PHASE(pulse, CURSOR_DELAY);
        break;
    case CURSOR_DELAY:
        if (pulse->timer > 0) {
            pulse->timer--;
        } else {
            WALK_SET_PHASE(pulse, CURSOR_GROW);
        }
        break;
    case CURSOR_GROW:
        if (pulse->scale < 1.0F) {
            pulse->scale += 0.05F;
        } else {
            pulse->scale = 1.0F;
            WALK_SET_PHASE(pulse, CURSOR_PULSE);
        }
        break;
    case CURSOR_PULSE:
        pulse->scale = 1.0F + (njSin(pulse->phase) * 0.1F);
        pulse->phase += 0x1000;
        if (pulse->phase > FULL_TURN) {
            pulse->phase -= FULL_TURN;
        }
        break;
    case CURSOR_CLOSE_DELAY:
        if (pulse->timer > 0) {
            pulse->timer--;
        } else {
            WALK_SET_PHASE(pulse, CURSOR_SHRINK);
        }
        break;
    case CURSOR_SHRINK:
        if (pulse->scale > 0.0F) {
            pulse->scale -= 0.05F;
        } else {
            pulse->scale = 0.0F;
            WALK_SET_PHASE(pulse, CURSOR_DONE);
        }
        break;
    case CURSOR_DONE:
        task_kill(task);
        return;
    }
    switch (pulse->mode) {
    case CURSOR_INIT:
    case CURSOR_DELAY:
    case CURSOR_GROW:
    case CURSOR_CLOSE_DELAY:
    case CURSOR_SHRINK:
    case CURSOR_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (pulse->mode < CURSOR_CLOSE_DELAY &&
        walk_menu_mode() == WALK_MENU_CLOSING) {
        pulse->timer = 10;
        WALK_SET_PHASE(pulse, CURSOR_CLOSE_DELAY);
    }
}
static void draw_cursor(struct task *task)
{
    struct walk_cursor_work *pulse = (struct walk_cursor_work *)task->work;
    struct walk_cursor *cursor = task->extra.p;
    NJS_SPRITE sprite;
    WALK_BLEND();
    WALK_CLEAR_COLOR();
    if (pulse->mode < CURSOR_GROW || pulse->mode > CURSOR_SHRINK) {
        return;
    }
    switch (cursor->grid) {
    case WALK_CURSOR_DRIVES:
        sprite.p.x = walk_card_cursor_left + (*cursor->column * 128.0F) - 12.0F;
        sprite.p.y = 164.0F + (*cursor->row * 130.0F);
        break;
    case WALK_CURSOR_CONTENTS:
        /* the cursor doesn't mark the button's row, below the contents' two */
        if (*cursor->row > 1) {
            return;
        }
        sprite.p.x =
            walk_content_cursor_left + (*cursor->column * 98.0F) - 12.0F;
        sprite.p.y = 143.0F + (*cursor->row * 80.0F);
        break;
#ifdef NONMATCHING
    /* this side draws no cursor for a grid the switch doesn't name. the
     * matching side leaves that grid's sprite unset */
    default:
        return;
#endif
    }
    sprite.sx = pulse->scale;
    sprite.sy = pulse->scale;
    sprite.tlist = garden_walk_texture_list;
    sprite.tanim = walk_cells;
    DRAW_CELL(sprite, CURSOR_CELL, -5.0F);
}
static void destroy_cursor(struct task *task)
{
    garden_walk_menu->cursor_task = NULL;
}
struct task *walk_create_cursor(struct task *parent, int delay, Sint8 grid)
{
    struct task *task;
    struct walk_cursor_work *pulse;
    struct walk_cursor *cursor;
    if (garden_walk_menu->cursor_task) {
        return NULL;
    }
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_cursor, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_cursor;
    task->destructor = destroy_cursor;
    pulse = (struct walk_cursor_work *)task->work;
    cursor = syCalloc(1, sizeof(struct walk_cursor));
    if (!cursor) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = cursor;
    pulse->scale = 0.0F;
    cursor->column = &garden_walk_menu->column;
    cursor->row = &garden_walk_menu->row;
    cursor->grid = grid;
    garden_walk_menu->cursor_task = task;
    return task;
}

static void step_item_preview(struct task *task)
{
#ifdef NONMATCHING
#else
    struct walk_model *frame = (struct walk_model *)task->work;
    NJS_MATRIX transform;
    if (game_state.time_stopped) {
        return;
    }
    /* @hack redundant: a transform past a return, and the matrix it uses,
     * where the step would end at the test above */
    if (1) {
        return;
    }
    njRotateY((NJS_MATRIX *)transform, frame->yaw);
    njCalcPoint((NJS_MATRIX *)transform, &frame->position, &frame->position);
    njRotateY((NJS_MATRIX *)transform, frame->yaw);
#endif
}
static void draw_item_preview(struct task *task)
{
    struct walk_model *frame = (struct walk_model *)task->work;
    WALK_BLEND();
    njPushMatrixEx();
    njSetTexture(&egg_texlist);
    njTranslateEx(&frame->position);
    njRotateY(NULL, walk_card_chao_yaw + garden_walk_menu->rotation);
    if (frame->id < GARDEN_SEED_TYPES) {
        njScale(NULL, walk_seed_scale, walk_seed_scale, walk_seed_scale);
        njCnkEasyDrawObject(garden_seed_models[frame->id]);
    } else {
        njScale(NULL, walk_fruit_scale, walk_fruit_scale, walk_fruit_scale);
        /* @hack type: the index cast to int, where the int sum would do */
        njCnkEasyDrawObject(
            garden_fruit_models[(int)(frame->id + GARDEN_FRUIT_ITEM_OFFSET)]);
    }
    njPopMatrixEx();
}
static void destroy_item_preview(struct task *task)
{
}
struct task *walk_create_item_preview(struct task *parent, Sint8 column,
                                      Sint8 row, Uint8 item)
{
    struct task *task;
    struct walk_model *frame;
    NJS_POINT3 base;
    NJS_POINT3 eye;
    NJS_POINT3 position;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    Angle yaw;
    Sint16 x;
    Sint16 y;
#ifdef NONMATCHING
    /* this side shows no preview for an item past the fruit's kinds */
    if (item >= GARDEN_FRUIT_TYPES - GARDEN_FRUIT_ITEM_OFFSET) {
        return NULL;
    }
#endif
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_item_preview, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_item_preview;
    task->destructor = destroy_item_preview;
    frame = (struct walk_model *)task->work;
    frame->id = item;
    frame->column = column;
    frame->row = row;
    /* @hack redundant: the copy of walk_origins' entry, whose x and y the
     * placement sets and whose z nothing reads */
    base = walk_origins[WALK_ITEM_ORIGIN];
    /* @hack type: x and y, Sint16 copies of frame's column and row, where
     * frame's own would do */
    x = frame->column;
    y = frame->row;
    eye = camera_views[0]->pos;
    direction = camera_views[0]->dir;
    offset.x = offset.y = 0.0F;
    offset.z = -walk_content_chao_depth;
    /* @hack spelling: base worked out alike in each branch, where once before
     * the test would do */
    if (frame->id < GARDEN_SEED_TYPES) {
        base.x = (walk_content_chao_left + walk_item_left - SCREEN_HALF_WIDTH) *
                 UNITS_PER_PIXEL;
        base.y = (walk_content_chao_top + walk_item_top - SCREEN_HALF_HEIGHT) *
                 UNITS_PER_PIXEL;
        position.x = base.x + (x * walk_content_chao_units_x *
                               walk_content_chao_column_spacing);
        position.y = base.y + (y * walk_content_chao_units_y *
                               walk_content_chao_row_spacing);
    } else {
        base.x = (walk_content_chao_left + walk_item_left - SCREEN_HALF_WIDTH) *
                 UNITS_PER_PIXEL;
        base.y = (walk_content_chao_top + walk_item_top - SCREEN_HALF_HEIGHT) *
                 UNITS_PER_PIXEL;
        position.x =
            base.x +
            ((walk_item_column_offset + (x * (walk_content_chao_column_spacing +
                                              walk_item_column_padding))) *
             walk_content_chao_units_x);
        position.y =
            base.y +
            ((walk_item_row_offset + (y * walk_content_chao_row_spacing)) *
             walk_content_chao_units_y);
    }
    position.z = 0.0F;
    yaw = WALK_CAMERA_YAW(direction);
    CARRY_TO_WORLD(eye, yaw, offset, position);
    frame->position = position;
    frame->yaw = walk_card_chao_yaw + garden_walk_menu->rotation;
    return task;
}

/* the button's work. scale springs toward 1 at speed, and phase turns
 * while the button has the cursor */
struct walk_button {
    Sint8 mode, submode;
    Uint8 unk_02[2];
    Sint16 flags;
    Uint16 timer;
    Sint32 unk_08;
    Angle phase;
    Sint32 unk_10[3];
    Float scale;
    Sint32 unk_20;
    Float speed;
};
enum button_phase { BUTTON_DELAY, BUTTON_OPEN, BUTTON_CLOSE };
/* the cursor's row below the contents, where the button is */
#define BUTTON_ROW 2
/* set while the button has the cursor. nothing but the step that sets and
 * clears it reads it */
#define BUTTON_FOCUSED 0x100
static void step_receive_button(struct task *task)
{
    struct walk_button *button = (struct walk_button *)task->work;
    switch (button->mode) {
    case BUTTON_DELAY:
        if (button->timer > 0) {
            button->timer--;
        } else {
            WALK_SET_PHASE(button, BUTTON_OPEN);
        }
        break;
    case BUTTON_OPEN:
        button->scale += button->speed;
        button->speed += (1.0F - button->scale) * 0.5F;
        button->speed *= 0.85F;
        /* @hack redundant: the tests of BUTTON_FOCUSED before it is set and
         * before it is cleared */
        if (garden_walk_menu->row == BUTTON_ROW) {
            button->phase += 0x500;
            if (!(button->flags & BUTTON_FOCUSED)) {
                button->flags |= BUTTON_FOCUSED;
            }
        } else {
            if (button->flags & BUTTON_FOCUSED) {
                button->flags &= ~BUTTON_FOCUSED;
            }
            button->phase = angle_approach(button->phase, 0, 0x500);
        }
        break;
    case BUTTON_CLOSE:
        button->scale += 0.14F;
        if (button->scale > 1.8F) {
            task_kill(task);
        }
        break;
    }
    switch (button->mode) {
    case BUTTON_DELAY:
    case BUTTON_CLOSE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (button->mode < BUTTON_CLOSE &&
        garden_walk_menu->mode == WALK_MENU_CLOSING) {
        WALK_SET_PHASE(button, BUTTON_CLOSE);
    }
}
static void draw_receive_button(struct task *task)
{
    struct walk_button *button = (struct walk_button *)task->work;
    NJS_SPRITE sprite;
    Float pulse;
    Float sx;
    Float sy;
    Sint8 i;
    int first;
    if (button->timer > 0) {
        return;
    }
    pulse = 1.05F - (njCos(button->phase) * 0.05F);
    sx = button->scale;
    sy = 2.0F - sx;
    sx *= pulse;
    sy *= pulse;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = sx;
    sprite.sy = sy;
    sprite.tlist = garden_walk_texture_list;
    sprite.tanim = walk_cells;
    WALK_BLEND();
    WALK_CLEAR_COLOR();
    sprite.p.x = SCREEN_HALF_WIDTH - (sx * 64.0F);
    sprite.p.y = 288.0F - (sy * 12.0F);
    if (garden_walk_menu->row == BUTTON_ROW) {
        first = 32;
    } else {
        first = 29;
    }
    for (i = 0; i < 3; i++) {
        DRAW_CELL(sprite, first + i, -11.0F);
        if (first == 32) {
            DRAW_CELL(sprite, 26 + i, -12.0F);
        }
    }
    sprite.p.x = SCREEN_HALF_WIDTH - (sx * 33.0F);
    sprite.p.y = 288.0F - (sy * 8.0F);
    njDrawSprite2D(&sprite, 24, -10.0F, sprite.tanim[first].attr);
    DRAW_CELL(sprite, 25, -10.0F);
}
static void destroy_receive_button(struct task *task)
{
}
void walk_create_receive_button(struct task *parent)
{
    struct task *task;
    struct walk_button *button;
    task = task_create_child(TASK_WORK, step_receive_button, parent);
#ifdef NONMATCHING
    /* this side makes no button where task_create_child fails. the matching
     * side writes through the null */
    if (!task) {
        return;
    }
#endif
    task->draw = draw_receive_button;
    task->destructor = destroy_receive_button;
    button = (struct walk_button *)task->work;
    button->timer = 70;
    button->phase = 0;
    button->scale = 0.3F;
}

struct walk_page {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    Angle unk_08, yaw, unk_10;
    NJS_POINT3 position;
    Float unk_20[2], alpha;
};
/* the page's icons. the count is a Sint8, as the loops over the icons are */
#define PAGE_ICONS ((Sint8)3)
struct walk_page_icon {
    Sint8 kind;
    NJS_POINT3 position;
    Float scale;
};
/* the page's drives. drive is the one in the middle, between drive - 1 and
 * drive + 1. selected points at the one to show, and the page turns to it.
 * points are where the three icons rest. step is how far each moves on a
 * step of a turn */
struct walk_page_icons {
    /* 2 after a turn toward a lower drive, and 1 after one toward a higher.
     * nothing reads it */
    Sint8 direction;
    struct walk_page_icon icons[PAGE_ICONS];
    Sint32 drive, *selected;
    NJS_POINT3 points[PAGE_ICONS];
    NJS_VECTOR step;
};
enum page_phase {
    PAGE_INIT,
    PAGE_DELAY,
    PAGE_PLACE,
    PAGE_FADE_IN,
    PAGE_OPEN,
    PAGE_UNUSED_FIVE,
    PAGE_UNUSED_SIX,
    PAGE_TURN,
    PAGE_CLOSE,
    PAGE_DONE
};
/* read each icon's drive type anew, through i and drive */
#define POLL_PAGE_DRIVES(icons, i, drive)                                      \
    for ((i) = 0; (i) < PAGE_ICONS; (i)++) {                                   \
        (drive) = (icons)->drive + (i) - 1;                                    \
        (icons)->icons[i].kind = garden_walk_drive_type(drive);                \
    }
static void step_drive_page(struct task *task)
{
    struct walk_page *page = (struct walk_page *)task->work;
    struct walk_page_icons *icons = task->extra.p;
    NJS_POINT3 base;
    NJS_POINT3 center;
    NJS_POINT3 eye;
    NJS_POINT3 point;
    NJS_VECTOR direction;
    NJS_VECTOR offset;
    NJS_VECTOR shift;
    Angle yaw;
    Sint8 i;
    Sint8 drive;
    Float *scale;
    Float full_scale;
    if (game_state.time_stopped) {
        return;
    }
    switch (page->mode) {
    case PAGE_INIT:
        page->timer = 60;
        WALK_SET_PHASE(page, PAGE_DELAY);
        break;
    case PAGE_DELAY:
        if (page->timer > 0) {
            page->timer--;
        } else {
            WALK_SET_PHASE(page, PAGE_PLACE);
        }
        break;
    case PAGE_PLACE: {
        Sint8 index;
        /* @hack redundant: the copy of walk_origins' entry, whose every field
         * the placement sets */
        base = walk_origins[WALK_PAGE_ORIGIN];
        eye = camera_views[0]->pos;
        direction = camera_views[0]->dir;
        offset.x = offset.y = 0.0F;
        offset.z = -walk_page_depth;
        base.x = walk_page_units_x * walk_page_left;
        base.y = walk_page_units_y * walk_page_top;
        base.z = 0.0F;
        shift.x = walk_drive_units_x * walk_page_card_spacing;
        shift.y = 0.0F;
        shift.z = 0.0F;
        yaw = WALK_CAMERA_YAW(direction);
        matrix_push_unit();
        njTranslateEx(&eye);
        njRotateY(NULL, yaw);
        njTranslateEx(&offset);
        njCalcPoint(NULL, &base, &center);
        vector_sum(&base, &shift, &point);
        njCalcPoint(NULL, &point, &icons->points[2]);
        shift.x = -shift.x;
        vector_sum(&base, &shift, &point);
        njCalcPoint(NULL, &point, &icons->points[0]);
        njPopMatrixEx();
        page->position = center;
        page->yaw = walk_page_yaw + garden_walk_menu->rotation;
        icons->points[1] = center;
        icons->drive = *icons->selected;
        for (index = 0; index < PAGE_ICONS; index++) {
            drive = icons->drive + index - 1;
            icons->icons[index].position = icons->points[index];
            if (drive < 0 || drive > CARD_DRIVES - 1) {
                icons->icons[index].kind = WALK_DRIVE_NONE;
                continue;
            }
            if (index == 1) {
                icons->icons[index].scale = walk_page_card_scale;
            } else {
                icons->icons[index].scale = walk_page_card_scale * 0.9F;
            }
            icons->icons[index].kind = garden_walk_drive_type(drive);
        }
        WALK_SET_PHASE(page, PAGE_FADE_IN);
        break;
    }
    case PAGE_FADE_IN:
        POLL_PAGE_DRIVES(icons, i, drive);
        page->alpha += 0.2F;
        if (page->alpha > 1.0F) {
            page->alpha = 1.0F;
            WALK_SET_PHASE(page, PAGE_OPEN);
        }
        break;
    case PAGE_OPEN:
        POLL_PAGE_DRIVES(icons, i, drive);
        break;
    case PAGE_TURN:
        page->alpha -= 0.05F;
        if (page->alpha <= 0.0F) {
            page->alpha = 0.0F;
            WALK_SET_PHASE(page, PAGE_PLACE);
            return;
        }
        for (i = 0; i < PAGE_ICONS; i++) {
            if (icons->icons[i].kind) {
                njAddVector(&icons->icons[i].position, &icons->step);
                scale = &icons->icons[i].scale;
                if (i == 1) {
                    full_scale = walk_page_card_scale;
                    if (full_scale * 0.95F > *scale) {
                        *scale = full_scale * 0.95F;
                    } else {
                        *scale -= full_scale * (0.05F * 0.05F);
                    }
                } else {
                    if (walk_page_card_scale > *scale) {
                        *scale = walk_page_card_scale;
                    } else {
                        *scale += walk_page_card_scale * (0.05F * 0.05F);
                    }
                }
            }
        }
        break;
    case PAGE_CLOSE:
        page->alpha -= 0.05F;
        if (page->alpha < 0.0F) {
            page->alpha = 0.0F;
            task_kill(task);
            return;
        }
        break;
    }
    switch (page->mode) {
    case PAGE_INIT:
    case PAGE_DELAY:
    case PAGE_PLACE:
    case PAGE_FADE_IN:
    case PAGE_TURN:
    case PAGE_CLOSE:
    case PAGE_DONE:
        walk_set_menu_busy(TRUE);
        break;
    }
    if (icons->drive != *icons->selected) {
        if (*icons->selected < icons->drive) {
            icons->direction = 2;
            icons->icons[2].kind = WALK_DRIVE_NONE;
            vector_diff(&icons->points[2], &icons->points[1], &icons->step);
#ifdef NONMATCHING
#else
            /* @hack redundant: an empty else if (0) between the two turns */
        } else if (0) {
#endif
        } else {
            icons->direction = 1;
            icons->icons[0].kind = WALK_DRIVE_NONE;
            vector_diff(&icons->points[0], &icons->points[1], &icons->step);
        }
        icons->step.x *= 0.05F;
        icons->step.y = 0.0F;
        icons->step.z *= 0.05F;
        WALK_SET_PHASE(page, PAGE_TURN);
        return;
    }
    if (page->mode >= PAGE_CLOSE ||
        garden_walk_menu->mode != WALK_MENU_CLOSING) {
        return;
    }
    WALK_SET_PHASE(page, PAGE_CLOSE);
}
/* the first cells of the connectors' letters, A to D, and of the sockets'
 * digits, 1 and 2. each connector has SOCKETS sockets */
#define LETTER_A_CELL 37
#define DIGIT_1_CELL 35
#define SOCKETS 2
static void draw_drive_page(struct task *task)
{
    NJS_SPRITE sprite;
    struct walk_page *page = (struct walk_page *)task->work;
    struct walk_page_icons *icons = task->extra.p;
    Sint8 i;
    Sint8 drive;
    Sint8 connector;
    Sint8 socket;
    WALK_SPRITE(sprite, 0.0F, 0.0F);
    WALK_BLEND();
    for (i = 0; i < PAGE_ICONS; i++) {
        drive = icons->drive + i - 1;
        if (drive < 0 || drive > CARD_DRIVES - 1) {
            continue;
        }
        connector = drive / SOCKETS;
        socket = drive % SOCKETS;
        WALK_WHITE(page->alpha);
        switch (i) {
        case 0:
            sprite.p.x = 176.0F;
            break;
        case 1:
            sprite.p.x = 299.0F;
            break;
        case 2:
            sprite.p.x = 418.0F;
            break;
        }
        sprite.p.y = 381.0F;
        /* the drive's name is its connector's letter and its socket's digit.
         * each takes the attr of a cell the other's index picks. the attr is
         * the same, as every cell from DIGIT_1_CELL to the last letter's
         * holds one */
        njDrawSprite2D(&sprite, LETTER_A_CELL + connector, -10.0F,
                       sprite.tanim[DIGIT_1_CELL + connector].attr);
        sprite.p.x += 22.0F;
        njDrawSprite2D(&sprite, DIGIT_1_CELL + socket, -10.0F,
                       sprite.tanim[LETTER_A_CELL + socket].attr);
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: a push and two stores that never run */
    if (0) {
        njPushMatrixEx();
        sprite.p.x = -10.0F;
        sprite.p.y = -10.0F;
    }
#endif
    njPushMatrixEx();
    njSetTexture(&egg_texlist);
    for (i = 0; i < PAGE_ICONS; i++) {
        if (icons->icons[i].kind) {
            njPushMatrixEx();
            njTranslateEx(&icons->icons[i].position);
            njRotateY(NULL, walk_page_yaw + garden_walk_menu->rotation);
            njScale(NULL, icons->icons[i].scale, icons->icons[i].scale, 0.1F);
            if (icons->icons[i].kind == WALK_DRIVE_CARD) {
                njCnkEasyDrawObject(&walk_vmu_model);
            } else {
                njCnkEasyDrawObject(&walk_multi_vmu_model);
            }
            njPopMatrixEx();
        }
    }
    njPopMatrixEx();
}
static void destroy_drive_page(struct task *task)
{
}
struct task *walk_create_drive_page(struct task *parent, Sint32 *drive)
{
    struct task *task;
    struct walk_page *page;
    struct walk_page_icons *icons;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_drive_page, parent);
    if (!task) {
        return NULL;
    }
    page = (struct walk_page *)task->work;
    task->draw = draw_drive_page;
    task->destructor = destroy_drive_page;
    icons = syCalloc(1, sizeof(struct walk_page_icons));
    if (!icons) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = icons;
    page->alpha = 0.0F;
    icons->drive = *drive;
    icons->selected = drive;
    return task;
}

struct walk_page_arrow {
    Sint8 mode, submode;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    Sint32 unk_08[2], side;
    Float x, y, width, alpha, scale;
};
enum arrow_side { ARROW_LEFT, ARROW_RIGHT };
enum arrow_phase {
    ARROW_DELAY,
    ARROW_FADE_IN,
    ARROW_EXTEND,
    ARROW_READY,
    ARROW_REVEAL,
    ARROW_HIDE,
    ARROW_PRESS,
    ARROW_STRETCH,
    ARROW_RELEASE,
    ARROW_CLOSE_DELAY,
    ARROW_FADE_OUT,
    ARROW_DONE
};
static void draw_page_arrow(struct task *task)
{
    struct walk_page_arrow *arrow = (struct walk_page_arrow *)task->work;
    NJS_SPRITE sprite;
    if (arrow->mode <= ARROW_DELAY) {
        return;
    }
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = arrow->scale;
    sprite.sy = arrow->scale;
    sprite.tlist = garden_walk_texture_list;
    sprite.tanim = walk_cells;
    WALK_BLEND();
    WALK_WHITE(arrow->alpha);
    if (arrow->side == ARROW_LEFT) {
        sprite.p.x = arrow->x;
        sprite.p.y = arrow->y;
        DRAW_CELL(sprite, 10, -10.0F);
        sprite.p.x = arrow->x - arrow->width - 24.0F;
        DRAW_CELL(sprite, 7, -10.0F);
        sprite.p.x = arrow->x - arrow->width;
        sprite.sx = arrow->width + 1.0F;
        DRAW_CELL(sprite, 8, -11.0F);
    } else {
        sprite.p.x = arrow->x;
        sprite.p.y = arrow->y;
        DRAW_CELL(sprite, 11, -10.0F);
        sprite.p.x = arrow->x + arrow->width + 24.0F;
        DRAW_CELL(sprite, 9, -10.0F);
        sprite.p.x = arrow->x + 24.0F;
        sprite.sx = arrow->width;
        DRAW_CELL(sprite, 8, -11.0F);
    }
}
static void step_page_arrow(struct task *task)
{
    struct walk_page_arrow *arrow = (struct walk_page_arrow *)task->work;
    Sint32 menu_mode;
    switch (arrow->mode) {
    case ARROW_DELAY:
        if (arrow->tick++ > arrow->timer) {
            arrow->tick = 0;
            WALK_SET_PHASE(arrow, ARROW_FADE_IN);
        }
        break;
    case ARROW_FADE_IN:
        if (arrow->alpha < 1.0F) {
            arrow->alpha += 0.07F;
        } else {
            arrow->alpha = 1.0F;
            WALK_SET_PHASE(arrow, ARROW_EXTEND);
        }
        break;
    case ARROW_EXTEND:
        if (arrow->width < 14.0F) {
            arrow->width += 2.0F;
        } else {
            arrow->width = 14.0F;
            WALK_SET_PHASE(arrow, ARROW_READY);
            if (arrow->side == ARROW_LEFT) {
                if (garden_walk_menu->selected != 0) {
                    WALK_SET_PHASE(arrow, ARROW_HIDE);
                }
            } else if (garden_walk_menu->selected != CARD_DRIVES - 1) {
                WALK_SET_PHASE(arrow, ARROW_HIDE);
            }
        }
        break;
    case ARROW_READY:
        break;
    case ARROW_HIDE:
        if (arrow->alpha > 0.0F) {
            arrow->alpha -= 0.025F;
        } else {
            arrow->alpha = 0.0F;
        }
        if (arrow->side == ARROW_LEFT) {
            if (garden_walk_menu->selected != 0) {
                WALK_SET_PHASE(arrow, ARROW_REVEAL);
            }
        } else if (garden_walk_menu->selected != CARD_DRIVES - 1) {
            WALK_SET_PHASE(arrow, ARROW_REVEAL);
        }
        break;
    case ARROW_REVEAL:
        if (arrow->alpha < 1.0F) {
            arrow->alpha += 0.025F;
        } else {
            arrow->alpha = 1.0F;
            WALK_SET_PHASE(arrow, ARROW_READY);
        }
        break;
    case ARROW_PRESS:
        if (arrow->scale < 1.5F) {
            arrow->scale += 0.1F;
        } else {
            arrow->scale = 1.5F;
            WALK_SET_PHASE(arrow, ARROW_STRETCH);
        }
        break;
    case ARROW_STRETCH:
        if (arrow->scale > 1.0F) {
            arrow->scale -= 0.15F;
        } else {
            arrow->scale = 1.0F;
        }
        if (arrow->width < 36.0F) {
            arrow->width += 2.0F;
        } else {
            WALK_SET_PHASE(arrow, ARROW_RELEASE);
        }
        break;
    case ARROW_RELEASE:
        if (arrow->scale > 1.0F) {
            arrow->scale -= 0.1F;
        } else {
            arrow->scale = 1.0F;
        }
        if (arrow->width > 14.0F) {
            arrow->width -= 1.0F;
        } else {
            arrow->width = 14.0F;
            WALK_SET_PHASE(arrow, ARROW_READY);
            if (arrow->side == ARROW_LEFT) {
                if (garden_walk_menu->selected == 0) {
                    WALK_SET_PHASE(arrow, ARROW_HIDE);
                }
            } else if (garden_walk_menu->selected == CARD_DRIVES - 1) {
                WALK_SET_PHASE(arrow, ARROW_HIDE);
            }
        }
        break;
    case ARROW_CLOSE_DELAY:
        if (arrow->tick++ > 20) {
            WALK_SET_PHASE(arrow, ARROW_FADE_OUT);
        }
        break;
    case ARROW_FADE_OUT:
        if (arrow->alpha > 0.0F) {
            arrow->alpha -= 0.025F;
        } else {
            arrow->alpha = 0.0F;
            WALK_SET_PHASE(arrow, ARROW_DONE);
        }
        break;
    case ARROW_DONE:
        task_kill(task);
        break;
    }
    switch (arrow->mode) {
    case ARROW_READY:
    case ARROW_PRESS:
    case ARROW_STRETCH:
    case ARROW_RELEASE:
        if (arrow->side == ARROW_LEFT) {
            if (player_pads[0]->on & PDD_DGT_TL) {
                WALK_SET_PHASE(arrow, ARROW_PRESS);
            }
        } else if (player_pads[0]->on & PDD_DGT_TR) {
            WALK_SET_PHASE(arrow, ARROW_PRESS);
        }
        break;
    }
    if (arrow->mode < ARROW_CLOSE_DELAY) {
        menu_mode = garden_walk_menu->mode;
        if (menu_mode == WALK_MENU_FREEZE || menu_mode == WALK_MENU_CLOSING) {
            WALK_SET_PHASE(arrow, ARROW_CLOSE_DELAY);
        }
    }
}
static void destroy_page_arrow(struct task *task)
{
}
static void create_page_arrow(struct task *parent, Sint32 side)
{
    struct task *task;
    struct walk_page_arrow *arrow;
    task = task_create_child(TASK_WORK, step_page_arrow, parent);
#ifdef NONMATCHING
    /* this side makes no arrow where task_create_child fails. the matching side
     * writes through the null */
    if (!task) {
        return;
    }
#endif
    task->draw = draw_page_arrow;
    task->destructor = destroy_page_arrow;
    arrow = (struct walk_page_arrow *)task->work;
    arrow->side = side;
    if (arrow->side == ARROW_LEFT) {
        arrow->x = 266.0F;
        arrow->y = 313.0F;
    } else {
        arrow->x = 348.0F;
        arrow->y = 313.0F;
    }
    arrow->width = 14.0F;
    arrow->scale = 1.0F;
    arrow->alpha = 0.0F;
    arrow->timer = 120;
}
void walk_create_page_arrows(struct task *parent)
{
    create_page_arrow(parent, ARROW_LEFT);
    create_page_arrow(parent, ARROW_RIGHT);
}

static void draw_name_letter(NJS_POINT3 *position, char letter, int flags)
{
    Uint8 code = letter;
    NJS_SPRITE sprite;
    NJS_TEXANIM cell;
    Float tex_u;
    Float tex_v;
    sprite.p.x = position->x;
    sprite.p.y = position->y;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = 1.0F;
    sprite.sy = 1.0F;
    sprite.tlist = &chao_common_texlist;
    sprite.tanim = &cell;
    tex_u = (code % 16) * 16.0F;
    tex_v = (code >> 4) * 16.0F;
    cell.sx = cell.sy = 14;
    cell.cx = cell.cy = 0;
    cell.u1 = tex_u;
    cell.u2 = tex_u + 16.0F;
    cell.v1 = tex_v;
    cell.v2 = tex_v + 16.0F;
    cell.texid = 0;
    njDrawSprite2D(&sprite, 0, position->z,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
}
static void draw_name_plate(NJS_POINT3 *position, const char *name, int flags,
                            Float scale)
{
    NJS_POINT3 pen;
    NJS_SPRITE sprite;
    const char *letter;
    int length;
    pen = *position;
    length = 0;
    WALK_BLEND();
    for (letter = name; *letter; letter++) {
        length++;
    }
    if (!length) {
        return;
    }
    constant_material_set(1.0F, 0.0F, 130.0F / 255.0F, 240.0F / 255.0F);
    pen.x += 4.0F - (length * 8.0F);
    pen.y += 5.0F;
    pen.z += 0.2F;
    letter = name;
    while (*letter != '\0') {
        draw_name_letter(&pen, *letter++, flags);
        pen.x += 16.0F;
    }
    pen = *position;
    pen.x += 4.0F - (length * 8.0F);
    sprite.p.x = pen.x - 8.0F;
    sprite.p.y = pen.y;
    sprite.p.z = 0.0F;
    sprite.ang = 0;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.tlist = garden_walk_texture_list;
    sprite.tanim = walk_cells;
    WALK_WHITE(1.0F);
    DRAW_CELL(sprite, 41, position->z);
    sprite.p.x += 8.0F;
    sprite.sx = length * 2;
    DRAW_CELL(sprite, 42, position->z);
    sprite.p.x += sprite.sx * 8.0F;
    sprite.sx = 1.0F;
    DRAW_CELL(sprite, 44, position->z + 0.1F);
    sprite.p.x -= 4.0F;
    DRAW_CELL(sprite, 43, position->z);
    WALK_CLEAR_COLOR();
}
static void step_preview_name(struct task *task)
{
}
static void destroy_preview_name(struct task *task)
{
}
static void draw_preview_name(struct task *task)
{
    struct task_work *work = task->work;
    struct task *chao;
    struct chao_work *actor;
#ifdef NONMATCHING
#else
    /* @hack redundant: a position nothing uses */
    NJS_POINT3 position;
#endif
    chao = walk_preview_chao(task->parent);
    if (chao) {
        actor = (struct chao_work *)chao->work;
#ifdef NONMATCHING
        /* a name can fill the profile's eight bytes, with no terminator */
        {
            char name[sizeof(actor->profile.parameters.name) + 1];
            memcpy(name, actor->profile.parameters.name, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            draw_name_plate(&work->pos, name, 0, 1.0F);
        }
#else
        draw_name_plate(&work->pos, actor->profile.parameters.name, 0, 1.0F);
#endif
    } else {
        task_kill(task);
    }
}
struct task *walk_show_preview_name(struct task *parent, NJS_POINT3 *position)
{
    struct task *task;
    struct task_work *work;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_preview_name, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_preview_name;
    task->destructor = destroy_preview_name;
    work = task->work;
    work->pos = *position;
    return task;
}
void walk_hide_preview_name(struct task *preview)
{
    struct task *name;
    if (preview) {
        name = preview->child;
        if (name) {
            task_kill(name);
        }
    }
}
void fun_0c522b48(struct task *task) /* NOLINT(misc-use-internal-linkage) */
{
}
