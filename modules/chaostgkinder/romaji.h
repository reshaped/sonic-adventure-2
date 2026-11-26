/* the input method's tables. plain_codes and shifted_codes give the
 * character or input_command of each USB HID usage on a Japanese keyboard.
 * syllables gives the kana each romaji spelling writes */
#ifndef CHAOSTGKINDER_ROMAJI_H
#define CHAOSTGKINDER_ROMAJI_H
#include <sg_xpt.h>

#include "chaostgkinder/kana.h"

enum input_command {
    KEY_F1 = 0xff00,
    KEY_F2 = 0xff01,
    KEY_F3 = 0xff02,
    KEY_F4 = 0xff03,
    KEY_F5 = 0xff04,
    KEY_HIRAGANA = 0xff05,
    KEY_KATAKANA = 0xff06,
    KEY_F8 = 0xff07,
    KEY_F9 = 0xff08,
    KEY_F10 = 0xff09,
    KEY_F11 = 0xff0a,
    KEY_F12 = 0xff0b,
    KEY_UP = 0xff0c,
    KEY_DOWN = 0xff0d,
    KEY_LEFT = 0xff0e,
    KEY_RIGHT = 0xff0f,
    KEY_CAPS_LOCK = 0xff10,
    KEY_SCROLL_LOCK = 0xff11,
    /* a command no usage gives */
    KEY_UNTYPED = 0xff12,
    KEY_PAUSE = 0xff13,
    KEY_PRINT_SCREEN = 0xff14,
    KEY_INSERT = 0xff15,
    KEY_HOME = 0xff16,
    KEY_END = 0xff17,
    KEY_PAGE_UP = 0xff18,
    KEY_PAGE_DOWN = 0xff19,
    KEY_APPLICATION = 0xff1a,
    KEY_KANA = 0xff1b,
    KEY_CONVERT = 0xff1c,
    KEY_NO_CONVERT = 0xff1d
};

#define KEYBOARD_USAGES 140
#define ROMAJI_SPELLINGS 168

static const Uint16 plain_codes[KEYBOARD_USAGES] = {
#include "chaostgkinder/romaji/plain_codes.inc"
};

static const Uint16 shifted_codes[KEYBOARD_USAGES] = {
#include "chaostgkinder/romaji/shifted_codes.inc"
};

/* compose_romaji converts at the first spelling here that ends the pending
 * letters. a spelling that ends a longer one must come after it, so the
 * five lone vowels come last */
static const struct roman_syllable syllables[ROMAJI_SPELLINGS + 1] = {
#include "chaostgkinder/romaji/syllables.inc"
};

#endif
