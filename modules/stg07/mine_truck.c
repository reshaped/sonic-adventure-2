/* the mine truck, MITRUCK in Aquatic Mine's object list. an attack breaks it
 * into pieces, with a cloud of dust */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg07/mine_truck.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#include "stg07/mine_truck/text.inc"

#include "stg07/scenery/mine_truck.h"

/* the truck's record, in task->motion */
struct mine_truck {
    NJS_OBJECT object; /* the copy of the collision object the world holds */
    NJS_POINT3 center; /* the collision model's center, in the world */
    Uint8 unk_40[8];
};

#define TRUCK(task) ((struct mine_truck *)(task)->motion.p)

#define MINE_TRUCK_SHAPES 1
#define MINE_TRUCK_PIECES 7
/* the copy collides while a player lies within the model's radius plus
 * REACH_MARGIN of its center. the same sum gives its size class */
#define REACH_MARGIN 15.0F
/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. a piece falls no lower than the truck's height plus
 * FLOOR_OFFSET */
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.4F
#define FLOOR_OFFSET (-10000.0F)
/* the work's tick counts the break's steps from FIRST_TICK. the pieces fly at
 * BREAK_TICK, and the truck ends past LAST_TICK */
#define FIRST_TICK 1
#define BREAK_TICK 2
#define LAST_TICK 4
#define BREAK_POINTS 20
#define BREAK_SOUND 5
/* the dust: DUST_PUFFS puffs about the point DUST_HEIGHT above the truck. at
 * random, a puff's radius takes up to DUST_RADIUS_RANGE more. its speed takes
 * up to DUST_SPEED_RANGE more, and its size up to DUST_SIZE_RANGE more */
#define DUST_PUFFS 17
#define DUST_HEIGHT 10.0F
#define DUST_RADIUS 6.0F
#define DUST_RADIUS_RANGE 3.0F
#define DUST_SPEED 1.0F
#define DUST_SPEED_RANGE 0.7F
#define DUST_SIZE 3.0F
#define DUST_SIZE_RANGE 4.0F

NJS_TEXLIST *mine_truck_texlists[5] = {
#include "stg07/mine_truck/mine_truck_texlists.inc"
};
static struct debris_part mine_truck_pieces[MINE_TRUCK_PIECES] = {
#include "stg07/mine_truck/mine_truck_pieces.inc"
};
static struct collision_shape mine_truck_shapes[MINE_TRUCK_SHAPES] = {
#include "stg07/mine_truck/mine_truck_shapes.inc"
};

static void mine_truck_destroy(struct task *task);
static void mine_truck_step(struct task *task);
static void mine_truck_draw(struct task *task);

void mine_truck_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;
    Uint32 flags;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct mine_truck));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = mine_truck_draw;
    task->step = mine_truck_step;
    task->destructor = mine_truck_destroy;
    work->submode = 0;
    object = task->motion.p;
    *object = mine_truck_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = work->ang.y;
    object->ang[2] = work->ang.z;
    object->ang[0] = work->ang.x;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, (NJS_POINT3 *)object->pos, (NJS_POINT3 *)object->pos);
    njPopMatrix(1);
    flags = WORLD_SOLID;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, (NJS_VECTOR *)object->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoint(NULL, &object->model->center, &TRUCK(task)->center);
    njPopMatrix(1);
    collision_init(task, mine_truck_shapes, MINE_TRUCK_SHAPES,
                   COLLISION_OBJECT);
}

static void mine_truck_destroy(struct task *task)
{
    if (TRUCK(task)->object.model != NULL) {
        world_remove(task, task->motion.p);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void mine_truck_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    struct collision_work *collision;
    /* @hack type: the truck's record held in a struct of one member, where
     * a plain pointer would do */
    struct {
        struct mine_truck *truck;
    } held;
    NJS_OBJECT *object;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->tick) {
        pos = work->pos;
        /* @hack redundant: a height of 0 added to pos.y */
        pos.y += 0.0F;
        spd.x = 0.0F;
        spd.y = PIECE_LIFT;
        spd.z = 0.0F;
        if (work->tick == BREAK_TICK) {
            debris_break(&pos, work->ang.y, mine_truck_pieces,
                         MINE_TRUCK_PIECES, PIECE_SPEED, PIECE_SPREAD,
                         work->pos.y + FLOOR_OFFSET, &spd);
        }
        if (work->tick > LAST_TICK) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
        work->tick++;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
    } else if (((collision = work->collision)->flags & COLLISION_TOUCHED) &&
               player_attacks_task(task, collision->hit->owner)) {
        pos = work->pos;
        pos.y += DUST_HEIGHT;
        for (i = 0; i < DUST_PUFFS; i++) {
            dust_short_sphere(
                &pos,
                DUST_RADIUS +
                    ((Float)rand() * (DUST_RADIUS_RANGE * RAND_SCALE)),
                DUST_SPEED + ((Float)rand() * (DUST_SPEED_RANGE * RAND_SCALE)),
                DUST_SIZE + ((Float)rand() * (DUST_SIZE_RANGE * RAND_SCALE)));
        }
        work->tick = FIRST_TICK;
        enemy_bounce_player(0);
        score_add(BREAK_POINTS);
        sound_play(BREAK_SOUND, NULL, 0, 0);
    } else {
        collision_register(task);
    }
    if (TRUCK(task)->object.model != NULL) {
        /* @hack spelling: the assignment to held.truck inside the address,
         * where a statement of its own would do */
        object = &(held.truck = task->motion.p)->object;
        if (player_near(&held.truck->center, object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void mine_truck_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)MINE_TRUCK_EDITOR_ANGLE_X;
        (void)MINE_TRUCK_EDITOR_ANGLE_Y;
        (void)MINE_TRUCK_EDITOR_ANGLE_Z;
    }
    njSetTexture(&mine_truck_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkEasyDrawModel(&mine_truck_model);
    njPopMatrix(1);
}
