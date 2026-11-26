#include <ninja.h>
#include <sg_xpt.h>

#include "draw/text_sprite.h"
#include "event/messages.h"
#include "event/script.h"
#include "event/typing.h"
#include "event/scene.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/text.h"

#define SPRITE_SLOTS 8
#define SPRITE_FRAMES 3
#define TYPING_SOUND 0x801e
#define TYPING_BYTES 256
#define LINE_SCALE 1.2F
/* the steps WAIT_CHARACTER counts down. a character also takes the step that
 * types it and the one that ends the wait. it takes CHARACTER_DELAY + 2 steps
 * in all */
#define CHARACTER_DELAY 6
#define FINISH_DELAY 5
/* a byte of the line that types as a pause. it takes a character's time and
 * shows nothing */
#define PAUSE '\a'

/* draw_text makes a new text sprite at each call. a slot keeps it until
 * expire_sprites has counted frames down to 0. a slot whose frames is 0 is
 * free */
struct sprite_slot {
    int frames;
    struct text_sprite *sprite;
};

enum phase { START_LINE, TYPE_CHARACTER, WAIT_CHARACTER, FINISH_LINE };

/* Shift JIS's full-width space, which types without the key sound */
static const char *wide_space = "\x81\x40";
struct typing_position {
    NJS_POINT2 screen; /* the line's top left corner, in pixels */
    Float depth;
};
static struct typing_position position = {
#include "event/typing/position.inc"
};
static struct sprite_slot slots[SPRITE_SLOTS];
static struct {
    Sint32 frame;
    enum phase phase;
    const char *text;
} line;
static Sint32 line_x;
static Sint32 wait;
static enum phase after_wait;
static Sint32 characters;
static char visible[TYPING_BYTES];

static void expire_sprites(void);
static void draw_text(const char *text);

Bool event_typing_start(void)
{
    int i;
    char *byte;

    /* @hack type: the slots cleared as bytes, counting down, where memset
     * would do */
    byte = (char *)slots;
    for (i = sizeof(slots); i != 0; i--) {
        *byte++ = 0;
    }
    if (event_messages != NULL) {
        for (i = 0; i < event_messages->count; i++) {
            if (event_tracks->lines[i].frame == EVENT_TYPED_FRAME) {
                line.text = event_messages->messages[i].text;
                line.phase = START_LINE;
                line.frame = 0;
                line_x = event_tracks->lines[i].duration;
                return TRUE;
            }
        }
    }
    line.text = NULL;
    line.frame = -1;
    line.phase = START_LINE;
    return FALSE;
}

/* text's size, its terminator included */
static int text_bytes(const char *text)
{
    int length = 0;
    while (text[length++] != '\0') {
    }
    return length;
}

Bool event_typing_step(void)
{
    Sint32 read;
    Sint32 written;
    int shown;
    Bool play_key;

    line.frame++;
    switch (line.phase) {
    case START_LINE:
        wait = 0;
        characters = 0;
        after_wait = TYPE_CHARACTER;
        line.phase = TYPE_CHARACTER;
        /* fall through */
    case TYPE_CHARACTER:
        characters++;
        written = 0;
        read = 0;
        play_key = FALSE;
        for (shown = 0; shown < characters; shown++) {
            /* @bug memory: nothing bounds the copy, so a line that types
             * TYPING_BYTES bytes or more writes past visible. the first write
             * past it is a character's second byte at 0x8c509698, or the
             * terminator at 0x8c5096d6 */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above. it stops at the end, and
             * before a character that leaves no room for the terminator. a
             * character's first byte right before the terminator ends the line
             * too */
            if (line.text[read] == '\0' ||
                (line.text[read] == '\n' && written == TYPING_BYTES - 1) ||
                (line.text[read] != '\n' && line.text[read] != PAUSE &&
                 (written >= TYPING_BYTES - 2 ||
                  line.text[read + 1] == '\0'))) {
#else
            if (line.text[read] == '\0') {
#endif
                /* @hack redundant: the stores of wait and the phase, which the
                 * stores after the loop replace */
                wait = 2;
                line.phase = FINISH_LINE;
                play_key = FALSE;
                after_wait = FINISH_LINE;
                break;
            }
            if (read >= text_bytes(line.text)) {
                /* @hack redundant: the stores of wait and the phase, which the
                 * stores after the loop replace */
                wait = 2;
                line.phase = FINISH_LINE;
                after_wait = FINISH_LINE;
            }
            if (line.text[read] == PAUSE) {
                play_key = FALSE;
                read++;
            } else if (line.text[read] == '\n') {
                visible[written] = line.text[read];
                written++;
                read++;
                play_key = FALSE;
            } else {
                visible[written] = line.text[read];
                visible[written + 1] = line.text[read + 1];
                if (line.text[read] != wide_space[0] ||
                    line.text[read + 1] != wide_space[1]) {
                    play_key = TRUE;
                } else {
                    play_key = FALSE;
                }
                written += 2;
                read += 2;
            }
        }
        visible[written] = '\0';
        if (play_key) {
            sound_play(TYPING_SOUND, NULL, 0, 0);
        }
        draw_text(visible);
        expire_sprites();
        wait = CHARACTER_DELAY;
        line.phase = WAIT_CHARACTER;
        break;
    case WAIT_CHARACTER:
        if (wait != 0) {
            wait--;
        } else {
            line.phase = after_wait;
        }
        draw_text(visible);
        expire_sprites();
        break;
    case FINISH_LINE:
        expire_sprites();
        if (wait++ > FINISH_DELAY) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}

#ifdef NONMATCHING
/* this side's end of the line where a skip asks it, as typing.h says */
void event_typing_end(void)
{
    if (line.phase != FINISH_LINE) {
        line.phase = FINISH_LINE;
        wait = 0;
    }
}
#endif

static void expire_sprites(void)
{
    int i;
    for (i = 0; i < SPRITE_SLOTS; i++) {
        if (slots[i].frames != 0) {
            slots[i].frames--;
            if (slots[i].frames == 0) {
                text_sprite_destroy(slots[i].sprite);
            }
        }
    }
}

static void draw_text(const char *text)
{
    struct text_sprite *sprite;
    int i;

    position.screen.x = line_x;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below. a language past TEXT_LANGUAGES
     * draws nothing, as does a sprite that can't be made. the hack's plain form
     * makes the one call */
    if (game_state.text_language < 0 ||
        game_state.text_language >= TEXT_LANGUAGES) {
        return;
    }
    sprite = text_sprite_create(text, game_state.text_language);
    if (sprite == NULL) {
        return;
    }
#else
    /* @hack redundant: a case and a call for each language, where one call
     * would do for any language the cases name */
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        sprite = text_sprite_create(text, game_state.text_language);
        break;
    case TEXT_ENGLISH:
        sprite = text_sprite_create(text, game_state.text_language);
        break;
    case TEXT_FRENCH:
        sprite = text_sprite_create(text, game_state.text_language);
        break;
    case TEXT_SPANISH:
        sprite = text_sprite_create(text, game_state.text_language);
        break;
    case TEXT_GERMAN:
        sprite = text_sprite_create(text, game_state.text_language);
        break;
    }
#endif
    /* @bug uninitialized: a text_language that no case names leaves sprite
     * unset. control skips its assignment at 0x8c509856, and the draw at
     * 0x8c509860 takes it unset */
    text_sprite_draw_text_at(sprite, &position.screen, LINE_SCALE);
    for (i = 0; i < SPRITE_SLOTS; i++) {
        if (slots[i].frames == 0) {
            slots[i].frames = SPRITE_FRAMES;
            slots[i].sprite = sprite;
            break;
        }
    }
}
