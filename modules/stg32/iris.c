/* the iris, BIGSHUTTER in Meteor Herd's object list: a gate of panels that
 * slide open along its arms. each arm holds an outer panel and an inner one.
 * while its switch is on, the inner panels slide out INNER_TRAVEL, and then the
 * outer ones OUTER_TRAVEL. the iris then ends. a switch turned off shuts it at
 * once. while a player is near, each panel has a child task. the task holds
 * the panel's node in the collision world */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "stg32/meteor_gate.h"
#include "stg32/scenery_shapes.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

enum iris_mode { IRIS_CLOSED, IRIS_OPENING, IRIS_OPEN };
/* the switch that opens the iris, in the low byte of ang.x */
#define IRIS_SWITCH(work) ((Uint8)(work)->ang.x)
#define INNER_TRAVEL 65.0F
#define OUTER_TRAVEL 65.5F
/* as the iris starts to open, it vibrates the pad of each player within
 * PLAYER_RANGE */
#define VIBRATION_POWER 5
#define VIBRATION_FREQ 20
#define VIBRATION_LENGTH 7
/* the panels' tasks live while a player lies within PLAYER_RANGE of pos.
 * player 0 must also lie within PLAYER_HEIGHT of pos.y */
#define PLAYER_RANGE 200.0F
#define PLAYER_HEIGHT 50.0F
#define OPEN_SOUND 9
#define OPEN_SOUND_PRIORITY 0
#define OPEN_SOUND_VOLUME 100
#define PANEL_FLAGS (WORLD_MOVING | WORLD_SOLID)
/* a panel's node takes the size class that its model's radius plus
 * CLASS_MARGIN fits */
#define CLASS_MARGIN 15.0F
/* a panel's step clears WORLD_ADDED while no player lies within its model's
 * radius plus NEAR_MARGIN. world_collect_near_players then passes over the
 * panel's node */
#define NEAR_MARGIN 130.0F

/* in a panel's submode: whether it is its arm's outer panel, not the inner */
#define OUTER(work) ((work)->submode)

/* the record at task->motion of the iris and of each panel. the iris keeps
 * how far its panels have slid, and a panel its node in the collision world */
struct iris_motion {
    /* @hack type: placed, a record laid over the node's first words, so that
     * panel_step copies the node's pos as one NJS_POINT3, where three stores
     * from pos[0], pos[1] and pos[2] would do */
    union {
        NJS_OBJECT object;
        struct {
            Uint32 evalflags;
            NJS_MODEL *model;
            NJS_POINT3 pos;
        } placed;
    } node;
    Float inner_extension;
    Float outer_extension;
};
#define IRIS(task) ((struct iris_motion *)(task)->motion.p)
/* the node of a panel's arm, which iris_step keeps in the panel task's unk_3c.
 * the arms are the children of herd_iris_collision. an arm's node places its
 * outer panel, and the arm's child node its inner one */
#define ARM(task) ((NJS_OBJECT *)(task)->unk_3c.p)

NJS_TEXLIST *herd_iris_textures[] = {&herd_iris_texlist, NULL};
static Float inner_speed = 0.5F;
static Float outer_speed = 1.0F;
static void iris_destroy(struct task *task);
static void iris_step(struct task *task);
static void iris_draw(struct task *task);
static void panel_init(struct task *task);

void herd_iris_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct iris_motion));
    if (!task->motion.p) {
        return;
    }
    task->draw = iris_draw;
    task->step = iris_step;
    task->destructor = iris_destroy;
    work->mode = IRIS_CLOSED;
    IRIS(task)->inner_extension = 0.0F;
    IRIS(task)->outer_extension = 0.0F;
}

static void iris_destroy(struct task *task)
{
    task_destroy_children(task);
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void iris_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    struct task *panel;
    NJS_OBJECT *object;

    if (placement_try_leave(task)) {
        return;
    }
    if (switch_is_on(IRIS_SWITCH(work))) {
        IRIS(task)->inner_extension += inner_speed;
        if (IRIS(task)->inner_extension >= INNER_TRAVEL) {
            IRIS(task)->inner_extension = INNER_TRAVEL;
            IRIS(task)->outer_extension += outer_speed;
            if (IRIS(task)->outer_extension >= OUTER_TRAVEL) {
                IRIS(task)->outer_extension = OUTER_TRAVEL;
                work->mode = IRIS_OPEN;
                if (task->placement) {
                    placement_kill(task);
                } else {
                    task_kill(task);
                }
                return;
            }
        }
        if (!work->mode) {
            sound_play_at(OPEN_SOUND, NULL, OPEN_SOUND_PRIORITY,
                          OPEN_SOUND_VOLUME, &work->pos);
            work->mode = IRIS_OPENING;
            /* @hack spelling: each assignment to player inside its test,
             * where a statement of its own would do */
            if ((player = player_work[0]) &&
                njDistanceP2P(&work->pos, &player->pos) < PLAYER_RANGE) {
                vibration_start_fading(0, VIBRATION_POWER, VIBRATION_FREQ,
                                       VIBRATION_LENGTH);
            }
            if ((player = player_work[1]) &&
                njDistanceP2P(&work->pos, &player->pos) < PLAYER_RANGE) {
                vibration_start_fading(1, VIBRATION_POWER, VIBRATION_FREQ,
                                       VIBRATION_LENGTH);
            }
        }
    } else {
        work->mode = IRIS_CLOSED;
        IRIS(task)->inner_extension = 0.0F;
        IRIS(task)->outer_extension = 0.0F;
    }
    /* @bug logic: tests player 0's height again for player 1. the test reads
     * player_work[0] (MOV.L at 0x8c504374) once player_work[1] isn't null
     * (TST at 0x8c504370) */
    if (work->mode != IRIS_OPEN &&
        (njAbs(player_work[0]->pos.y - work->pos.y) < PLAYER_HEIGHT ||
         (player_work[1] &&
          njAbs(player_work[0]->pos.y - work->pos.y) < PLAYER_HEIGHT)) &&
        player_near(&work->pos, PLAYER_RANGE)) {
        if (!task->child) {
            for (object = herd_iris_collision->child; object;
                 object = object->sibling) {
                panel =
                    task_create_child(TASK_WORK | TASK_FORCE, panel_init, task);
                if (panel) {
                    OUTER(panel->work) = TRUE;
                    panel->unk_3c.p = object;
                }
                panel =
                    task_create_child(TASK_WORK | TASK_FORCE, panel_init, task);
                if (panel) {
                    OUTER(panel->work) = FALSE;
                    panel->unk_3c.p = object;
                }
            }
        }
    } else if (task->child) {
        task_destroy_children(task);
    }
}

/* @hack spelling: the scale and the add as inline walks of three Floats,
 * where three statements on the fields would do */
#pragma inline(scale_vector, add_position)
static void scale_vector(Float *coord, Float scale)
{
    *coord++ *= scale;
    *coord++ *= scale;
    *coord *= scale;
}
static void add_position(Float *position, Float *coord)
{
    *coord++ += *position++;
    *coord++ += *position++;
    *coord += *position;
}

static void iris_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *outer;
    NJS_CNK_OBJECT *inner;
    NJS_VECTOR offset;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"<- ID";
        (void)"<- ANGLE Y";
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetTexture(&herd_iris_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    outer = herd_iris_object->child;
    while (outer) {
        inner = outer->child;
        offset.x = outer->pos[0];
        offset.y = 0.0F;
        offset.z = outer->pos[2];
        scale_vector(&offset.x,
                     IRIS(task)->outer_extension / njScalor(&offset));
        add_position(outer->pos, &offset.x);
        njTranslateEx(&offset);
        njCnkEasyDrawModel(outer->model);
        offset.x = inner->pos[0];
        offset.y = 0.0F;
        offset.z = inner->pos[2];
        scale_vector(&offset.x,
                     -IRIS(task)->inner_extension / njScalor(&offset));
        add_position(inner->pos, &offset.x);
        njTranslateEx(&offset);
        njCnkEasyDrawModel(inner->model);
        matrix_load_parent();
        outer = outer->sibling;
    }
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void panel_place(struct task *task)
{
    struct task_work *work = task->work;
    struct task *parent = task->parent;
    NJS_VECTOR outer;
    NJS_VECTOR inner;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&parent->work->pos);
    njRotateY(NULL, parent->work->ang.y);
    outer.x = ARM(task)->pos[0];
    outer.y = 0.0F;
    outer.z = ARM(task)->pos[2];
    inner.x = ARM(task)->child->pos[0];
    inner.y = 0.0F;
    inner.z = ARM(task)->child->pos[2];
    scale_vector(&outer.x, IRIS(parent)->outer_extension / njScalor(&outer));
    add_position(ARM(task)->pos, &outer.x);
    if (!OUTER(work)) {
        /* @bug logic: slides an inner panel's node toward the iris's center,
         * where iris_draw slides the drawn panel outward. herd_iris_object and
         * herd_iris_collision both place each inner panel inward of its outer
         * one. iris_draw scales by -inner_extension (FNEG at 0x8c504532), and
         * this scale by inner_extension (FMOV.S at 0x8c504670) */
        scale_vector(&inner.x,
                     IRIS(parent)->inner_extension / njScalor(&inner));
        add_position(ARM(task)->child->pos, &inner.x);
        add_position(&inner.x, &outer.x);
    }
    njCalcPoint(NULL, &outer, &outer);
    njPopMatrixEx();
    IRIS(task)->node.object.pos[0] = outer.x;
    IRIS(task)->node.object.pos[1] = outer.y;
    IRIS(task)->node.object.pos[2] = outer.z;
    work->pos = outer;
}

static void panel_destroy(struct task *task);
static void panel_step(struct task *task);

static void panel_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Sint32 flags;
    Float reach;

    task->motion.p = syCalloc(1, sizeof(struct iris_motion));
    if (!task->motion.p) {
        return;
    }
    task->step = panel_step;
    task->destructor = panel_destroy;
    if (OUTER(work)) {
        IRIS(task)->node.object = *ARM(task);
    } else {
        IRIS(task)->node.object = *ARM(task)->child;
    }
    object = &IRIS(task)->node.object;
    object->child = NULL;
    object->sibling = NULL;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->evalflags |= NJD_EVAL_BREAK;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->ang[1] = task->parent->work->ang.y;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = PANEL_FLAGS;
    reach = object->model->r + CLASS_MARGIN;
    if (reach <= WORLD_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SIZE_CLASS_2_MAX) {
            flags = PANEL_FLAGS | WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SIZE_CLASS_3_MAX) {
            flags = PANEL_FLAGS | WORLD_SIZE_CLASS(2);
        } else {
            flags = PANEL_FLAGS | WORLD_SIZE_CLASS(3);
        }
    }
    panel_place(task);
    world_add(flags, task, object);
}

static void panel_destroy(struct task *task)
{
    if (IRIS(task)->node.object.model) {
        world_remove(task, &IRIS(task)->node.object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* @hack spelling: the travel as an inline walk of three Floats, where three
 * statements on the fields would do */
#pragma inline(displacement)
static void displacement(const Float *previous, const Float *now, Float *out)
{
    *out++ = *now++ - *previous++;
    *out++ = *now++ - *previous++;
    *out = *now - *previous;
}

static void panel_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 previous;
    NJS_VECTOR travel;

    previous = IRIS(task)->node.placed.pos;
    panel_place(task);
    /* @hack type: travel passed as (Float *)&travel, where &travel.x would
     * do */
    displacement(&previous.x, IRIS(task)->node.object.pos, (Float *)&travel);
    TASK_FORCES_OF(task)->speed = travel;
    if (player_near(&work->pos,
                    IRIS(task)->node.object.model->r + NEAR_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}
