/* the titles by beetle kind and the names by bomb kind that the beetle's
 * debug display prints */
#ifndef SHARED_BEETLE_STATUS_H
#define SHARED_BEETLE_STATUS_H
#include <sg_xpt.h>

#include "shared/beetle.h"
#include "shared/enemy_bomb.h"

extern const char *beetle_titles[BEETLE_KINDS];
extern const char *beetle_bomb_names[BOMB_KINDS];

/* @hack type: the ready flag declared as an array of one, where the flag
 * alone would do */
extern volatile Sint8 bomber_draw_ready[1];

#endif
