#include <sg_xpt.h>
#include "chao/race_record.h"
#include "chaostgrace/standings.h"
#include "system/task.h"

Sint32 race_player_count;
Sint32 race_save_frames;
Sint32 race_save_finished;
struct race_participant race_participants[RACE_ENTRANTS];
struct task *race_finish_order[RACE_ENTRANTS];
struct garden_race_time race_fastest_time;
struct garden_race_time race_player_time;
Sint32 race_finish_count;
Sint32 race_entrant_count;

struct task *race_controller = NULL;
Sint32 race_clock_running = TRUE;
Sint32 race_awarded_emblem = FALSE;
