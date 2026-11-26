/* the race's controller and its standings. chaostgrace_step loads the
 * course and places the entrants. it then makes its task the controller,
 * which steps through race_mode's modes. the standings place each finisher
 * by its order across the line, and the others by course progress */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "camera/camera.h"
#include "chao/behavior.h"
#include "chao/course.h"
#include "chao/face.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/ground.h"
#include "chao/icon.h"
#include "chao/life.h"
#include "chao/messages.h"
#include "chao/model.h"
#include "chao/race.h"
#include "chao/race_chao.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/small_toys.h"
#include "chao/toy_art.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chaostgrace/banner_draw.h"
#include "chaostgrace/banners.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/garden.h"
#include "chaostgrace/grid.h"
#include "chaostgrace/map.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "chaostgrace/racer.h"
#include "chaostgrace/scavenger.h"
#include "chaostgrace/scene.h"
#include "chaostgrace/scene_files.h"
#include "chaostgrace/standings.h"
#include "collision/world.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "object/mover.h"
#include "object/set.h"
#include "player/player.h"
#include "save/save.h"
#include "shared/emblem.h"
#include "shared/object_light.h"
#include "sound/adx.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

void race_record_finish(struct task *chao)
{
    struct race_controller_state *state = race_controller->extra.p;
    int mode = state->mode;
    int entrant;

    /* @hack redundant: a test of state that never runs */
    if (0) {
        if (state) {
        }
    }
    if (mode >= RACE_GOAL || !race_clock_running) {
        return;
    }
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        if (race_participants[entrant].chao == chao) {
            race_participants[entrant].finished = TRUE;
            break;
        }
    }
    /* @bug memory: a finisher that is no entrant takes its place one past
     * race_participants' end, at 0x0c9c00a8 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a finisher that is no entrant
     * takes no place */
    if (entrant == RACE_ENTRANTS) {
        return;
    }
#endif
    if (race_participants[0].chao == chao) {
        garden_race_get_time(&race_player_time);
        if (garden_race_group() != RACE_MULTI) {
            race_clock_running = FALSE;
        }
    }
    if (!race_finish_count) {
        garden_race_get_time(&race_fastest_time);
    }
    race_finish_count++;
    race_participants[entrant].place = race_finish_count;
}

struct task *race_chao_in_place(int place)
{
    if (place >= 1 && place <= RACE_ENTRANTS) {
        return race_finish_order[place - 1];
    }
    return NULL;
}

void race_reset_finish_order(void)
{
    int entrant;

    race_finish_count = 0;
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        race_finish_order[entrant] = NULL;
    }
}

struct task *race_entrant(Uint32 entrant)
{
    return race_participants[entrant].chao;
}

void race_start_cheer(Uint32 entrant)
{
    struct task *chao = race_participants[entrant].chao;
    if (chao) {
        struct chao_work *work = CHAO_WORK(chao);
        work->flags |= RACE_CHEERED;
    }
}

void race_end_cheer(Uint32 entrant)
{
    struct task *chao = race_participants[entrant].chao;
    if (chao) {
        struct chao_work *work = CHAO_WORK(chao);
        work->flags &= ~RACE_CHEERED;
        chao_restore_eyes(chao);
        chao_restore_mouth(chao);
    }
}

Bool race_is_cheering(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    if (work->flags & RACE_CHEERED) {
        return TRUE;
    }
    return FALSE;
}

/* a place the rank shows holds PLACE_HOLD frames before it follows the
 * standings */
#define PLACE_HOLD 20

void race_update_standings(void)
{
    int finished = 0;
    int entrant;
    int other;

    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        if (race_participants[entrant].chao &&
            race_participants[entrant].finished) {
            finished++;
        }
    }
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        if (!race_participants[entrant].finished) {
            race_participants[entrant].place = finished + 1;
        }
    }
    for (entrant = 0; entrant < RACE_ENTRANTS - 1; entrant++) {
        if (race_participants[entrant].chao) {
            if (!race_participants[entrant].finished) {
                for (other = entrant + 1; other < RACE_ENTRANTS; other++) {
                    if (race_participants[other].chao) {
                        if (!race_participants[other].finished) {
                            if (CHAO_WORK(race_participants[entrant].chao)
                                    ->race_progress <
                                CHAO_WORK(race_participants[other].chao)
                                    ->race_progress) {
                                race_participants[entrant].place++;
                            } else {
                                race_participants[other].place++;
                            }
                        }
                    }
                }
            }
        }
    }
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        if (race_participants[entrant].display_timer > 0) {
            --race_participants[entrant].display_timer;
        }
        if (race_participants[entrant].previous_place !=
            race_participants[entrant].place) {
            if (race_participants[entrant].display_timer <= 0) {
                race_participants[entrant].previous_place =
                    race_participants[entrant].place;
                race_participants[entrant].display_timer = PLACE_HOLD;
            }
        }
    }
}

int race_place(int entrant)
{
    return race_participants[entrant].place;
}

int race_shown_place(int entrant)
{
    return race_participants[entrant].previous_place;
}

#pragma inline(entrant_at)
static struct task *entrant_at(Uint32 index)
{
    return race_participants[index].chao;
}

void race_release_entrants(void)
{
    int entrant;
    struct task *chao;
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++) {
        chao = entrant_at(entrant);
        if (chao) {
            chao_set_behavior(chao, race_run);
            CHAO_WORK(chao)->flags |= CHAO_COURSE_CONSTRAIN;
            CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
        }
    }
}

void race_set_mode(int mode)
{
    struct race_controller_state *state = race_controller->extra.p;
    state->mode = mode;
}

int race_mode(void)
{
    struct race_controller_state *state = race_controller->extra.p;
    return state->mode;
}

#pragma inline(controller_state)
static struct race_controller_state *controller_state(struct task *task)
{
    return task->extra.p;
}

/* @hack spelling: the land file and the land's build buffer as symbols,
 * where their areas' addresses, CHAO_RACE_LAND_AREA and CHAO_LAND_BUILD_AREA,
 * would do */
#ifdef NONMATCHING
/* the land file and the build buffer by their addresses, the plain form the
 * hack tag names. this side's race_start_course makes garden_land_file.h's
 * checks */
#include "shared/garden_land_file.h"
#include "system/areas.h"
#define RACE_LAND_FILE (*(struct race_land_file *)CHAO_RACE_LAND_AREA)
#define LAND_BUILD_AREA CHAO_LAND_BUILD_AREA
#else
extern struct race_land_file race_land_file;
extern Uint8 race_land_buffer[];
#define RACE_LAND_FILE race_land_file
#define LAND_BUILD_AREA race_land_buffer
#endif

/* the race's ten emblems are save_data.chao_emblems[0] to [9].
 * RACE_EMBLEM(name, index) is the emblem of that name, at index */
#ifdef NONMATCHING
/* the plain form the hack tag names: an element of save_data.chao_emblems */
#define RACE_EMBLEM(name, index) save_data.chao_emblems[index]
#else
/* @hack type: each emblem a one-byte array of its own, where an element of
 * save_data.chao_emblems would do */
extern Sint8 race_emblem_beginner[1], race_emblem_beginner_all[1];
extern Sint8 race_emblem_jewel_four[1], race_emblem_jewel_six[1],
    race_emblem_jewel_all[1];
extern Sint8 race_emblem_challenge_four[1], race_emblem_challenge_eight[1],
    race_emblem_challenge_all[1];
extern Sint8 race_emblem_dark[1], race_emblem_hero[1];
#define RACE_EMBLEM(name, index) (race_emblem_##name[0])

/* camera_hold, declared at its address as the call below makes it */
extern void camera_hold_as_called(void);
#endif

/* the controller's near radius for world_set_near_radius: START_NEAR_RADIUS
 * once the land starts, and RUN_NEAR_RADIUS on each step */
#define START_NEAR_RADIUS 10000.0F
#define RUN_NEAR_RADIUS 1000000.0F
/* the fewest slots the race's SET layout holds, its head included */
#define LAYOUT_SLOTS 1024
/* the frames the controller holds RACE_GOAL */
#define GOAL_HOLD 200
/* the first PODIUM_PLACES finishers take the victory pose */
#define PODIUM_PLACES 3
/* the results last RESULTS_FRAMES. A ends them once fewer than SKIP_FRAMES
 * are left. the controller waits at most SAVE_TIMEOUT frames for the
 * garden's save */
#define RESULTS_FRAMES 1000
#define SKIP_FRAMES 300
#define SAVE_TIMEOUT 7200
/* the medals a winning Chao wears. the jewel courses' medals run from
 * MEDAL_JEWEL, one for each course. the beginner courses', the challenge's,
 * the dark race's and the hero race's follow */
enum race_medal {
    MEDAL_JEWEL = 1,
    MEDAL_BEGINNER = 7,
    MEDAL_CHALLENGE = 8,
    MEDAL_DARK = 9,
    MEDAL_HERO = 10
};
enum intro_mode { INTRO_START, INTRO_NAMES, INTRO_DONE };
/* work->tick is the frames of a beat of the group's music. work->timer counts
 * the frames of the introductions. entrant n's name shows NAME_LEAD frames
 * before beat NAME_BEAT + n. the start banner shows a quarter beat before beat
 * START_BEAT. in a beginner or jewel race it waits for LONG_START_BEAT */
#define NAME_BEAT 6
#define NAME_LEAD 6
#define START_BEAT 15
#define LONG_START_BEAT 16

#define FRAMES_PER_MINUTE 3600
#define FRAME_TOTAL(time)                                                      \
    (((time).minutes * FRAMES_PER_MINUTE) +                                    \
     ((time).seconds * STEPS_PER_SECOND) + (time).frames)
/* @hack spelling: the body a bare block, where do { } while (0) would do */
#define SAVE_RACE_ORDER()                                                      \
    {                                                                          \
        for (place = 0; place < RACE_ENTRANTS; place++) {                      \
            if (race_participants[place].chao) {                               \
                race_finish_order[race_participants[place].place - 1] =        \
                    race_participants[place].chao;                             \
            }                                                                  \
        }                                                                      \
        race_goal_banner_create();                                             \
        state->mode = RACE_GOAL;                                               \
        work->timer = GOAL_HOLD;                                               \
    }
/* the events of the beginner and jewel courses, by course */
#define BEGINNER_EVENT(course) (course)
#define JEWEL_EVENT(course) (RACE_BEGINNER_COURSES + (course))
/* whether the best level won in event, counted from 0, is level or above */
#define BEST_AT_LEAST(event, level) (garden_race_best_level(event) >= (level))
#define BEGINNER_COMPLETE(level)                                               \
    (BEST_AT_LEAST(BEGINNER_EVENT(0), level) &&                                \
     BEST_AT_LEAST(BEGINNER_EVENT(1), level) &&                                \
     BEST_AT_LEAST(BEGINNER_EVENT(2), level) &&                                \
     BEST_AT_LEAST(BEGINNER_EVENT(3), level))
#define JEWEL_FOUR_COMPLETE(level)                                             \
    (BEST_AT_LEAST(JEWEL_EVENT(0), level) &&                                   \
     BEST_AT_LEAST(JEWEL_EVENT(1), level) &&                                   \
     BEST_AT_LEAST(JEWEL_EVENT(2), level) &&                                   \
     BEST_AT_LEAST(JEWEL_EVENT(3), level))
/* the challenge's emblems come at its fourth and eighth levels won */
#define CHALLENGE_FOUR_LEVEL 3
#define CHALLENGE_EIGHT_LEVEL 7
/* @hack spelling: the body a bare if, where do { } while (0) would do */
#define AWARD_IF(flag, condition)                                              \
    if (!(flag)) {                                                             \
        if (condition) {                                                       \
            (flag) = TRUE;                                                     \
            race_awarded_emblem = TRUE;                                        \
        }                                                                      \
    }

void race_controller_step(struct task *task)
{
    struct task *chao = task;
    struct task_work *work = chao->work;
    struct race_controller_state *state = controller_state(chao);
    int place;
    int save_status;

    world_set_near_radius(RUN_NEAR_RADIUS);
    switch (state->mode) {
    case RACE_OPENING:
        race_awarded_emblem = FALSE;
        break;
    case 1: /* a mode no code sets */
        break;
    case RACE_RUNNING:
        race_update_standings();
        switch (garden_race_group()) {
        case RACE_CHALLENGE:
        case RACE_DARK_COURSES:
        case RACE_HERO_COURSES:
        case RACE_BEGINNER:
        case RACE_JEWEL:
            if (race_participants[0].finished) {
                SAVE_RACE_ORDER();
            }
            break;
        case RACE_MULTI:
        default:
            if (race_finish_count >= race_entrant_count) {
                SAVE_RACE_ORDER();
            }
        }
        if (state->mode < RACE_GOAL && race_clock_running) {
            garden_race_tick();
        }
        break;
    case RACE_GOAL:
        if (!work->timer--) {
            state->mode = RACE_FADE_OUT;
            pause_disable();
        }
        break;
    case RACE_FADE_OUT:
        if (fade_to_color_step(255, 255, 255, 255)) {
            pause_enable();
            state->mode = RACE_PODIUM;
        }
        break;
    case RACE_PODIUM: {
        const struct garden_race_course *course = garden_race_course();
        NJS_POINT3 podium[RACE_ENTRANTS];
        struct garden_race_record previous_record;
        struct garden_race_record new_record;
        struct chao_parameters *parameters;
        struct garden_race_time *record;
        Uint32 current;
        Uint32 previous;
        struct chao_work *racer;
        /* @hack type: podium an array, filled through a cast, where a
         * struct race_podium local would do */
        *(struct race_podium *)podium = race_podium;
        switch (garden_race_group()) {
        case RACE_BEGINNER:
        case RACE_JEWEL:
            if (race_finish_order[0] == race_participants[0].chao) {
                if (course->award != RACE_NO_AWARD) {
                    if (!chao_has_small_toy(race_participants[0].chao,
                                            course->award)) {
                        race_present_toy(course->award, RACE_PERSONAL_TOY);
                        chao_add_small_toy(race_participants[0].chao,
                                           course->award);
                    }
                }
                race_win_banner_create();
                garden_race_record_level(garden_race_event(),
                                         garden_race_level());
                switch (garden_race_event()) {
                case BEGINNER_EVENT(0):
                case BEGINNER_EVENT(1):
                case BEGINNER_EVENT(2):
                case BEGINNER_EVENT(3):
                    if (BEGINNER_COMPLETE(RACE_BEGINNER_LEVELS - 1) &&
                        garden_race_level() == RACE_BEGINNER_LEVELS - 1) {
                        CHAO_WORK(race_participants[0].chao)
                            ->profile.parameters.medal = MEDAL_BEGINNER;
                    }
                    break;
                case JEWEL_EVENT(0):
                case JEWEL_EVENT(1):
                case JEWEL_EVENT(2):
                case JEWEL_EVENT(3):
                case JEWEL_EVENT(4):
                case JEWEL_EVENT(5):
                    if (garden_race_level() == RACE_JEWEL_LEVELS - 1) {
                        CHAO_WORK(race_participants[0].chao)
                            ->profile.parameters.medal =
                            garden_race_event() -
                            (RACE_BEGINNER_COURSES - MEDAL_JEWEL);
                    }
                    break;
                }
                AWARD_IF(RACE_EMBLEM(beginner, 0), BEGINNER_COMPLETE(0));
                AWARD_IF(RACE_EMBLEM(beginner_all, 1),
                         BEGINNER_COMPLETE(RACE_BEGINNER_LEVELS - 1));
                AWARD_IF(RACE_EMBLEM(jewel_four, 2), JEWEL_FOUR_COMPLETE(0));
                AWARD_IF(RACE_EMBLEM(jewel_six, 3),
                         JEWEL_FOUR_COMPLETE(0) &&
                             BEST_AT_LEAST(JEWEL_EVENT(4), 0) &&
                             BEST_AT_LEAST(JEWEL_EVENT(5), 0));
                AWARD_IF(
                    RACE_EMBLEM(jewel_all, 4),
                    JEWEL_FOUR_COMPLETE(RACE_JEWEL_LEVELS - 1) &&
                        BEST_AT_LEAST(JEWEL_EVENT(4), RACE_JEWEL_LEVELS - 1) &&
                        BEST_AT_LEAST(JEWEL_EVENT(5), RACE_JEWEL_LEVELS - 1));
            } else {
                race_retry_banner_create();
            }
            parameters =
                &CHAO_WORK(race_participants[0].chao)->profile.parameters;
            record = &parameters->personal_records[garden_race_event()];
            if (!record->minutes && !record->seconds && !record->frames) {
                njMemCopy(record, &race_player_time, sizeof(*record));
            } else {
                Uint32 now = FRAME_TOTAL(race_player_time);
                Uint32 best = FRAME_TOTAL(*record);
                /* @hack redundant: a test of best that never runs */
                if (0) {
                    if (best) {
                    }
                }
                if (now < best) {
                    njMemCopy(record, &race_player_time, sizeof(*record));
                }
            }
            garden_race_get_record(garden_race_event(), &previous_record);
            current = FRAME_TOTAL(race_fastest_time);
            previous = FRAME_TOTAL(previous_record.time);
            if (!previous || !previous_record.valid || current < previous) {
                njMemCopy(&new_record.time, &race_fastest_time,
                          sizeof(new_record.time));
                new_record.valid = TRUE;
                chao_pack_racer(
                    &new_record.chao,
                    &CHAO_WORK(race_finish_order[0])->profile.parameters);
                garden_race_set_record(garden_race_event(), &new_record);
            }
            break;
        case RACE_CHALLENGE:
        case RACE_DARK_COURSES:
        case RACE_HERO_COURSES:
            if (race_finish_order[0] == race_participants[0].chao) {
                if (course->award != RACE_NO_AWARD) {
                    if (!garden_has_award(course->award)) {
                        race_present_toy(course->award, RACE_GARDEN_TOY);
                        garden_add_award(course->award);
                    }
                }
                race_win_banner_create();
                garden_race_record_level(garden_race_event(),
                                         garden_race_level());
                switch (garden_race_event()) {
                case RACE_CHALLENGE_EVENT:
                    if (garden_race_level() == RACE_CHALLENGE_LEVELS - 1) {
                        CHAO_WORK(race_participants[0].chao)
                            ->profile.parameters.medal = MEDAL_CHALLENGE;
                    }
                    break;
                case RACE_DARK_EVENT:
                    if (garden_race_level() == RACE_ALIGNMENT_LEVELS - 1) {
                        CHAO_WORK(race_participants[0].chao)
                            ->profile.parameters.medal = MEDAL_DARK;
                    }
                    break;
                case RACE_HERO_EVENT:
                    if (garden_race_level() == RACE_ALIGNMENT_LEVELS - 1) {
                        CHAO_WORK(race_participants[0].chao)
                            ->profile.parameters.medal = MEDAL_HERO;
                    }
                    break;
                }
                AWARD_IF(
                    RACE_EMBLEM(challenge_four, 5),
                    BEST_AT_LEAST(RACE_CHALLENGE_EVENT, CHALLENGE_FOUR_LEVEL));
                AWARD_IF(
                    RACE_EMBLEM(challenge_eight, 6),
                    BEST_AT_LEAST(RACE_CHALLENGE_EVENT, CHALLENGE_EIGHT_LEVEL));
                AWARD_IF(RACE_EMBLEM(challenge_all, 7),
                         BEST_AT_LEAST(RACE_CHALLENGE_EVENT,
                                       RACE_CHALLENGE_LEVELS - 1));
                AWARD_IF(
                    RACE_EMBLEM(dark, 8),
                    BEST_AT_LEAST(RACE_DARK_EVENT, RACE_ALIGNMENT_LEVELS - 1));
                AWARD_IF(
                    RACE_EMBLEM(hero, 9),
                    BEST_AT_LEAST(RACE_HERO_EVENT, RACE_ALIGNMENT_LEVELS - 1));
            } else {
                race_retry_banner_create();
            }
            break;
        case RACE_MULTI:
            break;
        }
        for (place = 0; place < RACE_ENTRANTS; place++) {
            chao = race_finish_order[place];
            if (chao) {
                racer = CHAO_WORK(chao);
                if (place >= 0 && place <= PODIUM_PLACES - 1) {
                    chao_set_behavior(chao, race_victory_pose);
                    racer->base.ang.y = QUARTER_TURN;
                } else {
                    chao_set_behavior(chao, race_defeat_pose);
                    racer->base.ang.y = -QUARTER_TURN;
                }
                CHAO_WORK(chao)->flags &= ~CHAO_COURSE_CONSTRAIN;
                CHAO_WORK(chao)->flags &= ~CHAO_RACE_STAGE_COLLISION;
                CHAO_WORK(chao)->flags &= ~(Uint32)CHAO_MOVES;
                racer->base.pos = podium[place];
                mover_clear_spd(chao);
                mover_clear_acc(chao);
                chao_icon_place(chao);
            }
        }
        camera_active_count = 1;
        race_set_camera_step(0, race_camera_podium);
        work->timer = RESULTS_FRAMES;
        music_play_name(race_award_music);
        state->mode = RACE_SHOW_RESULTS;
    } break;
    case RACE_SHOW_RESULTS: {
        struct task *entrant;
        Uint32 index;
        for (index = 0; index < RACE_ENTRANTS; index++) {
            entrant = race_finish_order[index];
            if (entrant) {
                chao_icon_place(entrant);
                CHAO_WORK(entrant)->flags &= ~CHAO_COURSE_CONSTRAIN;
                CHAO_WORK(entrant)->flags |= CHAO_RACE_STAGE_COLLISION;
                CHAO_WORK(entrant)->flags |= CHAO_MOVES;
            }
            /* @hack redundant: a test of chao that never runs */
            if (0) {
                if (chao) {
                }
            }
        }
        state->mode++;
        /* fall through */
    }
    case RACE_FADE_IN:
        if (fade_to_color_step(0, 0, 0, 0)) {
            state->mode = RACE_SAVING;
            switch (garden_race_group()) {
            case RACE_MULTI:
                break;
            default:
                garden_file_write();
                race_save_finished = FALSE;
                race_save_frames = 0;
                break;
            }
        }
        break;
    case RACE_SAVING:
        if (garden_race_group() == RACE_MULTI) {
            race_save_finished = TRUE;
        } else {
            save_status = garden_file_state();
            switch (save_status) {
            case GARDEN_FILE_BUSY:
                if (race_save_frames++ > SAVE_TIMEOUT) {
                    race_save_finished = TRUE;
                }
                break;
            case GARDEN_FILE_OK:
                race_save_finished = TRUE;
                break;
            case GARDEN_FILE_ABSENT:
            case GARDEN_FILE_ERROR:
                race_save_finished = TRUE;
                break;
            }
        }
        if (work->timer > 0) {
            work->timer--;
        }
        if (race_save_finished &&
            (!work->timer || (work->timer < SKIP_FRAMES &&
                              (player_pads[0]->press & PDD_DGT_TA)))) {
            njReleaseTexture(&garden_toy_texlist);
            njReleaseTexture(&race_fruit_texlist);
            njReleaseTexture(&race_water_texlist);
            if (race_awarded_emblem) {
                njReleaseTexture(&garden_animal_texlist);
                save_write();
                emblem_start();
                /* @bug mismatched-call: the call at 0x0c9c1246 passes
                 * camera_hold no stage, which it takes and never reads */
#ifdef NONMATCHING
                /* this side keeps the tag's result, as camera_hold reads no
                 * stage. it calls the function by its own name. only
                 * symbols.txt places the second name, which this side's objects
                 * don't define */
                camera_hold(0);
#else
                camera_hold_as_called();
#endif
                game_state_steps = 0;
                state->mode = RACE_EMBLEM;
            } else {
                state->mode = RACE_LEAVE;
            }
        }
        break;
    case RACE_EMBLEM:
        if (!emblem_screen_is_open()) {
            state->mode = RACE_LEAVE;
        }
        break;
    case RACE_LEAVE:
        chao_change_area(CHAO_AREA_ENTRANCE);
        camera_release();
        state->mode = RACE_CLOSED;
        break;
    case RACE_READY:
    case RACE_CLOSED:
        break;
    }
    switch (work->mode) {
    case INTRO_START:
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            music_play_name(race_beginner_music);
            work->tick = RACE_BEGINNER_BEAT;
            break;
        case RACE_JEWEL:
            music_play_name(race_jewel_music);
            work->tick = RACE_JEWEL_BEAT;
            break;
        case RACE_CHALLENGE:
            music_play_name(race_challenge_music);
            work->tick = RACE_CHALLENGE_BEAT;
            break;
        case RACE_DARK_COURSES:
            music_play_name(race_dark_music);
            work->tick = RACE_DARK_BEAT;
            break;
        case RACE_HERO_COURSES:
            music_play_name(race_hero_music);
            work->tick = RACE_HERO_BEAT;
            break;
        case RACE_MULTI:
            music_play_name(race_jewel_music);
            work->tick = RACE_MULTI_BEAT;
            break;
        }
        work->mode++;
        work->timer = 0;
        break;
    case INTRO_NAMES: {
        Uint32 beat = work->tick;
        Uint32 first = (beat * NAME_BEAT) - NAME_LEAD;
        work->timer++;
/* @hack spelling: the body a bare if, where do { } while (0) would do */
#define INTRODUCE(entrant, time)                                               \
    if (work->timer == (time)) {                                               \
        if (race_participants[entrant].chao) {                                 \
            garden_create_name_label(race_participants[entrant].chao);         \
        }                                                                      \
    }
        INTRODUCE(0, first);
        INTRODUCE(1, first + beat);
        INTRODUCE(2, first + (beat * 2));
        INTRODUCE(3, first + (beat * 3));
        INTRODUCE(4, first + (beat * 4));
        INTRODUCE(5, first + (beat * 5));
        INTRODUCE(6, first + (beat * 6));
        INTRODUCE(7, first + (beat * 7));
#undef INTRODUCE
        switch (garden_race_group()) {
        case RACE_BEGINNER:
        case RACE_JEWEL:
            if (work->timer == (beat * LONG_START_BEAT) - (beat / 4)) {
                race_start_banner_create();
                work->mode++;
            }
            break;
        case RACE_CHALLENGE:
        case RACE_DARK_COURSES:
        case RACE_HERO_COURSES:
        case RACE_MULTI:
            if (work->timer == (beat * START_BEAT) - (beat / 4)) {
                race_start_banner_create();
                work->mode++;
            }
            break;
        default:
            if (work->timer == beat * START_BEAT) {
                race_start_banner_create();
                work->mode++;
            }
            break;
        }
    } break;
    case INTRO_DONE:
        break;
    }
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of chao_icon_place that never runs */
    if (0) {
        /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
        if (chao_icon_place) {
        }
    }
#endif
}

void race_draw_glass(struct task *task)
{
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njSetTexture(RACE_LAND_FILE.land->texlist);
    njCnkSimpleDrawObject(RACE_LAND_FILE.glass);
}

void race_glass_step(struct task *task)
{
}

void race_controller_draw(struct task *task)
{
}

void race_controller_close(struct task *task)
{
    player_resume(0);
    garden_messages_free();
    texture_release_shared(race_effect_textures, race_effect_groups);
    njReleaseTexture(&race_hud_texlist);
    if (!race_awarded_emblem) {
        njReleaseTexture(&garden_animal_texlist);
    }
    race_awarded_emblem = FALSE;
    switch (garden_race_course()->land) {
    case RACE_LAND_NEUTRAL:
        njReleaseTexture(&race_neutral_texlist);
        break;
    case RACE_LAND_HERO:
        njReleaseTexture(&race_hero_texlist);
        break;
    case RACE_LAND_DARK:
        njReleaseTexture(&race_dark_texlist);
        break;
    }
    set_end();
    if (race_layout) {
        loader_free(race_layout);
        race_layout = NULL;
    }
    world_land_stop();
    race_controller = NULL;
}

/* entrant 0 starts in FIRST_LANE, and each next one LANE_STEP nearer the
 * left. a racer's stamina is BASE_STAMINA, and STAMINA_POINT more for each
 * point of its stamina stat */
#define FIRST_LANE 0.89F
#define LANE_STEP 0.11F
#define BASE_STAMINA 10000
#define STAMINA_POINT 5

static void place_entrants(void)
{
    int track = garden_race_course()->track;
    NJS_VECTOR *offset;
    int entrant;
    struct task *chao;
    NJS_POINT3 position;
    NJS_VECTOR rotated;
    struct chao_parameters *parameters;

    switch (track) {
    case TRACK_BEG_SWIM:
    case TRACK_BEG_FLY:
    case TRACK_BEG_POWER:
        offset = race_narrow_offsets;
        break;
    case TRACK_BEG_RUN:
    case TRACK_JWL_SWIM:
    case TRACK_JWL_FLY:
    case TRACK_JWL_RUN:
    case TRACK_JWL_POWER:
    case TRACK_JWL_IL:
    case TRACK_JWL_LONG:
        offset = race_wide_offsets;
        break;
        /* @bug uninitialized: a track the switch doesn't name leaves offset
         * unset, which the loop at 0x0c9c16c6 reads */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it places no entrants on a
     * track the switch doesn't name */
    default:
        return;
#endif
    }
    race_entrant_count = 0;
    /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
    for (entrant = 0; entrant < RACE_ENTRANTS; entrant++, offset++) {
        if (garden_race_entrants[entrant]) {
            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, race_grids[track].yaw);
            njCalcVector(NULL, offset, &rotated);
            njPopMatrixEx();
            position = race_grids[track].pos;
            position.x += rotated.x;
            position.y += rotated.y;
            position.z += rotated.z;
            chao = race_enter_saved_chao(garden_race_entrants[entrant],
                                         &position, race_grids[track].yaw);
#ifdef NONMATCHING
            /* this side counts an entrant that garden_spawn_chao can't make as
             * finished last. the matching side writes through the null */
            if (!chao) {
                race_participants[entrant].chao = NULL;
                race_participants[entrant].place =
                    race_participants[entrant].previous_place = RACE_ENTRANTS;
                race_participants[entrant].finished = TRUE;
                continue;
            }
#endif
            chao_set_ground_mode(chao, CHAO_GROUND_RACE);
            CHAO_WORK(chao)->flags &= ~CHAO_BODY_SPRINGS;
            CHAO_WORK(chao)->flags &= ~CHAO_AGES;
            CHAO_WORK(chao)->lane = FIRST_LANE - (entrant * LANE_STEP);
            race_participants[entrant].chao = chao;
            race_participants[entrant].place =
                race_participants[entrant].previous_place = RACE_ENTRANTS;
            race_participants[entrant].finished = FALSE;
            parameters = &CHAO_WORK(chao)->profile.parameters;
            parameters->race_stamina =
                BASE_STAMINA +
                (parameters->stats[CHAO_STAMINA] * STAMINA_POINT);
            parameters->race_target_stamina = parameters->race_stamina;
            switch (track) {
            case TRACK_BEG_POWER:
            case TRACK_JWL_IL:
            case TRACK_JWL_LONG:
                CHAO_WORK(chao)->flags |= CHAO_RACE_FIXED_HEADING;
                break;
            }
            race_racing_entrants[race_entrant_count] = entrant;
            race_entrant_count++;
        } else {
            race_participants[entrant].chao = NULL;
            race_participants[entrant].place =
                race_participants[entrant].previous_place = RACE_ENTRANTS;
            race_participants[entrant].finished = TRUE;
        }
    }
}

#ifdef NONMATCHING
/* this side loads and checks the land, then reads the layout. it starts the
 * set, the land and the cameras. it undoes each step where a later one fails.
 * it returns the controller's state, or null */
static struct race_controller_state *
race_start_course(const struct garden_race_course *course,
                  struct race_land_file *land_file)
{
    static const char *const names[] = {
        race_neutral_land_file, race_hero_land_file, race_dark_land_file};
    char filename[256];
    struct set_head *layout;
    struct race_controller_state *controller;
    Sint32 size;

    if ((unsigned int)course->land >= sizeof(names) / sizeof(names[0]) ||
        (unsigned int)course->track >= RACE_RECORD_TRACKS) {
        return NULL;
    }
    size = loader_read_prs_checked(names[course->land], land_file,
                                   (uintptr_t)CHAO_LAND_BUILD_AREA -
                                       (uintptr_t)CHAO_RACE_LAND_AREA);
    if (size < (Sint32)sizeof(*land_file) ||
        !garden_land_ready(land_file, size, land_file->land) ||
        !garden_land_contains(land_file, size, land_file->glass,
                              sizeof(*land_file->glass))) {
        return NULL;
    }
    strcpy(filename, race_set_prefix);
    strcat(filename, race_track_sets.name[course->track]);
    strcat(filename, race_set_suffix);
    layout =
        set_read(race_land_sets.name[course->land], filename, LAYOUT_SLOTS);
    if (layout == NULL) {
        return NULL;
    }
    controller = syCalloc(1, sizeof(*controller));
    if (controller == NULL) {
        loader_free(layout);
        return NULL;
    }
    if (!set_start(&chaostgrace_objects, layout)) {
        goto release_controller;
    }
    if (!world_land_start(land_file->land)) {
        goto stop_set;
    }
    if (!race_camera_create(course->camera_track)) {
        world_land_stop();
        goto stop_set;
    }
    race_layout = layout;
    garden_select_track(course->track);
    world_set_near_radius(START_NEAR_RADIUS);
    return controller;

stop_set:
    set_end();
release_controller:
    syFree(controller);
    loader_free(layout);
    return NULL;
}
#endif

void chaostgrace_step(struct task *task)
{
    const struct garden_race_course *course = garden_race_course();
#ifdef NONMATCHING
    /* this side starts the course by race_start_course, and tries again the
     * next step where it can't */
    struct task *glass;
    struct light *light;
    struct race_land_file *land_file = &RACE_LAND_FILE;

    struct race_controller_state *controller =
        race_start_course(course, land_file);

    if (controller == NULL) {
        return;
    }
#else
    char filename[256];
    struct race_land_files lands;
    struct race_track_files tracks;
    struct task *glass;
    struct light *light;
    struct race_land_file *land_file;

    garden_select_track(course->track);
    race_camera_create(course->camera_track);
    /* @hack spelling: land_file set in the switch's expression, where a
     * statement before the switch would do */
    switch (land_file = &RACE_LAND_FILE, course->land) {
    case RACE_LAND_NEUTRAL:
        loader_read_prs(race_neutral_land_file, land_file);
        break;
    case RACE_LAND_HERO:
        loader_read_prs(race_hero_land_file, land_file);
        break;
    case RACE_LAND_DARK:
        loader_read_prs(race_dark_land_file, land_file);
        break;
    }
    world_land_start(land_file->land);
    world_set_near_radius(START_NEAR_RADIUS);
    lands = race_land_sets;
    tracks = race_track_sets;
    strcpy(filename, race_set_prefix);
    strcat(filename, tracks.name[course->track]);
    strcat(filename, race_set_suffix);
    race_layout = set_read(lands.name[course->land], filename, LAYOUT_SLOTS);
    set_start(&chaostgrace_objects, race_layout);
#endif
    texture_load_shared(race_effect_textures, race_effect_groups);
    switch (garden_race_course()->land) {
    case RACE_LAND_NEUTRAL:
        texture_load_pvm(race_neutral_objects, &race_neutral_texlist);
        texture_load_pvm(race_neutral_water, &race_water_texlist);
        break;
    case RACE_LAND_HERO:
        texture_load_pvm(race_hero_objects, &race_hero_texlist);
        texture_load_pvm(race_hero_water, &race_water_texlist);
        break;
    case RACE_LAND_DARK:
        texture_load_pvm(race_dark_objects, &race_dark_texlist);
        texture_load_pvm(race_dark_water, &race_water_texlist);
        break;
    }
    race_map_load();
    texture_load_pvm(race_hud_textures, &race_hud_texlist);
    texture_load_pvm(race_animal_textures, &garden_animal_texlist);
    texture_load_pvm(race_item_textures, &race_fruit_texlist);
    texture_load_pvm(race_toy_textures, &garden_toy_texlist);
    music_fade_in(0);
    njSetBackColor(0xff000000, 0xff000000, 0xff000000);
    garden_control_start();
    race_water_create();
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    simple_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetSimpleLightIntensity(1.0F, 0.2F);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, light = &race_key_light);
    light_set(1, &race_fill_light);
    light_set(2, light);
    light_set(3, light);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    light_apply_player(0);
    lights_load(race_light_file);
    world_build_land(land_file->land, (Uint32)LAND_BUILD_AREA);
    task->step = race_controller_step;
    task->draw = race_controller_draw;
    task->destructor = race_controller_close;
    race_controller = task;
#ifdef NONMATCHING
    /* this side takes the state race_start_course made */
    task->extra.p = controller;
#else
    task->extra.p = syCalloc(1, sizeof(struct race_controller_state));
#endif
    garden_restore_carried();
    place_entrants();
    race_trees_reset();
    switch (garden_race_course()->track) {
    case TRACK_JWL_IL:
    case TRACK_JWL_LONG:
        race_scavenger_start();
        break;
    }
    if (garden_race_course()->land == RACE_LAND_NEUTRAL) {
        race_pitfalls_create();
    }
    garden_race_clear_time();
    race_reset_finish_order();
    race_player_count = garden_race_entrant_count();
    player_pause(0);
    garden_messages_load(race_messages);
    race_clock_running = TRUE;
    glass = task_create(TASK_WORK, 2, race_glass_step, race_glass_name);
    glass->draw = race_draw_glass;
}
