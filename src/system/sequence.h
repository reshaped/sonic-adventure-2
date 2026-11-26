/* sequences: the stages and events the game plays one after another */
#ifndef SYSTEM_SEQUENCE_H
#define SYSTEM_SEQUENCE_H
#include <sg_xpt.h>

/* the game_flags entries that hold the sequence and its place in it */
/* the sequence, SEQUENCE_HERO to SEQUENCE_MENU_EVENT, or SEQUENCE_NONE */
#define FLAG_SEQUENCE 32
#define FLAG_SEQUENCE_STEP 30 /* the step the sequence has reached, from 0 */
/* FLAG_SEQUENCE_STEP as sequence_apply_step last found it, applied or not.
 * sequence_start sets it to 0 */
#define FLAG_SEQUENCE_STEP_COPY 31
/* 1 from each step's start until sequence_play_mini_event starts its mini
 * event */
#define FLAG_MINI_EVENT_UNPLAYED 33

/* FLAG_SEQUENCE's value while no sequence runs */
#define SEQUENCE_NONE 0
/* the sequences of the three stories. the title's test menu starts them as
 * FROM HERO, FROM DARK and FROM LAST */
#define SEQUENCE_HERO 1
#define SEQUENCE_DARK 2
#define SEQUENCE_LAST 3
/* the two sequences of the title's trial. SEQUENCE_TAIKEN_EVENT plays an
 * event as the trial's second demo. Start begins SEQUENCE_TAIKEN: the first
 * event, then City Escape as Sonic. sequence_start skips the event once
 * game_bits marks it seen */
#define SEQUENCE_TAIKEN_EVENT 4
#define SEQUENCE_TAIKEN 5
/* the fake title's six choices. each plays one stage as one character */
#define SEQUENCE_CITY_ESCAPE 6 /* as Sonic */
#define SEQUENCE_PRISON_LANE 7 /* as Tails */
#define SEQUENCE_WILD_CANYON 8 /* as Knuckles */
#define SEQUENCE_RADICAL_HIGHWAY 9 /* as Shadow */
#define SEQUENCE_SAND_OCEAN 10 /* as Eggman */
#define SEQUENCE_DRY_LAGOON 11 /* as Rouge */
/* the boss attack's three courses. the first two fight the bosses of the hero
 * story and of the dark story. the third fights both, then STAGE_BOSS_LAST1 and
 * STAGE_BOSS_LAST2 */
#define SEQUENCE_HERO_BOSSES 12
#define SEQUENCE_DARK_BOSSES 13
#define SEQUENCE_ALL_BOSSES 14
/* the event GAME_MODE_MENU plays on the passes of the menus that show no
 * logos */
#define SEQUENCE_MENU_EVENT 15

/* the events of the last events step that sequence_apply_step applied, in
 * order. a step of fewer than SEQUENCE_EVENTS ends its list with NO_EVENT.
 * other steps leave them as they are */
#define SEQUENCE_EVENTS 4
#define NO_EVENT (-1)
extern Sint32 sequence_events[SEQUENCE_EVENTS];

/* start sequence at its first step, or stop the one running with
 * SEQUENCE_NONE. it applies no step. it sets FLAG_SEQUENCE_STEP_COPY to 0
 * and FLAG_MINI_EVENT_UNPLAYED to 1. a sequence other than SEQUENCE_NONE
 * also sets game_state.mission to MISSION_FIRST. SEQUENCE_TAIKEN starts at
 * its second step instead once game_bits marks its event seen */
void sequence_start(int sequence);
/* Apply the current step. The original indexes its table directly. A step
 * sets game_state.mode to the mode that plays it, and the
 * table's end sets GAME_MODE_TO_MENU. an events or ending step sets
 * next_mode to GAME_MODE_SEQUENCE. an events step puts its events in
 * sequence_events. a stage step sets the stage stage_current_id returns and
 * the character stage_character(0) returns, but not player_character(0). it
 * also sets game_state's demo, two_players and split_screen to 0. under
 * SEQUENCE_NONE or a sequence past SEQUENCE_MENU_EVENT, the call applies
 * nothing. The call always copies the step to FLAG_SEQUENCE_STEP_COPY.
 * The port returns to the menu for an out-of-range step. */
void sequence_apply_step(void);
/* move to the sequence's next step without applying it. it sets
 * FLAG_MINI_EVENT_UNPLAYED to 1 */
void sequence_advance(void);
/* load and start the mini event that the game pairs with the current stage
 * and player_character(0). it does nothing where there is none, where no
 * sequence runs, or while FLAG_MINI_EVENT_UNPLAYED is 0. once it starts one,
 * it sets FLAG_MINI_EVENT_UNPLAYED to 0, whatever mini_event_start returns.
 * no mini event may be running, and its me<event>.prs must open, both
 * unchecked */
void sequence_play_mini_event(void);
/* TRUE where sequence_play_mini_event would start a mini event, and FALSE
 * otherwise */
Bool sequence_mini_event_due(void);

#endif
