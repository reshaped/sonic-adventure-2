#include "draw/color.h"
#include "stg22/fixture_models.h"
#include "system/angle.h"
#include <sg_xpt.h>
#include <stdlib.h>
#include <ninja.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/missile.h"
#include "shared/white_breath.h"
#include "sound/sound.h"
#include "stg22/poison.h"
#include "stg22/prop_textures.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the tank's record, at task->motion. hit is where a missile of its id
 * landed, or null. missile_target_add takes its address */
struct tank {
    NJS_POINT3 *hit;
};
#define TANK(task) ((struct tank *)(task)->motion.p)
/* a tank's modes, in its work's mode. a missile's hit or a player's touch
 * spends the tank. a touch also sets its work's tick to 1 */
enum tank_mode { TANK_WHOLE, TANK_SPENT };
/* the bits of ang.z that hold the tank's missile target id */
#define TARGET_ID_BITS 0x1f
/* the tank's poison cycles through POISON_FRAMES textures of
 * gadget_poison_texlist from POISON_FIRST, a frame every POISON_FRAME_STEPS
 * steps. the step writes the frame's texture id into word POISON_TID of the
 * tank's polygon list. the word's NJD_FFM_MASK bits stay */
#define POISON_FRAMES 13
#define POISON_FIRST 4
#define POISON_FRAME_STEPS 2
#define POISON_TID 7
/* a broken or touched tank fades over POISON_FADE_STEPS. its draw's alpha
 * runs from MOST_ALPHA down to 0 */
#define POISON_FADE_STEPS 120
#define MOST_ALPHA 255
#define WHITE_RGB 0xffffff
/* a missile breaks the tank into POISON_PIECES pieces, which fly at
 * PIECE_SPEED and PIECE_SPREAD as debris_break_flags takes them. the break
 * adds BREAK_RISE in y and BREAK_PUSH along the tank's own z to their speed.
 * it plays POISON_BREAK_SOUND at BREAK_VOLUME */
#define POISON_PIECES 5
#define PIECE_SPEED 0.5F
#define PIECE_SPREAD 0.8F
#define BREAK_RISE 0.5F
#define BREAK_PUSH 0.3F
#define POISON_BREAK_SOUND 0x1022
#define BREAK_VOLUME 100
/* a whole tank plays POISON_LOOP_SOUND each step, for LOOP_SOUND_DURATION
 * steps */
#define POISON_LOOP_SOUND 21
#define SOUND_PRIORITY 1
#define LOOP_SOUND_DURATION 20
/* a touched tank's gas rises from player 0 for POISON_GAS_STEPS. each step a
 * puff rises where a draw from 0 to 1 falls below GAS_ODDS. in x and z, a
 * puff moves by a draw less GAS_CENTER, times GAS_SPREAD. in y it moves from
 * GAS_RISE up to GAS_RISE + GAS_RISE_RANGE. its size runs from gas_min_scale
 * up to gas_min_scale + gas_scale_range. it draws GAS_DEPTH_BIAS nearer the
 * camera */
#define POISON_GAS_STEPS 60
#define GAS_ODDS 0.5F
#define GAS_SPREAD 0.7F
#define GAS_CENTER 0.5F
#define GAS_RISE 0.5F
#define GAS_RISE_RANGE 0.35F
#define GAS_DEPTH_BIAS 6.0F

NJS_TEXLIST *gadget_poison_texlists[] = {
    &gadget_poison_texlist,
    &gadget_poison_body_texlist,
    &gadget_poison_glass_texlist,
    NULL,
};

/* clang-format off */
static struct debris_part fragments[POISON_PIECES] = {
    {&gadget_poison_body_texlist, &gadget_poison_body_fragment,
     {0, 29.5F, 0}, {0, 0, 0}},
    {&gadget_poison_glass_texlist, &gadget_poison_glass_fragment,
     {5.500001F, 24.2F, 0}, {0, 0, 0}},
    {&gadget_poison_glass_texlist, &gadget_poison_glass_fragment,
     {5.500001F, 35.200005F, 0}, {0, 0, QUARTER_TURN}},
    {&gadget_poison_glass_texlist, &gadget_poison_glass_fragment,
     {-5.299999F, 35.200005F, 0}, {0, 0, HALF_TURN}},
    {&gadget_poison_glass_texlist, &gadget_poison_glass_fragment,
     {-5.6F, 24.250002F, 0}, {0, 0, -QUARTER_TURN}},
};
static struct collision_shape tank_shape = {
    0, SHAPE_BOX, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS), 0, 0, {0, 30.0F, 0}, 70.0F,
    30.0F, 8.0F, 0, {0, 0, 0},
};
/* clang-format on */
static Float gas_min_scale = 1.0F;
static Float gas_scale_range = 3.0F;
static Angle core_turn = 0x100;

static void tank_destroy(struct task *task);
static void tank_step(struct task *task);
static void tank_draw(struct task *task);

void gadget_poison_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct tank));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = tank_draw;
    task->step = tank_step;
    task->destructor = tank_destroy;
    collision_init(task, &tank_shape, 1, COLLISION_OBJECT);
    missile_target_add(work->ang.z & TARGET_ID_BITS, &TANK(task)->hit);
}

static void tank_destroy(struct task *task)
{
    missile_target_remove(&TANK(task)->hit);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void tank_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR velocity;
    int frame;
    Sint16 *texture;
    int material;

    if (placement_try_leave(task)) {
        return;
    }
    frame =
        (game_state.steps / POISON_FRAME_STEPS % POISON_FRAMES) + POISON_FIRST;
    texture = &gadget_poison_tank->model->plist[POISON_TID];
    material = *texture;
    *texture = (material & NJD_FFM_MASK) | frame;
    if (work->mode == TANK_WHOLE) {
        if (TANK(task)->hit) {
            work->mode = TANK_SPENT;
            velocity.x = 0.0F;
            velocity.y = BREAK_RISE;
            velocity.z = BREAK_PUSH;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, work->ang.y);
            njCalcVector(NULL, &velocity, &velocity);
            njPopMatrixEx();
            debris_break_flags(&work->pos, work->ang.y, fragments,
                               POISON_PIECES, PIECE_SPEED, PIECE_SPREAD,
                               work->pos.y, &velocity, DEBRIS_FADE_COLORS);
            sound_play_at(POISON_BREAK_SOUND, NULL, 0, BREAK_VOLUME,
                          &work->pos);
        }
        sound_play_at_for(POISON_LOOP_SOUND, work, SOUND_PRIORITY, 0,
                          LOOP_SOUND_DURATION, &work->pos);
        if (collision_next_player(task)) {
            player_miss(0);
            work->mode = TANK_SPENT;
            work->tick = 1;
        }
        if (work->collision) {
            collision_register(task);
        }
    } else {
        if (work->tick && player_work[0] && work->timer < POISON_GAS_STEPS &&
            rand() * RAND_SCALE < GAS_ODDS) {
            velocity.x = ((rand() * RAND_SCALE) - GAS_CENTER) * GAS_SPREAD;
            velocity.y = (rand() * (GAS_RISE_RANGE * RAND_SCALE)) + GAS_RISE;
            velocity.z = ((rand() * RAND_SCALE) - GAS_CENTER) * GAS_SPREAD;
            white_breath_tinted_create(&player_work[0]->pos, &velocity,
                                       (rand() * gas_scale_range * RAND_SCALE) +
                                           gas_min_scale,
                                       GAS_DEPTH_BIAS);
        }
        if (work->timer > POISON_FADE_STEPS) {
            if (!work->tick) {
                if (task->placement) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
            }
        } else {
            /* @hack spelling: timer set to itself plus 1, where timer++
             * would do */
            work->timer = work->timer + 1;
        }
        if (work->collision) {
            collision_register(task);
        }
    }
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ROT Y";
        (void)"<- ID";
    }
}

static void tank_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object = gadget_poison_tank;
    Float opacity;
    int alpha;
    /* @hack type: color a Sint32, where model_draw_tinted's Uint32 would do */
    Sint32 color;

    njSetTexture(&gadget_poison_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateEx(object->ang, 0);
    if (work->timer) {
        opacity = 1.0F - (work->timer * (1.0F / POISON_FADE_STEPS));
        alpha = opacity * MOST_ALPHA;
        if (alpha > MOST_ALPHA) {
            alpha = MOST_ALPHA;
        }
        if (alpha < 0) {
            alpha = 0;
        }
        color = ((Uint32)alpha << ALPHA_SHIFT) | WHITE_RGB;
        model_draw_tinted(object->model, color);
    } else {
        njCnkEasyDrawModel(object->model);
        object = object->child;
        njTranslateEx((NJS_VECTOR *)object->pos);
        njRotateEx(object->ang, 0);
        njRotateZ(NULL, game_state.steps * core_turn);
        njCnkEasyDrawModel(object->model);
    }
    njPopMatrixEx();
}
