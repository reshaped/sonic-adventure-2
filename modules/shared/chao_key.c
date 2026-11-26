/* the Chao key a Chao box leaves. it falls and bounces where it is made, and
 * flies off once a player touches it at rest */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/chao_key.h"
#include "shared/ring_sparkle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "shared/chao_key/text.inc"

/* the object file exports chao_key_init, though nothing calls it and no
 * header declares it. SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* a step an object list could give a key. unless placement_try_leave
 * returns TRUE, it makes task a key whose floor is its pos.y. the key rests
 * from its next step */
void chao_key_init(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

/* a key's modes, in its work's submode */
enum chao_key_mode { MODE_INIT, MODE_BOUNCE, MODE_REST };

/* a flight's modes, in its work's mode. a flight is the task that carries a
 * touched key off to chao_key_flight_target. a flight from a place in the
 * world starts in MODE_FROM_WORLD. its first step takes its work's pos
 * through the current matrix into the space the flight draws in */
enum chao_key_flight_mode { MODE_FLY, MODE_FROM_WORLD };

/* the task levels of a key and of its flight */
#define KEY_LEVEL 2
#define FLIGHT_LEVEL 3
/* the sound a flight starts with */
#define FLIGHT_SOUND 0x8006

/* FLOOR(work) is the height a key bounces on. FALL_SPEED(work) is its speed
 * up, in units a step. each step the speed keeps FALL_DRAG of itself and loses
 * FALL_GRAVITY. a bounce keeps BOUNCE of it, and one that leaves less than
 * REST_SPEED brings the key to rest */
#define FLOOR(work) ((work)->scl.x)
#define FALL_SPEED(work) ((work)->scl.y)
#define FALL_GRAVITY 0.08F
#define FALL_DRAG 0.985F
#define BOUNCE 0.6F
#define REST_SPEED 0.3F
#define TOUCH_SPEED 2.5F
/* a key draws KEY_HEIGHT above its pos. its shape is a sphere of
 * KEY_RADIUS about that point */
#define KEY_HEIGHT 5.0F
#define KEY_RADIUS 5.0F

/* a touch throws SPARKLES sparkles of size SPARKLE_SCALE about the key.
 * sparkle i lies SPARKLE_RADIUS out from the key's pos, a quarter turn round
 * from the one before. it lies SPARKLE_LOW + i * SPARKLE_RISE above the pos,
 * and rises at SPARKLE_SPEED */
#define SPARKLES 4
#define SPARKLE_SCALE 2.0F
#define SPARKLE_RADIUS 4.0F
#define SPARKLE_LOW 3.5F
#define SPARKLE_RISE 1.9F
#define SPARKLE_SPEED 0.01F

/* SCALE(work) scales a flight's key, from 1 down. each step a flight moves
 * its key EASE of the way to chao_key_flight_target. each _RANGE2 bounds the
 * square of the key's distance from the target before the move.
 *
 * the timer that spins and wobbles the key counts FAST_TIMER_STEP a step,
 * and 1 a step below SLOW_RANGE2. below SHRINK_RANGE2, each step multiplies
 * SCALE(work) by SHRINK. below TAKEN_RANGE2, the flight sets
 * game_state.chao_key to 1. the flight ends once SCALE(work) is below
 * END_SCALE */
#define SCALE(work) ((work)->scl.z)
#define EASE 0.1F
#define SLOW_RANGE2 6.0F
#define FAST_TIMER_STEP 3
#define SHRINK_RANGE2 3.0F
#define SHRINK 0.9F
#define TAKEN_RANGE2 1.5F
#define END_SCALE 0.2F

/* a flight's key turns about y by its timer times FLIGHT_SPIN. its wobble is
 * WOBBLE * njSin(timer * WOBBLE_RATE). the key's width and depth scale by
 * 1 + wobble. its height scales by (1 - wobble) * SQUASH */
#define FLIGHT_SPIN 288
#define WOBBLE 0.1F
#define WOBBLE_RATE 768
#define SQUASH 0.8F

/* both draws lay a glow quad over the key, at the pos of its child node,
 * chao_key_glow_object. a flight draws the quad at a distance of GLOW_DEPTH */
#define GLOW_TEXTURE 2
#define GLOW_COLOR 0xffffffff
#define GLOW_DEPTH 1.05F

#define CHAO_KEY_TEXTURES 3
#define KEY_SHAPES 1

static void chao_key_flight_draw(struct task *task);
static void chao_key_flight_step(struct task *task);
static void chao_key_flight_forget(struct task *task);
static void chao_key_setup(struct task *task);
static void chao_key_destroy(struct task *task);
static void chao_key_step(struct task *task);
static void chao_key_draw(struct task *task);

static NJS_TEXNAME chao_key_texnames[CHAO_KEY_TEXTURES] = {
#include "shared/chao_key/chao_key_texnames.inc"
};
NJS_TEXLIST chao_key_texlist = {
#include "shared/chao_key/chao_key_texlist.inc"
};

/* the glow node's model, a square facing +z. neither draw draws the model:
 * each lays its glow quad at the node's pos instead */
static Sint16 chao_key_glow_polygons[25] = {
#include "shared/chao_key/chao_key_glow_polygons.inc"
};
static Sint32 chao_key_glow_vertices[27] = {
#include "shared/chao_key/chao_key_glow_vertices.inc"
};
static NJS_CNK_MODEL chao_key_glow_model = {
#include "shared/chao_key/chao_key_glow_model.inc"
};
static NJS_CNK_OBJECT chao_key_glow_object = {
#include "shared/chao_key/chao_key_glow_object.inc"
};

/* the key's model: two strips of texture 1, then strips of texture 0 that
 * the environment maps */
static Sint16 chao_key_polygons[187] = {
#include "shared/chao_key/chao_key_polygons.inc"
};
static Sint32 chao_key_vertices[315] = {
#include "shared/chao_key/chao_key_vertices.inc"
};
static NJS_CNK_MODEL chao_key_model = {
#include "shared/chao_key/chao_key_model.inc"
};
/* the key's node, whose child is the glow node */
/* @hack type: chao_key_object an array of one, where a single node would do */
static NJS_CNK_OBJECT chao_key_object[1] = {
#include "shared/chao_key/chao_key_object.inc"
};

NJS_TEXLIST *chao_key_texlists[2] = {
#include "shared/chao_key/chao_key_texlists.inc"
};

/* what each step adds to a key's ang.y */
static Angle chao_key_spin = 0x200;
/* the key's tilt about x as it draws */
static Angle chao_key_tilt = FULL_TURN / 12;
/* the key's shape attacks at level 0, which beats no guard. so it hurts
 * nothing, whatever its SHAPE_HURTS_ bits */
/* clang-format off */
static struct collision_shape chao_key_shapes[KEY_SHAPES] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS),
     SHAPE_ATTACK_LEVEL(0) | SHAPE_HURTS_ALL, SHAPE_UNK_800000,
     {0.0F, KEY_HEIGHT, 0.0F}, KEY_RADIUS, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the task of the one flight that may run, or null while none runs */
static struct task *chao_key_flight = NULL;
/* the place each flight's key flies to, in the space the flight draws in */
static NJS_POINT3 chao_key_flight_target = {-43.0F, -30.0F, 100.0F};
/* a flight's scale of its key, before the wobble and SCALE(work) apply. -1
 * on each axis reflects the key through its origin */
static Float chao_key_flight_scale = -1.0F;

/* @hack spelling: three inline functions that step a Float pointer through
 * each vector, where the members written out at each use would do */
#pragma inline(vector_subtract, vector_scale, vector_add)

static void vector_subtract(NJS_VECTOR *out, NJS_VECTOR *minuend,
                            NJS_VECTOR *subtrahend)
{
    Float *dst = &out->x;
    Float *src = &minuend->x;
    Float *sub = &subtrahend->x;

    *dst++ = *src++ - *sub++;
    *dst++ = *src++ - *sub++;
    *dst = *src - *sub;
}

static void vector_scale(Float scale, NJS_VECTOR *vec)
{
    Float *dst = &vec->x;

    *dst++ *= scale;
    *dst++ *= scale;
    *dst *= scale;
}

static void vector_add(NJS_VECTOR *addend, NJS_VECTOR *sum)
{
    Float *src = &addend->x;
    Float *dst = &sum->x;

    *dst++ += *src++;
    *dst++ += *src++;
    *dst += *src;
}

/* the flight's draw. the key draws at the flight's pos under a unit matrix,
 * with no depth compare. its model's radius is 0 while it draws, which skips
 * njCnkSimpleDrawModel's model clip */
static void chao_key_flight_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 glow;
    Float wobble = njSin(work->timer * WOBBLE_RATE) * WOBBLE;
    Float radius = chao_key_object->model->r;

    chao_key_object->model->r = 0.0F;
    njSetTexture(&chao_key_texlist);
    njFogDisable();
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->timer * FLIGHT_SPIN);
    njScale(NULL, chao_key_flight_scale * (1.0F + wobble) * SCALE(work),
            chao_key_flight_scale * (1.0F - wobble) * SCALE(work) * SQUASH,
            chao_key_flight_scale * (1.0F + wobble) * SCALE(work));
    njZBufferMode(ZBUFFER_OFF);
    njCnkSimpleDrawModel(chao_key_object->model);
    njZBufferMode(ZBUFFER_ON);
    njCalcPoint(NULL, (NJS_POINT3 *)chao_key_object->child->pos, &glow);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njUnitMatrix(NULL);
    particle_draw_quad(&glow, GLOW_TEXTURE, QUARTER_TURN, 0, 1.0F, 1.0F,
                       GLOW_COLOR, PARTICLE_DEPTH_Z, GLOW_DEPTH, 0.0F);
    njPopMatrixEx();
    njFogEnable();
    chao_key_object->model->r = radius;
}

static void chao_key_flight_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR move;
    Float distance2;

    /* @hack spelling: the mode tested for truth, where work->mode != MODE_FLY
     * would do */
    if (work->mode) {
        work->mode = MODE_FLY;
        njCalcPoint(NULL, &work->pos, &work->pos);
    } else {
        vector_subtract(&move, &chao_key_flight_target, &work->pos);
        distance2 = njScalor2(&move);
        if (distance2 < SLOW_RANGE2) {
            work->timer++;
        } else {
            work->timer += FAST_TIMER_STEP;
        }
        if (distance2 < SHRINK_RANGE2) {
            SCALE(work) *= SHRINK;
        }
        if (distance2 < TAKEN_RANGE2) {
            game_state.chao_key = 1;
        }
        vector_scale(EASE, &move);
        /* @hack spelling: pos added into move, then move copied back to pos,
         * where move added to pos in place would do */
        vector_add(&work->pos, &move);
        work->pos = move;
        if (SCALE(work) < END_SCALE) {
            task_destroy(task);
        }
    }
}

/* the flight's destructor. game_state.chao_key keeps chao_key_taken TRUE
 * once chao_key_flight is null */
static void chao_key_flight_forget(struct task *task)
{
    if (chao_key_flight == task) {
        chao_key_flight = NULL;
    }
    game_state.chao_key = 1;
}

Bool chao_key_taken(void)
{
    if (game_state.chao_key == 1) {
        return TRUE;
    }
    if (chao_key_flight != NULL) {
        return TRUE;
    }
    return FALSE;
}

/* start a flight from pos, a place in the world, or at its target for a
 * null pos. it starts none where chao_key_taken returns TRUE, or where the
 * heap can't give its task */
static void chao_key_fly(NJS_POINT3 *pos)
{
    struct task *task;

    /* @hack redundant: the test of chao_key_flight, which chao_key_taken has
     * made already */
    if (!chao_key_taken() && chao_key_flight == NULL) {
        task = task_create(TASK_WORK, FLIGHT_LEVEL, chao_key_flight_step,
                           "ChaoKeyTask");
        if (task != NULL) {
            chao_key_flight = task;
            task->draw = chao_key_flight_draw;
            task->destructor = chao_key_flight_forget;
            SCALE(task->work) = 1.0F;
            if (pos != NULL) {
                task->work->mode = MODE_FROM_WORLD;
                task->work->pos = *pos;
            } else {
                task->work->pos = chao_key_flight_target;
            }
            sound_play(FLIGHT_SOUND, NULL, 0, 0);
        }
    }
}

static void chao_key_sparkle(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    int i;

    for (i = 0; i < SPARKLES; i++) {
        pos.x = work->pos.x + (njSin(i * QUARTER_TURN) * SPARKLE_RADIUS);
        pos.y = work->pos.y + (i * SPARKLE_RISE) + SPARKLE_LOW;
        pos.z = work->pos.z + (njCos(i * QUARTER_TURN) * SPARKLE_RADIUS);
        spd.x = 0.0F;
        spd.y = SPARKLE_SPEED;
        spd.z = 0.0F;
        ring_sparkle_create(&pos, &spd, SPARKLE_SCALE);
    }
}

void chao_key_init(struct task *task)
{
    /* @hack spelling: key, a copy of task, where task read in place would do */
    struct task *key = task;

    if (!placement_try_leave(key)) {
        chao_key_setup(key);
    }
}

struct task *chao_key_create(NJS_POINT3 *pos, Float floor_y, Float speed)
{
    struct task *task =
        task_create(TASK_WORK, KEY_LEVEL, chao_key_step, "ObjectChaoKeyExec");
    struct task_work *work;

    if (task != NULL) {
        work = task->work;
        work->pos = *pos;
        chao_key_setup(task);
        FLOOR(work) = floor_y;
        FALL_SPEED(work) = speed;
        return task;
    }
    return NULL;
}

/* make task a key whose floor is its pos.y, at a speed of 0. the call
 * clears ang.x and ang.z. task needs a work */
static void chao_key_setup(struct task *task)
{
    struct task_work *work = task->work;

    task->draw = chao_key_draw;
    task->step = chao_key_step;
    task->destructor = chao_key_destroy;
    /* @hack redundant: the submode set to MODE_INIT, which the store of
     * MODE_BOUNCE below overwrites */
    work->submode = MODE_INIT;
    work->ang.x = 0;
    work->ang.z = 0;
    collision_init(task, chao_key_shapes, KEY_SHAPES, COLLISION_OBJECT);
    FLOOR(work) = work->pos.y;
    FALL_SPEED(work) = 0.0F;
    work->submode = MODE_BOUNCE;
}

static void chao_key_destroy(struct task *task)
{
}

static void chao_key_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        work->pos.y += FALL_SPEED(work);
        if (work->pos.y <= FLOOR(work)) {
            FALL_SPEED(work) *= -BOUNCE;
            work->pos.y = FLOOR(work);
            if (njAbs(FALL_SPEED(work)) < REST_SPEED) {
                work->submode = MODE_REST;
            }
        }
        FALL_SPEED(work) = (FALL_SPEED(work) * FALL_DRAG) - FALL_GRAVITY;
        if (work->submode == MODE_REST && collision_next_player(task)) {
            chao_key_sparkle(task);
            /* @hack redundant: the store of TOUCH_SPEED, which nothing reads
             * once the touch ends the key */
            FALL_SPEED(work) = TOUCH_SPEED;
            work->submode = MODE_BOUNCE;
            chao_key_fly(&work->pos);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            collision_register(task);
        }
        work->ang.y += chao_key_spin;
    }
}

/* a key at rest draws on every step, and a bouncing key on every other */
static void chao_key_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, for the string it leaves in
     * the constants */
    if (0) {
        (void)"<- ANGLE Y";
    }
    njSetTexture(&chao_key_texlist);
    if (work->submode == MODE_REST || (game_state.steps & 1)) {
        njPushMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y + KEY_HEIGHT, work->pos.z);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, chao_key_tilt);
        njCnkSimpleDrawModel(&chao_key_model);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        particle_draw_quad((NJS_POINT3 *)chao_key_object->child->pos,
                           GLOW_TEXTURE, QUARTER_TURN, 0, 1.0F, 1.0F,
                           GLOW_COLOR, 0, 0.0F, 0.0F);
        njPopMatrix(1);
    }
}
