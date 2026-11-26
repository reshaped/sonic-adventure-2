/* the memory card warnings that MCWARN.PRS and ADVERTISE.PRS show at the
 * screen's center. their texts come from the mcwarn file */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <string.h>

#include "draw/color.h"
#include "draw/text_sprite.h"
#include "save/save.h"
#include "shared/card_warning.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/text.h"

/* the mcwarn file starts with a long for each text. a long below RAM is its
 * text's offset in the file. one at RAM or above is the text's address */
#define RAM ((char *)0x8c000000)

/* values of game_state.card_warning that neither save/save.h nor
 * system/game_state.h names, named for the texts card_text_get gives them */
#define WARNING_NO_PAD 4
#define WARNING_SAVE_FAILED 8

/* the screen's center, which a warning centers its text on, in pixels */
#define SCREEN_CENTER_X 320
#define SCREEN_CENTER_Y 240

/* the colors of a warning's text, its shadow and the band behind them, as
 * 0xRRGGBB. each color fades in to its _ALPHA, by FADE_STEP a step */
#define TEXT_RGB 0xb8c0c0
#define SHADOW_RGB 0x503010
#define BAND_RGB 0xb46c28
#define TEXT_ALPHA 0xe0
#define SHADOW_ALPHA 0xff
#define BAND_ALPHA 0xa0
#define FADE_STEP 4
/* the bits of a color's alpha, as a packed ARGB word holds it */
#define ALPHA_BITS 0xff000000
#define TEXT_COLOR (((Uint32)TEXT_ALPHA << ALPHA_SHIFT) | TEXT_RGB)
#define SHADOW_COLOR (((Uint32)SHADOW_ALPHA << ALPHA_SHIFT) | SHADOW_RGB)
#define BAND_COLOR (((Uint32)BAND_ALPHA << ALPHA_SHIFT) | BAND_RGB)

/* ALPHA_RAISE raises the alpha of field, an NJS_COLOR, by FADE_STEP to at
 * most top. ALPHA_LOWER lowers it by FADE_STEP to at least 0 */
#define ALPHA_RAISE(field, top)                                                \
    {                                                                          \
        Uint32 color = (field).color;                                          \
        Uint32 alpha = (Uint8)(color >> ALPHA_SHIFT);                          \
                                                                               \
        alpha += FADE_STEP;                                                    \
        if (alpha >= (top)) {                                                  \
            alpha = (top);                                                     \
        }                                                                      \
        color &= ~ALPHA_BITS;                                                  \
        alpha <<= ALPHA_SHIFT;                                                 \
        color |= alpha;                                                        \
        (field).color = color;                                                 \
    }
/* @hack type: color shifted as a Sint32 before its low byte is taken,
 * where color shifted alone would do */
#define ALPHA_LOWER(field)                                                     \
    {                                                                          \
        Uint32 color = (field).color;                                          \
        int alpha = (Uint8)((Sint32)color >> ALPHA_SHIFT);                     \
                                                                               \
        alpha -= FADE_STEP;                                                    \
        if (alpha < 0) {                                                       \
            alpha = 0;                                                         \
        }                                                                      \
        color &= ~ALPHA_BITS;                                                  \
        alpha = (Uint32)alpha << ALPHA_SHIFT;                                  \
        color |= alpha;                                                        \
        (field).color = color;                                                 \
    }

/* a warning's fade-in lasts FADE_IN_STEPS steps. the warnings of
 * no_card_warning_create and no_room_warning_create then hold for
 * HOLD_STEPS */
#define FADE_IN_STEPS 60
#define HOLD_STEPS 1200

/* the size of the buffer a warning formats its text in */
#define WARNING_TEXT_SIZE 512

/* a warning's task keeps its text sprite in unk_3c. ManTxt keeps there the
 * text language of the file it holds */
#define SPRITE(task) ((struct text_sprite *)(task)->unk_3c.p)
#define SET_SPRITE(task, sprite) ((task)->unk_3c.p = (sprite))
/* @hack type: the slot cleared with NULL cast to a Sint32, where 0 would do */
#define CLEAR_SPRITE(task)                                                     \
    SET_SPRITE(task, NULL) /* NOLINT(bugprone-casting-through-void) */
#define LANGUAGE(task) ((task)->unk_3c.l)

enum { MODE_FADE_IN, MODE_HOLD, MODE_FADE_OUT, MODE_ENDED };
enum {
    MODE_SAVE_START,
    MODE_SAVE_FADE_IN,
    MODE_SAVE_HOLD,
    MODE_SAVE_FADE_OUT,
    MODE_SAVE_CHANGED,
    MODE_SAVE_ENDED
};

/* ManTxt, the task that keeps the mcwarn file in its extra, or null */
static struct task *card_text_task = NULL;
static char *card_text_files[TEXT_LANGUAGES] = {
    "mcwarn_j.prs", "mcwarn_e.prs", "mcwarn_f.prs",
    "mcwarn_s.prs", "mcwarn_g.prs",
};
/* every warning's text while ManTxt holds no file */
/* @hack type: an array of one pointer, where the pointer would do */
static char *card_text_missing[1] = {"TEXT LOAD ERROR"};
static char *port_letters[PAD_PORTS] = {"A", "B", "C", "D"};

static void card_text_destroy(struct task *task)
{
    card_text_task = NULL;
}

/* ManTxt's step. where game_state.text_language has changed, it reads that
 * language's file in place of the last. where the read fails, the last file
 * stays, and the next step tries again */
static void card_text_step(struct task *task)
{
    void *file;

    if (game_state.text_language != LANGUAGE(task)) {
        file = loader_read_prs_alloc(card_text_files[game_state.text_language]);
        if (file != NULL) {
            LANGUAGE(task) = game_state.text_language;
            syFree(task->extra.p);
            task->extra.p = file;
        }
    }
}

static void card_text_start(void)
{
    if (card_text_task == NULL) {
        card_text_task = task_create(0, 0, card_text_step, "ManTxt");
        card_text_task->destructor = card_text_destroy;
        card_text_task->extra.p =
            loader_read_prs_alloc(card_text_files[game_state.text_language]);
        if (card_text_task->extra.p == NULL) {
            task_kill(card_text_task);
        } else {
            LANGUAGE(card_text_task) = game_state.text_language;
        }
    }
}

/* end ManTxt on its next step. nothing calls card_text_stop, and no header
 * declares it. it is external only because SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void card_text_stop(void)
{
    if (card_text_task != NULL) {
        task_kill(card_text_task);
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* the text of warning, a value of game_state.card_warning. a value without
 * a text gives card_text_missing's, as does every value while ManTxt holds
 * no file */
static char *card_text_get(int warning)
{
    Sint32 *file;
    char *text = card_text_missing[0];

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (card_text_task == NULL || (file = card_text_task->extra.p) == NULL) {
        return card_text_missing[0];
    }
    switch (warning) {
    case 0:
    case 1:
        break;
    case CARD_WARNING_NO_CARD:
        text = (char *)file[MCWARN_NO_CARD];
        break;
    case CARD_WARNING_NO_ROOM:
        text = (char *)file[MCWARN_BLOCKS];
        break;
    case WARNING_NO_PAD:
        text = (char *)file[MCWARN_NO_PAD];
        break;
    case SAVE_NOT_READY:
        text = (char *)file[MCWARN_SOCKET];
        break;
    case SAVE_NO_ROOM:
        text = (char *)file[MCWARN_SAVE_BLOCKS];
        break;
    case SAVE_NOT_OURS:
        text = (char *)file[MCWARN_NOT_OURS];
        break;
    case WARNING_SAVE_FAILED:
        text = (char *)file[MCWARN_SAVE_FAILED];
        break;
    case SAVE_BAD_CHECKSUM:
        text = (char *)file[MCWARN_LOAD_FAILED];
        break;
    }
    if (text < RAM) {
        text = (char *)file + (Uint32)text;
    }
    return text;
}

static void warning_destroy(struct task *task)
{
    struct text_sprite *sprite = SPRITE(task);

#ifdef NONMATCHING
    /* this side repairs, for warning_destroy, the tag in warning_step: it
     * destroys no null sprite */
    if (sprite != NULL) {
        text_sprite_destroy(sprite);
    }
#else
    text_sprite_destroy(sprite);
#endif
    CLEAR_SPRITE(task);
}

static void warning_step(struct task *task)
{
    struct task_work *work = task->work;
    struct text_sprite *sprite = SPRITE(task);

    /* @bug memory: reads through a null sprite where text_sprite_create
     * failed. no_card_warning_create and no_room_warning_create test the
     * sprite (at 0x354 and 0x406 of the object's code) and keep the task
     * without one. warning_step reads it untested (at 0x1fa and 0x27a), as
     * do warning_draw (at 0x2ee and 0x3a4) and warning_destroy (at 0x194) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a warning without a sprite ends on its
     * first step. warning_draw and warning_destroy pass over the null */
    if (sprite == NULL) {
        work->mode = MODE_ENDED;
        task_kill(task);
        return;
    }
#endif
    switch (work->mode) {
    case MODE_FADE_IN:
        if (work->timer++ >= FADE_IN_STEPS) {
            work->mode = MODE_HOLD;
            work->timer = 0;
        } else {
            ALPHA_RAISE(sprite->color, TEXT_ALPHA);
            ALPHA_RAISE(sprite->shadow_color, SHADOW_ALPHA);
            ALPHA_RAISE(sprite->band_color, BAND_ALPHA);
        }
        break;
    case MODE_HOLD:
        ++work->timer;
        if (work->timer >= HOLD_STEPS) {
            work->mode = MODE_FADE_OUT;
            work->timer = 0;
        }
        break;
    case MODE_FADE_OUT:
        if (!(sprite->color.color & ALPHA_BITS)) {
            work->mode = MODE_ENDED;
            task_kill(task);
            return;
        }
        ALPHA_LOWER(sprite->color);
        ALPHA_LOWER(sprite->shadow_color);
        ALPHA_LOWER(sprite->band_color);
        break;
    }
}

static void warning_draw(struct task *task)
{
    struct text_sprite *sprite = SPRITE(task);

#ifdef NONMATCHING
    /* this side repairs, for warning_draw, the tag in warning_step: it draws
     * nothing without a sprite */
    if (sprite == NULL) {
        return;
    }
#endif
    text_sprite_draw_all(sprite);
}

struct task *no_card_warning_create(void)
{
    struct task *task;
    struct text_sprite *sprite;
    char *text;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, 0, warning_step,
                            "DisplayMCWarnNoCardExecutor")) != NULL) {
        task->draw = warning_draw;
        task->destructor = warning_destroy;
    } else {
        return NULL;
    }
    card_text_start();
    text = card_text_get(CARD_WARNING_NO_CARD);
    sprite = text_sprite_create(text, game_state.text_language);
    if (sprite != NULL) {
        sprite->x = SCREEN_CENTER_X - (sprite->width >> 1);
        sprite->y = SCREEN_CENTER_Y - (sprite->height >> 1);
        sprite->band_y = sprite->y;
        sprite->color.color = TEXT_RGB;
        sprite->shadow_color.color = SHADOW_RGB;
        sprite->band_color.color = BAND_RGB;
        SET_SPRITE(task, sprite);
    }
    return task;
}

static void no_room_warning_destroy(struct task *task)
{
    warning_destroy(task);
}

static void no_room_warning_step(struct task *task)
{
    warning_step(task);
}

static void no_room_warning_draw(struct task *task)
{
    warning_draw(task);
}

struct task *no_room_warning_create(void)
{
    struct task *task;
    struct text_sprite *sprite;
    char text[WARNING_TEXT_SIZE];
    char *format;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, 0, no_room_warning_step,
                            "DisplayMCWarnNoCapacityExecutor")) != NULL) {
        task->draw = no_room_warning_draw;
        task->destructor = no_room_warning_destroy;
    } else {
        return NULL;
    }
    card_text_start();
    format = card_text_get(CARD_WARNING_NO_ROOM);
    sprintf(text, format, SAVE_BLOCKS);
    sprite = text_sprite_create(text, game_state.text_language);
    if (sprite != NULL) {
        sprite->x = SCREEN_CENTER_X - (sprite->width >> 1);
        sprite->y = SCREEN_CENTER_Y - (sprite->height >> 1);
        sprite->band_y = sprite->y;
        sprite->color.color = TEXT_COLOR;
        sprite->shadow_color.color = SHADOW_COLOR;
        sprite->band_color.color = BAND_COLOR;
        SET_SPRITE(task, sprite);
    }
    return task;
}

static void save_warning_free(struct task *task)
{
    struct text_sprite *sprite = SPRITE(task);

    if (sprite != NULL) {
        text_sprite_destroy(sprite);
        CLEAR_SPRITE(task);
    }
}

static void save_warning_destroy(struct task *task)
{
    save_warning_free(task);
}

/* make the sprite of the warning in the work's submode. the Japanese
 * format takes the drive's port letter before its socket, and every other
 * language's after it */
static void save_warning_build(struct task *task)
{
    struct task_work *work = task->work;
    struct text_sprite *sprite = SPRITE(task);
    char *format;
    char text[WARNING_TEXT_SIZE];
    int language;

    if (sprite != NULL) {
        save_warning_free(task);
    }
    format = card_text_get(work->submode);
    switch (work->submode) {
    case SAVE_NOT_READY:
        if (game_state.text_language == TEXT_JAPANESE) {
            sprintf(text, format,
                    port_letters[SAVE_DRIVE_PORT(game_state.save_drive)],
                    SAVE_DRIVE_SOCKET(game_state.save_drive));
        } else {
            sprintf(text, format, SAVE_DRIVE_SOCKET(game_state.save_drive),
                    port_letters[SAVE_DRIVE_PORT(game_state.save_drive)]);
        }
        break;
    case SAVE_NO_ROOM:
        sprintf(text, format, SAVE_BLOCKS);
        break;
    default:
        strcpy(text, format);
        break;
    }
    /* @hack spelling: language, a local for the text language, where
     * game_state.text_language passed in place would do */
    language = game_state.text_language;
    sprite = text_sprite_create(text, language);
    if (sprite != NULL) {
        sprite->x = SCREEN_CENTER_X - (sprite->width >> 1);
        sprite->y = SCREEN_CENTER_Y - (sprite->height >> 1);
        sprite->band_y = sprite->y;
        sprite->color.color = TEXT_RGB;
        sprite->shadow_color.color = SHADOW_RGB;
        sprite->band_color.color = BAND_RGB;
        SET_SPRITE(task, sprite);
    }
}

static void save_warning_step(struct task *task)
{
    struct task_work *work = task->work;
    struct text_sprite *sprite = SPRITE(task);
    Sint8 warning;

    /* @bug memory: reads through a null sprite where text_sprite_create
     * failed. save_warning_build tests the sprite (at 0x566 of the object's
     * code) and keeps the task without one. save_warning_draw tests it again
     * (at 0x7fa), but save_warning_step reads it untested (at 0x678 and
     * 0x734) */
#ifdef NONMATCHING
    /* this side repairs the tag above: a warning without a sprite ends at the
     * fade in or the fade out, the two modes that change the sprite */
    if (sprite == NULL &&
        (work->mode == MODE_SAVE_FADE_IN || work->mode == MODE_SAVE_FADE_OUT)) {
        work->mode = MODE_SAVE_ENDED;
        task_kill(task);
        return;
    }
#endif
    /* @hack spelling: warning set in the switch's expression, where a
     * statement of its own would do */
    switch ((warning = game_state.card_warning, work->mode)) {
    case MODE_SAVE_START:
        work->submode = warning;
        switch (warning) {
        case SAVE_NOT_READY:
        case SAVE_NO_ROOM:
        case SAVE_NOT_OURS:
            break;
        default:
            task_kill(task);
            break;
        }
        save_warning_build(task);
        work->mode = MODE_SAVE_FADE_IN;
        break;
    case MODE_SAVE_FADE_IN:
        if (work->timer++ >= FADE_IN_STEPS) {
            work->mode = MODE_SAVE_HOLD;
            work->timer = 0;
        } else {
            ALPHA_RAISE(sprite->color, TEXT_ALPHA);
            ALPHA_RAISE(sprite->shadow_color, SHADOW_ALPHA);
            ALPHA_RAISE(sprite->band_color, BAND_ALPHA);
        }
        break;
    case MODE_SAVE_HOLD:
        if (!warning) {
            work->mode = MODE_SAVE_FADE_OUT;
            work->timer = 0;
            break;
        } else if (pad_player_is_connected(0)) {
            if (work->submode != WARNING_NO_PAD && work->submode != 0 &&
                (player_pads[0]->press &
                 (pad_choose_buttons | pad_cancel_buttons))) {
                work->mode = MODE_SAVE_FADE_OUT;
                break;
            } else if (game_state.card_warning != work->submode) {
                work->mode = MODE_SAVE_CHANGED;
                break;
            }
        }
        break;
    case MODE_SAVE_CHANGED:
        save_warning_free(task);
        work->mode = MODE_SAVE_START;
        break;
    case MODE_SAVE_FADE_OUT:
        if (!(sprite->color.color & ALPHA_BITS)) {
            work->mode = MODE_SAVE_ENDED;
            task_kill(task);
            return;
        }
        ALPHA_LOWER(sprite->color);
        ALPHA_LOWER(sprite->shadow_color);
        ALPHA_LOWER(sprite->band_color);
        break;
    }
}

static void save_warning_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct text_sprite *sprite = SPRITE(task);

    switch (work->mode) {
    case MODE_SAVE_FADE_IN:
    case MODE_SAVE_HOLD:
    case MODE_SAVE_FADE_OUT:
        if (sprite != NULL) {
            text_sprite_draw_all(sprite);
        }
        break;
    }
}

struct task *save_warning_create(void)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, 0, save_warning_step,
                            "DisplayMCWarnMultiExecutor")) != NULL) {
        task->draw = save_warning_draw;
        task->destructor = save_warning_destroy;
    } else {
        return NULL;
    }
    card_text_start();
    return task;
}
