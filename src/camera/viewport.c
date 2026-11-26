/* the area of the screen each camera's view takes */
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/viewport.h"
#include "system/game_state.h"

/* the pairs of areas two cameras can take, which game_state.split_layout
 * picks */
#define SPLIT_LAYOUTS 8
/* the quarters of the screen, which four cameras take */
#define QUARTERS 4

/* an entry of the tables of areas: an area, and a word that no code reads */
struct viewport_entry {
    struct viewport_area area;
    Sint32 unk_20;
};

/* the areas of each two-camera layout, camera 0's first */
/* clang-format off */
static struct viewport_entry viewport_split_areas[SPLIT_LAYOUTS * 2] = {
    {{{{  0.0F,   0.0F}, {320.0F, 480.0F}}, {160.0F, 240.0F}, {480.0F, 240.0F}}, 0},
    {{{{320.0F,   0.0F}, {640.0F, 480.0F}}, {480.0F, 240.0F}, {480.0F, 240.0F}}, 0},
    {{{{  0.0F,   0.0F}, {640.0F, 240.0F}}, {320.0F, 120.0F}, {320.0F, 352.0F}}, 0},
    {{{{  0.0F, 240.0F}, {640.0F, 480.0F}}, {320.0F, 360.0F}, {  0.0F,   0.0F}}, 0},
    {{{{352.0F, 288.0F}, {576.0F, 448.0F}}, {464.0F, 368.0F}, {288.0F, 208.0F}}, 1},
    {{{{352.0F, 288.0F}, {576.0F, 448.0F}}, {464.0F, 368.0F}, {288.0F, 208.0F}}, 1},
    {{{{352.0F, 288.0F}, {576.0F, 448.0F}}, {464.0F, 368.0F}, {288.0F, 208.0F}}, 0},
    {{{{352.0F, 288.0F}, {576.0F, 448.0F}}, {464.0F, 368.0F}, {288.0F, 208.0F}}, 0},
    {{{{ 32.0F,  32.0F}, {256.0F, 192.0F}}, {144.0F, 112.0F}, {352.0F, 272.0F}}, 0},
    {{{{ 32.0F,  32.0F}, {256.0F, 192.0F}}, {144.0F, 112.0F}, {352.0F, 272.0F}}, 0},
    {{{{ 32.0F,  32.0F}, {256.0F, 192.0F}}, {144.0F, 112.0F}, {352.0F, 272.0F}}, 1},
    {{{{ 32.0F,  32.0F}, {256.0F, 192.0F}}, {144.0F, 112.0F}, {352.0F, 272.0F}}, 1},
    {{{{  0.0F,   0.0F}, {640.0F, 224.0F}}, {320.0F, 112.0F}, {320.0F, 368.0F}}, 0},
    {{{{  0.0F, 256.0F}, {640.0F, 480.0F}}, {320.0F, 368.0F}, {  0.0F,   0.0F}}, 0},
    {{{{  0.0F,   0.0F}, {640.0F, 240.0F}}, {320.0F, 160.0F}, {  0.0F,   0.0F}}, 0},
    {{{{  0.0F, 256.0F}, {640.0F, 480.0F}}, {320.0F, 400.0F}, {  0.0F,   0.0F}}, 0},
};
static struct viewport_entry viewport_quarters[QUARTERS] = {
    {{{{  0.0F,   0.0F}, {320.0F, 224.0F}}, {160.0F, 112.0F}, {  0.0F,   0.0F}}, 0},
    {{{{320.0F,   0.0F}, {640.0F, 224.0F}}, {480.0F, 112.0F}, {  0.0F,   0.0F}}, 0},
    {{{{  0.0F, 256.0F}, {320.0F, 480.0F}}, {160.0F, 368.0F}, {  0.0F,   0.0F}}, 0},
    {{{{320.0F, 256.0F}, {640.0F, 480.0F}}, {480.0F, 368.0F}, {  0.0F,   0.0F}}, 0},
};
/* clang-format on */
static void viewport_set_split(int camera)
{
    viewport_set_area(
        &viewport_split_areas[(game_state.split_layout * 2) + camera].area);
}

static void viewport_set_quarter(int camera)
{
    viewport_set_area(&viewport_quarters[camera].area);
    viewport_set_screen(VIEWPORT_AREA);
}

void viewport_set(int camera)
{
    /* @hack spelling: the tests of single_view and of one camera in arms of
     * their own, each calling viewport_set_whole, where one test joined with
     * || would do */
    if (game_state.single_view) {
        viewport_set_whole();
    } else if (camera_active_count == 1) {
        viewport_set_whole();
    } else if (camera_active_count == QUARTERS) {
        viewport_set_quarter(camera);
    } else {
        /* @hack redundant: a do loop that runs once */
        do {
            viewport_set_split(camera);
            viewport_set_screen(VIEWPORT_AREA);
        } while (0);
    }
}

void viewport_select_split(int layout)
{
    game_state.split_layout = layout;
}
