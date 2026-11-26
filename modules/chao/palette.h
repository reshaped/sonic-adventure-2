/* the palettes of a Chao's body. a palette slot is CHAO_PALETTE_BANKS banks
 * of CHAO_BANK_COLORS colors, from bank CHAO_PALETTE_BANKS * slot */
#ifndef CHAO_PALETTE_H
#define CHAO_PALETTE_H

#include <sg_xpt.h>

struct chao_parameters;
struct task;

#define CHAO_PALETTES 39
#define CHAO_PALETTE_BANKS 3
#define CHAO_BANK_COLORS 16
#define CHAO_COLOR_BYTES 4
#define CHAO_PALETTE_COLORS (CHAO_BANK_COLORS * CHAO_PALETTE_BANKS)
#define CHAO_PALETTE_BYTES (CHAO_COLOR_BYTES * CHAO_PALETTE_COLORS)
/* the textures of a child's body that take its palette, then -1s */
extern Sint32 child_palette_textures[8];
extern Uint8 garden_palette_colors[][CHAO_PALETTE_BYTES];

/* fill garden_palette_colors from the palette files. a file in a color mode
 * other than the four 16- and 32-bit ones halts the game */
void garden_load_palettes(void);
/* blend a child's or a hero adult's palette from its parameters into slot's
 * banks. any other type leaves them as they are */
void chao_update_palette(struct chao_parameters *parameters, int slot);
/* bind the paletted textures of the Chao's type to slot's banks. any type
 * but a child or a hero adult binds none */
void chao_bind_palette(struct task *chao, int slot);

#endif
