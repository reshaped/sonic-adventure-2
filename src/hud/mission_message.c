/* the mission message: the mission's title over its text from the MISSION
 * file. it shows in a framed window low on the screen, and the title card
 * starts it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <string.h>

#include "draw/text_sprite.h"
#include "hud/hud.h"
#include "hud/mission_message.h"
#include "hud/title_card.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"
#include "system/text.h"
#include "system/util.h"

/* the MISSION file, decoded into STAGE_SLOT. it opens with a row of MISSIONS
 * offsets for each stage from 0. each offset leads from the file's start to
 * a mission's text. an offset of 0 or an empty text is no text */
#define MISSION_FILE ((Sint32 *)STAGE_SLOT)
/* the rows that hold STAGE_CART_1's texts, by its driver */
#define CART_TEXTS_TAILS 19
#define CART_TEXTS_ROUGE 20
/* the minutes a MISSION_TIMED text shows for a stage time_limits lacks.
 * FORMAT_ROOM is the bytes the formatted text has past the format's length.
 * nothing checks that the text fits */
#define LIMIT_UNKNOWN_MINUTES 1
#define FORMAT_ROOM 3
/* the message's width and height where it has no sprite */
#define NO_SPRITE_SIZE 16

/* the message's modes, in its task's work. its timer counts every step from
 * the first until MESSAGE_END */
enum message_mode {
    MESSAGE_START = 0,
    MESSAGE_WAIT = 1, /* until the timer passes MESSAGE_WAITED */
    MESSAGE_SHOW_START = 2,
    MESSAGE_SHOW = 3, /* until the timer passes MESSAGE_SHOWN */
    MESSAGE_END = 4,
    MESSAGE_ENDED = 5 /* no step sets it. it ends the message as MESSAGE_END
                       * does */
};
#define MESSAGE_WAITED 20
#define MESSAGE_SHOWN 135

/* the message's layout, in pixels. the title's row and the text below it
 * start where the text centers on MESSAGE_MIDDLE. the shadow lies
 * SHADOW_OFFSET right of the text and below it. the window's frame lies
 * FRAME_MARGIN_X out from the text on each side. it lies FRAME_MARGIN_Y
 * above the row and below the text. a row wider than the text reaches past the
 * frame */
#define MESSAGE_MIDDLE 320.0F
#define TITLE_TOP 318.0F
#define TITLE_HEIGHT 14.0F
#define TEXT_TOP 338.0F
#define SHADOW_OFFSET 2.0F
#define FRAME_MARGIN_X 16.0F
#define FRAME_MARGIN_Y 10.0F
/* the title row's quads, by number: each mission's title, then the row's
 * two other pieces. the first piece lies ROW_GAP right of the title, and
 * the second right after it */
#define ROW_PIECE_1 MISSIONS
#define ROW_PIECE_2 (MISSIONS + 1)
#define ROW_QUADS (MISSIONS + 2)
#define QUAD_VERTICES 4
#define ROW_GAP 4.0F
#define ROW_PIECE_1_WIDTH 63.0F
#define ROW_PIECE_2_WIDTH 29.0F
#define ROW_QUAD(n) (&row_quads[(n) * QUAD_VERTICES])
/* the window's priority, behind the row's 1.0004 */
#define WINDOW_PRIORITY 1.0003F

/* the task levels of the message and of its window */
#define MESSAGE_LEVEL 0
#define WINDOW_LEVEL 2

/* the window's task, which the message's task holds in its unk_3c */
#ifdef NONMATCHING
/* this side holds it in extra, with a link back in the window's record. the
 * two tasks run at different levels, so either destructor can run first, and
 * each clears the other's link */
struct mission_frame {
    struct frame_window frame;
    struct task *message;
};
#define MISSION_FRAME_SIZE sizeof(struct mission_frame)
#define MISSION_WINDOW(owner) ((owner)->extra.task)

static void attach_window(struct task *message, struct task *window)
{
    message->extra.task = window;
    if (window != NULL) {
        struct mission_frame *record = window->extra.p;
        record->message = message;
    }
}
#define ATTACH_WINDOW(message, window) attach_window(message, window)
#else
#define MISSION_FRAME_SIZE sizeof(struct frame_window)
#define MISSION_WINDOW(owner) ((owner)->unk_3c.task)
#define ATTACH_WINDOW(message, window) ((message)->unk_3c.task = (window))
#endif

/* the message's sprite, or null */
static struct text_sprite *message_sprite = NULL;
#include "hud/mission_message/text.inc"

/* the letter of each language's MISSION file, by enum text_language */
static const char *mission_letters[TEXT_LANGUAGES] = {
#include "hud/mission_message/mission_letters.inc"
};
/* the title row's quads, in mission_numall */
static NJS_TEXTURE_VTX row_quads[ROW_QUADS * QUAD_VERTICES] = {
#include "hud/mission_message/row_quads.inc"
};
/* the text's quad, and its shadow's behind it */
static NJS_TEXTURE_VTX text_quad[4] = {
#include "hud/mission_message/text_quad.inc"
};
static NJS_TEXTURE_VTX shadow_quad[4] = {
#include "hud/mission_message/shadow_quad.inc"
};
/* the width of each mission's title, in pixels */
/* @bug logic: missions 3 and 4 take mission 2's width of 40 pixels, where
 * their quads' right corners and texture coordinates make them 39 and 49
 * pixels wide. mission 4's title draws squeezed. the two widths lie at 0x384
 * and 0x388 of the object's data */
static Float title_widths[MISSIONS] = {30.0F, 44.0F, 40.0F, 40.0F, 40.0F};

/* the message's size in pixels, as text_sprite_measure gave it */
static Uint16 message_width;
static Uint16 message_height;

/* destroy the message's sprite, if there is one. nothing calls
 * mission_message_free, and no header declares it. it is external only because
 * SHC leaves out a static function that nothing calls */
void mission_message_free(void); /* NOLINT(misc-use-internal-linkage) */

#ifdef NONMATCHING
/* The MISSION files contain fifty rows of five offsets followed by -1. */
#define MISSION_TEXT_STAGES 50
/* The PowerVR accepts texture sides up to 1024. At 16 bits per texel a
 * pair of sides this size uses the full 2 MiB pixel workspace. */
#define MISSION_TEXTURE_MAX 1024

static const char *mission_text_entry(const Sint32 *file, Sint32 size, int row)
{
    const int count = MISSION_TEXT_STAGES * MISSIONS;
    const Sint32 header = (count + 1) * sizeof(*file);
    Sint32 start;
    Sint32 end;
    int next;

    if (size < header || file[count] != -1) {
        return NULL;
    }
    start = file[row];
    if (start < header || start >= size) {
        return NULL;
    }
    /* Other entries may share a caption. Stop at the next stored text. */
    end = size;
    for (next = 0; next < count; next++) {
        if (file[next] > start && file[next] < end) {
            end = file[next];
        }
    }
    for (next = start; next < end; next++) {
        if (((const char *)file)[next] == '\0') {
            return (const char *)file + start;
        }
    }
    return NULL;
}

/* Only the two decimal time fields are formats. Japanese captions use the
 * game's two-byte kanji font and pad single-digit seconds with a zero. */
static Bool mission_caption_valid(const char *text, int language, Bool timed)
{
    const Uint8 *scan = (const Uint8 *)text;
    int fields = 0;
    int lines = 1;
    int glyphs = 0;

    if (*scan == 0) {
        return FALSE;
    }
    while (*scan != 0) {
        int ch = *scan++;

        if (ch == '\n') {
            if (++lines > MISSION_TEXTURE_MAX / (GLYPH_SIZE + TEXT_PITCH_Y)) {
                return FALSE;
            }
            glyphs = 0;
            continue;
        }
        /* Even the narrowest glyph consumes a pixel. This also bounds each
         * line before text_sprite_measure returns its width as a Uint16. */
        if (ch < ' ' || ++glyphs > MISSION_TEXTURE_MAX) {
            return FALSE;
        }
        if (language == TEXT_JAPANESE && ch >= 0x80) {
            int trail = *scan++;

            if (ch < 0x81 || ch > 0x98 || trail < 0x40 || trail > 0xfc ||
                trail == 0x7f || (ch == 0x98 && trail > 0x72)) {
                return FALSE;
            }
        } else if (timed && ch == '%') {
            if (fields == 0 && strncmp((const char *)scan, "1d", 2) == 0) {
                scan += 2;
            } else if (fields == 1 &&
                       strncmp((const char *)scan, "2d", 2) == 0) {
                scan += 2;
            } else if (fields == 1 &&
                       strncmp((const char *)scan, "02d", 3) == 0) {
                scan += 3;
            } else {
                return FALSE;
            }
            fields++;
        }
    }
    return fields == 0 || fields == 2;
}
#endif

void mission_message_load(void)
{
    char name[16];
    int stage;
#ifdef NONMATCHING
    /* this side reads the file's size for the checks below. it measures the
     * text before it makes a sprite, by metrics, which sets only the pitches
     * text_sprite_measure reads */
    struct text_sprite metrics;
    Sint32 size;
    const char *text;
#else
    Sint32 *offset;
    char *text;
#endif
    char *shown;
#ifdef NONMATCHING
    /* the timed mission's limit, found by i as a struct stage_time, where the
     * matching side walks the limits as bytes */
    const struct stage_time *duration;
    int i;
#else
    Sint8 *limit;
#endif
    Sint8 minute;
    Sint8 second;
    char *file;

    stage = stage_current_id();
#ifdef NONMATCHING
    /* this side frees any earlier message first. it loads none for a negative
     * stage, a mission or a language past its table, or a language whose file
     * letter isn't a capital */
    mission_message_free();
    if (stage < 0 || (unsigned)game_state.mission >= MISSIONS ||
        (unsigned)game_state.text_language >= TEXT_LANGUAGES ||
        mission_letters[game_state.text_language][0] < 'A' ||
        mission_letters[game_state.text_language][0] > 'Z') {
        return;
    }
#endif
    if (stage > STAGE_STG_END && stage != STAGE_CART_1) {
        return;
    }
    if (game_state.two_players != 0) {
        return;
    }
    strcpy(name, "MISSION");
    strcat(name, mission_letters[game_state.text_language]);
    strcat(name, ".PRS");
#ifdef NONMATCHING
    /* this side reads and decodes within WORK_AREA, whose end bounds the file.
     * the matching side decodes into STAGE_SLOT, unbounded */
    file = (char *)WORK_AREA;
    size = loader_read_prs_buffer(name, WORK_AREA, WORK_AREA_END - WORK_AREA);
#else
    file = (char *)MISSION_FILE;
#endif
    /* @hack spelling: an else after the return, where the if alone would
     * do */
    if (
#ifdef NONMATCHING
        /* the checked read's result: -1 where it fails */
        size < 0
#else
        loader_read_prs(name, file) == -1
#endif
    ) {
        return;
    } else if (stage == STAGE_CART_1) {
        switch (stage_character(0)) {
        case PLAYER_TAILS_MECH:
        case PLAYER_TAILS:
            stage = CART_TEXTS_TAILS;
            break;
        case PLAYER_ROUGE:
            stage = CART_TEXTS_ROUGE;
            break;
        default:
            return;
        }
    }
#ifdef NONMATCHING
    /* this side shows no text that mission_text_entry finds outside the file or
     * unended, and none that mission_caption_valid refuses */
    text = mission_text_entry((const Sint32 *)file, size,
                              stage * MISSIONS + game_state.mission);
    if (text == NULL ||
        !mission_caption_valid(text, game_state.text_language,
                               game_state.mission == MISSION_TIMED)) {
        return;
    }
#else
    offset = &((Sint32 *)file)[(stage * MISSIONS) + game_state.mission];
    /* @hack redundant: the tests of offset and text for null, which a place
     * in STAGE_SLOT never is */
    if (offset == NULL || *offset == 0) {
        return;
    }
    text = file + *offset;
    if (text == NULL || *text == '\0') {
        return;
    }
#endif
    if (game_state.mission == MISSION_TIMED) {
#ifdef NONMATCHING
        /* the limit found as a struct stage_time, the plain form of the hacks
         * below. stage names the cart's driver here, as the switch above set
         * it */
        duration = NULL;
        if (stage_current_id() == STAGE_CART_1) {
            duration =
                &cart_time_limits[stage == CART_TEXTS_TAILS ? CART_LIMIT_TAILS
                                                            : CART_LIMIT_ROUGE];
        } else {
            for (i = 0; i < TIME_LIMIT_STAGES; i++) {
                if (time_limits.stages[i].stage == stage) {
                    duration = &time_limits.stages[i].limit;
                    break;
                }
            }
        }
        minute = duration != NULL ? duration->minute : LIMIT_UNKNOWN_MINUTES;
        second = duration != NULL ? duration->second : 0;
#else
        /* @hack spelling: a switch with a case of its own for STAGE_CART_1,
         * where an if would do */
        switch (stage_current_id()) {
        case STAGE_CART_1:
            /* @hack type: the limits walked as bytes, here and below, where
             * struct stage_time and struct time_limit pointers would do */
            limit = (Sint8 *)cart_time_limits;
            switch (stage_character(0)) {
            case PLAYER_TAILS_MECH:
            case PLAYER_TAILS:
                minute = *limit++;
                second = *limit;
                break;
            case PLAYER_ROUGE:
                /* @hack spelling: the step to Rouge's limit inside the read
                 * of its minute, and the step to its second a statement of
                 * its own, where the two reads of the case above would do */
                minute = *(limit += sizeof(struct stage_time));
                limit++;
                second = *limit;
                break;
            default:
                return;
            }
            break;
        default:
            minute = LIMIT_UNKNOWN_MINUTES;
            second = 0;
            for (limit = &time_limits.stages[0].stage; *limit != STAGE_NONE;
                 limit += sizeof(struct time_limit)) {
                if (stage == *limit) {
                    limit++;
                    minute = *limit++;
                    second = *limit;
                    break;
                }
            }
            break;
        }
#endif
        shown = syMalloc(strlen(text) + FORMAT_ROOM);
#ifdef NONMATCHING
        /* this side shows no text where syMalloc fails, which sprintf would
         * write through */
        if (shown == NULL) {
            return;
        }
#endif
        sprintf(shown, text, minute, second);
    } else {
#ifdef NONMATCHING
        /* text_sprite_create uses WORK_AREA for its texture pixels. */
        shown = syMalloc(strlen(text) + 1);
        if (shown == NULL) {
            return;
        }
        strcpy(shown, text);
#else
        shown = text;
#endif
    }
#ifdef NONMATCHING
    /* this side measures the text before it makes a sprite, and makes none with
     * a side past MISSION_TEXTURE_MAX. shown is a heap copy on this side, which
     * it always frees */
    metrics.pitch_x = TEXT_PITCH_X;
    metrics.pitch_y = TEXT_PITCH_Y;
    text_sprite_measure(&metrics, shown, game_state.text_language,
                        &message_width, &message_height);
    if (message_width <= MISSION_TEXTURE_MAX &&
        message_height <= MISSION_TEXTURE_MAX) {
        message_sprite = text_sprite_create(shown, game_state.text_language);
    }
    syFree(shown);
#else
    if (message_sprite == NULL) {
        message_sprite = text_sprite_create(shown, game_state.text_language);
    }
    message_width = NO_SPRITE_SIZE;
    message_height = NO_SPRITE_SIZE;
    if (message_sprite != NULL) {
        text_sprite_measure(message_sprite, shown, game_state.text_language,
                            &message_width, &message_height);
    }
    if (game_state.mission == MISSION_TIMED) {
        syFree(shown);
    }
#endif
}

void mission_message_free(void)
{
    if (message_sprite != NULL) {
        text_sprite_destroy(message_sprite);
        message_sprite = NULL;
    }
}

/* draw the title's row, then the text and its shadow below it. each starts
 * at the left edge that centers the text. work's scl.x holds the text's width.
 * the text's quads span the sprite's whole texture, which can be wider than the
 * text */
static void mission_message_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXTURE_VTX *quad;
    Int texture;
    Float x;
    Float y;
    Float right;

#ifdef NONMATCHING
    /* this side draws nothing without a sprite, whose texture the draw takes */
    if (message_sprite == NULL) {
        return;
    }
#endif
    if (game_state.unk_0b == 0) {
        return;
    }
    switch (work->mode) {
    case MESSAGE_START:
    case MESSAGE_WAIT:
    case MESSAGE_ENDED:
        return;
    }
    njSetTexture(&missiontex_texlist);
    x = MESSAGE_MIDDLE - (work->scl.x * 0.5F);
    y = TITLE_TOP;
    njSetTextureNum(MISSIONTEX_NUMALL);
    texture = njGetTextureNumG();
    quad = ROW_QUAD(game_state.mission);
    quad[0].x = quad[1].x = x;
    quad[2].x = quad[3].x = x += title_widths[game_state.mission];
    right = x;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = quad[0].y + TITLE_HEIGHT;
    njDrawTexture(quad, 4, texture, 1);
    quad = ROW_QUAD(ROW_PIECE_1);
    quad[0].x = quad[1].x = right += ROW_GAP;
    right = quad[2].x = quad[3].x = quad[0].x + ROW_PIECE_1_WIDTH;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = quad[0].y + TITLE_HEIGHT;
    njDrawTexture(quad, 4, texture, 1);
    quad = ROW_QUAD(ROW_PIECE_2);
    quad[0].x = quad[1].x = right;
    quad[2].x = quad[3].x = quad[0].x + ROW_PIECE_2_WIDTH;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = quad[0].y + TITLE_HEIGHT;
    njDrawTexture(quad, 4, texture, 1);
    njSetTexture(&message_sprite->texlist);
    x = MESSAGE_MIDDLE - (work->scl.x * 0.5F);
    y = TEXT_TOP;
    quad = text_quad;
    quad[0].x = quad[1].x = x;
    quad[2].x = quad[3].x = x + message_sprite->texture_width;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = y + message_sprite->texture_height;
    njDrawTexture(quad, 4, message_sprite->texture, 1);
    y = TEXT_TOP + SHADOW_OFFSET;
    x += SHADOW_OFFSET;
    quad = shadow_quad;
    quad[0].x = quad[1].x = x;
    x += message_sprite->texture_width;
    quad[2].x = quad[3].x = x;
    quad[0].y = quad[2].y = y;
    quad[1].y = quad[3].y = y + message_sprite->texture_height;
    njDrawTexture(quad, 4, message_sprite->texture, 1);
}

static void mission_message_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MESSAGE_START:
        work->mode = MESSAGE_WAIT;
        /* fall through */
    case MESSAGE_WAIT:
        if (work->timer++ > MESSAGE_WAITED) {
            work->mode = MESSAGE_SHOW_START;
        }
        break;
    case MESSAGE_SHOW_START:
        work->mode = MESSAGE_SHOW;
        /* fall through */
    case MESSAGE_SHOW:
        if (work->timer++ > MESSAGE_SHOWN) {
            work->mode = MESSAGE_END;
        }
        break;
    case MESSAGE_END:
    case MESSAGE_ENDED:
        if (MISSION_WINDOW(task) != NULL) {
            task_kill(MISSION_WINDOW(task));
        }
        task_kill(task);
        break;
    }
}

static void mission_message_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side kills the window with the message. it clears the window's link
     * back first, so the window's destructor reaches no freed message */
    struct task *window = MISSION_WINDOW(task);
    if (window != NULL) {
        struct mission_frame *record = window->extra.p;
        record->message = NULL;
        task->extra.task = NULL;
        task_kill(window);
    }
#endif
    if (message_sprite != NULL) {
        text_sprite_destroy(message_sprite);
        message_sprite = NULL;
    }
}

static void mission_window_step(struct task *task);
static void mission_window_draw(struct task *task);
static void mission_window_destroy(struct task *task);
static struct task *mission_window_open(Float right, Float left, Float bottom,
                                        Float top);

void mission_message_start(void)
{
    struct task *task;

    if (message_sprite == NULL) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, MESSAGE_LEVEL, mission_message_step,
                            "MissionMessageDisplayerExecutor")) == NULL) {
#ifdef NONMATCHING
        /* this side frees the sprite, which no message task holds */
        mission_message_free();
#endif
        return;
    }
    task->draw = mission_message_draw;
    task->destructor = mission_message_destroy;
#ifdef NONMATCHING
    /* this side takes the size of the sprite mission_message_load made, where
     * the matching side loads the file again to measure the text */
    message_width = message_sprite->width;
    message_height = message_sprite->height;
#else
    mission_message_load();
#endif
    ATTACH_WINDOW(
        task, mission_window_open(
                  (message_width * 0.5F) + (MESSAGE_MIDDLE + FRAME_MARGIN_X),
                  (MESSAGE_MIDDLE - FRAME_MARGIN_X) - (message_width * 0.5F),
                  message_height + (TEXT_TOP + FRAME_MARGIN_Y),
                  TITLE_TOP - FRAME_MARGIN_Y));
    task->work->scl.x = message_width;
    task->work->scl.y = message_height;
}

/* the same as hud/hud.c's score_window_step */
static void mission_window_step(struct task *task)
{
    struct frame_window *window = task->extra.p;
    Bool done;
    int i;
    Float share;

    switch (window->mode) {
    case WINDOW_WIDEN:
        done = TRUE;
        for (i = 0; i < 4; i++) {
            window->corners[i].x = float_approach(
                window->corners[i].x, window->targets[i].x, WINDOW_WIDEN_STEP);
            if (window->corners[i].x != window->targets[i].x) {
                done = FALSE;
            }
        }
        if (done == TRUE) {
            window->mode = WINDOW_HEIGHTEN;
        }
        break;
    case WINDOW_HEIGHTEN:
        done = TRUE;
        for (i = 0; i < 4; i++) {
            window->corners[i].y =
                float_approach(window->corners[i].y, window->targets[i].y,
                               WINDOW_HEIGHTEN_STEP);
            if (window->corners[i].y != window->targets[i].y) {
                done = FALSE;
            }
        }
        if (done == TRUE) {
            window->mode = WINDOW_OPENED;
        }
        break;
    case WINDOW_OPEN:
        share = 1.0F / ((Float)WINDOW_OPEN_STEPS - window->count);
        for (i = 0; i < 4; i++) {
            window->corners[i].x +=
                (window->targets[i].x - window->corners[i].x) * share;
            window->corners[i].y +=
                (window->targets[i].y - window->corners[i].y) * share;
        }
        if (window->count >= WINDOW_OPEN_STEPS - 1) {
            window->mode = WINDOW_OPENED;
        } else {
            window->count++;
        }
        break;
    case WINDOW_OPENED:
        break;
    }
}

/* draw the window as hud/hud.c's score_window_draw does. its corners draw
 * from missiontex_texlist's mission_w0, its edges from mission_w1 and its
 * middle from mission_bg */
static void mission_window_draw(struct task *task)
{
    struct frame_window *window = task->extra.p;
    NJS_TEXTURE_VTX quad[4];
    Int corner;
    Int edge;
    Int middle;

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&missiontex_texlist);
    quad[0].z = quad[1].z = quad[2].z = quad[3].z = WINDOW_PRIORITY;
    quad[0].col = quad[1].col = quad[2].col = quad[3].col = 0xffffffff;
    njSetTextureNum(MISSIONTEX_W0);
    corner = njGetTextureNumG();
    njSetTextureNum(MISSIONTEX_W1);
    edge = njGetTextureNumG();
    njSetTextureNum(MISSIONTEX_BG);
    middle = njGetTextureNumG();

    quad[0].x = window->corners[0].x;
    quad[0].y = window->corners[0].y;
    quad[1].x = window->corners[0].x;
    quad[1].y = window->corners[0].y + WINDOW_BORDER;
    quad[2].x = window->corners[0].x + WINDOW_BORDER;
    quad[2].y = window->corners[0].y;
    quad[3].x = window->corners[0].x + WINDOW_BORDER;
    quad[3].y = window->corners[0].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, corner, 1);

    quad[0].y = window->corners[0].y + WINDOW_BORDER;
    quad[1].y = window->corners[1].y - WINDOW_BORDER;
    quad[2].y = window->corners[0].y + WINDOW_BORDER;
    quad[3].y = window->corners[1].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, edge, 1);

    quad[0].y = window->corners[1].y;
    quad[2].y = window->corners[1].y;
    njDrawTexture(quad, 4, corner, 1);

    quad[0].x = window->corners[0].x + WINDOW_BORDER;
    quad[0].y = window->corners[0].y;
    quad[1].x = window->corners[0].x + WINDOW_BORDER;
    quad[1].y = window->corners[0].y + WINDOW_BORDER;
    quad[2].x = window->corners[2].x - WINDOW_BORDER;
    quad[2].y = window->corners[2].y;
    quad[3].x = window->corners[2].x - WINDOW_BORDER;
    quad[3].y = window->corners[2].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 1.0F;
    quad[1].v = 0.0F;
    quad[2].u = 0.0F;
    quad[2].v = 1.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, edge, 1);

    quad[0].y = window->corners[0].y + WINDOW_BORDER;
    quad[1].y = window->corners[1].y - WINDOW_BORDER;
    quad[2].y = window->corners[2].y + WINDOW_BORDER;
    quad[3].y = window->corners[3].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, middle, 1);

    quad[0].y = window->corners[1].y;
    quad[2].y = window->corners[3].y;
    njDrawTexture(quad, 4, edge, 1);

    quad[0].x = window->corners[2].x;
    quad[0].y = window->corners[2].y;
    quad[1].x = window->corners[2].x;
    quad[1].y = window->corners[2].y + WINDOW_BORDER;
    quad[2].x = window->corners[2].x - WINDOW_BORDER;
    quad[2].y = window->corners[2].y;
    quad[3].x = window->corners[2].x - WINDOW_BORDER;
    quad[3].y = window->corners[2].y + WINDOW_BORDER;
    quad[0].u = 0.0F;
    quad[0].v = 0.0F;
    quad[1].u = 0.0F;
    quad[1].v = 1.0F;
    quad[2].u = 1.0F;
    quad[2].v = 0.0F;
    quad[3].u = 1.0F;
    quad[3].v = 1.0F;
    njDrawTexture(quad, 4, corner, 1);

    quad[0].y = window->corners[2].y + WINDOW_BORDER;
    quad[1].y = window->corners[3].y - WINDOW_BORDER;
    quad[2].y = window->corners[2].y + WINDOW_BORDER;
    quad[3].y = window->corners[3].y - WINDOW_BORDER;
    njDrawTexture(quad, 4, edge, 1);

    quad[0].y = window->corners[3].y;
    quad[2].y = window->corners[3].y;
    njDrawTexture(quad, 4, corner, 1);
    njFogEnable();
}

static void mission_window_destroy(struct task *task)
{
#ifdef NONMATCHING
    /* this side clears the message's link to this window, so the message's step
     * and destructor reach no freed window */
    struct mission_frame *record = task->extra.p;
    if (record->message != NULL) {
        record->message->extra.p = NULL;
        record->message = NULL;
    }
#endif
}

/* start a window that opens from its middle to the rectangle from left to
 * right and top to bottom. it returns the window's task, or null where
 * task_create or syCalloc fails */
static struct task *mission_window_open(Float right, Float left, Float bottom,
                                        Float top)
{
    struct task *task;
    struct frame_window *window;

    /* @hack spelling: each assignment inside its test, where a statement of
     * its own would do */
    if ((task = task_create(0, WINDOW_LEVEL, mission_window_step,
                            "MissionWindow")) == NULL) {
        return NULL;
    }
    if ((window = syCalloc(1, MISSION_FRAME_SIZE)) == NULL) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = window;
    window->mode = WINDOW_OPEN;
    window->targets[0].x = left;
    window->targets[0].y = top;
    window->targets[1].x = left;
    window->targets[1].y = bottom;
    window->targets[2].x = right;
    window->targets[2].y = top;
    window->targets[3].x = right;
    window->targets[3].y = bottom;
    window->corners[0].x = window->corners[1].x =
        ((right + left) * 0.5F) - WINDOW_BORDER;
    window->corners[2].x = window->corners[3].x =
        ((right + left) * 0.5F) + WINDOW_BORDER;
    window->corners[0].y = window->corners[2].y =
        ((bottom + top) * 0.5F) - WINDOW_BORDER;
    window->corners[1].y = window->corners[3].y =
        ((bottom + top) * 0.5F) + WINDOW_BORDER;
    task->draw = mission_window_draw;
    task->destructor = mission_window_destroy;
    return task;
}
