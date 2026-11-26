/* the Chao race's controller: the race's mode, its entrants' steps, and the
 * displays it makes */
#ifndef CHAOSTGRACE_RACE_H
#define CHAOSTGRACE_RACE_H
#include <sg_xpt.h>

enum {
    RACE_OPENING = 0,
    RACE_READY = 2,
    RACE_RUNNING = 3,
    RACE_GOAL = 4,
    RACE_FADE_OUT = 5,
    RACE_PODIUM = 6,
    RACE_SHOW_RESULTS = 7,
    RACE_FADE_IN = 8,
    RACE_SAVING = 9,
    RACE_EMBLEM = 10,
    RACE_LEAVE = 11,
    RACE_CLOSED = 12
};

enum race_toy_kind { RACE_PERSONAL_TOY, RACE_GARDEN_TOY };
/* the frames of a beat of each race group's music, which times the opening,
 * the entrants' introductions and the start */
#define RACE_BEGINNER_BEAT 62
#define RACE_JEWEL_BEAT 61
#define RACE_CHALLENGE_BEAT 59
#define RACE_DARK_BEAT 60
#define RACE_HERO_BEAT 57
#define RACE_MULTI_BEAT 65
/* the frames from the podium to the fade-in of the result's banner and of
 * the toy the race awards */
#define RACE_AWARD_DELAY 570

struct task;

int race_mode(void);
void race_water_create(void);
void race_present_toy(int toy, int kind);

void race_set_mode(int mode);
void race_release_entrants(void);
int race_shown_place(int entrant);
struct task *race_entrant(Uint32 entrant);
void race_start_cheer(Uint32 entrant);
void race_end_cheer(Uint32 entrant);
Bool race_is_cheering(struct task *chao);
int race_fall_in_pit(struct task *chao);
int race_run(struct task *chao);
int race_cross_finish(struct task *chao);
int race_end_finish(struct task *chao);
int race_victory_pose(struct task *chao);
extern int race_camera_cheer_target;
int race_defeat_pose(struct task *chao);
void race_stamina_create(Uint8 entrant);
void race_multiplayer_stamina_create(Uint8 entrant);
void race_clock_create(void);
void race_rank_create(Uint8 entrant);
void race_multiplayer_rank_create(Uint8 entrant);

#endif
