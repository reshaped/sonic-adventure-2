/* the kindergarten's keyboard: its message strings, the queue of a USB
 * keyboard's keys, and the soft keyboard */
#ifndef CHAOSTGKINDER_KEYBOARD_H
#define CHAOSTGKINDER_KEYBOARD_H
#include <sg_xpt.h>

/* the USB HID usages of the keys the code tests. kinder_keys_next gives them
 * in a key's low byte */
enum kinder_key {
    KBD_NO_EVENT = 0,
    KBD_ERROR_ROLLOVER = 1,
    KBD_POST_FAIL = 2,
    KBD_ERROR_UNDEFINED = 3,
    KBD_ENTER = 40,
    KBD_ESCAPE = 41,
    KBD_BACKSPACE = 42,
    KBD_TAB = 43,
    KBD_SPACE = 44,
    KBD_ZENKAKU_HANKAKU = 53,
    KBD_DELETE = 76,
    KBD_RIGHT = 79,
    KBD_LEFT = 80,
    KBD_DOWN = 81,
    KBD_UP = 82
};

/* decode the kbdtext file of game_state.text_language into the strings'
 * static buffer, over the last load's */
void kinder_strings_load(void);
/* does nothing. under GCC it empties the strings, which a widget may still
 * point into */
void kinder_strings_release(void);
/* the id-th string of the loaded file: a run of 16-bit codes ended by 0.
 * id must be at least 0 and below the file's string count. an id at or past
 * the count reads a string's code as its offset. a negative id reads its
 * offset from before the buffer. under GCC either gives an empty string */
Uint16 *kinder_string(Sint32 id);
void kinder_keys_start(void);
void kinder_keys_stop(void);
void kinder_keys_poll(void);
void kinder_keys_clear(void);
int kinder_key_held(Uint8 key);
int kinder_keys_active(void);
/* the modifier bits kinder_keys_poll last read. KBD_SHIFT_KEYS are the bits
 * of the left and right shift keys */
Uint8 kinder_key_modifiers(void);
#define KBD_SHIFT_KEYS 0x22
/* the next key in the queue, or 0 once the queue is empty. a key's low byte,
 * KBD_USAGE_MASK, holds its USB HID usage, and its high byte,
 * KBD_MODIFIER_MASK, holds the modifier bits */
Uint16 kinder_keys_next(void);
#define KBD_USAGE_MASK 0xff
#define KBD_MODIFIER_MASK 0xff00
void kinder_keyboard_draw(void);
void kinder_keyboard_open(void);
void kinder_keyboard_close(void);
Uint16 kinder_keyboard_input(Uint16 key);
void kinder_keyboard_get_position(Float *x, Float *y, Float *z);
void kinder_keyboard_set_position(Float x, Float y, Float z);
void kinder_keyboard_start(void);
void kinder_keyboard_stop(void);
#endif
