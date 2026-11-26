/* the lobby's two texts: the warning that the garden file was damaged, and
 * the exit's question */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include "chao/messages.h"
#include "chao/walk_screen.h"
#include "chaostglobby/lobby.h"
#include "draw/text_sprite.h"
#include "system/task.h"

/* the warning's state, laid over its task's work: text, frame and alpha lie
 * over the work's ang */
struct warning_work {
    Sint8 mode;
    Uint8 unused[7];
    struct text_sprite *text;
    int frame;
    int alpha;
};

/* a panel's top left corner on the screen, and its scale */
struct lobby_panel {
    Float x, y, sx, sy;
};

/* the question's state, in its task's extra, from syCalloc */
struct prompt_work {
    Sint8 mode, frame;
    Sint8 choice; /* the answer selected, an enum lobby_answer */
    struct text_sprite *message, *yes, *no;
    struct lobby_panel question, answers;
};

enum { WARNING_FADE_IN, WARNING_SHOWN, WARNING_FADE_OUT };
#define FADE_IN_STEPS 20
#define SHOWN_STEPS 240
#define FADE_OUT_STEPS 60
#define OPAQUE 255

/* where the warning's text and its band stand on the screen */
#define WARNING_X 80
#define WARNING_Y 96
#define BAND_X 0
#define BAND_Y 96

/* the bytes a formatted message may take, its NUL included */
#define TEXT_SIZE 256

enum { PROMPT_INIT, PROMPT_OPENING, PROMPT_OPEN };
/* a panel opens as sy, the scale of its height, rises from 0 to 1. it rises
 * by OPENING_RATE a step, over OPENING_STEPS steps */
#define OPENING_STEPS 11
#define OPENING_RATE 0.1F

/* each panel's top left corner on the screen, and its size in unscaled
 * units */
#define QUESTION_X 120.0F
#define QUESTION_Y 120.0F
#define QUESTION_WIDTH 400.0F
#define QUESTION_HEIGHT 98.0F
#define ANSWERS_X 120.0F
#define ANSWERS_Y 240.0F
#define ANSWERS_WIDTH 120.0F
#define ANSWERS_HEIGHT 104.0F

enum {
    BAD_FILE_FIRST = 27,
    BAD_FILE_SECOND = 28,
    EXIT_FIRST = 109,
    EXIT_SECOND = 110,
    EXIT_YES = 112,
    EXIT_NO = 113
};

static struct task *exit_prompt = NULL; /* the open question's task, or null */

static void warning_step(struct task *task)
{
    struct warning_work *work = (struct warning_work *)task->work;
    switch (work->mode) {
    case WARNING_FADE_IN:
        work->alpha = work->frame * OPAQUE / FADE_IN_STEPS;
        if (++work->frame == FADE_IN_STEPS) {
            work->frame = 0;
            work->mode = WARNING_SHOWN;
        }
        break;
    case WARNING_SHOWN:
        work->alpha = OPAQUE;
        if (++work->frame == SHOWN_STEPS) {
            work->frame = 0;
            work->mode = WARNING_FADE_OUT;
        }
        break;
    case WARNING_FADE_OUT:
        work->alpha = (FADE_OUT_STEPS - work->frame) * OPAQUE / FADE_OUT_STEPS;
        if (++work->frame == FADE_OUT_STEPS) {
            task_kill(task);
        }
        break;
    }
}

static void warning_draw(struct task *task)
{
    struct warning_work *work = (struct warning_work *)task->work;
    struct text_sprite *text = work->text;
    text->color.argb.a = work->alpha;
    text->shadow_color.argb.a = work->alpha;
    text->band_color.argb.a = work->alpha;
    text_sprite_draw(text);
}

static void warning_destroy(struct task *task)
{
    struct warning_work *work = (struct warning_work *)task->work;
    /* @hack spelling: text, a copy of work->text for its one use, where
     * work->text would do */
    struct text_sprite *text = work->text;
    text_sprite_destroy(text);
}

struct task *lobby_bad_file_message(void)
{
    struct task *task;
    struct warning_work *work;
    struct text_sprite *sprite;
    const char *first;
    const char *second;
    char text[TEXT_SIZE];

    /* @hack spelling: the assignments inside this function's two tests,
     * where statements of their own would do */
    if ((task = task_create(TASK_WORK, 2, warning_step,
                            "AloLobbyMessageTaskExec")) != NULL) {
        work = (struct warning_work *)task->work;
        first = garden_message(walk_message_text_id(BAD_FILE_FIRST));
        second = garden_message(walk_message_text_id(BAD_FILE_SECOND));
        sprintf(text, "%s\n%s", first, second);
        if ((sprite = garden_message_sprite(text, NULL, NULL)) != NULL) {
            sprite->color.color = 0xffffffff;
            sprite->shadow_color.color = 0xff440000;
            sprite->band_color.color = 0xffcc0000;
            sprite->x = WARNING_X;
            sprite->y = WARNING_Y;
            sprite->band_x = BAND_X;
            sprite->band_y = BAND_Y;
        }
        /* @bug memory: where garden_message_sprite fails, the warning's draw
         * and destroy go through the null sprite. the test of it (0x0c9c14e2)
         * guards only the sprite's fields. warning_draw writes through it
         * (0x0c9c1468), and warning_destroy hands it to text_sprite_destroy
         * (0x0c9c1480) */
        work->text = sprite;
        task->destructor = warning_destroy;
        task->draw = warning_draw;
    }
    return task;
}

static void prompt_step(struct task *task)
{
    struct prompt_work *work = task->extra.p;
    switch (work->mode) {
    case PROMPT_INIT:
        work->question.x = QUESTION_X;
        work->question.y = QUESTION_Y;
        work->question.sx = 1.0F;
        work->question.sy = 0.0F;
        work->answers.x = ANSWERS_X;
        work->answers.y = ANSWERS_Y;
        work->answers.sx = 1.0F;
        work->answers.sy = 0.0F;
        work->mode = PROMPT_OPENING;
        work->frame = 0;
        break;
    case PROMPT_OPENING:
        work->question.sy = work->frame * OPENING_RATE;
        work->answers.sy = work->frame * OPENING_RATE;
        if (++work->frame == OPENING_STEPS) {
            work->mode = PROMPT_OPEN;
            work->frame = 0;
        }
        break;
    }
}

#define FILL_COLOR 0xa00030c0
#define BORDER_COLOR 0xff00ffff
#define BORDER_WIDTH 8.0F
#define PANEL_DEPTH (1.0F / 1.3F)
#define CORNERS 4
/* where a panel's text starts, and how far below the first answer the second
 * is */
#define TEXT_INSET 24.0F
#define ANSWER_SPACING 32.0F
#define PANEL_X(panel, offset) ((panel).x + ((offset) * (panel).sx))
#define PANEL_Y(panel, offset) ((panel).y + ((offset) * (panel).sy))

/* draw a panel width by height in unscaled units: a fill inside a border
 * BORDER_WIDTH wide. each strip's corners run top left, bottom left, top
 * right, bottom right */
#define PANEL_DRAW(panel, width, height)                                       \
    do {                                                                       \
        NJS_POLYGON_VTX strip[CORNERS];                                        \
        strip[0].z = strip[1].z = strip[2].z = strip[3].z = PANEL_DEPTH;       \
        strip[0].x = strip[1].x = PANEL_X(panel, BORDER_WIDTH);                \
        strip[2].x = strip[3].x = PANEL_X(panel, (width) - BORDER_WIDTH);      \
        strip[0].y = strip[2].y = PANEL_Y(panel, BORDER_WIDTH);                \
        strip[1].y = strip[3].y = PANEL_Y(panel, (height) - BORDER_WIDTH);     \
        strip[0].col = strip[1].col = strip[2].col = strip[3].col =            \
            FILL_COLOR;                                                        \
        njDrawPolygon(strip, CORNERS, TRUE);                                   \
        strip[0].x = strip[1].x = PANEL_X(panel, 0.0F);                        \
        strip[2].x = strip[3].x = PANEL_X(panel, width);                       \
        strip[0].y = strip[2].y = PANEL_Y(panel, 0.0F);                        \
        strip[1].y = strip[3].y = PANEL_Y(panel, BORDER_WIDTH);                \
        strip[0].col = strip[1].col = strip[2].col = strip[3].col =            \
            BORDER_COLOR;                                                      \
        njDrawPolygon(strip, CORNERS, FALSE);                                  \
        strip[2].x = strip[3].x = PANEL_X(panel, BORDER_WIDTH);                \
        strip[0].y = strip[2].y = PANEL_Y(panel, BORDER_WIDTH);                \
        strip[1].y = strip[3].y = PANEL_Y(panel, (height) - BORDER_WIDTH);     \
        njDrawPolygon(strip, CORNERS, FALSE);                                  \
        strip[0].x = strip[1].x = PANEL_X(panel, (width) - BORDER_WIDTH);      \
        strip[2].x = strip[3].x = PANEL_X(panel, width);                       \
        njDrawPolygon(strip, CORNERS, FALSE);                                  \
        strip[0].x = strip[1].x = PANEL_X(panel, 0.0F);                        \
        strip[0].y = strip[2].y = PANEL_Y(panel, (height) - BORDER_WIDTH);     \
        strip[1].y = strip[3].y = PANEL_Y(panel, height);                      \
        njDrawPolygon(strip, CORNERS, FALSE);                                  \
    } while (0)

static void prompt_draw(struct task *task)
{
    struct prompt_work *work = task->extra.p;
    if (work->mode > PROMPT_INIT) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        {
            /* @hack spelling: x and y for the text's place, where the call's
             * own arguments would do */
            Float x;
            Float y;
            NJS_COLOR color;
            x = PANEL_X(work->question, TEXT_INSET);
            y = PANEL_Y(work->question, TEXT_INSET);
            color.color = 0xffffffff;
            /* @hack type: text_sprite_draw_text called through a cast to its
             * own type, where a plain call would do */
            ((void (*)(struct text_sprite *, Float, Float, Float, Float,
                       const Uint32 *))text_sprite_draw_text)(
                work->message, x, y, work->question.sx, work->question.sy,
                &color.color);
        }
        PANEL_DRAW(work->question, QUESTION_WIDTH, QUESTION_HEIGHT);
        {
            NJS_COLOR selected;
            NJS_COLOR unselected;
            Float left;
            Float top;
            selected.color = 0xffffd080;
            unselected.color = 0xff707070;
            left = PANEL_X(work->answers, TEXT_INSET);
            top = PANEL_Y(work->answers, TEXT_INSET);
            /* @hack spelling: !work->choice, where a test for
             * LOBBY_ANSWER_YES would do */
            text_sprite_draw_text(
                work->yes, left, top, work->answers.sx, work->answers.sy,
                !work->choice ? &selected.color : &unselected.color);
            top = PANEL_Y(work->answers, TEXT_INSET + ANSWER_SPACING);
            text_sprite_draw_text(
                work->no, left, top, work->answers.sx, work->answers.sy,
                work->choice == LOBBY_ANSWER_NO ? &selected.color
                                                : &unselected.color);
        }
        PANEL_DRAW(work->answers, ANSWERS_WIDTH, ANSWERS_HEIGHT);
    }
}

static void prompt_destroy(struct task *task)
{
    struct prompt_work *work = task->extra.p;
    text_sprite_destroy(work->message);
    text_sprite_destroy(work->yes);
    text_sprite_destroy(work->no);
    syFree(work);
    task->extra.p = NULL;
    task_kill(task);
}

struct task *lobby_exit_prompt(void)
{
    struct prompt_work *work;
    const char *first;
    const char *second;
    const char *answer;
    char text[TEXT_SIZE];

    /* @hack spelling: the tests of exit_prompt and of task_create's result
     * around the rest, where early returns would do */
    if (exit_prompt == NULL) {
        /* @hack spelling: the assignments inside this function's five tests,
         * where statements of their own would do */
        if ((exit_prompt = task_create(
                 0, 2, prompt_step, "AloLobbyExitConfirmTaskExec")) != NULL) {
            if ((work = syCalloc(1, sizeof(*work))) != NULL) {
                first = garden_message(walk_message_text_id(EXIT_FIRST));
                second = garden_message(walk_message_text_id(EXIT_SECOND));
                /* @hack spelling: the format as an array of its own, where
                 * the literal lobby_bad_file_message passes would do */
                {
                    static const char lines[] = "%s\n%s";
                    sprintf(text, lines, first, second);
                }
                if ((work->message = garden_message_sprite(text, NULL, NULL)) !=
                    NULL) {
                    answer = garden_message(walk_message_text_id(EXIT_YES));
                    if ((work->yes = garden_message_sprite(answer, NULL,
                                                           NULL)) != NULL) {
                        answer = garden_message(walk_message_text_id(EXIT_NO));
                        if ((work->no = garden_message_sprite(answer, NULL,
                                                              NULL)) != NULL) {
                            exit_prompt->extra.p = work;
                            exit_prompt->draw = prompt_draw;
                            exit_prompt->destructor = prompt_destroy;
                            return exit_prompt;
                        }
                        text_sprite_destroy(work->yes);
                    }
                    text_sprite_destroy(work->message);
                }
                syFree(work);
            }
            task_kill(exit_prompt);
            exit_prompt = NULL;
        }
    }
    return NULL;
}

void lobby_close_prompt(void)
{
    if (exit_prompt) {
        task_kill(exit_prompt);
        exit_prompt = NULL;
    }
}

void lobby_select_answer(enum lobby_answer choice)
{
    if (exit_prompt) {
        /* @hack spelling: the record reached through a cast of extra, where a
         * local, as prompt_step keeps, would do */
        ((struct prompt_work *)exit_prompt->extra.p)->choice = choice;
    }
}
