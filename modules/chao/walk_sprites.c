/* the walk menu's sprite cells and message links. garden_walk_texture_list
 * is its loaded texture list, or null while none is loaded */
#include <ninja.h>
#include "chao/walk_art.h"
#include "chao/walk_card.h"

NJS_TEXLIST *garden_walk_texture_list = NULL;

NJS_TEXANIM walk_cells[WALK_SPRITE_CELLS] = {
#include "chao/walk_sprites/walk_cells.inc"
};

struct walk_message_link walk_message_links[WALK_TEXT_LINKS] = {
#include "chao/walk_sprites/walk_message_links.inc"
};
