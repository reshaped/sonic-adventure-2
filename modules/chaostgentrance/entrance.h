/* the Chao Race entrance: its stage, opening camera, elevator, gate and
 * lamps */
#ifndef CHAOSTGENTRANCE_ENTRANCE_H
#define CHAOSTGENTRANCE_ENTRANCE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/race_record.h"

struct task;

/* the z of the hall's end, where the gate and the lamps stand */
#define HALL_END_Z (-101.5F)

/* the stage task's first step. it loads the stage and makes its camera,
 * elevator, gate and lamps. the task's own step then walks player 0 in, and
 * its destructor releases what this loaded */
void chaostgentrance_step(struct task *task);
/* push the opening camera onto screen 0's stack. the camera flies in to the
 * menu and calls entrance_menu_create once it settles. A skips it to its
 * settling after its first 61 frames. it skips at once on a return from the
 * race, and while game_state.chao_race_menu isn't CHAO_RACE_MENU_NONE.
 * entrance_camera_stop removes
 * screen 0's top mode, which must be this camera */
void entrance_camera_start(void);
void entrance_camera_stop(void);
/* the elevator, its floor below the hall and its doors shut. a start raises
 * the floor with the racers on it and opens the doors. once the floor stops,
 * the racers walk off, and the elevator later calls entrance_gate_open. a
 * start without an elevator does nothing */
void entrance_elevator_create(void);
void entrance_elevator_start(void);
/* the gate at the hall's end. an open changes the area to CHAO_AREA_RACE
 * once the gate has stood open a while. an open without a gate does
 * nothing */
void entrance_gate_create(void);
void entrance_gate_open(void);
/* call entrance_lamps_reset before the stage's first lamp. the first lamp
 * after it makes the lamps' manager, which animates every lamp's glow. the
 * manager has no destructor, so only the reset clears the pointer to it */
void entrance_lamps_reset(void);
void entrance_lamp_create(Float x, Float y, Float z);
/* spawn a Chao for each of garden_race_entrants on the elevator's floor, to
 * wait there. outside RACE_MULTI it first fills the other entrants from the
 * race's opponents. an opponent without a Chao leaves its entrant NULL.
 * entrance_release_racers walks each Chao to its place */
void entrance_racers_create(void);
void entrance_release_racers(void);
/* each entrant's Chao, or NULL for an empty entrant or a failed spawn */
extern struct task *entrance_racers[RACE_ENTRANTS];

/* entrance_label_ids[n] is the first cell of label n in the language's
 * cells. chaostgentrance_step points it at entrance_japanese_ids under
 * TEXT_JAPANESE and at entrance_english_ids under any other language */
extern const Sint32 *entrance_label_ids;
/* the labels the menu reads, by n. CONFIRM_LABEL is the confirm button's,
 * and BACK_LABEL the back button's. from GROUP_LABELS on, they are each race
 * group's button's by its enum garden_race_group, then the exit's. from
 * COURSE_LABELS on, they are each course's by its event. from PAGE_LABELS
 * on, they are each picker page's by its row of entrance_roster */
#define CONFIRM_LABEL 2
#define BACK_LABEL 3
#define GROUP_LABELS 4
#define COURSE_LABELS 11
#define PAGE_LABELS 21
#define ENTRANCE_LABELS 25
extern Sint32 entrance_japanese_ids[ENTRANCE_LABELS];
extern Sint32 entrance_english_ids[ENTRANCE_LABELS];
/* the stage's textures, which chaostgentrance_step loads and the stage's
 * destructor releases. only the language's chars and titles load: the
 * japanese under TEXT_JAPANESE, the english under any other language */
extern NJS_TEXLIST entrance_object_texlist;
extern NJS_TEXLIST entrance_common_texlist;
extern NJS_TEXLIST entrance_japanese_chars;
extern NJS_TEXLIST entrance_japanese_titles;
extern NJS_TEXLIST entrance_english_chars;
extern NJS_TEXLIST entrance_english_titles;
extern NJS_TEXLIST entrance_rival_texlist;
extern NJS_TEXLIST entrance_map_texlist;
extern NJS_TEXLIST entrance_vms_texlist;

#endif
