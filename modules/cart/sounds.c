#include "cart/stage_assets.h"
#include "sound/lists.h"

static const struct sound entries[30] = {
#include "cart/sounds/entries.inc"
};

const struct sound_list cart_sounds = {
#include "cart/sounds/cart_sounds.inc"
};
