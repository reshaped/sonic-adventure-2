/* FALLASHIBA, Green Hill's floor that crumbles once player 0 has stood on it
 * more than STEPS_TO_TRIGGER steps running. its pieces fall from the rim
 * inward. a piece falls (FALL_RADIUS - d) * STEPS_PER_UNIT steps into the
 * crumble. d is the piece's distance from the center, with z counted
 * DEPTH_WEIGHT times.
 *
 * each step measures d for the place under player 0 too. the floor keeps
 * WORLD_ADDED, and so holds players up, only while that place hasn't fallen.
 * it also needs a player within its model's radius plus REACH_MARGIN of its
 * pos. a place past FALL_RADIUS counts as fallen before the crumble starts */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "system/cache.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg31/green_hill_sounds.h"
#include "stg31/prop_art.h"
#include "stg31/platform_collision.h"
#include "stg31/platforms.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define STEPS_TO_TRIGGER 10
/* the floor ends FLOOR_LIFETIME steps into the crumble */
#define FLOOR_LIFETIME 1200
#define DEPTH_WEIGHT 2.0F
#define FALL_RADIUS 100.0F
#define STEPS_PER_UNIT 1.5F
/* a piece lies PIECE_LIFT * t - PIECE_HALF_GRAVITY * t * t above its place,
 * t steps into its fall */
#define PIECE_LIFT 0.5F
#define PIECE_HALF_GRAVITY 0.15F

enum floor_mode { FLOOR_WAIT, FLOOR_CRUMBLING };

NJS_TEXLIST *green_hill_floor_texlists[] = {&green_hill_crumbling_floor_texlist,
                                            &green_hill_floor_pieces_texlist,
                                            NULL};

static void floor_destroy(struct task *task);
static void floor_step(struct task *task);
static void floor_draw(struct task *task);

void green_hill_crumbling_floor_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;

    /* @hack redundant: a statement kept for its string, the SET editor's
     * label, which SHC places in this source's constants */
    (void)"<- ANGLE Y";

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(NJS_OBJECT));
    if (!task->motion.p) {
        return;
    }
    task->draw = floor_draw;
    task->step = floor_step;
    task->destructor = floor_destroy;
    work->mode = FLOOR_WAIT;
    *PLATFORM_OBJECT(task) = green_hill_floor_collision;
    object = task->motion.p;
    PLACE_PLATFORM(object, work);
    flags = WORLD_SOLID;
    radius = object->model->r + SIZE_MARGIN;
    ADD_SIZE_CLASS(flags, radius);
    world_add(flags, task, object);
}

static void floor_destroy(struct task *task)
{
    platform_release(task);
}

#ifdef NONMATCHING
#else
/* @hack spelling: x, y and z reached through Float pointers stepped across
 * them, where a subtraction of each by name would do */
#pragma inline(difference)
static void difference(Float *from, Float *to, Float *out)
{
    *out++ = *to++ - *from++;
    *out++ = *to++ - *from++;
    *out = *to - *from;
}
#endif

static void floor_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 previous;
    NJS_VECTOR offset;
    Float delay;
    Float elapsed;

    previous = work->pos;
    if (work->mode == FLOOR_CRUMBLING) {
        ++work->timer;
        if (work->timer > FLOOR_LIFETIME) {
            if (task->placement) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            return;
        }
    } else if (player_on_task(0, task)) {
        ++work->tick;
        if (work->tick > STEPS_TO_TRIGGER) {
            work->mode = FLOOR_CRUMBLING;
            sound_play(GREEN_HILL_CRUMBLE, NULL, 0, 0);
        }
    } else {
        work->tick = 0;
    }
    if (!PLATFORM_OBJECT(task)->model) {
        return;
    }
#ifdef NONMATCHING
    /* the plain form of the hack tagged at difference above */
    offset.x = player_work[0]->pos.x - work->pos.x;
    offset.y = player_work[0]->pos.y - work->pos.y;
    offset.z = player_work[0]->pos.z - work->pos.z;
#else
    difference(&work->pos.x, &player_work[0]->pos.x, &offset.x);
#endif
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, -work->ang.y);
    njCalcPoint(NULL, &offset, &offset);
    njPopMatrixEx();
    offset.z *= DEPTH_WEIGHT;
    offset.y = 0.0F;
    delay = (FALL_RADIUS - njScalor(&offset)) * STEPS_PER_UNIT;
    elapsed = work->timer - delay;
    FOLLOW_WORK(task, work, previous);
    if (elapsed <= 0.0F &&
        player_near(&work->pos,
                    PLATFORM_OBJECT(task)->model->r + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void floor_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *root;
    NJS_CNK_OBJECT *piece;
    NJS_VECTOR offset;
    Float delay;
    Float elapsed;
    Float height;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    if (work->timer) {
        njSetTexture(&green_hill_floor_pieces_texlist);
        root = &green_hill_floor_pieces;
        for (piece = root->child; piece; piece = piece->sibling) {
            offset = *(NJS_VECTOR *)piece->pos;
            offset.z *= DEPTH_WEIGHT;
            delay = (FALL_RADIUS - njScalor(&offset)) * STEPS_PER_UNIT;
            elapsed = work->timer - delay;
            if (elapsed > 0.0F) {
                height = (elapsed * PIECE_LIFT) -
                         (elapsed * elapsed * PIECE_HALF_GRAVITY);
                njTranslate(NULL, piece->pos[0], piece->pos[1] + height,
                            piece->pos[2]);
            } else {
                njTranslateEx((NJS_VECTOR *)piece->pos);
            }
            njRotateEx(piece->ang, XYZ_TURN);
            njCnkEasyDrawModel(piece->model);
            matrix_load_parent();
        }
    } else {
        njSetTexture(&green_hill_crumbling_floor_texlist);
        njCnkEasyDrawModel(green_hill_floor_object->model);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrix(2);
}
