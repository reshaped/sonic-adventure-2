/* the course screen's cards, which stand on a rail at the screen's right */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chao/actor.h"
#include "chao/race.h"
#include "chao/race_opponents.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/map_art.h"
#include "chaostgentrance/menu.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#ifdef NONMATCHING
/* garden_race_set_level by its own name: this side's objects don't define the
 * second name, which only symbols.txt places */
#define garden_race_set_level_int(level) garden_race_set_level(level)
#else
/* garden_race_set_level, declared to take an int, for rival_preview_step's
 * mismatched call */
extern void garden_race_set_level_int(int level);
#endif

struct preview_card {
    Sint8 mode, phase;
    Uint8 id, age;
    Sint16 flags;
    Uint16 delay;
    Uint8 unk_08[6];
    Sint16 index;
    Uint16 column, row;
    Float x, y, slide, alpha;
    Uint8 unk_24[12];
    Sint32 cell;
    struct task *chao; /* rival_preview's Chao, or NULL */
};

struct preview_rail {
    Sint8 mode, phase;
    Uint8 id, age;
    Sint16 flags;
    Uint16 delay;
    Uint8 unk_08[6];
    Uint16 height, limit, unk_12; /* the rail opens until height is limit */
    Float x, y;
    Uint8 unk_1c[24];
};

enum {
    PREVIEW_WAIT,
    PREVIEW_ENTER,
    PREVIEW_SHOWN,
    PREVIEW_EXIT_DELAY,
    PREVIEW_LEAVE
};
/* glyphs of the font garden_draw_world_name draws with */
enum { GLYPH_DASH = 13, GLYPH_ZERO = 16, GLYPH_COLON = 26, GLYPH_QUERY = 31 };
#define PREVIEW_INK 0xff4444ff
/* a card waits CARD_DELAY + 1 frames before it fades in. on a select or a
 * back, it waits EXIT_DELAY + 1 frames before it fades out. it fades by
 * FADE_STEP a frame */
#define CARD_DELAY 50
#define EXIT_DELAY 10
#define FADE_STEP 0.1F
/* a card's Chao stands at its card's point less an origin, times
 * WORLD_PER_PIXEL. the chosen Chao's x origin is PLAYER_ORIGIN_X, and the
 * rival's and the record holder's RIVAL_ORIGIN_X. the record holder's y is
 * HOLDER_ORIGIN_Y less its card's */
#define PLAYER_ORIGIN_X 245.0F
#define RIVAL_ORIGIN_X 395.0F
#define HOLDER_ORIGIN_Y 388.7F
#define PLAYER_YAW 0xe000
#define RIVAL_YAW 0x800
/* the offsets of the name and the time on the chosen Chao's card, and on
 * the record holder's. the team player's name stands TEAM_NAME_RAISE above
 * its own place */
#define NAME_X 320.0F
#define NAME_Y 222.0F
#define TIME_X 312.0F
#define TIME_Y 202.0F
#define HOLDER_NAME_X 278.0F
#define HOLDER_NAME_Y 247.0F
#define HOLDER_TIME_X 270.0F
#define HOLDER_TIME_Y 227.0F
#define TEAM_NAME_RAISE 11.5F
#define HUNDREDTHS_PER_SECOND 100
/* the map stands MAP_DROP below its card, at MAP_HEIGHT of its height */
#define MAP_DROP 0.8F
#define MAP_HEIGHT 0.9F
/* the rail opens and closes by RAIL_STEP a frame. it opens from RAIL_SHUT
 * to RAIL_HEIGHT, or to MULTI_RAIL_HEIGHT on multiplayer's screen */
#define RAIL_X 496.0F
#define RAIL_Y 236.0F
/* @hack spelling: RAIL_STEP and RAIL_SHUT Floats, which height, a Uint16,
 * meets through float arithmetic, where ints would do */
#define RAIL_STEP 20.0F
#define RAIL_SHUT 16.0F
#define RAIL_HEIGHT 280
#define MULTI_RAIL_HEIGHT 208
#define RAIL_DELAY 39
#define RAIL_EXIT_DELAY 20
/* the cards' distances above and below the rail's middle */
#define TOP_CARD_UP 98.0F
#define MIDDLE_CARD_UP 21.0F
#define BOTTOM_CARD_DOWN 77.0F
#define MULTI_TOP_CARD_UP 60.0F
#define MULTI_BOTTOM_CARD_DOWN 40.0F

/* cells of the common sheet. RAIL_TOP_CELLS, RAIL_CELLS and RAIL_BOTTOM_CELLS
 * each begin a run of three. so does CARD_CELLS, which each card draws
 * behind its own cell */
#define PLAYER_CARD 104
#define HOLDER_CARD 105
#define TEAM_CARD 106
#define RIVAL_CARD 107
#define CARD_CELLS 113
#define MAP_CARD 96
#define MAP_FRAME 99
#define MAP_BACK 100
#define MAP_EDGE 101
#define RAIL_TOP_CELLS 116
#define RAIL_CELLS 119
#define RAIL_BOTTOM_CELLS 122
#define PREVIEW_MODE(work, next)                                               \
    {                                                                          \
        (work)->mode = (next);                                                 \
        (work)->phase = 0;                                                     \
    }
#define WAIT_FOR(work, next)                                                   \
    if ((work)->age++ > (work)->delay) {                                       \
        (work)->age = 0;                                                       \
        PREVIEW_MODE(work, next);                                              \
    }
#define EXIT_PHASES(work)                                                      \
    case PREVIEW_EXIT_DELAY:                                                   \
        WAIT_FOR(work, PREVIEW_LEAVE);                                         \
        break;                                                                 \
    case PREVIEW_LEAVE:                                                        \
        (work)->alpha -= FADE_STEP;                                            \
        if ((work)->alpha < 0.0F) {                                            \
            (work)->alpha = 0.0F;                                              \
            task_kill(task);                                                   \
        }                                                                      \
        break
/* a card that isn't shown holds the menu, by setting entrance_menu->wait */
#define HOLD_MENU(work)                                                        \
    switch ((work)->mode) {                                                    \
    case PREVIEW_SHOWN:                                                        \
        break;                                                                 \
    default:                                                                   \
        entrance_menu->wait = TRUE;                                            \
        break;                                                                 \
    }
/* on a select or a back, a card runs release, waits frames and leaves */
#define CHECK_EXIT(work, frames, release)                                      \
    if ((work)->mode < PREVIEW_EXIT_DELAY) {                                   \
        if (entrance_menu->request == ENTRANCE_MENU_SELECT ||                  \
            entrance_menu->request == ENTRANCE_MENU_BACK) {                    \
            (work)->delay = (frames);                                          \
            release;                                                           \
            PREVIEW_MODE(work, PREVIEW_EXIT_DELAY);                            \
        }                                                                      \
    }
#define REMOVE_PREVIEW(which)                                                  \
    if (entrance_menu->preview[(which)]) {                                     \
        task_kill(entrance_menu->preview[(which)]);                            \
        entrance_menu->preview[(which)] = NULL;                                \
    }
#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side. it freezes and kills
 * only a preview that garden_spawn_chao made */
#define FREEZE_PLAYER(actor)                                                   \
    if (actor) {                                                               \
        FREEZE_CHAO(actor);                                                    \
    }
#define RELEASE_TEAM_PLAYER() REMOVE_PREVIEW(0)
#else
/* @bug memory: player_record_step at 0x0c9c78a0 and team_player_step at
 * 0x0c9c8342 clear flags in the work of garden_spawn_chao's result, untested
 * for NULL. team_player_step also kills it untested. rival_preview_step
 * tests the same call's result at 0x0c9c86ce */
#define FREEZE_PLAYER(actor) FREEZE_CHAO(actor)
#define RELEASE_TEAM_PLAYER()                                                  \
    {                                                                          \
        task_kill(entrance_menu->preview[0]);                                  \
        entrance_menu->preview[0] = NULL;                                      \
    }
#endif
#define PLACE_PLAYER()                                                         \
    position.x = (card->x - PLAYER_ORIGIN_X) * WORLD_PER_PIXEL;                \
    position.y =                                                               \
        PREVIEW_HEIGHT - ((card->y - PREVIEW_ORIGIN_Y) * WORLD_PER_PIXEL);     \
    position.z = PREVIEW_Z;                                                    \
    chao = garden_spawn_chao(garden_race_entrants[0], &position, PLAYER_YAW);  \
    FREEZE_PLAYER(chao);                                                       \
    entrance_menu->preview[0] = chao
#define SHOW_PLAYER()                                                          \
    case PREVIEW_WAIT:                                                         \
        WAIT_FOR(card, PREVIEW_ENTER);                                         \
        break;                                                                 \
    case PREVIEW_ENTER:                                                        \
        card->alpha += FADE_STEP;                                              \
        if (card->alpha > 1.0F) {                                              \
            PLACE_PLAYER();                                                    \
            card->alpha = 1.0F;                                                \
            PREVIEW_MODE(card, PREVIEW_SHOWN);                                 \
        }                                                                      \
        break;                                                                 \
    case PREVIEW_SHOWN:                                                        \
        break
#define CARD_SPRITE(work)                                                      \
    switch ((work)->mode) {                                                    \
    case PREVIEW_WAIT:                                                         \
        return;                                                                \
    }                                                                          \
    sprite.p.x = (work)->x;                                                    \
    sprite.p.y = (work)->y;                                                    \
    sprite.p.z = 0.0F;                                                         \
    sprite.ang = 0;                                                            \
    sprite.sx = 1.0F;                                                          \
    sprite.sy = 1.0F;                                                          \
    sprite.tlist = &entrance_common_texlist;                                   \
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;                       \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA)
#define DRAW(cell, depth) entrance_draw_cell(&sprite, (cell), (depth))
#define CARD_FRAME(cell)                                                       \
    CARD_SPRITE(card);                                                         \
    constant_material_set(card->alpha, 1.0F, 1.0F, 1.0F);                      \
    sprite.p.y = card->y;                                                      \
    DRAW(cell, -32.2F);                                                        \
    DRAW(CARD_CELLS, -32.3F);                                                  \
    DRAW(CARD_CELLS + 1, -32.3F);                                              \
    DRAW(CARD_CELLS + 2, -32.3F)
#define CAPTION_AT(position, offset_x, offset_y)                               \
    (position).x =                                                             \
        (card->x + entrance_caption_x - (offset_x)) * WORLD_PER_PIXEL;         \
    (position).y =                                                             \
        (entrance_caption_y - card->y + (offset_y)) * WORLD_PER_PIXEL;         \
    (position).z = PREVIEW_Z
#define DRAW_NAME(text, offset_x, offset_y)                                    \
    {                                                                          \
        NJS_POINT3 position;                                                   \
        CAPTION_AT(position, offset_x, offset_y);                              \
        if (!(text)[0]) {                                                      \
            char unnamed[5];                                                   \
            unnamed[0] = GLYPH_QUERY;                                          \
            unnamed[1] = GLYPH_QUERY;                                          \
            unnamed[2] = GLYPH_QUERY;                                          \
            unnamed[3] = GLYPH_QUERY;                                          \
            unnamed[4] = 0;                                                    \
            garden_draw_world_name(&position, unnamed, PREVIEW_INK, 1.0F);     \
        } else {                                                               \
            garden_draw_world_name(&position, (text), PREVIEW_INK, 1.0F);      \
        }                                                                      \
    }
/* @hack spelling: number - number / 10 * 10, where number % 10 would do */
#define TWO_DIGITS(index, number)                                              \
    text[index] = ((number) / 10) + GLYPH_ZERO;                                \
    text[(index) + 1] = (number) - (((number) / 10) * 10) + GLYPH_ZERO
#define EMPTY_DIGITS(index)                                                    \
    text[(index)] = GLYPH_DASH;                                                \
    text[(index) + 1] = GLYPH_DASH
#define DRAW_TIME(clock_value, offset_x, offset_y)                             \
    {                                                                          \
        const struct garden_race_time *clock = (clock_value);                  \
        Uint16 minutes = clock->minutes;                                       \
        Uint16 seconds = clock->seconds;                                       \
        Uint16 hundredths =                                                    \
            (clock->frames * HUNDREDTHS_PER_SECOND) / STEPS_PER_SECOND;        \
        char text[10];                                                         \
        NJS_POINT3 position;                                                   \
        CAPTION_AT(position, offset_x, offset_y);                              \
        if (minutes || seconds || hundredths) {                                \
            TWO_DIGITS(0, minutes);                                            \
            text[2] = GLYPH_COLON;                                             \
            TWO_DIGITS(3, seconds);                                            \
            text[5] = GLYPH_COLON;                                             \
            TWO_DIGITS(6, hundredths);                                         \
            text[8] = 0;                                                       \
            text[9] = 0;                                                       \
        } else {                                                               \
            EMPTY_DIGITS(0);                                                   \
            text[2] = GLYPH_COLON;                                             \
            EMPTY_DIGITS(3);                                                   \
            text[5] = GLYPH_COLON;                                             \
            EMPTY_DIGITS(6);                                                   \
            text[8] = 0;                                                       \
            text[9] = 0;                                                       \
        }                                                                      \
        garden_draw_world_name(&position, text, PREVIEW_INK, 1.0F);            \
    }
#define CREATE_PREVIEW(prefix)                                                 \
    static void prefix##_destroy(struct task *task)                            \
    {                                                                          \
        (void)task;                                                            \
    }                                                                          \
    static void prefix##_create(struct task *parent, Float x, Float y)         \
    {                                                                          \
        struct task *task;                                                     \
        struct preview_card *card;                                             \
        task = task_create_child(0, prefix##_step, parent);                    \
        task->draw = prefix##_draw;                                            \
        task->destructor = prefix##_destroy;                                   \
        task->work = syCalloc(1, sizeof(*card));                               \
        card = (struct preview_card *)task->work;                              \
        card->x = x;                                                           \
        card->y = y;                                                           \
        card->delay = CARD_DELAY;                                              \
        card->alpha = 0.0F;                                                    \
    }

static void player_record_step(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    struct task *chao;
    NJS_POINT3 position;
    switch (card->mode) {
        SHOW_PLAYER();
        EXIT_PHASES(card);
    }
    HOLD_MENU(card);
    CHECK_EXIT(card, EXIT_DELAY, REMOVE_PREVIEW(0));
}
static void player_record_draw(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_SPRITE sprite;
    CARD_FRAME(PLAYER_CARD);
    if (entrance_menu->preview[0]) {
        struct chao_work *chao = CHAO_WORK(entrance_menu->preview[0]);
        DRAW_NAME(chao->profile.parameters.name, NAME_X, NAME_Y);
        if (entrance_menu->cursor != entrance_menu->back) {
            DRAW_TIME(
                &chao->profile.parameters.personal_records[garden_race_event()],
                TIME_X, TIME_Y);
        }
    }
    CLEAR_COLOR();
}
CREATE_PREVIEW(player_record)

static void show_record_holder(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    struct task *chao;
    struct garden_race_record record;
    NJS_POINT3 position;
    REMOVE_PREVIEW(1);
    garden_race_get_record(garden_race_event(), &record);
    if (record.valid) {
        position.x = (card->x - RIVAL_ORIGIN_X) * WORLD_PER_PIXEL;
        position.y = (HOLDER_ORIGIN_Y - card->y) * WORLD_PER_PIXEL;
        position.z = PREVIEW_Z;
        chao =
            chao_create(NULL, NULL, &record.chao, &position, RIVAL_YAW, NULL);
        if (chao) {
            FREEZE_CHAO(chao);
        }
        entrance_menu->preview[1] = chao;
    }
}
static void record_holder_step(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    switch (card->mode) {
    case PREVIEW_WAIT:
        WAIT_FOR(card, PREVIEW_ENTER);
        break;
    case PREVIEW_ENTER:
        card->alpha += FADE_STEP;
        if (card->alpha > 1.0F) {
            show_record_holder(task);
            card->alpha = 1.0F;
            PREVIEW_MODE(card, PREVIEW_SHOWN);
        }
        break;
    case PREVIEW_SHOWN:
        if (entrance_menu->old_column != entrance_menu->column ||
            entrance_menu->old_cursor != entrance_menu->cursor) {
            if (entrance_menu->cursor != entrance_menu->back) {
                show_record_holder(task);
            } else {
                REMOVE_PREVIEW(1);
            }
        }
        break;
        EXIT_PHASES(card);
    }
    HOLD_MENU(card);
    CHECK_EXIT(card, EXIT_DELAY, REMOVE_PREVIEW(1));
}
static void record_holder_draw(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_SPRITE sprite;
    CARD_FRAME(HOLDER_CARD);
    if (entrance_menu->cursor != entrance_menu->back && !entrance_menu->wait) {
        struct garden_race_record record;
        garden_race_get_record(garden_race_event(), &record);
        DRAW_NAME(record.chao.name, HOLDER_NAME_X, HOLDER_NAME_Y);
        DRAW_TIME(&record.time, HOLDER_TIME_X, HOLDER_TIME_Y);
    }
    CLEAR_COLOR();
}
CREATE_PREVIEW(record_holder)

static void team_player_step(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    struct task *chao;
    NJS_POINT3 position;
    switch (card->mode) {
        SHOW_PLAYER();
        EXIT_PHASES(card);
    }
    HOLD_MENU(card);
    CHECK_EXIT(card, EXIT_DELAY, RELEASE_TEAM_PLAYER());
}
static void team_player_draw(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_SPRITE sprite;
    CARD_FRAME(TEAM_CARD);
    if (entrance_menu->preview[0]) {
        struct chao_work *chao = CHAO_WORK(entrance_menu->preview[0]);
        NJS_POINT3 position;
        position.x = (card->x + entrance_caption_x - NAME_X) * WORLD_PER_PIXEL;
        position.y = -((card->y - NAME_Y) * WORLD_PER_PIXEL) +
                     entrance_caption_y + TEAM_NAME_RAISE;
        position.z = PREVIEW_Z;
        garden_draw_world_name(&position, chao->profile.parameters.name,
                               PREVIEW_INK, 1.0F);
    }
    CLEAR_COLOR();
}
CREATE_PREVIEW(team_player)

static void rival_preview_step(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_POINT3 position;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: save is static, so the record
     * the rival's Chao keeps outlives the step */
    static struct chao_save save;
#else
    /* @bug memory: a pause while the rival's Chao lives writes the Chao's
     * record into this step's returned frame. the step passes &save to
     * garden_spawn_chao at 0x0c9c88e4. chao_create at 0x0c53f282 keeps &save
     * as the Chao's save record. the pause's garden_file_gather at 0x0c50b9ec
     * packs each registered Chao into its record with garden_pack_chao at
     * 0x0c5025e0 */
    struct chao_save save;
#endif
    switch (card->mode) {
    case PREVIEW_WAIT:
        WAIT_FOR(card, PREVIEW_ENTER);
        break;
    case PREVIEW_ENTER:
        card->alpha += FADE_STEP;
        if (card->alpha > 1.0F) {
            card->alpha = 1.0F;
            /* @bug mismatched-call: the step passes an int to
             * garden_race_set_level at 0x0c51639e, which takes a Sint8. the
             * int is 0 at 0x0c9c874a */
            garden_race_set_level_int(0);
            PREVIEW_MODE(card, PREVIEW_SHOWN);
        }
        break;
    case PREVIEW_SHOWN:
        switch (card->phase) {
        case 0:
            garden_race_build_opponents();
            if (card->chao) {
                task_destroy(card->chao);
                card->chao = NULL;
            }
            if (entrance_menu->cursor != entrance_menu->back) {
                struct race_field *field;
                struct race_template *opponent;
                Sint32 featured;
                Sint32 index;
                position.x = (card->x - RIVAL_ORIGIN_X) * WORLD_PER_PIXEL;
                position.y = PREVIEW_HEIGHT -
                             ((card->y - PREVIEW_ORIGIN_Y) * WORLD_PER_PIXEL);
                position.z = PREVIEW_Z;
                switch (garden_race_group()) {
                case RACE_CHALLENGE:
                    field =
                        &challenge_fields[challenge_races[garden_race_level()]
                                              .opponents];
                    break;
                case RACE_DARK_COURSES:
                    field = &dark_fields[dark_land_races[garden_race_level()]
                                             .opponents];
                    break;
                case RACE_HERO_COURSES:
                    field = &hero_fields[hero_land_races[garden_race_level()]
                                             .opponents];
                    break;
#ifdef NONMATCHING
                /* this side shows no rival for a group the switch doesn't name.
                 * for such a group, the matching side goes on with field
                 * unset */
                default:
                    return;
#endif
                }
                featured = field->featured;
                index = field->entrants[featured];
                opponent = &opponent_templates[index];
                garden_race_make_opponent(&save, opponent);
                card->chao = garden_spawn_chao(&save, &position, RIVAL_YAW);
                if (card->chao) {
                    FREEZE_CHAO(card->chao);
                }
            }
            card->phase++;
            /* fall through */
        case 1:
            if (entrance_menu->column != entrance_menu->old_column ||
                entrance_menu->cursor != entrance_menu->old_cursor) {
                garden_race_set_level((entrance_menu->cursor * GRID_COLUMNS) +
                                      entrance_menu->column);
                if (card->chao) {
                    task_kill(card->chao);
                    card->chao = NULL;
                }
                card->phase = 0;
            }
            break;
        }
        break;
        EXIT_PHASES(card);
    }
    HOLD_MENU(card);
    CHECK_EXIT(card, EXIT_DELAY, if (card->chao) { task_kill(card->chao); });
}
static void rival_preview_draw(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_SPRITE sprite;
    CARD_FRAME(RIVAL_CARD);
    CLEAR_COLOR();
}
CREATE_PREVIEW(rival_preview)

static void course_map_step(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    switch (card->mode) {
    case PREVIEW_WAIT:
        WAIT_FOR(card, PREVIEW_ENTER);
        break;
    case PREVIEW_ENTER:
        card->alpha += FADE_STEP;
        if (card->alpha > 1.0F) {
            card->alpha = 1.0F;
            PREVIEW_MODE(card, PREVIEW_SHOWN);
        }
        break;
    case PREVIEW_SHOWN:
        break;
        EXIT_PHASES(card);
    }
    HOLD_MENU(card);
    CHECK_EXIT(card, EXIT_DELAY, (void)0);
}
static void course_map_draw(struct task *task)
{
    struct preview_card *card = (struct preview_card *)task->work;
    NJS_SPRITE sprite;
    CARD_SPRITE(card);
    constant_material_set(card->alpha, 1.0F, 1.0F, 1.0F);
    sprite.p.y = card->y;
    DRAW(MAP_CARD, -32.2F);
    DRAW(MAP_BACK, -32.31F);
    DRAW(MAP_EDGE, -32.32F);
    switch (garden_race_group()) {
    case RACE_BEGINNER:
        BEGINNER_COLOR(card->alpha);
        break;
    case RACE_JEWEL:
        JEWEL_COLOR(card->alpha);
        break;
    case RACE_CHALLENGE:
        CHALLENGE_COLOR(card->alpha);
        break;
    case RACE_DARK_COURSES:
        DARK_COURSES_COLOR(card->alpha);
        break;
    case RACE_HERO_COURSES:
        HERO_COURSES_COLOR(card->alpha);
        break;
    case RACE_MULTI:
        MULTI_COLOR(card->alpha);
        break;
    }
    DRAW(MAP_FRAME, -32.3F);
    if (entrance_menu->cursor != entrance_menu->back) {
        sprite.p.y += MAP_DROP;
        sprite.tlist = &entrance_map_texlist;
        sprite.tanim = entrance_map_cells;
        sprite.ang = QUARTER_TURN;
        sprite.sy = MAP_HEIGHT;
        constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
        njDrawSprite2D(&sprite, garden_race_course()->track, -1.4F,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
        njDrawSprite2D(&sprite, RACE_RECORD_TRACKS, -1.5F,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE);
    }
    CLEAR_COLOR();
}
CREATE_PREVIEW(course_map)

static void preview_rail_step(struct task *task)
{
    struct preview_rail *rail = (struct preview_rail *)task->work;
    switch (rail->mode) {
    case PREVIEW_WAIT:
        WAIT_FOR(rail, PREVIEW_ENTER);
        break;
    case PREVIEW_ENTER:
        rail->height += RAIL_STEP;
        if (rail->height > rail->limit) {
            rail->height = rail->limit;
            PREVIEW_MODE(rail, PREVIEW_SHOWN);
        }
        break;
    case PREVIEW_SHOWN:
        break;
    case PREVIEW_EXIT_DELAY:
        WAIT_FOR(rail, PREVIEW_LEAVE);
        break;
    case PREVIEW_LEAVE:
        rail->height -= RAIL_STEP;
        if (rail->height < RAIL_SHUT) {
            rail->height = RAIL_SHUT;
            task_kill(task);
        }
        break;
    }
    HOLD_MENU(rail);
    CHECK_EXIT(rail, RAIL_EXIT_DELAY, (void)0);
}
static void preview_rail_draw(struct task *task)
{
    struct preview_rail *rail = (struct preview_rail *)task->work;
    NJS_SPRITE sprite;
    CARD_SPRITE(rail);
    sprite.p.y = rail->y - (rail->height * 0.5F);
    DRAW(RAIL_TOP_CELLS, -32.4F);
    DRAW(RAIL_TOP_CELLS + 1, -32.4F);
    DRAW(RAIL_TOP_CELLS + 2, -32.4F);
    sprite.p.y = rail->y + (rail->height * 0.5F);
    DRAW(RAIL_BOTTOM_CELLS, -32.4F);
    DRAW(RAIL_BOTTOM_CELLS + 1, -32.4F);
    DRAW(RAIL_BOTTOM_CELLS + 2, -32.4F);
    sprite.p.y = rail->y;
    sprite.sy = (rail->height * 0.5F) - 8.0F;
    DRAW(RAIL_CELLS, -32.4F);
    DRAW(RAIL_CELLS + 1, -32.4F);
    DRAW(RAIL_CELLS + 2, -32.4F);
    CLEAR_COLOR();
}
static void preview_rail_destroy(struct task *task)
{
    (void)task;
}
#define PLACE_RAIL(max_height)                                                 \
    rail->x = RAIL_X;                                                          \
    rail->y = RAIL_Y;                                                          \
    rail->limit = (max_height)
#define RIVAL_CARDS()                                                          \
    PLACE_RAIL(RAIL_HEIGHT);                                                   \
    team_player_create(parent, rail->x, rail->y - TOP_CARD_UP);                \
    rival_preview_create(parent, rail->x, rail->y - MIDDLE_CARD_UP);           \
    course_map_create(parent, rail->x, rail->y + BOTTOM_CARD_DOWN)

void entrance_course_preview(struct task *parent)
{
    struct task *task;
    struct preview_rail *rail;
    task = task_create_child(0, preview_rail_step, parent);
    task->draw = preview_rail_draw;
    task->destructor = preview_rail_destroy;
    task->work = syCalloc(1, sizeof(*rail));
    rail = (struct preview_rail *)task->work;
    rail->delay = RAIL_DELAY;
    rail->height = RAIL_SHUT;
    switch (garden_race_group()) {
    case RACE_BEGINNER:
    case RACE_JEWEL:
        PLACE_RAIL(RAIL_HEIGHT);
        player_record_create(parent, rail->x, rail->y - TOP_CARD_UP);
        record_holder_create(parent, rail->x, rail->y - MIDDLE_CARD_UP);
        course_map_create(parent, rail->x, rail->y + BOTTOM_CARD_DOWN);
        break;
    /* @hack spelling: a case for each group of the rivals' grid, where
     * they could share one */
    case RACE_CHALLENGE:
        RIVAL_CARDS();
        break;
    case RACE_DARK_COURSES:
        RIVAL_CARDS();
        break;
    case RACE_HERO_COURSES:
        RIVAL_CARDS();
        break;
    case RACE_MULTI:
        PLACE_RAIL(MULTI_RAIL_HEIGHT);
        record_holder_create(parent, rail->x, rail->y - MULTI_TOP_CARD_UP);
        course_map_create(parent, rail->x, rail->y + MULTI_BOTTOM_CARD_DOWN);
        break;
    }
}
