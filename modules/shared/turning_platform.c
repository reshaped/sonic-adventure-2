/* the turning platform, KAITEN in the object lists of both Sand Ocean stages.
 * it turns about y, and carries the players round with it. a fire burns at its
 * middle */
#include <ninja.h>
#include <sg_xpt.h>

#ifdef PLATFORM_ONE_PLAYER
#include "collision/collision.h"
#endif
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ocean_util.h"
#include "shared/turning_platform.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the bit of the work's ang.z that turns the platform the other way */
#define REVERSE 1
/* the platform's turn a step, and its angle about y, in task->unk_3c */
#define SPEED(work) ((work)->ang.z & REVERSE ? -(work)->ang.x : (work)->ang.x)
#define ANGLE(task) ((task)->unk_3c.l)
/* the flags world_add takes for the platform's copy, and for the child's */
#define PLATFORM_WORLD_FLAGS (WORLD_SIZE_CLASS(1) | WORLD_MOVING | WORLD_SOLID)
#define CHILD_WORLD_FLAGS                                                      \
    (WORLD_SIZE_CLASS(1) | WORLD_MOVING | GROUND_CLIMB_REVERSED | WORLD_SOLID)
/* the evalflags that would keep an object's pos, ang and scl from applying */
#define EVAL_UNITS (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
/* world_collect_near_players takes the copies only while a player lies
 * within NEAR_RADIUS times the scale, plus NEAR_MARGIN, of pos */
#define NEAR_RADIUS 150.0F
#define NEAR_MARGIN 15.0F
/* the sounds play from SOUND_HEIGHT times the scale above pos. each play
 * lasts SOUND_DURATION + 1 steps */
#define SOUND_HEIGHT 10.0F
#define TURN_SOUND 1
#define FIRE_SOUND 2
#define SOUND_PRIORITY 1
#define SOUND_DURATION 15
/* the fire's sound plays under a key of its own, one past task, the turn
 * sound's key */
#define FIRE_SOUND_KEY(task) ((char *)(task) + 1)
/* the fire's sound plays only while a player lies within fire_range of at,
 * the sounds' point. in the two-player stage, either player will do */
#define PLAYER_NEAR(slot, at)                                                  \
    (njDistanceP2P(&player_work[(slot)]->pos, (at)) < fire_range)
#ifdef PLATFORM_ONE_PLAYER
#define FIRE_SOUND_NEAR(at) PLAYER_NEAR(0, at)
#else
#define FIRE_SOUND_NEAR(at) (PLAYER_NEAR(0, at) || PLAYER_NEAR(1, at))
#endif
/* the word of the fire model's polygon list that holds its texture ID */
#define FIRE_TID 7

NJS_TEXLIST *turning_platform_texlists[] = {&turning_platform_texlist, NULL};
static Sint32 turn_volume = 25;
static Sint32 fire_volume = 25;
static Float fire_range = 100.0F;
#ifdef PLATFORM_ONE_PLAYER
/* the fire's shape keeps its size whatever the platform's scale */
/* clang-format off */
static struct collision_shape fire_shape = {
    0, SHAPE_CAPSULE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
    {0.0F, 25.0F, 0.0F}, 20.0F, 10.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
#endif

static void turning_platform_destroy(struct task *task);
static void turning_platform_step(struct task *task);
static void turning_platform_draw(struct task *task);
static void turning_platform_child_init(struct task *task);
static void turning_platform_child_destroy(struct task *task);
static void turning_platform_child_step(struct task *task);
static void turning_platform_child_draw(struct task *task);

void turning_platform_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    if (task_create_child(TASK_WORK | TASK_FORCE, turning_platform_child_init,
                          task) == NULL) {
        return;
    }
    task->draw = turning_platform_draw;
    task->step = turning_platform_step;
    task->destructor = turning_platform_destroy;
    /* @hack spelling: task->work read again, where work would do */
    ANGLE(task) = (SPEED(task->work) * game_state.steps) + work->ang.y;
    object = world_pool_alloc();
    /* @bug memory: copies turning_platform_object through null where
     * world_pool_alloc has no object to lend, at 0x56 of the object's code
     * in both stages. the destroy tests extra for null, at 0x8c503454 in
     * Sand Ocean and 0x8c500e48 in the two-player stage. the step writes the
     * copy's angle through extra untested, at 0x8c503584 and 0x8c500fd2 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the platform and its child, and set.c makes it again on a
     * later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = turning_platform_object;
    task->extra.p = object;
    object->evalflags &= ~EVAL_UNITS;
    object->scl[0] = object->scl[1] = object->scl[2] = task->work->scl.x + 1.0F;
    object->ang[0] = 0;
    object->ang[1] = ANGLE(task);
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(PLATFORM_WORLD_FLAGS, task, object);
#ifdef PLATFORM_ONE_PLAYER
    collision_init(task, &fire_shape, 1, COLLISION_OBJECT);
#endif
}

static void turning_platform_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void turning_platform_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Angle speed;
    Float x;
    Float z;
    NJS_POINT3 pos;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    ANGLE(task) = (SPEED(task->work) * game_state.steps) + work->ang.y;
    object = task->extra.p;
    speed = SPEED(task->work);
    ocean_turn_xz(player_work[0]->pos.x - work->pos.x,
                  player_work[0]->pos.z - work->pos.z, speed, &x, &z);
#ifdef PLATFORM_ONE_PLAYER
    TASK_FORCES_OF(task)
    [0].speed.x = TASK_FORCES_OF(task)[1].speed.x =
        x - player_work[0]->pos.x + work->pos.x;
    TASK_FORCES_OF(task)[0].speed.y = TASK_FORCES_OF(task)[1].speed.y = 0.0F;
    TASK_FORCES_OF(task)
    [0].speed.z = TASK_FORCES_OF(task)[1].speed.z =
        z - player_work[0]->pos.z + work->pos.z;
#else
    TASK_FORCES_OF(task)[0].speed.x = x - player_work[0]->pos.x + work->pos.x;
    TASK_FORCES_OF(task)[0].speed.y = 0.0F;
    TASK_FORCES_OF(task)[0].speed.z = z - player_work[0]->pos.z + work->pos.z;
#endif
    TASK_FORCES_OF(task)[0].turn.x = TASK_FORCES_OF(task)[1].turn.x = 0;
    TASK_FORCES_OF(task)[0].turn.y = TASK_FORCES_OF(task)[1].turn.y = -speed;
    TASK_FORCES_OF(task)[0].turn.z = TASK_FORCES_OF(task)[1].turn.z = 0;
#ifndef PLATFORM_ONE_PLAYER
    ocean_turn_xz(player_work[1]->pos.x - work->pos.x,
                  player_work[1]->pos.z - work->pos.z, speed, &x, &z);
    TASK_FORCES_OF(task)[1].speed.x = x - player_work[1]->pos.x + work->pos.x;
    TASK_FORCES_OF(task)[1].speed.y = 0.0F;
    TASK_FORCES_OF(task)[1].speed.z = z - player_work[1]->pos.z + work->pos.z;
#endif
    object->ang[1] = ANGLE(task);
    if (player_near((NJS_POINT3 *)object->pos,
                    ((task->work->scl.x + 1.0F) * NEAR_RADIUS) + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
#ifdef PLATFORM_ONE_PLAYER
    collision_register(task);
#endif
    pos = work->pos;
    pos.y += (task->work->scl.x + 1.0F) * SOUND_HEIGHT;
    sound_play_at_for(TURN_SOUND, task, SOUND_PRIORITY, turn_volume,
                      SOUND_DURATION, &pos);
    if (FIRE_SOUND_NEAR(&pos)) {
        sound_play_at_for(FIRE_SOUND, FIRE_SOUND_KEY(task), SOUND_PRIORITY,
                          fire_volume, SOUND_DURATION, &pos);
    }
}

static void turning_platform_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Sint16 texture;

    texture = turning_platform_fire_model->plist[FIRE_TID];
    texture =
        (texture & NJD_FFM_MASK) | (game_state.steps & (OCEAN_FIRE_FRAMES - 1));
    turning_platform_fire_model->plist[FIRE_TID] = texture;
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, ANGLE(task));
    njSetTexture(&turning_platform_texlist);
    /* @hack spelling: task->work read again, where work would do */
    ocean_model_draw_scaled(&turning_platform_model, task->work->scl.x + 1.0F);
    /* the fire faces the screen: its matrix keeps only the platform's place
     * in view space */
    njGetTranslation(NULL, &pos);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&pos);
    njRotateX(NULL, HALF_TURN);
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
    njSetTexture(&ocean_fire_texlist);
    ocean_model_draw_scaled(turning_platform_fire_model,
                            task->work->scl.x + 1.0F);
#ifdef PLATFORM_ONE_PLAYER
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
#endif
    draw_control_restore();
    njPopMatrixEx();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void turning_platform_child_init(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    /* @hack spelling: zero, a local for 0, where 0 in place would do */
    Angle zero;

    work->pos = parent->work->pos;
    work->ang = parent->work->ang;
    work->scl = parent->work->scl;
    task->draw = turning_platform_child_draw;
    task->step = turning_platform_child_step;
    task->destructor = turning_platform_child_destroy;
    zero = 0;
    /* @hack redundant: a goto to the statement after it */
    goto alloc;
alloc:
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies turning_platform_child_object through null where
     * world_pool_alloc has no object to lend, at 0x8c503836 in Sand Ocean
     * and 0x8c501256 in the two-player stage. the destroy tests extra for
     * null, at 0x8c5038c0 and 0x8c5012e0. the step writes the copy's angle
     * through extra untested, at 0x8c50390e and 0x8c50132e */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the
     * child takes this init as its step again, to try on its next step */
    if (object == NULL) {
        task->step = turning_platform_child_init;
        task->draw = NULL;
        task->destructor = NULL;
        return;
    }
#endif
    *object = turning_platform_child_object;
    object->evalflags &= ~EVAL_UNITS;
    /* @hack spelling: task->work read again, where work would do */
    object->scl[0] = object->scl[1] = object->scl[2] = task->work->scl.x + 1.0F;
    object->ang[0] = zero;
    object->ang[1] = ANGLE(parent);
    object->ang[2] = zero;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(CHILD_WORLD_FLAGS, task, object);
}

static void turning_platform_child_destroy(struct task *task)
{
    if (task->extra.p != NULL) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void turning_platform_child_step(struct task *task)
{
    struct task *parent = task->parent;
    struct task_work *work = task->work;
    NJS_OBJECT *object;

    work->pos = parent->work->pos;
    work->ang = parent->work->ang;
    work->scl = parent->work->scl;
    object = task->extra.p;
    object->ang[1] = ANGLE(parent);
    if (parent->work->flags & WORLD_ADDED) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void turning_platform_child_draw(struct task *task)
{
}
