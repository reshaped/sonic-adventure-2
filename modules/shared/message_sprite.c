/* message sprites: a message or a text drawn once, by a text sprite of its
 * own */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/text_sprite.h"
#include "shared/message_sprite.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"

/* the steps a sprite's task lives, which its work's mode counts down */
#define KEEP_STEPS 2
#define STEPS_LEFT(work) ((work)->mode)
#define TASK_LEVEL 2
/* the screen's middle x, on which a centered text draws */
#define SCREEN_MIDDLE 320

/* the band color message_sprite_draw_set_band gives a sprite, a translucent
 * blue as ARGB */
static Uint32 set_band_color = 0x800000ff;

char *message_sprite_text(int message)
{
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        return message_tables.japanese[message];
    case TEXT_ENGLISH:
        return message_tables.english[message];
    case TEXT_FRENCH:
        return message_tables.french[message];
    case TEXT_SPANISH:
        return message_tables.spanish[message];
    case TEXT_GERMAN:
        return message_tables.german[message];
    }
    return NULL;
}

struct text_sprite *message_sprite_create(int message)
{
    struct text_sprite *sprite;

#ifdef NONMATCHING
    /* GCC can't see that every path sets sprite. the language is one of the
     * switch's cases, as message_sprite.h requires */
    sprite = NULL;
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        sprite = text_sprite_create(message_tables.japanese[message],
                                    game_state.text_language);
        break;
    case TEXT_ENGLISH:
        sprite = text_sprite_create(message_tables.english[message],
                                    game_state.text_language);
        break;
    case TEXT_FRENCH:
        sprite = text_sprite_create(message_tables.french[message],
                                    game_state.text_language);
        break;
    case TEXT_SPANISH:
        sprite = text_sprite_create(message_tables.spanish[message],
                                    game_state.text_language);
        break;
    case TEXT_GERMAN:
        sprite = text_sprite_create(message_tables.german[message],
                                    game_state.text_language);
        break;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    return sprite;
}

static void message_sprite_free(struct task *task)
{
    if (task->motion.p != NULL) {
        text_sprite_destroy(task->motion.p);
        task->motion.p = NULL;
    }
}

static void message_sprite_keep(struct task *task)
{
    STEPS_LEFT(task->work)--;
    if (STEPS_LEFT(task->work) <= 0) {
        task_destroy(task);
    }
}

/* @hack spelling: the draw through draw_text, where text_sprite_draw_text
 * would do */
#pragma inline(draw_text)
static void draw_text(struct text_sprite *sprite, const NJS_POINT3 *pos,
                      Float scale, const Uint32 *color)
{
    text_sprite_draw_text(sprite, pos->x, pos->y, scale, scale, color);
}

void message_sprite_draw_set_band(int message, const NJS_POINT3 *pos,
                                  Float scale)
{
    struct text_sprite *sprite;
    struct task *task;
    NJS_POINT3 centered;
    char first;

#ifdef NONMATCHING
    /* GCC can't see that every path sets sprite and first. the language is
     * one of the switch's cases, as message_sprite.h requires */
    sprite = NULL;
    first = '\0';
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        sprite = text_sprite_create(message_tables.japanese[message],
                                    game_state.text_language);
        first = message_tables.japanese[message][0];
        break;
    case TEXT_ENGLISH:
        sprite = text_sprite_create(message_tables.english[message],
                                    game_state.text_language);
        first = message_tables.english[message][0];
        break;
    case TEXT_FRENCH:
        sprite = text_sprite_create(message_tables.french[message],
                                    game_state.text_language);
        first = message_tables.french[message][0];
        break;
    case TEXT_SPANISH:
        sprite = text_sprite_create(message_tables.spanish[message],
                                    game_state.text_language);
        first = message_tables.spanish[message][0];
        break;
    case TEXT_GERMAN:
        sprite = text_sprite_create(message_tables.german[message],
                                    game_state.text_language);
        first = message_tables.german[message][0];
        break;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    sprite->band_color.color = set_band_color;
    if (first == '\a' || first == '\t') {
        centered = *pos;
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        centered.x = SCREEN_MIDDLE - (sprite->width / 2);
        draw_text(sprite, &centered, scale, NULL);
    } else {
        draw_text(sprite, pos, scale, NULL);
    }
    if ((task = task_create(TASK_WORK, TASK_LEVEL, message_sprite_keep,
                            "keepMessage")) != NULL) {
        task->motion.p = sprite;
        STEPS_LEFT(task->work) = KEEP_STEPS;
        task->destructor = message_sprite_free;
    }
}

static void message_sprite_draw_color(int message, const NJS_POINT3 *pos,
                                      Float scale, const Uint32 *color)
{
    struct text_sprite *sprite;
    struct task *task;
    NJS_POINT3 centered;
    char first;

#ifdef NONMATCHING
    /* GCC can't see that every path sets sprite and first, as in
     * message_sprite_draw_set_band */
    sprite = NULL;
    first = '\0';
#endif
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        sprite = text_sprite_create(message_tables.japanese[message],
                                    game_state.text_language);
        first = message_tables.japanese[message][0];
        break;
    case TEXT_ENGLISH:
        sprite = text_sprite_create(message_tables.english[message],
                                    game_state.text_language);
        first = message_tables.english[message][0];
        break;
    case TEXT_FRENCH:
        sprite = text_sprite_create(message_tables.french[message],
                                    game_state.text_language);
        first = message_tables.french[message][0];
        break;
    case TEXT_SPANISH:
        sprite = text_sprite_create(message_tables.spanish[message],
                                    game_state.text_language);
        first = message_tables.spanish[message][0];
        break;
    case TEXT_GERMAN:
        sprite = text_sprite_create(message_tables.german[message],
                                    game_state.text_language);
        first = message_tables.german[message][0];
        break;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    if (first == '\a' || first == '\t') {
        centered = *pos;
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        centered.x = SCREEN_MIDDLE - (sprite->width / 2);
        draw_text(sprite, &centered, scale, color);
    } else {
        draw_text(sprite, pos, scale, color);
    }
    if ((task = task_create(TASK_WORK, TASK_LEVEL, message_sprite_keep,
                            "keepMessage")) != NULL) {
        task->motion.p = sprite;
        STEPS_LEFT(task->work) = KEEP_STEPS;
        task->destructor = message_sprite_free;
    }
}

void message_sprite_draw(int message, const NJS_POINT3 *pos, Float scale)
{
    message_sprite_draw_color(message, pos, scale, NULL);
}

void message_sprite_draw_text_color(const char *text, const NJS_POINT3 *pos,
                                    Float scale, const Uint32 *color)
{
    struct text_sprite *sprite;
    struct task *task;
    NJS_POINT3 centered;
    char first;

    sprite = text_sprite_create(text, game_state.text_language);
    first = text[0];
    if (first == '\a' || first == '\t') {
        centered = *pos;
        /* NOLINTNEXTLINE(bugprone-integer-division) */
        centered.x = SCREEN_MIDDLE - (sprite->width / 2);
        draw_text(sprite, &centered, scale, color);
    } else {
        draw_text(sprite, pos, scale, color);
    }
    if ((task = task_create(TASK_WORK, TASK_LEVEL, message_sprite_keep,
                            "keepMessage")) != NULL) {
        task->motion.p = sprite;
        STEPS_LEFT(task->work) = KEEP_STEPS;
        task->destructor = message_sprite_free;
    }
}

void message_sprite_draw_text(const char *text, const NJS_POINT3 *pos,
                              Float scale)
{
    message_sprite_draw_text_color(text, pos, scale, NULL);
}

int message_sprite_count(int message, char character)
{
    const char *text;
    int count;

    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        text = message_tables.japanese[message];
        break;
    case TEXT_ENGLISH:
        text = message_tables.english[message];
        break;
    case TEXT_FRENCH:
        text = message_tables.french[message];
        break;
    case TEXT_SPANISH:
        text = message_tables.spanish[message];
        break;
    case TEXT_GERMAN:
        text = message_tables.german[message];
        break;
    default:
        return 0;
    }
    /* @hack redundant: a jump to the label that follows it */
    goto count;
count:
    count = 0;
    while (*text != '\0') {
        if (*text == character) {
            count++;
        }
        text++;
    }
    return count;
}
