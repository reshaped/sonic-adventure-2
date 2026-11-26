/* powder, waves and gunfire: specks, the two trails of a player's wave, and a
 * gun's flashes */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "effect/powder.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the level of each task here */
#define EFFECT_LEVEL 3

/* a speck starts within half of POS_SPREAD of pos along each axis. its speed
 * gains up to half of SPEED_SPREAD either way along x and z. along y it gains
 * up to SPEED_RISE upward */
#define POS_SPREAD 0.5F
#define SPEED_SPREAD 1.0F
#define SPEED_RISE 0.5F
#define POWDER_FALL 0.04F /* how much of world_gravity a step adds to spd */

/* a speck's steps and color, by its index mod SPECK_CYCLE. a speck at 0 or
 * 2 is brief, at 1 middling and at 3 lasting. Green Forest and White Jungle
 * take the GREEN_ colors. the UNUSED_ colors are a default arm's, which no
 * index reaches */
#define SPECK_CYCLE 4
#define BRIEF_STEPS 14
#define MIDDLING_STEPS 30
#define LASTING_STEPS 60
#define BRIEF_COLOR 0xffff9060
#define MIDDLING_COLOR 0xffff9070
#define LASTING_COLOR 0xffffa090
#define UNUSED_COLOR 0xff70b0ff
#define GREEN_BRIEF_COLOR 0xff046010
#define GREEN_MIDDLING_COLOR 0xff008010
#define GREEN_LASTING_COLOR 0xff003000
#define GREEN_UNUSED_COLOR 0xffff1050

/* a speck's record, in its task's extra */
struct powder {
    Sint8 unk_00; /* 0, which nothing reads */
    Sint8 player; /* the slot of player_work it ends with */
    Sint16 timer; /* the steps it has left */
    NJS_COLOR color;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
};

/* a trail's modes, in its record's mode. wave_create gives its first trail
 * WAVE_FOLLOWING_FIRST, and its second WAVE_FOLLOWING_SECOND. a following
 * trail's draws move its second line to the player. that line's root is the
 * record's unk_200 in the first trail, and its unk_20c in the second. a
 * closing trail's lines stay where they are */
#define WAVE_FOLLOWING_FIRST 0
#define WAVE_FOLLOWING_SECOND 1
#define WAVE_CLOSING 2

/* a trail's line runs from its root, a point of the player's record, to its
 * tip. the tip is placed WAVE_REACH of the way from the root toward the
 * record's unk_230. a closing trail's step pulls each tip WAVE_REACH of the
 * way toward its root */
#define WAVE_REACH 0.3F
#define WAVE_KEEP (1.0F - WAVE_REACH)
#define WAVE_COLOR 0xff908080
#define WAVE_FADE 0x1f000000 /* what each step takes from the color */
#define WAVE_STEPS 8 /* the step that takes the timer to WAVE_STEPS ends it */
/* a trail's draws add the model while its timer is WAVE_MODEL_TIMER, between
 * its first step and its second. the model is one of three, in a cycle of
 * WAVE_MODEL_CYCLE steps */
#define WAVE_MODEL_TIMER 1
#define WAVE_MODEL_CYCLE 4
/* the model's scale along y is WAVE_SCALE_SUM less its scale along x and z */
#define WAVE_SCALE_MIN 0.7F
#define WAVE_SCALE_SPAN 0.5F
#define WAVE_SCALE_SUM 2.0F
#define WAVE_POINTS 4

/* a trail's record, in its task's extra. points holds the strip's corners, two
 * lines of a root and a tip each. the first line is where the trail started,
 * and the second where the player is */
struct wave {
    Sint8 mode; /* WAVE_FOLLOWING_FIRST to WAVE_CLOSING */
    Sint8 player; /* the slot of player_records it follows */
    Sint16 timer; /* the steps it has taken */
    Uint32 color; /* the strip's color, as an ARGB word */
    Float scale; /* the model's scale along x and z */
    NJS_POINT3 points[WAVE_POINTS];
};

/* a flash's look, in its work's submode. an aimed flash takes one of the
 * first GUNFIRE_LOOKS looks. they alternate gunfire_object_a and
 * gunfire_object_b by their low bit. the look after them, GUNFIRE_UNAIMED,
 * draws gunfire_object_c */
#define GUNFIRE_LOOKS 4
#define GUNFIRE_UNAIMED GUNFIRE_LOOKS
/* a flash is killed at the step that finds its timer above GUNFIRE_LAST */
#define GUNFIRE_LAST 1
#define GUNFIRE_BRIEF_TIMER 2 /* the timer a brief flash starts at */
#define GUNFIRE_SCALE_MIN 0.7F
#define GUNFIRE_SCALE_SPAN 0.5F
#define GUNFIRE_BRIEF_SCALE_MIN 0.5F
#define GUNFIRE_BRIEF_SCALE_SPAN 0.4F

static void powder_destroy(struct task *task);
static void powder_step(struct task *task);
static void powder_draw(struct task *task);
static void wave_destroy(struct task *task);
static void wave_step(struct task *task);
static void wave_draw(struct task *task);

/* the strip each trail's draw fills with its points and its color */
static NJS_POLYGON_VTX wave_strip[WAVE_POINTS] = {
#include "effect/powder/wave_strip.inc"
};

void powder_create(int player, int count, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    struct task *task;
    struct powder *powder;
    int i;

    /* @hack redundant: the test of count ahead of the loop, whose own test
     * does the same */
    if (count <= 0) {
        return;
    }
    for (i = 0; i < count; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = task_create(0, EFFECT_LEVEL, powder_step,
                                "PowderExecute")) == NULL) {
            break;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((powder = syCalloc(1, sizeof(struct powder))) == NULL) {
            task_kill(task);
            break;
        }
        powder->player = player;
        powder->pos = *pos;
        powder->pos.x += (0.5F - (rand() * RAND_SCALE)) * POS_SPREAD;
        powder->pos.y += (0.5F - (rand() * RAND_SCALE)) * POS_SPREAD;
        powder->pos.z += (0.5F - (rand() * RAND_SCALE)) * POS_SPREAD;
        powder->spd = *spd;
        powder->spd.x += (0.5F - (rand() * RAND_SCALE)) * SPEED_SPREAD;
        powder->spd.y += rand() * RAND_SCALE * SPEED_RISE;
        powder->spd.z += (0.5F - (rand() * RAND_SCALE)) * SPEED_SPREAD;
        if (stage_current_id() != STAGE_GREEN_FOREST &&
            stage_current_id() != STAGE_WHITE_JUNGLE) {
            switch (i & (SPECK_CYCLE - 1)) {
            case 0:
            case 2:
                powder->color.color = BRIEF_COLOR;
                powder->timer = BRIEF_STEPS;
                break;
            case 1:
                powder->color.color = MIDDLING_COLOR;
                powder->timer = MIDDLING_STEPS;
                break;
            case 3:
                powder->color.color = LASTING_COLOR;
                powder->timer = LASTING_STEPS;
                break;
            /* @hack redundant: a default arm that no index reaches */
            default:
                powder->color.color = UNUSED_COLOR;
                powder->timer = LASTING_STEPS;
                break;
            }
        } else {
            switch (i & (SPECK_CYCLE - 1)) {
            case 0:
            case 2:
                powder->color.color = GREEN_BRIEF_COLOR;
                powder->timer = BRIEF_STEPS;
                break;
            case 1:
                powder->color.color = GREEN_MIDDLING_COLOR;
                powder->timer = MIDDLING_STEPS;
                break;
            case 3:
                powder->color.color = GREEN_LASTING_COLOR;
                powder->timer = LASTING_STEPS;
                break;
            /* @hack redundant: a default arm that no index reaches */
            default:
                powder->color.color = GREEN_UNUSED_COLOR;
                powder->timer = LASTING_STEPS;
                break;
            }
        }
        task->extra.p = powder;
        task->draw = powder_draw;
        task->destructor = powder_destroy;
    }
}

static void powder_destroy(struct task *task)
{
}

static void powder_step(struct task *task)
{
    struct powder *powder = task->extra.p;
    NJS_VECTOR fall;

    if (player_work[powder->player] == NULL) {
        task_kill(task);
        return;
    }
    if (powder->timer-- <= 0) {
        task_kill(task);
        return;
    }
    njAddVector(&powder->pos, &powder->spd);
    fall = world_gravity;
    fall.x *= POWDER_FALL;
    fall.y *= POWDER_FALL;
    fall.z *= POWDER_FALL;
    njAddVector(&powder->spd, &fall);
}

static void powder_draw(struct task *task)
{
    struct powder *powder = task->extra.p;

    line_queue(&powder->pos, &powder->color, POINT_OPAQUE);
}

void wave_create(int player)
{
    struct player *record = player_records[player];
    struct task *task;
    struct wave *wave;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(0, EFFECT_LEVEL, wave_step, "WaveExecute")) ==
        NULL) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((wave = syCalloc(1, sizeof(struct wave))) == NULL) {
        task_kill(task);
        return;
    }
    wave->player = player;
    wave->points[0] = record->unk_200;
    wave->points[1].x =
        (record->unk_200.x * WAVE_KEEP) + (record->unk_230.x * WAVE_REACH);
    wave->points[1].y =
        (record->unk_200.y * WAVE_KEEP) + (record->unk_230.y * WAVE_REACH);
    wave->points[1].z =
        (record->unk_200.z * WAVE_KEEP) + (record->unk_230.z * WAVE_REACH);
    wave->mode = WAVE_FOLLOWING_FIRST;
    wave->color = WAVE_COLOR;
    wave->scale = (rand() * (WAVE_SCALE_SPAN * RAND_SCALE)) + WAVE_SCALE_MIN;
    task->extra.p = wave;
    task->draw = wave_draw;
    task->destructor = wave_destroy;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(0, EFFECT_LEVEL, wave_step, "WaveExecute")) ==
        NULL) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((wave = syCalloc(1, sizeof(struct wave))) == NULL) {
        task_kill(task);
        return;
    }
    wave->player = player;
    wave->points[0] = record->unk_20c;
    wave->points[1].x =
        (record->unk_20c.x * WAVE_KEEP) + (record->unk_230.x * WAVE_REACH);
    wave->points[1].y =
        (record->unk_20c.y * WAVE_KEEP) + (record->unk_230.y * WAVE_REACH);
    wave->points[1].z =
        (record->unk_20c.z * WAVE_KEEP) + (record->unk_230.z * WAVE_REACH);
    wave->mode = WAVE_FOLLOWING_SECOND;
    wave->color = WAVE_COLOR;
    wave->scale = (rand() * (WAVE_SCALE_SPAN * RAND_SCALE)) + WAVE_SCALE_MIN;
    task->extra.p = wave;
    task->draw = wave_draw;
    task->destructor = wave_destroy;
}

static void wave_destroy(struct task *task)
{
}

static void wave_step(struct task *task)
{
    struct wave *wave = task->extra.p;

    if (player_records[wave->player] == NULL) {
        task_kill(task);
        return;
    }
    wave->timer++;
    if (wave->timer >= WAVE_STEPS) {
        task_kill(task);
        return;
    }
    wave->color -= WAVE_FADE;
    if (wave->mode == WAVE_CLOSING) {
        wave->points[1].x =
            (wave->points[1].x * WAVE_KEEP) + (wave->points[0].x * WAVE_REACH);
        wave->points[1].y =
            (wave->points[1].y * WAVE_KEEP) + (wave->points[0].y * WAVE_REACH);
        wave->points[1].z =
            (wave->points[1].z * WAVE_KEEP) + (wave->points[0].z * WAVE_REACH);
        wave->points[3].x =
            (wave->points[3].x * WAVE_KEEP) + (wave->points[2].x * WAVE_REACH);
        wave->points[3].y =
            (wave->points[3].y * WAVE_KEEP) + (wave->points[2].y * WAVE_REACH);
        wave->points[3].z =
            (wave->points[3].z * WAVE_KEEP) + (wave->points[2].z * WAVE_REACH);
    }
}

/* in a split screen each camera draws the trail. a following trail closes
 * only at the draw for its player's camera or a later one. every camera thus
 * sees its second line at the player */
static void wave_draw(struct task *task)
{
    struct wave *wave = task->extra.p;
    struct player *record = player_records[wave->player];
    struct task_work *work;
    NJS_CNK_OBJECT *object;
    int i;

    if (record == NULL) {
        return;
    }
    if (wave->mode == WAVE_FOLLOWING_FIRST) {
        wave->points[2] = record->unk_200;
        wave->points[3].x =
            (record->unk_200.x * WAVE_KEEP) + (record->unk_230.x * WAVE_REACH);
        wave->points[3].y =
            (record->unk_200.y * WAVE_KEEP) + (record->unk_230.y * WAVE_REACH);
        wave->points[3].z =
            (record->unk_200.z * WAVE_KEEP) + (record->unk_230.z * WAVE_REACH);
        if (game_state.split_screen != 1 || game_state.single_view != 0 ||
            camera_current_index >= wave->player) {
            wave->mode = WAVE_CLOSING;
        }
    } else if (wave->mode == WAVE_FOLLOWING_SECOND) {
        wave->points[2] = record->unk_20c;
        wave->points[3].x =
            (record->unk_20c.x * WAVE_KEEP) + (record->unk_230.x * WAVE_REACH);
        wave->points[3].y =
            (record->unk_20c.y * WAVE_KEEP) + (record->unk_230.y * WAVE_REACH);
        wave->points[3].z =
            (record->unk_20c.z * WAVE_KEEP) + (record->unk_230.z * WAVE_REACH);
        if (game_state.split_screen != 1 || game_state.single_view != 0 ||
            camera_current_index >= wave->player) {
            wave->mode = WAVE_CLOSING;
        }
    }
    for (i = 0; i < WAVE_POINTS; i++) {
        wave_strip[i].x = wave->points[i].x;
        wave_strip[i].y = wave->points[i].y;
        wave_strip[i].z = wave->points[i].z;
        wave_strip[i].col = wave->color;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njDrawPolygon3DEx(wave_strip, WAVE_POINTS, 1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    if (wave->timer != WAVE_MODEL_TIMER) {
        return;
    }
    if (stage_current_id() == STAGE_GREEN_FOREST) {
        return;
    }
    if (stage_current_id() == STAGE_WHITE_JUNGLE) {
        return;
    }
    work = player_work[wave->player];
    njPushMatrixEx();
    njTranslateEx(&wave->points[2]);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    if (record->spd.x < 0.0F) {
        njRotateY(NULL, -work->ang.y);
    } else {
        njRotateY(NULL, HALF_TURN - work->ang.y);
    }
    switch (game_state.steps & (WAVE_MODEL_CYCLE - 1)) {
    case 0:
    case 2:
        object = &wave_object_a;
        break;
    case 1:
        object = &wave_object_b;
        break;
    case 3:
    default:
        object = &wave_object_c;
        break;
    }
    njScale(NULL, wave->scale, WAVE_SCALE_SUM - wave->scale, wave->scale);
    njCnkEasyDrawObject(object);
    njPopMatrixEx();
}

static void gunfire_destroy(struct task *task)
{
}

static void gunfire_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->timer++ > GUNFIRE_LAST) {
        task_kill(task);
    }
}

static void gunfire_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    njFogDisable();
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: an arm of its own for 1, for 3 and for the default,
     * where one arm would do */
    switch (work->submode) {
    case 0:
    case 2:
        object = &gunfire_object_a;
        break;
    case 1:
        object = gunfire_object_b;
        break;
    case 3:
        object = gunfire_object_b;
        break;
    case GUNFIRE_UNAIMED:
        object = &gunfire_object_c;
        break;
    default:
        object = gunfire_object_b;
        break;
    }
    njScaleEx(&work->scl);
    njCnkEasyDrawObject(object);
    njPopMatrixEx();
    njFogEnable();
}

void gunfire_create(NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    struct task *task;
    struct task_work *work;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, gunfire_step,
                            "GunfireExecute")) != NULL) {
        work = task->work;
        work->pos = *pos;
        work->scl.x =
            (rand() * (GUNFIRE_SCALE_SPAN * RAND_SCALE)) + GUNFIRE_SCALE_MIN;
        work->scl.y =
            (rand() * (GUNFIRE_SCALE_SPAN * RAND_SCALE)) + GUNFIRE_SCALE_MIN;
        work->scl.z =
            (rand() * (GUNFIRE_SCALE_SPAN * RAND_SCALE)) + GUNFIRE_SCALE_MIN;
        work->ang.y = (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN);
        work->ang.x = (Angle)(asinf(dir->y / njScalor(dir)) * ANGLE_PER_RADIAN);
        work->submode =
            (game_state.steps + (int)(rand() * (GUNFIRE_LOOKS * RAND_SCALE))) &
            (GUNFIRE_LOOKS - 1);
        task->draw = gunfire_draw;
        task->destructor = gunfire_destroy;
    }
}

void gunfire_create_brief(NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    struct task *task;
    struct task_work *work;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, gunfire_step,
                            "GunfireExecute")) != NULL) {
        work = task->work;
        work->pos = *pos;
        work->scl.x = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->scl.y = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->scl.z = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->ang.y = (Angle)(atan2f(dir->x, dir->z) * ANGLE_PER_RADIAN);
        work->ang.x = (Angle)(asinf(dir->y / njScalor(dir)) * ANGLE_PER_RADIAN);
        work->submode =
            (game_state.steps + (int)(rand() * (GUNFIRE_LOOKS * RAND_SCALE))) &
            (GUNFIRE_LOOKS - 1);
        work->timer = GUNFIRE_BRIEF_TIMER;
        task->draw = gunfire_draw;
        task->destructor = gunfire_destroy;
    }
}

void gunfire_create_unaimed(NJS_POINT3 *pos)
{
    struct task *task;
    struct task_work *work;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, EFFECT_LEVEL, gunfire_step,
                            "GunfireExecute")) != NULL) {
        work = task->work;
        work->pos = *pos;
        work->scl.x = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->scl.y = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->scl.z = (rand() * (GUNFIRE_BRIEF_SCALE_SPAN * RAND_SCALE)) +
                      GUNFIRE_BRIEF_SCALE_MIN;
        work->ang.y = RANDOM_TURN();
        work->ang.x = rand() * (HALF_TURN * RAND_SCALE);
        work->submode = GUNFIRE_UNAIMED;
        work->timer = GUNFIRE_BRIEF_TIMER;
        task->draw = gunfire_draw;
        task->destructor = gunfire_destroy;
    }
}
