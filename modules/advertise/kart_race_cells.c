/* ADVERTISE.PRS's texture animation, from the disc. its D lies between
 * kart_rouge.c and kart_race.c */
#include <ninja.h>

#include "advertise/kart_race_cells.h"

NJS_TEXANIM kart_race_cells[KART_RACE_CELLS] = {
#include "advertise/kart_race_cells/kart_race_cells.inc"
};
