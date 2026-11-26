/* the cells of the kart race panel's sprites, for the kart race and download
 * menus. kart_race_panel.c names the cells it draws. kart_race_panel_create
 * centers every cell but the bar, which draws from its top left corner */
#ifndef ADVERTISE_KART_RACE_CELLS_H
#define ADVERTISE_KART_RACE_CELLS_H
#include <ninja.h>

#define KART_RACE_CELLS 61
extern NJS_TEXANIM kart_race_cells[KART_RACE_CELLS];
#endif
