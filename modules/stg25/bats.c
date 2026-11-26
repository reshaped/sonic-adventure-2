/* the bats, BAT in Death Chamber's object list: a swarm of bats that hang in
 * a line. a bat flies out over the swarm's pos once player 0 comes near. it
 * flies about there for a while, and then flies home */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "object/placement.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg25/bats.h"
#include "stg25/chamber_util.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"
#include "stg25/bats/text.inc"

/* the work's fields, which bats.h describes */
#define STAY_TURN(work) ((work)->ang.x & 0xff00)
#define STAY_RANGE(work) ((work)->ang.x & 0xff)
#define FLY_RANGE(work) ((work)->ang.y & 0xff00)
#define BAT_COUNT(work) ((Uint8)(work)->ang.y)
#define FLY_DISTANCE(work) ((work)->ang.z % 1000)

enum chamber_bat_mode { MODE_STAY, MODE_FLY_OUT, MODE_FLY, MODE_RETURN };
#define NO_NEXT_MODE (-1)

/* a bat of a swarm, in a list from the swarm task's extra */
struct bat {
    Sint32 mode;
    Sint32 next_mode; /* the mode it takes at its next step, or NO_NEXT_MODE */
    /* a random angle. each wave starts at phase times its rate. the wings'
     * beat runs phase steps ahead of game_state.steps */
    Angle phase;
    NJS_POINT3 pos;
    /* x and y turn the bat's z axis along its flight while it flies about.
     * they turn it against its flight while it flies out or home. z is its
     * bank */
    struct angles ang;
    Float scale;
    Angle spin; /* what ang.z gains each step it flies about or home */
    /* the part of each wave's gain a step that swing doesn't vary */
    Angle reach_rate;
    Angle tilt_rate;
    Angle turn_rate;
    /* a bat flying about lies away from the swarm's pos. the direction to it
     * tilts from straight up, and turns about y. the sines of these waves set
     * the distance, the tilt and the turn */
    Angle reach_wave;
    Angle tilt_wave;
    Angle turn_wave;
    /* an angle whose cosine speeds and slows each wave's gain */
    Angle swing;
    Sint32 timer; /* the steps it has flown about */
    NJS_POINT3 home; /* where it hangs */
    NJS_POINT3 goal; /* where it flies out to */
    struct bat *prev;
    struct bat *next;
    struct task *swarm;
};

/* a flying bat's wings beat through its FLIGHT_FRAMES models and back, a
 * frame a step. a beat takes BEAT_STEPS steps, a power of 2. a hanging bat
 * draws STAY_FRAME's model */
#define FLIGHT_FRAMES 5
#define STAY_FRAME FLIGHT_FRAMES
#define BEAT_STEPS 8

#define SCALE_BASE 1.75F
#define SCALE_SPREAD 0.25F
/* a bat flying about lies REACH_BASE of FLY_DISTANCE from the swarm's pos,
 * give or take REACH_SWING of it */
#define REACH_BASE 0.75F
#define REACH_SWING 0.25F
/* the turn wave gains only TURN_WAVE_SHARE of its rate plus swing's part */
#define TURN_WAVE_SHARE 0.25F
/* a hanging bat flies out with 1 chance in WAKE_CHANCE a step. it does so
 * only while player 0 lies within WAKE_REACH of its home, by |dx| + |dz|. it
 * drops DROP below its home as it sets off */
#define WAKE_REACH 50.0F
#define WAKE_CHANCE 30.0F
#define DROP 2.0F
/* past FLY_STEPS steps of flying about, a bat flies home with 1 chance in
 * RETURN_CHANCE a step. it does so only while player 0 lies RETURN_REACH or
 * more from its home, by |dx| + |dz| */
#define FLY_STEPS 420
#define RETURN_REACH 100.0F
#define RETURN_CHANCE 120.0F
/* a bat flying about eases its bank, ang.z, toward a target. the target is
 * BANK_SCALE times the turn from its new heading back to its last. each step
 * its spin gains SPIN_PULL of the gap, and loses SPIN_DRAG of itself. a bat
 * flying home eases its bank toward 0 the same way */
#define BANK_SCALE 16.0F
#define SPIN_PULL 0.003F
#define SPIN_DRAG 0.04F
#define CRY_SOUND 8
#define CRY_SOUND_PRIORITY 0

/* how far a bat flying out or home moves a step, with FLY_RANGE in eighths of a
 * turn */
#define FLIGHT_STEP(work)                                                      \
    (chamber_bat_flight_speed * FLY_DISTANCE(work) * (Float)FLY_RANGE(work) *  \
     (1.0F / EIGHTH_TURN))

static NJS_TEXNAME chamber_bat_stay_texture_names[1] = {
#include "stg25/bats/chamber_bat_stay_texture_names.inc"
};

static NJS_TEXLIST chamber_bat_stay_texlist = {
#include "stg25/bats/chamber_bat_stay_texlist.inc"
};

static Sint16 chamber_bat_stay_polygons[97] = {
#include "stg25/bats/chamber_bat_stay_polygons.inc"
};

static Sint32 chamber_bat_stay_vertices[63] = {
#include "stg25/bats/chamber_bat_stay_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_stay_model = {
#include "stg25/bats/chamber_bat_stay_model.inc"
};

static NJS_CNK_OBJECT chamber_bat_stay_object = {
#include "stg25/bats/chamber_bat_stay_object.inc"
};

static NJS_TEXNAME chamber_bat_texture_names_0[1] = {
#include "stg25/bats/chamber_bat_texture_names_0.inc"
};

static NJS_TEXLIST chamber_bat_texlist_0 = {
#include "stg25/bats/chamber_bat_texlist_0.inc"
};

static Sint16 chamber_bat_model_0_polygons[97] = {
#include "stg25/bats/chamber_bat_model_0_polygons.inc"
};

static Sint32 chamber_bat_model_0_vertices[63] = {
#include "stg25/bats/chamber_bat_model_0_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_model_0 = {
#include "stg25/bats/chamber_bat_model_0.inc"
};

static NJS_CNK_OBJECT chamber_bat_0_object = {
#include "stg25/bats/chamber_bat_0_object.inc"
};

static NJS_TEXNAME chamber_bat_texture_names_1[1] = {
#include "stg25/bats/chamber_bat_texture_names_1.inc"
};

static NJS_TEXLIST chamber_bat_texlist_1 = {
#include "stg25/bats/chamber_bat_texlist_1.inc"
};

static Sint16 chamber_bat_model_1_polygons[97] = {
#include "stg25/bats/chamber_bat_model_1_polygons.inc"
};

static Sint32 chamber_bat_model_1_vertices[63] = {
#include "stg25/bats/chamber_bat_model_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_model_1 = {
#include "stg25/bats/chamber_bat_model_1.inc"
};

static NJS_CNK_OBJECT chamber_bat_1_object = {
#include "stg25/bats/chamber_bat_1_object.inc"
};

static NJS_TEXNAME chamber_bat_wing_1_texture_names[1] = {
#include "stg25/bats/chamber_bat_wing_1_texture_names.inc"
};

static NJS_TEXLIST chamber_bat_wing_1_texlist = {
#include "stg25/bats/chamber_bat_wing_1_texlist.inc"
};

static Sint16 chamber_bat_wing_1_polygons[97] = {
#include "stg25/bats/chamber_bat_wing_1_polygons.inc"
};

static Sint32 chamber_bat_wing_1_vertices[63] = {
#include "stg25/bats/chamber_bat_wing_1_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_wing_1_model = {
#include "stg25/bats/chamber_bat_wing_1_model.inc"
};

static NJS_CNK_OBJECT chamber_bat_wing_1_object = {
#include "stg25/bats/chamber_bat_wing_1_object.inc"
};

static NJS_TEXNAME chamber_bat_texture_names_2[1] = {
#include "stg25/bats/chamber_bat_texture_names_2.inc"
};

static NJS_TEXLIST chamber_bat_texlist_2 = {
#include "stg25/bats/chamber_bat_texlist_2.inc"
};

static Sint16 chamber_bat_model_2_polygons[97] = {
#include "stg25/bats/chamber_bat_model_2_polygons.inc"
};

static Sint32 chamber_bat_model_2_vertices[63] = {
#include "stg25/bats/chamber_bat_model_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_model_2 = {
#include "stg25/bats/chamber_bat_model_2.inc"
};

static NJS_CNK_OBJECT chamber_bat_2_object = {
#include "stg25/bats/chamber_bat_2_object.inc"
};

static NJS_TEXNAME chamber_bat_wing_2_texture_names[1] = {
#include "stg25/bats/chamber_bat_wing_2_texture_names.inc"
};

static NJS_TEXLIST chamber_bat_wing_2_texlist = {
#include "stg25/bats/chamber_bat_wing_2_texlist.inc"
};

static Sint16 chamber_bat_wing_2_polygons[97] = {
#include "stg25/bats/chamber_bat_wing_2_polygons.inc"
};

static Sint32 chamber_bat_wing_2_vertices[63] = {
#include "stg25/bats/chamber_bat_wing_2_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_wing_2_model = {
#include "stg25/bats/chamber_bat_wing_2_model.inc"
};

static NJS_CNK_OBJECT chamber_bat_wing_2_object = {
#include "stg25/bats/chamber_bat_wing_2_object.inc"
};

static NJS_TEXNAME chamber_bat_texture_names_3[1] = {
#include "stg25/bats/chamber_bat_texture_names_3.inc"
};

static NJS_TEXLIST chamber_bat_texlist_3 = {
#include "stg25/bats/chamber_bat_texlist_3.inc"
};

static Sint16 chamber_bat_model_3_polygons[97] = {
#include "stg25/bats/chamber_bat_model_3_polygons.inc"
};

static Sint32 chamber_bat_model_3_vertices[63] = {
#include "stg25/bats/chamber_bat_model_3_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_model_3 = {
#include "stg25/bats/chamber_bat_model_3.inc"
};

static NJS_CNK_OBJECT chamber_bat_3_object = {
#include "stg25/bats/chamber_bat_3_object.inc"
};

static NJS_TEXNAME chamber_bat_texture_names_4[1] = {
#include "stg25/bats/chamber_bat_texture_names_4.inc"
};

static NJS_TEXLIST chamber_bat_texlist_4 = {
#include "stg25/bats/chamber_bat_texlist_4.inc"
};

static Sint16 chamber_bat_model_4_polygons[97] = {
#include "stg25/bats/chamber_bat_model_4_polygons.inc"
};

static Sint32 chamber_bat_model_4_vertices[63] = {
#include "stg25/bats/chamber_bat_model_4_vertices.inc"
};

static NJS_CNK_MODEL chamber_bat_model_4 = {
#include "stg25/bats/chamber_bat_model_4.inc"
};

static NJS_CNK_OBJECT chamber_bat_4_object = {
#include "stg25/bats/chamber_bat_4_object.inc"
};

/* SHC expands each call of chamber_bat_set_next_mode under #pragma inline.
 * the image also holds a copy of the function, which SHC writes only for an
 * external one. no header declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void chamber_bat_set_next_mode(struct bat *bat, Sint32 mode);
/* NOLINTEND(misc-use-internal-linkage) */

static void chamber_bats_destroy(struct task *task);
static void chamber_bats_step(struct task *task);
static void chamber_bats_draw(struct task *task);
static struct bat *chamber_bat_add(struct task *task);
static void chamber_bat_remove(struct bat *bat);
static void chamber_bat_step(struct bat *bat);
static void chamber_bat_draw(struct bat *bat);
static void chamber_bat_take_mode(struct bat *bat);
static void chamber_bat_stay(struct bat *bat);
static void chamber_bat_fly_out(struct bat *bat);
static void chamber_bat_fly(struct bat *bat);
static void chamber_bat_return(struct bat *bat);
static Bool chamber_bats_try_leave(struct task *task);

static NJS_CNK_MODEL *chamber_bat_models[FLIGHT_FRAMES + 1] = {
#include "stg25/bats/chamber_bat_models.inc"
};
NJS_TEXLIST *chamber_bat_texlists[7] = {
#include "stg25/bats/chamber_bat_texlists.inc"
};
static Float chamber_bat_flight_speed = 0.0333F;
static void (*chamber_bat_modes[])(struct bat *bat) = {
    chamber_bat_stay,
    chamber_bat_fly_out,
    chamber_bat_fly,
    chamber_bat_return,
};
/* the waves' tuning, in 65536ths of a turn. chamber_bat_add picks each rate
 * from its base up to its base plus its spread. the tilt and turn waves share
 * chamber_bat_angle_rate_spread, chamber_bat_angle_rate_base and
 * chamber_bat_angle_swing */
static Uint32 chamber_bat_angle_rate_spread = 64;
static Uint32 chamber_bat_angle_rate_base = 192;
static Uint32 chamber_bat_angle_swing = 64;
static Uint32 chamber_bat_reach_rate_spread = 32;
static Uint32 chamber_bat_reach_rate_base = 96;
static Uint32 chamber_bat_reach_swing = 32;
static Sint32 chamber_bat_swing_rate = 256;
/* a bat that isn't hanging cries with chance chamber_bat_cry_chance a step.
 * it does so only while player 0 lies within chamber_bat_cry_range of it */
static Float chamber_bat_cry_range = 150.0F;
static Sint32 chamber_bat_cry_volume = 55;
static Float chamber_bat_cry_chance = 0.001F;

void chamber_bats_init(struct task *task)
{
    int i;

    if (chamber_bats_try_leave(task)) {
        return;
    }
    task->draw = chamber_bats_draw;
    task->step = chamber_bats_step;
    task->destructor = chamber_bats_destroy;
    /* @hack spelling: task->work read again at each test of the loop, where a
     * local would do */
    for (i = 0; i < BAT_COUNT(task->work); i++) {
        chamber_bat_add(task);
    }
}

static void chamber_bats_destroy(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- STAY ANG Y/RANGE";
        (void)"<- FLY RANGE/NUM BATS";
        (void)"<- FLY DISTANCE(   )";
        (void)"<- STAY POSITION X";
        (void)"<- STAY POSITION Y";
        (void)"<- STAY POSITION Z";
    }
    while (task->extra.p) {
        chamber_bat_remove(task->extra.p);
    }
}

static void chamber_bats_step(struct task *task)
{
    /* @hack spelling: bat read from task->extra before the test, where a read
     * after it would do */
    struct bat *bat = task->extra.p;

    if (chamber_bats_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, the destructor's loop and
     * two stores, which gives the step the image's registers. the loop
     * after the else would do alone */
    if (0) {
        task->draw = NULL;
        task->destructor = NULL;
        while (task->extra.p) {
            chamber_bat_remove(task->extra.p);
        }
    } else {
        for (; bat; bat = bat->next) {
            chamber_bat_step(bat);
        }
    }
}

static void chamber_bats_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct bat *bat;
    NJS_POINT3 pos;
    Float scale;

    for (bat = task->extra.p; bat; bat = bat->next) {
        /* @hack redundant: a block no path reaches, four reads of task,
         * which keep task in the register the image gives it */
        if (0) {
            if (task->extra.p) {
            }
            if (task->work) {
            }
            if (task->placement) {
            }
            if (task->step) {
            }
        }
        chamber_bat_draw(bat);
    }
    /* @hack redundant: a block no path reaches, a draw that gives the draw
     * the image's frame and saved registers */
    if (0) {
        scale = 1.0F;
        pos = work->pos;
        njPushMatrixEx();
        njTranslateEx(&pos);
        chamber_model_draw_scaled(chamber_bat_models[STAY_FRAME], scale);
        njPopMatrixEx();
    }
}

/* add a bat to the front of task's list. its home lies at a random point of the
 * line the bats hang along. it returns the bat, or null where the heap can't
 * give one */
static struct bat *chamber_bat_add(struct task *task)
{
    /* @hack spelling: task->work read again for its fields, where work would
     * do */
    struct task_work *work = task->work;
    struct bat *bat;
    NJS_POINT3 offset;
    /* @hack spelling: line, a pointer to offset set before the modes, where
     * offset itself would do */
    NJS_POINT3 *line;
    Angle tilt;
    Angle turn;
    Float along;
    Float reach;

    /* @hack spelling: the assignment inside the test, where a statement of its
     * own would do */
    if ((bat = syMalloc(sizeof(struct bat))) == NULL) {
        return NULL;
    }
    bat->prev = NULL;
    bat->next = task->extra.p;
    if (bat->next) {
        bat->next->prev = bat;
    }
    bat->swarm = task;
    task->extra.p = bat;
    bat->scale = (RANDOM_UNIT() * SCALE_SPREAD) + SCALE_BASE;
    bat->reach_rate = (RANDOM_UNIT() * chamber_bat_reach_rate_spread) +
                      chamber_bat_reach_rate_base;
    bat->tilt_rate = (RANDOM_UNIT() * chamber_bat_angle_rate_spread) +
                     chamber_bat_angle_rate_base;
    bat->turn_rate = (RANDOM_UNIT() * chamber_bat_angle_rate_spread) +
                     chamber_bat_angle_rate_base;
    if (bat->tilt_rate & 1) {
        bat->tilt_rate = -bat->tilt_rate;
    }
    if (bat->turn_rate & 1) {
        bat->turn_rate = -bat->turn_rate;
    }
    /* @hack spelling: ang's parts cleared in the order z, y, x, where x, y, z
     * would do */
    bat->ang.z = 0;
    bat->ang.y = 0;
    bat->ang.x = 0;
    bat->spin = 0;
    bat->phase = RANDOM_UNIT() * (Float)FULL_TURN;
    bat->reach_wave = bat->phase * bat->reach_rate;
    bat->tilt_wave = bat->phase * bat->tilt_rate;
    bat->turn_wave = bat->phase * bat->turn_rate;
    bat->swing = RANDOM_UNIT() * (Float)FULL_TURN;
    line = &offset;
    bat->mode = MODE_STAY;
    bat->next_mode = NO_NEXT_MODE;
    chamber_polar_xz(STAY_TURN(task->work) + QUARTER_TURN,
                     STAY_RANGE(task->work), &line->x, &line->z);
    along = (RANDOM_UNIT() * 2.0F) - 1.0F;
    bat->home.x = task->work->scl.x + (along * line->x);
    bat->home.y = task->work->scl.y;
    bat->home.z = task->work->scl.z + (along * line->z);
#ifdef NONMATCHING
    /* this side repairs the tag in chamber_bat_draw: a new bat starts at its
     * home, where chamber_bat_stay puts it at its first step */
    bat->pos = bat->home;
#endif
    reach = (njSin(RANDOM_UNIT() * (Float)FULL_TURN) *
             FLY_DISTANCE(task->work) * REACH_SWING) +
            (FLY_DISTANCE(task->work) * REACH_BASE);
    tilt =
        njSin(RANDOM_UNIT() * (Float)FULL_TURN) * (Float)FLY_RANGE(task->work);
    turn =
        njSin(RANDOM_UNIT() * (Float)FULL_TURN) * (Float)FLY_RANGE(task->work);
    offset.x = offset.z = 0.0F;
    offset.y = reach;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, turn);
    njRotateX(NULL, tilt);
    njCalcPoint(NULL, &offset, &bat->goal);
    njPopMatrixEx();
    return bat;
}

/* take bat out of its swarm's list, and free it. bat may be null */
static void chamber_bat_remove(struct bat *bat)
{
    if (bat == NULL) {
        return;
    }
    if (bat->next) {
        bat->next->prev = bat->prev;
    }
    if (bat->prev) {
        bat->prev->next = bat->next;
    } else if (bat->swarm) {
        bat->swarm->extra.p = bat->next;
    }
    syFree(bat);
}

static void chamber_bat_step(struct bat *bat)
{
    chamber_bat_take_mode(bat);
    chamber_bat_modes[bat->mode](bat);
    if (bat->mode != MODE_STAY && chamber_bat_cry_chance > RANDOM_UNIT() &&
        chamber_bat_cry_range >
            njDistanceP2P(&player_work[0]->pos, &bat->pos)) {
        sound_play_at(CRY_SOUND, NULL, CRY_SOUND_PRIORITY,
                      chamber_bat_cry_volume, &bat->pos);
    }
}

static void chamber_bat_draw(struct bat *bat)
{
    struct task *swarm = bat->swarm;
    int beat;
    int frame;

    if (bat->mode == MODE_STAY) {
        njPushMatrixEx();
        /* @bug uninitialized: draws a bat that has had no step yet at the pos
         * syMalloc left it, on the frame the swarm starts (ADD at
         * 0x8c507e8a). chamber_bat_add stores no pos (0x8c507a62 to
         * 0x8c507dc0). chamber_bat_stay sets it to the home first on the next
         * step */
        njTranslateEx(&bat->pos);
        njRotateY(NULL, STAY_TURN(swarm->work));
        njSetTexture(chamber_bat_texlists[STAY_FRAME]);
        chamber_model_draw_scaled(chamber_bat_models[STAY_FRAME], bat->scale);
        njPopMatrixEx();
        return;
    }
    beat = (bat->phase + game_state.steps) & (BEAT_STEPS - 1);
    /* @hack type: the frame worked out in Float, where BEAT_STEPS - beat in
     * int would do */
    frame = (beat >= FLIGHT_FRAMES) ? _builtin_fabsf((Float)(beat - BEAT_STEPS))
                                    : (Float)beat;
    njPushMatrixEx();
    njTranslateEx(&bat->pos);
    njRotateY(NULL, bat->ang.y);
    njRotateX(NULL, bat->ang.x);
    njRotateZ(NULL, bat->ang.z);
    /* the model faces along its -z. while the bat flies about, ang turns its
     * z along the flight */
    if (bat->mode == MODE_FLY) {
        njRotateY(NULL, HALF_TURN);
    }
    njSetTexture(chamber_bat_texlists[frame]);
    chamber_model_draw_scaled(chamber_bat_models[frame], bat->scale);
    njPopMatrixEx();
}

static void chamber_bat_take_mode(struct bat *bat)
{
    if (bat->next_mode != NO_NEXT_MODE) {
        bat->mode = bat->next_mode;
        bat->next_mode = NO_NEXT_MODE;
    }
}

#pragma inline(chamber_bat_set_next_mode)
void chamber_bat_set_next_mode(struct bat *bat, Sint32 mode)
{
    bat->next_mode = mode;
}

static void chamber_bat_stay(struct bat *bat)
{
    NJS_POINT3 away;
    Float dist;

    bat->pos = bat->home;
    dist = _builtin_fabsf(player_work[0]->pos.x - bat->pos.x) +
           _builtin_fabsf(player_work[0]->pos.z - bat->pos.z);
    if (dist < WAKE_REACH && (int)(RANDOM_UNIT() * WAKE_CHANCE) == 0) {
        chamber_bat_set_next_mode(bat, MODE_FLY_OUT);
        /* @hack spelling: the drop added as its negative, where -= DROP would
         * do */
        bat->pos.y += -DROP;
        /* @hack spelling: away, a vector for the call's three parts, where the
         * differences passed in place would do */
        away.x = bat->pos.x - bat->goal.x;
        away.y = bat->pos.y - bat->goal.y;
        away.z = bat->pos.z - bat->goal.z;
        direction_angles_xy(away.x, away.y, away.z, &bat->ang.x, &bat->ang.y);
    }
}

static void chamber_bat_fly_out(struct bat *bat)
{
    struct task *swarm = bat->swarm;
    NJS_POINT3 stride;

    if (njDistanceP2P(&bat->pos, &bat->goal) <= FLIGHT_STEP(swarm->work)) {
        bat->pos = bat->goal;
        bat->timer = 0;
        chamber_bat_set_next_mode(bat, MODE_FLY);
    } else {
        stride.x = stride.y = 0.0F;
        stride.z = -FLIGHT_STEP(swarm->work);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&bat->pos);
        njRotateY(NULL, bat->ang.y);
        njRotateX(NULL, bat->ang.x);
        njCalcPoint(NULL, &stride, &bat->pos);
        njPopMatrixEx();
    }
}

static void chamber_bat_fly(struct bat *bat)
{
    struct task *swarm = bat->swarm;
    /* @hack spelling: swarm->work read again for FLY_DISTANCE and FLY_RANGE,
     * where work would do */
    struct task_work *work = swarm->work;
    NJS_POINT3 rise;
    NJS_POINT3 last;
    NJS_POINT3 away;
    Float reach;
    Angle tilt;
    Angle turn;
    Angle bank;
    Float dist;

    bat->swing += chamber_bat_swing_rate;
    bat->reach_wave +=
        bat->reach_rate + (njCos(bat->swing) * chamber_bat_reach_swing);
    reach = (njSin(bat->reach_wave) * FLY_DISTANCE(swarm->work) * REACH_SWING) +
            (FLY_DISTANCE(swarm->work) * REACH_BASE);
    bat->tilt_wave +=
        bat->tilt_rate + (njCos(bat->swing) * chamber_bat_angle_swing);
    bat->turn_wave +=
        (bat->turn_rate + (njCos(bat->swing) * chamber_bat_angle_swing)) *
        TURN_WAVE_SHARE;
    tilt = njSin(bat->tilt_wave) * (Float)FLY_RANGE(swarm->work);
    turn = njSin(bat->turn_wave) * (Float)FULL_TURN;
    rise.x = rise.z = 0.0F;
    rise.y = reach;
    last = bat->pos;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, turn);
    njRotateX(NULL, tilt);
    njCalcPoint(NULL, &rise, &bat->pos);
    njPopMatrixEx();
    bank = bat->ang.y;
    direction_angles_xy(bat->pos.x - last.x, bat->pos.y - last.y,
                        bat->pos.z - last.z, &bat->ang.x, &bat->ang.y);
    bank = (chamber_angle_turn(bat->ang.y, bank) * BANK_SCALE) - bat->ang.z;
    bat->spin += (bank * SPIN_PULL) - (bat->spin * SPIN_DRAG);
    bat->ang.z += bat->spin;
    dist = _builtin_fabsf(player_work[0]->pos.x - bat->home.x) +
           _builtin_fabsf(player_work[0]->pos.z - bat->home.z);
    bat->timer++;
    if (bat->timer > FLY_STEPS && dist >= RETURN_REACH &&
        (int)(RANDOM_UNIT() * RETURN_CHANCE) == 0) {
        chamber_bat_set_next_mode(bat, MODE_RETURN);
        away.x = bat->pos.x - bat->home.x;
        away.y = bat->pos.y - bat->home.y + DROP;
        away.z = bat->pos.z - bat->home.z;
        direction_angles_xy(away.x, away.y, away.z, &bat->ang.x, &bat->ang.y);
    }
}

static void chamber_bat_return(struct bat *bat)
{
    struct task *swarm = bat->swarm;
    NJS_POINT3 home;
    NJS_POINT3 stride;

    home = bat->home;
    /* @hack spelling: the drop added as its negative, where -= DROP would do */
    home.y += -DROP;
    if (njDistanceP2P(&bat->pos, &home) <= FLIGHT_STEP(swarm->work)) {
        bat->pos = home;
        chamber_bat_set_next_mode(bat, MODE_STAY);
    } else {
        stride.x = stride.y = 0.0F;
        stride.z = -FLIGHT_STEP(swarm->work);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&bat->pos);
        njRotateY(NULL, bat->ang.y);
        njRotateX(NULL, bat->ang.x);
        njCalcPoint(NULL, &stride, &bat->pos);
        njPopMatrixEx();
        bat->spin += (-bat->ang.z * SPIN_PULL) - (bat->spin * SPIN_DRAG);
        bat->ang.z += bat->spin;
    }
}

/* whether the swarm has left range, as chamber_bats_init defines it. once it
 * has, the call sets task's step to task_destroy */
static Bool chamber_bats_try_leave(struct task *task)
{
    /* @hack spelling: camera, read before work, where the reads in the
     * order of their use would do */
    struct task_work *camera = camera_task_work;
    struct task_work *work = task->work;
    Float dx;
    Float dy;
    Float dz;
    Float pos_dist_sq;
    Float stay_dist_sq;

    dx = work->pos.x - camera->pos.x;
    dy = work->pos.y - camera->pos.y;
    dz = work->pos.z - camera->pos.z;
    pos_dist_sq = (dx * dx) + (dy * dy) + (dz * dz);
    dx = work->scl.x - camera->pos.x;
    dy = work->scl.y - camera->pos.y;
    dz = work->scl.z - camera->pos.z;
    /* @hack redundant: a block no path reaches, a read of dx, which sorts
     * the registers of the first distance's parts as the image does */
    if (0) {
        if (dx) {
        }
    }
    stay_dist_sq = (dx * dx) + (dy * dy) + (dz * dz);
    if (pos_dist_sq > task->placement->leave.range +
                          (FLY_DISTANCE(work) * FLY_DISTANCE(work)) &&
        stay_dist_sq > task->placement->leave.range +
                           (STAY_RANGE(work) * STAY_RANGE(work))) {
        task->step = task_destroy;
        return TRUE;
    }
    /* @hack redundant: a block no path reaches, a call across which work
     * lives, which keeps work in the register the image gives it */
    if (0) {
        task_destroy(NULL);
        if (work) {
        }
    }
    return FALSE;
}
