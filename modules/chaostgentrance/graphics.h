/* the cells of the entrance menu's sprites, and the macros its draws share */
#ifndef CHAOSTGENTRANCE_GRAPHICS_H
#define CHAOSTGENTRANCE_GRAPHICS_H
#include <ninja.h>
#include "draw/lines.h"

/* the cells, by sheet. NJS_SPRITE's tanim isn't const, so each draw casts
 * the const away. no draw writes a cell */
extern const NJS_TEXANIM entrance_common_cells[];
extern const NJS_TEXANIM entrance_rival_cells[];
extern const NJS_TEXANIM entrance_japanese_title_cells[];
extern const NJS_TEXANIM entrance_japanese_cells[];
extern const NJS_TEXANIM entrance_english_title_cells[];
extern const NJS_TEXANIM entrance_english_cells[];

void entrance_draw_cell(NJS_SPRITE *sprite, Int cell, Float depth);

#define ENTRANCE_DRAW_CELL(sprite, cell, depth)                                \
    njDrawSprite2D(&(sprite), (cell), (depth), (sprite).tanim[(cell)].attr)

#define RGB(red, green, blue)                                                  \
    constant_material_set(1.0F, (red) / 255.0F, (green) / 255.0F,              \
                          (blue) / 255.0F)
#define TINT(alpha, red, green, blue)                                          \
    constant_material_set((alpha), (red) / 255.0F, (green) / 255.0F,           \
                          (blue) / 255.0F)
/* each race group's color, which tints the group's title, its tabs and its
 * course map's frame. the beginner, jewel and multiplayer rows take it too */
#define BEGINNER_COLOR(alpha) TINT((alpha), 239, 124, 112)
#define JEWEL_COLOR(alpha) TINT((alpha), 190, 96, 255)
#define CHALLENGE_COLOR(alpha) TINT((alpha), 99, 202, 111)
#define DARK_COURSES_COLOR(alpha) TINT((alpha), 23, 171, 234)
#define HERO_COURSES_COLOR(alpha) TINT((alpha), 144, 53, 58)
#define MULTI_COLOR(alpha) TINT((alpha), 255, 95, 155)
/* the color of a course or multiplayer row under the cursor */
#define LIT_COLOR(alpha) TINT((alpha), 248, 201, 58)
#define CLEAR_COLOR() constant_material_set(0.0F, 0.0F, 0.0F, 0.0F)

#endif
