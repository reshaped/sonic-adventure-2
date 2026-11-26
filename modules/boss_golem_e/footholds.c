/* boss_golem/footholds.c compiled again for BOSS_GOLEM_E.PRS, with this
 * binary's names and switches */
#include "boss_golem_e/footholds.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define golem_floor_y golem_e_floor_y
#define golem_footholds_build golem_e_footholds_build
#define golem_footholds_create golem_e_footholds_create
#define golem_footholds_open_gaps golem_e_footholds_open_gaps
#define golem_foothold_request golem_e_foothold_request
#define golem_foothold_break golem_e_foothold_break
#define golem_foothold_hit golem_e_foothold_hit
#define golem_footholds_restore golem_e_footholds_restore
#define golem_foothold_state golem_e_foothold_state
#define golem_footholds_lowest_state golem_e_footholds_lowest_state
#define golem_footholds_lowest_near golem_e_footholds_lowest_near
/* NOLINTEND(readability-identifier-naming) */
#define FOOTHOLD_HIT SECOND_HIT_BREAKS
#define FOOTHOLD_DRAW_START njSetTexture(&golem_foothold_a_texlist);
#define FOOTHOLD_SINK_SOUND 10
#define TAKE_LOWEST(foothold) COMPARE_READ(foothold)

#include "boss_golem/footholds.c" /* NOLINT(bugprone-suspicious-include) */
