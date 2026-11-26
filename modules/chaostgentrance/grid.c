/* the rivals' grid of the challenge, dark and hero groups, a tile a level.
 * the panel above it names the level under the cursor */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "chao/messages.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "chaostgentrance/options.h"
#include "chaostgentrance/rows.h"
#include "chaostgentrance/slide.h"
#include "draw/text_sprite.h"
#include "system/task.h"

/* a tile shows the rival of a level won. the next level shows its rival
 * dimmed once it is unlocked, and a mark before then. each level past it
 * shows the mark smaller and dimmed */
enum tile_state {
    TILE_FINISHED,
    TILE_NEXT_RIVAL,
    TILE_UNKNOWN_RIVAL,
    TILE_LOCKED
};
/* a tile's delay is TILE_DELAY, TILE_STAGGER less a column right and
 * TILE_STAGGER more a row down */
#define TILE_LEFT 68.0F
#define TILE_TOP 197.0F
#define TILE_WIDTH 88.0F
#define TILE_HEIGHT 72.0F
#define TILE_DELAY 60
#define TILE_STAGGER 4
#define LOCKED_SCALE 0.85F
/* when the cursor moves between levels, the panel hides its caption for
 * CAPTION_RELOAD frames. it makes the next caption in that time. a caption
 * wider than CAPTION_WIDTH squeezes to fit it */
#define CAPTION_RELOAD 2
#define CAPTION_WIDTH 272
/* the caption draws at CAPTION_HEIGHT of its height, over its shadow,
 * SHADOW_OFFSET pixels right and down */
#define CAPTION_HEIGHT 0.88F
#define SHADOW_OFFSET 2.0F
#define SHADOW_COLOR 0xff1050b0
#define PANEL_X 200.0F
#define PANEL_Y 146.0F
#define PANEL_DELAY 39

/* cells of the common sheet. CHALLENGE_PANEL_CELLS and PANEL_CELLS each
 * begin a run of three */
#define TILE_CELL 89
#define LIT_TILE_CELL 90
#define MARK_SHADOW_CELL 91
#define MARK_CELL 92
#define PANEL_CELLS 127
#define CHALLENGE_PANEL_CELLS 130

struct event_tile {
    Sint8 mode, phase;
    Uint8 unk_02, age;
    Uint16 unk_04, delay;
    Sint32 unk_08;
    union {
        Uint16 state, reload;
    } detail;
    Uint16 index, column, row;
    Float x, y, slide;
    Uint8 unk_20[16];
    Sint32 rival;
};

/* each race's caption, a garden_message id in msgChaoEntrance, by the race's
 * index in its group. a row here is a row of the grid */
/* clang-format off */
static Sint32 challenge_captions[RACE_CHALLENGE_LEVELS] = {
    92, 79, 78, 84,
    83, 82, 76, 75,
    77, 80, 81, 87};
static Sint32 dark_captions[RACE_ALIGNMENT_LEVELS] = {94, 95, 96, 97};
static Sint32 hero_captions[RACE_ALIGNMENT_LEVELS] = {99, 100, 101, 102};
/* clang-format on */
static struct text_sprite *event_caption = NULL;
static Uint16 caption_width, caption_height;

static void event_tile_step(struct task *task)
{
    struct event_tile *tile = (struct event_tile *)task->work;
    switch (tile->mode) {
    case ROW_WAIT:
        ROW_WAIT_FOR(tile, ROW_ENTER);
        break;
    case ROW_ENTER:
        ROW_SLIDE_IN(tile, (void)0);
        break;
    case ROW_SHOWN:
        break;
    case ROW_WAIT_TO_LEAVE:
        ROW_WAIT_FOR(tile, ROW_LEAVE);
        break;
    case ROW_LEAVE:
        ROW_SLIDE_OUT(task, tile);
        break;
    }
    ROW_CHECK_REQUEST(tile);
}

static void event_tile_draw(struct task *task)
{
    struct event_tile *tile = (struct event_tile *)task->work;
    NJS_SPRITE sprite;
    START_ROW_SPRITE(sprite, tile);
    RGB(255, 255, 255);
    if (entrance_menu->column == tile->column &&
        entrance_menu->cursor == tile->row) {
        DRAW(LIT_TILE_CELL, -11.2F);
    } else {
        DRAW(TILE_CELL, -11.2F);
    }
    RGB(255, 255, 255);
    switch (tile->detail.state) {
    case TILE_FINISHED:
        sprite.tlist = &entrance_rival_texlist;
        sprite.tanim = (NJS_TEXANIM *)entrance_rival_cells;
        DRAW(tile->rival, -11.3F);
        break;
    case TILE_NEXT_RIVAL:
        RGB(100, 100, 100);
        sprite.tlist = &entrance_rival_texlist;
        sprite.tanim = (NJS_TEXANIM *)entrance_rival_cells;
        DRAW(tile->rival, -11.3F);
        break;
    case TILE_UNKNOWN_RIVAL:
        RGB(0, 0, 0);
        DRAW(MARK_SHADOW_CELL, -11.4F);
        RGB(255, 255, 255);
        DRAW(MARK_CELL, -11.3F);
        break;
    case TILE_LOCKED:
        RGB(0, 0, 0);
        DRAW(MARK_SHADOW_CELL, -11.4F);
        RGB(100, 100, 100);
        sprite.sx = LOCKED_SCALE;
        sprite.sy = LOCKED_SCALE;
        DRAW(MARK_CELL, -11.3F);
        sprite.sx = 1.0F;
        sprite.sy = 1.0F;
        break;
    }
    CLEAR_COLOR();
}

static void event_tile_destroy(struct task *task)
{
}

void entrance_event_tiles(struct task *parent)
{
    struct task *task;
    struct event_tile *tile;
    int count;
    int finished;
    int unlocked;
    int index;

    switch (garden_race_group()) {
    case RACE_CHALLENGE:
        finished = garden_race_best_level(RACE_CHALLENGE_EVENT);
        unlocked = garden_race_unlocked(RACE_CHALLENGE_EVENT);
        if (finished < FIRST_CHALLENGES - 1) {
            count = FIRST_CHALLENGES;
        } else {
            count = RACE_CHALLENGE_LEVELS;
        }
        break;
    case RACE_DARK_COURSES:
        count = RACE_ALIGNMENT_LEVELS;
        finished = garden_race_best_level(RACE_DARK_EVENT);
        unlocked = garden_race_unlocked(RACE_DARK_EVENT);
        break;
    case RACE_HERO_COURSES:
        count = RACE_ALIGNMENT_LEVELS;
        finished = garden_race_best_level(RACE_HERO_EVENT);
        unlocked = garden_race_unlocked(RACE_HERO_EVENT);
        break;
#ifdef NONMATCHING
    /* this side shows no tiles for a group the switch doesn't name. for such a
     * group, the matching side goes on with the counts unset */
    default:
        return;
#endif
    }
    for (index = 0; index < count; index++) {
        task = task_create_child(0, event_tile_step, parent);
        task->draw = event_tile_draw;
        task->destructor = event_tile_destroy;
        task->work = syCalloc(1, sizeof(*tile));
        tile = (struct event_tile *)task->work;
        tile->slide = ROW_OUT;
        tile->index = index;
        tile->column = index % GRID_COLUMNS;
        tile->row = index / GRID_COLUMNS;
        tile->x = (tile->column * TILE_WIDTH) + TILE_LEFT;
        tile->y = (tile->row * TILE_HEIGHT) + TILE_TOP;
        tile->delay = TILE_DELAY - (tile->column * TILE_STAGGER) +
                      (tile->row * TILE_STAGGER);
        if (finished >= index) {
            tile->detail.state = TILE_FINISHED;
        } else if (finished == index - 1) {
            if (unlocked == index - 1) {
                tile->detail.state = TILE_UNKNOWN_RIVAL;
            } else {
                tile->detail.state = TILE_NEXT_RIVAL;
            }
        } else {
            tile->detail.state = TILE_LOCKED;
        }
        switch (garden_race_group()) {
        case RACE_CHALLENGE:
            tile->rival = challenge_races[index].opponents;
            break;
        case RACE_DARK_COURSES:
            tile->rival =
                dark_land_races[index].opponents + RACE_CHALLENGE_LEVELS;
            break;
        case RACE_HERO_COURSES:
            tile->rival = hero_land_races[index].opponents +
                          RACE_CHALLENGE_LEVELS + RACE_ALIGNMENT_LEVELS;
            break;
        }
    }
}

/* @hack spelling: a macro, expanded at each use, where a function would do */
#define SET_EVENT_CAPTION(index)                                               \
    switch (garden_race_group()) {                                             \
    case RACE_CHALLENGE:                                                       \
        event_caption =                                                        \
            garden_message_sprite(garden_message(challenge_captions[index]),   \
                                  &caption_width, &caption_height);            \
        break;                                                                 \
    case RACE_DARK_COURSES:                                                    \
        event_caption =                                                        \
            garden_message_sprite(garden_message(dark_captions[index]),        \
                                  &caption_width, &caption_height);            \
        break;                                                                 \
    case RACE_HERO_COURSES:                                                    \
        event_caption =                                                        \
            garden_message_sprite(garden_message(hero_captions[index]),        \
                                  &caption_width, &caption_height);            \
        break;                                                                 \
    }

static void event_info_step(struct task *task)
{
    struct event_tile *tile = (struct event_tile *)task->work;
    int index;
    switch (tile->mode) {
    case ROW_WAIT:
        ROW_WAIT_FOR(tile, ROW_ENTER);
        break;
    case ROW_ENTER:
        ROW_SLIDE_IN(tile, (void)0);
        break;
    case ROW_SHOWN:
        if (tile->detail.reload > 0) {
            if (tile->detail.reload == CAPTION_RELOAD) {
                index = (entrance_menu->cursor * GRID_COLUMNS) +
                        entrance_menu->column;
                if (event_caption) {
                    text_sprite_destroy(event_caption);
                }
                SET_EVENT_CAPTION(index);
            }
            tile->detail.reload--;
        }
        if ((entrance_menu->old_column != entrance_menu->column ||
             entrance_menu->old_cursor != entrance_menu->cursor) &&
            entrance_menu->cursor != entrance_menu->back &&
            entrance_menu->old_cursor != entrance_menu->back) {
            tile->detail.reload = CAPTION_RELOAD;
        }
        break;
    case ROW_WAIT_TO_LEAVE:
        ROW_WAIT_FOR(tile, ROW_LEAVE);
        break;
    case ROW_LEAVE:
        ROW_SLIDE_OUT(task, tile);
        break;
    }
    ROW_CHECK_REQUEST(tile);
}

static void event_info_draw(struct task *task)
{
    struct event_tile *tile = (struct event_tile *)task->work;
    NJS_SPRITE sprite;
    NJS_POINT3 origin;
    Uint32 color;
    Float sx;
    Float sy;
    START_ROW_SPRITE(sprite, tile);
    if (garden_race_group() == RACE_CHALLENGE) {
        DRAW(CHALLENGE_PANEL_CELLS, -11.4F);
        DRAW(CHALLENGE_PANEL_CELLS + 1, -11.4F);
        DRAW(CHALLENGE_PANEL_CELLS + 2, -11.4F);
    } else {
        DRAW(PANEL_CELLS, -11.4F);
        DRAW(PANEL_CELLS + 1, -11.4F);
        DRAW(PANEL_CELLS + 2, -11.4F);
    }
    if (tile->detail.reload == 0 &&
        entrance_menu->cursor != entrance_menu->back) {
        sx = 1.0F;
        sy = CAPTION_HEIGHT;
        if (caption_width > CAPTION_WIDTH) {
            sx = (Float)CAPTION_WIDTH / caption_width;
        }
        origin.x = tile->x + tile->slide - (caption_width * sx * 0.5F);
        origin.y = tile->y - (caption_height * sy * 0.5F);
        color = SHADOW_COLOR;
        text_sprite_draw_text(event_caption, origin.x + SHADOW_OFFSET,
                              origin.y + SHADOW_OFFSET, sx, sy, &color);
        color = 0xffffffff;
        text_sprite_draw_text(event_caption, origin.x, origin.y, sx, sy,
                              &color);
    }
    CLEAR_COLOR();
}

static void event_info_destroy(struct task *task)
{
    if (event_caption) {
        text_sprite_destroy(event_caption);
    }
    event_caption = NULL;
}

void entrance_event_info(struct task *parent)
{
    struct task *task;
    struct event_tile *tile;
    entrance_message_set(COURSES_MESSAGE);
    task = task_create_child(0, event_info_step, parent);
    task->draw = event_info_draw;
    task->destructor = event_info_destroy;
    task->work = syCalloc(1, sizeof(*tile));
    tile = (struct event_tile *)task->work;
    tile->x = PANEL_X;
    tile->y = PANEL_Y;
    tile->delay = PANEL_DELAY;
    tile->slide = ROW_FAR_OUT;
    SET_EVENT_CAPTION(0);
}
