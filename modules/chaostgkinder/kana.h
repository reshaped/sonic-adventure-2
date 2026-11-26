/* A roman spelling and its Shift-JIS outputs. An absent katakana string
 * makes both scripts use the first output, including full-width symbols. */
#ifndef CHAOSTGKINDER_KANA_H
#define CHAOSTGKINDER_KANA_H
struct roman_syllable {
    const char *roman, *hiragana, *katakana;
};

#endif
