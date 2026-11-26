/* the emblem screen: an emblem that turns, and the count of emblems.
 * EMBLEMGET.PRS and CHAOSTGRACE.PRS hold it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "draw/fade.h"
#include "draw/light.h"
#include "save/save.h"
#include "shared/congratulations.h"
#include "shared/emblem.h"
#include "shared/sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "system/vector.h"

#include "shared/emblem/text.inc"

Sint32 emblem_screen_open;
static NJS_CAMERA emblem_camera;

#define EMBLEM_LEVEL 2

/* the screen's timeline, in steps past the end of the emblem's motion. the new
 * count shows from NEW_COUNT_STEPS. the fade out runs from FADE_OUT_STEPS to
 * FADE_OUT_END_STEPS. the sparkles stop at FADE_OUT_END_STEPS too. the screen
 * ends at END_STEPS */
#define NEW_COUNT_STEPS 60
#define FADE_OUT_STEPS 180
/* @hack type: FADE_OUT_END_STEPS and END_STEPS as Floats, where int step
 * counts would do */
#define FADE_OUT_END_STEPS 240.0F
#define END_STEPS 300.0F
#define NEW_COUNT_SOUND 0x8006
/* a sparkle starts on the two steps of every four whose count has SPARKLE_SKIP
 * clear */
#define SPARKLE_SKIP 2
#define SPARKLE_TURN NJM_DEG_ANG(5)
/* a random offset from -1 up to 1 */
#define JITTER() ((rand() * (2.0F * RAND_SCALE)) - 1.0F)

/* the count's place on the screen. HUNDREDS_X, TENS_X and UNITS_X are where a
 * count of three, two or one digits starts. HUNDREDS_SPACING and TENS_SPACING
 * step to the next digit */
#define COUNT_Y 350.0F
#define COUNT_PRI (-35.0F)
#define HUNDREDS_X 250.0F
#define HUNDREDS_SPACING 44.0F
#define TENS_X 270.0F
#define TENS_SPACING 50.0F
#define UNITS_X 295.0F

static void emblem_step(struct task *task);
static void emblem_draw(struct task *task);
static void emblem_destroy(struct task *task);
static void emblem_draw_count(int count);

static struct task *emblem_task = NULL;
/* the steps the emblem's task has taken */
static Sint32 emblem_steps = 0;
/* where the emblem stands, and its turns about x, y and z, in degrees */
static Float emblem_x = 0.0F;
static Float emblem_y = 0.0F;
static Float emblem_z = -35.0F;
static Float emblem_ang_x = 0.0F;
static Float emblem_ang_y = 0.0F;
static Float emblem_ang_z = 0.0F;
/* a sparkle's size, where it starts before its turn about y, and its speed */
static Float emblem_sparkle_scale = 2.0F;
static Float emblem_sparkle_x = 12.0F;
static Float emblem_sparkle_y = 0.0F;
static Float emblem_sparkle_z = 0.0F;
static Float emblem_sparkle_speed_x = 0.0F;
static Float emblem_sparkle_speed_y = 0.0F;
static Float emblem_sparkle_speed_z = 0.0F;
static Sint32 dat_emblem_0 = 123;
static Angle emblem_sparkle_turn = 0;
static Float emblem_frame = 0.0F;

/* tentative definitions, for emblem_start, emblem_step and emblem_draw. SHC
 * places initialized data in the order of its definitions. emblem_draw's
 * static, emblem_sparkle_origin, lies between the variables above and this
 * data, so the data follows emblem_draw */
static NJS_TEXLIST emblem_texlist;
static NJS_CNK_OBJECT emblem_object;
static NJS_MOTION emblem_motion[1];

Bool emblem_screen_is_open(void)
{
    if (emblem_screen_open) {
        return TRUE;
    }
    return FALSE;
}

void emblem_start(void)
{
    njInitCamera(&emblem_camera);
    njSetCamera(&emblem_camera);
    texture_load_pvm(EMBLEM_ARCHIVE, &emblem_texlist);
    emblem_task =
        task_create(TASK_WORK, EMBLEM_LEVEL, emblem_step, EMBLEM_MANAGER);
    emblem_screen_open = 1;
    game_state.unlock_pending = 0;
}

static void emblem_step(struct task *task)
{
    task->draw = emblem_draw;
    task->step = emblem_step;
    task->destructor = emblem_destroy;
    if (emblem_steps >= emblem_motion->nbFrame + END_STEPS ||
        game_state.emblems == save_data.emblems) {
        task_destroy(task);
        if (game_state.unlock_id > 0) {
            congratulations_start();
        } else {
            emblem_screen_open = 0;
        }
    }
    ++emblem_steps;
}

static void emblem_draw(struct task *task)
{
    /* where the last sparkle started, (1, 0, 0) before the first. the light
     * shines from there onto the emblem */
    static NJS_POINT3 emblem_sparkle_origin = {1.0F, 0.0F, 0.0F};
    struct task_work *work = task->work;
    NJS_VECTOR light;
    NJS_VECTOR speed;

    njSetCamera(&emblem_camera);
    work->pos.x = emblem_x;
    work->pos.y = emblem_y;
    work->pos.z = emblem_z;
    vector_diff(&work->pos, &emblem_sparkle_origin, &light);
    njUnitVector(&light);
    njCalcVector(NULL, &light, &light);
    simple_light_set_direction(light.x, light.y, light.z);
    njCnkSetSimpleLightIntensity(1.0F, 0.5F);
    njCnkSetSimpleLightColor(1.0F, 1.0F, 1.0F);
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, (Angle)(emblem_ang_z * ANGLE_PER_DEGREE));
    njRotateX(NULL, (Angle)(emblem_ang_x * ANGLE_PER_DEGREE));
    njRotateY(NULL, (Angle)(emblem_ang_y * ANGLE_PER_DEGREE));
    njSetTexture(&emblem_texlist);
    njCnkSimpleDrawMotion(&emblem_object, emblem_motion, emblem_frame);
    njPopMatrixEx();
    if (emblem_steps < emblem_motion->nbFrame + FADE_OUT_END_STEPS &&
        !(emblem_steps & SPARKLE_SKIP)) {
        njPushMatrixEx();
        emblem_sparkle_origin.x = emblem_sparkle_x;
        emblem_sparkle_origin.y = emblem_sparkle_y;
        emblem_sparkle_origin.z = emblem_sparkle_z;
        njUnitMatrix(NULL);
        njRotateY(NULL, emblem_sparkle_turn);
        njCalcVector(NULL, &emblem_sparkle_origin, &emblem_sparkle_origin);
        vector_sum(&emblem_sparkle_origin, &work->pos, &emblem_sparkle_origin);
        emblem_sparkle_origin.x += JITTER();
        emblem_sparkle_origin.y += JITTER();
        emblem_sparkle_origin.z += JITTER();
        speed.x = emblem_sparkle_speed_x;
        speed.y = emblem_sparkle_speed_y;
        speed.z = emblem_sparkle_speed_z;
        sparkle_create(&emblem_sparkle_origin, &speed, emblem_sparkle_scale);
        njPopMatrixEx();
    }
    if (emblem_frame < emblem_motion->nbFrame - 1.0F) {
        emblem_frame += 1.0F;
    }
    emblem_sparkle_turn += SPARKLE_TURN;
    if ((Uint32)emblem_steps < emblem_motion->nbFrame + NEW_COUNT_STEPS) {
        fade_in_step();
        emblem_draw_count(game_state.emblems);
    } else if ((Uint32)emblem_steps < emblem_motion->nbFrame + FADE_OUT_STEPS) {
        if ((Uint32)emblem_steps == emblem_motion->nbFrame + NEW_COUNT_STEPS) {
            sound_play(NEW_COUNT_SOUND, NULL, 0, 0);
        }
        emblem_draw_count(save_data.emblems);
    } else if (emblem_steps < emblem_motion->nbFrame + FADE_OUT_END_STEPS) {
        fade_out_step();
        emblem_draw_count(save_data.emblems);
    } else {
        emblem_draw_count(save_data.emblems);
    }
}

static Float emblem_count_x = 300.0F;
static Float dat_emblem_1 = 300.0F;
static Float dat_emblem_2 = -200.0F;
static Float dat_emblem_3 = 30.0F;
static Float emblem_digit_scale = 2.0F;

static NJS_TEXNAME emblem_texnames[6] = {
#include "shared/emblem/emblem_texnames.inc"
};
static NJS_TEXLIST emblem_texlist = {
#include "shared/emblem/emblem_texlist.inc"
};

#define EMBLEM_POINTS 192
static Sint16 emblem_polygons[3509] = {
#include "shared/emblem/emblem_polygons.inc"
};
static Sint32 emblem_vertices[1155] = {
#include "shared/emblem/emblem_vertices.inc"
};
static NJS_CNK_MODEL emblem_model = {
#include "shared/emblem/emblem_model.inc"
};
static NJS_CNK_OBJECT emblem_object = {
#include "shared/emblem/emblem_object.inc"
};
/* the emblem's motion: three turns about y over 120 frames, slowing to a stop.
 * frame 118 has no key */
static NJS_MKEY_A emblem_ang_keys[119] = {
#include "shared/emblem/emblem_ang_keys.inc"
};
static NJS_MDATA2 emblem_motion_data[1] = {
#include "shared/emblem/emblem_motion_data.inc"
};
static NJS_MOTION emblem_motion[1] = {
#include "shared/emblem/emblem_motion.inc"
};
/* the cells of fileselect_suuji: the digits 0 to 9, then three more */
static NJS_TEXANIM emblem_digit_cells[13] = {
#include "shared/emblem/emblem_digit_cells.inc"
};

static void emblem_destroy(struct task *task)
{
    njReleaseTexture(&emblem_texlist);
    emblem_task = NULL;
    game_state.emblems = save_data.emblems;
}

/* draw emblem_digit_cells[cell] at pos, scaled by scale. pos->z is the sprite's
 * priority */
/* @hack type: attr an int, where the Uint32 that njDrawSprite2D takes would
 * do */
static void emblem_draw_digit(int cell, Float scale, NJS_POINT3 *pos, int attr)
{
    NJS_SPRITE sprite;

    sprite.tlist = &emblem_texlist;
    sprite.tanim = emblem_digit_cells;
    sprite.sx = scale;
    sprite.sy = scale;
    sprite.p.x = pos->x;
    sprite.p.y = pos->y;
    sprite.ang = 0;
    njDrawSprite2D(&sprite, cell, pos->z, attr);
}

/* draw count, which must be 0 to 999, centered on the screen */
static void emblem_draw_count(int count)
{
    NJS_POINT3 pos;
    /* @hack spelling: spacing, a local for the step to the next digit, where
     * the step itself would do */
    Float spacing;

    /* @hack redundant: a store of emblem_count_x that every branch below
     * overwrites */
    pos.x = emblem_count_x;
    pos.y = COUNT_Y;
    pos.z = COUNT_PRI;
    if (count >= 100) {
        spacing = HUNDREDS_SPACING;
        pos.x = HUNDREDS_X;
        emblem_draw_digit(count / 100, emblem_digit_scale, &pos,
                          NJD_SPRITE_ALPHA);
        count %= 100;
        pos.x += spacing;
        emblem_draw_digit(count / 10, emblem_digit_scale, &pos,
                          NJD_SPRITE_ALPHA);
        count %= 10;
        pos.x += spacing;
        emblem_draw_digit(count, emblem_digit_scale, &pos, NJD_SPRITE_ALPHA);
    } else if (count >= 10) {
        pos.x = TENS_X;
        spacing = TENS_SPACING;
        emblem_draw_digit(count / 10, emblem_digit_scale, &pos,
                          NJD_SPRITE_ALPHA);
        count %= 10;
        pos.x += spacing;
        emblem_draw_digit(count, emblem_digit_scale, &pos, NJD_SPRITE_ALPHA);
    } else {
        pos.x = UNITS_X;
        emblem_draw_digit(count, emblem_digit_scale, &pos, NJD_SPRITE_ALPHA);
    }
}
