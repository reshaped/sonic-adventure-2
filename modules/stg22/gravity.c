/* the stage task's step, and the background task's step and destructor */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/texture_anim.h"
#include "player/player.h"
#include "stg22/crazy_gadget.h"
#include "stg22/glow.h"
#include "stg22/stage_data.h"
#include "stg22/surfaces.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

#define EVENT_LEVEL 2
#define SAMPLES 4
#define WALL_DISTANCE 30.0F
#define SCROLL_PHASES 4
/* gadget_kill_volumes[TURNED_ONLY] makes player 0 fall out only while gravity
 * doesn't point along -y */
#define TURNED_ONLY 5
#define VOLUME_HALF_SIZE 0.5F
#define VOLUME_DEPTH 30.0F
/* the work's scl.x is the glow's strength, and scl.y its target. on a step
 * that lays the glow, the strength closes GLOW_EASE of its gap to the target.
 * on any other step it keeps GLOW_FADE of itself. the target is 1 while
 * player 0's center lies within GLOW_NEAR of the glow's pos. a distance d
 * past that takes it to glow_falloff / (glow_falloff + d - GLOW_NEAR). a
 * flicker scales the target by the sine of an angle that gains
 * GLOW_FLICKER_SPEED a step. the scale runs GLOW_FLICKER_HALF either side of
 * its middle, from GLOW_FLICKER_LOW up to 1 */
#define GLOW_EASE 0.2F
#define GLOW_NEAR 6.0F
#define GLOW_FLICKER_SPEED 640
#define GLOW_FLICKER_LOW 0.8F
#define GLOW_FLICKER_HALF 0.1F
#define GLOW_FADE 0.98F

static Uint32 animation_speed = 4;
static Sint32 scroll_period = 53;
Float gadget_glow_reach = 9.0F;
static Float glow_falloff = 10.0F;
static Sint16 scroll_u[SCROLL_PHASES] = {0, 128, 0, 128};
static Sint16 scroll_v[SCROLL_PHASES] = {0, 0, 128, 128};

static void events_destroy(struct task *task)
{
    if (task == gadget_event_task) {
        gadget_event_task = NULL;
    }
}

#define GADGET_UP_AXIS stage_gravity_up_axis
#include "stg22/gravity_axis_impl.h"

void crazy_gadget_step(struct task *task)
{
    struct task_work *work = task->work;
    world_gravity_ang_x = (Angle)(asinf(-world_gravity.z) * ANGLE_PER_RADIAN);
    world_gravity_ang_z =
        -(Angle)(atan2f(-world_gravity.x, -world_gravity.y) * ANGLE_PER_RADIAN);
    collision_set_up_axis(stage_gravity_up_axis(&world_gravity));
    /* @hack redundant: a switch on the work's mode whose every case does
     * nothing */
    switch (work->mode) {
    default:
        break;
    case 0:
        break;
    }
    if (!gadget_event_task) {
        gadget_event_task = task_create(TASK_WORK, EVENT_LEVEL,
                                        gadget_events_step, "ObjSetManExec");
        if (gadget_event_task) {
            gadget_event_task->destructor = events_destroy;
        }
    }
}

void gadget_background_step(struct task *task)
{
    struct task_work *work;
    int i;
    int phase;
    int tex_u;
    int tex_v;
    int down;
    Bool found;
    int sample_index;
    int adjacent;
    int opposite;
    int missing;
    Bool near_wall;
    struct surface_probe *first;
    struct surface_probe *sample;
    struct gadget_kill_volume *volume;
    Float rise_a;
    Float rise_b;
    Float height;
    Float target;
    Angle pitch;
    Angle roll;

    work = task->work;
    down = SURFACE_NEG_X;
    ++work->timer;
    if (!GADGET_GLOW(task)) {
        task->unk_3c.p = syCalloc(1, sizeof(struct surface_probe));
    }
    phase = (work->timer / scroll_period) % SCROLL_PHASES;
    tex_u = scroll_u[phase];
    tex_v = scroll_v[phase];
    for (i = 0; (Uint32)i < sizeof(gadget_surface_animations) /
                                sizeof(gadget_surface_animations[0]);
         ++i) {
        if (gadget_surface_animations[i].scroll == 0) {
            texture_anim_apply(gadget_surface_animations[i].object->model,
                               gadget_surface_animations[i].animation,
                               game_state.steps * animation_speed);
        } else {
            texture_anim_scroll(gadget_surface_animations[i].object->model,
                                gadget_surface_animations[i].animation, tex_u,
                                tex_v);
        }
    }
    if (player_work[0] && kill_areas_task && kill_areas_task->child == NULL &&
        (player_records[0]->flags & PLAYER_MISSED) == 0) {
        NJS_POINT3 local;
        volume = gadget_kill_volumes;
        njPushMatrixEx();
        for (i = 0; (Uint32)i < sizeof(gadget_kill_volumes) /
                                    sizeof(gadget_kill_volumes[0]);
             ++i, ++volume) {
            njUnitMatrix(NULL);
            njTranslateEx((NJS_VECTOR *)volume->object->pos);
            njRotateEx(volume->object->ang, 0);
            njScale(NULL, volume->object->scl[0], 1.0F, volume->object->scl[2]);
            njInvertMatrix(NULL);
            if (work->collision) {
                njCalcPoint(NULL, &player_work[0]->collision->shapes->center,
                            &local);
            } else {
                njCalcPoint(NULL, &player_work[0]->pos, &local);
            }
            if (i != TURNED_ONLY || world_gravity.y > -GRAVITY_ON_AXIS) {
                if (local.x < VOLUME_HALF_SIZE && local.x > -VOLUME_HALF_SIZE &&
                    local.y > -VOLUME_DEPTH && local.y < 0.0F &&
                    local.z > -VOLUME_HALF_SIZE && local.z < VOLUME_HALF_SIZE) {
                    player_fall_out(0);
                    break;
                }
            }
        }
        njPopMatrixEx();
    }
    if (world_gravity.x > GRAVITY_ON_AXIS) {
        down = SURFACE_POS_X;
    }
    /* @hack redundant: a test of x's component that sets down to the
     * SURFACE_NEG_X it holds already */
    if (world_gravity.x < -GRAVITY_ON_AXIS) {
        down = SURFACE_NEG_X;
    }
    if (world_gravity.y > GRAVITY_ON_AXIS) {
        down = SURFACE_POS_Y;
    }
    if (world_gravity.y < -GRAVITY_ON_AXIS) {
        down = SURFACE_NEG_Y;
    }
    if (world_gravity.z > GRAVITY_ON_AXIS) {
        down = SURFACE_POS_Z;
    }
    if (world_gravity.z < -GRAVITY_ON_AXIS) {
        down = SURFACE_NEG_Z;
    }
    work->submode = 0;
    if (GADGET_GLOW(task) && player_work[0]) {
        NJS_POINT3 center;
        found = FALSE;
        center = player_work[0]->collision->shapes->center;
        switch (down) {
        case SURFACE_POS_Y:
        case SURFACE_NEG_Y:
#define GLOW_AXIS_Y
#include "stg22/project_glow.h"
#undef GLOW_AXIS_Y
            break;
        case SURFACE_POS_X:
        case SURFACE_NEG_X:
#define GLOW_AXIS_X
#include "stg22/project_glow.h"
#undef GLOW_AXIS_X
            break;
        case SURFACE_POS_Z:
        case SURFACE_NEG_Z:
#define GLOW_AXIS_Z
#include "stg22/project_glow.h"
#undef GLOW_AXIS_Z
            break;
        }
        /* @hack redundant: a test of down with an empty body, in a block no
         * path reaches */
        if (0) {
            if (down) {
            }
        }
        if (found) {
            Float distance =
                njDistanceP2P(&player_work[0]->collision->shapes->center,
                              &GADGET_GLOW(task)->pos);
            /* @bug logic: the glow's falloff never applies. the step has
             * just set the glow's pos to player 0's center, at 0x8c500e98,
             * 0x8c5012c6 or 0x8c501706 by gravity's axis. the distance is 0,
             * so the test at 0x8c50172c never holds */
            if (distance > GLOW_NEAR) {
                target = glow_falloff / (glow_falloff + distance - GLOW_NEAR);
            } else {
                target = 1.0F;
            }
            target = target *
                     (GLOW_FLICKER_LOW +
                      (GLOW_FLICKER_HALF *
                       (njSin(game_state.steps * GLOW_FLICKER_SPEED) + 1.0F)));
            work->scl.y = target;
            work->scl.x =
                work->scl.x + ((work->scl.y - work->scl.x) * GLOW_EASE);
        } else {
            work->scl.x *= GLOW_FADE;
        }
    }
}

void gadget_background_destroy(struct task *task)
{
    if (gadget_background_task == task) {
        gadget_background_task = NULL;
    }
    if (GADGET_GLOW(task)) {
        syFree(GADGET_GLOW(task));
        task->unk_3c.l = 0;
    }
}

/* background.c's code starts at the next 32-byte boundary */
/* clang-format off */
#ifdef DREAMCAST
#ifdef NONMATCHING
#else
/* @asm open: no inspect rule reads the NOPs its .ALIGN pads with */
#pragma asm
        .ALIGN      32
#pragma endasm
#endif
#endif
/* clang-format on */
