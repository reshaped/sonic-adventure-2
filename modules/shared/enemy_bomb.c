/* the bombs the enemies drop and fire */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_bomb/text.inc"
#include "shared/blast.h"
#include "shared/cage.h"
#include "shared/enemy_bomb.h"
#include "shared/ring.h"
#include "shared/spark.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/snapshot.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a bomb's work->mode */
enum bomb_mode {
    MODE_FALL,
    MODE_LIE, /* on the floor, until COUNT runs out */
    MODE_BLAST,
    MODE_FIRE, /* a fire bomb's blast */
    MODE_HELD, /* until WORK_HELD_BY_PLAYER clears */
    MODE_DONE /* the blast is over, and the step kills the bomb */
};

/* a bomb's own fields of its work and its enemy_work. scl.x and scl.y scale
 * the bomb's draw, until a burst takes them for RADIUS and LINGER */
#define RADIUS(work) ((work)->scl.x)
#define LINGER(work) ((work)->scl.y)
/* a bomb's fuse, from FUSE_LENGTH down by FUSE_STEP a step. a lying bomb's
 * stops at 0. the only read of FUSE is the place of a spark that nothing
 * makes */
#define FUSE(work) ((work)->scl.z)
/* a lying bomb's count, from LIE_COUNT down. the timer starts again at each
 * value, and each value n lasts n + 1 steps. the bomb bursts in the step
 * COUNT reaches 0 */
#define COUNT(work) ((work)->tick)
/* a heavy bomb's frame of heavy_bomb_anim */
#define FRAME(enemy) ((enemy)->unk_04e)
/* a heavy bomb's scale, from HEAVY_SCALE_START up to HEAVY_SCALE by
 * HEAVY_SCALE_GROWTH a step. any other bomb's is 1 */
#define SCALE(enemy) ((enemy)->unk_094)
/* what each step adds to spd.y, before enemy_bomb_move zeroes it. it stays 0,
 * since nothing sets it */
#define ACCEL(enemy) ((enemy)->unk_00c)

/* the indexes of a bomb's shapes. a burst turns OUTER_SHAPE off and
 * BLAST_SHAPE on. a burst into MODE_FIRE turns FIRE_SHAPE on too */
#define OUTER_SHAPE 0
#define BLAST_SHAPE 1
#define BODY_SHAPE 2
#define FIRE_SHAPE 3
/* a fire bomb's record holds every shape, and any other bomb's all but
 * FIRE_SHAPE */
#define BOMB_SHAPES 3
#define FIRE_BOMB_SHAPES 4
/* the radius of a bomb's record. it covers the largest blast whose RADIUS
 * starts at BLAST_START */
#define RECORD_RADIUS 40.0F

/* a blast's RADIUS grows from BLAST_START by BLAST_STEP a step. it grows
 * while it is below BLAST_END, or below HEAVY_BLAST_END in a heavy bomb.
 * BLAST_SHAPE grows by the same steps, and FIRE_SHAPE too in MODE_FIRE.
 * LINGER then falls from 0 by LINGER_STEP a step, and the blast ends once it
 * is below LINGER_END */
#define BLAST_START 5.0F
#define BLAST_STEP 1.0F
#define BLAST_END 25.0F
#define HEAVY_BLAST_END 37.5F
#define LINGER_STEP 0.05F
#define LINGER_END (-1.0F)
#define BLAST_SPIN 0x100 /* how far a blast turns ang.y each step */

/* a falling bomb bursts at a hit only once its timer passes ARMED_STEPS */
#define ARMED_STEPS 20
#define GRAVITY (-0.06F)
/* a falling normal or heavy bomb on the floor squashes where spd.y is past
 * SQUASH_SPEED either way. the squash sets squash_spd to SQUASH_SPRING */
#define SQUASH_SPEED 0.3F
#define SQUASH_SPRING 0.4F
/* a falling bomb on the floor lies once spd.y is within LIE_SPEED of 0 */
#define LIE_SPEED 0.1F
/* a lying bomb on the floor holds still while its speed is below REST_SPEED */
#define REST_SPEED 0.05F
#define LIE_COUNT 15
#define FUSE_LENGTH 1.0F
#define FUSE_STEP 0.003F

#define HEAVY_SCALE_START 0.5F
#define HEAVY_SCALE 2.0F
#define HEAVY_SCALE_GROWTH 0.2F
/* a heavy bomb's BODY_SHAPE grows with it, from a radius and a size_y of 0.
 * each step adds the _GROWTH values, up to HEAVY_BODY_RADIUS and
 * HEAVY_BODY_SIZE_Y */
#define HEAVY_BODY_RADIUS 9.0F
#define HEAVY_BODY_GROWTH 0.45F
#define HEAVY_BODY_SIZE_Y 10.0F
#define HEAVY_BODY_SIZE_Y_GROWTH 0.5F
/* a heavy bomb's bottom lies HEAVY_DEPTH below its pos */
#define HEAVY_DEPTH 10.0F
/* in MODE_HELD, scl.x and scl.y move toward 1 by HELD_GROWTH a step */
#define HELD_GROWTH 0.1F

/* a thrown bomb starts THROW_RISE up. along its heading it starts at
 * THROW_SPEED_PER_DISTANCE times the nearest player's distance, up to
 * THROW_SPEED */
#define THROW_RISE 2.0F
#define THROW_SPEED 1.2F
#define THROW_SPEED_PER_DISTANCE 0.013F

/* a lying bomb flashes red and unlit at the start of each count. it flashes
 * while its timer is FLASH_STEPS or less */
#define FLASH_STEPS 2
/* a fire bomb's blast draws a column of fire FIRE_HEIGHT high, standing on
 * its pos. the column's radius is RADIUS */
#define FIRE_HEIGHT 10.0F

/* a burst's puff size, sparks, scraps, sound and vibration. a heavy bomb's
 * burst takes the HEAVY_ values in their place. only a one-player game
 * vibrates, and only player 0's pad */
#define BLAST_PUFF 15.0F
#define BLAST_SPARKS 48
#define BLAST_SCRAPS 16
#define BLAST_SOUND 0x4019
#define BLAST_VOLUME 90
#define BLAST_VIBRATION_POWER 3
#define BLAST_VIBRATION_FREQ 7
#define HEAVY_PUFF 25.0F
#define HEAVY_SPARKS 80
#define HEAVY_SCRAPS 48
#define HEAVY_SOUND 0x401a
#define HEAVY_VIBRATION_POWER 5
#define HEAVY_VIBRATION_FREQ 15
/* a fire bomb's burst plays FIRE_SOUND beside BLAST_SOUND */
#define FIRE_SOUND 0x401b
#define FIRE_VOLUME 10
/* the sound a bomb plays as it starts, unless it is thrown */
#define START_SOUND 0x4004
#define START_VOLUME 0

/* a BOMB_RING comes up a ring where a random fraction is RING_ODDS or more.
 * the ring starts RING_DROP below the bomb's pos */
#define RING_ODDS 0.95F
#define RING_DROP 10.0F

/* the task levels of a bomb without a parent, of a scattered ring, and of
 * the textures' task */
#define BOMB_LEVEL 3
#define RING_LEVEL 2
#define TEXTURE_LEVEL 0
/* the index in lights of the light the bombs draw in */
#define BOMB_LIGHT 8
#define TURN_SPEED 0xe0
#define SHADOW_SIZE 2.0F
#define HEAVY_SHADOW_SIZE 14.0F
#define SHADOW_RATIO 1.0F

static NJS_TEXNAME enemy_bomb_texnames[3] = {
#include "shared/enemy_bomb/enemy_bomb_texnames.inc"
};
NJS_TEXLIST enemy_bomb_texlist = {
#include "shared/enemy_bomb/enemy_bomb_texlist.inc"
};
/* the heavy bomb's model */
#define HEAVY_BOMB_POINTS 34
static Sint16 heavy_bomb_polygons[281] = {
#include "shared/enemy_bomb/heavy_bomb_polygons.inc"
};
static Sint32 heavy_bomb_vertices[207] = {
#include "shared/enemy_bomb/heavy_bomb_vertices.inc"
};
static NJS_CNK_MODEL heavy_bomb_model = {
#include "shared/enemy_bomb/heavy_bomb_model.inc"
};
static NJS_CNK_OBJECT heavy_bomb_object = {
#include "shared/enemy_bomb/heavy_bomb_object.inc"
};
/* the normal bomb's model */
#define NORMAL_BOMB_POINTS 20
static Sint16 normal_bomb_polygons[165] = {
#include "shared/enemy_bomb/normal_bomb_polygons.inc"
};
static Sint32 normal_bomb_vertices[123] = {
#include "shared/enemy_bomb/normal_bomb_vertices.inc"
};
NJS_CNK_MODEL enemy_bomb_normal_model = {
#include "shared/enemy_bomb/enemy_bomb_normal_model.inc"
};
static NJS_CNK_OBJECT normal_bomb_object = {
#include "shared/enemy_bomb/normal_bomb_object.inc"
};
/* the model of a BOMB_RED_ONE and of a fire bomb */
#define RED_BOMB_POINTS 20
static Sint16 red_bomb_polygons[165] = {
#include "shared/enemy_bomb/red_bomb_polygons.inc"
};
static Sint32 red_bomb_vertices[123] = {
#include "shared/enemy_bomb/red_bomb_vertices.inc"
};
static NJS_CNK_MODEL red_bomb_model = {
#include "shared/enemy_bomb/red_bomb_model.inc"
};
static NJS_CNK_OBJECT red_bomb_object = {
#include "shared/enemy_bomb/red_bomb_object.inc"
};
/* a heavy bomb's texture scroll, at frame FRAME. each frame moves the u of
 * the second strip's 18 vertices by -20. that step is the high half of
 * heavy_bomb_anim's frames */
static Sint16 heavy_bomb_uvs[40] = {
#include "shared/enemy_bomb/heavy_bomb_uvs.inc"
};
static struct texture_anim heavy_bomb_anim = {
#include "shared/enemy_bomb/heavy_bomb_anim.inc"
};

/* a normal bomb's shapes, and a BOMB_RED_ONE's */
static struct collision_shape enemy_bomb_shapes[BOMB_SHAPES] = {
#include "shared/enemy_bomb/enemy_bomb_shapes.inc"
};
static struct collision_shape heavy_bomb_shapes[BOMB_SHAPES] = {
#include "shared/enemy_bomb/heavy_bomb_shapes.inc"
};
static struct collision_shape fire_bomb_shapes[FIRE_BOMB_SHAPES] = {
#include "shared/enemy_bomb/fire_bomb_shapes.inc"
};

/* the task that keeps enemy_bomb_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *enemy_bomb_texture_task = NULL;

static void enemy_bomb_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.y += ACCEL(enemy).y + GRAVITY;
    njAddVector(&work->pos, &enemy->spd);
    ACCEL(enemy).x = 0.0F;
    ACCEL(enemy).y = 0.0F;
    ACCEL(enemy).z = 0.0F;
}

/* make the effects of the bomb's burst. the caller sets its mode and
 * shapes */
static void enemy_bomb_burst(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (work->submode == BOMB_HEAVY) {
        if (enemy->flags & ENEMY_ON_FLOOR) {
            pos = work->pos;
            pos.y -= HEAVY_DEPTH;
            blast_create(BLAST_HUGE_RAISED, &pos);
        } else {
            blast_create(BLAST_HUGE, &work->pos);
        }
        spd.x = 0.0F;
        spd.y = 0.0F;
        spd.z = 0.0F;
        burst_create_wobbling(&work->pos, &spd, HEAVY_PUFF);
        spark_throw(HEAVY_SPARKS, &work->pos, NULL);
        scraps_throw(HEAVY_SCRAPS, &work->pos, &work->ang);
        sound_play_at(HEAVY_SOUND, NULL, 0, SOUND_LOUDEST, &work->pos);
        if (!game_state.two_players) {
            vibration_start(0, HEAVY_VIBRATION_POWER, HEAVY_VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
        }
    } else {
        if (enemy->flags & ENEMY_ON_FLOOR) {
            blast_create(BLAST_LARGE_RAISED, &work->pos);
        } else {
            blast_create(BLAST_LARGE, &work->pos);
        }
        /* @hack spelling: pos holds the puff's zero speed, where spd would
         * do */
        pos.x = 0.0F;
        pos.y = 0.0F;
        pos.z = 0.0F;
        burst_create_wobbling(&work->pos, &pos, BLAST_PUFF);
        spark_throw(BLAST_SPARKS, &work->pos, NULL);
        scraps_throw(BLAST_SCRAPS, &work->pos, &work->ang);
        sound_play_at(BLAST_SOUND, NULL, 0, BLAST_VOLUME, &work->pos);
        if (work->submode == BOMB_FIRE) {
            sound_play_at(FIRE_SOUND, NULL, 0, FIRE_VOLUME, &work->pos);
        }
        if (!game_state.two_players) {
            vibration_start(0, BLAST_VIBRATION_POWER, BLAST_VIBRATION_FREQ,
                            VIBRATION_UNTIMED);
        }
    }
}

static void enemy_bomb_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    Float scl_x = work->scl.x;
    Float scl_y = work->scl.y;

    light_apply(BOMB_LIGHT);
    njSetTexture(&enemy_bomb_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->submode == BOMB_HEAVY) {
        enemy = task->motion.p;
        scl_x *= SCALE(enemy);
        scl_y *= SCALE(enemy);
        texture_anim_apply(&heavy_bomb_model, &heavy_bomb_anim, FRAME(enemy));
        njScale(NULL, scl_x, scl_y, scl_x);
        njCnkSimpleDrawModel(&heavy_bomb_model);
    } else {
        njScale(NULL, scl_x, scl_y, scl_x);
        njCnkSimpleDrawModel(&enemy_bomb_normal_model);
    }
    njPopMatrixEx();
    light_apply(light_after_draw);
    /* @hack redundant: a return as the last statement */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* draw a BOMB_RED_ONE or a fire bomb */
static void enemy_bomb_draw_red(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: scl_x and scl_y, locals for scl.x and scl.y, where
     * the fields read in place would do */
    Float scl_x = work->scl.x;
    Float scl_y = work->scl.y;

    light_apply(BOMB_LIGHT);
    njSetTexture(&enemy_bomb_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njScale(NULL, scl_x, scl_y, scl_x);
    njCnkSimpleDrawModel(&red_bomb_model);
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void enemy_bomb_draw_ring(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: scl_x and scl_y, locals for scl.x and scl.y, where
     * the fields read in place would do */
    Float scl_x = work->scl.x;
    Float scl_y = work->scl.y;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njScale(NULL, scl_x, scl_y, scl_x);
    ring_draw_model();
    njPopMatrixEx();
}

/* burst the bomb on WORK_HURT, or a BOMB_RED_ONE or a fire bomb on
 * WORK_UNK_1 or WORK_UNK_8 too. a held bomb under WORK_UNK_8 also bursts
 * where it last touched an item or an enemy. it must have touched a record
 * this step, unchecked */
static void enemy_bomb_burst_if_hit(struct task_work *work,
                                    struct enemy_work *enemy)
{
    /* @hack spelling: collision, a local for the work's record, where
     * work->collision read in place would do */
    struct collision_work *collision = work->collision;
    /* @hack spelling: hit, a flag the tests set and the next test compares
     * with 1, where hit alone would do */
    int hit = 0;

    if (work->submode == BOMB_RED_ONE || work->submode == BOMB_FIRE) {
        if (work->flags & (WORK_UNK_1 | WORK_HURT | WORK_UNK_8)) {
            hit = 1;
        }
    } else if (work->flags & WORK_HURT) {
        hit = 1;
    }
    if (hit == 1) {
        RADIUS(work) = BLAST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[OUTER_SHAPE].flags |= SHAPE_OFF;
        enemy_bomb_burst(work, enemy);
        if (work->submode == BOMB_FIRE) {
            work->collision->shapes[FIRE_SHAPE].flags &= ~SHAPE_OFF;
            work->mode = MODE_FIRE;
        } else {
            work->mode = MODE_BLAST;
        }
        work->timer = 0;
    }
    if ((work->flags & WORK_HELD_BY_PLAYER) && (work->flags & WORK_UNK_8)) {
        if (collision->hit->kind == COLLISION_ITEM ||
            collision->hit->kind == COLLISION_ENEMY) {
            RADIUS(work) = BLAST_START;
            LINGER(work) = 0.0F;
            work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
            work->collision->shapes[OUTER_SHAPE].flags |= SHAPE_OFF;
            enemy_bomb_burst(work, enemy);
            work->mode = MODE_BLAST;
            work->timer = 0;
        }
    }
}

static void enemy_bomb_fall(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    /* @bug logic: a burst here doesn't end the fall step, so
     * enemy_squash_step then writes its scales over RADIUS and LINGER. RADIUS
     * then starts below BLAST_START, so BLAST_SHAPE grows for more steps and
     * ends larger. LINGER starts above 0, so the blast lasts longer. a
     * BOMB_RED_ONE or a fire bomb that lands in that step sets the two anew
     * and bursts again. burst_if_hit's call is at 0x484 of the object's code,
     * and the squash's at 0x49a. the landing's stores are at 0x4be and 0x4c4,
     * and its burst at 0x4dc */
    if (work->timer > ARMED_STEPS) {
        enemy_bomb_burst_if_hit(work, enemy);
    }
    enemy_bomb_move(work, enemy);
    enemy_meet_stage(work, enemy);
    enemy_squash_step(work, enemy);
    if (enemy->flags & ENEMY_ON_FLOOR) {
        if (work->submode == BOMB_RED_ONE || work->submode == BOMB_FIRE) {
            RADIUS(work) = BLAST_START;
            LINGER(work) = 0.0F;
            work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
            work->collision->shapes[OUTER_SHAPE].flags |= SHAPE_OFF;
            enemy_bomb_burst(work, enemy);
            if (work->submode == BOMB_FIRE) {
                work->collision->shapes[FIRE_SHAPE].flags &= ~SHAPE_OFF;
                work->mode = MODE_FIRE;
            } else {
                work->mode = MODE_BLAST;
            }
            work->timer = 0;
        } else if (njAbs(enemy->spd.y) > SQUASH_SPEED) {
            enemy->squash_spd = SQUASH_SPRING;
        }
    }
    if ((work->submode == BOMB_NORMAL || work->submode == BOMB_HEAVY) &&
        !game_state.time_stopped) {
        FUSE(work) -= FUSE_STEP;
        /* @hack redundant: the place and speed of a spark that nothing
         * makes, where three calls of rand, their results unused, would do */
        pos.x = work->pos.x;
        pos.y = work->pos.y + (FUSE(work) * 2.5F) + 2.3F;
        pos.z = work->pos.z;
        spd.x = (0.5F - ((Float)rand() * RAND_SCALE)) * 1.2F;
        spd.y = (((Float)rand() * RAND_SCALE) + 0.2F) * 0.9F;
        spd.z = (0.5F - ((Float)rand() * RAND_SCALE)) * 1.2F;
    }
    /* @bug logic: a burst in the fall step doesn't stop the lie below,
     * whose test leaves out the mode. a bursting bomb on the floor whose spd.y
     * is within LIE_SPEED of 0 then lies, and bursts again once its count runs
     * out. the store of MODE_LIE at 0x60a of the object's code follows
     * burst_if_hit's call at 0x484. it follows the landing's stores of
     * MODE_FIRE and MODE_BLAST at 0x4fc and 0x500 too */
    if (njAbs(enemy->spd.y) <= LIE_SPEED && (enemy->flags & ENEMY_ON_FLOOR)) {
        work->mode = MODE_LIE;
        work->timer = 0;
        COUNT(work) = LIE_COUNT;
    }
}

static void enemy_bomb_draw_lying(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    njSetTexture(&enemy_bomb_texlist);
    draw_control_save();
    draw_attr_save();
    if (work->timer <= FLASH_STEPS) {
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL |
                           NJD_CONTROL_3D_CONSTANT_ATTR);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        draw_attr_set(0, NJD_FST_IL | NJD_FST_IS);
        constant_material_set(1.0F, 2.0F, -2.0F, -2.0F);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    if (work->submode == BOMB_HEAVY) {
        enemy = task->motion.p;
        texture_anim_apply(&heavy_bomb_model, &heavy_bomb_anim, FRAME(enemy));
        njScale(NULL, SCALE(enemy), SCALE(enemy), SCALE(enemy));
        njCnkSimpleDrawModel(&heavy_bomb_model);
    } else {
        njCnkSimpleDrawModel(&enemy_bomb_normal_model);
    }
    njPopMatrixEx();
    if (work->timer <= FLASH_STEPS) {
        njFogEnable();
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_control_restore();
    draw_attr_restore();
    /* @hack redundant: a return as the last statement */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void enemy_bomb_draw_fire(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: unused, a local nothing reads, which sizes the frame */
    Uint8 unused[12];
    NJS_VECTOR scl;
    NJS_POINT3 pos;

    pos = work->pos;
    pos.y += FIRE_HEIGHT / 2;
    /* @hack spelling: the chain from scl.z to scl.x, where one from scl.x to
     * scl.z would do */
    scl.z = scl.x = RADIUS(work);
    scl.y = FIRE_HEIGHT;
    njPushMatrixEx();
    njTranslateEx(&pos);
    njScaleEx(&scl);
    njCnkEasyDrawObject(debug_models[DEBUG_MODEL_CYLINDER]);
    njPopMatrixEx();
}

static void enemy_bomb_lie(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: state, a local for game_state's address, where
     * game_state read in place would do */
    struct game_state *state;

    enemy_bomb_burst_if_hit(work, enemy);
    /* @hack spelling: one test inside the other, where && would do */
    if (!(njScalor(&enemy->spd) < REST_SPEED &&
          (enemy->flags & ENEMY_ON_FLOOR))) {
        if (!(work->flags & WORK_HELD_BY_PLAYER)) {
            enemy_bomb_move(work, enemy);
            enemy_meet_stage(work, enemy);
        }
    }
    state = &game_state;
    if (work->timer > COUNT(work)) {
        work->timer = 0;
        if (COUNT(work) > 0 && !state->time_stopped) {
            COUNT(work)--;
        }
    }
    if (!state->time_stopped) {
        FUSE(work) -= FUSE_STEP;
    }
    if (FUSE(work) < 0.0F) {
        FUSE(work) = 0.0F;
    }
    if (COUNT(work) < 1) {
        work->mode = MODE_BLAST;
        work->timer = 0;
        RADIUS(work) = BLAST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[OUTER_SHAPE].flags |= SHAPE_OFF;
        enemy_bomb_burst(work, enemy);
    }
}

static void enemy_bomb_blast(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: blast, a local for the blast's shape, where
     * shapes[BLAST_SHAPE] read at each use would do */
    struct collision_shape *blast = &work->collision->shapes[BLAST_SHAPE];

    if (game_state.time_stopped) {
        return;
    }
    if (work->submode == BOMB_HEAVY && RADIUS(work) < HEAVY_BLAST_END) {
        RADIUS(work) += BLAST_STEP;
        blast->radius += BLAST_STEP;
    } else if (work->submode != BOMB_HEAVY && RADIUS(work) < BLAST_END) {
        RADIUS(work) += BLAST_STEP;
        blast->radius += BLAST_STEP;
    } else {
        LINGER(work) -= LINGER_STEP;
        if (LINGER(work) < LINGER_END) {
            work->mode = MODE_DONE;
            work->timer = 0;
        }
    }
    work->ang.y += BLAST_SPIN;
}

static void enemy_bomb_blast_fire(struct task_work *work,
                                  struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (RADIUS(work) < BLAST_END) {
        struct collision_shape *shape = &work->collision->shapes[BLAST_SHAPE];

        RADIUS(work) += BLAST_STEP;
        shape->radius += BLAST_STEP;
        /* @hack spelling: shape stepped from the blast's shape on to
         * the fire's, where shapes[FIRE_SHAPE] would do */
        shape += FIRE_SHAPE - BLAST_SHAPE;
        shape->radius += BLAST_STEP;
    } else {
        LINGER(work) -= LINGER_STEP;
        if (LINGER(work) < LINGER_END) {
            work->mode = MODE_DONE;
            work->timer = 0;
        }
    }
    work->ang.y += BLAST_SPIN;
}

static void enemy_bomb_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void enemy_bomb_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (placement_try_leave(task)) {
        return;
    }
    /* the parent was killed, and is destroyed at its next step */
    if (task->parent != NULL && task->parent->step == task_destroy) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case MODE_FALL:
        enemy_bomb_fall(work, enemy);
        if (work->mode == MODE_LIE) {
            task->draw = enemy_bomb_draw_lying;
        } else if (work->mode == MODE_BLAST) {
            task->draw = NULL;
        } else if (work->mode == MODE_FIRE) {
            task->draw = enemy_bomb_draw_fire;
        }
        break;
    case MODE_LIE:
        enemy_bomb_lie(work, enemy);
        if (work->mode == MODE_BLAST) {
            task->draw = NULL;
        }
        break;
    case MODE_BLAST:
        enemy_bomb_blast(work, enemy);
        if (collision_next_player(task) != NULL) {
            task->unk_3c.l |= BOMB_HIT_PLAYER;
            if (task->parent != NULL) {
                task->parent->work->flags |= BOMB_HIT_PLAYER;
            }
        }
        if (work->mode != MODE_BLAST) {
            task_kill(task);
            return;
        }
        break;
    case MODE_FIRE:
        enemy_bomb_blast_fire(work, enemy);
        if (collision_next_player(task) != NULL) {
            task->unk_3c.l |= BOMB_HIT_PLAYER;
            if (task->parent != NULL) {
                task->parent->work->flags |= BOMB_HIT_PLAYER;
            }
        }
        if (work->mode != MODE_FIRE) {
            task_kill(task);
            return;
        }
        break;
    case MODE_HELD:
        if (!(work->flags & WORK_HELD_BY_PLAYER)) {
            /* @bug memory: a held BOMB_RING that came up a ring falls once
             * let go, and the fall reads and writes through its null
             * enemy_work. this store of MODE_FALL starts the fall, and
             * enemy_bomb_init's test expects the null. the store is at 0xb9c
             * of the object's code, and the test at 0xe80. the fall reaches
             * the null in enemy_bomb_move at 0x16, enemy_meet_stage's probe at
             * 0x8c021630, enemy_squash_step at 0x8c022e10 and enemy_bomb_fall
             * at 0x4a0 */
            work->mode = MODE_FALL;
        }
        if (work->submode == BOMB_HEAVY) {
            SCALE(enemy) =
                float_approach(SCALE(enemy), HEAVY_SCALE, HEAVY_SCALE_GROWTH);
        }
        work->scl.x = float_approach(work->scl.x, 1.0F, HELD_GROWTH);
        work->scl.y = float_approach(work->scl.y, 1.0F, HELD_GROWTH);
        return;
    }
    /* @hack spelling: one test inside the other, where && would do */
    if (!game_state.time_stopped) {
        if (work->submode == BOMB_HEAVY) {
            FRAME(enemy)++;
            SCALE(enemy) =
                float_approach(SCALE(enemy), HEAVY_SCALE, HEAVY_SCALE_GROWTH);
            work->collision->shapes[BODY_SHAPE].radius =
                float_approach(work->collision->shapes[BODY_SHAPE].radius,
                               HEAVY_BODY_RADIUS, HEAVY_BODY_GROWTH);
            work->collision->shapes[BODY_SHAPE].size_y =
                float_approach(work->collision->shapes[BODY_SHAPE].size_y,
                               HEAVY_BODY_SIZE_Y, HEAVY_BODY_SIZE_Y_GROWTH);
        }
    }
    if (!(work->flags & WORK_HELD_BY_PLAYER)) {
        collision_register(task);
        if (!game_state.time_stopped) {
            work->timer++;
        }
    }
}

static void enemy_bomb_init(struct task *task, enum bomb_start start)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = NULL;
    int player;
    NJS_VECTOR spd;

    if (work->submode == BOMB_HEAVY) {
        collision_init(task, heavy_bomb_shapes, BOMB_SHAPES, COLLISION_ENEMY);
    } else if (work->submode == BOMB_FIRE) {
        collision_init(task, fire_bomb_shapes, FIRE_BOMB_SHAPES,
                       COLLISION_ENEMY);
    } else if (work->submode != BOMB_RING) {
        collision_init(task, enemy_bomb_shapes, BOMB_SHAPES, COLLISION_ENEMY);
    }
    /* @hack spelling: the work reached again through task, where work would
     * do */
    if (task->work->collision != NULL) {
        task->work->collision->flags &= ~COLLISION_TARGET;
        collision_set_radius(task, RECORD_RADIUS);
    }
    if (work->submode != BOMB_RING) {
        enemy = enemy_work_alloc(task);
        /* @hack spelling: shadow_ratio set in each arm, where one store
         * after the test would do */
        if (work->submode == BOMB_HEAVY) {
            enemy->shadow_size = HEAVY_SHADOW_SIZE;
            enemy->shadow_ratio = SHADOW_RATIO;
        } else {
            enemy->shadow_size = SHADOW_SIZE;
            enemy->shadow_ratio = SHADOW_RATIO;
        }
    }
    /* a bomb keeps the submode BOMB_RING only under BOMB_HELD, so a thrown
     * bomb has an enemy_work */
    if (start == BOMB_THROWN) {
        player = player_nearest(&work->pos);
        /* @bug logic: the tests for no player never hold, since
         * player_nearest returns 0, not -1, where it finds none. the tests
         * are CMP/EQ #-1,R0 at 0xd74 and 0xdaa of the object's code.
         * player_nearest sets that 0 by MOV R4,R10 at 0x8c04b042, before its
         * search */
        /* @bug memory: reads player 0's work through null where
         * player_nearest finds no player and slot 0 is empty.
         * enemy_bomb_create's test of the nearest player's work, at 0x119a of
         * the object's code, expects the null. enemy_bomb_init reads through
         * it at 0xd86, and enemy_player_distance2 at 0x8c0224ca */
#ifdef NONMATCHING
        /* this side repairs the tag above: an empty slot counts as no player,
         * so the bomb keeps its heading and flies at THROW_SPEED */
        if (player_work[player] == NULL) {
            player = -1;
        }
#endif
        /* @hack spelling: player tested against -1 twice, where one test
         * and its else would do */
        if (player != -1) {
            struct task_work *target;
            Float dz;

            /* @hack spelling: target assigned inside the subtraction, where
             * a statement of its own would do */
            dz = (target = player_work[player])->pos.z - work->pos.z;
            work->ang.y = (Angle)(atan2f(target->pos.x - work->pos.x, dz) *
                                  ANGLE_PER_RADIAN);
        }
        if (player != -1) {
            spd.z = njSqrt(enemy_player_distance2(work, player)) *
                    THROW_SPEED_PER_DISTANCE;
        } else {
            spd.z = THROW_SPEED;
        }
        spd.y = THROW_RISE;
        spd.x = 0.0F;
        if (spd.z > THROW_SPEED) {
            spd.z = THROW_SPEED;
        }
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, work->ang.y);
        njCalcVector(NULL, &spd, &enemy->spd);
        njPopMatrixEx();
    } else if (enemy != NULL) {
        enemy->spd.x = 0.0F;
        enemy->spd.y = 0.0F;
        enemy->spd.z = 0.0F;
        sound_play_at(START_SOUND, NULL, 0, START_VOLUME, &work->pos);
    }
    if (work->submode == BOMB_NORMAL) {
        fall_set_kind(work, task->motion.p, FALL_KIND_NORMAL);
        work->flags &= ~WORK_HELD_BY_PLAYER;
    } else if (work->submode == BOMB_HEAVY) {
        fall_set_kind(work, task->motion.p, FALL_KIND_HEAVY);
        work->flags &= ~WORK_HELD_BY_PLAYER;
    }
    work->timer = 0;
    task->unk_3c.l = 0;
    if (work->submode == BOMB_HEAVY) {
        SCALE(enemy) = HEAVY_SCALE_START;
    } else if (work->submode != BOMB_RING) {
        SCALE(enemy) = 1.0F;
    }
    /* @hack type: the random angles worked out as Floats, where rand() * 2
     * would do */
    work->ang.x = (Angle)RANDOM_TURN();
    work->ang.z = (Angle)RANDOM_TURN();
    if (work->submode != BOMB_RING) {
        enemy->turn_speed = TURN_SPEED;
        enemy->aim_y = work->ang.y;
        enemy->motion.frame = 0.0F;
        enemy->probe_offset.x = 0.0F;
        enemy->probe_offset.y = 0.0F;
        enemy->probe_offset.z = 0.0F;
        if (work->submode == BOMB_HEAVY) {
            enemy->top = 12.0F;
            enemy->radius = 10.0F;
            enemy->bottom = -HEAVY_DEPTH;
        } else {
            enemy->top = 3.0F;
            enemy->radius = 3.0F;
            enemy->bottom = -3.0F;
        }
        enemy->wall_bounce = 0.7F;
        enemy->floor_bounce = 0.5F;
        enemy->friction = 0.5F;
        enemy->unk_0e4 = 0.7F;
        enemy->damage = 2.0F;
        FUSE(work) = FUSE_LENGTH;
        enemy_set_home(work, enemy);
    }
    if (start == BOMB_HELD) {
        work->mode = MODE_HELD;
        work->flags |= WORK_HELD_BY_PLAYER;
        work->scl.x = 0.0F;
        work->scl.y = 0.0F;
    } else {
        work->mode = MODE_FALL;
    }
    if (work->submode != BOMB_RING) {
        task->destructor = enemy_bomb_destroy;
        if (work->submode == BOMB_NORMAL || work->submode == BOMB_HEAVY) {
            task->draw = enemy_bomb_draw;
        } else {
            task->draw = enemy_bomb_draw_red;
        }
    } else {
        task->draw = enemy_bomb_draw_ring;
    }
    if (start == BOMB_BURSTING) {
        RADIUS(work) = BLAST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes[BLAST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes[OUTER_SHAPE].flags |= SHAPE_OFF;
        enemy_bomb_burst(work, enemy);
        if (work->submode == BOMB_FIRE) {
            work->collision->shapes[FIRE_SHAPE].flags &= ~SHAPE_OFF;
            work->mode = MODE_FIRE;
            task->draw = enemy_bomb_draw_fire;
        } else {
            work->mode = MODE_BLAST;
            task->draw = NULL;
        }
        work->timer = 0;
    }
}

static void enemy_bomb_texture_destroy(struct task *task)
{
    njReleaseTexture(&enemy_bomb_texlist);
    enemy_bomb_texture_task = NULL;
}

static void enemy_bomb_texture_step(struct task *task)
{
}

void enemy_bomb_load(void)
{
    if (enemy_bomb_texture_task == NULL) {
        texture_load_pvm("E_BOMTEX", &enemy_bomb_texlist);
        enemy_bomb_texture_task =
            task_create(0, TEXTURE_LEVEL, enemy_bomb_texture_step, "ManTex");
        enemy_bomb_texture_task->destructor = enemy_bomb_texture_destroy;
    }
    blast_load_texture();
}

struct task *enemy_bomb_create(struct task *parent, enum bomb_kind kind,
                               enum bomb_start start, NJS_POINT3 *pos)
{
    struct task *task;

    if (kind == BOMB_RING) {
        if ((Float)rand() * RAND_SCALE >= RING_ODDS) {
            /* @hack spelling: an empty arm for BOMB_HELD, where the test
             * start != BOMB_HELD would do */
            if (start == BOMB_HELD) {
            } else {
                struct task *ring;

                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((ring = task_create(TASK_WORK | TASK_EXTRA, RING_LEVEL,
                                        scattered_ring_step, "Tobitiri")) !=
                    NULL) {
                    struct task_work *work = ring->work;
                    struct task_work *target;
                    int player;

                    work->pos = *pos;
                    work->pos.y -= RING_DROP;
                    game_state.ring_total++;
                    snapshot_current->ring_total++;
                    player = player_nearest(pos);
                    target = player_work[player];
                    if (target != NULL) {
                        work->ang.y = (Angle)(atan2f(target->pos.x - pos->x,
                                                     target->pos.z - pos->z) *
                                              ANGLE_PER_RADIAN);
                    }
                }
                return ring;
            }
        } else {
            kind = BOMB_NORMAL;
        }
    }
    if (parent != NULL) {
        task = task_create_child(TASK_WORK, enemy_bomb_step, parent);
    } else {
        task = task_create(TASK_WORK, BOMB_LEVEL, enemy_bomb_step,
                           "EnemyBombExecutor");
    }
    if (task != NULL) {
        task->work->submode = kind;
        task->work->pos = *pos;
        enemy_bomb_init(task, start);
        enemy_bomb_load();
    }
    return task;
}
