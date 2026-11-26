/* the emi's labels. no code reads the titles of its kinds. only a readout in
 * emi_draw that never runs prints the names of the cannon's bombs */
#include "shared/enemy_emi.h"
#include "shared/enemy_emi_art.h"

const char *emi_kind_names[EMI_KINDS] = {
    "< ENEMY EMI NORMAL >", "< ENEMY EMI SPIKED >",  "< ENEMY EMI DOOL >",
    "< ENEMY EMI CANNON >", "< ENEMY EMI ARMORED >",
};

const char *emi_bomb_names[] = {
    "NORMAL", "RED ONE", "HEAVY", "FIRE", "RING",
};
