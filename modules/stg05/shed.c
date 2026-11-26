/* the shed, PPSHED in Pumpkin Hill's object list. an attack or a missile
 * breaks it into pieces, with a cloud of dust */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/missile.h"
#include "shared/breakable.h"
#include "stg05/shed.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the set editor's labels for the shed's placement */
static const char shed_edit_angle_y[12] = "<- ANGLE Y";
static const char shed_edit_id[8] = "<- ID";

#include "stg05/shed/text.inc"
#include "stg05/scenery_art/shed.h"

/* the bits of ang.z that hold the shed's id among the missiles' targets */
#define TARGET_ID_BITS 0x1f
#define SHED_SHAPES 1
#define SHED_PIECES 15
/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. a piece falls no lower than the shed's height plus
 * FLOOR_OFFSET */
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.3F
#define FLOOR_OFFSET (-10000.0F)
/* the work's tick counts the break's steps from FIRST_TICK. the pieces fly at
 * BREAK_TICK, and the shed ends past LAST_TICK */
#define FIRST_TICK 1
#define BREAK_TICK 2
#define LAST_TICK 4
#define BREAK_SOUND_KIND 6
/* the dust: DUST_PUFFS puffs about the point DUST_HEIGHT above the shed. a
 * puff's radius, speed and size each take up to their _RANGE more at
 * random */
#define DUST_PUFFS 17
#define DUST_HEIGHT 10.0F
#define DUST_RADIUS 6.0F
#define DUST_RADIUS_RANGE 3.0F
#define DUST_SPEED 1.0F
#define DUST_SPEED_RANGE 0.7F
#define DUST_SIZE 3.0F
#define DUST_SIZE_RANGE 4.0F

NJS_TEXLIST *shed_texlists[] = {&shed_texlist, &shed_piece_0_texlist,
                                &shed_piece_1_texlist, &shed_piece_2_texlist,
                                NULL};
/* clang-format off */
static struct collision_shape shed_shapes[SHED_SHAPES] = {
    {0, SHAPE_SPHERE,
     PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 10.0F, 0.0F}, 15.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* the parts the shed breaks into: five of each of the three pieces' models */
static struct debris_part shed_pieces[SHED_PIECES] = {
    {&shed_piece_1_texlist, &shed_piece_1_model,
     {-3.200062F, 14.741453F, -7.369122F}, {0, 0, 0}},
    {&shed_piece_1_texlist, &shed_piece_1_model,
     {6.534426F, 9.292566F, -0.240035F}, {0, -QUARTER_TURN, 0}},
    {&shed_piece_1_texlist, &shed_piece_1_model,
     {-6.762234F, 9.416843F, -2.080377F}, {0, -0x3d85, 0}},
    {&shed_piece_1_texlist, &shed_piece_1_model,
     {1.982415F, 9.416843F, -6.512598F}, {0, HALF_TURN, 0}},
    {&shed_piece_1_texlist, &shed_piece_1_model,
     {-2.928958F, 9.416843F, 7.14343F}, {0, 0, 0}},
    {&shed_piece_0_texlist, &shed_piece_0_model,
     {-0.24511F, 20.953176F, 3.281919F}, {0x212f, 0x1243, 0x172b}},
    {&shed_piece_0_texlist, &shed_piece_0_model,
     {9.337977F, 18.862003F, 8.967818F}, {0x73cb, -0x3e9e, 0x8c33}},
    {&shed_piece_0_texlist, &shed_piece_0_model,
     {-8.873773F, 18.361115F, 10.144787F}, {0x7f97, 0x78, 0x7ffe}},
    {&shed_piece_0_texlist, &shed_piece_0_model,
     {8.391621F, 19.06237F, -8.857654F}, {0, 0, 0}},
    {&shed_piece_0_texlist, &shed_piece_0_model,
     {-9.337981F, 18.661646F, -8.055005F}, {0x6a5, 0x3d7d, 0x6a2}},
    {&shed_piece_2_texlist, &shed_piece_2_model,
     {3.268361F, 15.220339F, 7.888335F}, {0, HALF_TURN, 0}},
    {&shed_piece_2_texlist, &shed_piece_2_model,
     {-7.512493F, 13.622958F, 2.214056F}, {0, QUARTER_TURN, 0}},
    {&shed_piece_2_texlist, &shed_piece_2_model,
     {7.101854F, 14.368621F, 3.54797F}, {0, QUARTER_TURN, 0}},
    {&shed_piece_2_texlist, &shed_piece_2_model,
     {2.083471F, 9.792274F, 5.974088F}, {0, 0, 0}},
    {&shed_piece_2_texlist, &shed_piece_2_model,
     {-2.54621F, 9.724344F, -5.799391F}, {0, HALF_TURN, 0}},
};
/* clang-format on */

static void shed_destroy(struct task *task);
static void shed_step(struct task *task);
static void shed_draw(struct task *task);

void shed_init(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = shed_draw;
    task->step = shed_step;
    task->destructor = shed_destroy;
    work->submode = 0;
    collision_init(task, shed_shapes, SHED_SHAPES, COLLISION_OBJECT);
    missile_target_add(work->ang.z & TARGET_ID_BITS, &work->hit);
    work->tick = 0;
}

static void shed_destroy(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    /* @bug memory: a missile of the shed's id can write into the shed's freed
     * work. shed_init adds the target at 0x8c503498 even where
     * collision_init gave the work no record. the test at 0x8c5034ac then
     * keeps the destructor from removing it */
#ifdef NONMATCHING
    /* this side repairs the tag above: the shed removes its target whether or
     * not it has a record, as shed_init adds the target either way */
    missile_target_remove(&work->hit);
#else
    if (work->collision != NULL) {
        missile_target_remove(&work->hit);
    }
#endif
}

static void shed_step(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->tick) {
        pos = work->pos;
        /* @hack redundant: the += 0.0F, which changes no height */
        pos.y += 0.0F;
        spd.x = 0.0F;
        spd.y = PIECE_LIFT;
        spd.z = 0.0F;
        if (work->tick == BREAK_TICK) {
            debris_break(&pos, work->ang.y, shed_pieces, SHED_PIECES,
                         PIECE_SPEED, PIECE_SPREAD, work->pos.y + FLOOR_OFFSET,
                         &spd);
        }
        object_sound_play(BREAK_SOUND_KIND, &work->pos);
        if (work->tick > LAST_TICK) {
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        }
        work->tick++;
        return;
    }
    if ((work->flags & WORK_HURT) || work->hit != NULL) {
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
    }
    collision_register(task);
}

static void shed_draw(struct task *task)
{
    struct breakable_work *work = BREAKABLE(task);

    njSetTexture(&shed_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&shed_model);
    njPopMatrix(1);
}
