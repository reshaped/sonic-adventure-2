/* the Chao world's file on a memory card: its name and its header's size.
 * the comments, palette and icon here are what the Dreamcast's file menu
 * shows. the icon has three frames of 32 by 32 pixels, four bits to a
 * pixel */
#ifndef CHAO_WORLD_CARD_ART_H
#define CHAO_WORLD_CARD_ART_H
#include <sg_xpt.h>
extern const char world_card_task_name[15];
extern const char world_card_filename[16];
extern const Uint32 world_card_header_bytes;
extern const char world_card_comment[18], world_card_title[34];
extern const Uint16 world_card_palette[16];
extern const Uint8 world_card_icon[3][512];
extern Uint16 world_card_icon_count, world_card_icon_speed;
extern void *world_card_visual;
extern Uint16 world_card_visual_type;
#endif
