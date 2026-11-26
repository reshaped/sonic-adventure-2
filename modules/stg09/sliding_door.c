/* the sliding door, CAGED in Prison Lane's object list. the cell door slides
 * aside once a group of enemies is beaten */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg09/door.h"
#include "stg09/sliding_door.h"
#include "system/angle.h"
#include "system/task.h"

#include "stg09/sliding_door/text.inc"

/* the door's modes, in the work's mode. in MODE_OPEN the door has slid, and
 * bounces at its end */
enum sliding_door_mode { MODE_SHUT, MODE_SLIDING, MODE_OPEN };

/* how far the door slides to open. a door that starts open starts OPEN_START
 * aside. its first step brings it back to DOOR_SLIDE, where it bounces */
#define DOOR_SLIDE 50.0F
#define OPEN_START 60.0F
#define GROUP(work) ((Uint8)(work)->ang.x)
/* in ang.z: the door slides toward +z, not -z */
#define SLIDE_PLUS_Z 0x1
/* the dust the door raises as it reaches its end stands upright. its center
 * lies DUST_HEIGHT up, and DUST_INSET short of the door's travel along z */
#define DUST_HEIGHT 30.0F
#define DUST_INSET 25.0F
#define DUST_HALF_WIDTH 30.0F

#define SLIDING_DOOR_SHAPES 1

static NJS_TEXLIST sliding_door_texlist;
static NJS_CNK_OBJECT sliding_door_object;

static struct collision_shape sliding_door_shapes[SLIDING_DOOR_SHAPES];

static void sliding_door_destroy(struct task *task);
static void sliding_door_step(struct task *task);
static void sliding_door_draw(struct task *task);

void sliding_door_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct door));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = sliding_door_draw;
    task->step = sliding_door_step;
    task->destructor = sliding_door_destroy;
    DOOR(task)->travel = 0.0F;
    collision_init(task, sliding_door_shapes, SLIDING_DOOR_SHAPES,
                   COLLISION_OBJECT);
    if (task->placement != NULL) {
        if (placement_get_state(task)) {
            work->mode = MODE_OPEN;
            DOOR(task)->travel = OPEN_START;
        }
    }
}

static void sliding_door_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->placement != NULL && work->mode == MODE_OPEN) {
        placement_set_state(task, DOOR_STATE_DONE);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void sliding_door_step(struct task *task)
{
    struct task_work *work = task->work;
    Float past;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SHUT:
        if (enemy_group_empty(GROUP(work))) {
            work->mode = MODE_SLIDING;
            work->timer = 0;
        }
        break;
    case MODE_SLIDING:
        DOOR(task)->travel += work->scl.x;
        if (DOOR(task)->travel >= DOOR_SLIDE) {
            NJS_POINT3 edge;
            struct angles ang;

            edge.z = (work->ang.z & SLIDE_PLUS_Z)
                         ? DOOR(task)->travel - DUST_INSET
                         : -DOOR(task)->travel + DUST_INSET;
            edge.y = DUST_HEIGHT;
            edge.x = 0.0F;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &edge, &edge);
            njPopMatrix(1);
            ang.x = 0;
            ang.y = work->ang.y;
            ang.z = QUARTER_TURN;
            dust_short_rect(&edge, DUST_HALF_WIDTH, DOOR_DUST_HALF_DEPTH, &ang,
                            DOOR_DUST_SCALE, DOOR_DUST_DENSITY,
                            DOOR_DUST_SPEED);
            sound_play_at_for(DOOR_STOP_SOUND, work, DOOR_SOUND_PRIORITY,
                              DOOR_SOUND_VOLUME, DOOR_STOP_UPDATES, &work->pos);
            work->mode = MODE_OPEN;
            DOOR(task)->speed += DOOR_KICK;
            work->timer = 0;
        } else {
            sound_play_at_for(DOOR_MOVE_SOUND, work, DOOR_SOUND_PRIORITY,
                              DOOR_SOUND_VOLUME, DOOR_MOVE_UPDATES, &work->pos);
        }
        break;
    case MODE_OPEN:
        DOOR(task)->travel += DOOR(task)->speed;
        past = DOOR(task)->travel - DOOR_SLIDE;
        DOOR(task)->speed =
            -(past * DOOR_SPRING) + (DOOR(task)->speed * DOOR_DAMPING);
        if (DOOR(task)->travel >= DOOR_SLIDE) {
            DOOR(task)->travel = DOOR_SLIDE;
            DOOR(task)->speed = -njAbs(DOOR(task)->speed);
        }
    }
    if (work->mode != MODE_SHUT) {
        if (!enemy_group_empty(GROUP(work))) {
            work->mode = MODE_SHUT;
            DOOR(task)->speed = 0.0F;
            DOOR(task)->travel = 0.0F;
            work->timer = 0;
        }
    }
    if (work->collision != NULL) {
        /* @hack spelling: the test of the shape's z plus the bit, where a
         * test of the bit would do while sliding_door_shapes' z is 0 */
        work->collision->shapes->center.z =
            sliding_door_shapes[0].center.z + (work->ang.z & SLIDE_PLUS_Z)
                ? DOOR(task)->travel
                : -DOOR(task)->travel;
        collision_register(task);
    }
}

static void sliding_door_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants */
    if (0) {
        (void)SLIDING_DOOR_EDITOR_ENE_ID;
        (void)SLIDING_DOOR_EDITOR_ANGLE_Y;
        (void)SLIDING_DOOR_EDITOR_DOORMODE;
        (void)SLIDING_DOOR_EDITOR_SHUT_SPEED;
    }
    njPushMatrix(NULL);
    njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njTranslate(NULL, 0.0F, 0.0F,
                (work->ang.z & SLIDE_PLUS_Z) ? DOOR(task)->travel
                                             : -DOOR(task)->travel);
    njSetTexture(&sliding_door_texlist);
    object = &sliding_door_object;
    njCnkEasyDrawModel(object->model);
    njPopMatrix(1);
}

#include "stg09/fixtures/sliding_door.h"

NJS_TEXLIST *sliding_door_texlists[3] = {
#include "stg09/sliding_door/sliding_door_texlists.inc"
};
/* the door's box. sliding_door_step slides the record's copy with the
 * door */
static struct collision_shape sliding_door_shapes[SLIDING_DOOR_SHAPES] = {
#include "stg09/sliding_door/sliding_door_shapes.inc"
};
