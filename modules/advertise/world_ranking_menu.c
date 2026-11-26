/* the world ranking's menu. its start gathers the lines of
 * emblem_results.h's screen. its step never opens the screen */
#include <sg_xpt.h>

#include "advertise/emblem_results.h"
#include "advertise/menu_textures.h"
#include "advertise/menus.h"
#include "advertise/world_ranking.h"
#include "advertise/world_ranking_menu.h"
#include "sound/adx.h"

struct world_ranking_work world_ranking_work;

/* nothing calls these, and no header declares them. they are external only
 * because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* close emblem_results.h's screen, as emblem_results_menu_close does */
void world_ranking_menu_close(void);
/* fun_8c517ebc and fun_8c517ec0 do nothing */
void fun_8c517ebc(void);
void fun_8c517ec0(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* world_ranking_menu_start clears unk_00 and unk_08. nothing reads them */
struct world_ranking_menu_state {
    Sint32 unk_00;
    Sint32 unk_04;
    Sint32 unk_08;
};
static struct world_ranking_menu_state ranking_state;

/* menu_control.step, which world_ranking_menu_step never reads */
#define STEP_OPEN 0

void world_ranking_menu_start(void)
{
    menu_textures_select(MENU_TEXTURES_WORLD_RANKING);
    menu_control.step = STEP_OPEN;
    emblem_results_gather();
    music_play_name("advsng_3.adx");
    ranking_state.unk_00 = 0;
    ranking_state.unk_08 = 0;
}

void world_ranking_menu_close(void)
{
    menu_works.work_19->open = FALSE;
    menu_works.work_19->unk_04 = 0;
}

Bool world_ranking_menu_step(void)
{
    return FALSE;
}

void fun_8c517ebc(void)
{
}

void fun_8c517ec0(void)
{
}
