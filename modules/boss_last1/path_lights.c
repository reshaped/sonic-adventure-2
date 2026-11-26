#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "boss_last1/arena.h"
#include "boss_last1/path_lights.h"
#include "draw/lines.h"
#include "stage/path.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/vector.h"

/* a light's sprite scale swings LIGHT_PULSE either side of LIGHT_SIZE as
 * its work's ang.x turns. each step turns ang.x and ang.y by LIGHT_TURN */
#define LIGHT_SIZE 0.2F
#define LIGHT_PULSE 0.05F
#define LIGHT_TURN 0x1000

/* a light's run along its path. point is the first point of the light's
 * segment, and distance the light's distance from it */
struct moving_light {
    Float speed;
    Float distance;
    Uint16 segment; /* point's index in the path */
    Uint16 count; /* the path's count */
    Angle turn_x;
    Angle turn_y;
    NJS_VECTOR direction; /* the segment's, as a unit vector */
    struct path_point *point;
};
struct path_emitter {
    Float speed; /* a copy of light_speed, which nothing reads */
    Sint16 count;
    struct path *path;
};

/* a light leaves every emission_interval[0] + 1 steps. nothing reads
 * emission_interval[1] */
static Uint16 emission_interval[2] = {10, 4};
static Float light_speed = 3.0F; /* a light's run along its path each step */

static void segment_direction(struct path_point *point, NJS_VECTOR *out)
{
    NJS_POINT3 ends[2];
    NJS_VECTOR direction;
    /* @hack spelling: end, a cursor stepped to the segment's end, where
     * point[1] would do */
    struct path_point *end = point;
    ends[0] = end->pos;
    end++;
    ends[1] = end->pos;
    vector_diff(&ends[1], &ends[0], &direction);
    njUnitVector(&direction);
    *out = direction;
}

static void light_step(struct task *task)
{
    struct task_work *work = task->work;
    struct moving_light *light = task->extra.p;
    struct path_point *point = light->point;
    NJS_VECTOR offset;
    if (!game_state.time_stopped) {
        light->distance += light->speed;
        if (light->distance > light->point->length) {
            light->segment++;
            if (light->segment != light->count - 1) {
                light->distance -= point->length;
                point++;
                segment_direction(point, &light->direction);
                light->point = point;
            } else {
                task_kill(task);
                return;
            }
        }
        offset.x = light->direction.x * light->distance;
        offset.y = light->direction.y * light->distance;
        offset.z = light->direction.z * light->distance;
        work->ang.x += light->turn_x;
        work->ang.y += light->turn_y;
        vector_sum(&point->pos, &offset, &work->pos);
    }
}

static void light_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXANIM light_cell = {
#include "boss_last1/path_lights/light_cell.inc"
    };
    NJS_SPRITE sprite = {{0, 0, 0}, 1.0F, 1.0F, 0, &biolizard_effect_texlist,
                         NULL};
    NJS_SPRITE *draw;
    /* @hack spelling: the sprite set through draw, its address, where the
     * sprite itself would do */
    draw = &sprite;
    njSetTexture(&biolizard_effect_texlist);
    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    draw->sx = draw->sy = LIGHT_SIZE + (LIGHT_PULSE * njSin(work->ang.x));
    draw->p = work->pos;
    draw->tanim = &light_cell;
    njDrawSprite3D(draw, 0,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void light_destroy(struct task *task)
{
}

static struct task *light_create(struct task *parent, struct path_point *point,
                                 Sint8 count)
{
    struct task *task;
    struct task_work *work;
    struct moving_light *light;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, light_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    light = syCalloc(1, sizeof(*light));
    if (!light) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = light;
    light->point = point;
    light->speed = light_speed;
    light->distance = 0.0F;
    light->segment = 0;
    light->count = count;
    light->turn_x = LIGHT_TURN;
    light->turn_y = LIGHT_TURN;
    work->pos = point->pos;
    segment_direction(light->point, &light->direction);
    task->draw = light_draw;
    task->destructor = light_destroy;
    return task;
}

static void path_emitter_step(struct task *task)
{
    struct task_work *work = task->work;
    struct path_emitter *emitter = task->extra.p;
    if (!game_state.time_stopped && !game_state.unk_09) {
        if (!work->timer--) {
            light_create(task, emitter->path->points, emitter->count);
            work->timer = emission_interval[0];
        }
    }
}

static void path_emitter_draw(struct task *task)
{
}
static void path_emitter_destroy(struct task *task)
{
}

struct task *biolizard_path_lights_create(struct task *parent,
                                          struct path *path)
{
    struct task *task;
    struct task_work *work;
    struct path_emitter *emitter;
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, path_emitter_step, parent);
    if (!task) {
        return NULL;
    }
    work = task->work;
    emitter = syCalloc(1, sizeof(*emitter));
    if (!emitter) {
        task_kill(task);
        return NULL;
    }
    task->extra.p = emitter;
    emitter->path = path;
    emitter->count = emitter->path->count;
    emitter->speed = light_speed;
    work->timer = emission_interval[0];
    task->draw = path_emitter_draw;
    task->destructor = path_emitter_destroy;
    return task;
}
