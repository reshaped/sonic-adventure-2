/* each track's water and each track's racer step. a track's water is a list
 * of spans of race progress, each with the water's height. race_check_water
 * uses a span's height while a Chao's progress lies strictly between its
 * start and end. a span with a negative start ends the list */
#ifndef CHAOSTGRACE_TRACK_EVENTS_H
#define CHAOSTGRACE_TRACK_EVENTS_H
#include <sg_xpt.h>
#include "chao/race_record.h"
struct task;
struct race_water_span {
    Float start, end, height;
};
extern struct race_water_span beg_swim_water[], beg_fly_water[],
    beg_run_water[], beg_power_water[];
extern struct race_water_span jwl_swim_water[], jwl_fly_water[],
    jwl_run_water[], jwl_power_water[];
extern struct race_water_span jwl_il_water[], jwl_long_water[];
extern struct race_water_span *race_water_span;
extern void (*race_track_steps[RACE_RECORD_TRACKS])(struct task *);
void race_step_beg_swim(struct task *chao);
void race_step_beg_fly(struct task *chao);
void race_step_beg_run(struct task *chao);
void race_step_beg_power(struct task *chao);
void race_step_jwl_swim(struct task *chao);
void race_step_jwl_fly(struct task *chao);
void race_step_jwl_run(struct task *chao);
void race_step_jwl_power(struct task *chao);
void race_step_jwl_il(struct task *chao);
void race_step_jwl_long(struct task *chao);
#endif
