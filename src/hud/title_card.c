/* the title card a stage opens with: its name, and six overlays that move
 * and fade about it. the name's task holds the card up until play starts */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <string.h>

#include "hud/mission_message.h"
#include "hud/title_card.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#ifdef NONMATCHING
/* port_wide and port_pillarbox, by which this side carries the name off the
 * wider picture */
#include "engine/dc/port.h"
#endif

/* the textures of a character's MISSIONTEX_<code>2 PVM, by their places in
 * missiontex2_texlist */
#define MISSIONTEX2_TATEMOJI 0
#define MISSIONTEX2_KAZARI1 1
#define MISSIONTEX2_TEXTURES 2
/* the one texture of the title's PVM. it holds the name's two rows. below
 * them lie the piece stage_num_display slides and the blue band's two
 * pieces */
#define STAGE_TITLE 0

#define SOUND_TITLE_CARD 0x8019 /* the sound the card starts with */

/* the task levels of the stage name and of the overlays.
 * task_destroy_levels_2_to_5 spares the stage name's */
#define NAME_LEVEL 0
#define OVERLAY_LEVEL 2

/* the place in stage_title_name of the stage's two digits, after the nine
 * letters of "stg_title" */
#define TITLE_NUMBER_AT 9

/* an overlay's color: white, at an alpha from 0 to 1 */
#define WHITE_AT(alpha) (((Uint32)((alpha) * 255.0F) << 24) | 0x00ffffff)

#include "hud/title_card/text.inc"

static NJS_TEXNAME missiontex_texname[MISSIONTEX_TEXTURES] = {
#include "hud/title_card/missiontex_texname.inc"
};
NJS_TEXLIST missiontex_texlist = {missiontex_texname, MISSIONTEX_TEXTURES};
static NJS_TEXNAME missiontex2_texname[MISSIONTEX2_TEXTURES] = {
#include "hud/title_card/missiontex2_texname.inc"
};
static NJS_TEXLIST missiontex2_texlist = {missiontex2_texname,
                                          MISSIONTEX2_TEXTURES};
/* the title's list, as texture_load_pvm_alloc made it */
static NJS_TEXLIST *stage_title_texlist = NULL;

/* the overlays' quads, in pixels, each with its priority and its part of its
 * texture. the steps and draws write the x of each corner they move. the draws
 * that use a matrix take a quad's x and y from the _corners points after it.
 * they take its color from an alpha the work holds or gives. the blue band's
 * first four corners are a tile of its edge. the last four are the band, left
 * of the edge */
static NJS_TEXTURE_VTX blue_band_quads[8] = {
#include "hud/title_card/blue_band_quads.inc"
};
static NJS_TEXTURE_VTX adv2_quad[4] = {
#include "hud/title_card/adv2_quad.inc"
};
static NJS_POINT3 adv2_corners[4] = {
#include "hud/title_card/adv2_corners.inc"
};
static NJS_TEXTURE_VTX adv_quad[4] = {
#include "hud/title_card/adv_quad.inc"
};
static NJS_POINT3 adv_corners[4] = {
#include "hud/title_card/adv_corners.inc"
};
static NJS_TEXTURE_VTX dec1_quad[4] = {
#include "hud/title_card/dec1_quad.inc"
};
static NJS_POINT3 dec1_corners[4] = {
#include "hud/title_card/dec1_corners.inc"
};
static NJS_TEXTURE_VTX dec0_quad[4] = {
#include "hud/title_card/dec0_quad.inc"
};
static NJS_TEXTURE_VTX stage_num_quad[4] = {
#include "hud/title_card/stage_num_quad.inc"
};
/* the stage's name, its upper row and then its lower */
static NJS_TEXTURE_VTX stage_name_quads[8] = {
#include "hud/title_card/stage_name_quads.inc"
};
#ifdef NONMATCHING
#else
/* the name of the title's PVM. title_card_start writes the stage's two
 * digits over its last two, in the string literal itself */
/* @hack type: the name held in an array of one pointer, where a pointer
 * would do */
static char *stage_title_name[1] = {"stg_title00"};
#endif

/* the overlays' modes, in their tasks' work. each overlay starts in its
 * _START mode, which sets it up and moves on to the next */
enum blue_band_mode {
    BLUE_BAND_START = 0,
    BLUE_BAND_SLIDE_IN = 1,
    BLUE_BAND_HOLDING = 2,
    BLUE_BAND_SLIDE_OUT = 3
};
enum adv2_mode {
    ADV2_START = 0,
    ADV2_FADE_IN = 1,
    ADV2_HOLDING = 2,
    ADV2_FADE_OUT = 3
};
enum adv_mode { ADV_START = 0, ADV_SHRINK = 1, ADV_HOLDING = 2, ADV_GROW = 3 };
enum dec1_mode {
    DEC1_START = 0,
    DEC1_TURN_IN = 1,
    DEC1_FADE_START = 2,
    DEC1_FADE_OUT = 3
};
enum dec0_mode {
    DEC0_START = 0,
    DEC0_WAITING = 1,
    DEC0_SLIDE_IN = 2,
    DEC0_HOLDING = 3,
    DEC0_SLIDE_OUT = 4
};
enum stage_num_mode {
    STAGE_NUM_START = 0,
    STAGE_NUM_SLIDE_IN = 1,
    STAGE_NUM_HOLDING = 2,
    STAGE_NUM_SLIDE_OUT = 3
};
/* the stage name's modes, which it steps through in order */
enum stage_name_mode {
    NAME_START = 0,
    NAME_SLIDE_IN = 1,
    NAME_HOLDING = 2,
    /* by this mode, NAME_HOLDING has released missiontex2_texlist and
     * recorded STAGE_END_CARD_SHOWN */
    NAME_WAITING = 3,
    NAME_UPPER_OUT = 4, /* the upper row slides out alone */
    NAME_SLIDE_OUT = 5,
    NAME_ENDING = 6
};

/* the x of the blue band's edge, in pixels. the edge is a tile
 * BLUE_BAND_TILE pixels square, drawn BLUE_BAND_TILES times down from the
 * screen's top. the band is BLUE_BAND_WIDTH pixels wide, left of the edge */
#define BLUE_BAND_OUT 0.0F
#define BLUE_BAND_IN 129.0F
#define BLUE_BAND_IN_STEP 16.0F
#define BLUE_BAND_HOLD 150
#define BLUE_BAND_OUT_STEP 8.0F
#define BLUE_BAND_TILE 32.0F
#define BLUE_BAND_TILES 15
#define BLUE_BAND_WIDTH 160.0F

#define ADV2_SCROLL_TO (-512.0F)
#define ADV2_SCROLL_STEP 8.0F
#define ADV2_FADE_IN_STEP 0.04F
#define ADV2_HOLD 48
#define ADV2_FADE_OUT_STEP 0.05F

/* adv_display's place, and the scale it starts and ends at. it grows back
 * toward ADV_GROW_TO, and ends once it reaches ADV_SCALE. ADV_Z moves
 * nothing on the screen, since the draws take only the corners' x and y */
#define ADV_X 129.0F
#define ADV_Y 0.0F
#define ADV_Z 0.52F
#define ADV_SCALE 2.0F
#define ADV_SHRINK_STEP 0.04F
#define ADV_HOLD 40
#define ADV_GROW_TO 20.0F
#define ADV_GROW_STEP 0.025F

/* dec1_display's place, and the turn and the scale it starts at. its work
 * keeps its alpha in scl.x and its scale in scl.z */
#define DEC1_X 394.0F
#define DEC1_Y 256.0F
#define DEC1_Z 0.5F
#define DEC1_TURN (-QUARTER_TURN)
#define DEC1_TURN_STEP 384
#define DEC1_SCALE 5.0F
#define DEC1_SCALE_STEP 0.1F
#define DEC1_FADE_IN_STEP 0.02F
#define DEC1_FADE_OUT_STEP 0.1F

/* the x of dec0_display's left and right edges, in pixels */
#define DEC0_LEFT_START (-128.0F)
#define DEC0_RIGHT_START 0.0F
#define DEC0_WAIT 7
#define DEC0_LEFT_IN 346.0F
#define DEC0_RIGHT_IN 474.0F
#define DEC0_IN_STEP 64.0F
#define DEC0_HOLD 105
#define DEC0_LEFT_OUT 640.0F
#define DEC0_RIGHT_OUT 768.0F
#define DEC0_OUT_STEP 36.0F

/* the same for stage_num_display. its slide in stops once its right edge
 * reaches STAGE_NUM_RIGHT_SHOWN, short of STAGE_NUM_RIGHT_IN */
#define STAGE_NUM_LEFT_START (-180.0F)
#define STAGE_NUM_RIGHT_START 0.0F
#define STAGE_NUM_LEFT_IN 191.0F
#define STAGE_NUM_RIGHT_IN 371.0F
#define STAGE_NUM_RIGHT_SHOWN 349.0F
#define STAGE_NUM_IN_STEP 24.0F
#define STAGE_NUM_HOLD 112
#define STAGE_NUM_OUT_STEP 32.0F

/* the x of the left and right edges of the name's rows, in pixels. the
 * lower row starts 64 pixels right of the upper. a row is 255 pixels wide
 * as it starts, and 256 once in place */
#define NAME_UPPER_LEFT_START 641.0F
#define NAME_UPPER_RIGHT_START 896.0F
#define NAME_LOWER_LEFT_START 705.0F
#define NAME_LOWER_RIGHT_START 960.0F
#define NAME_LEFT_IN 191.0F
#define NAME_RIGHT_IN 447.0F
#define NAME_LEFT_OUT (-256.0F)
#define NAME_RIGHT_OUT 0.0F
#define NAME_STEP 32.0F
/* the stage name's timer counts from 0 in each mode that reads it. the name
 * holds until the timer passes NAME_HOLD. it starts the mission message as
 * the timer reaches NAME_MESSAGE_AT. it then waits until the timer passes
 * NAME_WAIT. its upper row slides out alone through the step the timer
 * reaches NAME_UPPER_ALONE. once both rows are out, the task ends after the
 * timer passes NAME_LINGER */
#define NAME_HOLD 90
#define NAME_MESSAGE_AT 65
#define NAME_WAIT 60
#define NAME_UPPER_ALONE 5
#define NAME_LINGER 67

/* the overlays, each a task title_card_start starts. an overlay's functions
 * are named for its task, as adv_display_step is AdvDisplayExecutor's step.
 * its work holds its mode and its timer, and its last mode kills the task.
 * no overlay draws while game_state.unk_0b is 0 */

static void blue_band_display_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXTURE_VTX *quad;
    Int texture;
    int i;

    if (game_state.unk_0b == 0) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njSetTexture(stage_title_texlist);
    njSetTextureNum(STAGE_TITLE);
    texture = njGetTextureNumG();
    /* @hack spelling: the tile's corners written through quad as it steps,
     * where blue_band_quads indexed would do */
    quad = blue_band_quads;
    quad->x = work->pos.x;
    quad->y = 0.0F;
    quad++;
    quad->x = work->pos.x;
    quad->y = BLUE_BAND_TILE;
    quad++;
    quad->x = work->pos.x + BLUE_BAND_TILE;
    quad->y = 0.0F;
    quad++;
    quad->x = work->pos.x + BLUE_BAND_TILE;
    quad->y = BLUE_BAND_TILE;
    for (i = 0; i < BLUE_BAND_TILES; i++) {
        quad = blue_band_quads;
        njDrawTexture(quad, 4, texture, 1);
        /* @hack spelling: each step of quad inside the store before it,
         * where a step of its own would do */
        quad++->y += BLUE_BAND_TILE;
        quad++->y += BLUE_BAND_TILE;
        quad++->y += BLUE_BAND_TILE;
        quad->y += BLUE_BAND_TILE;
    }
    /* @hack spelling: the band's corners written through quad as it steps,
     * where blue_band_quads indexed would do */
    quad = &blue_band_quads[4];
    quad->x = work->pos.x - BLUE_BAND_WIDTH;
    quad++;
    quad->x = work->pos.x - BLUE_BAND_WIDTH;
    quad++;
    quad->x = work->pos.x;
    quad++;
    quad->x = work->pos.x;
    njDrawTexture(&blue_band_quads[4], 4, texture, 1);
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}

/* work's pos.x is the x where the band meets its edge, which still shows with
 * the band out */
static void blue_band_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case BLUE_BAND_START:
        work->pos.x = BLUE_BAND_OUT;
        work->mode = BLUE_BAND_SLIDE_IN;
        task->draw = blue_band_display_draw;
        break;
    case BLUE_BAND_SLIDE_IN:
        work->pos.x =
            float_approach(work->pos.x, BLUE_BAND_IN, BLUE_BAND_IN_STEP);
        if (work->pos.x >= BLUE_BAND_IN) {
            work->mode = BLUE_BAND_HOLDING;
        }
        break;
    case BLUE_BAND_HOLDING:
        if (work->timer++ > BLUE_BAND_HOLD) {
            work->mode = BLUE_BAND_SLIDE_OUT;
        }
        break;
    case BLUE_BAND_SLIDE_OUT:
        work->pos.x =
            float_approach(work->pos.x, BLUE_BAND_OUT, BLUE_BAND_OUT_STEP);
        if (work->pos.x <= BLUE_BAND_OUT) {
            task_kill(task);
        }
        break;
    }
}

/* draw mission_tatemoji as adv_display_draw does, but unscaled, with work's
 * scl.x as its alpha */
static void adv2_display_draw(struct task *task)
{
    struct task_work *work = task->work;
    Int texture;
    NJS_POINT3 corners[4];

    if (game_state.unk_0b == 0) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&missiontex2_texlist);
    njSetTextureNum(MISSIONTEX2_TATEMOJI);
    texture = njGetTextureNumG();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njCalcPoints(NULL, adv2_corners, corners, 4);
    njPopMatrixEx();
    adv2_quad[0].x = corners[0].x;
    adv2_quad[0].y = corners[0].y;
    adv2_quad[1].x = corners[1].x;
    adv2_quad[1].y = corners[1].y;
    adv2_quad[2].x = corners[2].x;
    adv2_quad[2].y = corners[2].y;
    adv2_quad[3].x = corners[3].x;
    adv2_quad[3].y = corners[3].y;
    adv2_quad[0].col = adv2_quad[1].col = adv2_quad[2].col = adv2_quad[3].col =
        WHITE_AT(work->scl.x);
    /* @hack spelling: the quad passed as the address of its first vertex,
     * where adv2_quad would do */
    njDrawTexture(&adv2_quad[0], 4, texture, 1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void adv2_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case ADV2_START:
        work->pos.x = ADV_X;
        work->pos.y = ADV_Y;
        work->pos.z = ADV_Z;
        work->scl.x = 0.0F;
        work->mode = ADV2_FADE_IN;
        task->draw = adv2_display_draw;
        break;
    case ADV2_FADE_IN:
        work->pos.y =
            float_approach(work->pos.y, ADV2_SCROLL_TO, ADV2_SCROLL_STEP);
        work->scl.x = float_approach(work->scl.x, 1.0F, ADV2_FADE_IN_STEP);
        if (work->scl.x >= 1.0F) {
            work->mode = ADV2_HOLDING;
        }
        break;
    case ADV2_HOLDING:
        work->pos.y =
            float_approach(work->pos.y, ADV2_SCROLL_TO, ADV2_SCROLL_STEP);
        if (work->timer++ > ADV2_HOLD) {
            work->mode = ADV2_FADE_OUT;
        }
        break;
    case ADV2_FADE_OUT:
        work->pos.y =
            float_approach(work->pos.y, ADV2_SCROLL_TO, ADV2_SCROLL_STEP);
        work->scl.x = float_approach(work->scl.x, 0.0F, ADV2_FADE_OUT_STEP);
        if (work->scl.x <= 0.0F) {
            task_kill(task);
        }
        break;
    }
}

/* draw mission_tatemoji added to what lies under it. its alpha runs from 0
 * at the scale work's scl.z keeps to 1 at a scale of 1. the draw leaves the
 * blending at NJD_COLOR_BLENDING_SRCALPHA and INVSRCALPHA */
static void adv_display_draw(struct task *task)
{
    struct task_work *work = task->work;
    Int texture;
    NJS_POINT3 corners[4];
    Float alpha;

    if (game_state.unk_0b == 0) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&missiontex2_texlist);
    njSetTextureNum(MISSIONTEX2_TATEMOJI);
    texture = njGetTextureNumG();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njScaleEx(&work->scl);
    njCalcPoints(NULL, adv_corners, corners, 4);
    njPopMatrixEx();
    adv_quad[0].x = corners[0].x;
    adv_quad[0].y = corners[0].y;
    adv_quad[1].x = corners[1].x;
    adv_quad[1].y = corners[1].y;
    adv_quad[2].x = corners[2].x;
    adv_quad[2].y = corners[2].y;
    adv_quad[3].x = corners[3].x;
    adv_quad[3].y = corners[3].y;
    alpha = (work->scl.z - work->scl.x) / (work->scl.z - 1.0F);
    if (alpha < 0.0F) {
        alpha = 0.0F;
    }
    if (alpha > 1.0F) {
        alpha = 1.0F;
    }
    adv_quad[0].col = adv_quad[1].col = adv_quad[2].col = adv_quad[3].col =
        WHITE_AT(alpha);
    /* @hack spelling: the quad passed as the address of its first vertex,
     * where adv_quad would do */
    njDrawTexture(&adv_quad[0], 4, texture, 1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* mission_tatemoji scales about the top right corner of adv_corners */
static void adv_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case ADV_START:
        work->pos.x = ADV_X;
        work->pos.y = ADV_Y;
        work->pos.z = ADV_Z;
        work->scl.x = work->scl.y = work->scl.z = ADV_SCALE;
        work->mode = ADV_SHRINK;
        task->draw = adv_display_draw;
        break;
    case ADV_SHRINK:
        work->scl.x = work->scl.y =
            float_approach(work->scl.x, 1.0F, ADV_SHRINK_STEP);
        if (work->scl.x <= 1.0F) {
            work->mode = ADV_HOLDING;
        }
        break;
    case ADV_HOLDING:
        if (work->timer++ > ADV_HOLD) {
            work->mode = ADV_GROW;
        }
        break;
    case ADV_GROW:
        work->scl.x = work->scl.y =
            float_approach(work->scl.x, ADV_GROW_TO, ADV_GROW_STEP);
        if (work->scl.x >= ADV_SCALE) {
            task_kill(task);
        }
        break;
    }
}

static void dec1_display_draw(struct task *task)
{
    struct task_work *work = task->work;
    Int texture;
    NJS_POINT3 corners[4];

    if (game_state.unk_0b == 0) {
        return;
    }
    njSetTexture(&missiontex2_texlist);
    njSetTextureNum(MISSIONTEX2_KAZARI1);
    texture = njGetTextureNumG();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njScale(NULL, work->scl.z, work->scl.z, work->scl.z);
    njCalcPoints(NULL, dec1_corners, corners, 4);
    njPopMatrixEx();
    dec1_quad[0].x = corners[0].x;
    dec1_quad[0].y = corners[0].y;
    dec1_quad[1].x = corners[1].x;
    dec1_quad[1].y = corners[1].y;
    dec1_quad[2].x = corners[2].x;
    dec1_quad[2].y = corners[2].y;
    dec1_quad[3].x = corners[3].x;
    dec1_quad[3].y = corners[3].y;
    dec1_quad[0].col = dec1_quad[1].col = dec1_quad[2].col = dec1_quad[3].col =
        WHITE_AT(work->scl.x);
    /* @hack spelling: the quad passed as in adv_display_draw */
    njDrawTexture(&dec1_quad[0], 4, texture, 1);
}

/* stg_kazari1 turns and shrinks about its middle */
static void dec1_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case DEC1_START:
        work->pos.x = DEC1_X;
        work->pos.y = DEC1_Y;
        work->pos.z = DEC1_Z;
        work->scl.x = 0.0F;
        work->scl.z = DEC1_SCALE;
        work->ang.z = DEC1_TURN;
        work->mode = DEC1_TURN_IN;
        task->draw = dec1_display_draw;
        break;
    case DEC1_TURN_IN:
        work->scl.z = float_approach(work->scl.z, 1.0F, DEC1_SCALE_STEP);
        work->scl.x = float_approach(work->scl.x, 1.0F, DEC1_FADE_IN_STEP);
        work->ang.z = angle_approach(work->ang.z, 0, DEC1_TURN_STEP);
        if (work->scl.x >= 1.0F) {
            work->mode = DEC1_FADE_START;
        }
        break;
    case DEC1_FADE_START:
        work->mode = DEC1_FADE_OUT;
        /* fall through */
    case DEC1_FADE_OUT:
        work->scl.x = float_approach(work->scl.x, 0.0F, DEC1_FADE_OUT_STEP);
        if (work->scl.x <= 0.0F) {
            task_kill(task);
        }
        break;
    }
}

static void dec0_display_draw(struct task *task)
{
    Int texture;

    if (game_state.unk_0b == 0) {
        return;
    }
    njSetTexture(&missiontex_texlist);
    njSetTextureNum(MISSIONTEX_KAZARI0);
    texture = njGetTextureNumG();
    njDrawTexture(dec0_quad, 4, texture, 1);
}

/* the timer runs on from DEC0_WAITING into DEC0_HOLDING, so DEC0_HOLD
 * counts from the wait's start */
static void dec0_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case DEC0_START:
        dec0_quad[0].x = dec0_quad[1].x = DEC0_LEFT_START;
        dec0_quad[2].x = dec0_quad[3].x = DEC0_RIGHT_START;
        work->mode = DEC0_WAITING;
        task->draw = dec0_display_draw;
        break;
    case DEC0_WAITING:
        if (work->timer++ > DEC0_WAIT) {
            work->mode = DEC0_SLIDE_IN;
        }
        break;
    case DEC0_SLIDE_IN:
        dec0_quad[0].x = dec0_quad[1].x =
            float_approach(dec0_quad[0].x, DEC0_LEFT_IN, DEC0_IN_STEP);
        dec0_quad[2].x = dec0_quad[3].x =
            float_approach(dec0_quad[2].x, DEC0_RIGHT_IN, DEC0_IN_STEP);
        if (dec0_quad[0].x >= DEC0_LEFT_IN) {
            work->mode = DEC0_HOLDING;
        }
        break;
    case DEC0_HOLDING:
        if (work->timer++ > DEC0_HOLD) {
            work->mode = DEC0_SLIDE_OUT;
        }
        break;
    case DEC0_SLIDE_OUT:
        dec0_quad[0].x = dec0_quad[1].x =
            float_approach(dec0_quad[0].x, DEC0_LEFT_OUT, DEC0_OUT_STEP);
        dec0_quad[2].x = dec0_quad[3].x =
            float_approach(dec0_quad[2].x, DEC0_RIGHT_OUT, DEC0_OUT_STEP);
        if (dec0_quad[0].x >= DEC0_LEFT_OUT) {
            task_kill(task);
        }
        break;
    }
}

static void stage_num_display_draw(struct task *task)
{
    Int texture;

    if (game_state.unk_0b == 0) {
        return;
    }
    njSetTexture(stage_title_texlist);
    njSetTextureNum(STAGE_TITLE);
    texture = njGetTextureNumG();
    njDrawTexture(stage_num_quad, 4, texture, 1);
}

static void stage_num_display_step(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case STAGE_NUM_START:
        stage_num_quad[0].x = stage_num_quad[1].x = STAGE_NUM_LEFT_START;
        stage_num_quad[2].x = stage_num_quad[3].x = STAGE_NUM_RIGHT_START;
        work->mode = STAGE_NUM_SLIDE_IN;
        task->draw = stage_num_display_draw;
        break;
    case STAGE_NUM_SLIDE_IN:
        stage_num_quad[0].x = stage_num_quad[1].x = float_approach(
            stage_num_quad[0].x, STAGE_NUM_LEFT_IN, STAGE_NUM_IN_STEP);
        stage_num_quad[2].x = stage_num_quad[3].x = float_approach(
            stage_num_quad[2].x, STAGE_NUM_RIGHT_IN, STAGE_NUM_IN_STEP);
        if (stage_num_quad[2].x >= STAGE_NUM_RIGHT_SHOWN) {
            work->mode = STAGE_NUM_HOLDING;
        }
        break;
    case STAGE_NUM_HOLDING:
        if (work->timer++ > STAGE_NUM_HOLD) {
            work->mode = STAGE_NUM_SLIDE_OUT;
        }
        break;
    case STAGE_NUM_SLIDE_OUT:
        stage_num_quad[0].x = stage_num_quad[1].x = float_approach(
            stage_num_quad[0].x, STAGE_NUM_LEFT_START, STAGE_NUM_OUT_STEP);
        stage_num_quad[2].x = stage_num_quad[3].x = float_approach(
            stage_num_quad[2].x, STAGE_NUM_RIGHT_START, STAGE_NUM_OUT_STEP);
        if (stage_num_quad[2].x <= STAGE_NUM_RIGHT_START) {
            task_kill(task);
        }
        break;
    }
}

#ifdef NONMATCHING
/* how far past the 4:3 picture's side the port's 16:9 picture shows, in the
 * 4:3 picture's pixels */
#define WIDE_MARGIN (320.0F / WIDE_ASPECT - 320.0F)

/* draw a row of the name, the four corners at quad. under 16:9 a row past
 * the 4:3 picture's side still shows in the wider picture, where the row
 * out at NAME_LEFT_OUT waits NAME_LINGER steps. so this side draws a row
 * WIDE_MARGIN further out by the share of its slide it has made from
 * NAME_LEFT_IN: none of it in place, and all of it at NAME_LEFT_OUT or at
 * the upper row's start. the rows themselves stay where the step put them */
static void name_row_draw(const NJS_TEXTURE_VTX *quad, Int texture)
{
    NJS_TEXTURE_VTX row[4];
    Float slid = quad[0].x - NAME_LEFT_IN;
    Float share;
    int i;

    if (slid < 0.0F) {
        share = slid / (NAME_LEFT_IN - NAME_LEFT_OUT);
    } else {
        share = slid / (NAME_UPPER_LEFT_START - NAME_LEFT_IN);
    }
    if (share < -1.0F) {
        share = -1.0F;
    } else if (share > 1.0F) {
        share = 1.0F;
    }
    if (!port_wide || port_pillarbox) {
        share = 0.0F;
    }
    for (i = 0; i < 4; i++) {
        row[i] = quad[i];
        row[i].x += share * WIDE_MARGIN;
    }
    njDrawTexture(row, 4, texture, 1);
}
#endif

static void stage_name_display_draw(struct task *task)
{
    Int texture;

    if (game_state.unk_0b == 0) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(stage_title_texlist);
    njSetTextureNum(STAGE_TITLE);
    texture = njGetTextureNumG();
#ifdef NONMATCHING
    /* this side carries each row off the wider picture as it slides */
    name_row_draw(stage_name_quads, texture);
    name_row_draw(&stage_name_quads[4], texture);
#else
    njDrawTexture(stage_name_quads, 4, texture, 1);
    njDrawTexture(&stage_name_quads[4], 4, texture, 1);
#endif
}

static void stage_name_display_destroy(struct task *task)
{
    if (task->work->mode < NAME_WAITING) {
        njReleaseTexture(&missiontex2_texlist);
    }
    njReleaseTexture(&missiontex_texlist);
    njReleaseTexture(stage_title_texlist);
    syFree(stage_title_texlist);
    pause_enable();
}

/* the task that holds the card up, keeping Start from opening the pause
 * menu */
static void stage_name_display_step(struct task *task)
{
    struct task_work *work = task->work;

    pause_disable();
    switch (work->mode) {
    case NAME_START:
        stage_name_quads[0].x = stage_name_quads[1].x = NAME_UPPER_LEFT_START;
        stage_name_quads[2].x = stage_name_quads[3].x = NAME_UPPER_RIGHT_START;
        stage_name_quads[4].x = stage_name_quads[5].x = NAME_LOWER_LEFT_START;
        stage_name_quads[6].x = stage_name_quads[7].x = NAME_LOWER_RIGHT_START;
        work->mode = NAME_SLIDE_IN;
        task->draw = stage_name_display_draw;
        task->destructor = stage_name_display_destroy;
        break;
    case NAME_SLIDE_IN:
        if (stage_name_quads[4].x <= NAME_LEFT_IN) {
            work->mode = NAME_HOLDING;
        }
        stage_name_quads[0].x = stage_name_quads[1].x =
            float_approach(stage_name_quads[0].x, NAME_LEFT_IN, NAME_STEP);
        stage_name_quads[2].x = stage_name_quads[3].x =
            float_approach(stage_name_quads[2].x, NAME_RIGHT_IN, NAME_STEP);
        stage_name_quads[4].x = stage_name_quads[5].x =
            float_approach(stage_name_quads[4].x, NAME_LEFT_IN, NAME_STEP);
        stage_name_quads[6].x = stage_name_quads[7].x =
            float_approach(stage_name_quads[6].x, NAME_RIGHT_IN, NAME_STEP);
        break;
    case NAME_HOLDING:
        if (work->timer++ > NAME_HOLD) {
            work->mode = NAME_WAITING;
            work->timer = 0;
            njReleaseTexture(&missiontex2_texlist);
            stage_end(STAGE_END_CARD_SHOWN);
        } else if (work->timer == NAME_MESSAGE_AT) {
            mission_message_start();
        }
        break;
    case NAME_WAITING:
        if (work->timer++ > NAME_WAIT) {
            work->mode = NAME_UPPER_OUT;
            work->timer = 0;
        }
        break;
    case NAME_UPPER_OUT:
        stage_name_quads[0].x = stage_name_quads[1].x =
            float_approach(stage_name_quads[0].x, NAME_LEFT_OUT, NAME_STEP);
        stage_name_quads[2].x = stage_name_quads[3].x =
            float_approach(stage_name_quads[2].x, NAME_RIGHT_OUT, NAME_STEP);
        if (work->timer++ >= NAME_UPPER_ALONE) {
            work->mode = NAME_SLIDE_OUT;
        }
        break;
    case NAME_SLIDE_OUT:
        stage_name_quads[0].x = stage_name_quads[1].x =
            float_approach(stage_name_quads[0].x, NAME_LEFT_OUT, NAME_STEP);
        stage_name_quads[2].x = stage_name_quads[3].x =
            float_approach(stage_name_quads[2].x, NAME_RIGHT_OUT, NAME_STEP);
        stage_name_quads[4].x = stage_name_quads[5].x =
            float_approach(stage_name_quads[4].x, NAME_LEFT_OUT, NAME_STEP);
        stage_name_quads[6].x = stage_name_quads[7].x =
            float_approach(stage_name_quads[6].x, NAME_RIGHT_OUT, NAME_STEP);
        if (stage_name_quads[4].x <= NAME_LEFT_OUT) {
            work->mode = NAME_ENDING;
            work->timer = 0;
        }
        break;
    case NAME_ENDING:
        if (work->timer++ > NAME_LINGER) {
            task_kill(task);
        }
        break;
    }
}

Bool title_card_start(void)
{
    char number[4];
    char name[20];
#ifdef NONMATCHING
    /* this side writes the digits into an array of its own, as C leaves a write
     * to a string literal undefined */
    char archive_name[] = "stg_title00";
    char *digits = archive_name + TITLE_NUMBER_AT;
#else
    char *digits = stage_title_name[0] + TITLE_NUMBER_AT;
#endif
    const char *code;

    switch (stage_current_id()) {
    case STAGE_GREEN_FOREST:
    case STAGE_WHITE_JUNGLE:
    case STAGE_PUMPKIN_HILL:
    case STAGE_SKY_RAIL:
    case STAGE_AQUATIC_MINE:
    case STAGE_SECURITY_HALL:
    case STAGE_PRISON_LANE:
    case STAGE_METAL_HARBOR:
    case STAGE_IRON_GATE:
    case STAGE_WEAPONS_BED:
    case STAGE_CITY_ESCAPE:
    case STAGE_RADICAL_HIGHWAY:
    case STAGE_WILD_CANYON:
    case STAGE_MISSION_STREET:
    case STAGE_DRY_LAGOON:
    case STAGE_SAND_OCEAN:
    case STAGE_CRAZY_GADGET:
    case STAGE_HIDDEN_BASE:
    case STAGE_ETERNAL_ENGINE:
    case STAGE_DEATH_CHAMBER:
    case STAGE_EGG_QUARTERS:
    case STAGE_LOST_COLONY:
    case STAGE_PYRAMID_CAVE:
    case STAGE_FINAL_RUSH:
    case STAGE_GREEN_HILL:
    case STAGE_METEOR_HERD:
    case STAGE_FINAL_CHASE:
    case STAGE_COSMIC_WALL:
    case STAGE_MAD_SPACE:
    case STAGE_CHAO:
        /* @hack redundant: the number printed into a buffer of its own and
         * copied, where sprintf into digits would do */
        sprintf(number, "%02d", stage_current_id());
        strcpy(digits, number);
        break;
    /* @hack spelling: a case of its own that copies "36", where the case
     * above would print the same */
    case STAGE_RELAY_FIRST:
        strcpy(digits, "36");
        break;
    case STAGE_CART_1:
        if (stage_character(0) == PLAYER_ROUGE) {
            strcpy(digits, "71");
        } else {
            strcpy(digits, "70");
        }
        break;
    default:
        return FALSE;
    }
#ifdef NONMATCHING
    /* this side names the PVM by its own copy of the name, the one the digits
     * went into */
    stage_title_texlist = texture_load_pvm_alloc(archive_name);
#else
    stage_title_texlist = texture_load_pvm_alloc(stage_title_name[0]);
#endif
    if (stage_title_texlist == NULL) {
        return FALSE;
    }
    if (task_create(TASK_WORK, NAME_LEVEL, stage_name_display_step,
                    "StageNameDisplayExecutor") == NULL) {
        njReleaseTexture(stage_title_texlist);
        syFree(stage_title_texlist);
        return FALSE;
    }
    sound_play(SOUND_TITLE_CARD, NULL, 0, 0);
    switch (stage_character(0)) {
    case PLAYER_SONIC:
        code = "SO";
        break;
    case PLAYER_SHADOW:
        code = "SH";
        break;
    case PLAYER_KNUCKLES:
        code = "KN";
        break;
    case PLAYER_ROUGE:
        code = "RO";
        break;
    /* @hack spelling: each mech's case before its character's, Eggman's here
     * and Tails's below, where the characters' order would do */
    case PLAYER_EGGMAN_MECH:
    case PLAYER_EGGMAN:
        code = "EG";
        break;
    case PLAYER_TAILS_MECH:
    case PLAYER_TAILS:
        if (stage_current_id() == STAGE_RELAY_TAILS) {
            code = "SO";
        } else {
            code = "TA";
        }
        break;
    default:
        code = "SO";
        break;
    }
    sprintf(name, "MISSIONTEX_%s", code);
    texture_load_pvm(name, &missiontex_texlist);
    sprintf(name, "MISSIONTEX_%s2", code);
    texture_load_pvm(name, &missiontex2_texlist);
    task_create(TASK_WORK, OVERLAY_LEVEL, stage_num_display_step,
                "StageNumDisplayExecutor");
    task_create(TASK_WORK, OVERLAY_LEVEL, dec0_display_step,
                "Dec0DisplayExecutor");
    task_create(TASK_WORK, OVERLAY_LEVEL, dec1_display_step,
                "Dec1DisplayExecutor");
    task_create(TASK_WORK, OVERLAY_LEVEL, adv_display_step,
                "AdvDisplayExecutor");
    task_create(TASK_WORK, OVERLAY_LEVEL, adv2_display_step,
                "Adv2DisplayExecutor");
    task_create(TASK_WORK, OVERLAY_LEVEL, blue_band_display_step,
                "BlueBandDisplayExecutor");
    mission_message_load();
    return TRUE;
}
