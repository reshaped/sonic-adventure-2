/* the fire pot, FIREPOT in Pyramid Cave's object list: a solid pot with a fire
 * burning on it */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg28/cave_util.h"
#include "stg28/firepot.h"
#include "stg28/pyramid_cave.h"
#include "system/port.h"
#include "system/task.h"
#include "system/util.h"

#include "stg28/firepot/text.inc"
#include "stg28/models/firepot.h"

#define FIREPOT_KINDS 1
#define KIND(work) ((Sint32)njAbs((work)->scl.z) % FIREPOT_KINDS)

#define SCALE_ACROSS(work) ((work)->scl.x + 1.0F)
#define SCALE_UP(work) ((work)->scl.y + 1.0F)

/* the pot's shape in the collision world collides only while a player lies near
 * its pos. near is within REACH_MARGIN plus POT_REACH times the pot's larger
 * scale */
#define POT_REACH 70.58F
#define REACH_MARGIN 10.0F
/* the fire draws FIRE_HEIGHT above the pot's pos, and its sound plays
 * SOUND_HEIGHT above that pos. the pot's scale up scales both */
#define FIRE_HEIGHT 58.2466F
#define SOUND_HEIGHT 8.0F
/* the fire's sound, an id in pyramid_cave_sound_list. a play of it lasts
 * FIRE_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#define FIRE_SOUND 5
#define FIRE_SOUND_PRIORITY 1
#define FIRE_SOUND_STEPS 15

#define FIREPOT_SHAPES 2
enum cave_firepot_shape { FIRE_SHAPE, POT_SHAPE };
/* the fire hurts a player whose guard lies below level 2 */
#define FIRE_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))

KEPT static NJS_CNK_MODEL *firepot_models[FIREPOT_KINDS] = {
#include "stg28/firepot/firepot_models.inc"
};
KEPT static NJS_CNK_MODEL *flame_models[FIREPOT_KINDS] = {
#include "stg28/firepot/flame_models.inc"
};
NJS_TEXLIST *cave_firepot_texlists[FIREPOT_KINDS + 1] = {
#include "stg28/firepot/cave_firepot_texlists.inc"
};
KEPT static NJS_TEXLIST *fire_texlists[FIREPOT_KINDS + 1] = {
#include "stg28/firepot/fire_texlists.inc"
};
static char *firepot_kind_names[FIREPOT_KINDS] = {
#include "stg28/firepot/firepot_kind_names.inc"
};
/* the fire's shape and the pot's, at a scale of 1 */
/* clang-format off */
static struct collision_shape firepot_shapes[FIREPOT_SHAPES] = {
    {0, SHAPE_CAPSULE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), FIRE_LEVELS,
     SHAPE_HURTABLE, {0.0F, 55.0F, 0.0F}, 20.0F, 25.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL),
     SHAPE_GUARD_LEVEL(SHAPE_TOP_LEVEL), SHAPE_UNK_2000,
     {0.0F, 10.0F, 0.0F}, 30.0F, 10.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the fire's volume, and how near player 0 must lie for the step to play its
 * sound */
static Sint32 fire_volume = 25;
static Float fire_range = 100.0F;

static void firepot_destroy(struct task *task);
static void firepot_step(struct task *task);
static void firepot_draw(struct task *task);

void cave_firepot_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    struct collision_shape shapes[FIREPOT_SHAPES];

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ANGLE Y (   )";
        (void)"<- SCALE XZ(1.0+x)";
        (void)"<- SCALE Y (1.0+x)";
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = firepot_draw;
    task->step = firepot_step;
    task->destructor = firepot_destroy;
    shapes[FIRE_SHAPE] = firepot_shapes[FIRE_SHAPE];
    shapes[POT_SHAPE] = firepot_shapes[POT_SHAPE];
    shapes[FIRE_SHAPE].radius *= SCALE_ACROSS(task->work);
    shapes[FIRE_SHAPE].size_y *= SCALE_UP(task->work);
    shapes[FIRE_SHAPE].center.y *= SCALE_UP(task->work);
    shapes[POT_SHAPE].radius *= SCALE_ACROSS(task->work);
    shapes[POT_SHAPE].size_y *= SCALE_UP(task->work);
    shapes[POT_SHAPE].center.y *= SCALE_UP(task->work);
    collision_init(task, shapes, FIREPOT_SHAPES, COLLISION_OBJECT);
    object = world_pool_alloc();
    task->extra.p = object;
    /* @bug memory: copies the collision object through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c508da0). firepot_destroy
     * tests task->extra for null (TST at 0x8c508e4c). firepot_step hands
     * player_near the copy's pos untested (ADD at 0x8c508eb4) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the fire pot, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = cave_firepot_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    object->scl[0] = object->scl[2] = SCALE_ACROSS(task->work);
    object->scl[1] = SCALE_UP(task->work);
    object->ang[0] = 0;
    object->ang[1] = work->ang.y;
    object->ang[2] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    world_add(WORLD_SOLID | WORLD_SIZE_CLASS(1), task, object);
    work->ang.x = work->ang.z = 0;
    FIRE_FRAME(task) = rand() * (FIRE_FRAMES * RAND_SCALE);
}

static void firepot_destroy(struct task *task)
{
    if (task->extra.p) {
        world_remove(task, task->extra.p);
        world_pool_free(task->extra.p);
    }
    task->extra.p = NULL;
}

static void firepot_step(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    NJS_POINT3 pos;
    Float across;
    Float up;
    Float size;

    if (placement_try_leave(task)) {
        return;
    }
    object = task->extra.p;
    across = SCALE_ACROSS(task->work);
    up = SCALE_UP(task->work);
    if (up < across) {
        size = across;
    } else {
        size = up;
    }
    /* @hack redundant: a block no path reaches */
    if (0) {
        if (up) {
        }
        task_destroy(NULL);
        task_destroy(NULL);
        task_destroy(NULL);
    }
    if (player_near((NJS_POINT3 *)object->pos,
                    (size * POT_REACH) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    collision_register(task);
    pos = work->pos;
    pos.y += SCALE_UP(task->work) * SOUND_HEIGHT;
    if (fire_range > njDistanceP2P(&player_work[0]->pos, &pos)) {
        sound_play_at_for(FIRE_SOUND, task, FIRE_SOUND_PRIORITY, fire_volume,
                          FIRE_SOUND_STEPS, &pos);
    }
    if (++FIRE_FRAME(task) >= FIRE_FRAMES) {
        FIRE_FRAME(task) = 0;
    }
}

static void firepot_draw(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;
    NJS_VECTOR scale;
    Sint16 texture;

    scale.x = scale.z = SCALE_ACROSS(task->work);
    scale.y = SCALE_UP(task->work);
    FIRE_SET_FRAME(flame_models[KIND(task->work)], FIRE_FRAME(task), texture);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njSetTexture(cave_firepot_texlists[KIND(task->work)]);
    cave_model_draw_scaled_v(firepot_models[KIND(task->work)], &scale);
    njPopMatrixEx();
    njTranslate(NULL, 0.0F, SCALE_UP(task->work) * FIRE_HEIGHT, 0.0F);
    /* the turn about y cancels the pot's and takes the camera's. so the fire
     * faces the camera from every side, but doesn't tilt toward it */
    njRotateY(NULL, -work->ang.y + camera_views[camera_current_index]->ang.y);
    njFogDisable();
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(ADDITIVE_BLEND);
    njSetTexture(fire_texlists[KIND(task->work)]);
    cave_model_draw_scaled_v(flame_models[KIND(task->work)], &scale);
    njSetCnkBlendMode(ALPHA_BLEND);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_control_restore();
    njFogEnable();
    njPopMatrixEx();
}
