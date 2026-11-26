#include "chao/race_record.h"
#include "chaostgrace/track_events.h"
#define END_WATER {-1.0F, -1.0F, 0.0F}

struct race_water_span beg_swim_water[] = {{20.0F, 22.0F, -47.0F}, END_WATER};
struct race_water_span beg_fly_water[] = {{7.0F, 11.0F, -30.0F}, END_WATER};
struct race_water_span beg_run_water[] = {END_WATER};
struct race_water_span beg_power_water[] = {END_WATER};
struct race_water_span jwl_swim_water[] = {
    {3.0F, 7.4F, -30.0F}, {19.3F, 25.0F, -47.0F}, END_WATER};
struct race_water_span jwl_fly_water[] = {
    {5.0F, 27.0F, -30.0F}, {31.0F, 38.0F, -47.0F}, END_WATER};
struct race_water_span jwl_run_water[] = {END_WATER};
struct race_water_span jwl_power_water[] = {{5.0F, 8.0F, -30.0F}, END_WATER};
struct race_water_span jwl_il_water[] = {{4.0F, 8.0F, 37.5F}, END_WATER};
struct race_water_span jwl_long_water[] = {{5.5F, 8.0F, -30.0F},
                                           {26.7F, 30.5F, 37.5F},
                                           {63.5F, 66.0F, -47.0F},
                                           END_WATER};
struct race_water_span *race_water_span = 0;
void (*race_track_steps[RACE_RECORD_TRACKS])(struct task *) = {
    race_step_beg_swim,  race_step_beg_fly,   race_step_beg_run,
    race_step_beg_power, race_step_jwl_swim,  race_step_jwl_fly,
    race_step_jwl_run,   race_step_jwl_power, race_step_jwl_il,
    race_step_jwl_long};
