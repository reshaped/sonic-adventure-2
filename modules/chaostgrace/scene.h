/* the data the race's controller works from, and the callbacks
 * chaostgrace_step gives its task */
#ifndef CHAOSTGRACE_SCENE_H
#define CHAOSTGRACE_SCENE_H
#include <ninja.h>
#include "chao/race.h"
#include "chao/race_record.h"
#include "draw/light.h"
#include "system/texture.h"
struct task;
struct set_head;
struct land_table;
/* the head of a race land's file as it decodes: its land, and the glass
 * race_draw_glass draws */
struct race_land_file {
    struct land_table *land;
    NJS_CNK_OBJECT *glass;
};
/* each land's SET file, by garden_race_land */
struct race_land_files {
    const char *name[RACE_LANDS];
};
/* each track's part of the name of its SET file */
struct race_track_files {
    const char *name[RACE_RECORD_TRACKS];
};
/* where each place stands for the results, from the first */
struct race_podium {
    NJS_POINT3 position[RACE_ENTRANTS];
};
extern const struct race_podium race_podium;
extern const struct race_land_files race_land_sets;
extern const struct race_track_files race_track_sets;
/* the SET layout chaostgrace_step reads, which race_controller_close frees */
extern struct set_head *race_layout;
extern struct light race_key_light, race_fill_light;
extern NJS_TEXLIST **race_effect_groups[];
extern struct texture_pvm race_effect_textures[];
extern NJS_TEXLIST race_water_texlist;
extern const char race_award_music[], race_beginner_music[], race_jewel_music[];
extern const char race_challenge_music[], race_dark_music[], race_hero_music[];
/* each entrant's place on a starting grid, before the grid's yaw turns it.
 * the beginner swim, fly and power tracks start on the narrow grid */
extern NJS_VECTOR race_narrow_offsets[RACE_ENTRANTS],
    race_wide_offsets[RACE_ENTRANTS];
void race_controller_step(struct task *task);
void race_controller_draw(struct task *task);
void race_controller_close(struct task *task);
void race_glass_step(struct task *task);
void race_draw_glass(struct task *task);
#endif
