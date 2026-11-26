/* BOMBE in Security Hall's object list: a canister that a player's attack
 * breaks. it breaks into its cap, its base and four pieces of its wall */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/white_breath.h"
#include "shared/dust.h"
#include "effect/burst.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg08/fixture_art.h"
#include "stg08/cylinder_tex.h"
#include "stg08/machines.h"
#include "system/angle.h"
#include "system/task.h"

#define BURST_POINTS 20
#define BURST_SOUND_KIND 9
/* the pieces fly out at PIECE_SPEED plus up to PIECE_SPREAD at random, with
 * PIECE_LIFT up. their floor is the highest at or below the point
 * FLOOR_LOOK_HEIGHT above the canister's pos */
#define PIECE_SPEED 0.9F
#define PIECE_SPREAD 0.5F
#define PIECE_LIFT 0.7F
#define FLOOR_LOOK_HEIGHT 1.0F
#define PIECE_FLAGS (DEBRIS_TRAIL | DEBRIS_LANDING_BURST | DEBRIS_VANISH)

NJS_TEXLIST *security_cylinder_textures[] = {
    &security_cylinder_texlist,
    &security_cylinder_cap_texlist,
    &security_cylinder_wall_texlist,
    &security_cylinder_base_texlist,
    &white_breath_texlist,
    &burst_texlist,
    &dust_texlist,
    NULL,
};

static Bool compiled = FALSE;
static struct model_build builds[] = {{MODEL_BUILD_ALONE,
                                       &security_cylinder_texlist,
                                       security_cylinder_object, NULL}};
/* clang-format off */
static struct debris_part pieces[] = {
    {&security_cylinder_cap_texlist, &security_cylinder_cap_model,
     {0.0F, 23.199997F, 0.0F}, {0, 0, 0}},
    {&security_cylinder_base_texlist, &security_cylinder_base_model,
     {0.0F, 1.2F, 0.0F}, {0, 0, 0}},
    {&security_cylinder_wall_texlist, &security_cylinder_wall_model,
     {-2.0F, 11.099998F, -2.0F}, {0, 0, 0}},
    {&security_cylinder_wall_texlist, &security_cylinder_wall_model,
     {2.0F, 11.0F, -2.0F}, {0, -QUARTER_TURN, 0}},
    {&security_cylinder_wall_texlist, &security_cylinder_wall_model,
     {-2.0F, 11.0F, 2.0F}, {0, QUARTER_TURN, 0}},
    {&security_cylinder_wall_texlist, &security_cylinder_wall_model,
     {2.0F, 10.999998F, 2.1F}, {0, HALF_TURN, 0}},
};
static struct collision_shape shape = {
    0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE, {0.0F, 12.5F, 0.0F}, 8.0F, 14.5F, 0.0F,
    0, {0, 0, 0}};
/* clang-format on */

static void cylinder_destroy(struct task *task);
static void cylinder_step(struct task *task);
static void cylinder_draw(struct task *task);
static void cylinder_draw_compiled(struct task *task);

Uint8 *security_cylinder_build(Uint8 *buffer)
{
    if (model_builds_compile(&buffer, builds, 1) == 1) {
        compiled = TRUE;
    }
    return buffer;
}

void security_cylinder_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        if (compiled) {
            task->draw = cylinder_draw_compiled;
        } else {
            task->draw = cylinder_draw;
        }
        task->step = cylinder_step;
        task->destructor = cylinder_destroy;
        collision_init(task, &shape, 1, COLLISION_OBJECT);
    }
}

static void cylinder_destroy(struct task *task)
{
    /* @hack redundant: a statement kept for its string, the set editor's
     * label for ang.y, which SHC places in this source's constants */
    (void)"<- ANGLE Y";
}

static void cylinder_step(struct task *task)
{
    struct task_work *work = task->work;
    struct collision_work *collision;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR lift;
    Float floor;

    if (placement_try_leave(task)) {
        return;
    }
    collision = work->collision;
    if (collision && (collision->flags & COLLISION_TOUCHED) &&
        player_attacks_task(task, collision->hit->owner)) {
        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        score_add(BURST_POINTS);
        if (task->placement) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
        floor =
            surface_probe_floor(pos.x, pos.y + FLOOR_LOOK_HEIGHT, pos.z, &ang);
        debris_break_flags(&pos, work->ang.y, pieces,
                           sizeof(pieces) / sizeof(pieces[0]), PIECE_SPEED,
                           PIECE_SPREAD, floor, &lift, PIECE_FLAGS);
        object_sound_play(BURST_SOUND_KIND, &work->pos);
    }
    collision_register(task);
}

static void cylinder_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&security_cylinder_texlist);
    njCnkEasyDrawModel(security_cylinder_object->model);
    njPopMatrix(1);
}

static void cylinder_draw_compiled(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&security_cylinder_texlist);
    njDirectDrawObject(builds[0].built);
    njPopMatrix(1);
}
