/* the checkpoint, GATE2 in the cart's object list. the first touch by a
 * racer adds scl.y seconds to the race's countdown. no code calls
 * cart_checkpoint_create */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "cart/course.h"
#include "cart/course_art.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/texture_remap.h"
#include "cart/vehicle.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the lamps show CART_START_TEXTURE in place of CART_LAMP_TEXTURE while
 * FLASH_BIT of game_state.steps is clear */
#define FLASH_BIT 0x4
/* a checkpoint's second voice plays FOLLOW_UP_STEPS + 1 steps after its
 * first. Rouge's voices follow Tails's TAILS_CHECKPOINTS in lines */
#define FOLLOW_UP_STEPS 240
#define TAILS_CHECKPOINTS 3
#define PASS_SOUND 25

/* clang-format off */
static struct collision_shape checkpoint_shape[] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_IGNORE_PLAYERS | SHAPE_IGNORE_PICKUPS, {0, 30, 0}, 100, 50, 5, 0,
     {0, 0, 0}}};
/* clang-format on */
static NJS_MATERIAL checkpoint_material[1] = {
#include "cart/checkpoint/checkpoint_material.inc"
};
/* nine strips, each a count, then that many indices. NJD_TRIMESH_END in a
 * count reverses the winding its strip starts with */
static Sint16 checkpoint_strips[75] = {
#include "cart/checkpoint/checkpoint_strips.inc"
};
static NJS_MESHSET checkpoint_mesh[1] = {
#include "cart/checkpoint/checkpoint_mesh.inc"
};
static NJS_POINT3 checkpoint_points[48] = {
#include "cart/checkpoint/checkpoint_points.inc"
};
static NJS_VECTOR checkpoint_normals[48] = {
#include "cart/checkpoint/checkpoint_normals.inc"
};
static NJS_MODEL checkpoint_arch = {
#include "cart/checkpoint/checkpoint_arch.inc"
};
/* the checkpoint's solid: an arch of a crossbar on two posts. each
 * checkpoint copies checkpoint_land and places the copy at its own position
 * and yaw */
static NJS_OBJECT checkpoint_land = {
#include "cart/checkpoint/checkpoint_land.inc"
};
/* the voices each story race's checkpoints play, in the order they are
 * passed. the switch in announce_checkpoint names those with no second
 * voice, and their -1 never plays */
struct checkpoint_line {
    Sint32 first, second;
};
/* clang-format off */
static struct checkpoint_line lines[] = {
    {0x702, -1}, {0x703, 0x704}, {0x705, -1},
    {0x709, -1}, {0x70a, 0x70b}, {0x70c, -1},
};
/* clang-format on */
static Sint32 next_checkpoint;
static void checkpoint_draw(struct task *task);
static void checkpoint_destroy(struct task *task);
static void announce_checkpoint(void);
static void follow_up_step(struct task *task);

struct task *cart_checkpoint_create(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task =
        task_create(TASK_WORK, 2, cart_checkpoint_step, "CartGate2Exec");
    task->draw = checkpoint_draw;
    task->destructor = checkpoint_destroy;
    task->work->pos = *pos;
    task->work->ang = *ang;
    return task;
}
void cart_reset_checkpoints(void)
{
    next_checkpoint = 0;
}
void cart_checkpoint_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *land;
    if (cart_out_of_range(&work->pos)) {
        return;
    }
    if (work->mode == 0) {
        work->mode++;
        work->tick = 0;
        work->timer = 0;
        if (work->scl.x < 1.0F) {
            work->tick = 0;
        } else {
            work->tick = 1;
        }
        task->draw = checkpoint_draw;
        task->destructor = checkpoint_destroy;
        collision_init(task, checkpoint_shape, 1, COLLISION_OBJECT_MUTUAL);
        land = syCalloc(1, sizeof(*land));
#ifdef NONMATCHING
        /* this side kills the task where the heap has no land. the matching
         * side copies into the land through the null */
        if (!land) {
            task_kill(task);
            return;
        }
#endif
        task->motion.p = land;
        *land = checkpoint_land;
        land->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        land->ang[0] = 0;
        land->ang[1] = work->ang.y;
        land->ang[2] = 0;
        land->pos[0] = work->pos.x;
        land->pos[1] = work->pos.y;
        land->pos[2] = work->pos.z;
        world_add(WORLD_SIZE_CLASS(1) | WORLD_SOLID, task, land);
    } else if (work->timer == 0) {
        if (work->collision->flags & COLLISION_TOUCHED) {
            if (cart_is_racer(work->collision->hits[0].task)) {
                work->timer++;
                sound_play(PASS_SOUND, NULL, 0, 0);
                cart_race.countdown += (int)work->scl.y * CART_STEPS_PER_SECOND;
                ((struct cart_vehicle *)cart_racers[0]->motion.p)->lap++;
                cart_show_time_extension();
                announce_checkpoint();
            }
        }
        collision_register(task);
        /* @hack redundant: an empty else after this arm */
    } else {
    }
    if ((game_state.steps & FLASH_BIT) != 0) {
        work->tick = 1;
    } else {
        work->tick = 0;
    }
}
static void checkpoint_draw(struct task *task)
{
    struct task_work *work = task->work;
    if (cart_out_of_view(&work->pos)) {
        return;
    }
    switch (work->tick) {
    case 0:
        cart_replace_texture(cart_texture_remaps, cart_checkpoint_model,
                             CART_LAMP_TEXTURE, CART_START_TEXTURE);
        break;
    case 1:
        cart_replace_texture(cart_texture_remaps, cart_checkpoint_model,
                             CART_LAMP_TEXTURE, CART_LAMP_TEXTURE);
        break;
    }
    njPushMatrixEx();
    njTranslateV(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njSetTexture(&cart_object_textures);
    njCnkEasyDrawObject(cart_checkpoint_model);
    njPopMatrixEx();
}
static void checkpoint_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
    }
}
static void announce_checkpoint(void)
{
    int index;
    struct task *task;
    switch (cart_setup.course_mode) {
    case CART_TAILS:
        index = next_checkpoint;
        break;
    case CART_ROUGE:
        index = next_checkpoint + TAILS_CHECKPOINTS;
        break;
        /* @bug uninitialized: outside the story races, reads index at
         * 0x8c507f06 unset, and plays lines[index] */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: outside the story races, it
     * announces nothing */
    default:
        return;
#endif
    }
    next_checkpoint++;
#ifdef NONMATCHING
    /* this side plays no line for an index past lines */
    if ((unsigned)index >= sizeof(lines) / sizeof(lines[0])) {
        return;
    }
#endif
    voice_play_priority_2(lines[index].first);
    /* @hack spelling: a break for each checkpoint with no second voice, where
     * one break after the four labels would do */
    switch (index) {
    case 0:
        break;
    case 2:
        break;
    case 3:
        break;
    case 5:
        break;
    default:
        task = task_create(TASK_WORK, 2, follow_up_step, "cartGateSerifu");
        task->draw = NULL;
        task->destructor = NULL;
        task->work->tick = index;
        task->work->timer = FOLLOW_UP_STEPS;
        break;
    }
}
static void follow_up_step(struct task *task)
{
    if (!task->work->timer--) {
        voice_play_priority_2(lines[task->work->tick].second);
        task_destroy(task);
    }
}
