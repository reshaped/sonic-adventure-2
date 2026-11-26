/* the menu's title, which enters from the right, and its caption, which
 * enters from the left. each slides in, grows its mark and fades in its
 * plate. on the Chao and course screens the title is the race group's, in
 * the group's color */
#include <ninja.h>
#include <sg_xpt.h>
#include "chao/race.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/graphics.h"
#include "chaostgentrance/menu.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"

enum title_mode {
    TITLE_INIT,
    TITLE_DELAY,
    TITLE_ENTER,
    TITLE_GROW,
    TITLE_FADE_IN,
    TITLE_SHOWN,
    TITLE_EXIT_DELAY,
    TITLE_FADE_OUT,
    TITLE_SHRINK,
    TITLE_LEAVE,
    TITLE_FINISHED
};

/* the title's work, and the caption's */
struct title_work {
    Sint8 mode, phase;
    Uint8 id, age;
    Sint16 flags;
    /* it waits delay + 1 frames to enter, and delay + 1 to leave */
    Uint16 delay;
    Sint32 name; /* its first cell on the language's title sheet */
    Sint32 plate; /* its plate's cell on the common sheet */
    Uint32 unk_10;
    Float slide; /* the offset from its place, which entering brings to 0 */
    Uint8 unk_18[8];
    Float x; /* the caption's place on x */
    /* the scale of the mark, the cells drawn last, from MARK_SMALL to 1 */
    Float scale;
    Float alpha; /* the plate's alpha */
    Uint32 unk_2c;
};

/* the title stands at TITLE_X, TITLE_Y, and the caption at its x,
 * CAPTION_Y. each starts and ends OFFSCREEN pixels from its place */
#define TITLE_X 60.0F
#define TITLE_Y 64.0F
#define CAPTION_Y 104.0F
#define OFFSCREEN 600.0F
/* the caption's x on the group and Chao screens, and on the course screen */
#define CAPTION_X 584.0F
#define COURSE_CAPTION_X 345.0F
/* each frame of its entry, the slide shrinks to SLIDE_EASE of itself and
 * moves SLIDE_PUSH further toward 0. as it leaves, it grows by LEAVE_GROWTH
 * of itself and moves LEAVE_PUSH further from 0 */
#define SLIDE_EASE 0.95F
#define SLIDE_PUSH 2.0F
#define LEAVE_GROWTH 0.05F
#define LEAVE_PUSH 5.0F
#define MARK_SMALL 0.75F
#define GROW_STEP 0.015F
#define SHRINK_STEP 0.02F
#define FADE_IN_STEP 0.04F
#define FADE_OUT_STEP 0.08F
/* 1 / (1 - MARK_SMALL), which takes the mark's scale to 0 to 1. a small
 * mark stands MARK_SHIFT right of its place on the title. on the caption it
 * stands CAPTION_MARK_SHIFT left of it */
#define MARK_RANGE 4.0F
#define MARK_SHIFT 20.0F
#define CAPTION_MARK_SHIFT 12.0F
/* a title is the run of cells from its first. the run ends before the next
 * cell whose height is neither TALL_CELL nor SHORT_CELL. it ends before a
 * cell centered at x START_CX or 0 too */
#define TALL_CELL 32
#define SHORT_CELL 16
#define START_CX (-32)

#define BEGIN_TITLE(xpos, ypos)                                                \
    sprite.p.x = (xpos);                                                       \
    sprite.p.y = (ypos);                                                       \
    sprite.p.z = 0.0F;                                                         \
    sprite.ang = 0;                                                            \
    sprite.sx = 1.0F;                                                          \
    sprite.sy = 1.0F;                                                          \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR,                                 \
                        NJD_COLOR_BLENDING_INVSRCALPHA);                       \
    if (game_state.text_language == TEXT_JAPANESE) {                           \
        sprite.tlist = &entrance_japanese_titles;                              \
        sprite.tanim = (NJS_TEXANIM *)entrance_japanese_title_cells;           \
    } else {                                                                   \
        sprite.tlist = &entrance_english_titles;                               \
        sprite.tanim = (NJS_TEXANIM *)entrance_english_title_cells;            \
    }

#define TITLE_CELL(cell, depth) ENTRANCE_DRAW_CELL(sprite, (cell), (depth))
static void title_draw(struct task *task)
{
    struct title_work *title = (struct title_work *)task->work;
    NJS_SPRITE sprite;
    const NJS_TEXANIM *cell;
    int index;
    BEGIN_TITLE(title->slide + TITLE_X, TITLE_Y);
    index = title->name;
    cell = &sprite.tanim[index];
    do {
        njDrawSprite2D(&sprite, index, -50.0F, cell->attr);
        index++;
        cell = &sprite.tanim[index];
    } while ((cell->sy == TALL_CELL || cell->sy == SHORT_CELL) &&
             cell->cx != START_CX && cell->cx != 0);
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    constant_material_set(title->alpha, 1.0F, 1.0F, 1.0F);
    /* @hack spelling: the plate's cell through index, where title->plate
     * would do */
    index = title->plate;
    TITLE_CELL(index, -50.0F);
    RGB(255, 255, 255);
    TITLE_CELL(2, -51.5F);
    switch (entrance_menu->screen) {
    case ENTRANCE_SELECT_GROUP:
        RGB(239, 124, 112);
        break;
    case ENTRANCE_SELECT_CHAO:
    case ENTRANCE_SELECT_COURSE:
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            BEGINNER_COLOR(1.0F);
            break;
        case RACE_JEWEL:
            JEWEL_COLOR(1.0F);
            break;
        case RACE_CHALLENGE:
            CHALLENGE_COLOR(1.0F);
            break;
        case RACE_DARK_COURSES:
            DARK_COURSES_COLOR(1.0F);
            break;
        case RACE_HERO_COURSES:
            HERO_COURSES_COLOR(1.0F);
            break;
        case RACE_MULTI:
            MULTI_COLOR(1.0F);
            break;
        }
        break;
    }
    TITLE_CELL(3, -50.2F);
    TITLE_CELL(4, -51.4F);
    sprite.p.x =
        title->slide +
        ((1.0F - ((title->scale - MARK_SMALL) * MARK_RANGE)) * MARK_SHIFT) +
        TITLE_X;
    sprite.sx = title->scale;
    sprite.sy = title->scale;
    TITLE_CELL(1, -50.1F);
    RGB(255, 255, 255);
    TITLE_CELL(0, -50.25F);
}

static void title_destroy(struct task *task)
{
    entrance_menu->title = NULL;
}

#define TITLE_ART(name_cell, plate_cell)                                       \
    {                                                                          \
        title->name = (name_cell);                                             \
        title->plate = (plate_cell);                                           \
    }

/* @hack spelling: select_title_art inlined where it is called, where a call
 * would do */
#pragma inline(select_title_art)
/* set the title's cells for the screen shown. the japanese and english title
 * sheets hold their titles in one order */
static void select_title_art(struct title_work *title)
{
    switch (entrance_menu->screen) {
    case ENTRANCE_SELECT_GROUP:
        TITLE_ART(0, 46);
        break;
    case ENTRANCE_SELECT_COURSE:
    case ENTRANCE_SELECT_CHAO:
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            TITLE_ART(3, 47);
            break;
        case RACE_JEWEL:
            TITLE_ART(6, 48);
            break;
        case RACE_CHALLENGE:
            TITLE_ART(8, 53);
            break;
        case RACE_DARK_COURSES:
            TITLE_ART(11, 49);
            break;
        case RACE_HERO_COURSES:
            TITLE_ART(12, 50);
            break;
        case RACE_MULTI:
            TITLE_ART(14, 51);
            break;
        }
        break;
    }
}

#define TITLE_MODE(next)                                                       \
    {                                                                          \
        title->mode = (next);                                                  \
        title->phase = 0;                                                      \
    }
#define TITLE_START(offset)                                                    \
    title->slide = (offset);                                                   \
    title->scale = MARK_SMALL;                                                 \
    title->alpha = 0.0F;                                                       \
    TITLE_MODE(TITLE_DELAY)
#define TITLE_WAIT_TO_ENTER()                                                  \
    if (title->age++ > title->delay) {                                         \
        TITLE_MODE(TITLE_ENTER);                                               \
        title->age = 0;                                                        \
    }
#define TITLE_GROW_MARK()                                                      \
    if (title->scale < 1.0F) {                                                 \
        title->scale += GROW_STEP;                                             \
    } else {                                                                   \
        title->scale = 1.0F;                                                   \
        TITLE_MODE(TITLE_FADE_IN);                                             \
    }
#define TITLE_REVEAL_PLATE()                                                   \
    if (title->alpha < 1.0F) {                                                 \
        title->alpha += FADE_IN_STEP;                                          \
    } else {                                                                   \
        title->alpha = 1.0F;                                                   \
        TITLE_MODE(TITLE_SHOWN);                                               \
    }
#define TITLE_WAIT_TO_LEAVE()                                                  \
    if (title->age++ > title->delay) {                                         \
        TITLE_MODE(TITLE_FADE_OUT);                                            \
    }
#define TITLE_HIDE_PLATE()                                                     \
    if (title->alpha > 0.0F) {                                                 \
        title->alpha -= FADE_OUT_STEP;                                         \
    } else {                                                                   \
        title->alpha = 0.0F;                                                   \
        TITLE_MODE(TITLE_SHRINK);                                              \
    }
#define TITLE_SHRINK_MARK()                                                    \
    if (title->scale > MARK_SMALL) {                                           \
        title->scale -= SHRINK_STEP;                                           \
    } else {                                                                   \
        title->scale = MARK_SMALL;                                             \
        TITLE_MODE(TITLE_LEAVE);                                               \
    }

static void title_step(struct task *task)
{
    struct title_work *title = (struct title_work *)task->work;
    switch (title->mode) {
    case TITLE_INIT:
        /* @hack redundant: a test of the language whose branches make one
         * call, where the call alone would do */
        if (game_state.text_language == TEXT_JAPANESE) {
            select_title_art(title);
        } else {
            select_title_art(title);
        }
        TITLE_START(OFFSCREEN);
        break;
    case TITLE_DELAY:
        TITLE_WAIT_TO_ENTER();
        break;
    case TITLE_ENTER:
        title->slide = (title->slide * SLIDE_EASE) - SLIDE_PUSH;
        if (title->slide < 0.0F) {
            title->slide = 0.0F;
            TITLE_MODE(TITLE_GROW);
        }
        break;
    case TITLE_GROW:
        TITLE_GROW_MARK();
        break;
    case TITLE_FADE_IN:
        TITLE_REVEAL_PLATE();
        break;
    case TITLE_SHOWN:
        break;
    case TITLE_EXIT_DELAY:
        TITLE_WAIT_TO_LEAVE();
        break;
    case TITLE_FADE_OUT:
        TITLE_HIDE_PLATE();
        break;
    case TITLE_SHRINK:
        TITLE_SHRINK_MARK();
        break;
    case TITLE_LEAVE:
        title->slide += (title->slide * LEAVE_GROWTH) + LEAVE_PUSH;
        if (title->slide > OFFSCREEN) {
            TITLE_MODE(TITLE_FINISHED);
        }
        break;
    case TITLE_FINISHED:
        task_kill(task);
        break;
    }
    switch (title->mode) {
    case TITLE_INIT:
    case TITLE_ENTER:
    case TITLE_GROW:
    case TITLE_EXIT_DELAY:
    case TITLE_FADE_OUT:
    case TITLE_SHRINK:
    case TITLE_LEAVE:
    case TITLE_FINISHED:
        entrance_menu->wait = TRUE;
        break;
    }
    /* the Chao and course screens show the same title, so a select or back
     * between them keeps it */
    if (title->mode < TITLE_EXIT_DELAY) {
        if (entrance_menu->request == ENTRANCE_MENU_SELECT &&
            entrance_menu->screen == ENTRANCE_SELECT_GROUP) {
            TITLE_MODE(TITLE_EXIT_DELAY);
        }
        if (entrance_menu->request == ENTRANCE_MENU_BACK &&
            entrance_menu->screen == ENTRANCE_SELECT_CHAO) {
            TITLE_MODE(TITLE_EXIT_DELAY);
        }
    }
    if (title->mode < TITLE_EXIT_DELAY &&
        entrance_menu->request == ENTRANCE_MENU_LEAVE) {
        TITLE_MODE(TITLE_EXIT_DELAY);
    }
}

void entrance_title_create(struct task *parent, Sint16 delay)
{
    struct task *task;
    if (!entrance_menu->title) {
        task = task_create_child(TASK_WORK, title_step, parent);
        task->draw = title_draw;
        task->destructor = title_destroy;
        ((struct title_work *)task->work)->delay = delay;
        entrance_menu->title = task;
    }
}

static void caption_draw(struct task *task)
{
    struct title_work *title = (struct title_work *)task->work;
    NJS_SPRITE sprite;
    /* @hack spelling: the name's cell and the plate's through index, where
     * title->name and title->plate would do */
    int index;
    BEGIN_TITLE(title->x + title->slide, CAPTION_Y);
    index = title->name;
    TITLE_CELL(index, -60.0F);
    sprite.tlist = &entrance_common_texlist;
    sprite.tanim = (NJS_TEXANIM *)entrance_common_cells;
    constant_material_set(title->alpha, 1.0F, 1.0F, 1.0F);
    index = title->plate;
    TITLE_CELL(index, -50.0F);
    RGB(255, 255, 255);
    TITLE_CELL(7, -60.14F);
    switch (entrance_menu->screen) {
    case ENTRANCE_SELECT_GROUP:
        RGB(190, 96, 255);
        break;
    case ENTRANCE_SELECT_CHAO:
        RGB(181, 228, 114);
        break;
    case ENTRANCE_SELECT_COURSE:
        RGB(248, 201, 58);
        break;
    }
    TITLE_CELL(8, -60.13F);
    TITLE_CELL(9, -51.2F);
    sprite.p.x = title->x + title->slide -
                 ((1.0F - ((title->scale - MARK_SMALL) * MARK_RANGE)) *
                  CAPTION_MARK_SHIFT);
    sprite.sx = title->scale;
    sprite.sy = title->scale;
    TITLE_CELL(6, -50.1F);
    RGB(255, 255, 255);
    TITLE_CELL(5, -51.25F);
}

static void caption_destroy(struct task *task)
{
    entrance_menu->caption = NULL;
}

static void caption_step(struct task *task)
{
    struct title_work *title = (struct title_work *)task->work;
    switch (title->mode) {
    case TITLE_INIT:
        switch (entrance_menu->screen) {
        case ENTRANCE_SELECT_GROUP:
            TITLE_ART(2, 52);
            break;
        case ENTRANCE_SELECT_CHAO:
            TITLE_ART(1, 54);
            break;
        case ENTRANCE_SELECT_COURSE:
            TITLE_ART(2, 52);
            break;
        }
        switch (entrance_menu->screen) {
        case ENTRANCE_SELECT_GROUP:
        case ENTRANCE_SELECT_CHAO:
            title->x = CAPTION_X;
            break;
        case ENTRANCE_SELECT_COURSE:
            title->x = COURSE_CAPTION_X;
            break;
        }
        TITLE_START(-OFFSCREEN);
        break;
    case TITLE_DELAY:
        TITLE_WAIT_TO_ENTER();
        break;
    case TITLE_ENTER:
        title->slide = (title->slide * SLIDE_EASE) + SLIDE_PUSH;
        if (title->slide > 0.0F) {
            title->slide = 0.0F;
            TITLE_MODE(TITLE_GROW);
        }
        break;
    case TITLE_GROW:
        TITLE_GROW_MARK();
        break;
    case TITLE_FADE_IN:
        TITLE_REVEAL_PLATE();
        break;
    case TITLE_SHOWN:
        break;
    case TITLE_EXIT_DELAY:
        TITLE_WAIT_TO_LEAVE();
        break;
    case TITLE_FADE_OUT:
        TITLE_HIDE_PLATE();
        break;
    case TITLE_SHRINK:
        TITLE_SHRINK_MARK();
        break;
    case TITLE_LEAVE:
        title->slide += (title->slide * LEAVE_GROWTH) - LEAVE_PUSH;
        if (title->slide < -OFFSCREEN) {
            TITLE_MODE(TITLE_FINISHED);
        }
        break;
    case TITLE_FINISHED:
        task_kill(task);
        break;
    }
    switch (title->mode) {
    case TITLE_FADE_IN:
    case TITLE_SHOWN:
        break;
    default:
        entrance_menu->wait = TRUE;
        break;
    }
    if (title->mode < TITLE_EXIT_DELAY) {
        if (entrance_menu->request == ENTRANCE_MENU_SELECT ||
            entrance_menu->request == ENTRANCE_MENU_BACK) {
            TITLE_MODE(TITLE_EXIT_DELAY);
        }
    }
}

void entrance_caption_create(struct task *parent, Sint16 delay)
{
    struct task *task;
    if (!entrance_menu->caption) {
        task = task_create_child(TASK_WORK, caption_step, parent);
        task->draw = caption_draw;
        task->destructor = caption_destroy;
        ((struct title_work *)task->work)->delay = delay;
        entrance_menu->caption = task;
    }
}
