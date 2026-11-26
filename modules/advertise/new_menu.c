/* the new menu's buttons, in groups that swing on to the screen and off it
 * on circles. new_menu_screen.h's screen runs them through new_menu_work */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_projection.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/new_menu.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/new_menu/text.inc"

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void new_menu_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

#define TASK_LEVEL 2
#define GROUPS 5
/* a group's swing, in degrees: SWUNG_OUT out of sight, SWUNG_IN in place */
#define SWUNG_OUT 0
#define SWUNG_IN 180
#define SWING_STEP 6 /* on each of the task's steps */
/* the first of each later group's buttons in button_cells, and the count of
 * all */
#define SINGLE_FIRST NEW_MENU_TOP_BUTTONS
#define VERSUS_FIRST (SINGLE_FIRST + NEW_MENU_SINGLE_BUTTONS)
#define EXTRAS_FIRST (VERSUS_FIRST + NEW_MENU_VERSUS_BUTTONS)
#define BUTTONS (EXTRAS_FIRST + NEW_MENU_EXTRAS_BUTTONS)
/* the most buttons a group but the top has */
#define MOST_BUTTONS 4
/* the languages button_cells holds a button's cells in, as
 * new_menu_work.language picks */
#define LANGUAGES 2
/* the circle each button swings in on, as menu_projection.h's menu_panel
 * takes it */
#define CIRCLE_DEPTH (-100.0F)
#define CIRCLE_RADIUS 50.0F
/* the sprite cells. a button draws over a back cell, lit or dim. the top
 * group's are the TOP_BACK_ cells, and the others' the BACK_ cells */
#define CELLS 68
#define TOP_BACK_LIT 14
#define TOP_BACK_DIM 15
#define BACK_LIT 16
#define BACK_DIM 17
/* each group's draw priority, and how much further back a button's back
 * cell draws */
#define TOP_PRIORITY (-9.0F)
#define OTHER_PRIORITY (-5.0F)
#define BACK_BEHIND 1.0F
/* a refused top button's level is REFUSED_LEVEL times the scale its
 * group's swing gives. the screen refuses only NEW_MENU_TOP_VERSUS and
 * NEW_MENU_TOP_INTERNET */
#define REFUSED_LEVEL 0.5F

/* a button's cells in button_cells */
enum { CELL_LIT, CELL_DIM, CELL_REFUSED, BUTTON_CELLS };

/* a button's place at full swing, and the roll of the circle it swings in
 * on */
struct button_layout {
    NJS_POINT3 pos;
    Angle roll;
};
/* a group's swing, from SWUNG_OUT to SWUNG_IN, and where it puts each of
 * the group's buttons */
struct swing {
    Sint32 angle;
    Sint32 count; /* the buttons it shows */
    NJS_POINT3 pos[NEW_MENU_SLOTS];
};
/* a group: its buttons' places, the count of them, its swing and its record
 * in the work */
struct group_layout {
    struct button_layout *layout;
    Int buttons;
    struct swing *swing;
    struct new_menu_group *work;
};

struct swings {
    struct swing top;
    struct swing single;
    struct swing versus;
    struct swing extras;
    struct swing unused;
};

static struct swings new_menu_swings;

/* the swings new_menu_reset sets: every group swung out */
static struct swings swings_start = {0};
static struct task *new_menu_task = NULL;
/* by button, then by language, each of the button's cells. CELL_REFUSED is
 * -1 where the button has no such cell. no refused cell draws: the top
 * group has none, and the other groups draw no refused button */
/* clang-format off */
static Sint32 button_cells[BUTTONS][LANGUAGES][BUTTON_CELLS] = {
    {{0, 6, -1}, {0, 6, -1}},
    {{1, 7, -1}, {1, 7, -1}},
    {{2, 8, -1}, {2, 8, -1}},
    {{3, 9, -1}, {3, 9, -1}},
    {{4, 10, -1}, {4, 10, -1}},
    {{24, 25, -1}, {46, 47, -1}},
    {{26, 27, -1}, {48, 49, -1}},
    {{28, 29, -1}, {50, 51, -1}},
    {{30, 31, -1}, {52, 53, -1}},
    {{32, 33, -1}, {54, 55, -1}},
    {{28, 29, -1}, {50, 51, -1}},
    {{34, 35, -1}, {56, 57, -1}},
    {{36, 37, 38}, {58, 59, 60}},
    {{39, 40, -1}, {61, 62, -1}},
    {{41, 42, -1}, {63, 64, -1}},
    {{43, 44, 45}, {65, 66, 67}},
};
/* clang-format on */

/* the layouts by the count of buttons shown, for every group but the
 * top. a refused button shows nothing, and those shown close up */
static struct button_layout layout_1[1] = {
#include "advertise/new_menu/layout_1.inc"
};
static struct button_layout layout_2[2] = {
#include "advertise/new_menu/layout_2.inc"
};
static struct button_layout layout_3[3] = {
#include "advertise/new_menu/layout_3.inc"
};
static struct button_layout layout_4[4] = {
#include "advertise/new_menu/layout_4.inc"
};
static struct button_layout top_layout[NEW_MENU_TOP_BUTTONS] = {
#include "advertise/new_menu/top_layout.inc"
};
static struct button_layout single_layout[NEW_MENU_SINGLE_BUTTONS] = {
#include "advertise/new_menu/single_layout.inc"
};
static struct button_layout versus_layout[NEW_MENU_VERSUS_BUTTONS] = {
#include "advertise/new_menu/versus_layout.inc"
};
static struct button_layout extras_layout[NEW_MENU_EXTRAS_BUTTONS] = {
#include "advertise/new_menu/extras_layout.inc"
};
/* the groups. only the top's layout is read: the others take
 * layouts_by_count's */
KEPT static struct group_layout groups[GROUPS] = {
#include "advertise/new_menu/groups.inc"
};
KEPT static struct button_layout *layouts_by_count[MOST_BUTTONS + 1] = {
#include "advertise/new_menu/layouts_by_count.inc"
};
static NJS_TEXNAME texnames[12] = {
#include "advertise/new_menu/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/new_menu/texlist.inc"
};
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/new_menu/cells.inc"
};

static void new_menu_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = cells[cell].attr * ANGLE_PER_DEGREE;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* draw button, below BUTTONS, at pos over its back cell. both draw at
 * scale, with level in each channel of the constant material. a lit button
 * takes the lit cells, and a refused one its CELL_REFUSED where it has
 * one */
static void new_menu_button_draw(Int button, Float level, Float scale,
                                 NJS_POINT3 *pos, Bool lit, Bool refused)
{
    NJS_POINT3 point;

    draw_attr_save();
    constant_material_set(level, level, level, level);
    point = *pos;
    if (button < NEW_MENU_TOP_BUTTONS) {
        point.z = TOP_PRIORITY;
    } else {
        point.z = OTHER_PRIORITY;
    }
    if (refused &&
        button_cells[button][new_menu_work.language][CELL_REFUSED] >= 0) {
        new_menu_sprite_draw(
            button_cells[button][new_menu_work.language][CELL_REFUSED], scale,
            &point, MENU_SPRITE_ATTR);
    } else if (lit) {
        new_menu_sprite_draw(
            button_cells[button][new_menu_work.language][CELL_LIT], scale,
            &point, MENU_SPRITE_ATTR);
    } else {
        new_menu_sprite_draw(
            button_cells[button][new_menu_work.language][CELL_DIM], scale,
            &point, MENU_SPRITE_ATTR);
    }
    point.z -= BACK_BEHIND;
    if (button < NEW_MENU_TOP_BUTTONS) {
        if (lit) {
            new_menu_sprite_draw(TOP_BACK_LIT, scale, &point, MENU_SPRITE_ATTR);
        } else {
            new_menu_sprite_draw(TOP_BACK_DIM, scale, &point, MENU_SPRITE_ATTR);
        }
    } else {
        if (lit) {
            new_menu_sprite_draw(BACK_LIT, scale, &point, MENU_SPRITE_ATTR);
        } else {
            new_menu_sprite_draw(BACK_DIM, scale, &point, MENU_SPRITE_ATTR);
        }
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

/* place group's buttons at the group's swing. a group but the top lays out
 * only its buttons not refused, by their count */
static void new_menu_place_group(Int group)
{
    struct menu_panel panel;
    Int i;
    Int placed;

    if (group != NEW_MENU_TOP) {
        groups[group].swing->count = 0;
        for (i = 0; i < groups[group].buttons; i++) {
            if (groups[group].work->refused[i] == 0) {
                groups[group].swing->count++;
            }
        }
        placed = 0;
        for (i = 0; i < groups[group].buttons; i++) {
            if (groups[group].work->refused[i] == 0) {
                panel.pos =
                    layouts_by_count[groups[group].swing->count][placed].pos;
                panel.roll =
                    layouts_by_count[groups[group].swing->count][placed].roll;
                panel.depth = CIRCLE_DEPTH;
                panel.radius = CIRCLE_RADIUS;
                menu_panel_point(&panel, &groups[group].swing->pos[i],
                                 groups[group].swing->angle);
                placed++;
            }
        }
    } else {
        for (i = 0; i < groups[group].buttons; i++) {
            panel.pos = groups[group].layout[i].pos;
            panel.roll = groups[group].layout[i].roll;
            panel.depth = CIRCLE_DEPTH;
            panel.radius = CIRCLE_RADIUS;
            menu_panel_point(&panel, &groups[group].swing->pos[i],
                             groups[group].swing->angle);
        }
    }
}

static void new_menu_step(struct task *task)
{
    if (!new_menu_work.shown) {
        new_menu_work.top.shown = FALSE;
        new_menu_work.single.shown = FALSE;
        new_menu_work.versus.shown = FALSE;
        new_menu_work.extras.shown = FALSE;
        new_menu_work.unused.shown = FALSE;
    }
    new_menu_work.top.swinging = FALSE;
    if (new_menu_work.top.shown) {
        if (menu_approach_int(&new_menu_swings.top.angle, SWUNG_IN,
                              SWING_STEP)) {
            new_menu_work.top.swinging = TRUE;
            new_menu_place_group(NEW_MENU_TOP);
        }
    } else {
        if (menu_approach_int(&new_menu_swings.top.angle, SWUNG_OUT,
                              SWING_STEP)) {
            new_menu_work.top.swinging = TRUE;
            new_menu_place_group(NEW_MENU_TOP);
        }
    }
    new_menu_work.single.swinging = FALSE;
    if (new_menu_work.single.shown) {
        if (menu_approach_int(&new_menu_swings.single.angle, SWUNG_IN,
                              SWING_STEP)) {
            new_menu_work.single.swinging = TRUE;
            new_menu_place_group(NEW_MENU_SINGLE);
        }
    } else {
        if (menu_approach_int(&new_menu_swings.single.angle, SWUNG_OUT,
                              SWING_STEP)) {
            new_menu_work.single.swinging = TRUE;
            new_menu_place_group(NEW_MENU_SINGLE);
        }
    }
    new_menu_work.versus.swinging = FALSE;
    if (new_menu_work.versus.shown) {
        if (menu_approach_int(&new_menu_swings.versus.angle, SWUNG_IN,
                              SWING_STEP)) {
            new_menu_work.versus.swinging = TRUE;
            new_menu_place_group(NEW_MENU_VERSUS);
        }
    } else {
        if (menu_approach_int(&new_menu_swings.versus.angle, SWUNG_OUT,
                              SWING_STEP)) {
            new_menu_work.versus.swinging = TRUE;
            new_menu_place_group(NEW_MENU_VERSUS);
        }
    }
    new_menu_work.extras.swinging = FALSE;
    if (new_menu_work.extras.shown) {
        if (menu_approach_int(&new_menu_swings.extras.angle, SWUNG_IN,
                              SWING_STEP)) {
            new_menu_work.extras.swinging = TRUE;
            new_menu_place_group(NEW_MENU_EXTRAS);
        }
    } else {
        if (menu_approach_int(&new_menu_swings.extras.angle, SWUNG_OUT,
                              SWING_STEP)) {
            new_menu_work.extras.swinging = TRUE;
            new_menu_place_group(NEW_MENU_EXTRAS);
        }
    }
    new_menu_work.unused.swinging = FALSE;
    new_menu_work.swinging =
        new_menu_work.top.swinging | new_menu_work.single.swinging |
        new_menu_work.versus.swinging | new_menu_work.extras.swinging;
}

static void new_menu_draw(struct task *task)
{
    Int i;
    Float scale;

    if (new_menu_swings.top.angle) {
        scale = new_menu_swings.top.angle * (1.0F / SWUNG_IN);
        for (i = 0; i < NEW_MENU_TOP_BUTTONS; i++) {
            if ((i == NEW_MENU_TOP_VERSUS || i == NEW_MENU_TOP_INTERNET) &&
                new_menu_work.top.refused[i]) {
                /* @hack spelling: each lit times a comparison, where a
                 * conditional expression would do */
                new_menu_button_draw(i, scale * REFUSED_LEVEL, scale,
                                     &new_menu_swings.top.pos[i],
                                     new_menu_work.top.lit *
                                         (new_menu_work.top.cursor == i),
                                     new_menu_work.top.refused[i]);
            } else {
                new_menu_button_draw(
                    i, scale, scale, &new_menu_swings.top.pos[i],
                    new_menu_work.top.lit * (new_menu_work.top.cursor == i),
                    new_menu_work.top.refused[i]);
            }
        }
    }
    if (new_menu_swings.single.angle) {
        scale = new_menu_swings.single.angle * (1.0F / SWUNG_IN);
        for (i = 0; i < NEW_MENU_SINGLE_BUTTONS; i++) {
            if (!new_menu_work.single.refused[i]) {
                new_menu_button_draw(i + SINGLE_FIRST, scale, scale,
                                     &new_menu_swings.single.pos[i],
                                     new_menu_work.single.lit *
                                         (new_menu_work.single.cursor == i),
                                     new_menu_work.single.refused[i]);
            }
        }
    }
    if (new_menu_swings.versus.angle) {
        scale = new_menu_swings.versus.angle * (1.0F / SWUNG_IN);
        for (i = 0; i < NEW_MENU_VERSUS_BUTTONS; i++) {
            if (!new_menu_work.versus.refused[i]) {
                new_menu_button_draw(i + VERSUS_FIRST, scale, scale,
                                     &new_menu_swings.versus.pos[i],
                                     new_menu_work.versus.lit *
                                         (new_menu_work.versus.cursor == i),
                                     new_menu_work.versus.refused[i]);
            }
        }
    }
    if (new_menu_swings.extras.angle) {
        scale = new_menu_swings.extras.angle * (1.0F / SWUNG_IN);
        for (i = 0; i < NEW_MENU_EXTRAS_BUTTONS; i++) {
            if (!new_menu_work.extras.refused[i]) {
                new_menu_button_draw(i + EXTRAS_FIRST, scale, scale,
                                     &new_menu_swings.extras.pos[i],
                                     new_menu_work.extras.lit *
                                         (new_menu_work.extras.cursor == i),
                                     new_menu_work.extras.refused[i]);
            }
        }
    }
}

static void new_menu_destroy(struct task *task)
{
    new_menu_release();
}

void new_menu_create(void)
{
    struct task *task;
    Int i;
    NJS_TEXANIM *cell;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, new_menu_step, "newMenuExec");
    task->draw = new_menu_draw;
    task->destructor = new_menu_destroy;
    new_menu_task = task;
    byte = (Uint8 *)&new_menu_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(new_menu_work); left != 0; left--) {
        *byte++ = 0;
    }
    new_menu_reset();
    /* @hack redundant: a do loop that runs once */
    do {
        cell = cells;
    } while (0);
    for (i = 0; i < CELLS; i++) {
        cell[i].cx = cell[i].sx / 2;
        cell[i].cy = cell[i].sy / 2;
    }
}

void new_menu_reset(void)
{
    Int loaded;
    Uint8 *byte;
    int left;

    loaded = new_menu_work.loaded;
    byte = (Uint8 *)&new_menu_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(new_menu_work); left != 0; left--) {
        *byte++ = 0;
    }
    new_menu_swings = swings_start;
    new_menu_work.loaded = loaded;
    new_menu_work.extras.cursor = NEW_MENU_EXTRAS_TUTORIAL;
}

void new_menu_end(void)
{
    if (new_menu_task != NULL) {
        task_destroy(new_menu_task);
        new_menu_task = NULL;
    }
}

void new_menu_load(void)
{
    if (!new_menu_work.loaded) {
        texture_load_pvm(NEW_MENU_ARCHIVE, &texlist);
        new_menu_work.loaded = TRUE;
    }
}

void new_menu_release(void)
{
    if (new_menu_work.loaded) {
        njReleaseTexture(&texlist);
        new_menu_work.loaded = FALSE;
        /* @hack redundant: a return at the end of the test's branch */
        return;
    }
}
