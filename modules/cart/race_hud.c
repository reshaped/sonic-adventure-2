/* the race's HUD: its times, place, lap and countdown. this source also draws
 * the time extension's banner and the versus result */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "cart/boost_window.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/pads.h"
#include "cart/race.h"
#include "cart/stage.h"
#include "cart/vehicle.h"
#include "hud/hud.h"
#include "player/player.h"
#include "save/save.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"

struct screen_point {
    Sint32 x, y, priority;
};
/* the cells of cart_hud_sprites the HUD draws */
enum cell {
    TIME = 0,
    BEST = 1,
    LAP = 2,
    TIME_LEFT = 3,
    SECONDS = 4,
    EXTENSION = 5,
    EXTENSION_SHADOW = 6,
    DIGIT_0 = 7,
    SLASH = 17,
    FIRST_SUFFIX = 18,
    FOURTH_SUFFIX = 21,
    FIRST_PLACE = 22,
    PLACE_LINE = 29
};
/* where each race's points start in layout: a story race's, a one-player
 * race's and a two-player race's */
enum layout_base { STORY = 0, SOLO = 9, VERSUS = 21 };
static const char extension_name[12] = "timeExtExec";
static const char layout_name[12] = "layoutExec";
static const char result_pvm[12] = "resultCart";
static const char result_name[16] = "cartResultExec";

NJS_TEXANIM cart_hud_sprites[CART_HUD_SPRITES] = {
#include "cart/race_hud/cart_hud_sprites.inc"
};

/* clang-format off */
/* the row draw_time draws a time's pairs of digits in */
static struct hud_number digits = {
    HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS, 0, 0, 99, 0,
    {24, 52, 0}, 0.5F, 0xffffffff};
/* the row of the colons between the pairs */
static struct hud_number colon = {
    HUD_NUMBER_SCALED | HUD_NUMBER_COLONS, 0, 0, 0, 0, {48, 52, 0}, 0.5F,
    0xffffffff};
/* the row of a time whose minute is CART_NO_TIME, in the cells
 * cart_number_cell gives */
static struct hud_number dashes = {
    HUD_NUMBER_SCALED | HUD_NUMBER_HOOK, 0, 0, 0, 0, {24, 52, 0}, 0.5F,
    0xffffffff};
/* clang-format on */
/* the points the HUD draws at */
static struct screen_point layout[29] = {
#include "cart/race_hud/layout.inc"
};
#include "cart/race_hud/text.inc"

/* the versus result's cells, of the one texture of result_textures */
#define RESULT_CELLS 8
static NJS_TEXNAME result_texture = {
#include "cart/race_hud/result_texture.inc"
};
static NJS_TEXLIST result_textures = {&result_texture, 1};
static NJS_TEXANIM result_cells[RESULT_CELLS] = {
#include "cart/race_hud/result_cells.inc"
};

Bool cart_result_done, cart_result_open, cart_story_clear;
static void extension_step(struct task *task);
static void extension_draw(struct task *task);
static void hud_step(struct task *task);
static void draw_layout(struct task *task);
static void result_step(struct task *task);
static void result_draw(struct task *task);
static void result_destroy(struct task *task);

static void draw_cell(int cell, const struct screen_point *pos, Uint32 attr)
{
    NJS_SPRITE sprite;
    sprite.tlist = &cart_object_textures;
    sprite.tanim = cart_hud_sprites;
    sprite.sx = 1;
    sprite.sy = 1;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    njDrawSprite2D(&sprite, cell, pos->priority, attr | NJD_SPRITE_ALPHA);
}
/* draw_time's pen, in pixels. each colon stands COLON_STEP right of the pair
 * before it. each pair stands COLON_STEP - COLON_BACK + PAIR_STEP right of the
 * pair before it. each pair draws up to PAIR_MOST, and the seconds up to
 * SECONDS_PER_MINUTE - 1 */
#define COLON_STEP 16
#define COLON_BACK 11
#define PAIR_STEP 32
#define PAIR_MOST 99
#define SECONDS_PER_MINUTE 60
#define NEXT_PAIR(row)                                                         \
    do {                                                                       \
        colon.pos = (row).pos;                                                 \
        colon.pos.x += COLON_STEP;                                             \
        hud_number_draw(&colon);                                               \
        colon.pos.x -= COLON_BACK;                                             \
        (row).pos = colon.pos;                                                 \
    } while (0)

static void draw_time(const struct save_time *time,
                      const struct screen_point *pos, Uint32 attr)
{
    if (time->minute < 0) {
        dashes.pos.x = pos->x;
        dashes.pos.y = pos->y;
        dashes.pos.z = pos->priority;
        dashes.max = PAIR_MOST;
        hud_number_draw(&dashes);
        NEXT_PAIR(dashes);
        dashes.pos.x += PAIR_STEP;
        hud_number_draw(&dashes);
        NEXT_PAIR(dashes);
        dashes.pos.x += PAIR_STEP;
        hud_number_draw(&dashes);
    } else {
        digits.pos.x = pos->x;
        digits.pos.y = pos->y;
        digits.pos.z = pos->priority;
        digits.max = PAIR_MOST;
        digits.value = time->minute;
        hud_number_draw(&digits);
        NEXT_PAIR(digits);
        digits.max = SECONDS_PER_MINUTE - 1;
        digits.value = time->second;
        digits.pos.x += PAIR_STEP;
        hud_number_draw(&digits);
        NEXT_PAIR(digits);
        digits.max = PAIR_MOST;
        digits.value = time->hundredths;
        digits.pos.x += PAIR_STEP;
        hud_number_draw(&digits);
    }
}
/* the time extension's banner slides from BANNER_IN to BANNER_STOP in
 * SLIDE_STEPS steps. it waits HOLD_STEPS, and slides on as far again. its
 * shadow slides the other way: at SHADOW_MIRROR less the banner's x,
 * SHADOW_DROP lower, and behind it */
#define BANNER_IN 700
#define BANNER_STOP 200
#define BANNER_Y 200
#define SLIDE_STEPS 20
#define HOLD_STEPS 120
#define SHADOW_MIRROR 401.0F
#define SHADOW_DROP 1
#define BANNER_PRIORITY (-1)
#define SHADOW_PRIORITY (-2)
/* the banner's x at timer steps into a slide. the division leaves it in whole
 * pixels */
/* NOLINTBEGIN(bugprone-integer-division) */
#define SLIDE_X(timer)                                                         \
    (BANNER_IN + ((BANNER_STOP - BANNER_IN) * (timer) / SLIDE_STEPS))
/* NOLINTEND(bugprone-integer-division) */
void cart_show_time_extension(void)
{
    task_create(TASK_WORK, 2, extension_step, extension_name);
}
static void extension_step(struct task *task)
{
    struct task_work *work = task->work;
    if (work->mode != 0) {
        switch (work->tick) {
        case 0:
            work->pos.x = SLIDE_X(work->timer);
            work->timer++;
            if (work->timer > SLIDE_STEPS) {
                work->timer = 0;
                work->tick++;
            }
            break;
        case 1:
            work->pos.x = BANNER_STOP;
            work->timer++;
            if (work->timer > HOLD_STEPS) {
                work->timer = SLIDE_STEPS;
                work->tick++;
            }
            break;
        case 2:
            work->pos.x = SLIDE_X(work->timer);
            work->timer++;
            if (work->timer > 2 * SLIDE_STEPS) {
                task_destroy(task);
            }
            break;
        }
    } else {
        task->draw = extension_draw;
        work->pos.x = BANNER_IN;
        work->pos.y = BANNER_Y;
        work->mode++;
    }
}
static void extension_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct screen_point pos;
    pos.x = work->pos.x;
    pos.y = work->pos.y;
    pos.priority = BANNER_PRIORITY;
    draw_cell(EXTENSION, &pos, 0);
    pos.x = SHADOW_MIRROR - work->pos.x;
    pos.y = work->pos.y + SHADOW_DROP;
    pos.priority = SHADOW_PRIORITY;
    draw_cell(EXTENSION_SHADOW, &pos, 0);
}
/* each cell's offset in pixels from the cell drawn before it, or for the
 * first, from the point */
#define COUNT_TENS_X 60
#define COUNT_DIGIT_X 40
#define COUNT_SECONDS_Y 16
#define PLACE_LINE_X 26
#define PLACE_LINE_Y 20
#define PLACE_RISE 26
#define PLACE_SUFFIX_X 50
#define PLACE_SUFFIX_Y 16
#define LAP_DIGIT_X 60
#define LAP_DIGIT_RISE 8
#define LAP_SLASH_X 30
static void draw_countdown(int seconds, const struct screen_point *pos)
{
    int digit;
    int tens;
    struct screen_point at;
    if (seconds > PAIR_MOST) {
        seconds = PAIR_MOST;
    } else if (seconds < 0) {
        seconds = 0;
    }
    at = *pos;
    draw_cell(TIME_LEFT, &at, 0);
    at.x += COUNT_TENS_X;
    tens = seconds / 10;
    if (tens != 0) {
        draw_cell(tens + DIGIT_0, &at, 0);
    }
    at.x += COUNT_DIGIT_X;
    digit = seconds % 10;
    draw_cell(digit + DIGIT_0, &at, 0);
    at.x += COUNT_DIGIT_X;
    at.y += COUNT_SECONDS_Y;
    draw_cell(SECONDS, &at, 0);
}
static void draw_place(int place, const struct screen_point *pos)
{
    struct screen_point at;
    at = *pos;
    at.x += PLACE_LINE_X;
    at.y += PLACE_LINE_Y;
    draw_cell(PLACE_LINE, &at, 0);
    at.y -= PLACE_RISE;
    draw_cell(place + FIRST_PLACE - 1, &at, 0);
    at.x += PLACE_SUFFIX_X;
    at.y += PLACE_SUFFIX_Y;
    switch (place - 1) {
    case 0:
    case 1:
    case 2:
        draw_cell(place + FIRST_SUFFIX - 1, &at, 0);
        break;
    default:
        draw_cell(FOURTH_SUFFIX, &at, 0);
        break;
    }
}
static void draw_lap(int lap, const struct screen_point *pos)
{
    struct screen_point at;
    if (lap > CART_LAPS - 1) {
        lap = CART_LAPS - 1;
    } else if (lap < 0) {
        lap = 0;
    }
    at = *pos;
    draw_cell(LAP, &at, 0);
    at.x += LAP_DIGIT_X;
    at.y -= LAP_DIGIT_RISE;
    draw_cell(lap + DIGIT_0 + 1, &at, 0);
    at.x += LAP_SLASH_X;
    draw_cell(SLASH, &at, 0);
    at.x += LAP_SLASH_X;
    draw_cell(DIGIT_0 + CART_LAPS, &at, 0);
}

void cart_start_hud(void)
{
    int course;
    task_create(TASK_WORK, 2, hud_step, layout_name);
    cart_result_done = 0;
#ifdef NONMATCHING
    /* this side shows no best time for a course without a record in the save.
     * the reads below would take that time from past their tables */
    if (!cart_time_record_available()) {
        cart_race.best.minute = CART_NO_TIME;
        cart_race.best.second = 0;
        cart_race.best.hundredths = 0;
        return;
    }
#endif
    if (cart_setup.course_mode != CART_DOWNLOAD) {
        cart_race.best.minute =
            save_data.kart_records[game_state.kart_course].best[0].time.minute;
        cart_race.best.second =
            save_data.kart_records[game_state.kart_course].best[0].time.second;
        cart_race.best.hundredths =
            save_data.kart_records[game_state.kart_course]
                .best[0]
                .time.hundredths;
    } else {
        course = download_chosen.slot;
        cart_race.best.minute = save_data.download_best_times[course].minute;
        cart_race.best.second = save_data.download_best_times[course].second;
        cart_race.best.hundredths =
            save_data.download_best_times[course].hundredths;
    }
}
static void hud_step(struct task *task)
{
    task->draw = draw_layout;
}

/* how far below hud_draw's place hud_draw_rings_at draws each race's rings */
#define STORY_RINGS_Y 48
#define SOLO_RINGS_Y 32
#define UPPER_RINGS_Y 16
#define LOWER_RINGS_Y 262
/* the pulse hud_draw_rings_at takes: the low byte of game_state.steps, as
 * green and blue */
#define RING_PULSE                                                             \
    (((game_state.steps << 8) & 0xff00) | (game_state.steps & 0xff))
static void draw_layout(struct task *task)
{
    struct cart_vehicle *racers[CART_PLAYERS];
    /* @hack spelling: racer, a pointer to the racer drawn in racers, where
     * racers[0] and racers[1] would do */
    struct cart_vehicle **racer;
    /* @hack spelling: setup, a pointer to cart_setup, where cart_setup itself
     * would do */
    struct cart_setup *setup;
    /* @hack spelling: first, a pointer to layout, where layout itself would
     * do */
    struct screen_point *first;
    int i;

    if (cart_result_done != 0) {
        return;
    }
    for (i = 0; i < CART_PLAYERS; i++) {
        cart_race.player[i].task = cart_racers[i];
    }
    *(racer = racers) = cart_race.player[0].task->motion.p;
    if (game_state.two_players) {
        racers[1] = cart_race.player[1].task->motion.p;
    }
    setup = &cart_setup;
    /* @hack redundant: a draw of the rings under if (0) */
    if (0) {
        hud_draw_rings_at(0, 0xff00, 0);
    }
    first = layout;
    /* @hack redundant: three steps of first and a test of play_mode, under
     * if (0) */
    if (0) {
        first++;
        first++;
        first++;
        if (setup->play_mode) {
        }
    }
    if (setup->play_mode) {
        draw_cell(TIME, &first[STORY], 0);
        draw_time(&cart_race.time, &layout[STORY + 1], 0);
        draw_countdown((cart_race.countdown + (CART_STEPS_PER_SECOND - 1)) /
                           CART_STEPS_PER_SECOND,
                       &layout[STORY + 2]);
        draw_cell(LAP, &layout[STORY + 3], 0);
        draw_time(&cart_race.player[0].lap[0], &layout[STORY + 4], 0);
        draw_cell(LAP, &layout[STORY + 5], 0);
        draw_time(&cart_race.player[0].lap[1], &layout[STORY + 6], 0);
        hud_draw_rings_at(0, RING_PULSE, STORY_RINGS_Y);
        hud_draw_lives_of(0, setup->course_mode == CART_ROUGE
                                 ? PLAYER_ROUGE
                                 : PLAYER_TAILS_MECH);
    } else if (game_state.two_players) {
        draw_cell(TIME, &first[VERSUS], 0);
        draw_time(&cart_race.time, &layout[VERSUS + 1], 0);
        draw_place((*racer)->place, &layout[VERSUS + 2]);
        draw_lap((*racer)->furthest_lap, &layout[VERSUS + 3]);
        draw_cell(TIME, &layout[VERSUS + 4], 0);
        draw_time(&cart_race.time, &layout[VERSUS + 5], 0);
        draw_place((*(racer = &racers[1]))->place, &layout[VERSUS + 6]);
        draw_lap((*racer)->furthest_lap, &layout[VERSUS + 7]);
        hud_draw_rings_at(0, RING_PULSE, UPPER_RINGS_Y);
        hud_draw_rings_at(1, RING_PULSE, LOWER_RINGS_Y);
    } else {
        draw_cell(TIME, &first[SOLO], 0);
        draw_time(&cart_race.time, &layout[SOLO + 1], 0);
        if (setup->course_mode != CART_DOWNLOAD ||
            game_state.kart_character_karts != 1) {
            draw_cell(BEST, &layout[SOLO + 2], 0);
            draw_time(&cart_race.best, &layout[SOLO + 3], 0);
        }
        draw_place((*racer)->place, &layout[SOLO + 4]);
        draw_cell(LAP, &layout[SOLO + 5], 0);
        draw_time(&cart_race.player[0].lap[0], &layout[SOLO + 6], 0);
        draw_cell(LAP, &layout[SOLO + 7], 0);
        draw_time(&cart_race.player[0].lap[1], &layout[SOLO + 8], 0);
        draw_cell(LAP, &layout[SOLO + 9], 0);
        draw_time(&cart_race.player[0].lap[2], &layout[SOLO + 10], 0);
        draw_lap((*racer)->furthest_lap, &layout[SOLO + 11]);
        hud_draw_rings_at(0, RING_PULSE, SOLO_RINGS_Y);
    }
    cart_boost_window_set(0, cart_boost_ready[0]);
    cart_boost_window_set(1, cart_boost_ready[1]);
}
/* a versus result shows its winner for WINNER_STEPS steps, then a choice to
 * retry or quit. the choice quits on its own after IDLE_STEPS steps with no
 * press */
#define WINNER_STEPS 300
#define IDLE_STEPS 3600
/* the menus' sounds of a move and a choice */
#define CURSOR_SOUND 0x8000
#define CHOOSE_SOUND 0x8001
enum result_mode { SHOW_WINNER, CHOOSE, CHOSEN };
enum result_choice { CHOICE_RETRY, CHOICE_QUIT };
/* result_cells. WINNER draws on the winner's half of the screen, and PROMPT
 * above the choices. each choice has a cell for chosen and one for not */
enum result_cell { WINNER = 1, PROMPT, RETRY_CHOSEN, RETRY, QUIT_CHOSEN, QUIT };
/* where the result draws them */
#define RESULT_X 320
#define UPPER_WINNER_Y 160
#define LOWER_WINNER_Y 320
#define PROMPT_Y 200
#define CHOICES_Y 300
#define RETRY_X 240
#define QUIT_X 400
static void draw_result_cell(int cell, Float scale, const NJS_POINT3 *pos,
                             int attr)
{
    NJS_SPRITE sprite;
    sprite.tlist = &result_textures;
    sprite.tanim = result_cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}
void cart_load_result_textures(void)
{
    int i;
    for (i = 0; i < RESULT_CELLS; i++) {
        result_cells[i].cx = result_cells[i].sx / 2;
        result_cells[i].cy = result_cells[i].sy / 2;
    }
    texture_load_pvm(result_pvm, &result_textures);
    cart_result_open = 0;
}
void cart_release_result_textures(void)
{
    njReleaseTexture(&result_textures);
}
void cart_show_versus_result(void)
{
    task_create(TASK_WORK, 2, result_step, result_name);
    cart_result_open = 1;
}
static void result_step(struct task *task)
{
    struct task_work *work = task->work;
    task->draw = result_draw;
    task->destructor = result_destroy;
    switch (work->mode) {
    case SHOW_WINNER:
        work->timer++;
        if (work->timer > WINNER_STEPS) {
            work->timer = 0;
            work->mode = CHOOSE;
            work->tick = CHOICE_QUIT;
            cart_result_done = 1;
        }
        break;
    case CHOOSE:
        if ((pad_press_repeat[0] & PDD_DGT_KL) |
            (pad_press_repeat[1] & PDD_DGT_KL)) {
            work->timer = 0;
            if (work->tick != CHOICE_RETRY) {
                work->tick = CHOICE_RETRY;
                sound_play(CURSOR_SOUND, NULL, 0, 0);
            }
        } else if ((pad_press_repeat[0] & PDD_DGT_KR) |
                   (pad_press_repeat[1] & PDD_DGT_KR)) {
            work->timer = 0;
            if (work->tick == CHOICE_RETRY) {
                work->tick = CHOICE_QUIT;
                sound_play(CURSOR_SOUND, NULL, 0, 0);
            }
            /* @hack type: the pads and the buttons cast to Sint32, where their
             * own unsigned types would do */
        } else if (((Sint32)pad_press_repeat[0] & (Sint32)pad_choose_buttons) |
                   ((Sint32)pad_press_repeat[1] & (Sint32)pad_choose_buttons)) {
            sound_play(CHOOSE_SOUND, NULL, 0, 0);
            if (work->tick) {
                task_destroy(task);
            } else {
                stage_end(STAGE_END_RESTART);
                work->mode = CHOSEN;
            }
            cart_result_done = 1;
        } else {
            work->timer++;
            if (work->timer > IDLE_STEPS) {
                task_destroy(task);
                cart_result_done = 1;
            }
        }
        break;
    case CHOSEN:
        break;
    }
}
static void result_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct cart_vehicle *racers[CART_PLAYERS];
    racers[0] = cart_racers[0]->motion.p;
    racers[1] = cart_racers[1]->motion.p;
    switch (work->mode) {
    case SHOW_WINNER:
        if (racers[0]->place < racers[1]->place) {
            pos.y = UPPER_WINNER_Y;
        } else {
            pos.y = LOWER_WINNER_Y;
        }
        pos.x = RESULT_X;
        pos.z = 0;
        draw_result_cell(WINNER, 1, &pos, NJD_SPRITE_ALPHA);
        break;
    case CHOOSE:
        pos.x = RESULT_X;
        pos.y = PROMPT_Y;
        pos.z = 0;
        draw_result_cell(PROMPT, 1, &pos, NJD_SPRITE_ALPHA);
        pos.y = CHOICES_Y;
        if (work->tick != CHOICE_RETRY) {
            pos.x = RETRY_X;
            draw_result_cell(RETRY, 1, &pos, NJD_SPRITE_ALPHA);
            pos.x = QUIT_X;
            draw_result_cell(QUIT_CHOSEN, 1, &pos, NJD_SPRITE_ALPHA);
        } else {
            pos.x = RETRY_X;
            draw_result_cell(RETRY_CHOSEN, 1, &pos, NJD_SPRITE_ALPHA);
            pos.x = QUIT_X;
            draw_result_cell(QUIT, 1, &pos, NJD_SPRITE_ALPHA);
        }
        break;
    }
}
static void result_destroy(struct task *task)
{
    cart_result_open = 0;
}
