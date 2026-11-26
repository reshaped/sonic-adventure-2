/* the main menu's panels. each panel swings on a circle of its own. its
 * group's swing sets where it stands on it */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/main_menu.h"
#include "advertise/menu_projection.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "advertise/main_menu/text.inc"

#define TASK_LEVEL 2
#define TEXTURES 8
/* a group's swing, in degrees. at SWUNG_OUT its panels stand unseen on
 * their circles. at SWUNG_IN they stand in their places */
#define SWUNG_OUT 0
#define SWUNG_IN 180
#define SWING_STEP 6 /* a swing's move on each of the task's steps */
/* each panel's circle, as menu_projection.h's menu_panel takes it */
#define PANEL_DEPTH (-100.0F)
#define PANEL_RADIUS 50.0F

/* the panels of each group. faces holds them in the groups' order */
#define GROUP_0_PANELS 5
#define GROUP_1_PANELS 4
#define GROUP_2_PANELS 3
#define GROUP_3_PANELS 4
/* the first of each later group's panels in faces */
#define GROUP_1_FIRST GROUP_0_PANELS
#define GROUP_2_FIRST (GROUP_1_FIRST + GROUP_1_PANELS)
#define GROUP_3_FIRST (GROUP_2_FIRST + GROUP_2_PANELS)
#define PANELS (GROUP_3_FIRST + GROUP_3_PANELS)
/* the languages a panel has faces in, as main_menu_work's language picks */
#define LANGUAGES 2
/* a panel's faces: its front and the cell behind it, or its refused face
 * alone */
#define FACE_FRONT 0
#define FACE_BACK 1
#define FACE_REFUSED 2
#define FACES 3
/* the frames behind the faces. group 0's panels draw a bar behind them,
 * and the other groups' a tile. a lit panel draws its lit frame */
#define BAR_CELL 64
#define BAR_LIT_CELL 65
#define TILE_CELL 66
#define TILE_LIT_CELL 67
#define CELLS 68
/* each panel's front draws at its group's priority, and each cell behind
 * it 1 further back */
#define BAR_PRIORITY (-9.0F)
#define TILE_PRIORITY (-5.0F)
#define PANEL_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR)

/* set dat_8c5cc520 to 1. nothing calls fun_8c50419a, and no header declares
 * it. it is external only because SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void fun_8c50419a(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* a panel's place, and the roll of the circle it swings in along */
struct panel_layout {
    NJS_POINT3 pos;
    Angle roll;
};
/* a group's swing, from SWUNG_OUT to SWUNG_IN, and where it puts each
 * panel on the screen */
struct swing {
    Sint32 angle;
    NJS_POINT3 pos[MAIN_MENU_GROUP_PANELS];
};
struct swings {
    /* @hack type: the swings as five members, where an array of
     * MAIN_MENU_GROUPS would do */
    struct swing swing_0;
    struct swing swing_1;
    struct swing swing_2;
    struct swing swing_3;
    struct swing swing_4;
};
struct group_layout {
    struct panel_layout *panels;
    Int count;
    struct swing *swing;
};

static struct swings main_menu_swings;
/* Volatile retains the store in fun_8c50419a. The game never reads it. */
static volatile Sint32 dat_8c5cc520;

/* the swings main_menu_reset sets */
static struct swings swings_start = {
    {SWUNG_OUT}, {SWUNG_OUT}, {SWUNG_OUT}, {SWUNG_OUT}, {SWUNG_OUT},
};
/* by panel, then by language, each of the panel's faces */
/* clang-format off */
static Int faces[PANELS][LANGUAGES][FACES] = {
    {{ 0,  1,  0}, {32, 33,  0}},
    {{ 2,  3,  0}, {34, 35,  0}},
    {{ 4,  5,  0}, {36, 37,  0}},
    {{ 6,  7,  0}, {38, 39,  0}},
    {{ 8,  9,  0}, {40, 41,  0}},
    {{10, 11,  0}, {42, 43,  0}},
    {{12, 13,  0}, {44, 45,  0}},
    {{14, 15,  0}, {46, 47,  0}},
    {{16, 17,  0}, {48, 49,  0}},
    {{18, 19,  0}, {50, 51,  0}},
    {{14, 15,  0}, {46, 47,  0}},
    {{20, 21,  0}, {52, 53,  0}},
    {{22, 23, 24}, {54, 55, 56}},
    {{25, 26,  0}, {57, 58,  0}},
    {{27, 28,  0}, {59, 60,  0}},
    {{29, 30, 31}, {61, 62, 63}},
};
/* clang-format on */
static struct panel_layout group_0_panels[GROUP_0_PANELS] = {
#include "advertise/main_menu/group_0_panels.inc"
};
static struct panel_layout group_1_panels[GROUP_1_PANELS] = {
#include "advertise/main_menu/group_1_panels.inc"
};
static struct panel_layout group_2_panels[GROUP_2_PANELS] = {
#include "advertise/main_menu/group_2_panels.inc"
};
static struct panel_layout group_3_panels[GROUP_3_PANELS] = {
#include "advertise/main_menu/group_3_panels.inc"
};
KEPT static struct group_layout groups[MAIN_MENU_GROUPS] = {
#include "advertise/main_menu/groups.inc"
};
/* where group 0's chosen panel stands once the group has swung out */
static NJS_POINT3 chosen_out = {
#include "advertise/main_menu/chosen_out.inc"
};
static NJS_TEXNAME texnames[TEXTURES] = {
#include "advertise/main_menu/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/main_menu/texlist.inc"
};
/* main_menu_create sets each cell's center, cx and cy, to its middle */
static NJS_TEXANIM cells[CELLS] = {
#include "advertise/main_menu/cells.inc"
};

static void main_menu_sprite_draw(Int cell, Float scale, const NJS_POINT3 *pos,
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

/* draw panel's faces and frame at pos's x and y, in main_menu_work's
 * language. scale sizes them, and sets every channel of the constant
 * material they draw in. a refused panel shows its refused face alone */
static void main_menu_panel_draw(Int panel, Float scale, const NJS_POINT3 *pos,
                                 Bool lit, Bool refused)
{
    NJS_POINT3 point;

    draw_attr_save();
    constant_material_set(scale, scale, scale, scale);
    point = *pos;
    if (panel < GROUP_1_FIRST) {
        point.z = BAR_PRIORITY;
    } else {
        point.z = TILE_PRIORITY;
    }
    if (refused) {
        main_menu_sprite_draw(
            faces[panel][main_menu_work.language][FACE_REFUSED], scale, &point,
            PANEL_ATTR);
    } else {
        main_menu_sprite_draw(faces[panel][main_menu_work.language][FACE_FRONT],
                              scale, &point, PANEL_ATTR);
        point.z -= 1.0F;
        main_menu_sprite_draw(faces[panel][main_menu_work.language][FACE_BACK],
                              scale, &point, PANEL_ATTR);
    }
    point.z -= 1.0F;
    if (panel < GROUP_1_FIRST) {
        if (lit) {
            main_menu_sprite_draw(BAR_LIT_CELL, scale, &point, PANEL_ATTR);
        } else {
            main_menu_sprite_draw(BAR_CELL, scale, &point, PANEL_ATTR);
        }
    } else {
        if (lit) {
            main_menu_sprite_draw(TILE_LIT_CELL, scale, &point, PANEL_ATTR);
        } else {
            main_menu_sprite_draw(TILE_CELL, scale, &point, PANEL_ATTR);
        }
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

/* put each of group's panels where its swing's angle has it. group 0's
 * chosen panel moves from its place toward chosen_out as the angle falls */
static void main_menu_group_place(Int group)
{
    struct menu_panel panel;
    struct menu_lerp lerp;
    Int i;

    for (i = 0; i < groups[group].count; i++) {
        if (group == 0 && main_menu_work.group_0.chosen - 1 == i) {
            panel.pos = groups[group].panels[i].pos;
            panel.roll = groups[group].panels[i].roll;
            panel.depth = PANEL_DEPTH;
            panel.radius = PANEL_RADIUS;
            menu_panel_point(&panel, &groups[group].swing->pos[i], SWUNG_IN);
            lerp.from = &chosen_out;
            lerp.to = &groups[group].swing->pos[i];
            lerp.from_time = SWUNG_OUT;
            lerp.to_time = SWUNG_IN;
            lerp.time = groups[group].swing->angle;
            lerp.out = &groups[group].swing->pos[i];
            menu_lerp(&lerp);
        } else {
            panel.pos = groups[group].panels[i].pos;
            panel.roll = groups[group].panels[i].roll;
            panel.depth = PANEL_DEPTH;
            panel.radius = PANEL_RADIUS;
            menu_panel_point(&panel, &groups[group].swing->pos[i],
                             groups[group].swing->angle);
        }
    }
}

/* swing groups 0 to 3, and place a group's panels on each step that moves
 * its swing. group 4 never swings */
static void main_menu_step(struct task *task)
{
    main_menu_work.group_0.swinging = FALSE;
    if (main_menu_work.group_0.shown) {
        if (menu_approach_int(&main_menu_swings.swing_0.angle, SWUNG_IN,
                              SWING_STEP)) {
            main_menu_work.group_0.swinging = TRUE;
            main_menu_group_place(0);
        }
    } else {
        if (menu_approach_int(&main_menu_swings.swing_0.angle, SWUNG_OUT,
                              SWING_STEP)) {
            main_menu_work.group_0.swinging = TRUE;
            main_menu_group_place(0);
        }
    }
    main_menu_work.group_1.swinging = FALSE;
    if (main_menu_work.group_1.shown) {
        if (menu_approach_int(&main_menu_swings.swing_1.angle, SWUNG_IN,
                              SWING_STEP)) {
            main_menu_work.group_1.swinging = TRUE;
            main_menu_group_place(1);
        }
    } else {
        if (menu_approach_int(&main_menu_swings.swing_1.angle, SWUNG_OUT,
                              SWING_STEP)) {
            main_menu_work.group_1.swinging = TRUE;
            main_menu_group_place(1);
        }
    }
    main_menu_work.group_2.swinging = FALSE;
    if (main_menu_work.group_2.shown) {
        if (menu_approach_int(&main_menu_swings.swing_2.angle, SWUNG_IN,
                              SWING_STEP)) {
            main_menu_work.group_2.swinging = TRUE;
            main_menu_group_place(2);
        }
    } else {
        if (menu_approach_int(&main_menu_swings.swing_2.angle, SWUNG_OUT,
                              SWING_STEP)) {
            main_menu_work.group_2.swinging = TRUE;
            main_menu_group_place(2);
        }
    }
    main_menu_work.group_3.swinging = FALSE;
    if (main_menu_work.group_3.shown) {
        if (menu_approach_int(&main_menu_swings.swing_3.angle, SWUNG_IN,
                              SWING_STEP)) {
            main_menu_work.group_3.swinging = TRUE;
            main_menu_group_place(3);
        }
    } else {
        if (menu_approach_int(&main_menu_swings.swing_3.angle, SWUNG_OUT,
                              SWING_STEP)) {
            main_menu_work.group_3.swinging = TRUE;
            main_menu_group_place(3);
        }
    }
}

/* draw groups 0 to 3 while the work is shown. each draws at its swing's
 * share of its full size */
static void main_menu_draw(struct task *task)
{
    Int i;
    Float scale;

    if (main_menu_work.shown) {
        if (main_menu_swings.swing_0.angle) {
            scale = main_menu_swings.swing_0.angle * (1.0F / SWUNG_IN);
            for (i = 0; i < GROUP_0_PANELS; i++) {
                if (main_menu_work.group_0.chosen - 1 != i) {
                    /* @hack spelling: the lit flag times a comparison, here
                     * and in groups 1 to 3, where && would do */
                    main_menu_panel_draw(
                        i, scale, &main_menu_swings.swing_0.pos[i],
                        main_menu_work.group_0.lit *
                            (main_menu_work.group_0.cursor == i),
                        main_menu_work.group_0.refused[i]);
                }
            }
        }
        if (main_menu_work.group_0.chosen > 0) {
            main_menu_panel_draw(main_menu_work.group_0.chosen - 1, 1.0F,
                                 &main_menu_swings.swing_0
                                      .pos[main_menu_work.group_0.chosen - 1],
                                 TRUE, FALSE);
        }
        if (main_menu_swings.swing_1.angle) {
            scale = main_menu_swings.swing_1.angle * (1.0F / SWUNG_IN);
            for (i = 0; i < GROUP_1_PANELS; i++) {
                main_menu_panel_draw(i + GROUP_1_FIRST, scale,
                                     &main_menu_swings.swing_1.pos[i],
                                     main_menu_work.group_1.lit *
                                         (main_menu_work.group_1.cursor == i),
                                     main_menu_work.group_1.refused[i]);
            }
        }
        if (main_menu_swings.swing_2.angle) {
            scale = main_menu_swings.swing_2.angle * (1.0F / SWUNG_IN);
            for (i = 0; i < GROUP_2_PANELS; i++) {
                main_menu_panel_draw(i + GROUP_2_FIRST, scale,
                                     &main_menu_swings.swing_2.pos[i],
                                     main_menu_work.group_2.lit *
                                         (main_menu_work.group_2.cursor == i),
                                     main_menu_work.group_2.refused[i]);
            }
        }
        if (main_menu_swings.swing_3.angle) {
            scale = main_menu_swings.swing_3.angle * (1.0F / SWUNG_IN);
            for (i = 0; i < GROUP_3_PANELS; i++) {
                main_menu_panel_draw(i + GROUP_3_FIRST, scale,
                                     &main_menu_swings.swing_3.pos[i],
                                     main_menu_work.group_3.lit *
                                         (main_menu_work.group_3.cursor == i),
                                     main_menu_work.group_3.refused[i]);
            }
        }
    }
}

static void main_menu_destroy(struct task *task)
{
    main_menu_release();
}

void main_menu_create(void)
{
    struct task *task;
    Int i;
    NJS_TEXANIM *cell;
    Uint8 *byte;
    int left;

    task = task_create(TASK_WORK, TASK_LEVEL, main_menu_step, "mainMenuExec");
    task->draw = main_menu_draw;
    task->destructor = main_menu_destroy;
    byte = (Uint8 *)&main_menu_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(main_menu_work); left != 0; left--) {
        *byte++ = 0;
    }
    main_menu_reset();
    /* @hack spelling: the cells through a local pointer, set in a loop that
     * runs once, where their own name would do */
    do {
        cell = cells;
    } while (0);
    for (i = 0; i < CELLS; i++) {
        cell[i].cx = cell[i].sx / 2;
        cell[i].cy = cell[i].sy / 2;
    }
}

void main_menu_reset(void)
{
    Sint32 loaded;
    Uint8 *byte;
    int left;

    loaded = main_menu_work.loaded;
    byte = (Uint8 *)&main_menu_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(main_menu_work); left != 0; left--) {
        *byte++ = 0;
    }
    main_menu_swings = swings_start;
    main_menu_work.loaded = loaded;
}

void fun_8c50419a(void)
{
    dat_8c5cc520 = 1;
}

void main_menu_load(void)
{
    if (!main_menu_work.loaded) {
        texture_load_pvm(MAIN_MENU_ARCHIVE, &texlist);
        main_menu_work.loaded = TRUE;
    }
}

void main_menu_release(void)
{
    if (main_menu_work.loaded) {
        njReleaseTexture(&texlist);
        main_menu_work.loaded = FALSE;
        /* @hack redundant: a return at the end of the branch */
        return;
    }
}
