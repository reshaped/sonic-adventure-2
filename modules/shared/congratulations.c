/* the congratulations: the messages for an award, which the emblem screen
 * shows after the emblem */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/viewport.h"
#include "draw/fade.h"
#include "draw/screen.h"
#include "hud/message_window.h"
#include "save/save.h"
#include "shared/congratulations.h"
#include "shared/emblem.h"
#include "shared/message_sprite.h"
#include "shared/screen_sprite.h"
#include "sound/voice_set.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"

#define LEVEL 2
#define CONGRATULATIONS_TEXTURES 2

/* each message shows for MESSAGE_STEPS steps. its text's top lies at screen
 * y MESSAGE_Y, and its band isn't drawn */
#define MESSAGE_STEPS 300
#define MESSAGE_Y 240
/* the voice set each message of an award with a banner plays */
#define BANNER_VOICES 15

/* the rainbow scrolls right SCROLL_STEP pixels a step, and wraps every
 * RAINBOW_WRAP pixels. it draws as two copies of its cell, which is
 * RAINBOW_WIDTH pixels wide. the second copy's left edge lies at x
 * RAINBOW_SEAM plus the wrapped scroll. the first copy ends there. the
 * two cover rainbow_area at every scroll */
#define SCROLL_STEP 3
#define RAINBOW_WRAP 512
#define RAINBOW_WIDTH 511
#define RAINBOW_SEAM 70

/* the indexes of congratulations_cells. no sprite takes cell 2. that cell
 * draws the title texture's bottom-right corner, u and v 240 to 255, as a
 * 31-pixel square */
enum { CELL_RAINBOW = 0, CELL_TITLE = 1, CELLS = 3 };

enum { MODE_START, MODE_MESSAGE, MODE_WAIT, MODE_FADE_OUT };

/* an award's messages, as the numbers message_sprite_text takes */
struct award_messages {
    int *messages;
    int count;
    Bool banner; /* the award shows the title over the rainbow */
};

struct congratulations_state {
    int scroll; /* how far the rainbow has scrolled, in pixels */
    int message; /* which of the award's messages shows */
    int timer; /* the steps the message has left */
};
static struct congratulations_state congratulations_state;

#ifdef NONMATCHING
/* The message sequence also runs without the optional title and rainbow art. */
static Bool banner_loaded;
#endif

#include "shared/congratulations/text.inc"

static NJS_TEXNAME congratulations_texnames[CONGRATULATIONS_TEXTURES] = {
#include "shared/congratulations/congratulations_texnames.inc"
};
/* Keep the capacity of the writable slots when the archive is absent. */
#ifdef NONMATCHING
/* this side's list names its slots and their count from the start. a missing
 * archive then leaves them. the disc-free build's list is zeros */
static NJS_TEXLIST congratulations_texlist = {congratulations_texnames,
                                              CONGRATULATIONS_TEXTURES};
#else
static NJS_TEXLIST congratulations_texlist = {
#include "shared/congratulations/congratulations_texlist.inc"
};
#endif
static NJS_TEXANIM congratulations_cells[CELLS] = {
#include "shared/congratulations/congratulations_cells.inc"
};
/* the band the rainbow scrolls in */
static struct viewport_area rainbow_area = {
    {{100.0F, 120.0F}, {524.0F, 168.0F}},
    {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
    {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
};
static int award_message_numbers[] = {
    155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
};
/* by award number. 0 is no award */
static struct award_messages award_messages[] = {
    {NULL, 0, FALSE},
    {&award_message_numbers[0], 2, FALSE},
    {&award_message_numbers[2], 2, FALSE},
    {&award_message_numbers[4], 1, FALSE},
    {&award_message_numbers[5], 1, FALSE},
    {&award_message_numbers[6], 1, FALSE},
    {&award_message_numbers[7], 1, TRUE},
    {&award_message_numbers[8], 1, TRUE},
    {&award_message_numbers[9], 1, TRUE},
    {&award_message_numbers[10], 1, TRUE},
    {&award_message_numbers[11], 1, TRUE},
    {&award_message_numbers[12], 1, TRUE},
    {&award_message_numbers[13], 2, TRUE},
    {&award_message_numbers[15], 2, TRUE},
    {&award_message_numbers[17], 2, TRUE},
    {&award_message_numbers[19], 2, TRUE},
    {&award_message_numbers[21], 2, TRUE},
    {&award_message_numbers[23], 2, TRUE},
    {&award_message_numbers[25], 1, TRUE},
};
static struct screen_sprite title_sprite = {
#include "shared/congratulations/title_sprite.inc"
};
static struct screen_sprite rainbow_sprite = {
#include "shared/congratulations/rainbow_sprite.inc"
};

static void congratulations_step(struct task *task);
static void congratulations_draw(struct task *task);
static void congratulations_destroy(struct task *task);

/* clip the draws that follow to rainbow_area, or to the whole screen again */
static void congratulations_clip(Bool on)
{
    if (on) {
        viewport_set_area(&rainbow_area);
        viewport_set_screen(VIEWPORT_AREA);
    } else {
        viewport_set_whole();
    }
}

void congratulations_start(void)
{
    int i;
    struct task *task;

    for (i = 0; i < CELLS; i++) {
        /* @hack spelling: a switch of one case, where if (i == CELL_TITLE)
         * would do */
        switch (i) {
        case CELL_TITLE:
            congratulations_cells[i].cx = congratulations_cells[i].sx / 2;
            break;
        }
    }
#ifdef NONMATCHING
    /* this side loads the banner only where its title cell has a size. the
     * disc-free build's zeros give none. it records whether the banner
     * loaded */
    banner_loaded =
        congratulations_cells[CELL_TITLE].sx != 0 &&
        texture_load_pvm(CONGRATULATIONS_ARCHIVE, &congratulations_texlist);
#else
    texture_load_pvm(CONGRATULATIONS_ARCHIVE, &congratulations_texlist);
#endif
    task = task_create(TASK_WORK, LEVEL, congratulations_step, "execCong");
#ifdef NONMATCHING
    /* this side releases the banner and returns where task_create fails. the
     * matching side writes through the null */
    if (task == NULL) {
        congratulations_destroy(NULL);
        return;
    }
#endif
    task->work->mode = MODE_START;
    congratulations_state.message = 0;
    congratulations_state.scroll = 0;
}

static void congratulations_step(struct task *task)
{
    struct task_work *work = task->work;
    char *text;
    struct message_window *window;

    switch (work->mode) {
    case MODE_START:
        task->draw = congratulations_draw;
        task->destructor = congratulations_destroy;
        /* fall through */
    case MODE_MESSAGE:
        text =
            message_sprite_text(award_messages[game_state.unlock_id]
                                    .messages[congratulations_state.message]);
        window = message_window_record(message_window_open_unowned(
            MESSAGE_WINDOW_LOW, text, MESSAGE_STEPS, game_state.text_language));
        window->sprite.band_color.color = 0;
        window->sprite.y = MESSAGE_Y;
        congratulations_state.timer = MESSAGE_STEPS;
        work->mode = MODE_WAIT;
        if (award_messages[game_state.unlock_id].banner) {
            voice_set_play(BANNER_VOICES);
        }
        break;
    case MODE_WAIT:
        fade_in_step();
        congratulations_state.timer--;
        if (congratulations_state.timer <= 0) {
            congratulations_state.message++;
            if (congratulations_state.message <
                award_messages[game_state.unlock_id].count) {
                work->mode = MODE_MESSAGE;
            } else {
                work->mode = MODE_FADE_OUT;
            }
        }
        break;
    case MODE_FADE_OUT:
        if (fade_out_step()) {
            task_destroy(task);
        }
        break;
    }
    congratulations_state.scroll += SCROLL_STEP;
}

static void congratulations_draw(struct task *task)
{
    struct screen_sprite *sprite;

#ifdef NONMATCHING
    /* this side draws no banner that didn't load */
    if (!banner_loaded) {
        return;
    }
#endif
    if (game_state.unlock_id == AWARD_ALL_EMBLEMS &&
        stage_current_id() == STAGE_CHAO) {
        return;
    }
    if (award_messages[game_state.unlock_id].banner) {
        screen_sprite_draw(&title_sprite);
        congratulations_clip(TRUE);
        sprite = &rainbow_sprite;
        sprite->pos.x = (congratulations_state.scroll % RAINBOW_WRAP) -
                        (RAINBOW_WIDTH - RAINBOW_SEAM);
        screen_sprite_draw(sprite);
        sprite->pos.x =
            (congratulations_state.scroll % RAINBOW_WRAP) + RAINBOW_SEAM;
        screen_sprite_draw(sprite);
        congratulations_clip(FALSE);
    }
}

static void congratulations_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side releases only a banner that loaded */
    if (banner_loaded) {
        njReleaseTexture(&congratulations_texlist);
        banner_loaded = FALSE;
    }
#else
    njReleaseTexture(&congratulations_texlist);
#endif
    emblem_screen_open = 0;
}
