#include <ninja.h>
#include <sg_xpt.h>
#include "chao/walk_layout.h"

const NJS_POINT3 walk_origins[WALK_ORIGINS] = {{0.0F, 0.0F, 0.0F}};

Float walk_drive_depth = 29.0F;
Float walk_drive_units_x = 0.0316F;
Float walk_drive_units_y = 0.0316F;
Float walk_drive_column_spacing = 257.0F;
Float walk_drive_row_shift = -3.0F;
Float walk_drive_row_spacing = -260.0F;
Float walk_drive_scale = 1.0F;
Angle walk_drive_yaw = 0xb400;
Float walk_drive_left = -30.0F;
Float walk_drive_top = 300.0F;

Float walk_card_chao_depth = 25.5F;
Float walk_card_chao_units_x = 0.0316F;
Float walk_card_chao_units_y = 0.0316F;
Float walk_card_chao_column_spacing = 223.0F;
Float walk_card_chao_row_spacing = -225.0F;
Float walk_card_chao_left = 20.0F;
Float walk_card_chao_top = 290.0F;

Angle walk_card_chao_yaw = 0xc000;
Angle walk_card_chao_column_turn = -0x800;

Float walk_content_chao_depth = 25.5F;
Float walk_content_chao_units_x = 0.0316F;
Float walk_content_chao_units_y = 0.0316F;
Float walk_content_chao_column_spacing = 163.0F;
Float walk_content_chao_row_spacing = -140.0F;
Float walk_content_chao_left = -17.0F;
Float walk_content_chao_top = 290.0F;

Float walk_egg_column_spacing = 180.0F;
Float walk_egg_row_spacing = -175.0F;
Float walk_egg_left = 55.0F;
Float walk_egg_top = -65.0F;
Float walk_egg_scale = 0.4F;
Float walk_egg_depth = 20.0F;
Float walk_content_egg_left = 0.0F;
Float walk_content_egg_top = 18.0F;
Float walk_content_egg_scale = 0.55F;
Float walk_card_cursor_left = 107.0F;
Float walk_content_cursor_left = 95.0F;
Float walk_item_left = -30.0F;
Float walk_item_top = 10.0F;
Float walk_item_column_offset = 40.0F;
Float walk_item_column_padding = 4.0F;
Float walk_item_row_offset = 45.0F;
Float walk_seed_scale = 1.5F;
Float walk_fruit_scale = 0.9F;

Float walk_page_depth = 16.0F;
Float walk_page_units_per_pixel[2] = {0.0316F, 0.0316F};
Float walk_page_units_x = 0.0316F;
Float walk_page_units_y = 0.0316F;
Float walk_page_left = 0.0F;
Float walk_page_top = -137.60001F;
Float walk_page_card_spacing = 142.0F;
Float walk_page_card_scale = 0.55F;
Angle walk_page_yaw = 0xb400;
