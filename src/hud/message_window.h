/* the message window, which shows a text a page at a time and plays each
 * page's voice. one window is open at a time. draw/text_sprite.c defines
 * it, beside the text sprites its pages draw in */
#ifndef HUD_MESSAGE_WINDOW_H
#define HUD_MESSAGE_WINDOW_H
#include <sg_xpt.h>

#include "draw/text_sprite.h"

struct task;

/* the kinds of window message_window_open takes. a MESSAGE_WINDOW_INSTANT
 * window opens and closes at once, and every other kind fades in and out.
 * a MESSAGE_WINDOW_HIDDEN window never draws */
#define MESSAGE_WINDOW_INSTANT 0
#define MESSAGE_WINDOW_FADE 1
#define MESSAGE_WINDOW_HIDDEN 5
/* a MESSAGE_WINDOW_UNFOLD window unfolds as it fades in. it widens first,
 * at a tenth of its full height, and then grows to its full height. it folds
 * the same way as it fades out */
#define MESSAGE_WINDOW_UNFOLD 4
/* a page's band is as tall as its text. except under MESSAGE_WINDOW_FITTED
 * and MESSAGE_WINDOW_UNFOLD, it is no less than MESSAGE_WINDOW_HEIGHT. a
 * MESSAGE_WINDOW_LOW window puts a text shorter than its band at the band's
 * foot. the other kinds put it at the top */
#define MESSAGE_WINDOW_LOW 2
#define MESSAGE_WINDOW_FITTED 3

/* where each page's sprite starts, in pixels. MESSAGE_WINDOW_X and
 * MESSAGE_WINDOW_Y are its top left corner on the screen.
 * MESSAGE_WINDOW_WIDTH is the least width its text is laid out in.
 * MESSAGE_WINDOW_HEIGHT is the least height of its band. every page takes the
 * texture of global index MESSAGE_WINDOW_TEXTURE */
#define MESSAGE_WINDOW_X 70
#define MESSAGE_WINDOW_Y 352
#define MESSAGE_WINDOW_WIDTH 500
#define MESSAGE_WINDOW_HEIGHT 52
#define MESSAGE_WINDOW_TEXTURE 100

/* a window's record, which its task's motion points to. the window's alpha
 * is the submode of the task's work, read as a Uint8 from 0 to 255. the
 * task's unk_3c holds the voice_base message_window_open_at_voice took */
struct message_window {
    /* the sprite of the page that shows. each page sets it up again, so a
     * caller's change to it lasts until the next page */
    struct text_sprite sprite;
    int color; /* the text's own color as 0xRRGGBB, or -1 for white */
    int unk_58; /* each page sets it to 0 */
    /* the steps the page still shows, and -1 once the last page is over */
    int count;
    int language;
    int time; /* message_window_open's time */
    char *text; /* the window's copy of its text, from the Shinobi heap */
    char *next; /* the '\f' that starts the next page, or null */
    struct task **owner; /* message_window_open's owner */
    Sint16 pitch_x; /* the pitches each page's sprite takes */
    Sint16 pitch_y;
    int unk_78;
    int kind; /* a MESSAGE_WINDOW_ kind */
    /* the stream that plays the page's voice, as voice_play returned it, or
     * -1 */
    int voice;
};

/* the open window's task, or null where none is open */
struct task *message_window_current(void);
/* the record of a window's task, or null where the task has none. task
 * must not be null */
struct message_window *message_window_record(struct task *task);
/* destroy the open window, if any. then open a window of kind on a copy of
 * text in language, and return its task. return null where the heap can't
 * give the task, its record or the copy. a window that replaces an open one
 * starts at full alpha. where owner isn't null, *owner gets the task. the
 * window sets *owner back to null as it ends, if *owner still holds it. a null
 * return leaves *owner as it was, but the window it destroyed may have cleared
 * it.
 *
 * a '\f' starts each page after the first, and may start the first. codes may
 * follow it, up to a space or a ':'. W<n> gives the page a count of n, and S<n>
 * the voice n. A<n> gives it the voice n + voice_base, and message_window_open
 * takes voice_base as 0. the rest of the page is a text as draw/text_sprite.h
 * has it. the window draws it in WORK_AREA, over what that held. a page with a
 * voice plays it as the page opens.
 *
 * a page shows its count of steps at full alpha: its W code's, or else time.
 * a negative time gives each page a count of 15. the window then holds the
 * page at 0 until player_pads[-time - 1] presses A. at a count of 0 the window
 * turns to its next page, or closes after its last.
 *
 * a MESSAGE_WINDOW_INSTANT window never counts down. its page with a count
 * above 0 stays until something destroys the window. any other kind fades in
 * over 7 steps and counts down at full alpha. it fades out over 11 steps as
 * it closes. a window destroyed while its alpha isn't 0 stops its page's voice.
 * no window draws while game_state.message_window_on isn't 1 */
struct task *message_window_open(int kind, const char *text, int time,
                                 int language, struct task **owner);
struct task *message_window_open_at_voice(int kind, const char *text, int time,
                                          int language, struct task **owner,
                                          int voice_base);
/* message_window_open without an owner */
struct task *message_window_open_unowned(int kind, const char *text, int time,
                                         int language);

#endif
