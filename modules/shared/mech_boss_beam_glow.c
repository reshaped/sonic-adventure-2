/* the mech bosses' beam glows: sprites that fly into a charge of
 * shared/mech_boss_beam.h and fade. the charge throws them */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/lines.h"
#include "shared/mech_boss_beam.h"
#include "shared/mech_boss_beam_glow.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#define START_SIZE 0.002F
#define START_ALPHA 1.0F
/* a glow starts to fade after a step that begins within NEAR_SHARE times
 * *radius of its target. it starts at the latest after FLIGHT_STEPS + 1 steps.
 * it ends once its alpha falls below END_ALPHA */
#define NEAR_SHARE 0.8F
#define FLIGHT_STEPS 180
#define END_ALPHA 0.05F
#define DAMPING 0.94F

enum mech_boss_beam_glow_mode { MODE_FLY, MODE_FADE };

/* a glow's work, laid over the struct task_work that task_create_child
 * allocates */
struct glow_work {
    Sint8 mode;
    Uint8 unk_01[5];
    Uint16 timer; /* the steps it has flown */
    struct angles unk_08;
    NJS_POINT3 pos;
    Float size;
    Float growth; /* what size gains each step */
    Float speed; /* how far it moves in a step */
};

/* a glow's TASK_MOTION record, laid over struct task_motion */
struct glow_motion {
    NJS_VECTOR spd;
    Uint8 unk_0c[4];
    Float alpha;
    Float accel;
    NJS_POINT3 *target;
    Float *radius;
};

#define WORK(task) ((struct glow_work *)(task)->work)

struct beam_glow_tuning {
    Float growth; /* what a glow's size gains each step */
    Float full_size;
    Float unk_08[5];
    Float fade; /* what its alpha loses each step as it fades */
};
/* clang-format off */
static struct beam_glow_tuning glow_tuning = {
    0.002F, 0.045F, {0.0F, 0.0F, 0.0F, 0.0F, 0.0F}, 0.08F,
};
/* clang-format on */
/* the whole of c6_ht16, drawn 32 pixels square about its center */
static NJS_TEXANIM glow_cell = {
#include "shared/mech_boss_beam_glow/glow_cell.inc"
};
static NJS_SPRITE glow_sprite = {
#include "shared/mech_boss_beam_glow/glow_sprite.inc"
};

static void glow_destroy(struct task *task);
static void glow_step(struct task *task);
static void glow_draw(struct task *task);

struct task *mech_boss_beam_glow_create(struct task *parent,
                                        struct mech_boss_beam_glow_spec *spec)
{
    struct task *task;
    struct glow_work *work;
    struct glow_motion *motion;

    if (parent != NULL) {
        task = task_create_child(TASK_MOTION | TASK_WORK, glow_step, parent);
        if (task == NULL) {
            return NULL;
        }
        work = WORK(task);
        motion = task->motion.p;
        work->pos.x = spec->target->x + spec->offset.x;
        work->pos.y = spec->target->y + spec->offset.y;
        work->pos.z = spec->target->z + spec->offset.z;
        work->size = START_SIZE;
        work->growth = glow_tuning.growth;
        motion->alpha = START_ALPHA;
        motion->accel = spec->accel;
        motion->target = spec->target;
        motion->radius = spec->radius;
        task->draw = glow_draw;
        task->destructor = glow_destroy;
        return task;
    }
#ifdef NONMATCHING
    /* for a null parent the image returns R0 as the caller left it, which C
     * can't give. this side returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static void glow_destroy(struct task *task)
{
}

static void glow_step(struct task *task)
{
    struct glow_work *work = WORK(task);
    struct glow_motion *motion = task->motion.p;
    NJS_POINT3 *pos;
    NJS_VECTOR diff;
    NJS_VECTOR dir;
    NJS_POINT3 target;
    Float dist;
    Float near;

    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case MODE_FLY:
        /* @hack spelling: pos, a local for motion->target, where
         * motion->target read in place would do */
        pos = motion->target;
        target = *pos;
        diff.x = target.x - work->pos.x;
        diff.y = target.y - work->pos.y;
        diff.z = target.z - work->pos.z;
        dir = diff;
        if (work->size < glow_tuning.full_size) {
            work->size += work->growth;
        }
        njUnitVector(&dir);
        /* @hack redundant: spd damped, x twice and z never, where the stores
         * after it replace all three */
        motion->spd.x *= DAMPING;
        motion->spd.y *= DAMPING;
        motion->spd.x *= DAMPING;
        work->speed += motion->accel;
        motion->spd.x = dir.x * work->speed;
        motion->spd.y = dir.y * work->speed;
        motion->spd.z = dir.z * work->speed;
        work->pos.x += motion->spd.x;
        work->pos.y += motion->spd.y;
        work->pos.z += motion->spd.z;
        dist = njScalor(&diff);
        near = *motion->radius * NEAR_SHARE;
        if (dist < near) {
            work->mode = MODE_FADE;
        }
        if (work->timer++ >= FLIGHT_STEPS) {
            work->mode = MODE_FADE;
        }
        break;
    case MODE_FADE:
        motion->alpha -= glow_tuning.fade;
        if (motion->alpha < END_ALPHA) {
            task_kill(task);
        }
        break;
    }
}

static void glow_draw(struct task *task)
{
    struct glow_work *work = WORK(task);
    struct glow_motion *motion = task->motion.p;
    /* @hack spelling: sprite, a pointer to glow_sprite, where glow_sprite
     * named in place would do */
    NJS_SPRITE *sprite = &glow_sprite;
    Float alpha;

    sprite->sx = sprite->sy = work->size;
    sprite->p = work->pos;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    alpha = motion->alpha;
    constant_material_set(alpha, alpha, alpha, alpha);
    njSetTexture(&grobo_eff_texlist);
    njDrawSprite3D(sprite, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}
