#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/sound_test_layout.h"
#include "camera/viewport.h"
#include "draw/screen.h"

NJS_POINT3 menu_sound_test_message_pos = {
#include "advertise/sound_test_layout/menu_sound_test_message_pos.inc"
};
struct menu_sound_test_view menu_sound_test_view_start = {
    0, {0, 30, 25}, {0, 0, 0}, {{0, 0, 0, {NULL, NULL, 0, 0}}}};
struct task *menu_sound_test_task = NULL;
struct viewport_area menu_sound_test_disc_area = {
    {{0.0F, 0.0F}, {SCREEN_WIDTH, SCREEN_HEIGHT}},
    {160.0F, 240.0F},
    {0.0F, 0.0F}};
Float menu_sound_test_arrow_x = 450.0F;
Float menu_sound_test_arrow_up_y = 80.0F;
Float menu_sound_test_arrow_down_y = 350.0F;
struct menu_list menu_sound_test_lists_list = {
#include "advertise/sound_test_layout/menu_sound_test_lists_list.inc"
};
struct menu_list menu_sound_test_songs_list = {
#include "advertise/sound_test_layout/menu_sound_test_songs_list.inc"
};
