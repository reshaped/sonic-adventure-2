/* the sound effects, played in slots on the sound driver's MIDI ports, and
 * the loads of their banks.
 *
 * a sound id names a sound in one of sound/lists.h's eight places. bits 12
 * to 15 of the id pick its place. 0 picks place 5, and 8 picks place 0. 1
 * to 4, 6 and 7 pick the place they number, and no other value picks one.
 * the id's low byte indexes the place's list. an id below 0 names no sound.
 * nor does one whose place is null, or whose low byte is past the list's
 * end.
 *
 * a sound plays in one of 53 slots. each call of sound_update is an update
 * of every slot. a play call gives its slot a time. each update counts it
 * down by one, from the update that starts the sound. no update sends a
 * started slot a change while its time is 0. at time 0 an update stops the
 * sound and frees the slot, unless the play call says otherwise.
 *
 * a held sound plays until a stop call. its time is a Sint16 that starts at
 * -1 and counts down, wrapping from -32768 to 32767. so it reaches 0 65,535
 * updates after the start, and the slot gets no change from then on */
#ifndef SOUND_SOUND_H
#define SOUND_SOUND_H
#include <ninja.h>
#include <sg_xpt.h>

struct sound_list;
struct task;

/* the play calls. each plays the sound of id for key, and returns the slot
 * it plays in. it returns -1 where it plays nothing. key is any pointer the
 * caller picks to tell its sounds apart, such as its task. the calls that
 * change or stop a sound find its slot by id and key.
 *
 * a call plays nothing where id names no sound, or where the sound's number
 * is below 0. while game_state.sound_paused is 1, it plays only the paused
 * ids, SOUND_CURSOR_ID to SOUND_PAUSE_ID. otherwise, while game_state.cleared
 * is 1, it plays only a sound with SOUND_AFTER_CLEAR.
 *
 * a sound's own priority, where above 0, takes the place of priority. a call
 * takes a slot of its sound's kind as below, and plays nothing where it
 * finds none:
 * - a sequence plays in one of 5 slots, each on a MIDI port of its own. it
 *   takes a free one, or else cuts a sound of its priority or below
 * - a note plays in one of 32 slots, the channels of two MIDI ports. it can
 *   cut a sound below its priority while another slot is free. it cuts one
 *   of its priority only where every slot plays at its priority or above
 * - the character's sound, where it plays as a note, plays in 15 slots of its
 *   own. it takes the first whose time is 0, as a free slot's is, or else
 *   cuts one still playing. so it always finds a slot. sound_load_end and
 *   the loads that reset the ports pick which id is the character's sound
 *
 * a sound of a paused id that takes a note's slot from another id's sound
 * stops that sound at once.
 *
 * volume runs from -127, the quietest, to SOUND_LOUDEST. every call that
 * finds a slot of id for key, but the sound_start_ calls, stores volume
 * there. unlike sound_set_volume, it doesn't mark the volume for sending. a
 * later sound_set_volume of that volume does nothing.
 *
 * a call with pos plays the sound there, and pos must not be null. within 20
 * of the camera, the sound plays at volume. beyond 20 its volume falls
 * evenly, by 127 at the sound's range, and no lower than -127. the camera is
 * the first one, or the nearer one in a two-player game. without a camera
 * the sound plays at volume.
 *
 * a call with pos plays a sound with SOUND_RANGE only within the sound's
 * range of either camera. without a camera it plays nothing */

/* the loudest volume a play call takes */
#define SOUND_LOUDEST 127
/* the paused ids, the menus' sounds */
#define SOUND_CURSOR_ID 0x8000 /* the cursor moved */
#define SOUND_CHOOSE_ID 0x8001 /* a row chosen */
#define SOUND_PAUSE_ID 0x8002 /* a pause or a message opening or closing */
/* the ring's sound, which sound_play_ring plays */
#define SOUND_RING_ID 0x8005

/* play the sound of id with a time of 120. each call takes a slot anew */
Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume);
/* sound_play at pos. the distance sets the volume once, as the sound
 * starts */
Sint8 sound_play_at(int id, const void *key, Sint8 priority, Sint16 volume,
                    const NJS_POINT3 *pos);
/* play the sound of id until a stop call. a call that finds a slot of id
 * for key gives it a time of -1 again. it also drops whatever earlier calls
 * asked of the slot. so a stop not yet made is undone, and a start not yet
 * made never comes. the slot stays taken until a stop call */
Sint8 sound_play_held(int id, const void *key, Sint8 priority, Sint16 volume);
/* play the sound of id at pos until a stop call. its volume follows its
 * distance at each update. a call that finds a slot of id for key moves it
 * to pos. it gives the slot a time of -1 again */
Sint8 sound_play_held_at(int id, const void *key, Sint8 priority, Sint16 volume,
                         const NJS_POINT3 *pos);
/* play the sound of id with a time of duration + 1. a call that finds a
 * slot of id for key gives it that time again */
Sint8 sound_play_for(int id, const void *key, Sint8 priority, Sint16 volume,
                     Sint16 duration);
/* sound_play_for at pos. its volume follows its distance at each update. a
 * call that finds a slot of id for key moves it to pos */
Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos);
/* play the sound of id twice, each time for duration + 1. the second play
 * starts at the update after the first's time runs out. a call that finds a
 * slot of id for key leaves its time as it is. it asks for one more play
 * after the one playing */
Sint8 sound_play_repeating(int id, const void *key, Sint8 priority,
                           Sint16 volume, Sint16 duration);
/* sound_play_repeating at pos. its volume follows its distance at each
 * update. a call that finds a slot of id for key moves it to pos */
Sint8 sound_play_repeating_at(int id, const void *key, Sint8 priority,
                              Sint16 volume, Sint16 duration,
                              const NJS_POINT3 *pos);
/* play SOUND_RING_ID for player, 0 or 1, with a null key. it has a time of
 * 120 and volume 0. in a one-player game its pan is -64 while
 * game_state.rings[player] is odd, and 64 otherwise. in a two-player game
 * player 0's pan is -64, and player 1's is 64 */
Sint8 sound_play_ring(Sint8 player);
/* play the sound of id as a sequence until its MIDI port stops. it plays at
 * priority 2 whatever its own, and priority isn't read. the port is first
 * tested at the slot's tenth update. a sound that doesn't play as a sequence
 * plays nothing. nor does a call that finds every sequence slot taken at a
 * priority above 2. the call tests neither game_state.sound_paused nor
 * cleared, but sound_update applies both to its slot */
Sint8 sound_play_sequence(int id, const void *key, Sint8 priority,
                          Sint16 volume);

/* the task calls play the sound of id with task as the key. task must not
 * be null, unchecked. a call plays nothing where task has no work, no step,
 * or task_destroy as its step. a call that finds a slot of id for task gives
 * it its time again. an _at call plays the sound at the pos of task's work.
 * the sound follows that pos at each update.
 *
 * each update reads task until the sound ends or a stop call unties the
 * slot. for an _at call it reads the pos of task's work too. so stop a task
 * call's sound before task_destroy can free its task. an update that finds
 * task_destroy as the step before the free stops the sound itself. The port
 * detaches every remaining slot keyed by a task during destruction. */

#ifdef NONMATCHING
/* Called before releasing task or its work. Task-following slots end on
 * their usual stop schedule. Other sounds with this key can finish. */
void sound_detach_task(const struct task *task);
#endif

/* sound_play_held_at at the pos of task's work */
Sint8 sound_play_task_held_at(int id, struct task *task, Sint8 priority,
                              Sint16 volume);
/* sound_play_for with task as the key */
Sint8 sound_play_task_for(int id, struct task *task, Sint8 priority,
                          Sint16 volume, Sint16 duration);
/* sound_play_at_for at the pos of task's work */
Sint8 sound_play_task_at_for(int id, struct task *task, Sint8 priority,
                             Sint16 volume, Sint16 duration);

/* sound_play_for, but a call that finds a slot of id for key returns it
 * untouched */
Sint8 sound_start_for(int id, const void *key, Sint8 priority, Sint16 volume,
                      Sint16 duration);
/* sound_start_for at pos, with no test of SOUND_RANGE. its volume follows
 * its distance at each update. a call that finds a slot of id for key moves
 * it to pos, and changes nothing else */
Sint8 sound_start_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                         Sint16 duration, const NJS_POINT3 *pos);

/* sound_stop_id stops each slot of id, sound_stop_key each slot of key, and
 * sound_stop each slot of id for key. the next update stops a slot. a sound
 * not yet started stops at the update after the one that starts it. a play
 * that sound_play_repeating asked for still plays, and stops at its end. a
 * stop unties a task call's slot from its task at once */
void sound_stop_id(int id);
void sound_stop_key(const void *key);
void sound_stop(int id, const void *key);
/* stop every slot's sound now, and free the slot. a level that
 * sound_hold_fx_level holds is set again */
void sound_stop_all(void);
/* sound_stop_all, then set game_state.sound_paused to 0 */
void sound_stop_all_and_unpause(void);

/* a second key for task's sounds besides task itself: one byte past its
 * address */
#define SOUND_SECOND_KEY(task) ((char *)(task) + 1)
/* the id that matches every slot of key in the calls that change a slot */
#define SOUND_ANY_ID (-1)

/* the calls below change each slot of id for key, or for SOUND_ANY_ID each
 * slot of key. the next update sends the change */

/* set the volume, which then no longer falls with distance. a slot whose
 * volume is volume already is left as it is, and may still fall */
void sound_set_volume(const void *key, int id, Sint8 volume);
/* set the pan, from -127 to 127, with 0 in the middle. it replaces
 * sound_pan_by_pos's pan */
void sound_set_pan(const void *key, int id, Sint8 pan);
/* set the pitch, from -16384 to 16383, with 0 for the sound's own. it
 * replaces sound_pitch_by_pos's pitch */
void sound_set_pitch(const void *key, int id, Sint16 pitch);
/* where pos isn't null, move the sound to pos. then pan it at each update by
 * its angle from the camera's look. the pan is 0 within 25 degrees of ahead
 * or behind, and 127 or -127 at right angles. the angle is measured from the
 * first camera's place, and along its look. in a two-player game the look is
 * the nearer camera's. the pan is 0 while game_state.mono is 1. the first
 * camera must exist, unchecked. the call replaces sound_set_pan's pan */
void sound_pan_by_pos(int id, const void *key, const NJS_POINT3 *pos);
/* where pos isn't null, move the sound to pos. then set its pitch at each
 * update by its distance d from the camera. in a two-player game the camera
 * is the nearer one. the first such update since the start sets 0. each later
 * one adds 10.24 times the fall in d since the one before. but where d falls
 * while the pitch is above 0, the pitch becomes -10.24 times d. it holds while
 * d is beyond 400, and stays within -4096 to 4096. the first camera must exist,
 * unchecked. the call replaces sound_set_pitch's pitch */
void sound_pitch_by_pos(int id, const void *key, const NJS_POINT3 *pos);

/* make an update of every slot. a slot's time counts updates. the game makes
 * one a step. while the event overlay plays a movie, its player makes one a
 * vertical blank.
 *
 * it starts the sounds the play calls asked for. it sends what changed of
 * each volume, pan and pitch. a pan or pitch that follows pos reaches the
 * port one change late.
 *
 * while game_state.sound_paused is 1, it passes over every slot but those of
 * the paused ids. while game_state.cleared isn't 0, a sound without
 * SOUND_AFTER_CLEAR doesn't start. its slot stops once its time is 0, and
 * where that time is -1, it becomes 10 first */
void sound_update(void);

/* set game_state.sound_paused to 1, and pause the music and the voices as
 * sound_pause does. then silence every note's slot but those of the paused
 * ids. pause every sequence's port, those ids' included */
void sound_pause_all(void);
/* set game_state.sound_paused to 0, and resume the music and the voices as
 * sound_resume does. then send every note's slot but those of the paused ids
 * its volume again. a sequence's port that was paused while it
 * played goes on. any other sequence's slot that holds a sound plays it
 * again from the start, unless sound_play_sequence played it */
void sound_resume_all(void);

/* set every MIDI port's effect input level to level, and hold it.
 * sound_stop_all sets a held level again, and sound_release_fx_level ends
 * the hold */
void sound_hold_fx_level(int level);
/* where a level is held, set every port's to 0 and end the hold */
void sound_release_fx_level(void);
/* turn the effect program off, and pass every queued command on to the
 * sound driver. it returns 0 */
int sound_fx_off(void);
/* choose effect program 0 and output 0, and pass every queued command on to
 * the sound driver. it returns 0 */
int sound_fx_on(void);
/* while game_state.mono is 1, set the sound driver to mono at master volume
 * 14. otherwise set it to stereo at 15 */
void sound_apply_mono(void);

/* start the sound. every play, stop and change call needs it first, as do
 * sound_update and most loads. they read the work it allocates. where it
 * can't allocate the work, they read a null one, unchecked.
 *
 * it starts the sound library with the driver manatee.drv, and opens the
 * eight MIDI ports. it loads PLAYER_SONIC's sounds and voices, and puts
 * lists 0 and 1 in their places. then it resets the ports, as the loads
 * below say, and turns the effect program on. where the allocation or
 * sdLibInit fails, it does none of that. in every case it clears
 * game_state.sound_paused, and last sets the driver to mono or stereo as
 * sound_apply_mono does. on the port, a driver it can't read or start stops
 * the game, by port_stop */
void sound_system_init(void);

/* read the file name into WORK_AREA as loader_read_prs_or_file reads it.
 * where the read returns 0, as for a file that won't open, return -1.
 * otherwise stop every sound as sound_stop_all does, and send the file to
 * sound memory with sdMultiUnitDownload. then return 0, whether or not the
 * file gets there in the original. The port returns -1 if the read or
 * the download fails. */
int sound_load_file(const char *name);
/* read the file name into WORK_AREA in whole sectors. send it to sound
 * memory as bank number bank. a file whose first word is
 * SDE_DATA_TYPE_MIDI_DRM_BANK, _MIDI_SEQ_BANK or _SHOT_BANK is a bank of
 * that kind. any other is a SDE_DATA_TYPE_MIDI_PRG_BANK. it returns -1 where
 * the file won't open, and 0 otherwise in the original. The port also
 * returns -1 if the read or the transfer fails. */
int sound_load_bank(int bank, const char *name);
/* store list in the sound lists at place, 0 to 7, unchecked */
void sound_set_list(const struct sound_list *list, Sint8 place);
/* put lists 0 and 7 and character's sounds and voices in their places, as
 * sound_lists_set_character does */
void sound_set_character_lists(Sint8 character);

/* the character's sound of Knuckles and Rouge, and of the mechs. Sonic's
 * and Shadow's is SOUND_RING_ID */
#define SOUND_KNUCKLES_ID 0x1009
#define SOUND_MECH_ID 0x2007

/* the loads. to start over is to free every slot, unset the character's
 * sound and empty every place.
 *
 * to reset the ports is to reset their levels as sdMidiResetAllPrm does, and
 * play the reset sequence on the first three. the reset can wait two
 * seconds. then, on a stage below STAGE_CART_1 other than STAGE_GREEN_HILL,
 * it picks the character's sound by stage_character(0):
 * - SOUND_RING_ID for PLAYER_SONIC and PLAYER_SHADOW
 * - SOUND_KNUCKLES_ID for PLAYER_KNUCKLES and PLAYER_ROUGE
 * - SOUND_MECH_ID for PLAYER_EGGMAN_MECH and PLAYER_TAILS_MECH
 * - none for any other character
 * on any other stage the character's sound stays as it was.
 *
 * sound_load_begin and sound_load_end bracket the loads a caller makes
 * itself. sound_load_begin releases a held level and starts over. it sets
 * game_state.sound_paused to 0 and turns the effect program off. sound_load_end
 * resets the ports, and turns the effect program on */
void sound_load_begin(void);
void sound_load_end(void);

/* each load below frees every slot. the slots' sounds play on, unless the
 * load says it stops them. each sets game_state.sound_paused to 0. each
 * turns the effect program off while it loads, and on after, unless it says
 * otherwise */

/* empty places 4 to 7, and load the stage's file name as sound_load_file
 * loads it. where it loads, put list in place 5, and lists 4 and 7 in
 * theirs. a null name loads nothing */
void sound_load_stage(const char *name, const struct sound_list *list);
/* load name and list as sound_load_stage does. then load the voices of
 * stage_character(0) into place 3, as sound_load_voices(players, ...) loads
 * them. turn the effect program on, and reset the ports. where
 * stage_character(0) is below 0, the call loads no voices and resets
 * nothing. the effect program then stays off */
void sound_load_stage_and_voices(const char *name,
                                 const struct sound_list *list, Sint8 players);
/* stop every sound and release a held level. then load name as
 * sound_load_stage does, but without lists 4 and 7. a name that starts with
 * "e" loads e_test.mlt first. where that loads, list 0 goes in place 0.
 * last, reset the ports */
void sound_load_event(const char *name, const struct sound_list *list);
/* the cart stages' load. stop every sound, release a held level, start over,
 * and load name. where it loads, put list in place 5. lists 0, 1, 4 and 7
 * go in theirs. last, reset the ports */
void sound_load_cart(const char *name, const struct sound_list *list);
/* the menus' load. release a held level, start over and load name. where it
 * loads, put list 0 in place 0 and list in place 5. last, reset the ports */
void sound_load_menus(const char *name, const struct sound_list *list);
/* the Chao stage's load. load name, and put list in place 5 where it loads.
 * every other place stays as it is */
void sound_load_chao(const char *name, const struct sound_list *list);
/* release a held level and start over. then load the sounds and voices of
 * stage_character(0) into places 2 and 3, as sound_load_character does. put
 * lists 0 and 1 in their places, and last, reset the ports. on a stage from
 * STAGE_CART_1 to 79, or from STAGE_CHAO on, the call does nothing */
void sound_load_player_sounds(void);

/* fun_8c059fb2 and fun_8c05bce0 do nothing */
void fun_8c059fb2(void);
void fun_8c05bce0(void);

#endif
