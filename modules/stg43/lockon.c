/* the target, LOCKON and LOCKON_FAR in Cosmic Wall's object list. the two
 * kinds differ only in range: LOCKON_FAR's is 800, and LOCKON takes the set
 * manager's 400 */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "stg43/lockon/text.inc"

#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "sound/sound.h"
#include "stg43/objects.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a kind of piece a broken target throws: its textures, its model, and how
 * many of it */
struct piece_kind {
    NJS_TEXLIST *textures;
    NJS_CNK_MODEL *model;
    int count;
};
#define PIECE_KINDS 2

/* a scl.y of TWO_POINTS or more gives the target two weak points. they lie
 * TARGET_OFFSET either side of pos along x. the one toward +x has the index
 * RIGHT_POINT, and the other LEFT_POINT, as a lone point does */
#define TWO_POINTS 1.0F
#define LEFT_POINT 0
#define RIGHT_POINT 1
#define TARGET_OFFSET 10.0F
/* the bob's height above pos, kept in the work's scl.z. the low
 * BOB_RATE_BITS of ang.z are the bob's turn each step. scl.x is the height it
 * reaches either way */
#define BOB_HEIGHT(work) ((work)->scl.z)
#define BOB_RATE_BITS 0xfff
/* the sound the target plays at its pos, and its priority, volume and time */
#define TARGET_SOUND 18
#define TARGET_SOUND_PRIORITY 1
#define TARGET_SOUND_VOLUME 0
#define TARGET_SOUND_STEPS 30
/* object_sound_play's kind for the loss of a weak point */
#define HIT_SOUND_KIND 7
/* a piece flies at PIECE_SPEED plus up to PIECE_SPEED_RANGE more. its start
 * point is scaled by PIECE_SCALE along each axis */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F
#define PIECE_SCALE 5.0F
/* the points a broken target scores */
#define BREAK_POINTS 20

static NJS_TEXLIST lockon_single_texlist;
static NJS_TEXLIST lockon_double_texlist;
static NJS_CNK_OBJECT lockon_single_object;
static NJS_CNK_OBJECT lockon_double_object;
static NJS_TEXLIST lockon_single_piece_a_texlist;
static NJS_TEXLIST lockon_single_piece_b_texlist;
static NJS_TEXLIST lockon_double_piece_a_texlist;
static NJS_TEXLIST lockon_double_piece_b_texlist;
static NJS_CNK_MODEL lockon_single_piece_a;
static NJS_CNK_MODEL lockon_single_piece_b;
static NJS_CNK_MODEL lockon_double_piece_a;
static NJS_CNK_MODEL lockon_double_piece_b;

static struct piece_kind single_pieces[PIECE_KINDS];
static struct piece_kind double_pieces[PIECE_KINDS];
static struct collision_shape target_shape[1];
static Angle rotor_speed;

static void lockon_destroy(struct task *task);
static void lockon_step(struct task *task);
static void lockon_draw(struct task *task);

void cosmic_lockon_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;

    if (!placement_try_leave(task)) {
        task->draw = lockon_draw;
        task->step = lockon_step;
        task->destructor = lockon_destroy;
        if (work->scl.y >= TWO_POINTS) {
            position = work->pos;
            position.x += TARGET_OFFSET;
            weak_point_create_shaped(task, RIGHT_POINT, &position, target_shape,
                                     1);
            position.x -= TARGET_OFFSET * 2.0F;
            weak_point_create_shaped(task, LEFT_POINT, &position, target_shape,
                                     1);
        } else {
            weak_point_create_shaped(task, LEFT_POINT, &work->pos, target_shape,
                                     1);
        }
    }
}

static void lockon_destroy(struct task *task)
{
}

static void lockon_step(struct task *task)
{
    struct task_work *work = task->work;
    int kind;
    int piece;
    struct task *target;
    Float height;
    Angle phase;

    /* @hack spelling: the sound's call joined to the else if's test by a
     * comma, where a call of its own after the return would do */
    if (placement_try_leave(task)) {
        return;
    } else if (sound_play_at_for(TARGET_SOUND, task, TARGET_SOUND_PRIORITY,
                                 TARGET_SOUND_VOLUME, TARGET_SOUND_STEPS,
                                 &work->pos),
               !task->child) {
        NJS_POINT3 center;
        NJS_VECTOR scatter;

        center = work->pos;
        scatter.x = PIECE_SCALE;
        scatter.y = PIECE_SCALE;
        scatter.z = PIECE_SCALE;
        /* a two-point target throws as many of each kind as single_pieces
         * gives */
        for (kind = 0; kind < PIECE_KINDS; kind++) {
            for (piece = 0; piece < single_pieces[kind].count; piece++) {
                debris_throw_flags(
                    &center, DEBRIS_THROW_ANY, &scatter, work->ang.y,
                    PIECE_SPEED +
                        ((Float)rand() * (PIECE_SPEED_RANGE * RAND_SCALE)),
                    NO_FLOOR,
                    work->scl.y >= TWO_POINTS ? double_pieces[kind].model
                                              : single_pieces[kind].model,
                    work->scl.y >= TWO_POINTS ? double_pieces[kind].textures
                                              : single_pieces[kind].textures,
                    DEBRIS_FADE_COLORS);
            }
        }
        score_add(BREAK_POINTS);
        if (task->placement != NULL) {
            placement_kill(task);
        } else {
            task_kill(task);
        }
    } else {
        /* the submode holds a bit for each weak point lost. the tick holds
         * the bits at the step before */
        if (work->submode != work->tick) {
            object_sound_play(HIT_SOUND_KIND, &work->pos);
        }
        work->tick = work->submode;
        phase = (Angle)game_state.steps * (work->ang.z & BOB_RATE_BITS);
        height = njSin(phase) * work->scl.x;
        if (task->child != NULL) {
            if (work->scl.y >= TWO_POINTS) {
                target = task->child;
                do {
                    if (target->work->tick == RIGHT_POINT) {
                        NJS_POINT3 right;

                        right = work->pos;
                        right.x += TARGET_OFFSET;
                        right.y += height;
                        target->work->pos = right;
                    } else {
                        NJS_POINT3 left;

                        left = work->pos;
                        left.x -= TARGET_OFFSET;
                        left.y += height;
                        target->work->pos = left;
                    }
                    target = target->next;
                } while (target != task->child);
            } else {
                NJS_POINT3 center;

                center = work->pos;
                center.y += height;
                task->child->work->pos = center;
            }
        }
        BOB_HEIGHT(work) = height;
    }
}

static void lockon_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *housing;
    NJS_CNK_OBJECT *rotor;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)LOCKON_EDITOR_ANGLE_Y;
        (void)LOCKON_EDITOR_HUWAHUWA_SPD;
        (void)LOCKON_EDITOR_HUWAHUWA_SCL;
        (void)LOCKON_EDITOR_MODEL;
    }
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + BOB_HEIGHT(work), work->pos.z);
    njRotateY(NULL, work->ang.y);
    if (work->scl.y < TWO_POINTS) {
        housing = &lockon_single_object;
        njSetTexture(&lockon_single_texlist);
    } else {
        housing = &lockon_double_object;
        njSetTexture(&lockon_double_texlist);
    }
    njCnkEasyDrawModel(housing->model);
    rotor = housing->child;
    njRotateZ(NULL, game_state.steps * rotor_speed);
    njCnkEasyDrawModel(rotor->model);
    njPopMatrixEx();
}

#include "stg43/foreground/lockon.h"

NJS_TEXLIST *cosmic_lockon_texlists[7] = {
#include "stg43/lockon/cosmic_lockon_texlists.inc"
};
static struct piece_kind single_pieces[PIECE_KINDS] = {
#include "stg43/lockon/single_pieces.inc"
};
static struct piece_kind double_pieces[PIECE_KINDS] = {
#include "stg43/lockon/double_pieces.inc"
};
static struct collision_shape target_shape[1] = {
#include "stg43/lockon/target_shape.inc"
};
/* the rotor's turn each step */
static Angle rotor_speed = {
#include "stg43/lockon/rotor_speed.inc"
};
