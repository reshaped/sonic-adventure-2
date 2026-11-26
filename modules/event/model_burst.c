#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "event/effect_track.h"
#include "event/loader.h"
#include "event/model_burst.h"
#include "system/angle.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"
#include "system/vector.h"

static const char texture_name[32] = {
#include "event/model_burst/texture_name.inc"
};

struct vertex_normal {
    NJS_POINT3 position;
    NJS_VECTOR normal;
};

struct crystal_vertex_chunk {
    Sint32 header, count;
    struct vertex_normal vertices[12];
    Sint32 end;
};

struct shard_vertex_chunk {
    Sint32 header, count;
    struct vertex_normal vertices[4];
    Sint32 end;
};

static Sint16 crystal_polygons[83] = {
#include "event/model_burst/crystal_polygons.inc"
};
static struct crystal_vertex_chunk crystal_vertices = {
#include "event/model_burst/crystal_vertices.inc"
};
static NJS_CNK_MODEL crystal_model = {
#include "event/model_burst/crystal_model.inc"
};
static NJS_CNK_OBJECT crystal = {
#include "event/model_burst/crystal.inc"
};

static Sint16 long_shard_polygons[21] = {
#include "event/model_burst/long_shard_polygons.inc"
};
static struct shard_vertex_chunk long_shard_vertices = {
#include "event/model_burst/long_shard_vertices.inc"
};
static NJS_CNK_MODEL long_shard_model = {
#include "event/model_burst/long_shard_model.inc"
};
static NJS_CNK_OBJECT long_shard = {
#include "event/model_burst/long_shard.inc"
};

static Sint16 short_shard_polygons[21] = {
#include "event/model_burst/short_shard_polygons.inc"
};
static struct shard_vertex_chunk short_shard_vertices = {
#include "event/model_burst/short_shard_vertices.inc"
};
static NJS_CNK_MODEL short_shard_model = {
#include "event/model_burst/short_shard_model.inc"
};
static NJS_CNK_OBJECT short_shard = {
#include "event/model_burst/short_shard.inc"
};

static NJS_TEXNAME textures[1] = {
#include "event/model_burst/textures.inc"
};
NJS_TEXLIST event_effect_texlist = {
#include "event/model_burst/event_effect_texlist.inc"
};
/* the models a burst's model indexes: the crystal, then its long and short
 * shards */
KEPT static NJS_CNK_OBJECT *models[3] = {
#include "event/model_burst/models.inc"
};

#define RANDOM_UP_TO(limit) ((limit) * (Float)rand() * RAND_SCALE)
/* a random factor from 0.5 to 1.5 */
#define RANDOM_FACTOR() (RANDOM_UP_TO(1.0F) + 0.5F)
/* the most a piece turns a frame about each axis, in degrees */
#define SPIN_DEGREES 15.0F
#define BURST_LIFETIME 180
/* the task level of a burst's pieces */
#define EFFECT_LEVEL 2

static void burst_step(struct task *task);
static void burst_draw(struct task *task);
static void burst_destroy(struct task *task);

static void launch_velocity(struct angles *direction, Sint32 spread,
                            Float speed, NJS_VECTOR *velocity)
{
    struct angles angles;
    angles = *direction;
    /* each angle turns by spread / 2 either way, that half rounded toward 0 */
    /* NOLINTBEGIN(bugprone-integer-division) */
    angles.x += RANDOM_UP_TO(spread) - (Float)(spread / 2);
    angles.y += RANDOM_UP_TO(spread) - (Float)(spread / 2);
    angles.z += RANDOM_UP_TO(spread) - (Float)(spread / 2);
    /* NOLINTEND(bugprone-integer-division) */
    velocity->x = 0.0F;
    velocity->y = speed * RANDOM_FACTOR();
    velocity->z = 0.0F;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njRotateXYZ(NULL, angles.x, angles.y, angles.z);
    njCalcVector(NULL, velocity, velocity);
    njPopMatrix(1);
}

void event_model_burst_start(struct event_model_burst *burst)
{
    struct task *task;
    struct event_model_burst *piece;
    int i;

    for (i = 0; i < burst->count; i++) {
        task =
            task_create(0, EFFECT_LEVEL, burst_step, "singleModelEffectExec");
        task->draw = burst_draw;
        task->destructor = burst_destroy;
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((task->work = syMalloc(sizeof(*piece))) == NULL) {
            /* @bug mismatched-call: on a failed allocation, 0x8c50791e
             * passes the task to task_destroy_level, which takes a level */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it destroys the task */
            task_destroy(task);
#else
            task_destroy_level((int)task);
#endif
            return;
        }
        piece = (struct event_model_burst *)task->work;
        *piece = *burst;
        piece->scale *= RANDOM_FACTOR();
        piece->age = 0;
        launch_velocity(&piece->direction, piece->spread, piece->speed,
                        &piece->velocity);
        piece->rotation.x = NJM_DEG_ANG(RANDOM_UP_TO(360.0F));
        piece->rotation.y = NJM_DEG_ANG(RANDOM_UP_TO(360.0F));
        piece->rotation.z = NJM_DEG_ANG(RANDOM_UP_TO(360.0F));
        piece->spin.x =
            NJM_DEG_ANG(RANDOM_UP_TO(2 * SPIN_DEGREES) - SPIN_DEGREES);
        piece->spin.y =
            NJM_DEG_ANG(RANDOM_UP_TO(2 * SPIN_DEGREES) - SPIN_DEGREES);
        piece->spin.z =
            NJM_DEG_ANG(RANDOM_UP_TO(2 * SPIN_DEGREES) - SPIN_DEGREES);
    }
}

static void burst_step(struct task *task)
{
    struct event_model_burst *piece = (struct event_model_burst *)task->work;
    vector_sum(&piece->position, &piece->velocity, &piece->position);
    vector_sum(&piece->velocity, &piece->acceleration, &piece->velocity);
    piece->rotation.x += piece->spin.x;
    piece->rotation.y += piece->spin.y;
    piece->rotation.z += piece->spin.z;
    piece->age++;
    if (piece->age > BURST_LIFETIME) {
        task_destroy(task);
    }
}

static void burst_draw(struct task *task)
{
    struct event_model_burst *piece = (struct event_model_burst *)task->work;
    njSetTexture(&event_effect_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &piece->position);
    njRotateXYZ(NULL, piece->rotation.x, piece->rotation.y, piece->rotation.z);
    njScale(NULL, piece->scale, piece->scale, piece->scale);
    njCnkSimpleDrawModel(models[piece->model]->model);
    njPopMatrix(1);
#ifdef NONMATCHING
    /* the burst editor can set trail_interval to 0, or trail to a kind that
     * makes no particle */
    if (piece->trail_interval == 0 || piece->trail < 0 ||
        piece->trail >= EVENT_PARTICLES ||
        event_particles[piece->trail] == NULL) {
        return;
    }
#endif
    if (piece->age % piece->trail_interval == 0) {
        event_particles[piece->trail](&piece->position, &piece->velocity, 1.0F);
    }
}

static void burst_destroy(struct task *task)
{
    if (task->work != NULL) {
        syFree(task->work);
        task->work = NULL;
    }
}
