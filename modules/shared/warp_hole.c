/* warp holes, tasks named WpHoleExec: swarms of points that swirl into a core
 * or out of it. the points draw as glowing sprites. warp_hole_particle_create
 * makes particles of the same texture */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "draw/color.h"
#include "effect/particle.h"
#include "object/set.h"
#include "shared/warp_hole.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* a hole's record, in its task's motion */
struct warp_hole {
    NJS_POINT3 *points;
    /* each point's steps left until warp_hole_place places it anew */
    Uint16 *timers;
    Sint32 count; /* the length of points and of timers */
    struct task **owner; /* where the hole keeps its task, or null */
};

/* @hack type: motion read as a void * through its address, where
 * (task)->motion.p would do */
#define HOLE(task) ((struct warp_hole *)*(void **)&(task)->motion)
/* the task level of a hole */
#define HOLE_LEVEL 3

/* warp_hole_place's flags. a point swirls in to the core from a distance
 * warp_hole_place picks. a PLACE_OUTWARD point swirls out from the core to
 * that distance instead. the distance lies near the edge, or with
 * PLACE_ANYWHERE anywhere from the core to the edge */
#define PLACE_OUTWARD 0x1
#define PLACE_ANYWHERE 0x2
/* a point's steps are held to 1 through MAX_STEPS */
#define MAX_STEPS 65000.0F

/* each draw draws the points warp_hole_passes times, and each time is a pass.
 * the last pass's sprites are white. each pass before it is PASS_DIMMING darker
 * than the next. each of red, green and blue runs up to CHANNEL_MAX */
#define PASS_DIMMING 32
#define CHANNEL_MAX 255
#define CHANNEL_BITS 8
/* the packed RGB color whose three channels are each gray */
#define GRAY(gray)                                                             \
    (((gray) << (2 * CHANNEL_BITS)) + ((gray) << CHANNEL_BITS) + (gray))
#define SPRITE_OPAQUE 0xff000000
/* the texture of warp_hole_texlist the sprites take, and njPtclSpriteStart's
 * flag for the translucent list */
#define SPRITE_TEXTURE 0
#define TRANSLUCENT 1

/* a particle's alpha falls by FADE_STEP a step, and it ends once its alpha is
 * below FADE_END. by a chance of DRIFT_CHANCE a step, its speed takes a random
 * push. the push is up to DRIFT either way in x and in z */
#define FADE_STEP 2
#define FADE_END 10
#define DRIFT_CHANCE 0.03F
#define DRIFT 0.1F
/* a particle spins at a random rate of up to SPIN degrees a step either way.
 * its edge_angle is a quarter turn plus a random angle of up to EDGE_SPREAD
 * degrees */
#define SPIN 1.5F
#define EDGE_SPREAD 20.0F

static void warp_hole_step(struct task *task);
static void warp_hole_draw(struct task *task);
static void warp_hole_destroy(struct task *task);
static int warp_hole_particle_step(struct particle_group *group,
                                   struct particle *particle);

#include "shared/warp_hole/text.inc"

static NJS_TEXNAME warp_hole_texname[1] = {
#include "shared/warp_hole/warp_hole_texname.inc"
};
NJS_TEXLIST warp_hole_texlist = {warp_hole_texname, 1};
/* the scale a step gives a point's offset from the hole's center. an outward
 * hole's step scales by its inverse */
static Float warp_hole_shrink = 0.96F;
/* the degrees a step turns a point about y, then x, then z, once scaled. an
 * outward hole's step turns each the other way */
static Float warp_hole_turn_y = -1.0F;
static Float warp_hole_turn_x = 0.5F;
static Float warp_hole_turn_z = 1.9F;
/* the size of a pass's sprites, as a fraction of the next pass's. the last
 * pass draws at the hole's size */
static Float warp_hole_pass_size_ratio = 0.75F;
/* the core's radius is warp_hole_core * scale. the edge lies scale beyond
 * the core */
static Float warp_hole_core = 0.07F;
/* a distance near the edge lies no more than warp_hole_spread * scale inside
 * the edge */
static Float warp_hole_spread = 0.2F;
/* each pass after the first draws the points where a step would move them
 * from the pass before */
static Uint32 warp_hole_passes = 2;
/* clang-format off */
static struct particle_group warp_hole_particle_group = {
    PARTICLE_BLEND_ADD, &warp_hole_texlist,
    0, 1, 0.0F, 0.975F, 0.0018F, -0.002F,
    warp_hole_particle_step, 75000.0F, NULL, 0, NULL, NULL,
};
/* clang-format on */

/* place point anew, in a random direction from the hole's center, at a
 * distance flags pick. set *timer to the steps the swirl takes between that
 * distance and the core */
static void warp_hole_place(NJS_POINT3 *point, Float scale, Uint16 *timer,
                            Uint8 flags)
{
    Float core;
    Float distance;
    Float steps;
    Float fraction;
    Float ratio;

    core = warp_hole_core * scale;
    if (flags & PLACE_ANYWHERE) {
        distance = core + ((Float)rand() * scale * RAND_SCALE);
    } else {
        fraction = (warp_hole_spread * (Float)rand() * RAND_SCALE) -
                   warp_hole_spread + 1.0F;
        distance = core + (fraction * scale);
    }
    point->x = 0.0F;
    point->y = distance;
    point->z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, (Angle)RANDOM_TURN());
    njRotateX(NULL, (Angle)RANDOM_TURN());
    njCalcPoint(NULL, point, point);
    njPopMatrix(1);
    /* @hack spelling: core / distance worked out here and again for ratio,
     * where ratio set once before the logf would do */
    steps = logf(core / distance) / logf(warp_hole_shrink);
    if (steps > MAX_STEPS) {
        steps = MAX_STEPS;
    }
    if (steps < 1.0F) {
        steps = 1.0F;
    }
    *timer = (Uint16)steps;
    if (flags & PLACE_OUTWARD) {
        ratio = core / distance;
        point->x *= ratio;
        point->y *= ratio;
        point->z *= ratio;
    }
}

struct task *warp_hole_create(NJS_POINT3 *pos, Float scale, int count,
                              Uint8 outward, Float size, struct task **owner)
{
    struct task *task;
    int i;

    task = task_create(TASK_WORK, HOLE_LEVEL, warp_hole_step, "WpHoleExec");
    /* @hack spelling: the rest nested in the tests of task and of
     * placement_try_leave, where a return of null after each would do */
    if (task != NULL) {
        task->work->pos = *pos;
        if (!placement_try_leave(task)) {
            task->motion.p = syCalloc(1, sizeof(struct warp_hole));
            if (task->motion.p == NULL) {
                task_destroy(task);
                return NULL;
            }
            HOLE(task)->points = syCalloc(count, sizeof(NJS_POINT3));
            HOLE(task)->timers = syCalloc(count, sizeof(Uint16));
            if (HOLE(task)->points == NULL || HOLE(task)->timers == NULL) {
                syFree(HOLE(task)->points);
                syFree(HOLE(task)->timers);
                syFree(task->motion.p);
                task->motion.p = NULL;
                task_destroy(task);
                return NULL;
            }
            task->draw = warp_hole_draw;
            task->step = warp_hole_step;
            task->destructor = warp_hole_destroy;
            WARP_HOLE_SCALE(task) = scale;
            WARP_HOLE_SIZE(task) = size;
            HOLE(task)->count = count;
            if (outward) {
                outward = PLACE_OUTWARD;
            }
            task->work->mode = outward;
            for (i = 0; i < count; i++) {
                warp_hole_place(&HOLE(task)->points[i], scale,
                                &HOLE(task)->timers[i],
                                outward | PLACE_ANYWHERE);
            }
            HOLE(task)->owner = owner;
            if (owner != NULL) {
                *owner = task;
            }
            return task;
        }
    }
    return NULL;
}

static void warp_hole_step(struct task *task)
{
    Uint16 *timer;
    Float grow;
    int count;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    if (task->work->mode) {
        njRotateZ(NULL, (Angle)(-warp_hole_turn_z * ANGLE_PER_DEGREE));
        njRotateX(NULL, (Angle)(-warp_hole_turn_x * ANGLE_PER_DEGREE));
        njRotateY(NULL, (Angle)(-warp_hole_turn_y * ANGLE_PER_DEGREE));
        grow = 1.0F / warp_hole_shrink;
        njScale(NULL, grow, grow, grow);
    } else {
        njRotateZ(NULL, (Angle)(warp_hole_turn_z * ANGLE_PER_DEGREE));
        njRotateX(NULL, (Angle)(warp_hole_turn_x * ANGLE_PER_DEGREE));
        njRotateY(NULL, (Angle)(warp_hole_turn_y * ANGLE_PER_DEGREE));
        njScale(NULL, warp_hole_shrink, warp_hole_shrink, warp_hole_shrink);
    }
    njCalcPoints(NULL, HOLE(task)->points, HOLE(task)->points,
                 HOLE(task)->count);
    njPopMatrix(1);
    count = HOLE(task)->count;
    timer = HOLE(task)->timers;
    for (i = 0; i < count; i++, timer++) {
        if ((*timer)-- == 0) {
            warp_hole_place(&HOLE(task)->points[i], WARP_HOLE_SCALE(task),
                            timer, task->work->mode);
        }
    }
}

static void warp_hole_draw(struct task *task)
{
    Float size;
    Float grow;
    int gray;
    Uint32 color;
    int i;

    njFogDisable();
    njPushMatrixEx();
    njTranslateV(NULL, &task->work->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&warp_hole_texlist);
    for (i = 0; (Uint32)i < warp_hole_passes; i++) {
        if (i > 0) {
            if (task->work->mode) {
                njRotateZ(NULL, (Angle)(-warp_hole_turn_z * ANGLE_PER_DEGREE));
                njRotateX(NULL, (Angle)(-warp_hole_turn_x * ANGLE_PER_DEGREE));
                njRotateY(NULL, (Angle)(-warp_hole_turn_y * ANGLE_PER_DEGREE));
                grow = 1.0F / warp_hole_shrink;
                njScale(NULL, grow, grow, grow);
            } else {
                njRotateZ(NULL, (Angle)(warp_hole_turn_z * ANGLE_PER_DEGREE));
                njRotateX(NULL, (Angle)(warp_hole_turn_x * ANGLE_PER_DEGREE));
                njRotateY(NULL, (Angle)(warp_hole_turn_y * ANGLE_PER_DEGREE));
                njScale(NULL, warp_hole_shrink, warp_hole_shrink,
                        warp_hole_shrink);
            }
        }
        size = WARP_HOLE_SIZE(task) *
               powf(warp_hole_pass_size_ratio, warp_hole_passes - i - 1);
        gray = CHANNEL_MAX - ((warp_hole_passes - i - 1) * PASS_DIMMING);
        color = GRAY(gray) + SPRITE_OPAQUE;
        njPtclSpriteStart(SPRITE_TEXTURE, color, TRANSLUCENT);
        njPtclDrawSprite(HOLE(task)->points, HOLE(task)->count, size, size);
        njPtclSpriteEnd();
    }
    njPopMatrix(1);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void warp_hole_destroy(struct task *task)
{
    if (HOLE(task)->owner != NULL && *HOLE(task)->owner == task) {
        *HOLE(task)->owner = NULL;
    }
    syFree(HOLE(task)->points);
    syFree(HOLE(task)->timers);
    /* force and extra hold the scale and the size as Floats. task_destroy
     * would free them as pointers */
    task->force.p = NULL;
    task->extra.p = NULL;
    syFree(task->motion.p);
    task->motion.p = NULL;
}

struct particle *warp_hole_particle_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale)
{
    struct particle *particle;

    particle = particle_create(&warp_hole_particle_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = (Angle)RANDOM_TURN();
        particle->edge_angle =
            (Angle)((Float)rand() *
                    (EDGE_SPREAD * ANGLE_PER_DEGREE * RAND_SCALE)) +
            QUARTER_TURN;
        particle->ang_spd = RANDOM_ANGLE(SPIN);
    }
    return particle;
}

/* the particles' step. it moves a particle as particle_move does, but adds a
 * random drift. it fades the particle's alpha where particle_move steps its
 * frame */
static int warp_hole_particle_step(struct particle_group *group,
                                   struct particle *particle)
{
    Uint32 alpha;

    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    if ((Float)rand() * RAND_SCALE < DRIFT_CHANCE) {
        particle->spd.x += (((Float)rand() * RAND_SCALE) - 0.5F) * (2 * DRIFT);
        particle->spd.z += (((Float)rand() * RAND_SCALE) - 0.5F) * (2 * DRIFT);
    }
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    alpha = (Uint8)(particle->color >> ALPHA_SHIFT);
    if (alpha < FADE_END || particle->scale < 0.0F) {
        return 0;
    }
    alpha -= FADE_STEP;
    particle->color = (particle->color & ~COLOR_ALPHA) | (alpha << ALPHA_SHIFT);
    /* @bug uninitialized: returns no value where the particle lives on.
     * particle_step_all frees a particle whose step returns 0, by the
     * TST R0,R0 at 0x8c036e84. the RTS at 0x85e of the object's code returns
     * scale's offset in the particle, which MOV #4,R0 at 0x834 set. it is
     * nonzero, so the particle lives */
#ifdef NONMATCHING
    /* this side keeps the tag's result. it returns 1, nonzero as scale's offset
     * is */
    return 1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
