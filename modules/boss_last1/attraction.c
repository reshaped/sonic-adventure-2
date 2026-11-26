#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_last1/attraction.h"
#include "draw/lines.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

struct attraction_spawn {
    NJS_POINT3 *target;
    Float *radius;
    Float acceleration;
    NJS_VECTOR offset;
};

/* a particle's TASK_MOTION record. the particle's work->scl.z holds its
 * speed, which grows by acceleration each step */
struct particle_motion {
    NJS_VECTOR velocity;
    Float unused;
    Float alpha;
    Float acceleration;
    NJS_POINT3 *target;
    Float *radius;
};

/* a particle's work->mode */
enum { PARTICLE_HOMING, PARTICLE_FADING };

/* a particle fades once it has homed more than this many steps, near its
 * target or not */
#define HOMING_STEPS 180
/* the opacity a fading particle loses each step, and the opacity it dies
 * below */
#define FADE_STEP 0.08F
#define FADED 0.05F

static NJS_TEXANIM cell = {
#include "boss_last1/attraction/cell.inc"
};
static NJS_SPRITE particle_sprite = {{0, 0, 0}, 0, 0, 0, NULL, &cell};
static void particle_destroy(struct task *task);
static void particle_step(struct task *task);
static void particle_draw(struct task *task);
static void particle_texture(struct task *task, NJS_TEXLIST **textures,
                             Sint16 *texture);

#pragma inline(emitter_scale)
static Float emitter_scale(struct task *task)
{
    struct attraction *emitter = task->parent->extra.p;
    return emitter->scale;
}

static struct task *
attraction_particle_create(struct task *parent,
                           const struct attraction_spawn *spawn)
{
    struct task *task;
    struct task_work *work;
    struct particle_motion *motion;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK | TASK_MOTION, particle_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    motion = task->motion.p;
    work->pos.x = spawn->target->x + spawn->offset.x;
    work->pos.y = spawn->target->y + spawn->offset.y;
    work->pos.z = spawn->target->z + spawn->offset.z;
    work->scl.x = 0.0F;
    motion->alpha = 1.0F;
    motion->acceleration = spawn->acceleration;
    motion->target = spawn->target;
    motion->radius = spawn->radius;
    task->draw = particle_draw;
    task->destructor = particle_destroy;
    return task;
}

static void particle_destroy(struct task *task)
{
}

static void particle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct particle_motion *motion = task->motion.p;
    NJS_VECTOR delta;
    NJS_VECTOR direction;
    Float scale;
    Float distance;
    Float *radius;
    NJS_POINT3 *target;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case PARTICLE_HOMING:
        scale = emitter_scale(task);
        target = motion->target;
        /* @hack redundant: the growth of work->scl.x toward the emitter's
         * scale. nothing reads work->scl.x */
        if (work->scl.x < scale) {
            work->scl.x += 0.25F * scale;
        }
        vector_diff(target, &work->pos, &delta);
        direction = delta;
        njUnitVector(&direction);
        /* @hack redundant: a damping of the velocity, whose result the
         * homing velocity below replaces */
        motion->velocity.x *= 0.94F;
        motion->velocity.y *= 0.94F;
        motion->velocity.z *= 0.94F;
        work->scl.z += motion->acceleration;
        motion->velocity.x = direction.x * work->scl.z;
        motion->velocity.y = direction.y * work->scl.z;
        motion->velocity.z = direction.z * work->scl.z;
        work->pos.x += motion->velocity.x;
        work->pos.y += motion->velocity.y;
        work->pos.z += motion->velocity.z;
        radius = motion->radius;
        distance = njScalor(&delta);
        if (distance < *radius) {
            work->mode = PARTICLE_FADING;
        }
        if (work->timer++ >= HOMING_STEPS) {
            work->mode = PARTICLE_FADING;
        }
        break;
    case PARTICLE_FADING:
        motion->alpha -= FADE_STEP;
        if (motion->alpha < FADED) {
            task_kill(task);
        }
        break;
    }
}

static void particle_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct particle_motion *motion = task->motion.p;
    NJS_SPRITE *sprite;
    NJS_TEXLIST *textures;
    Sint16 texture;
    Float scale;
    struct task *particle;
    struct attraction *emitter;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: a particle of an emitter without
     * textures draws nothing */
    if (!((struct attraction *)task->parent->extra.p)->textures) {
        return;
    }
#endif
    /* @bug uninitialized: a particle of an emitter without textures draws
     * with an unset texture list and texid. particle_texture skips both
     * writes at 0x8c50aea8, and the sprite takes them at 0x8c50ae44 and
     * 0x8c50ae4a */
    particle_texture(task, &textures, &texture);
    /* @hack spelling: the scale read through a second name for task, where
     * emitter_scale(task) would do */
    particle = task;
    emitter = particle->parent->extra.p;
    scale = emitter->scale;
    sprite = &particle_sprite;
    sprite->sx = sprite->sy = scale;
    sprite->p = work->pos;
    sprite->tlist = textures;
    sprite->tanim->texid = texture;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(motion->alpha, motion->alpha, motion->alpha,
                          motion->alpha);
    njSetTexture(textures);
    njDrawSprite3D(sprite, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    constant_material_set(0, 0, 0, 0);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void particle_texture(struct task *task, NJS_TEXLIST **textures,
                             Sint16 *texture)
{
    struct attraction *emitter = task->parent->extra.p;
    if (emitter->textures) {
        *textures = emitter->textures;
        *texture = emitter->texture;
    }
}

static void attraction_step(struct task *task)
{
    struct task_work *work = task->work;
    struct attraction *emitter = task->extra.p;
    struct attraction_spawn spawn;
    Float spread;
    /* work->scl.x counts the particles due, a fraction of one carried from
     * step to step */
    work->scl.x += emitter->rate;
    while (work->scl.x >= 1.0F) {
        spawn.target = emitter->target;
        spawn.acceleration =
            emitter->acceleration +
            ((Float)rand() * emitter->acceleration_range * RAND_SCALE);
        spread = emitter->spread;
        spawn.offset.x = spread - ((Float)rand() * spread * (2 * RAND_SCALE));
        spawn.offset.y = spread - ((Float)rand() * spread * (2 * RAND_SCALE));
        spawn.offset.z = spread - ((Float)rand() * spread * (2 * RAND_SCALE));
        spawn.radius = &emitter->radius;
        attraction_particle_create(task, &spawn);
        work->scl.x -= 1.0F;
    }
}

static void attraction_destroy(struct task *task)
{
}

struct task *biolizard_attraction_create(struct task *parent,
                                         const struct attraction *settings)
{
    struct task *task;
    struct attraction *emitter;
    if (parent) {
        task = task_create_child(TASK_WORK, attraction_step, parent);
        if (!task) {
            return NULL;
        }
        emitter = syCalloc(1, sizeof(*emitter));
        if (!emitter) {
            task_kill(task);
            return NULL;
        }
        task->extra.p = emitter;
        *emitter = *settings;
        task->destructor = attraction_destroy;
        return task;
    }
    /* @bug uninitialized: returns no value for a null parent. the branch at
     * 0x8c50af8c reaches the return at 0x8c50b032 without setting it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
