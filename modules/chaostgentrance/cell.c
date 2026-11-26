#include <sg_xpt.h>
#include <ninja.h>
#include "chaostgentrance/graphics.h"

void entrance_draw_cell(NJS_SPRITE *sprite, Int cell, Float depth)
{
    njDrawSprite2D(sprite, cell, depth, sprite->tanim[cell].attr);
}
