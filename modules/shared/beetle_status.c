#include <sg_xpt.h>

#include "shared/beetle.h"
#include "shared/beetle_status.h"
#include "shared/enemy_bomb.h"

const char *beetle_titles[BEETLE_KINDS] = {
    "< ENEMY KUMI FIXED >",           "< ENEMY KUMI MOVING >",
    "< ENEMY KUMI ELEC. DISCHARGE >", "< ENEMY KUMI MOVING ELEC. >",
    "< ENEMY KUMI GUNNER >",          "< ENEMY KUMI WARP! >",
    "< ENEMY KUMI BUMPER >",          "< ENEMY KUMI BOMBER >",
};

const char *beetle_bomb_names[BOMB_KINDS] = {
    "NORMAL", "RED ONE", "HEAVY", "FIRE", "RING",
};

volatile Sint8 bomber_draw_ready[1] = {1};
