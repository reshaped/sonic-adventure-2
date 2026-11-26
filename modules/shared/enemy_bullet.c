/* the bullets the enemies fire */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/enemy_bullet/text.inc"
#include "shared/dust.h"
#include "shared/enemy_bullet.h"
#include "shared/spark.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"

/* a bullet's work->mode */
enum bullet_mode {
    MODE_FLY,
    MODE_BURST,
    MODE_DONE /* the burst is over, and the bullet kills itself */
};

/* the holder, bullet_texture_task, keeps builds: models that
 * njCnkDirectObjectCompile compiled for njDirectDrawObject. its force holds
 * STREAK_BUILDS builds of the streak, each BUILD_SIZE bytes. its extra holds
 * the bullet's build */
#define BUILD_SIZE(holder) ((holder)->unk_3c.l)
#define STREAK_BUILDS 10
/* the index in lights of the light the builds are compiled under */
#define BUILD_LIGHT 8
/* streak build n's material has an alpha of STREAK_ALPHA less n times
 * STREAK_FADE */
#define STREAK_ALPHA (-0.5F)
#define STREAK_FADE 0.04F

/* a streak draws the build its timer picks, while its FADE is above
 * FADE_GONE. FADE starts FADE_PER_SPEED times the bullet's speed above
 * FADE_GONE, and no higher than STREAK_ALPHA. it falls by STREAK_FADE a step.
 * the streak is killed once its timer reaches STREAK_STEPS */
#define FADE(work) ((work)->scl.x)
#define FADE_PER_SPEED 0.3F
#define FADE_GONE (-1.0F)
#define STREAK_STEPS 10
/* a bullet starts a streak on the steps whose count has STREAK_SKIP clear */
#define STREAK_SKIP 1
/* what each step would add to spd, which bullet_move zeroes */
#define ACCEL(enemy) ((enemy)->unk_00c)

/* a burst's RADIUS grows from BURST_START by BURST_STEP a step, while it is
 * below BURST_END. LINGER then falls from 0 by LINGER_STEP a step, and the
 * burst ends once it is below LINGER_END. nothing draws a burst, so RADIUS and
 * LINGER only time it */
#define RADIUS(work) ((work)->scl.x)
#define LINGER(work) ((work)->scl.y)
#define BURST_START 5.0F
#define BURST_END 25.0F
#define BURST_STEP 1.0F
#define LINGER_STEP 0.05F
#define LINGER_END (-1.0F)

/* a bullet bursts at a hit only once its timer passes ARMED_STEPS */
#define ARMED_STEPS 20
/* how far a flying bullet turns about z each step, and a burst about y */
#define FLY_SPIN 0x1400
#define BURST_SPIN 0x100
#define BULLET_RADIUS 2.0F /* the radius of the bullet's shape */
/* a bullet's probe of the stage reaches PROBE_SIZE above, below and across
 * its pos */
#define PROBE_SIZE 3.0F
/* the task levels of a bullet without a parent, and of the task that holds
 * the textures */
#define BULLET_LEVEL 3
#define TEXTURE_LEVEL 0
/* a bullet throws FIRE_SPARKS sparks as it is fired, and STAGE_SPARKS where
 * it meets the stage. they start at SPARK_SPEED times its spd. its firing
 * also puffs dust of scale DUST_SCALE, at DUST_SPEED times its spd */
#define STAGE_SPARKS 16
#define FIRE_SPARKS 8
#define SPARK_SPEED 0.1F
#define DUST_SPEED 0.2F
#define DUST_SCALE 2.0F
#define FIRE_SOUND 0x4000

static NJS_TEXNAME bullet_texname[1] = {
#include "shared/enemy_bullet/bullet_texname.inc"
};
static NJS_TEXLIST bullet_texlist = {
#include "shared/enemy_bullet/bullet_texlist.inc"
};

/* the bullet's model, whose tip points along +z */
#define BULLET_POINTS 13
static Sint16 bullet_polygons[83] = {
#include "shared/enemy_bullet/bullet_polygons.inc"
};
static Sint32 bullet_vertices[55] = {
#include "shared/enemy_bullet/bullet_vertices.inc"
};
static NJS_CNK_MODEL bullet_model = {
#include "shared/enemy_bullet/bullet_model.inc"
};
static NJS_CNK_OBJECT bullet_object = {
#include "shared/enemy_bullet/bullet_object.inc"
};

/* the streak's model, whose tip points along -z */
#define STREAK_POINTS 5
static Sint16 streak_polygons[39] = {
#include "shared/enemy_bullet/streak_polygons.inc"
};
static Sint32 streak_vertices[33] = {
#include "shared/enemy_bullet/streak_vertices.inc"
};
static NJS_CNK_MODEL streak_model = {
#include "shared/enemy_bullet/streak_model.inc"
};
static NJS_CNK_OBJECT streak_object = {
#include "shared/enemy_bullet/streak_object.inc"
};

static struct collision_shape bullet_shape = {
#include "shared/enemy_bullet/bullet_shape.inc"
};

/* the task that keeps bullet_texlist loaded and holds the builds, or null
 * while none does. its destructor releases the list */
static struct task *bullet_texture_task = NULL;

/* a bullet's streaks are its children, and draw under one push of the
 * matrix. the first pushes it, and the last pops it. each other streak loads
 * the pushed matrix back for the next. bullet_texture_task must not be
 * null, unchecked */
static void streak_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: offset, which holds the build's index before its
     * offset, where a local for each would do */
    Uint32 offset;

    if (task->parent->child == task) {
        njPushMatrixEx();
    }
    if (FADE(work) > FADE_GONE) {
        offset = work->timer;
        if (offset >= STREAK_BUILDS) {
            offset = STREAK_BUILDS - 1;
        }
        offset *= BUILD_SIZE(bullet_texture_task);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        if (bullet_texture_task->force.p != NULL) {
            njDirectDrawObject(
                (NJS_DIRECT_HEAD *)((Uint8 *)bullet_texture_task->force.p +
                                    offset));
        }
    }
    if (task->parent->child == task->next) {
        njPopMatrixEx();
        njFogEnable();
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    } else {
        matrix_load_parent();
    }
}

static void streak_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task) || game_state.time_stopped) {
        return;
    }
    if (work->timer++ >= STREAK_STEPS) {
        task_kill(task);
    } else {
        FADE(work) -= STREAK_FADE;
    }
}

static void streak_create(struct task *parent, NJS_POINT3 *pos,
                          struct angles *ang, NJS_VECTOR *spd)
{
    struct task *task;
    struct task_work *work;

    if (game_state.steps & STREAK_SKIP) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create_child(TASK_WORK, streak_step, parent)) != NULL) {
        work = task->work;
        work->pos = *pos;
        work->ang = *ang;
        FADE(work) = (njScalor(spd) * FADE_PER_SPEED) + FADE_GONE;
        if (FADE(work) > STREAK_ALPHA) {
            FADE(work) = STREAK_ALPHA;
        }
        task->draw = streak_draw;
    }
}

static void bullet_move(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    work->pos.x += enemy->spd.x;
    work->pos.y += enemy->spd.y;
    work->pos.z += enemy->spd.z;
    ACCEL(enemy).x = 0.0F;
    ACCEL(enemy).y = 0.0F;
    ACCEL(enemy).z = 0.0F;
}

static void bullet_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: model, a local for the build, where extra passed in
     * place would do */
    NJS_DIRECT_HEAD *model;
    struct angles ang;
    NJS_VECTOR scl;

    /* never true: the step clears a bullet's draw as it bursts */
    if (work->mode == MODE_BURST) {
        ang.x = ang.y = ang.z = 0;
        /* @hack spelling: the chain from scl.z to scl.x, where one from
         * scl.x to scl.z would do */
        scl.z = scl.y = scl.x = RADIUS(work);
        debug_draw_model(&work->pos, &ang, &scl, DEBUG_MODEL_SPHERE);
        return;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njRotateZ(NULL, work->ang.z);
    if (bullet_texture_task != NULL && bullet_texture_task->extra.p != NULL) {
        model = bullet_texture_task->extra.p;
        njDirectDrawObject(model);
    } else {
        njCnkSimpleDrawModel(&bullet_model);
    }
    njPopMatrixEx();
}

static void bullet_burst_if_hit(struct task_work *work,
                                struct enemy_work *enemy)
{
    /* @hack spelling: hit, a flag the test sets and the next test compares
     * with 1, where the first test alone would do */
    int hit = 0;

    if (work->flags & (WORK_UNK_1 | WORK_HURT | WORK_UNK_8)) {
        hit = 1;
    }
    if (hit == 1) {
        work->mode = MODE_BURST;
        work->timer = 0;
        RADIUS(work) = BURST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

static void bullet_fly(struct task_work *work, struct enemy_work *enemy)
{
    NJS_VECTOR spd;

    if (work->timer > ARMED_STEPS) {
        bullet_burst_if_hit(work, enemy);
    }
    bullet_move(work, enemy);
    if (work->submode) {
        enemy_meet_stage(work, enemy);
    }
    if (enemy->flags & (ENEMY_UNDER_CEILING | ENEMY_ON_FLOOR | ENEMY_AT_WALL)) {
        spd.x = enemy->spd.x * SPARK_SPEED;
        spd.y = enemy->spd.y * SPARK_SPEED;
        spd.z = enemy->spd.z * SPARK_SPEED;
        spark_throw(STAGE_SPARKS, &work->pos, &spd);
        work->mode = MODE_BURST;
        work->timer = 0;
        RADIUS(work) = BURST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
    if (!game_state.time_stopped) {
        work->ang.z += FLY_SPIN;
    }
}

static void bullet_burst(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (RADIUS(work) < BURST_END) {
        RADIUS(work) += BURST_STEP;
    } else {
        LINGER(work) -= LINGER_STEP;
        if (LINGER(work) < LINGER_END) {
            work->mode = MODE_DONE;
            work->timer = 0;
        }
    }
    work->ang.y += BURST_SPIN;
}

static void bullet_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void bullet_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (placement_try_leave(task)) {
        return;
    }
    /* the parent was killed, and is destroyed at its next step */
    if (task->parent != NULL && task->parent->step == task_destroy) {
        task->draw = NULL;
        work->collision->shapes->flags |= SHAPE_OFF;
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case MODE_FLY:
        bullet_fly(work, enemy);
        if (work->mode == MODE_BURST) {
            task->draw = NULL;
        } else {
            if (!game_state.time_stopped) {
                streak_create(task, &work->pos, &work->ang, &enemy->spd);
            }
            collision_register(task);
        }
        break;
    case MODE_BURST:
        bullet_burst(work, enemy);
        if (work->mode != MODE_BURST) {
            task_kill(task);
            return;
        }
        break;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
}

static void bullet_init(struct task *task, NJS_VECTOR *spd)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    /* @hack redundant: unused, a local nothing reads, which sizes the
     * frame */
    NJS_VECTOR unused;

    collision_init(task, &bullet_shape, 1, COLLISION_ENEMY);
    /* @hack spelling: the work reached again through task, where work would
     * do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    /* the collision record's radius reaches one step's move past the
     * shape's */
    collision_set_radius(task, njScalor(spd) + BULLET_RADIUS);
    enemy = enemy_work_alloc(task);
    enemy->spd = *spd;
    work->scl.x = 1.0F;
    work->scl.y = 1.0F;
    work->ang.x = (Angle)(asinf(-enemy->spd.y / njScalor(&enemy->spd)) *
                          ANGLE_PER_RADIAN);
    work->ang.y =
        (Angle)(atan2f(enemy->spd.x, enemy->spd.z) * ANGLE_PER_RADIAN);
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = PROBE_SIZE;
    enemy->radius = PROBE_SIZE;
    enemy->bottom = -PROBE_SIZE;
    enemy->damage = 2.0F;
    enemy_set_home(work, enemy);
    task->destructor = bullet_destroy;
    task->draw = bullet_draw;
}

static void bullet_texture_destroy(struct task *task)
{
    njReleaseTexture(&bullet_texlist);
    bullet_texture_task = NULL;
}

static void bullet_texture_step(struct task *task)
{
}

void enemy_bullet_load(void)
{
    NJS_CNK_OBJECT *object;
    Uint32 size;
    Uint8 *build;
    Float alpha;
    int i;
    /* @hack spelling: holder and model, locals for the holder and its
     * build, where extra passed in place would do */
    struct task *holder;
    void *model;

    if (bullet_texture_task != NULL) {
        return;
    }
    texture_load_pvm("E_BULTEX", &bullet_texlist);
    bullet_texture_task =
        task_create(0, TEXTURE_LEVEL, bullet_texture_step, "ManTex");
    /* @bug memory: writes the holder's destructor through null where the
     * heap has no room for the holder, before the test for it. the store is
     * at 0x6ba of the object's code */
#ifdef NONMATCHING
    /* this side repairs the tag above: it tests for the holder before it
     * sets the holder's destructor */
    if (bullet_texture_task == NULL) {
        return;
    }
#endif
    bullet_texture_task->destructor = bullet_texture_destroy;
    if (bullet_texture_task == NULL) {
        return;
    }
    njSetTexture(&bullet_texlist);
    light_apply(BUILD_LIGHT);
    njFogEnable();
    njSetDirectClip();
    /* @hack spelling: object set inside the call's argument, where a
     * statement of its own would do */
    size = njCnkDirectObjectCompileSize(object = &streak_object);
    BUILD_SIZE(bullet_texture_task) = size;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((bullet_texture_task->force.p = syCalloc(STREAK_BUILDS, size)) !=
        NULL) {
        alpha = STREAK_ALPHA;
        build = bullet_texture_task->force.p;
        /* @hack redundant: the reads of object's model and child into model,
         * which nothing uses */
        model = object->model;
        model = object->child;
        for (i = 0; i < STREAK_BUILDS; i++) {
            constant_material_set(alpha, 0.0F, 0.0F, 0.0F);
            njCnkDirectObjectCompile(object, build);
            /* @hack spelling: build = build + size, where build += size
             * would do */
            build = build + size;
            alpha -= STREAK_FADE;
        }
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    }
    size = njCnkDirectObjectCompileSize(&bullet_object);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((bullet_texture_task->extra.p = syCalloc(1, size)) != NULL) {
        holder = bullet_texture_task;
        model = holder->extra.p;
        njCnkDirectObjectCompile(&bullet_object, model);
    }
}

void enemy_bullet_fire(struct task *parent, int kind, NJS_VECTOR *spd,
                       NJS_POINT3 *pos)
{
    struct task *task;
    NJS_VECTOR scaled;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, bullet_step, parent);
    } else {
        task = task_create(TASK_WORK, BULLET_LEVEL, bullet_step,
                           "EnemyBulletExecutor");
    }
    if (task != NULL) {
        task->work->submode = kind;
        task->work->pos = *pos;
        bullet_init(task, spd);
        enemy_bullet_load();
        scaled.x = spd->x * DUST_SPEED;
        scaled.y = spd->y * DUST_SPEED;
        scaled.z = spd->z * DUST_SPEED;
        dust_create(pos, &scaled, DUST_SCALE);
        scaled.x = spd->x * SPARK_SPEED;
        scaled.y = spd->y * SPARK_SPEED;
        scaled.z = spd->z * SPARK_SPEED;
        spark_throw(FIRE_SPARKS, pos, &scaled);
        sound_play_at(FIRE_SOUND, NULL, 0, SOUND_LOUDEST, pos);
    }
}
