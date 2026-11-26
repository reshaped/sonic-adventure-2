/* the GCC game's end of an event's typed line, for a skip that comes while
 * it types: the line ends where it stands, and every sprite it drew is
 * destroyed before the step says the line is over */
#include <assert.h>
#include <string.h>

/* modules/event/typing.c, with the line's place on the screen written in
 * for the disc's own (tests/test_event_typing.py) */
#include "typing.inc"

struct game_state game_state;
struct message_list *event_messages;
struct event_tracks *event_tracks;

static struct event_tracks tracks;
static struct message line_message;
static struct message_list messages = {0, &line_message, 1};
static struct text_sprite sprites[64];
static int created;
static int destroyed;
static int drawn;
static int keys;

struct text_sprite *text_sprite_create(const char *text, int language)
{
    assert(created < 64 && language == game_state.text_language);
    return &sprites[created++];
}

void text_sprite_destroy(struct text_sprite *sprite)
{
    assert(sprite >= sprites && sprite < sprites + created);
    destroyed++;
}

void text_sprite_draw_text_at(struct text_sprite *sprite, NJS_POINT2 *pos,
                              Float scale)
{
    assert(pos->x == 48.0F && pos->y == 400.0F);
    drawn++;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    assert(id == TYPING_SOUND);
    keys++;
    return 0;
}

/* the steps the line takes from its start, with end called before step
 * end_at, or never where end_at is below 0 */
static int steps_to_end(int end_at)
{
    int steps = 0;

    created = destroyed = drawn = keys = 0;
    assert(event_typing_start());
    for (;;) {
        if (steps == end_at) {
            event_typing_end();
        }
        steps++;
        if (event_typing_step()) {
            return steps;
        }
        assert(steps < 1000);
    }
}

int main(void)
{
    /* four characters of two bytes each */
    static char text[] = "\x82\xa0\x82\xa2\x82\xa4\x82\xa6";
    int whole;
    int typed;

    event_messages = &messages;
    event_tracks = &tracks;
    line_message.text = text;
    tracks.lines[0].frame = EVENT_TYPED_FRAME;
    tracks.lines[0].duration = 48;

    /* the whole line: a sprite a step while it types, each destroyed */
    whole = steps_to_end(-1);
    assert(keys == 4 && created == drawn && destroyed == created);
    typed = created;

    /* ended at its third step, it types no more. the step says so once
     * FINISH_LINE's own steps have destroyed its sprites */
    assert(steps_to_end(2) == 2 + FINISH_DELAY + 2);
    assert(created == 2 && destroyed == 2 && keys == 1);
    assert(created < typed && 2 + FINISH_DELAY + 2 < whole);

    /* ended before its first step, it draws nothing */
    assert(steps_to_end(0) == FINISH_DELAY + 2);
    assert(created == 0 && destroyed == 0 && keys == 0);

    /* an end while the line finishes changes nothing */
    assert(steps_to_end(whole - 2) == whole);
    assert(created == typed && destroyed == typed);
    return 0;
}
