/* the side shutter, SHELTH in Iron Gate's object list. stg11/shutter.c is its
 * sibling, whose door drops. that source keeps the shutters' manager and the
 * stage's flags */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/flash.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stg11/shutter.h"
#include "stg11/shutter_panel.h"
#include "stg11/side_shutter.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "stg11/side_shutter/text.inc"

/* a side shutter's modes, in its work's mode. the placement's state keeps the
 * mode from one task to the next */
enum side_shutter_mode {
    MODE_OPEN,
    MODE_HALF,
    MODE_SHUTTING,
    MODE_SHUT,
    MODE_BROKEN
};

/* in the work's ang.x. its low byte, modulo SHUTTER_NUMBERS, is the shutter's
 * number, which picks its plate's texture and its voice. an open door stands
 * to the shutter's -x side, or under SHUTTER_REVERSED to its +x side */
#define SHUTTER_NUMBERS 5
#define NUMBER(work) ((Uint8)(work)->ang.x % SHUTTER_NUMBERS)
/* a placed side shutter numbered OPENING_NUMBER plays SHUTTER_OPENING_VOICE
 * as it starts to shut. the voice plays once a stage, as
 * SHUTTER_FLAG_OPENING_VOICE records */
#define OPENING_NUMBER 0

/* a side shutter's record, in task->motion. open is how far the door stands
 * open, 0 when shut. speed is open's change a step while the door bounces
 * shut */
struct side_shutter {
    Float open;
    Float speed;
};
#define SIDE_SHUTTER(task) ((struct side_shutter *)(task)->motion.p)

/* the shut door's timer counts its steps up to PANEL_COUNT_STEPS. the
 * shutter makes panel n on the step the timer reads n * PANEL_STEPS */
#define PANELS 4
#define PANEL_STEPS 6
#define PANEL_COUNT_STEPS 1000
/* in the shutter's work's submode: the panels hit, and SHUTTER_BROKE, which
 * the break sets and no code reads */
#define PANEL_HIT(n) (1 << (n))
#define SHUTTER_PANELS_HIT (PANEL_HIT(PANELS) - 1)
#define SHUTTER_BROKE 0x80
/* a panel slides in from PANEL_SLIDE_START, by PANEL_SLIDE_STEP a step. one
 * whose model lies above PANEL_UPPER slides down, and any other up */
#define PANEL_SLIDE_START 6.0F
#define PANEL_SLIDE_STEP 1.2F
#define PANEL_UPPER 30.0F
/* the panel's number, 0 to 3, in its work's tick */
#define PANEL(work) ((work)->tick)
/* while the door is shut, the lamps of the panels not yet hit blink. each is
 * lit for the first half of every BLINK_STEPS of game_state.steps. a lamp's
 * model has LAMP_VERTICES vertices */
#define BLINK_STEPS 60
#define LAMP_VERTICES 4
/* the plate shows texture PLATE_TEXTURE_FIRST plus the shutter's number */
#define PLATE_TEXTURE_FIRST 8

/* the half sizes of the stop, a box that keeps a walker's lock-on from passing
 * the door */
#define STOP_SIZE_X 130.0F
#define STOP_SIZE_Y 100.0F
#define STOP_SIZE_Z 1.0F

/* the common sounds the break plays */
#define BREAK_SOUND_1 0x1022
#define BREAK_SOUND_2 0x1011
#define BREAK_VOLUME 60
/* the voice a broken shutter plays, by its number. it plays once a stage,
 * as the shutter's SHUTTER_FLAG_BROKEN flag records */
#define BROKEN_VOICE(n) (SHUTTER_OPENING_VOICE + 1 + (n))
#define VOICE_PRIORITY 0
#define BREAK_POINTS 20

/* the shutting door's dust lies about the point SHUT_DUST_Y above the
 * shutter's pos */
#define SHUT_DUST_Y 25.0F
/* the door's fall as it breaks: its turn a step about x, and its speed */
#define BREAK_SPIN_X (-9)
#define BREAK_SPEED_Y 1.5F
#define BREAK_SPEED_Z (-3.0F)
#define BREAK_FLAGS                                                            \
    (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_NO_SPIN_Y |                  \
     DEBRIS_NO_SPIN_Z | DEBRIS_BIG_PARTICLES | DEBRIS_THROUGH_FLOOR)
/* the puff and the flash where a panel is hit. the puff starts at
 * (HIT_DUST_X, HIT_DUST_Y, HIT_DUST_Z) in the panel's frame. it flies along
 * the panel's z at HIT_DUST_SPEED */
#define HIT_DUST_SPEED 0.3F
#define HIT_UNUSED_SPEED 4.0F
#define HIT_DUST_X 0.0F
#define HIT_DUST_Y (-1.5F)
#define HIT_DUST_Z 4.0F
#define HIT_DUST_SCALE 6.0F
#define HIT_FLASH_SCALE 20.0F
#define HIT_SPARKS 3

#define SHUTTER_SHAPES 5
#define DOOR_SHAPE 1
#define PANEL_SHAPES 1

/* a panel and its lamp. the panel slides onto the door, where an attack can
 * hit it. the lamp is part of the door */
struct side_shutter_panel {
    NJS_CNK_OBJECT *panel;
    NJS_CNK_OBJECT *lamp;
};

/* the door, the plate, the panels and the lamps take side_shutter_texlist.
 * the frame is the shutter's, with a list of its own */
static NJS_TEXLIST side_shutter_texlist;
static NJS_CNK_OBJECT side_shutter_door_object;
static NJS_CNK_OBJECT shutter_plate_object;
static struct shutter_plate_polygon shutter_plate_polygons;
static NJS_CNK_OBJECT shutter_panel_0;
static NJS_CNK_OBJECT shutter_lamp_0;
static NJS_CNK_OBJECT shutter_panel_1;
static NJS_CNK_OBJECT shutter_lamp_1;
static NJS_CNK_OBJECT shutter_panel_2;
static NJS_CNK_OBJECT shutter_lamp_2;
static NJS_CNK_OBJECT shutter_panel_3;
static NJS_CNK_OBJECT shutter_lamp_3;

static struct side_shutter_panel side_shutter_panels[PANELS];
static struct collision_shape side_shutter_shapes[SHUTTER_SHAPES];
static Uint32 lamp_lit_color;
static Uint32 lamp_unlit_color;
static Uint32 door_see_through_color;
static struct collision_shape side_panel_shapes[PANEL_SHAPES];

static void side_shutter_destroy(struct task *task);
static void side_shutter_step(struct task *task);
static void side_shutter_draw(struct task *task);
static void side_panel_init(struct task *task);
static void side_panel_destroy(struct task *task);
static void side_panel_draw(struct task *task);
static void side_panel_step(struct task *task);

void side_shutter_init(struct task *task)
{
    struct task_work *work = task->work;

    shutter_manager_create();
    if (task->placement != NULL && placement_used(task)) {
        shutter_flag_set(SHUTTER_FLAG_BROKEN(NUMBER(work)));
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct side_shutter));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = side_shutter_draw;
    task->step = side_shutter_step;
    task->destructor = side_shutter_destroy;
    SIDE_SHUTTER(task)->open = SHUTTER_OPEN_FULL;
    collision_init(task, side_shutter_shapes, SHUTTER_SHAPES, COLLISION_OBJECT);
    if (task->placement != NULL) {
        work->mode = placement_get_state(task);
        if (placement_used(task)) {
            work->mode = MODE_BROKEN;
        }
        switch (work->mode) {
        case MODE_SHUTTING:
            SIDE_SHUTTER(task)->open = SHUTTER_OPEN_HALF;
            break;
        case MODE_SHUT:
        case MODE_BROKEN:
            SIDE_SHUTTER(task)->open = 0.0F;
            break;
        }
    }
}

static void side_shutter_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL) {
        placement_set_state(task, work->mode);
        if (work->mode == MODE_BROKEN) {
            placement_set_used(task);
        }
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void side_shutter_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, which stores 0 and
     * SHUTTER_NEAR_MARGIN in the record's speed */
    if (0) {
        SIDE_SHUTTER(task)->speed = 0.0F;
        SIDE_SHUTTER(task)->speed = SHUTTER_NEAR_MARGIN;
    }
    switch (work->mode) {
    case MODE_OPEN:
        /* @hack redundant: a block no path reaches, which stores 0 and
         * SHUTTER_NUMBERS in the timer */
        if (0) {
            work->timer = 0;
            work->timer = SHUTTER_NUMBERS;
        }
        if (player_near(&work->pos, work->scl.z + SHUTTER_NEAR_MARGIN)) {
            work->mode = MODE_HALF;
            work->timer = 0;
            sound_play(SHUTTER_START_SOUND, NULL, SHUTTER_SOUND_PRIORITY,
                       SHUTTER_SOUND_VOLUME);
            if (NUMBER(work) == OPENING_NUMBER && task->placement != NULL &&
                !shutter_flag_test(SHUTTER_FLAG_OPENING_VOICE)) {
                shutter_flag_set(SHUTTER_FLAG_OPENING_VOICE);
                voice_play(SHUTTER_OPENING_VOICE, VOICE_PRIORITY);
            }
        }
        break;
    case MODE_HALF:
        SIDE_SHUTTER(task)->open -= work->scl.x + SHUTTER_MIN_SPEED;
        if (!(SIDE_SHUTTER(task)->open > SHUTTER_OPEN_HALF)) {
            SIDE_SHUTTER(task)->open = SHUTTER_OPEN_HALF;
            work->timer++;
            if (work->timer > SHUTTER_WAIT_STEPS(work)) {
                sound_play(SHUTTER_HALF_SOUND, NULL, SHUTTER_SOUND_PRIORITY,
                           SHUTTER_SOUND_VOLUME);
                work->mode = MODE_SHUTTING;
            }
        }
        break;
    case MODE_SHUTTING: {
        NJS_POINT3 edge;
        struct angles ang;

        sound_play_for(SHUTTER_SHUTTING_SOUND, work, SHUTTER_SHUTTING_PRIORITY,
                       SHUTTER_SOUND_VOLUME, SHUTTER_SHUTTING_UPDATES);
        SIDE_SHUTTER(task)->open -= work->scl.y + SHUTTER_MIN_SPEED;
        if (!(SIDE_SHUTTER(task)->open > 0.0F)) {
            SIDE_SHUTTER(task)->open = 0.0F;
            work->mode = MODE_SHUT;
            work->timer = 0;
            /* @hack spelling: the edge's x picked by a test of SHUTTER_REVERSED
             * plus SHUT_DUST_Y, which always holds, where -open would do */
            edge.x = SHUTTER_REVERSED(work) + SHUT_DUST_Y
                         ? -SIDE_SHUTTER(task)->open
                         : SIDE_SHUTTER(task)->open;
            edge.y = SHUT_DUST_Y;
            edge.z = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &edge, &edge);
            njPopMatrixEx();
            /* dust_short_rect ignores ang.x, so this quarter turn tilts
             * nothing */
            ang.x = QUARTER_TURN;
            ang.y = work->ang.y;
            ang.z = 0;
            dust_short_rect(&edge, SHUTTER_DUST_HALF_X, SHUTTER_DUST_HALF_Z,
                            &ang, SHUTTER_DUST_SCALE, SHUTTER_DUST_DENSITY,
                            SHUTTER_DUST_SPEED);
            SIDE_SHUTTER(task)->speed += SHUTTER_SHUT_KICK;
        }
        break;
    }
    case MODE_SHUT: {
        struct angles ang;
        struct debris_spin spin;
        NJS_VECTOR spd;

        SIDE_SHUTTER(task)->open += SIDE_SHUTTER(task)->speed;
        SIDE_SHUTTER(task)->speed =
            -(SIDE_SHUTTER(task)->open * SHUTTER_BOUNCE_STIFFNESS) +
            (SIDE_SHUTTER(task)->speed * SHUTTER_BOUNCE_DAMPING);
        if (SIDE_SHUTTER(task)->open < 0.0F) {
            SIDE_SHUTTER(task)->open = 0.0F;
            SIDE_SHUTTER(task)->speed = njAbs(SIDE_SHUTTER(task)->speed);
        }
        if ((work->submode & SHUTTER_PANELS_HIT) == SHUTTER_PANELS_HIT) {
            ang.y = work->ang.y;
            ang.x = 0;
            ang.z = 0;
            spin.x = BREAK_SPIN_X;
            spin.y = 0;
            spin.z = 0;
            spd.x = 0.0F;
            spd.y = BREAK_SPEED_Y;
            spd.z = BREAK_SPEED_Z;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &spd, &spd);
            njPopMatrixEx();
            /* @bug logic: the break throws stg11/shutter.c's door as its
             * debris, shutter_door_object with shutter_texlist (0x8c50234c).
             * the draw draws the side shutter's own, side_shutter_door_object
             * with side_shutter_texlist (0x8c502720) */
            debris_launch_spin(&work->pos, &ang, work->pos.y, &spd, &spin,
                               shutter_door_object->model, &shutter_texlist,
                               BREAK_FLAGS);
            score_add(BREAK_POINTS);
            sound_play_at(BREAK_SOUND_1, NULL, SHUTTER_SOUND_PRIORITY,
                          BREAK_VOLUME, &work->pos);
            sound_play_at(BREAK_SOUND_2, NULL, SHUTTER_SOUND_PRIORITY,
                          BREAK_VOLUME, &work->pos);
            work->mode = MODE_BROKEN;
            work->submode |= SHUTTER_BROKE;
            if (task->placement != NULL &&
                !shutter_flag_test(SHUTTER_FLAG_BROKEN(NUMBER(work)))) {
                shutter_flag_set(SHUTTER_FLAG_BROKEN(NUMBER(work)));
                voice_play(BROKEN_VOICE(NUMBER(work)), VOICE_PRIORITY);
            }
        }
        if (work->timer < PANEL_COUNT_STEPS) {
            i = work->timer / PANEL_STEPS;
            if (i < PANELS && work->timer % PANEL_STEPS == 0) {
                struct task *panel =
                    task_create_child(TASK_WORK, side_panel_init, task);

                if (panel != NULL) {
                    PANEL(panel->work) = i;
                }
            }
            work->timer++;
        }
        break;
    }
    case MODE_BROKEN:
        break;
    /* @hack redundant: a default with nothing to do */
    default:
        break;
    }
    if (work->mode == MODE_SHUTTING || work->mode == MODE_SHUT) {
        /* @bug logic: a side shutter that its placement's state starts shut
         * gets no stop. its first step makes panel 0 (0x8c50242a), and this
         * test then finds the panel as task's child (0x8c502450) */
        if (task->child == NULL) {
            Angle ang[3];
            NJS_VECTOR size;

            size.x = STOP_SIZE_X;
            size.y = STOP_SIZE_Y;
            size.z = STOP_SIZE_Z;
            ang[0] = 0;
            ang[1] = work->ang.y - QUARTER_TURN;
            ang[2] = 0;
            /* @hack type: the angles' address cast from the array's, where
             * the array alone would do */
            lockon_box_create(task, (Angle *)&ang, &size);
        }
    } else if (task->child != NULL) {
        task_kill(task->child);
    }
    if (work->collision != NULL) {
        /* @hack spelling: the door's x picked by a test of its shape's
         * center.x plus SHUTTER_REVERSED, where center.x plus a pick by
         * SHUTTER_REVERSED would do. center.x is 0, so the two agree */
        work->collision->shapes[DOOR_SHAPE].center.x =
            side_shutter_shapes[DOOR_SHAPE].center.x + SHUTTER_REVERSED(work)
                ? SIDE_SHUTTER(task)->open
                : -SIDE_SHUTTER(task)->open;
        if (work->mode != MODE_BROKEN) {
            collision_register(task);
        }
    }
}

/* set the color of the first count vertices of the chunk at vlist. the chunk
 * must be NJD_CV_D8, with count vertices or more, unchecked */
static void side_vlist_set_colors(Uint32 *vlist, int count, Uint32 color)
{
    vlist += CHUNK_HEAD_WORDS + CHUNK_V_WORDS;
    while (count--) {
        *vlist = color;
        vlist += CHUNK_D8_WORDS;
    }
}

static void side_shutter_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 player;
    NJS_POINT3 pos;
    NJS_CNK_OBJECT *object;
    Bool blink;
    int i;
    Sint16 bit;
    Sint16 *texture;
    Sint16 filter;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- MODE[REV] : DOORNUM";
        (void)"<- MODE[   ] : DOORNUM";
        (void)"<- ANGLE Y";
        (void)"<- WAITTIME";
        (void)"<- SHUT SPEED1";
        (void)"<- SHUT SPEED2";
        (void)"<- START RANGE";
    }
    njCalcPoint(NULL, &player_work[camera_current_index]->pos, &player);
    njCalcPoint(NULL, &work->pos, &pos);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    object = &shutter_frame_object;
    njSetTexture(&shutter_frame_texlist);
    njCnkEasyDrawObject(object);
    njTranslate(NULL,
                SHUTTER_REVERSED(work) ? SIDE_SHUTTER(task)->open
                                       : -SIDE_SHUTTER(task)->open,
                0.0F, 0.0F);
    texture = &shutter_plate_polygons.texture.slot;
    filter = *texture & NJD_FFM_MASK;
    *texture = filter | (NUMBER(work) + PLATE_TEXTURE_FIRST);
    njSetTexture(&side_shutter_texlist);
    if (work->mode != MODE_BROKEN) {
        if (player.z > pos.z && !game_state.demo) {
            draw_control_save();
            draw_attr_save();
            draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
            njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA);
            object = &side_shutter_door_object;
            model_draw_tinted(object->model, door_see_through_color);
            draw_attr_restore();
            draw_control_restore();
        } else {
            object = &side_shutter_door_object;
            njCnkEasyDrawModel(object->model);
            object = &shutter_plate_object;
            njPushMatrixEx();
            njTranslateEx((NJS_VECTOR *)object->pos);
            njCnkEasyDrawModel(object->model);
            njPopMatrixEx();
            blink = game_state.steps % BLINK_STEPS < BLINK_STEPS / 2;
            for (i = 0; i < PANELS; i++) {
                bit = PANEL_HIT(i);
                object = side_shutter_panels[i].lamp;
                njPushMatrixEx();
                njTranslateEx((NJS_VECTOR *)object->pos);
                side_vlist_set_colors(
                    (Uint32 *)object->model->vlist, LAMP_VERTICES,
                    blink && !(work->submode & bit) && work->mode == MODE_SHUT
                        ? lamp_lit_color
                        : lamp_unlit_color);
                njCnkEasyDrawModel(object->model);
                njPopMatrixEx();
            }
        }
    }
    njPopMatrixEx();
}

static void side_panel_init(struct task *task)
{
    struct task_work *work = task->work;

    task->step = side_panel_step;
    /* @hack spelling: the destructor set before the draw, where struct task's
     * order would set the draw first */
    task->destructor = side_panel_destroy;
    task->draw = side_panel_draw;
    collision_init(task, side_panel_shapes, PANEL_SHAPES, COLLISION_ITEM);
    enemy_work_alloc(task);
    work->pos = task->parent->work->pos;
    work->ang = task->parent->work->ang;
    PANEL_SLIDE(task) = PANEL_SLIDE_START;
}

/* @hack redundant: task register, where a plain parameter would do */
static void side_panel_destroy(register struct task *task)
{
    enemy_destroy(task);
    /* force holds PANEL_SLIDE's Float, which task_destroy mustn't free */
    task->force.p = NULL;
}

/* @bug logic: a new panel draws once at its shutter's pos, before
 * side_panel_step places it. the panel's init runs in the walk of its
 * shutter's step, and gives it this draw. shutter_panel_draw waits for its
 * step's PANEL_PLACED (0x8c501ae0), and this draw tests nothing (0x8c502888) */
static void side_panel_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;
    Float side;
    int panel = PANEL(work);

    object = side_shutter_panels[panel].panel;
    if (object->pos[1] > PANEL_UPPER) {
        side = 1.0F;
    } else {
        side = -1.0F;
    }
    njSetTexture(&side_shutter_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, side * PANEL_SLIDE(task), 0.0F);
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}

static void side_panel_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *shutter = task->parent;
    struct task_work *shutter_work = shutter->work;
    NJS_CNK_OBJECT *object = side_shutter_panels[PANEL(work)].panel;
    NJS_VECTOR spd;
    NJS_VECTOR unused_spd;
    NJS_POINT3 pos;
    Angle ang[3];
    int player;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&shutter_work->pos);
    njRotateY(NULL, shutter_work->ang.y);
    /* @bug logic: places the panel as if its door weren't SHUTTER_REVERSED, so
     * a SHUTTER_REVERSED door's panels move against it while it bounces. the
     * draw picks the door's side by SHUTTER_REVERSED (0x8c5025f0), and this
     * step takes -open alone (0x8c5029bc) */
    njTranslate(NULL, -SIDE_SHUTTER(shutter)->open, 0.0F, 0.0F);
    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, &work->pos);
    njPopMatrixEx();
    PANEL_SLIDE(task) -= PANEL_SLIDE_STEP;
    if (PANEL_SLIDE(task) < 0.0F) {
        PANEL_SLIDE(task) = 0.0F;
    }
    if (PANEL_SLIDE(task) == 0.0F && (work->flags & WORK_HURT)) {
        shutter_work->submode |= PANEL_HIT(PANEL(work));
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = HIT_DUST_SPEED;
        unused_spd.x = 0.0F;
        unused_spd.y = 0.0F;
        unused_spd.z = HIT_UNUSED_SPEED;
        pos.x = HIT_DUST_X;
        pos.y = HIT_DUST_Y;
        pos.z = HIT_DUST_Z;
        /* @hack redundant: angles stored that nothing reads */
        ang[0] = 0;
        ang[1] = work->ang.y;
        ang[2] = 0;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcVector(NULL, &spd, &spd);
        /* @hack redundant: a speed turned that nothing reads */
        njCalcVector(NULL, &unused_spd, &unused_spd);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        dust_create(&pos, &spd, HIT_DUST_SCALE);
        flash_manager_create_sparks(pos.x, pos.y, pos.z, HIT_FLASH_SCALE,
                                    HIT_SPARKS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        player = player_nearest(&shutter_work->pos);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&shutter_work->pos);
        njRotateY(NULL, shutter_work->ang.y);
        njInvertMatrix(NULL);
        njCalcPoint(NULL, &player_work[player]->pos, &spd);
        njPopMatrixEx();
        if (spd.z > 0.0F) {
            collision_register(task);
        }
    }
}

#include "stg11/fixtures/shutter_materials.h"

static NJS_TEXLIST side_shutter_texlist = {
#include "stg11/side_shutter/side_shutter_texlist.inc"
};

#include "stg11/fixtures/shutter_panel_0.h"

#include "stg11/fixtures/shutter_panel_3.h"

#include "stg11/fixtures/shutter_panel_2.h"

#include "stg11/fixtures/shutter_panel_1.h"

#include "stg11/fixtures/shutter_lamp_3.h"

#include "stg11/fixtures/shutter_lamp_1.h"

#include "stg11/fixtures/shutter_lamp_2.h"

#include "stg11/fixtures/shutter_lamp_0.h"

#include "stg11/fixtures/shutter_plate.h"

static Sint16 side_shutter_door_polygons[925] = {
#include "stg11/side_shutter/side_shutter_door_polygons.inc"
};

static Sint32 side_shutter_door_vertices[811] = {
#include "stg11/side_shutter/side_shutter_door_vertices.inc"
};

static NJS_CNK_MODEL side_shutter_door_model = {
#include "stg11/side_shutter/side_shutter_door_model.inc"
};

static NJS_CNK_OBJECT side_shutter_door_object = {
#include "stg11/side_shutter/side_shutter_door_object.inc"
};

static struct side_shutter_panel side_shutter_panels[PANELS] = {
#include "stg11/side_shutter/side_shutter_panels.inc"
};
NJS_TEXLIST *side_shutter_texlists[5] = {
#include "stg11/side_shutter/side_shutter_texlists.inc"
};
/* the sill, the door, the two posts and the lintel. the step moves the
 * record's copy of the door's box with the door */
/* clang-format off */
static struct collision_shape side_shutter_shapes[SHUTTER_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -7.0F, 0.0F}, 35.0F, 5.5F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 25.0F, 0.0F}, 35.0F, 30.0F, 5.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {-37.0F, 25.0F, 0.0F}, 5.0F, 30.0F, 10.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 57.0F, 0.0F}, 35.0F, 7.5F, 10.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the lamps' colors, lit and unlit, and the door's see-through tint. outside
 * a demo, the door draws alone in the tint. it does so while the current
 * camera's player lies nearer the camera than the shutter's pos */
static Uint32 lamp_lit_color = 0xffffffff;
static Uint32 lamp_unlit_color = 0xff808080;
static Uint32 door_see_through_color = 0x65ffffff;
/* clang-format off */
static struct collision_shape side_panel_shapes[PANEL_SHAPES] = {
    {10, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_UNK_800000, {0.0F, 0.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
