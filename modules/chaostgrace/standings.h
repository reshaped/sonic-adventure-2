/* the race's standings: the order the entrants finish in, and each entrant's
 * place. the place the rank shows follows it at most once in 20 frames */
#ifndef CHAOSTGRACE_STANDINGS_H
#define CHAOSTGRACE_STANDINGS_H
#include <sg_xpt.h>
#include "chao/race_record.h"
struct task;

#define RACE_CHEERED 0x100
struct race_participant {
    Sint32 place;
    Uint32 finished;
    Sint32 previous_place, display_timer;
    struct task *chao;
};
struct race_controller_state {
    Sint32 mode;
};
extern Sint32 race_player_count, race_save_frames, race_save_finished;
extern struct race_participant race_participants[RACE_ENTRANTS];
extern struct task *race_finish_order[RACE_ENTRANTS];
extern struct garden_race_time race_fastest_time, race_player_time;
extern Sint32 race_finish_count, race_entrant_count;
extern struct task *race_controller;
extern Sint32 race_clock_running, race_awarded_emblem;

void race_reset_finish_order(void);
struct task *race_chao_in_place(int place);
void race_update_standings(void);
int race_place(int entrant);
#endif
