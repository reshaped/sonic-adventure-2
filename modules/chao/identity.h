/* a Chao's gene. origin is the saved world's console and timestamp, which
 * every Chao born in that world shares. birth is the Chao's own stamp,
 * which tells those Chao apart */
#ifndef CHAO_IDENTITY_H
#define CHAO_IDENTITY_H
#include <sg_xpt.h>

#include "chao/emotion.h"
#include "chao/life.h"

struct garden_origin {
    Uint32 console[2];
    Uint32 timestamp;
};

struct chao_gene {
    Uint32 birth;
    struct garden_origin origin;
    Uint8 skill[CHAO_SKILLS][2], life[2];
    Sint8 run_power[2], fly_swim[2], alignment[2];
    Uint8 personality[CHAO_FICKLENESS - CHAO_CURIOSITY + 1][2];
    Uint8 taste[2], tv[2], music[2];
    Uint8 color[2], texture[2], shiny[2];
};

#endif
