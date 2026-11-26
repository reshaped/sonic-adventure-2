/* the gun, VULCAN in Cosmic Wall's object list: a solid housing that recoils
 * at each attack that counts. it breaks at the fourth */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg43/vulcan/text.inc"

#include "collision/collision.h"
#include "collision/world.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "stg43/objects.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the record the gun keeps in task->motion, which solid_impl.h's init
 * allocates. collision is the copy in the world, which stays at the work's pos
 * while the housing recoils. offset is the recoil, and speed what each step
 * adds to it. flash is 1 at a counted attack, and fades from there */
struct vulcan_motion {
    NJS_OBJECT collision;
    Float flash;
    NJS_VECTOR speed;
    NJS_VECTOR offset;
};
#define VULCAN(task) ((struct vulcan_motion *)(task)->motion.p)

/* the gun's copy in the world collides only while a player lies within
 * COLLIDE_RANGE of its pos */
#define COLLIDE_RANGE 80.0F
/* the work's timer counts the attacks that count, and the gun breaks at the
 * HIT_LIMIT-th. a counted attack sets the work's tick to HIT_GAP. each step
 * takes 1 from a tick above 0. an attack counts only at tick 0, HIT_GAP or
 * more steps after the last that counted */
#define HIT_LIMIT 4
#define HIT_GAP 2
/* a counted attack adds KICK to speed along y. each step adds speed to
 * offset. it then sets speed to speed * DRAG - offset * SPRING, axis by axis.
 * nothing kicks x or z, so offset's x and z stay 0. the housing draws
 * |offset.y| below pos */
#define KICK 2.0F
#define DRAG_X 0.87F
#define DRAG_Y 0.85F
#define DRAG_Z 0.87F
#define SPRING_X 0.53F
#define SPRING_Y 0.52F
#define SPRING_Z 0.58F
/* each step keeps FLASH_FADE of flash. while flash is above 0, the housing
 * draws tinted from COLOR_NONE toward FLASH_RED by flash */
#define FLASH_FADE 0.975F
#define FLASH_RED 0xffff5050
/* the angle the light's pulse advances a step. a pulse takes 0x10000 /
 * PULSE_RATE steps, about 102 */
#define PULSE_RATE 0x280
/* the gun breaks into the PIECES of vulcan_pieces. each flies away from pos
 * at PIECE_SPEED plus up to PIECE_SPREAD, and gains PIECE_LIFT up. they fall
 * to a floor PIECE_DROP below pos */
#define PIECES 9
#define PIECE_SPEED 0.8F
#define PIECE_SPREAD 1.0F
#define PIECE_LIFT 1.5F
#define PIECE_DROP 0.1F
/* object_sound_play's kinds for a counted attack and for the break */
#define HIT_SOUND_KIND 2
#define BREAK_SOUND_KIND 3
/* the points a broken gun scores */
#define BREAK_POINTS 20

static NJS_TEXLIST vulcan_texlist;
static NJS_CNK_OBJECT vulcan_object;
static NJS_OBJECT vulcan_collision;
static NJS_TEXLIST vulcan_cap_texlist;
static NJS_TEXLIST vulcan_corner_texlist;
static NJS_TEXLIST vulcan_side_texlist;
static NJS_CNK_MODEL vulcan_cap;
static NJS_CNK_MODEL vulcan_corner;
static NJS_CNK_MODEL vulcan_side;

static struct debris_part vulcan_pieces[PIECES];
static struct collision_shape hit_shape;
static Uint32 bright_color;
static Uint32 dim_color;

static void vulcan_destroy(struct task *task);
static void vulcan_step(struct task *task);
static void vulcan_draw(struct task *task);

#define SOLID_INIT cosmic_vulcan_init
#define SOLID_DRAW vulcan_draw
#define SOLID_STEP vulcan_step
#define SOLID_DESTROY vulcan_destroy
#define SOLID_COLLISION vulcan_collision
#define SOLID_MOTION struct vulcan_motion
#define SOLID_SHAPE hit_shape
#include "stg43/solid_impl.h" /* IWYU pragma: keep -- the code it compiles here */

static void vulcan_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: pos, a copy of the work's pos for the break, where
     * &work->pos would do */
    NJS_POINT3 pos;
    NJS_VECTOR lift;

    if (placement_try_leave(task)) {
        return;
    }
    if (player_near(&work->pos, COLLIDE_RANGE)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
    if (collision_next_attack(task) && !work->tick) {
        VULCAN(task)->speed.y += KICK;
        VULCAN(task)->flash = 1.0F;
        work->timer++;
        work->tick = HIT_GAP;
        object_sound_play(HIT_SOUND_KIND, &work->pos);
    }
    if (work->tick) {
        work->tick--;
    }
    VULCAN(task)->offset.x += VULCAN(task)->speed.x;
    VULCAN(task)->offset.y += VULCAN(task)->speed.y;
    VULCAN(task)->offset.z += VULCAN(task)->speed.z;
    VULCAN(task)->speed.x =
        (VULCAN(task)->speed.x * DRAG_X) - (VULCAN(task)->offset.x * SPRING_X);
    VULCAN(task)->speed.y =
        (VULCAN(task)->speed.y * DRAG_Y) - (VULCAN(task)->offset.y * SPRING_Y);
    VULCAN(task)->speed.z =
        (VULCAN(task)->speed.z * DRAG_Z) - (VULCAN(task)->offset.z * SPRING_Z);
    VULCAN(task)->flash *= FLASH_FADE;
    if (work->timer >= HIT_LIMIT) {
        pos = work->pos;
        lift.x = 0.0F;
        lift.y = PIECE_LIFT;
        lift.z = 0.0F;
        debris_break_flags(&pos, work->ang.y, vulcan_pieces, PIECES,
                           PIECE_SPEED, PIECE_SPREAD, work->pos.y - PIECE_DROP,
                           &lift, DEBRIS_FADE_COLORS);
        object_sound_play(BREAK_SOUND_KIND, &work->pos);
        score_add(BREAK_POINTS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        collision_register(task);
    }
}

static void vulcan_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *housing;
    NJS_CNK_OBJECT *light;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * label, which SHC still places in this source's constants */
    if (0) {
        (void)VULCAN_EDITOR_ANGLE_Y;
    }
    njSetTexture(&vulcan_texlist);
    housing = &vulcan_object;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, VULCAN(task)->offset.x, -njAbs(VULCAN(task)->offset.y),
                VULCAN(task)->offset.z);
    if (VULCAN(task)->flash > 0.0F) {
        model_draw_tinted(housing->model, color_blend(COLOR_NONE, FLASH_RED,
                                                      VULCAN(task)->flash));
    } else {
        njCnkEasyDrawModel(housing->model);
    }
    light = housing->child;
    model_draw_tinted(
        light->model,
        color_blend(bright_color, dim_color,
                    (njSin(game_state.steps * PULSE_RATE) + 1.0F) * 0.5F));
    njPopMatrixEx();
}

#include "stg43/foreground/vulcan.h"

NJS_TEXLIST *cosmic_vulcan_texlists[5] = {
#include "stg43/vulcan/cosmic_vulcan_texlists.inc"
};
static struct debris_part vulcan_pieces[PIECES] = {
#include "stg43/vulcan/vulcan_pieces.inc"
};
static struct collision_shape hit_shape = {
#include "stg43/vulcan/hit_shape.inc"
};
static Uint32 bright_color = {
#include "stg43/vulcan/bright_color.inc"
};
static Uint32 dim_color = {
#include "stg43/vulcan/dim_color.inc"
};
