/* Mad Space's fireballs, FIREBALL and FB2 in its object list. FIREBALL is a
 * burning rock that a touch or its timer breaks. shared/breaking_fireball.h
 * compiles its code. FB2 is a rock that runs round a path */
#include "stg44/fireball/text.inc"
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "stg44/fireball.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

#define ORBIT_FIREBALL
#define FIREBALL_INIT mad_space_fireball_init
#define FIREBALL_TEXLIST mad_space_fireball_texlist
#define FIREBALL_OBJECT mad_space_fireball_object
#define FIREBALL_SHARD_TEXLIST mad_space_fireball_shard_texlist
#define FIREBALL_SHARD_OBJECT mad_space_fireball_shard
#define FIREBALL_PIECE_TEXLIST mad_space_fireball_piece_texlist
#define FIREBALL_PIECE_OBJECT mad_space_fireball_piece
/* a fireball's timer starts once a player comes within scl.x plus
 * BREAK_NEAR. the fireball breaks once the timer passes BREAK_WAIT */
#define BREAK_WAIT 120
#define BREAK_NEAR 30.0F
/* the break plays BREAK_SOUND for each piece it throws */
#define BREAK_SOUND 9
/* a fireball and FB2's rock play LOOP_SOUND while a player lies within
 * SOUND_RANGE */
#define LOOP_SOUND 7
#define SOUND_RANGE 500.0F
/* the break throws PIECES pieces, the first SHARDS of them shards. each
 * piece's speed in y gains PIECE_LIFT after the speed's turn */
#define PIECES 20
#define SHARDS 5
#define PIECE_FLOOR (-100000.0F)
#define PIECE_LIFT 0.5F
/* the chance that a step sheds a spark */
#define SPARK_CHANCE 0.2F
/* while its timer runs, the fireball blinks from BLINK_COLOR toward
 * BLINK_RED. the blink turns BLINK_RATE Angle units a step of the timer */
#define BLINK_RATE 0x500
#define BLINK_COLOR 0xffffffff
#define BLINK_RED 0xffff2020
#define HURT_PLAYER                                                            \
    (SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |               \
     SHAPE_HURTS_PLAYERS)
/* FB2's rock turns SPIN_X, SPIN_Y and SPIN_Z Angle units a step about x, y
 * and z */
#define SPIN_X 0xb6
#define SPIN_Y 0x7f
#define SPIN_Z 0x12
/* the rock moves FB2's scl.x plus BASE_SPEED a step, in the units of the path
 * before its scale. the path takes a scale of FB2's scl.y plus BASE_SCALE. the
 * rock's speed in the world scales with it */
#define BASE_SPEED 0.5F
#define BASE_SCALE 1.0F
#define ROCK_SHAPES 1
#define SOUND_PRIORITY 1

/* the rock's place on FB2's path, in its task's motion */
#define CURSOR(task) ((struct loop_cursor *)(task)->motion.p)
/* FB2's path, in the unk_3c of FB2's task and of its rock's */
#define ORBIT_PATH(task) ((struct path *)(task)->unk_3c.p)

#include "stg44/meteor_models.h"

static struct path mad_space_fireball_path;

NJS_TEXLIST *mad_space_fireball_textures[5] = {
#include "stg44/fireball/mad_space_fireball_textures.inc"
};
/* clang-format off */
static struct collision_shape fireball_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), HURT_PLAYER,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 32.0F, 0, 0, 0, {0, 0, 0},
};
/* clang-format on */
static Uint32 dat_8c5898a0 = 0xff0070ff;
static Uint32 fireball_spark_color = 0xff3020ff;
static Float fireball_spark_size = 20.0F;
static Float fireball_spark_size_range = 30.0F;
/* the shapes of FB2's rock: safe_shape where FB2's scl.z is 1 or more, and
 * harmful_shape otherwise */
/* clang-format off */
static struct collision_shape safe_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, SHAPE_HURTABLE,
    {0.0F, 0.0F, 0.0F}, 32.0F, 0, 0, 0, {0, 0, 0},
};
static struct collision_shape harmful_shape = {
    0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), HURT_PLAYER,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 32.0F, 0, 0, 0, {0, 0, 0},
};
/* clang-format on */

#include "stg44/loop_path_impl.h"
#include "shared/breaking_fireball.h"

static void orbit_destroy(struct task *task);
static void orbit_draw(struct task *task);
static void orbit_step(struct task *task);

/* @hack redundant: an int return of loop_advance's 0, which no caller reads */
static int orbit_advance(struct task *task, Float distance)
{
    struct task_work *work = task->work;
    struct task_work *placement = task->parent->work;
    NJS_POINT3 pos;
    NJS_VECTOR direction;
    Angle slope_x;
    Angle slope_z;
    Float scale;
    int result;

    result = loop_advance(ORBIT_PATH(task->parent), &CURSOR(task)->segment,
                          &CURSOR(task)->fraction, distance, &pos, &direction,
                          &slope_x, &slope_z);
    /* @hack spelling: scl.y read through task->parent->work, where placement
     * would do */
    scale = task->parent->work->scl.y + BASE_SCALE;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&placement->pos);
    njRotateEx(&placement->ang.x, 0);
    njScale(NULL, scale, scale, scale);
    njCalcPoint(NULL, &pos, &work->pos);
    njPopMatrixEx();
    return result;
}

static void orbit_rock_init(struct task *task)
{
    task->motion.p = syCalloc(1, sizeof(struct loop_cursor));
    if (task->motion.p) {
        task->step = orbit_step;
        task->draw = orbit_draw;
        task->destructor = orbit_destroy;
        CURSOR(task)->fraction = 0.0F;
        CURSOR(task)->segment = 0;
        task->unk_3c.l = task->parent->unk_3c.l;
        orbit_advance(task, 0.0F);
        if (task->parent->work->scl.z >= 1.0F) {
            collision_init(task, &safe_shape, ROCK_SHAPES, COLLISION_OBJECT);
        } else {
            collision_init(task, &harmful_shape, ROCK_SHAPES, COLLISION_OBJECT);
        }
    }
}

static void orbit_destroy(struct task *task)
{
    /* @hack redundant: a read of task->work that the destructor never uses */
    struct task_work *work = task->work;
    (void)work;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void orbit_draw(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&mad_space_orbit_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(mad_space_orbit_object->model);
    njPopMatrixEx();
}

static void orbit_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR velocity;
    struct particle *spark;

    /* @hack redundant: a goto to the statement after it */
    goto step;
step:
    collision_register(task);
    if (RANDOM_UNIT() < SPARK_CHANCE) {
        velocity.x = SIGNED_RANDOM();
        velocity.y = SIGNED_RANDOM();
        velocity.z = SIGNED_RANDOM();
        pos = work->pos;
        spark = burst_create_fast(
            &pos, &velocity,
            fireball_spark_size +
                ((Float)rand() * fireball_spark_size_range * RAND_SCALE),
            0.0F);
        if (spark) {
            spark->color = fireball_spark_color;
        }
    }
    work->ang.x += SPIN_X;
    work->ang.y += SPIN_Y;
    work->ang.z += SPIN_Z;
    orbit_advance(task, task->parent->work->scl.x + BASE_SPEED);
    if (player_near(&work->pos, SOUND_RANGE)) {
        sound_play_at_for(LOOP_SOUND, task, SOUND_PRIORITY, SOUND_LOUDEST,
                          LOOP_SOUND_STEPS, &work->pos);
    }
}

void mad_space_orbit_init(struct task *task)
{
    /* @hack type: work a volatile pointer, which keeps a store nothing
     * reads, where no local would do */
    struct task_work *volatile work = task->work;
    struct task *rock;

    /* @hack redundant: the if (1) around the init's body, where the body alone
     * would do */
    if (1) {
        if (!placement_try_leave(task)) {
            task->unk_3c.p = &mad_space_fireball_path;
            if (!task->child) {
                task_create_child(TASK_WORK | TASK_FORCE, orbit_rock_init,
                                  task);
            }
        }
        return;
    }
    /* @hack redundant: a block no path reaches, which SHC still compiles. it
     * holds the editor's strings. it copies FB2's pos, ang and scl into the
     * rock, and draws the rock */
    {
        rock = task->child;
        (void)"<- ANG X";
        (void)"<- ANG Y";
        (void)"<- ANG Z";
        (void)"<- SPD";
        (void)"<- PATHSCL";
        (void)"<- COL(NODMG)";
        (void)"<- COL(DMG)";
        matrix_load_parent();
        rock->work->pos = rock->parent->work->pos;
        rock->work->ang = rock->parent->work->ang;
        rock->work->scl = rock->parent->work->scl;
        rock->draw(rock);
        rock->parent = task;
    }
}

/* the path FB2's rock runs round, in FB2's frame at a scale of 1 */
static struct path_point orbit_points[51] = {
#include "stg44/fireball/orbit_points.inc"
};
static struct path mad_space_fireball_path = {
#include "stg44/fireball/mad_space_fireball_path.inc"
};
