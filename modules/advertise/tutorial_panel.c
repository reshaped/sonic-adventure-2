/* the tutorial's panel: a model over a backdrop, the list of lessons, and a
 * board of pictures. its task draws the panel as tutorial.h's menu sets the
 * work */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "advertise/menu_draw.h"
#include "advertise/menu_util.h"
#include "advertise/menus.h"
#include "advertise/tutorial_art.h"
#include "advertise/tutorial_panel.h"
#include "advertise/tutorial_panel/text.inc"
#include "draw/chunk.h"
#include "draw/light.h"
#include "system/angle.h"
#include "system/math.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "system/vector.h"

#define TASK_LEVEL 2
#define FADE_STEP (1.0F / 30)
#define FRAME_STEP 1.0F
#define BACKDROP_PRIORITY (-10000.0F)
/* below SOLID_LEVEL, the model draws translucent in the constant material */
#define SOLID_LEVEL 0.98F
/* the shadow: the model flattened to SHADOW_SCALE of its height, at
 * SHADOW_Y */
#define SHADOW_Y (-1.0F)
#define SHADOW_SCALE 0.01F
/* the board stands SLIDE_DISTANCE toward -x while its level is 0, and in
 * its place at 1 */
#define SLIDE_DISTANCE 20.0F

/* the stops the model moves to: the first under TUTORIAL_MODEL_HOME, and one
 * picked at random under TUTORIAL_MODEL_SWIM */
#define STOPS 13
/* under TUTORIAL_MODEL_HOME the model moves HOME_STEP a step on each axis
 * toward its first stop. it bobs there once its squared distance from the
 * stop is below HOME_RANGE_SQUARED. the phases in tutorial_bob_phases then
 * turn by BOB_STEP_X, BOB_STEP_Y and BOB_STEP_Z, for a bob of BOB_SIZE each
 * way */
#define HOME_RANGE_SQUARED 4.0F
#define HOME_STEP 1.0F
#define BOB_SIZE 1.0F
#define BOB_STEP_X NJM_DEG_ANG(2)
#define BOB_STEP_Y NJM_DEG_ANG(1)
#define BOB_STEP_Z NJM_DEG_ANG(0.5)
/* under TUTORIAL_MODEL_SWIM each axis's speed gains SWIM_ACCEL a step toward
 * the stop, up to SWIM_SPEED either way. the model picks a stop at random
 * once its squared distance from the stop is below SWIM_RANGE_SQUARED */
#define SWIM_ACCEL 0.005F
#define SWIM_SPEED 0.5F
#define SWIM_RANGE_SQUARED 1.0F
#define TURN_STEP NJM_DEG_ANG(5)
#define SPIN_Z_STEP NJM_DEG_ANG(6)
#define SPIN_Y_STEP NJM_DEG_ANG(12)

/* cells of tutorial_cells. the list's rows take cells 0 to 9 under
 * TEXT_JAPANESE. any other text language takes the cells LANGUAGE_OFFSET on */
#define FRAME_CELL 20 /* the first of FRAME_CELLS, drawn side by side */
#define FRAME_CELLS 2
#define ROW_LIT_BACK 22
#define ROW_DIM_BACK 23
#define LANGUAGE_OFFSET 10
#define BACKDROP_TEXTURE 7
#define FRAME_X 100.0F
#define FRAME_Y 50.0F
/* the sprites' priorities: the frame's, and the list's */
#define PANEL_PRIORITY (-100.0F)
#define LIST_PRIORITY (-90.0F)
/* The ID low byte follows the material and the texture command's header. */
#define PICTURE_TEXTURE_BYTE                                                   \
    (sizeof(tutorial_picture_plist[0].material) +                              \
     sizeof(tutorial_picture_plist[0].texture.head))
/* tutorial_shot000 in texlist is the board's empty picture. */
#define NO_PICTURE_TEXTURE 21

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* destroy the task, where there is one */
void tutorial_panel_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the panel's levels, the camera motion's frame and the model's place. a
 * level is 0 while its part is hidden, and 1 once it has faded in */
struct tutorial_anim {
    Float level; /* the model's, its shadow's, the frame's and the backdrop's */
    Float list_level;
    Float board_level;
    Float frame;
    NJS_POINT3 pos;
    struct angles ang; /* x and y turn the model. nothing reads z */
    /* the model's speed under TUTORIAL_MODEL_SWIM. TUTORIAL_MODEL_HOME clears
     * it on the way to the first stop */
    NJS_VECTOR spd;
    Sint32 stop; /* the stop the model swims to, an index of stops */
};
/* the panel's levels and the model's place, and the phases of the model's
 * bob on each axis */
static struct tutorial_anim tutorial_anim;
static Angle tutorial_bob_phases[3];

/* the texture list of the PVM efftex_common */
static NJS_TEXNAME efftex_common_texnames[121] = {
#include "advertise/tutorial_panel/efftex_common_texnames.inc"
};
static NJS_TEXLIST efftex_common_texlist = {
#include "advertise/tutorial_panel/efftex_common_texlist.inc"
};
/* every part hidden, the camera's motion at its first frame, and the model
 * at its first stop */
/* clang-format off */
static struct tutorial_anim anim_start = {
    0.0F, 0.0F, 0.0F,
    0.0F, {8.0F, 0.0F, -2.0F}, {0, 0, 0}, {0.0F, 0.0F, 0.0F}, 0,
};
/* clang-format on */
/* the task tutorial_panel_create made, or null */
static struct task *panel_task = NULL;
static Float list_x = 220.0F;
static Float list_y = 150.0F;
static Float row_step = 35.0F;
/* the list's row at the window's top. the window holds every lesson, so it
 * stays 0 */
static Sint32 scroll_top = 0;
/* clang-format off */
static struct menu_row lesson_rows[TUTORIAL_LESSONS] = {
    {0, 5},
    {1, 6},
    {2, 7},
    {3, 8},
    {4, 9},
};
/* clang-format on */
static NJS_POINT3 stops[STOPS] = {
#include "advertise/tutorial_panel/stops.inc"
};
/* the point the model faces under TUTORIAL_MODEL_HOME, where the camera's
 * motion places the camera */
static NJS_POINT3 face_point = {
#include "advertise/tutorial_panel/face_point.inc"
};
static NJS_TEXNAME texnames[23] = {
#include "advertise/tutorial_panel/texnames.inc"
};
static NJS_TEXLIST texlist = {
#include "advertise/tutorial_panel/texlist.inc"
};
static NJS_TEXNAME kihon_texnames[8] = {
#include "advertise/tutorial_panel/kihon_texnames.inc"
};
NJS_TEXLIST tutorial_kihon_texlist = {
#include "advertise/tutorial_panel/tutorial_kihon_texlist.inc"
};
static NJS_TEXNAME sonic_texnames[18] = {
#include "advertise/tutorial_panel/sonic_texnames.inc"
};
NJS_TEXLIST tutorial_sonic_texlist = {
#include "advertise/tutorial_panel/tutorial_sonic_texlist.inc"
};
static NJS_TEXNAME tails_texnames[19] = {
#include "advertise/tutorial_panel/tails_texnames.inc"
};
NJS_TEXLIST tutorial_tails_texlist = {
#include "advertise/tutorial_panel/tutorial_tails_texlist.inc"
};
static NJS_TEXNAME knuckles_texnames[25] = {
#include "advertise/tutorial_panel/knuckles_texnames.inc"
};
NJS_TEXLIST tutorial_knuckles_texlist = {
#include "advertise/tutorial_panel/tutorial_knuckles_texlist.inc"
};
static NJS_TEXNAME chao_texnames[17] = {
#include "advertise/tutorial_panel/chao_texnames.inc"
};
NJS_TEXLIST tutorial_chao_texlist = {
#include "advertise/tutorial_panel/tutorial_chao_texlist.inc"
};

static void tutorial_sprite_draw(Int cell, Float scale, NJS_POINT3 *pos,
                                 Int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &texlist;
    sprite.tanim = tutorial_cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

static void tutorial_camera_set(void)
{
    NJS_CAMERA camera;

    njInitCamera(&camera);
    njCameraMotion(&camera, tutorial_camera_motion, tutorial_anim.frame);
    simple_light_set_direction(0.0F, 0.0F, 1.0F);
    njCnkSetSimpleLightIntensity(1.0F, 0.6F);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
    easy_light_set_direction(0.0F, 0.0F, 1.0F);
    njCnkSetEasyLightIntensity(1.0F, 1.0F);
    njCnkSetEasyLightColor(0.0F, 0.0F, 0.0F);
}

static void tutorial_model_draw(void)
{
    if (tutorial_anim.level < SOLID_LEVEL) {
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
    }
    /* @bug logic: below SOLID_LEVEL, the draw leaves the constant attributes
     * as njSetConstantAttr set them. menu_fade_begin saves the attributes
     * again with draw_attr_save, over the draw's own save, so the
     * draw_attr_restore at the end restores CHUNK_TYPE_BITS and NJD_FST_UA
     * (the saves at 0x8c5169e4 and in the call at 0x8c516a10) */
    menu_fade_begin(tutorial_anim.level);
    njSetTexture(&texlist);
    njPushMatrixEx();
    njTranslate(NULL, tutorial_anim.pos.x, SHADOW_Y, tutorial_anim.pos.z);
    njScale(NULL, 1.0F, SHADOW_SCALE, 1.0F);
    njRotateX(NULL, tutorial_anim.ang.x);
    njRotateY(NULL, tutorial_anim.ang.y);
    njCnkEasyDrawObject(&tutorial_model_object);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateV(NULL, &tutorial_anim.pos);
    njRotateX(NULL, tutorial_anim.ang.x);
    njRotateY(NULL, tutorial_anim.ang.y);
    njCnkSimpleDrawObject(&tutorial_model_object);
    njPopMatrixEx();
    if (tutorial_anim.level < SOLID_LEVEL) {
        draw_control_restore();
        draw_attr_restore();
    }
}

static void tutorial_frame_draw(void)
{
    /* @hack spelling: the locals declared row, then pos, where another
     * order would do */
    struct menu_sprite_row row;
    NJS_POINT3 pos;

    menu_fade_begin(tutorial_anim.level);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    pos.x = FRAME_X;
    pos.y = FRAME_Y;
    pos.z = PANEL_PRIORITY;
    row.tanim = tutorial_cells;
    row.first = FRAME_CELL;
    row.count = FRAME_CELLS;
    row.pos = &pos;
    row.draw = tutorial_sprite_draw;
    menu_sprite_row_draw(&row);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    menu_backdrop_draw(BACKDROP_TEXTURE, &texlist, BACKDROP_PRIORITY,
                       MENU_BACKDROP_TINT);
}

static void tutorial_list_draw(void)
{
    struct menu_scroll scroll;
    struct menu_list list;

    menu_fade_begin(tutorial_anim.list_level);
    scroll.top = &scroll_top;
    scroll.cursor = &tutorial_work.lesson;
    scroll.shown = TUTORIAL_LESSONS;
    list.rows = lesson_rows;
    list.count = TUTORIAL_LESSONS;
    list.lit_back = ROW_LIT_BACK;
    list.dim_back = ROW_DIM_BACK;
    list.language_offset = LANGUAGE_OFFSET;
    list.pos.x = list_x;
    list.pos.y = list_y;
    list.pos.z = LIST_PRIORITY;
    list.step.x = 0.0F;
    list.step.y = row_step;
    list.step.z = 0.0F;
    list.draw = tutorial_sprite_draw;
    menu_list_draw(&list, &scroll, 1.0F);
}

static void tutorial_board_draw(void)
{
    njTranslate(NULL, -((1.0F - tutorial_anim.board_level) * SLIDE_DISTANCE),
                0.0F, 0.0F);
    njSetTexture(&texlist);
    njCnkSimpleDrawObject(&tutorial_board_object);
    if (tutorial_work.picture >= 0) {
        njSetTexture(tutorial_pictures[tutorial_work.page].texlist);
    }
    njCnkSimpleDrawObject(&tutorial_picture_object);
}

static void tutorial_model_move(Int mode)
{
    /* @hack redundant: unused, a vector nothing reads */
    NJS_VECTOR unused;
    NJS_VECTOR vec;
    struct angles ang;

    /* @hack spelling: each case's turn toward the heading written out,
     * where one after the switch would do */
    switch (mode) {
    case TUTORIAL_MODEL_HOME:
        vector_diff(&tutorial_anim.pos, &stops[0], &vec);
        if (njScalor2(&vec) < HOME_RANGE_SQUARED) {
            tutorial_bob_phases[0] += BOB_STEP_X;
            tutorial_bob_phases[1] += BOB_STEP_Y;
            tutorial_bob_phases[2] += BOB_STEP_Z;
            tutorial_anim.pos.x =
                stops[0].x + (njSin(tutorial_bob_phases[0]) * BOB_SIZE);
            tutorial_anim.pos.y =
                stops[0].y + (njSin(tutorial_bob_phases[1]) * BOB_SIZE);
            tutorial_anim.pos.z =
                stops[0].z + (njSin(tutorial_bob_phases[2]) * BOB_SIZE);
        } else {
            menu_approach(&tutorial_anim.pos.x, stops[0].x, HOME_STEP);
            menu_approach(&tutorial_anim.pos.y, stops[0].y, HOME_STEP);
            menu_approach(&tutorial_anim.pos.z, stops[0].z, HOME_STEP);
            tutorial_anim.spd.x = 0.0F;
            tutorial_anim.spd.y = 0.0F;
            tutorial_anim.spd.z = 0.0F;
        }
        vector_diff(&face_point, &tutorial_anim.pos, &vec);
        direction_angles_xy(vec.x, vec.y, vec.z, &ang.x, &ang.y);
        tutorial_anim.ang.x =
            angle_approach(tutorial_anim.ang.x, ang.x, TURN_STEP);
        tutorial_anim.ang.y =
            angle_approach(tutorial_anim.ang.y, ang.y, TURN_STEP);
        break;
    case TUTORIAL_MODEL_SWIM:
        menu_accelerate(&tutorial_anim.pos.x, stops[tutorial_anim.stop].x,
                        &tutorial_anim.spd.x, SWIM_ACCEL);
        menu_accelerate(&tutorial_anim.pos.y, stops[tutorial_anim.stop].y,
                        &tutorial_anim.spd.y, SWIM_ACCEL);
        menu_accelerate(&tutorial_anim.pos.z, stops[tutorial_anim.stop].z,
                        &tutorial_anim.spd.z, SWIM_ACCEL);
        if (_builtin_fabsf(tutorial_anim.spd.x) > SWIM_SPEED) {
            if (tutorial_anim.spd.x > 0.0F) {
                tutorial_anim.spd.x = SWIM_SPEED;
            } else {
                tutorial_anim.spd.x = -SWIM_SPEED;
            }
        }
        if (_builtin_fabsf(tutorial_anim.spd.y) > SWIM_SPEED) {
            if (tutorial_anim.spd.y > 0.0F) {
                tutorial_anim.spd.y = SWIM_SPEED;
            } else {
                tutorial_anim.spd.y = -SWIM_SPEED;
            }
        }
        if (_builtin_fabsf(tutorial_anim.spd.z) > SWIM_SPEED) {
            if (tutorial_anim.spd.z > 0.0F) {
                tutorial_anim.spd.z = SWIM_SPEED;
            } else {
                tutorial_anim.spd.z = -SWIM_SPEED;
            }
        }
        vector_sum(&tutorial_anim.pos, &tutorial_anim.spd, &tutorial_anim.pos);
        vector_diff(&tutorial_anim.pos, &stops[tutorial_anim.stop], &vec);
        if (njScalor2(&vec) < SWIM_RANGE_SQUARED) {
            tutorial_anim.stop = (Sint32)((Float)rand() * (STOPS * RAND_SCALE));
        }
        vec = tutorial_anim.spd;
        direction_angles_xy(vec.x, vec.y, vec.z, &ang.x, &ang.y);
        tutorial_anim.ang.x =
            angle_approach(tutorial_anim.ang.x, ang.x, TURN_STEP);
        tutorial_anim.ang.y =
            angle_approach(tutorial_anim.ang.y, ang.y, TURN_STEP);
        break;
    }
    tutorial_spin_z_object.evalflags &= ~NJD_EVAL_UNIT_ANG;
    tutorial_spin_y_object.evalflags &= ~NJD_EVAL_UNIT_ANG;
    tutorial_spin_z_object.ang[2] += SPIN_Z_STEP;
    tutorial_spin_y_object.ang[1] += SPIN_Y_STEP;
}

static void tutorial_board_set_picture(void)
{
    /* @hack spelling: the chunk list through a local pointer, where its own
     * name would do */
    Sint8 *plist;

    plist = (Sint8 *)tutorial_picture_plist;
    if (tutorial_work.picture < 0) {
        plist[PICTURE_TEXTURE_BYTE] = NO_PICTURE_TEXTURE;
    } else {
        plist[PICTURE_TEXTURE_BYTE] = tutorial_work.picture;
    }
}

static void tutorial_panel_step(struct task *task)
{
    /* @hack spelling: the fadings cleared in this order, where the work's
     * order would do */
    tutorial_work.fading = FALSE;
    tutorial_work.list.moving = FALSE;
    tutorial_work.board.moving = FALSE;
    tutorial_work.model.moving = FALSE;
    if (!tutorial_work.shown) {
        tutorial_work.model.shown = FALSE;
        tutorial_work.list.shown = FALSE;
        tutorial_work.board.shown = FALSE;
    } else {
        tutorial_model_move(tutorial_work.model_mode);
    }
    if (tutorial_work.model.shown) {
        if (menu_approach(&tutorial_anim.level, 1.0F, FADE_STEP)) {
            tutorial_work.model.moving = TRUE;
        }
        if (tutorial_work.camera_moved) {
            menu_approach(&tutorial_anim.frame,
                          tutorial_camera_motion->nbFrame - 1.0F, FRAME_STEP);
        } else {
            menu_approach(&tutorial_anim.frame, 0.0F, FRAME_STEP);
        }
    } else {
        if (menu_approach(&tutorial_anim.level, 0.0F, FADE_STEP)) {
            tutorial_work.model.moving = TRUE;
        }
    }
    if (tutorial_work.list.shown) {
        if (menu_approach(&tutorial_anim.list_level, 1.0F, FADE_STEP)) {
            tutorial_work.list.moving = TRUE;
        }
    } else {
        if (menu_approach(&tutorial_anim.list_level, 0.0F, FADE_STEP)) {
            tutorial_work.list.moving = TRUE;
        }
    }
    if (tutorial_work.board.shown) {
        if (menu_approach(&tutorial_anim.board_level, 1.0F, FADE_STEP)) {
            tutorial_work.board.moving = TRUE;
        }
    } else {
        if (menu_approach(&tutorial_anim.board_level, 0.0F, FADE_STEP)) {
            tutorial_work.board.moving = TRUE;
        }
    }
    tutorial_board_set_picture();
    /* @hack redundant: fading ORed in, where it is FALSE already */
    tutorial_work.fading = tutorial_work.fading | tutorial_work.model.moving |
                           tutorial_work.list.moving |
                           tutorial_work.board.moving;
}

static void tutorial_panel_draw(struct task *task)
{
    njFogDisable();
    /* @hack spelling: each level tested alone, here and below, where a
     * compare with 0.0F would do */
    if (tutorial_anim.level || tutorial_anim.board_level) {
        tutorial_camera_set();
    }
    if (tutorial_anim.level) {
        tutorial_frame_draw();
        tutorial_model_draw();
    }
    if (tutorial_anim.list_level) {
        tutorial_list_draw();
    }
    if (tutorial_anim.board_level) {
        tutorial_board_draw();
    }
}

static void tutorial_panel_destroy(struct task *task)
{
    tutorial_panel_release();
}

void tutorial_panel_create(void)
{
    struct task *task;
    NJS_TEXANIM *cell;
    Uint8 *byte;
    int left;
    Int i;

    task =
        task_create(TASK_WORK, TASK_LEVEL, tutorial_panel_step, "tutorialExec");
    task->draw = tutorial_panel_draw;
    task->destructor = tutorial_panel_destroy;
    panel_task = task;
    byte = (Uint8 *)&tutorial_work;
    /* @hack spelling: a pointer beside a count of the bytes left tested
     * against 0, where byte[i] for an unsigned i counting up would do */
    for (left = sizeof(tutorial_work); left != 0; left--) {
        *byte++ = 0;
    }
    tutorial_anim = anim_start;
    /* @hack redundant: a do loop that runs once */
    do {
        cell = tutorial_cells;
    } while (0);
    for (i = 0; i < TUTORIAL_CELLS; i++) {
        cell[i].cx = cell[i].sx / 2;
        cell[i].cy = cell[i].sy / 2;
    }
}

void tutorial_panel_end(void)
{
    if (panel_task) {
        task_destroy(panel_task);
        panel_task = NULL;
    }
}

static void tutorial_picture_load(Int lesson);

void tutorial_panel_load(void)
{
    Int i;

    if (!tutorial_work.loaded) {
        texture_load_pvm(TUTORIAL_PANEL_ARCHIVE, &texlist);
        for (i = 0; i < TUTORIAL_LESSONS; i++) {
            tutorial_picture_load(i);
        }
        tutorial_work.loaded = TRUE;
    }
}

void tutorial_panel_release(void)
{
    Int i;

    if (tutorial_work.loaded) {
        njReleaseTexture(&texlist);
        for (i = 0; i < TUTORIAL_LESSONS; i++) {
            njReleaseTexture(tutorial_pictures[i].texlist);
            tutorial_work.pictures_loaded[i] = FALSE;
        }
        tutorial_work.loaded = FALSE;
    }
}

/* load lesson's pictures where its pictures_loaded is 0, and set it */
static void tutorial_picture_load(Int lesson)
{
    /* @hack spelling: each lesson's flag compared with FALSE, where ! would
     * do */
    switch (lesson) {
    case 0:
        if (tutorial_work.pictures_loaded[0] == FALSE) {
            texture_load_pvm(TUTORIAL_PANEL_KIHON_ARCHIVE,
                             tutorial_pictures[0].texlist);
            tutorial_work.pictures_loaded[0] = TRUE;
        }
        break;
    case 1:
        if (tutorial_work.pictures_loaded[1] == FALSE) {
            texture_load_pvm(TUTORIAL_PANEL_SONIC_ARCHIVE,
                             tutorial_pictures[1].texlist);
            tutorial_work.pictures_loaded[1] = TRUE;
        }
        break;
    case 2:
        if (tutorial_work.pictures_loaded[2] == FALSE) {
            texture_load_pvm(TUTORIAL_PANEL_TAILS_ARCHIVE,
                             tutorial_pictures[2].texlist);
            tutorial_work.pictures_loaded[2] = TRUE;
        }
        break;
    case 3:
        if (tutorial_work.pictures_loaded[3] == FALSE) {
            texture_load_pvm(TUTORIAL_PANEL_KNUCKLES_ARCHIVE,
                             tutorial_pictures[3].texlist);
            tutorial_work.pictures_loaded[3] = TRUE;
        }
        break;
    case 4:
        if (tutorial_work.pictures_loaded[4] == FALSE) {
            texture_load_pvm(TUTORIAL_PANEL_CHAO_ARCHIVE,
                             tutorial_pictures[4].texlist);
            tutorial_work.pictures_loaded[4] = TRUE;
        }
        break;
    }
}
