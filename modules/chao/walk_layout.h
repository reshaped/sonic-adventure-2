/* where the walk menu places its drives, Chao, eggs, items and pages in
 * front of the camera. a left or top is in screen pixels, which units_x and
 * units_y turn into world units */
#ifndef CHAO_WALK_LAYOUT_H
#define CHAO_WALK_LAYOUT_H
#include <ninja.h>
#include <sg_xpt.h>

extern Float walk_drive_depth;
extern Float walk_drive_units_x;
extern Float walk_drive_units_y;
extern Float walk_drive_column_spacing;
extern Float walk_drive_row_shift;
extern Float walk_drive_row_spacing;
extern Float walk_drive_scale;
extern Angle walk_drive_yaw;
extern Float walk_drive_left;
extern Float walk_drive_top;

extern Float walk_card_chao_depth;
extern Float walk_card_chao_units_x;
extern Float walk_card_chao_units_y;
extern Float walk_card_chao_column_spacing;
extern Float walk_card_chao_row_spacing;
extern Float walk_card_chao_left;
extern Float walk_card_chao_top;

extern Angle walk_card_chao_yaw;
extern Angle walk_card_chao_column_turn;

extern Float walk_content_chao_depth;
extern Float walk_content_chao_units_x;
extern Float walk_content_chao_units_y;
extern Float walk_content_chao_column_spacing;
extern Float walk_content_chao_row_spacing;
extern Float walk_content_chao_left;
extern Float walk_content_chao_top;

extern Float walk_egg_column_spacing;
extern Float walk_egg_row_spacing;
extern Float walk_egg_left;
extern Float walk_egg_top;
extern Float walk_egg_scale;
extern Float walk_egg_depth;
extern Float walk_content_egg_left;
extern Float walk_content_egg_top;
extern Float walk_content_egg_scale;
extern Float walk_card_cursor_left;
extern Float walk_content_cursor_left;
extern Float walk_item_left;
extern Float walk_item_top;
extern Float walk_item_column_offset;
extern Float walk_item_column_padding;
extern Float walk_item_row_offset;
extern Float walk_seed_scale;
extern Float walk_fruit_scale;

extern Float walk_page_depth;
extern Float walk_page_units_per_pixel[2];
extern Float walk_page_units_x;
extern Float walk_page_units_y;
extern Float walk_page_left;
extern Float walk_page_top;
extern Float walk_page_card_spacing;
extern Float walk_page_card_scale;
extern Angle walk_page_yaw;

/* the indexes of walk_origins, from which each placement starts its point.
 * every origin is 0, 0, 0 */
enum walk_origin {
    WALK_DRIVE_ORIGIN = 0,
    WALK_CHAO_ORIGIN = 3,
    WALK_EGG_ORIGIN = 5,
    WALK_ITEM_ORIGIN = 7,
    WALK_PAGE_ORIGIN = 8,
    WALK_ORIGINS = 9
};
extern const NJS_POINT3 walk_origins[WALK_ORIGINS];
#endif
