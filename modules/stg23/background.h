#ifndef STG23_BACKGROUND_H
#define STG23_BACKGROUND_H
#include <ninja.h>

#include "system/texture.h"

/* Hidden Base's land blocks, as bits of land_block_mask[0]. the clip task
 * sets the bit of the camera's block, and the background draws by it */
enum base_block {
    BASE_BLOCK_1 = 0x02,
    BASE_BLOCK_2 = 0x04,
    BASE_BLOCK_3 = 0x08,
    BASE_BLOCK_4 = 0x10,
    BASE_BLOCK_5 = 0x20,
    BASE_BLOCK_6 = 0x40,
    BASE_BLOCK_7 = 0x80,
    BASE_BLOCK_8 = 0x100,
    BASE_BLOCK_9 = 0x200,
    BASE_BLOCK_10 = 0x400,
    BASE_BLOCK_11 = 0x800,
    BASE_BLOCK_12 = 0x1000,
    BASE_BLOCK_13 = 0x2000
};

extern NJS_TEXLIST base_quicksand_texlist;
extern NJS_TEXLIST base_far_pyramid_texlist;
extern NJS_TEXLIST base_low_desert_texlist;
extern NJS_TEXLIST base_high_desert_texlist;

extern NJS_TEXLIST **base_background_groups[];
extern struct texture_pvm base_background_pvms[];
void base_quicksand_draw(void);
extern NJS_CNK_MODEL base_far_pyramid;
extern NJS_CNK_MODEL base_low_desert;
extern NJS_CNK_MODEL base_high_desert;
extern NJS_OBJECT base_sink_0;
extern NJS_OBJECT base_sink_1;
extern NJS_OBJECT base_sink_2;
extern NJS_OBJECT base_sink_3;
extern NJS_OBJECT base_sink_4;
extern NJS_OBJECT base_sink_5;
extern NJS_OBJECT base_sink_6;

#endif
