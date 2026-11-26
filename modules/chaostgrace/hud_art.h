/* the cells of the race HUD's sprites, which draw with AL_RACE_ADV_TEX's
 * textures */
#ifndef CHAOSTGRACE_HUD_ART_H
#define CHAOSTGRACE_HUD_ART_H
#include <ninja.h>

enum { RACE_HUD_CELL_COUNT = 69 };
extern const NJS_TEXANIM race_hud_cells[RACE_HUD_CELL_COUNT];
/* NJS_SPRITE's tanim isn't const, so each draw takes the cells through this
 * cast. no draw writes a cell */
#define RACE_HUD_CELLS ((NJS_TEXANIM *)race_hud_cells)

#endif
