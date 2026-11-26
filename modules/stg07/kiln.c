/* the kiln, MIKILN in Aquatic Mine's object list. Knuckles's or Rouge's attack
 * swings its door open */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg07/kiln.h"
#include "system/task.h"

#include "stg07/kiln/text.inc"

#include "stg07/scenery/kiln.h"

/* the copy of the collision object that the world holds, in task->motion */
#define WORLD_OBJECT(task) ((NJS_OBJECT *)(task)->motion.p)

#define KILN_SHAPES 3
/* the door's shape, which stops colliding as the door opens */
#define DOOR_SHAPE 0

/* the kiln's phases, in its work's submode. MODE_STRUCK lasts STRUCK_STEPS + 1
 * steps. in MODE_OPEN the work's timer is the door's angle, which grows by
 * door_speed a step up to DOOR_OPEN */
enum kiln_mode { MODE_SHUT, MODE_STRUCK, MODE_OPEN };
#define STRUCK_STEPS 8
#define DOOR_OPEN 0x4200

/* the copy collides while a player lies within the model's radius plus
 * REACH_MARGIN of its pos. the same sum gives its size class */
#define REACH_MARGIN 15.0F

NJS_TEXLIST *kiln_texlists[2] = {
#include "stg07/kiln/kiln_texlists.inc"
};
/* the door's swing each step, in angle units */
static Sint32 door_speed = 0x100;
/* the door, which hurts players and which an attack can hurt, and two
 * capsules above it */
/* clang-format off */
static struct collision_shape kiln_shapes[KILN_SHAPES] = {
    {0, SHAPE_BOX,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 6.0F, 18.0F}, 11.0F, 6.0F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 37.5F, 4.0F}, 5.0F, 10.5F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CAPSULE,
     PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0, 0,
     {0.0F, 26.0F, 4.0F}, 14.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static void kiln_destroy(struct task *task);
static void kiln_step(struct task *task);
static void kiln_draw(struct task *task);

void kiln_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = kiln_draw;
    task->step = kiln_step;
    task->destructor = kiln_destroy;
    work->submode = MODE_SHUT;
    work->timer = 0;
    object = task->motion.p;
    *object = kiln_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_NO_CLIMB;
    radius = object->model->r + REACH_MARGIN;
    if (radius <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    collision_init(task, kiln_shapes, KILN_SHAPES, COLLISION_OBJECT);
}

static void kiln_destroy(struct task *task)
{
    if (WORLD_OBJECT(task)->model != NULL) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void kiln_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    int slot;
    int character;
    int mode;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->submode) {
    case MODE_OPEN:
        work->timer += door_speed;
        if (work->timer > DOOR_OPEN) {
            work->timer = DOOR_OPEN;
        }
        break;
    case MODE_STRUCK:
        if (work->timer-- == 0) {
            work->submode = MODE_OPEN;
            work->timer = 0;
            work->collision->shapes[DOOR_SHAPE].flags |= SHAPE_OFF;
        }
        break;
    default:
        if (work->flags & WORK_HURT) {
            collision_restart_scans();
            while ((hit = collision_next_player(task)) != NULL) {
                slot = player_slot_of(hit);
                /* @hack spelling: character, a local for the hit's
                 * character, where the call in the switch would do */
                character = player_character_of(hit);
                switch (character) {
                case PLAYER_KNUCKLES:
                case PLAYER_ROUGE:
                    mode = player_work[slot]->mode;
                    if (mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
                        mode <= PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
                        work->submode = MODE_STRUCK;
                        work->timer = STRUCK_STEPS;
                    }
                    break;
                }
            }
        }
        break;
    }
    collision_register(task);
    if (WORLD_OBJECT(task)->model != NULL) {
        object = task->motion.p;
        /* @hack spelling: the pos read through task->motion again, where
         * object would do */
        if (player_near((NJS_POINT3 *)WORLD_OBJECT(task)->pos,
                        object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void kiln_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&kiln_texlist);
    object = &kiln_object;
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateX(NULL, work->timer);
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}
