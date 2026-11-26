/* FIREBALL in the object list of the second fight of Sonic and Shadow. it is
 * a burning rock that breaks once a player comes near */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camset.h"
#include "camera/zones.h"
#include "collision/collision.h"
#include "effect/burst.h"
#include "effect/particle.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg42/fireball.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

#include "stg42/fireball/text.inc"

static NJS_TEXNAME meteor_rock_0_textures[1] = {
#include "stg42/fireball/meteor_rock_0_textures.inc"
};

static NJS_TEXLIST meteor_rock_0_texlist = {
#include "stg42/fireball/meteor_rock_0_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_0.h"

static NJS_CNK_OBJECT meteor_rock_0 = {
#include "stg42/fireball/meteor_rock_0.inc"
};

static NJS_TEXNAME meteor_rock_1_textures[1] = {
#include "stg42/fireball/meteor_rock_1_textures.inc"
};

static NJS_TEXLIST meteor_rock_1_texlist = {
#include "stg42/fireball/meteor_rock_1_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_1.h"

static NJS_CNK_OBJECT meteor_rock_1 = {
#include "stg42/fireball/meteor_rock_1.inc"
};

static NJS_TEXNAME meteor_rock_2_textures[2] = {
#include "stg42/fireball/meteor_rock_2_textures.inc"
};

static NJS_TEXLIST meteor_rock_2_texlist = {
#include "stg42/fireball/meteor_rock_2_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_2.h"

static NJS_CNK_OBJECT meteor_rock_2 = {
#include "stg42/fireball/meteor_rock_2.inc"
};

static NJS_TEXNAME meteor_rock_3_textures[2] = {
#include "stg42/fireball/meteor_rock_3_textures.inc"
};

static NJS_TEXLIST meteor_rock_3_texlist = {
#include "stg42/fireball/meteor_rock_3_texlist.inc"
};

#include "shared/meteor_shapes/meteor_rock_3.h"

static NJS_CNK_OBJECT meteor_rock_3 = {
#include "stg42/fireball/meteor_rock_3.inc"
};

static NJS_TEXNAME fireball_textures[1] = {
#include "stg42/fireball/fireball_textures.inc"
};

static NJS_TEXLIST fireball_texlist = {
#include "stg42/fireball/fireball_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fireball.h"

static NJS_CNK_OBJECT fireball_object[1] = {
#include "stg42/fireball/fireball_object.inc"
};

static NJS_TEXNAME fireball_piece_2_textures[2] = {
#include "stg42/fireball/fireball_piece_2_textures.inc"
};

static NJS_TEXLIST fireball_piece_2_texlist = {
#include "stg42/fireball/fireball_piece_2_texlist.inc"
};

#include "shared/meteor_shapes/meteor_fragment.h"

static NJS_CNK_OBJECT fireball_piece_2_object[1] = {
#include "stg42/fireball/fireball_piece_2_object.inc"
};

static NJS_TEXNAME fireball_piece_1_textures[2] = {
#include "stg42/fireball/fireball_piece_1_textures.inc"
};

static NJS_TEXLIST fireball_piece_1_texlist = {
#include "stg42/fireball/fireball_piece_1_texlist.inc"
};

#include "shared/meteor_shapes/meteor_shard.h"

static NJS_CNK_OBJECT fireball_piece_1_object[1] = {
#include "stg42/fireball/fireball_piece_1_object.inc"
};

NJS_TEXLIST *fireball_texture_group[8] = {
#include "stg42/fireball/fireball_texture_group.inc"
};

static NJS_CNK_OBJECT *meteor_rocks[4] = {
#include "stg42/fireball/meteor_rocks.inc"
};

/* fireball_init's camera zone, kept in the task's unk_3c. it keeps the
 * camera out of a sphere of ZONE_RADIUS about pos */
#define ZONE(task) ((task)->unk_3c.p)
#define ZONE_RADIUS 55.0F

/* a fireball's timer starts once a player comes within scl.x plus
 * BREAK_NEAR. the fireball breaks once the timer passes BREAK_WAIT */
#define BREAK_WAIT 60
#define BREAK_NEAR 30.0F
/* the work's submode once the fireball breaks. nothing reads it */
#define BROKEN 1
/* the break throws PIECES pieces, the first SHARDS of them shards. each
 * piece's speed in y gains PIECE_LIFT after the speed's turn */
#define PIECES 20
#define SHARDS 5
#define PIECE_LIFT 0.5F
#define PIECE_FLOOR (-100000.0F)
/* rand's 0 to RAND_MAX, times PIECE_TURN, spans two turns */
#define PIECE_TURN 4.0F
/* the chance that a step sheds a spark */
#define SPARK_CHANCE 0.2F

/* while its timer runs, the fireball blinks from BLINK_COLOR toward
 * BLINK_RED. the blink turns BLINK_RATE Angle units a step of the timer */
#define BLINK_RATE 0x500
#define BLINK_COLOR 0xffffffff
#define BLINK_RED 0xffff2020

static void fireball_destroy(struct task *task);
static void fireball_draw(struct task *task);
static void fireball_step(struct task *task);

/* clang-format off */
static struct collision_shape fireball_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(2) | SHAPE_GUARD_LEVEL(1) | SHAPE_HURT |
        SHAPE_HURTS_PLAYERS,
    SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 32.0F, 0.0F, 0.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Uint32 dat_8c5366d0 = 0xff0070ff;
static Uint32 spark_color = 0xff3020ff;
static Float spark_size = 20.0F;
static Float spark_size_range = 30.0F;
/* the fireball's spin about x and about y, in degrees a step */
static Float spin_x = 0.36F;
static Float spin_y = 0.45000002F;

void fireball_init(struct task *task)
{
    struct zone_shape shape;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. they are the SET editor's
     * captions */
    if (0) {
        (void)"<- START RANGE";
        (void)"<- ROTSPD X";
        (void)"<- ROTSPD Y";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->step = fireball_step;
    task->draw = fireball_draw;
    task->destructor = fireball_destroy;
    collision_init(task, &fireball_shape, 1, COLLISION_OBJECT);
    shape.shape = ZONE_SPHERE;
    shape.pos = task->work->pos;
    shape.ang.x = 0;
    shape.ang.y = 0;
    shape.ang.z = 0;
    /* a sphere zone's radius is scl.x, and its tests read the radius
     * squared from scl.z. none reads scl.y, which stays unset */
    shape.scl.x = ZONE_RADIUS;
    shape.scl.z = ZONE_RADIUS * ZONE_RADIUS;
    ZONE(task) = camset_add_marked(&shape);
}

static void fireball_destroy(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;

    if (ZONE(self)) {
        camset_free_zone(ZONE(self));
    }
}

static void fireball_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    /* @hack spelling: color, a local for the blended color, where the blend
     * in the call would do */
    NJS_COLOR color;
    Float blink;

    /* @hack redundant: a goto to the statement after it */
    goto draw;
draw:
    njSetTexture(&fireball_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, 0);
    if (work->timer) {
        blink = njSin(work->timer * BLINK_RATE);
        if (blink < 0.0F) {
            blink = 0.0F;
        }
        color.color = color_blend(BLINK_COLOR, BLINK_RED, blink);
        model_draw_tinted(fireball_object->model, color.color);
    } else {
        njCnkEasyDrawModel(fireball_object->model);
    }
    njPopMatrixEx();
}

static void fireball_step(struct task *task)
{
    /* @hack spelling: self, a copy of task, where task would do */
    struct task *self = task;
    struct task_work *work = self->work;
    NJS_POINT3 pos;
    NJS_POINT3 world_pos;
    NJS_VECTOR spd;
    NJS_VECTOR world_spd;
    struct angles ang;
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    struct particle *spark;
    Float roll;
    int i;

    if (placement_try_leave(self)) {
        return;
    }
    if (collision_next_player(self) || work->timer > BREAK_WAIT) {
        work->submode = BROKEN;
        pos.y = work->collision->shapes->radius;
        pos.x = 0.0F;
        pos.z = 0.0F;
        spd.x = 0.0F;
        spd.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njPushMatrixEx();
        /* @hack redundant: a test of the first piece's object no path
         * reaches */
        if (0) {
            /* NOLINTNEXTLINE(clang-diagnostic-pointer-bool-conversion) */
            if (fireball_piece_1_object) {
            }
        }
        for (i = 0; i < PIECES; i++) {
            roll = RANDOM_UNIT();
            spd.y = (roll * 2.0F) + 1.0F;
            pos.y = work->collision->shapes->radius * (roll + 1.0F) * 0.5F;
            ang.y = 0;
            ang.z = (Angle)(rand() * PIECE_TURN);
            njRotateZ(NULL, ang.z);
            ang.x = (Angle)(rand() * PIECE_TURN);
            njRotateX(NULL, ang.x);
            njCalcPoint(NULL, &pos, &world_pos);
            njCalcVector(NULL, &spd, &world_spd);
            if (i < SHARDS) {
                model = fireball_piece_1_object->model;
                texlist = &fireball_piece_1_texlist;
            } else {
                model = fireball_piece_2_object->model;
                texlist = &fireball_piece_2_texlist;
            }
            world_spd.y += PIECE_LIFT;
            debris_launch_built_flags(&world_pos, &ang, PIECE_FLOOR, &world_spd,
                                      model, NULL, texlist, DEBRIS_FADE_COLORS);
            matrix_load_parent();
        }
        njPopMatrix(2);
        placement_kill(self);
        return;
    }
    collision_register(self);
    if (work->timer) {
        work->timer++;
    } else if (player_near(&work->pos, work->scl.x + BREAK_NEAR)) {
        work->timer = 1;
    }
    work->ang.x += DEGREES(spin_x);
    work->ang.y += DEGREES(spin_y);
    if (RANDOM_UNIT() < SPARK_CHANCE) {
        /* @hack spelling: the spark's speed kept in world_pos, where a
         * vector of its own would do */
        world_pos.x = (RANDOM_UNIT() - 0.5F) * 2.0F;
        world_pos.y = RANDOM_UNIT() - 0.2F;
        world_pos.z = (RANDOM_UNIT() - 0.5F) * 2.0F;
        pos = work->pos;
        spark = burst_create_fast(
            &pos, &world_pos,
            (rand() * spark_size_range * RAND_SCALE) + spark_size, 0.0F);
        if (spark) {
            spark->color = spark_color;
        }
    }
}
