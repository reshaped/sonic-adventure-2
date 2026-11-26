/* The two walker duels share the Tails and Dr. Eggman title glyphs. Each
 * stage keeps its own title descriptors and intro duration. */
#ifndef SHARED_WALKER_TITLE_LETTERS_H
#define SHARED_WALKER_TITLE_LETTERS_H

#include "shared/boss_title.h"
#include "shared/walker_title_textures.h"

static struct boss_title_letter tails_letters[5] = {
#include "shared/walker_title_letters/tails_letters.inc"
};
static struct boss_title_letter eggman_letters[9] = {
#include "shared/walker_title_letters/eggman_letters.inc"
};
static struct boss_title tails_title = {
#include "shared/walker_title_letters/tails_title.inc"
};
static struct boss_title eggman_title = {
#include "shared/walker_title_letters/eggman_title.inc"
};

#endif
