/* what a form's text field types for each USB HID usage, unshifted and
 * shifted. field_syllables lists the spellings it converts as they are
 * typed. romaji becomes katakana, and punctuation and digits become
 * full-width */
#ifndef CHAOSTGKINDER_FIELD_KEYS_H
#define CHAOSTGKINDER_FIELD_KEYS_H
#include "chaostgkinder/kana.h"

#define FIELD_KEY_USAGES 57
#define FIELD_SPELLINGS 168

static const char field_keys[FIELD_KEY_USAGES] = {
#include "chaostgkinder/field_keys/field_keys.inc"
};

static const char shifted_field_keys[FIELD_KEY_USAGES] = {
#include "chaostgkinder/field_keys/shifted_field_keys.inc"
};

static const struct roman_syllable field_syllables[FIELD_SPELLINGS + 1] = {
#include "chaostgkinder/field_keys/field_syllables.inc"
};

#endif
