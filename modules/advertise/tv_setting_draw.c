/* the TV setting's panel. each step, the task moves the view toward what
 * tv_setting_work asks for. a choice grows and fades in as it swings in */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_projection.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/tv_setting.h"
#include "advertise/tv_setting_draw.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "shared/message_sprite.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/tv_setting_draw/text.inc"

#define TASK_LEVEL 2
/* a fade runs from 0 to 1 in FADE_STEPS steps */
#define FADE_STEPS 30
#define FADE_STEP (1.0F / FADE_STEPS)
/* the choices turn between 0 and TURN_END degrees, TURN_STEP a step.
 * menu_panel_point swings each in from a circle of CHOICE_RADIUS, moved
 * CHOICE_DEPTH along z */
#define TURN_END 180
#define TURN_STEP 6
#define CHOICE_DEPTH (-100.0F)
#define CHOICE_RADIUS 50.0F
#define BACKDROP_PRIORITY (-9000.0F)
#define BACKDROP_TEXTURE 10
#define TEST_TEXTURE 0
/* choice n draws the cell LIT_CELL + n while lit, and DIM_CELL + n while
 * dim. its button, LIT_BUTTON_CELL or DIM_BUTTON_CELL, draws BUTTON_DEPTH
 * behind it */
#define LIT_BUTTON_CELL 0
#define DIM_BUTTON_CELL 1
#define LIT_CELL 2
#define DIM_CELL 5
#define CHOICE_PRIORITY (-9.0F)
#define BUTTON_DEPTH 1.0F
/* COUNTDOWN_CELL + n draws while n whole seconds of the test are left */
#define COUNTDOWN_CELL 8
/* each second, the countdown's digit shrinks from nearly twice its size
 * to nothing */
#define COUNTDOWN_SHRINK (2.0F / TV_SETTING_SECOND_STEPS)
/* the countdown draws in the middle of the screen */
#define COUNTDOWN_POS {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, -10.0F}
/* the test's help is the three messages from TEST_HELP. each other
 * choice's is the two from HELP_50HZ or HELP_60HZ. the last line draws at
 * HELP_POS, and each line above it HELP_LINE_HEIGHT higher */
#define TEST_HELP 48
#define HELP_50HZ 51
#define HELP_60HZ 53
#define HELP_POS {0.0F, 400.0F, 20.0F}
#define HELP_LINE_HEIGHT 32.0F
#define TEXTURES 22
#define CELLS 13

/* the task's view of tv_setting_work. the choices have a turn in degrees,
 * from 0 to TURN_END, and each its place on the screen */
struct tv_setting_view {
    Float backdrop_fade;
    Sint32 turn;
    NJS_POINT3 choices[TV_SETTING_CHOICES];
    Float test_fade;
};
/* where a choice lands, and the roll of its circle, as struct menu_panel
 * takes them */
struct tv_setting_place {
    NJS_POINT3 pos;
    Angle roll;
};

/* the view */
static struct tv_setting_view tv_setting_view;

static struct tv_setting_view view_start = {
    0.0F,
    0,
    {{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}},
    0.0F,
};
static struct task *panel_task = NULL;
KEPT static struct tv_setting_place places[TV_SETTING_CHOICES] = {
#include "advertise/tv_setting_draw/places.inc"
};
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/tv_setting_draw/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/tv_setting_draw/texlist.inc"
};
/* tv_setting_draw_create sets each cell's cx and cy to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/tv_setting_draw/cells.inc"
};

static void tv_setting_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                   Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* put each choice where its circle has turned it */
static void tv_setting_choices_place(void)
{
    struct menu_panel panel;
    Int i;

    for (i = 0; i < TV_SETTING_CHOICES; i++) {
        panel.pos = places[i].pos;
        panel.roll = places[i].roll;
        panel.depth = CHOICE_DEPTH;
        panel.radius = CHOICE_RADIUS;
        menu_panel_point(&panel, &tv_setting_view.choices[i],
                         tv_setting_view.turn);
    }
}

/* draw the choice whose lit cell is lit_cell, lit or dim, over its button.
 * scale sizes both and fades them */
static void tv_setting_choice_draw(Int lit_cell, Float scale, NJS_POINT3 *pos,
                                   Bool lit)
{
    NJS_POINT3 point;

    draw_attr_save();
    constant_material_set(scale, scale, scale, scale);
    point = *pos;
    point.z = CHOICE_PRIORITY;
    if (lit) {
        tv_setting_sprite_draw(lit_cell, scale, &point, MENU_SPRITE_ATTR);
    } else {
        tv_setting_sprite_draw(lit_cell + (DIM_CELL - LIT_CELL), scale, &point,
                               MENU_SPRITE_ATTR);
    }
    point.z -= BUTTON_DEPTH;
    if (lit) {
        tv_setting_sprite_draw(LIT_BUTTON_CELL, scale, &point,
                               MENU_SPRITE_ATTR);
    } else {
        tv_setting_sprite_draw(DIM_BUTTON_CELL, scale, &point,
                               MENU_SPRITE_ATTR);
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

static void tv_setting_draw_step(struct task *task)
{
    tv_setting_work.moving = FALSE;
    if (!tv_setting_work.shown) {
        tv_setting_work.backdrop.shown = FALSE;
        tv_setting_work.choices.shown = FALSE;
        tv_setting_work.test.shown = FALSE;
    }
    tv_setting_work.backdrop.moving = FALSE;
    if (tv_setting_work.backdrop.shown) {
        if (menu_approach(&tv_setting_view.backdrop_fade, 1.0F, FADE_STEP)) {
            tv_setting_work.backdrop.moving = TRUE;
        }
    } else {
        if (menu_approach(&tv_setting_view.backdrop_fade, 0.0F, FADE_STEP)) {
            tv_setting_work.backdrop.moving = TRUE;
        }
    }
    tv_setting_work.choices.moving = FALSE;
    if (tv_setting_work.choices.shown) {
        if (menu_approach_int(&tv_setting_view.turn, TURN_END, TURN_STEP)) {
            tv_setting_work.choices.moving = TRUE;
            tv_setting_choices_place();
        }
    } else {
        if (menu_approach_int(&tv_setting_view.turn, 0, TURN_STEP)) {
            tv_setting_work.choices.moving = TRUE;
            tv_setting_choices_place();
        }
    }
    tv_setting_work.test.moving = FALSE;
    if (tv_setting_work.test.shown) {
        if (menu_approach(&tv_setting_view.test_fade, 1.0F, FADE_STEP)) {
            tv_setting_work.test.moving = TRUE;
        }
    } else {
        if (menu_approach(&tv_setting_view.test_fade, 0.0F, FADE_STEP)) {
            tv_setting_work.test.moving = TRUE;
        }
    }
    /* @hack redundant: moving itself in the sum of the parts' moving,
     * though the step cleared it */
    tv_setting_work.moving =
        tv_setting_work.moving | tv_setting_work.backdrop.moving |
        tv_setting_work.choices.moving | tv_setting_work.test.moving;
}

static void tv_setting_draw(struct task *task)
{
    Int i;
    Float scale;
    Int seconds;

    if (tv_setting_view.backdrop_fade != 0.0F) {
        menu_fade_begin(tv_setting_view.backdrop_fade);
        menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                           MENU_BACKDROP_TINT);
        menu_tint_end();
    }
    if (tv_setting_view.turn != 0) {
        scale = tv_setting_view.turn * (1.0F / TURN_END);
        for (i = 0; i < TV_SETTING_CHOICES; i++) {
            /* @hack spelling: the lit flag as bright times a comparison,
             * where bright && the comparison would do */
            tv_setting_choice_draw(
                i + LIT_CELL, scale, &tv_setting_view.choices[i],
                tv_setting_work.choices.bright * (tv_setting_work.cursor == i));
        }
        if (tv_setting_work.help_shown) {
            NJS_POINT3 pos = HELP_POS;

            switch (tv_setting_work.cursor) {
            case TV_SETTING_TEST:
                message_sprite_draw(TEST_HELP + 2, &pos, 1.0F);
                pos.y -= HELP_LINE_HEIGHT;
                message_sprite_draw(TEST_HELP + 1, &pos, 1.0F);
                pos.y -= HELP_LINE_HEIGHT;
                message_sprite_draw(TEST_HELP, &pos, 1.0F);
                break;
            case TV_SETTING_50HZ:
                message_sprite_draw(HELP_50HZ + 1, &pos, 1.0F);
                pos.y -= HELP_LINE_HEIGHT;
                message_sprite_draw(HELP_50HZ, &pos, 1.0F);
                break;
            case TV_SETTING_60HZ:
                message_sprite_draw(HELP_60HZ + 1, &pos, 1.0F);
                pos.y -= HELP_LINE_HEIGHT;
                message_sprite_draw(HELP_60HZ, &pos, 1.0F);
                break;
            }
        }
    }
    if (tv_setting_view.test_fade != 0.0F) {
        menu_fade_begin(tv_setting_view.test_fade);
        menu_backdrop_draw(TEST_TEXTURE, &texlist, BACKDROP_PRIORITY, 0);
        {
            NJS_POINT3 pos = COUNTDOWN_POS;

            seconds = tv_setting_work.test_steps / TV_SETTING_SECOND_STEPS;
            if (seconds > TV_SETTING_TEST_SECONDS - 1) {
                seconds = TV_SETTING_TEST_SECONDS - 1;
            } else if (seconds < 0) {
                seconds = 0;
            }
            scale = (tv_setting_work.test_steps % TV_SETTING_SECOND_STEPS) *
                    COUNTDOWN_SHRINK;
            tv_setting_sprite_draw(seconds + COUNTDOWN_CELL, scale, &pos,
                                   MENU_SPRITE_ATTR);
        }
        menu_tint_end();
    }
}

static void tv_setting_draw_destroy(struct task *task)
{
    tv_setting_draw_release();
}

void tv_setting_draw_create(void)
{
    struct task *task;
    NJS_TEXANIM *cell;
    NJS_TEXANIM *end;
    Uint8 *byte;
    Int left;

    task = task_create(TASK_WORK, TASK_LEVEL, tv_setting_draw_step,
                       "tvSettingExec");
    task->draw = tv_setting_draw;
    task->destructor = tv_setting_draw_destroy;
    panel_task = task;
    byte = (Uint8 *)&tv_setting_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(tv_setting_work); left != 0; left--) {
        *byte++ = 0;
    }
    tv_setting_view = view_start;
    /* @hack spelling: the walk of the cells by a pointer and an end set from
     * it and moved on, where an index counting to CELLS would do */
    cell = end = cells;
    end += CELLS;
    for (; cell < end; cell++) {
        cell->cx = cell->sx / 2;
        cell->cy = cell->sy / 2;
    }
}

/* destroy the task, where tv_setting_draw_create made one that this hasn't
 * destroyed. nothing calls tv_setting_draw_end, and no header declares it.
 * it is external only because SHC leaves out a static function that nothing
 * calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void tv_setting_draw_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

void tv_setting_draw_end(void)
{
    if (panel_task) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

void tv_setting_draw_load(void)
{
    if (!tv_setting_work.loaded) {
        texture_load_pvm(TV_SETTING_ARCHIVE, &texlist);
        tv_setting_work.loaded = TRUE;
    }
}

void tv_setting_draw_release(void)
{
    if (tv_setting_work.loaded == 0) {
        return;
    }
    njReleaseTexture(&texlist);
    tv_setting_work.loaded = FALSE;
}
