#include <sg_xpt.h>

#include "chaostgentrance/entrance.h"
#include "chaostgentrance/map_art.h"

/* the first cell of each label in entrance_japanese_cells and in
 * entrance_english_cells. a label takes one cell or two, and not always as
 * many on each sheet */
Sint32 entrance_japanese_ids[25] = {0,  1,  2,  3,  4,  6,  8,  10, 12,
                                    14, 16, 18, 19, 20, 21, 22, 23, 24,
                                    25, 26, 27, 28, 30, 32, 34};

Sint32 entrance_english_ids[25] = {0,  1,  2,  3,  4,  6,  8,  10, 12,
                                   14, 16, 18, 19, 21, 23, 25, 27, 28,
                                   29, 30, 31, 32, 34, 36, 38};

Float entrance_caption_x = -46.0F;
Float entrance_caption_y = 199.0F;

/* a zero word that no code reads. the image holds it after
 * entrance_caption_y */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Uint32 dat_0c9ce0bc = 0;
