/* the world file as the card reads and writes it. garden opens with
 * garden_saved_world's bytes, and chao holds garden_saved_chao's */
#ifndef CHAO_SAVE_BUFFERS_H
#define CHAO_SAVE_BUFFERS_H
#include "chao/garden_save.h"

struct garden_file_image {
    Uint8 garden[0x380];
    Uint8 chao[0x6000];
};
extern struct garden_file_image garden_file_image;
extern struct garden_save garden_saved_world;

#endif
