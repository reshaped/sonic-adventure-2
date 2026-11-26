/* the race: its clock, its racers and its gates. the calls below start,
 * time and end it */
#ifndef CART_RACE_H
#define CART_RACE_H
#include "cart/pads.h"
#include "save/save.h"

struct task;
#define CART_RACERS 6
#define CART_LAPS 3
#define CART_GATES 2048
#define CART_GATE_POINTS 4
/* the race's clock counts steps, CART_STEPS_PER_SECOND to a second */
#define CART_STEPS_PER_SECOND 60
/* the minute of a time not yet set, which the HUD draws as dashes */
#define CART_NO_TIME (-1)

struct cart_laps {
    struct save_time lap[CART_LAPS];
    struct task *task;
};
struct cart_race {
    struct save_time time, best;
    Sint32 frames, countdown;
    struct cart_laps player[CART_PLAYERS];
};
struct cart_gate_point {
    Float x, z;
};

extern struct cart_race cart_race;
extern struct task *cart_racers[CART_RACERS];
/* the course's gates in driving order, in world x and z. a gate is four
 * points across the road, from point 0 to point 3. cart_gate_sections holds
 * the course section each stands in */
extern struct cart_gate_point cart_gates[CART_GATES][CART_GATE_POINTS];
extern Sint32 cart_gate_sections[CART_GATES], cart_gate_count;
extern Bool cart_clock_held, cart_finished;
extern Sint32 cart_start_count;
extern Bool cart_result_done, cart_result_open, cart_story_clear;
void cart_start_race(void);
void cart_start_hud(void);
void cart_load_result_textures(void);
void cart_release_result_textures(void);
#ifdef NONMATCHING
/* whether the race's course has a best time in the save. this side tests it
 * before it reads or writes one */
Bool cart_time_record_available(void);
#endif
void cart_clock_start(void);
void cart_clock_stop(void);
/* clear cart_clock_held at the next step, which starts the race's time */
void cart_release_start(void);
void cart_finish(void);
/* end the stage once the race is lost. it restarts while player 0 has a life
 * to lose, and quits otherwise. a call once the countdown has run out ends
 * the stage 181 steps on, after the time-over voice on a story course. any
 * other call ends it 61 steps on */
void cart_time_over(void);
/* whether the gate of task's kart, or of cart_racers[0]'s, lies in a tunnel.
 * each also holds the MIDI ports' effect level, at 0 there and at -127
 * elsewhere */
Bool cart_tunnel_sound(struct task *task);
Bool cart_leader_tunnel_sound(void);
int cart_highest_lap(void);
void cart_show_versus_result(void);
#endif
