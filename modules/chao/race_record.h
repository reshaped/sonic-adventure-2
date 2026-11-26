/* the garden race's records: each track's best time and the Chao that ran
 * it. each event's best level and the race's clock are here too */
#ifndef CHAO_RACE_RECORD_H
#define CHAO_RACE_RECORD_H
#include <sg_xpt.h>

#define RACE_RECORD_TRACKS 10
#define RACE_EVENTS 13
#define RACE_ENTRANTS 8

struct garden_race_time {
    Uint8 minutes, seconds, frames;
};

/* run_power, fly_swim, alignment and evolution hold the Chao's values times
 * 10000 */
struct garden_race_chao {
    Uint8 type;
    Sint8 eye, mouth, emotion_ball;
    Uint8 hat, costume, medal, color, texture, shiny;
    Sint8 animal_parts[8];
    Sint16 run_power, fly_swim, alignment, evolution;
    char name[8];
    Uint16 stats[7];
};

struct garden_race_record {
    Uint8 valid;
    struct garden_race_time time;
    struct garden_race_chao chao;
};

/* the highest level of event won so far, counting from 0, or -1 for none.
 * an event outside 0 to RACE_EVENTS - 1 returns an undefined value */
int garden_race_best_level(int event);
/* garden_race_record_level raises event's best level to level, if level is
 * one of the levels of event's group. garden_race_advance_record raises it
 * by one, but not past the number of the group's levels. a best at that
 * number names no level. nothing calls garden_race_advance_record */
void garden_race_record_level(int event, int level);
void garden_race_advance_record(int event);
/* copy track's record to *record, or from it. a track outside 0 to
 * RACE_RECORD_TRACKS - 1 copies nothing */
void garden_race_get_record(int track, struct garden_race_record *record);
void garden_race_set_record(int track, const struct garden_race_record *record);
void garden_race_clear_time(void);
/* advance the race clock one 60 Hz frame. it stops at 99 minutes, 59
 * seconds and 59 frames */
void garden_race_tick(void);
void garden_race_get_time(struct garden_race_time *time);

#endif
