/* the walk menu's texture lists, one for Japanese and one for the other
 * languages. its VMU models, sprite cells and message links are here too. a
 * message is a chain of links, each a text's id and the next link. next is
 * another link's index, WALK_NEXT_TEXT for the following one, or
 * WALK_END_TEXT */
#ifndef CHAO_WALK_ART_H
#define CHAO_WALK_ART_H
#include <ninja.h>
#include <sg_xpt.h>
#define WALK_TEXTURES 9
#define WALK_SPRITE_CELLS 53
#define WALK_TEXT_LINKS 116
#define WALK_NEXT_TEXT 127
#define WALK_END_TEXT (-1)
struct walk_message_link {
    Uint8 text;
    Sint8 next;
};
extern NJS_TEXLIST walk_textures_jp, walk_textures_intl;
extern NJS_CNK_OBJECT walk_vmu_model, walk_multi_vmu_model;
extern NJS_TEXANIM walk_cells[WALK_SPRITE_CELLS];
extern struct walk_message_link walk_message_links[WALK_TEXT_LINKS];
#endif
