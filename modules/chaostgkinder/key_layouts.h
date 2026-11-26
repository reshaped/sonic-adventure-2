/* the soft keyboard's six key maps and the atlas they draw from. each map is
 * 16 keys by 7 rows, in row order. a zero code's cell belongs to the next
 * key, which it widens by a cell. a key's row and column place its picture
 * in the atlas, in cells of KEY_SIZE texels */
#ifndef CHAOSTGKINDER_KEY_LAYOUTS_H
#define CHAOSTGKINDER_KEY_LAYOUTS_H
#include <sg_xpt.h>

#define KEYBOARD_COLUMNS 16
#define KEYBOARD_ROWS 7
#define KEYBOARD_CELLS (KEYBOARD_ROWS * KEYBOARD_COLUMNS)
/* a cell's side, in pixels on the screen and in texels in the atlas */
#define KEY_SIZE 22

static const char keyboard_atlas[12] = {
#include "chaostgkinder/key_layouts/keyboard_atlas.inc"
};

struct soft_key {
    Uint16 code;
    Sint8 row, column;
};

static const struct soft_key hiragana_keys[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/hiragana_keys.inc"
};

static const struct soft_key katakana_keys[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/katakana_keys.inc"
};

static const struct soft_key tab_two_keys[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/tab_two_keys.inc"
};

static const struct soft_key tab_three_keys[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/tab_three_keys.inc"
};

static const struct soft_key shifted_keys[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/shifted_keys.inc"
};

static const struct soft_key shifted_tab_three[KEYBOARD_CELLS] = {
#include "chaostgkinder/key_layouts/shifted_tab_three.inc"
};

#endif
