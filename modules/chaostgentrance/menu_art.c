/* the entrance menu's cells, by sheet. a cell's u and v count 256 to a
 * texture's side. each title sheet ends with a zero-sized cell, which stops
 * title_draw after the last title */
#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgentrance/graphics.h"

const NJS_TEXANIM entrance_common_cells[156] = {
#include "chaostgentrance/menu_art/entrance_common_cells.inc"
};

/* the rivals' portraits, a cell each, by rival. every cell has the same
 * center, so grid.c draws any rival's at a tile's place */
const NJS_TEXANIM entrance_rival_cells[20] = {
#include "chaostgentrance/menu_art/entrance_rival_cells.inc"
};

const NJS_TEXANIM entrance_japanese_title_cells[18] = {
#include "chaostgentrance/menu_art/entrance_japanese_title_cells.inc"
};

const NJS_TEXANIM entrance_japanese_cells[37] = {
#include "chaostgentrance/menu_art/entrance_japanese_cells.inc"
};

const NJS_TEXANIM entrance_english_title_cells[16] = {
#include "chaostgentrance/menu_art/entrance_english_title_cells.inc"
};

const NJS_TEXANIM entrance_english_cells[41] = {
#include "chaostgentrance/menu_art/entrance_english_cells.inc"
};

/* a zero word that no code reads. the image holds it before event.c's
 * event_order */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Uint32 dat_0c9cdf54 = 0;
