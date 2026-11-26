/* the lanes, SLANE in Radical Highway's object list: stretches of road the
 * players ride. the quakes, SCOL there, set them off. once a player stands in
 * a quake, the lanes of its id crack and fall, tilt or swing. the quake shakes
 * the screen while they move. it is shared/lane.c without SLANE2, with 16
 * ids, other sounds, and steps that stop with time */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "effect/burst.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg14/lane.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "stg14/lane/text.inc"

/* one sine of a shake. at the shake's step n, it adds amp * sin(n * turn) */
struct quake_wave {
    Angle turn;
    Float amp;
};

/* a lane's record, which task->motion holds */
struct lane {
    NJS_OBJECT object; /* the lane's copy of highway_lane_world */
    Float fall_spd; /* along y */
    /* swing as an Angle, negated under LANE_REVERSE. the draw turns the lane
     * by it about z for LANE_TYPE_D, and about x for the others */
    Angle turn;
    Float alpha; /* 1 until the lane fades, then falling to 0 */
    Float shake; /* the height the lane's shake adds to pos.y */
    Float swing_spd; /* degrees a step */
    Float swing; /* the swing's or the tilt's angle, in degrees */
};

/* Intact and cracked roadway with six debris origins and the solid road
 * surface. */

static NJS_TEXNAME highway_lane_textures[7] = {
#include "stg14/lane/highway_lane_textures.inc"
};

static NJS_TEXLIST highway_lane_texlist = {
#include "stg14/lane/highway_lane_texlist.inc"
};

static Sint16 highway_lane_polygons[287] = {
#include "stg14/lane/highway_lane_polygons.inc"
};

static Sint32 highway_lane_vertices[203] = {
#include "stg14/lane/highway_lane_vertices.inc"
};

static NJS_CNK_MODEL highway_lane_mesh = {
#include "stg14/lane/highway_lane_mesh.inc"
};

static NJS_CNK_OBJECT highway_lane_object = {
#include "stg14/lane/highway_lane_object.inc"
};

static NJS_TEXNAME highway_lane_cracked_textures[7] = {
#include "stg14/lane/highway_lane_cracked_textures.inc"
};

static NJS_TEXLIST highway_lane_cracked_texlist = {
#include "stg14/lane/highway_lane_cracked_texlist.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_6 = {
#include "stg14/lane/highway_lane_debris_origin_6.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_5 = {
#include "stg14/lane/highway_lane_debris_origin_5.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_4 = {
#include "stg14/lane/highway_lane_debris_origin_4.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_3 = {
#include "stg14/lane/highway_lane_debris_origin_3.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_2 = {
#include "stg14/lane/highway_lane_debris_origin_2.inc"
};

static NJS_CNK_OBJECT highway_lane_debris_origin_1 = {
#include "stg14/lane/highway_lane_debris_origin_1.inc"
};

static Sint16 highway_lane_cracked_polygons[733] = {
#include "stg14/lane/highway_lane_cracked_polygons.inc"
};

static Sint32 highway_lane_cracked_vertices[595] = {
#include "stg14/lane/highway_lane_cracked_vertices.inc"
};

static NJS_CNK_MODEL highway_lane_cracked_mesh = {
#include "stg14/lane/highway_lane_cracked_mesh.inc"
};

static NJS_CNK_OBJECT highway_lane_cracked_object = {
#include "stg14/lane/highway_lane_cracked_object.inc"
};

static NJS_MATERIAL highway_lane_world_materials[1] = {
#include "stg14/lane/highway_lane_world_materials.inc"
};

static Sint16 highway_lane_world_indices_0[41] = {
#include "stg14/lane/highway_lane_world_indices_0.inc"
};

static NJS_MESHSET highway_lane_world_meshsets[1] = {
#include "stg14/lane/highway_lane_world_meshsets.inc"
};

static NJS_POINT3 highway_lane_world_points[16] = {
#include "stg14/lane/highway_lane_world_points.inc"
};

static NJS_VECTOR highway_lane_world_normals[16] = {
#include "stg14/lane/highway_lane_world_normals.inc"
};

static NJS_MODEL highway_lane_world_model = {
#include "stg14/lane/highway_lane_world_model.inc"
};

static NJS_OBJECT highway_lane_world = {
#include "stg14/lane/highway_lane_world.inc"
};

static Sint32 unused_lane_values[] = {80, 30};

static void quake_destroy(struct task *task);
static void quake_step(struct task *task);
static void quake_draw(struct task *task);
static void lane_destroy(struct task *task);
static void lane_step(struct task *task);
static void lane_draw(struct task *task);

/* a quake's or a lane's id is the low four bits of its ang.x. a quake lists
 * itself in quake_tasks at its id. a lane follows the quake listed at its
 * own */
#define QUAKE_SLOTS 16
#define QUAKE_ID(work) ((work)->ang.x & (QUAKE_SLOTS - 1))
/* a player within scl.x plus QUAKE_MARGIN of a quake's pos stands in it */
#define QUAKE_MARGIN 10.0F
/* a quake's tick counts down the steps its shake has left. a quake step that
 * finds QUAKE_HIT set puts it back to SHAKE_STEPS. the quake's timer counts
 * the shake's steps, and goes back to 0 once the shake ends. the shake sums
 * QUAKE_WAVES sines along each axis, and shrinks with the tick */
#define SHAKE_CAMERA 0
#define SHAKE_STEPS 60
#define QUAKE_WAVES 4
/* the rumble a quake starts with its shake, as vibration_start takes it */
#define RUMBLE_POWER 3
#define RUMBLE_FREQ 15
#define RUMBLE_TIME 60
/* the rumble's time and the shake's steps a quake would start as a player
 * enters it. neither starts: quake_step's test of NO_TYPE never passes */
#define ENTER_RUMBLE_TIME 30
#define ENTER_SHAKE_STEPS 30
/* a shaking quake plays RUMBLE_SOUND with a chance of RUMBLE_SOUND_CHANCE
 * each step. the quake's and the lanes' sounds play at SOUND_PRIORITY and
 * SOUND_VOLUME, as sound_play takes them */
#define RUMBLE_SOUND 1
#define RUMBLE_SOUND_CHANCE 0.08F
#define SOUND_PRIORITY 0
#define SOUND_VOLUME 0
/* rand() * RAND_TURN is a random Angle less than a full turn */
#define RAND_TURN 2.0F
/* a lane's type is its ang.z masked by TYPE_BITS, the low four bits, modulo
 * LANE_TYPES. A, B and E crack and fall. C tilts, D swings and F stays.
 * lane_type_names holds the object's own names for them */
enum lane_type {
    LANE_TYPE_A,
    LANE_TYPE_B,
    LANE_TYPE_C,
    LANE_TYPE_D,
    LANE_TYPE_E,
    LANE_TYPE_F,
    LANE_TYPES
};
#define NO_TYPE LANE_TYPES /* past every type, so LANE_TYPE never gives it */
#define TYPE_BITS 0xf
#define LANE_TYPE(work) (((work)->ang.z & TYPE_BITS) % LANE_TYPES)
/* a lane's work's mode. a lane waits until its quake turns on. a cracking
 * lane then falls, and lands or breaks and fades. a tilting lane comes to rest
 * tilted. a swinging lane settles and waits again once its quake turns off */
enum lane_mode {
    MODE_WAIT,
    MODE_CRACK,
    MODE_FALL,
    MODE_TILTED,
    MODE_FADE,
    MODE_FALLEN,
    MODE_TILT,
    MODE_SWING
};
/* a quake's work's mode. its submode is player_near's result for the last
 * player to stand in it, or 0 before any has. the rumbles of the quake and its
 * lanes go to that player. player_near gives NEAR_SLOT_0 for slot 0 */
enum quake_mode {
    MODE_QUAKE_OFF,
    MODE_QUAKE_ON /* while a player stands in it */
};
#define NEAR_SLOT_0 1

/* a quake's task->unk_3c, which keeps it shaking. the quake's step clears
 * it. a cracking, falling or swinging lane sets it each step. so does a
 * tilting lane, until it first passes TILT_LIMIT */
#define QUAKE_HIT(task) ((task)->unk_3c.l)
#define LANE(task) ((struct lane *)(task)->motion.p)
/* a lane's flags, in its task->unk_3c. LANE_CRACKED and LANE_SETTLING share a
 * bit. only cracking and tilting lanes read the first, and only swinging lanes
 * the second */
#define LANE_FLAGS(task) ((task)->unk_3c.l)
#define LANE_CRACKED 0x1 /* the lane draws its cracked model */
#define LANE_SETTLING 0x1 /* the swing slows to rest */
#define LANE_SWING_BACK 0x2 /* the lane swings toward -SWING_LIMIT */
/* a bit of a lane's ang.x that turns its swing or tilt the other way */
#define LANE_REVERSE 0x100
/* a lane's size class in the collision world is measured by its model's
 * radius plus WORLD_MARGIN */
#define WORLD_MARGIN 15.0F
/* the world collects a lane's copy only while a player lies near: within the
 * copy's radius plus REACH_MARGIN */
#define REACH_MARGIN 50.0F
/* as its quake turns on, a lane sets its timer to its distance from the quake
 * times PHASE_PER_DISTANCE. the lane's shake runs that many steps ahead of
 * the quake's timer */
#define PHASE_PER_DISTANCE 0.1F
/* a cracking lane goes by its quake's timer. it draws cracked from CRACK_STEP
 * and falls from FALL_STEP. at each odd timer above BURST_FIRST and below
 * BURST_END, a puff bursts from a piece. the piece is
 * highway_lane_cracked_object's child (timer - BURST_FIRST) / 2, from 0 */
#define BURST_FIRST 55
#define BURST_END 78
#define CRACK_STEP 70
#define FALL_STEP 120
/* a puff rises at BURST_RISE and draws BURST_DEPTH_BIAS nearer, as
 * burst_create takes them */
#define BURST_RISE 0.1F
#define BURST_DEPTH_BIAS 8.5F
/* each step a falling lane's speed gains FALL_GRAVITY down, then keeps
 * FALL_DRAG of itself */
#define FALL_GRAVITY 0.07F
#define FALL_DRAG 0.99F
/* a lane whose fall ends plays LAND_SOUND where it stays, and BREAK_SOUND
 * where it breaks. it rumbles at LAND_RUMBLE_POWER too */
#define LAND_SOUND 3
#define BREAK_SOUND 2
#define LAND_RUMBLE_POWER 5
/* the dust a lane raises as its fall ends, as dust_rect takes it */
#define DUST_HALF_X 55.0F
#define DUST_HALF_Z 50.0F
#define DUST_SCALE 5.0F
#define DUST_DENSITY 0.11F
#define DUST_SPEED 0.3F
/* a swing pushes out to SWING_LIMIT degrees and bounces there, until a bounce
 * leaves slower than SWING_SETTLE_SPEED. it then settles back and swings the
 * other way. a swing's or a tilt's speed keeps SWING_KEEP of itself a step */
#define SWING_LIMIT 30.0F
#define SWING_SETTLE_SPEED 0.3F
#define SWING_KEEP 0.998F
/* a tilt's speed gains TILT_PUSH a step, out to TILT_LIMIT degrees. there it
 * bounces back with TILT_BOUNCE of its speed. the tilt holds where it is once
 * its quake's timer passes TILT_STEPS */
#define TILT_LIMIT 15.0F
#define TILT_PUSH 0.035F
#define TILT_BOUNCE 0.6F
#define TILT_STEPS 150
/* a breaking lane fades over FADE_STEPS, then ends its task. a fading lane
 * draws tinted white, with its alpha times ALPHA_MAX as the tint's alpha */
#define FADE_STEPS 70
#define ALPHA_MAX 0xff
#define WHITE_RGB 0xffffff
/* the byte placement_set_state keeps once a lane's fall ends. a lane made
 * again from its placement starts where its fall ends */
#define STATE_FALLEN 1
/* a lane's step that finds its quake's tick above LANE_SHAKE_FROM sets the
 * lane's tick to LANE_SHAKE_STEPS. a step that finds the lane's tick above 0
 * sets the lane's shake and counts the tick down. the shake is lane_shake's sum
 * times LANE_SHAKE_HEIGHT, scaled by the tick over LANE_SHAKE_STEPS. on any
 * other step the shake keeps SHAKE_KEEP of itself */
#define LANE_SHAKE_STEPS 30
#define LANE_SHAKE_FROM 50
#define LANE_SHAKE_HEIGHT 3.0F
#define SHAKE_KEEP 0.99F

NJS_TEXLIST *highway_lane_texlists[3] = {
#include "stg14/lane/highway_lane_texlists.inc"
};
static const char *lane_type_names[LANE_TYPES] = {
#include "stg14/lane/lane_type_names.inc"
};
/* a settling swing keeps swing_drag of its angle a step, and ends within
 * swing_rest degrees of 0. otherwise a swing's speed gains swing_push a step,
 * and keeps swing_bounce of itself as it bounces off SWING_LIMIT */
static Float swing_rest = 10.0F;
static Float swing_push = 0.08F;
static Float swing_drag = 0.95F;
static Float swing_bounce = 0.6F;
/* the quake of each id, or null, in QUAKE_SLOTS slots. the list is null while
 * no quake is listed */
static struct task **quake_tasks = NULL;
/* the screen's shake along y, x and z. each axis sums its own table at the
 * quake's timer */
static struct quake_wave quake_shake_y[QUAKE_WAVES] = {
    {11261, 0.3F},
    {3728, 0.19F},
    {485, 0.3F},
    {137, 0.2F},
};
static struct quake_wave quake_shake_x[QUAKE_WAVES] = {
    {11261, 0.4F},
    {2048, 0.19F},
    {408, 0.3F},
    {105, 0.2F},
};
static struct quake_wave quake_shake_z[QUAKE_WAVES] = {
    {4674, 0.31F},
    {3728, 0.3F},
    {306, 0.3F},
    {121, 0.1F},
};
/* the table a lane's shake sums, at its quake's timer plus its own */
static struct quake_wave lane_shake[QUAKE_WAVES] = {
    {11261, 0.2F},
    {3712, 0.19F},
    {526, 0.3F},
    {137, 0.2F},
};
/* two shapes, of which collision_init copies LANE_SHAPES: the first alone.
 * it collides only while the lane falls, and hurts what it meets. a falling
 * lane writes flags and a center.y past the copy, where the second's copy
 * would lie. the center.y it writes is lane_shapes[1]'s, less the height left
 * to fall */
#define LANE_SHAPES 1
/* clang-format off */
static struct collision_shape lane_shapes[] = {
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_ALL, SHAPE_HURTABLE,
     {0.0F, -15.0F, 0.0F}, 48.0F, 10.0F, 48.0F, 0, {0, 0, 0}},
    {0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, -13.0F, 0.0F}, 48.0F, 10.0F, 48.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* a puff's scale is burst_scale plus up to burst_scale_range, at random */
static Float burst_scale = 2.0F;
static Float burst_scale_range = 2.0F;

/* @hack type: step an int cast to Uint16 where it is read, where a Uint16
 * parameter would do */
static Float quake_wave_sum(struct quake_wave *wave, int count, int step)
{
    Float sum = 0.0F;

    while (count--) {
        sum += njSin(wave->turn * (Uint16)step) * wave->amp;
        wave++;
    }
    return sum;
}

/* list task at id, unless a quake is listed there. a null list is allocated
 * first, and nothing is listed where the heap can't give it */
static void quake_list_add(struct task *task, int id)
{
    if (quake_tasks != NULL) {
        if (quake_tasks[id] == NULL) {
            quake_tasks[id] = task;
        }
    } else {
        quake_tasks = syCalloc(QUAKE_SLOTS, sizeof(struct task *));
        if (quake_tasks != NULL) {
            quake_tasks[id] = task;
        }
    }
}

/* take task out of the list, and free the list once no quake is listed */
static void quake_list_remove(struct task *task)
{
    int i;
    int empty;

    if (quake_tasks != NULL) {
        empty = 0;
        for (i = 0; i < QUAKE_SLOTS; i++) {
            if (quake_tasks[i] == task) {
                quake_tasks[i] = NULL;
            }
            if (quake_tasks[i] == NULL) {
                empty++;
            }
        }
        if (empty == QUAKE_SLOTS) {
            syFree(quake_tasks);
            quake_tasks = NULL;
        }
    }
}

void highway_lane_quake_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- LANE ID";
        (void)"<- LANE TYPE";
        (void)"<- COL SCL";
        (void)"<- FALL HEIGHT <0:BREAK";
        (void)"DIR:REVERSE";
        (void)"DIR:NORM";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = quake_draw;
    task->step = quake_step;
    task->destructor = quake_destroy;
    quake_list_add(task, QUAKE_ID(work));
}

static void quake_destroy(struct task *task)
{
    quake_list_remove(task);
}

static void quake_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;
    NJS_POINT3 shake;
    NJS_VECTOR dir;
    int step;
    int type;
    Float scale;

    /* @hack spelling: the step's body in the else of its first test, where
     * the return alone would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (game_state.time_stopped) {
            /* @hack redundant: statements after the return, which no path
             * reaches, kept for the registers SHC gives the step. the image
             * holds none of their code, so they are stand-ins */
            if (1) {
                return;
            }
            type = work->ang.z & TYPE_BITS;
            type %= LANE_TYPES;
            work->timer = type;
            work->mode = MODE_QUAKE_OFF;
            work->tick = 0xf;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((player = player_near(&work->pos, work->scl.x + QUAKE_MARGIN))) {
            work->mode = MODE_QUAKE_ON;
            work->submode = player;
            /* @bug logic: no player rumbles as it enters, and an idle quake
             * always sets its timer back to 0. both tests read the quake's
             * own ang.z as a lane's type. that never equals NO_TYPE, at 0x20e
             * and 0x28a of the object's code */
            if (LANE_TYPE(work) == NO_TYPE) {
                if (!work->tick) {
                    vibration_start(player - NEAR_SLOT_0, RUMBLE_POWER,
                                    RUMBLE_FREQ, ENTER_RUMBLE_TIME);
                }
                work->tick = ENTER_SHAKE_STEPS;
            }
        } else {
            work->mode = MODE_QUAKE_OFF;
        }
        if (QUAKE_HIT(task)) {
            if (!work->tick && work->submode) {
                vibration_start(work->submode - NEAR_SLOT_0, RUMBLE_POWER,
                                RUMBLE_FREQ, RUMBLE_TIME);
            }
            work->tick = SHAKE_STEPS;
        } else if (LANE_TYPE(work) != NO_TYPE && !work->tick) {
            work->timer = 0;
        }
        if (work->tick) {
            step = work->timer++;
            scale = work->tick * (1.0F / SHAKE_STEPS);
            shake.x = quake_wave_sum(quake_shake_x, QUAKE_WAVES, step) * scale;
            shake.y = quake_wave_sum(quake_shake_y, QUAKE_WAVES, step) * scale;
            shake.z = quake_wave_sum(quake_shake_z, QUAKE_WAVES, step) * scale;
            camera_set_shake(SHAKE_CAMERA, &shake);
            if (rand() * RAND_SCALE < RUMBLE_SOUND_CHANCE) {
                /* @hack redundant: dir, a random vector up to scl.x plus
                 * QUAKE_MARGIN long, and the matrix that turns it, which
                 * nothing reads. only its rand() calls change what follows */
                dir.x = 0.0F;
                dir.y = rand() * (work->scl.x + QUAKE_MARGIN) * RAND_SCALE;
                dir.z = 0.0F;
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateEx(&work->pos);
                njRotateX(NULL, rand() * RAND_TURN);
                njRotateZ(NULL, rand() * RAND_TURN);
                njCalcVector(NULL, &dir, &dir);
                njPopMatrixEx();
                sound_play(RUMBLE_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
            }
            work->tick--;
        }
        QUAKE_HIT(task) = 0;
    }
}

static void quake_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;

    /* @hack redundant: a body after the return, which no path reaches, kept
     * for the frame SHC gives the draw */
    if (1) {
        return;
    }
    pos = work->pos;
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njPopMatrixEx();
}

void highway_lane_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct lane));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = lane_draw;
    task->step = lane_step;
    task->destructor = lane_destroy;
    object = &LANE(task)->object;
    /* @hack spelling: the copy in both branches, where one before the test
     * would do */
    if (LANE_TYPE(work) == LANE_TYPE_D) {
        *object = highway_lane_world;
        object->ang[1] = work->ang.y - QUARTER_TURN;
    } else {
        *object = highway_lane_world;
        object->ang[1] = work->ang.y;
    }
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    size = object->model->r + WORLD_MARGIN;
    if (!(size > WORLD_SIZE_CLASS_1_MAX)) {
        if (size > WORLD_SIZE_CLASS_2_MAX) {
            flags = WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID;
        } else if (size > WORLD_SIZE_CLASS_3_MAX) {
            flags = WORLD_SIZE_CLASS(2) | WORLD_MOVING | WORLD_SOLID;
        } else {
            flags = WORLD_SIZE_CLASS(3) | WORLD_MOVING | WORLD_SOLID;
        }
    }
    world_add(flags, task, object);
    collision_init(task, lane_shapes, LANE_SHAPES, COLLISION_OBJECT);
    work->collision->shapes[0].flags |= SHAPE_OFF;
    /* @bug memory: writes a second shape's flags past the collision record's
     * copy of one shape. collision_init copies LANE_SHAPES, one, at 0x540 of
     * the object's code. this store writes past the copy at 0x556, and
     * lane_step at 0xf92, 0xfa6 and 0xfc8 */
#ifdef NONMATCHING
    /* this side repairs the tag above: it writes nothing past the record's
     * one shape, and nor does lane_step */
#else
    work->collision->shapes[1].flags |= SHAPE_OFF;
#endif
    if (task->placement != NULL && placement_get_state(task)) {
        if (work->scl.y > 0.0F) {
            work->mode = MODE_FALLEN;
        } else {
            work->mode = MODE_FADE;
        }
        /* @hack spelling: the height added negated, where taking it would
         * do */
        work->pos.y += -njAbs(work->scl.y);
    }
    LANE(task)->alpha = 1.0F;
}

static void lane_destroy(struct task *task)
{
    if (LANE(task)->object.model != NULL) {
        world_remove(task, &LANE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

#pragma inline(quake_find)
static struct task *quake_find(int id)
{
    if (quake_tasks != NULL) {
        return quake_tasks[id];
    }
    return NULL;
}

/* set the three Floats at out to those at to, less those at from */
#pragma inline(point_difference)
static void point_difference(Float *from, Float *to, Float *out)
{
    *out++ = *to++ - *from++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}

static void lane_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *quake;
    struct task_force *rider;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *child;
    NJS_POINT3 pos;
    Angle old_turn;
    Float spd;
    Float dir;
    Float scale;
    Float rise;
    int i;
    int k;
    int piece;
    int quake_timer;

    /* @hack spelling: the quake's lookup in the else of the first test,
     * where a statement of its own after the return would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        quake = quake_find(QUAKE_ID(work));
        if (quake == NULL) {
            return;
        }
    }
    old_turn = LANE(task)->turn;
    pos = work->pos;
    for (i = 0; i < TASK_FORCES; i++) {
        if (player_work[i] != NULL) {
            TASK_FORCES_OF(task)[i].speed.x = 0.0F;
            TASK_FORCES_OF(task)[i].speed.y = 0.0F;
            TASK_FORCES_OF(task)[i].speed.z = 0.0F;
            TASK_FORCES_OF(task)[i].turn.x = 0;
            TASK_FORCES_OF(task)[i].turn.y = 0;
            TASK_FORCES_OF(task)[i].turn.z = 0;
        }
    }
    if (!game_state.time_stopped) {
        switch (work->mode) {
        case MODE_WAIT:
            if (quake->work->mode) {
                work->timer = njDistanceP2P(&work->pos, &quake->work->pos) *
                              PHASE_PER_DISTANCE;
                switch (LANE_TYPE(work)) {
                case LANE_TYPE_A:
                case LANE_TYPE_B:
                case LANE_TYPE_E:
                    work->mode = MODE_CRACK;
                    break;
                case LANE_TYPE_C:
                    work->mode = MODE_TILT;
                    break;
                case LANE_TYPE_D:
                    work->mode = MODE_SWING;
                    break;
                case LANE_TYPE_F:
                    break;
                }
                /* @hack redundant: a break inside the test, where the one after
                 * it would do */
                break;
            }
            break;
        case MODE_CRACK:
            QUAKE_HIT(quake) = 1;
            if (quake->work->timer < FALL_STEP) {
                if (quake->work->timer == CRACK_STEP) {
                    LANE_FLAGS(task) = LANE_CRACKED;
                }
                quake_timer = quake->work->timer;
                if (quake_timer > BURST_FIRST && quake_timer < BURST_END &&
                    (quake_timer & 1)) {
                    NJS_POINT3 piece_pos;
                    NJS_VECTOR burst_spd;

                    /* @hack spelling: piece in two statements, where one
                     * would do */
                    piece = quake->work->timer - BURST_FIRST;
                    piece = piece >> 1;
                    i = 0;
                    /* @hack spelling: object, a local for the cracked node's
                     * address, where the node read in place would do */
                    object = &highway_lane_cracked_object;
                    njPushMatrixEx();
                    njUnitMatrix(NULL);
                    njTranslateEx(&work->pos);
                    njRotateY(NULL, work->ang.y);
                    if (LANE_TYPE(work) != LANE_TYPE_D) {
                        njRotateX(NULL, LANE(task)->turn);
                    } else {
                        njRotateZ(NULL, LANE(task)->turn);
                    }
                    /* @hack spelling: child set before the burst's speed, where
                     * the loop's start would set it */
                    child = object->child;
                    burst_spd.x = 0.0F;
                    burst_spd.y = BURST_RISE;
                    burst_spd.z = 0.0F;
                    for (; child; child = child->sibling, i++) {
                        if (i == piece) {
                            njCalcPoint(NULL, (NJS_POINT3 *)child->pos,
                                        &piece_pos);
                            burst_create(
                                &piece_pos, &burst_spd,
                                burst_scale +
                                    (rand() * burst_scale_range * RAND_SCALE),
                                BURST_DEPTH_BIAS);
                        }
                    }
                    njPopMatrixEx();
                }
            } else {
                work->mode = MODE_FALL;
            }
            break;
        case MODE_FALL:
            QUAKE_HIT(quake) = 1;
            /* @hack spelling: the fall's speed taken inside the test, and the
             * comparison turned about, where a statement of its own and the
             * height's side first would do */
            if ((spd = LANE(task)->fall_spd) < -njAbs(work->scl.y)) {
                struct angles ang;

                /* @bug logic: the fall's last step moves the lane up by the
                 * height left, where every other step moves it down. the
                 * speed takes the height's absolute value, not its negation,
                 * at 0x902 of the object's code */
                spd = njAbs(work->scl.y);
                if (work->scl.y > 0.0F) {
                    work->mode = MODE_FALLEN;
                    sound_play(LAND_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                    if (quake->work->submode) {
                        vibration_start(quake->work->submode - NEAR_SLOT_0,
                                        LAND_RUMBLE_POWER, RUMBLE_FREQ,
                                        VIBRATION_UNTIMED);
                    }
                } else {
                    work->mode = MODE_FADE;
                    sound_play(BREAK_SOUND, NULL, SOUND_PRIORITY, SOUND_VOLUME);
                    if (quake->work->submode) {
                        vibration_start(quake->work->submode - NEAR_SLOT_0,
                                        LAND_RUMBLE_POWER, RUMBLE_FREQ,
                                        VIBRATION_UNTIMED);
                    }
                }
                if (task->placement != NULL) {
                    placement_set_state(task, STATE_FALLEN);
                }
                ang.y = work->ang.y;
                ang.x = 0;
                ang.z = 0;
                dust_rect(&work->pos, DUST_HALF_X, DUST_HALF_Z, &ang,
                          DUST_SCALE, DUST_DENSITY, DUST_SPEED);
            }
            work->scl.y += work->scl.y > 0.0F ? spd : -spd;
            if (task->force.p != NULL) {
                if (player_work[0] != NULL) {
                    TASK_FORCES_OF(task)[0].speed.y = spd;
                }
                if (player_work[1] != NULL) {
                    TASK_FORCES_OF(task)[1].speed.y = spd;
                }
            }
            /* @hack redundant: a test of spd that no path reaches */
            if (0) {
                if (spd) {
                }
            }
            work->pos.y += spd;
            LANE(task)->fall_spd -= FALL_GRAVITY;
            LANE(task)->fall_spd *= FALL_DRAG;
            break;
        case MODE_SWING:
            QUAKE_HIT(quake) = 1;
            if (LANE_FLAGS(task) & LANE_SETTLING) {
                LANE(task)->swing *= swing_drag;
                if (swing_rest > njAbs(LANE(task)->swing)) {
                    if (!quake->work->mode) {
                        work->mode = MODE_WAIT;
                    } else {
                        if (LANE_FLAGS(task) & LANE_SWING_BACK) {
                            LANE_FLAGS(task) &= ~LANE_SWING_BACK;
                        } else {
                            LANE_FLAGS(task) |= LANE_SWING_BACK;
                        }
                        LANE_FLAGS(task) &= ~LANE_SETTLING;
                        LANE(task)->swing_spd = 0.0F;
                    }
                }
            } else {
                dir = LANE_FLAGS(task) & LANE_SWING_BACK ? -1 : 1;
                LANE(task)->swing += LANE(task)->swing_spd * dir;
                LANE(task)->swing_spd =
                    (LANE(task)->swing_spd * SWING_KEEP) + swing_push;
                if (LANE(task)->swing * dir > SWING_LIMIT) {
                    LANE(task)->swing_spd =
                        -(LANE(task)->swing_spd * swing_bounce);
                    LANE(task)->swing = dir * SWING_LIMIT;
                    if (njAbs(LANE(task)->swing_spd) < SWING_SETTLE_SPEED) {
                        LANE_FLAGS(task) |= LANE_SETTLING;
                    }
                }
            }
            LANE(task)->turn =
                work->ang.x & LANE_REVERSE
                    ? (Angle)(-LANE(task)->swing * ANGLE_PER_DEGREE)
                    : (Angle)(LANE(task)->swing * ANGLE_PER_DEGREE);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            /* @bug logic: a swinging lane turns its riders about another axis
             * than its own. only a LANE_TYPE_D lane swings, and its draw turns
             * it about z, at 0x1100 of the object's code. its riders turn
             * about x, at 0xbd2 */
            njRotateX(NULL, LANE(task)->turn - old_turn);
            njRotateY(NULL, -work->ang.y);
            njTranslate(NULL, -pos.x, -pos.y, -pos.z);
            rider = TASK_FORCES_OF(task);
            for (k = 0; k < TASK_FORCES; k++) {
                if (player_work[k] != NULL) {
                    NJS_POINT3 moved;
                    NJS_VECTOR move;

                    njCalcPoint(NULL, &player_work[k]->pos, &moved);
                    /* @hack spelling: the swing's difference walked by
                     * point_difference's pointers, where a statement for each
                     * axis would do */
                    point_difference(&player_work[k]->pos.x, &moved.x, &move.x);
                    if (task->force.p != NULL) {
                        rider->speed = move;
                    }
                }
                rider++;
            }
            njPopMatrixEx();
            break;
        case MODE_TILT:
            if (!(LANE_FLAGS(task) & LANE_CRACKED)) {
                QUAKE_HIT(quake) = 1;
            }
            if (quake->work->timer > TILT_STEPS) {
                work->mode = MODE_TILTED;
            }
            LANE(task)->swing += LANE(task)->swing_spd;
            LANE(task)->swing_spd =
                (LANE(task)->swing_spd * SWING_KEEP) + TILT_PUSH;
            if (LANE(task)->swing > TILT_LIMIT) {
                LANE(task)->swing_spd = -(LANE(task)->swing_spd * TILT_BOUNCE);
                LANE(task)->swing = TILT_LIMIT;
                LANE_FLAGS(task) |= LANE_CRACKED;
            }
            LANE(task)->turn =
                work->ang.x & LANE_REVERSE
                    ? (Angle)(-LANE(task)->swing * ANGLE_PER_DEGREE)
                    : (Angle)(LANE(task)->swing * ANGLE_PER_DEGREE);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njRotateX(NULL, LANE(task)->turn - old_turn);
            njRotateY(NULL, -work->ang.y);
            njTranslate(NULL, -pos.x, -pos.y, -pos.z);
            rider = TASK_FORCES_OF(task);
            for (k = 0; k < TASK_FORCES; k++) {
                if (player_work[k] != NULL) {
                    NJS_POINT3 moved;
                    NJS_VECTOR move;

                    njCalcPoint(NULL, &player_work[k]->pos, &moved);
                    /* @hack spelling: the tilt's difference walked by
                     * point_difference's pointers, where a statement for each
                     * axis would do */
                    point_difference(&player_work[k]->pos.x, &moved.x, &move.x);
                    if (task->force.p != NULL) {
                        rider->speed = move;
                    }
                }
                rider++;
            }
            njPopMatrixEx();
            break;
        case MODE_FADE:
            /* @hack redundant: the turn stored back as it was */
            LANE(task)->turn = old_turn;
            LANE(task)->alpha -= 1.0F / FADE_STEPS;
            if (LANE(task)->alpha < 0.0F) {
                LANE(task)->alpha = 0.0F;
                if (task->placement != NULL) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
            }
            break;
        /* @hack redundant: two cases that store the turn back as it was */
        case MODE_FALLEN:
            LANE(task)->turn = old_turn;
            break;
        case MODE_TILTED:
            LANE(task)->turn = old_turn;
            break;
        }
        LANE(task)->object.pos[1] = work->pos.y + LANE(task)->shake;
        /* @hack spelling: the turn stored in both branches, where one store
         * before the test would do */
        if (LANE_TYPE(work) == LANE_TYPE_D) {
            LANE(task)->object.ang[0] = LANE(task)->turn;
            LANE(task)->object.ang[1] = work->ang.y - QUARTER_TURN;
        } else {
            LANE(task)->object.ang[0] = LANE(task)->turn;
            LANE(task)->object.ang[1] = work->ang.y;
        }
        if (quake->work->tick > LANE_SHAKE_FROM) {
            work->tick = LANE_SHAKE_STEPS;
        }
        if (work->tick) {
            scale = work->tick * (1.0F / LANE_SHAKE_STEPS);
            rise = quake_wave_sum(lane_shake, QUAKE_WAVES,
                                  quake->work->timer + work->timer) *
                   scale * LANE_SHAKE_HEIGHT;
            if (task->force.p != NULL) {
                if (player_work[0] != NULL) {
                    TASK_FORCES_OF(task)[0].speed.y = rise - LANE(task)->shake;
                }
                if (player_work[1] != NULL) {
                    TASK_FORCES_OF(task)[1].speed.y = rise - LANE(task)->shake;
                }
            }
            LANE(task)->shake = rise;
            /* @hack redundant: a test of rise that no path reaches */
            if (0) {
                if (rise + rise + rise) {
                }
            }
            work->tick--;
        } else {
            LANE(task)->shake *= SHAKE_KEEP;
        }
    }
    if (work->mode == MODE_FALL) {
        work->collision->shapes[0].flags &= ~SHAPE_OFF;
#ifdef NONMATCHING
        /* this side repairs the tag in highway_lane_init: it writes no second
         * shape past the record's one */
#else
        work->collision->shapes[1].flags &= ~SHAPE_OFF;
        work->collision->shapes[1].center.y =
            lane_shapes[1].center.y - njAbs(work->scl.y);
#endif
        collision_fit_radius(task);
    } else {
        work->collision->shapes[0].flags |= SHAPE_OFF;
#ifdef NONMATCHING
        /* this side repairs the tag in highway_lane_init: it writes no second
         * shape past the record's one */
#else
        work->collision->shapes[1].flags |= SHAPE_OFF;
#endif
    }
    collision_register(task);
    if (player_near((NJS_POINT3 *)LANE(task)->object.pos,
                    LANE(task)->object.model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void lane_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *cracked;
    /* @hack type: color an int, where the Uint32 model_draw_tinted takes
     * would do */
    int color;
    int level;

    texlist = &highway_lane_texlist;
    /* @hack spelling: object and cracked, locals for the nodes' addresses,
     * where the nodes read in place would do */
    object = &highway_lane_object;
    /* @hack redundant: a block no path reaches, kept for the registers SHC
     * gives the draw */
    if (0) {
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        njCnkEasyDrawModel(object->model);
    }
    cracked = &highway_lane_cracked_object;
    model = object->model;
    switch (work->mode) {
    case MODE_CRACK:
        if (!LANE_FLAGS(task)) {
            break;
        }
        texlist = &highway_lane_cracked_texlist;
        model = cracked->model;
        break;
    case MODE_TILT:
        if (!(LANE_FLAGS(task) & LANE_CRACKED)) {
            break;
        }
    case MODE_FALL:
    case MODE_FADE:
    case MODE_FALLEN:
    case MODE_TILTED:
        texlist = &highway_lane_cracked_texlist;
        model = cracked->model;
        break;
    }
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + LANE(task)->shake,
                work->pos.z);
    njRotateY(NULL, work->ang.y);
    if (LANE_TYPE(work) != LANE_TYPE_D) {
        njRotateX(NULL, LANE(task)->turn);
    } else {
        njRotateZ(NULL, LANE(task)->turn);
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(texlist);
    if (LANE(task)->alpha == 1.0F) {
        njCnkEasyDrawModel(model);
    } else {
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_UA | NJD_FST_IS);
        level = LANE(task)->alpha * ALPHA_MAX;
        if (level > ALPHA_MAX) {
            level = ALPHA_MAX;
        }
        if (level < 0) {
            level = 0;
        }
        color = (level << ALPHA_SHIFT) + WHITE_RGB;
        model_draw_tinted(model, color);
        draw_attr_restore();
        draw_control_restore();
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
}
