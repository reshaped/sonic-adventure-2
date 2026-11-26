/* the hidden animal, WSMMLS in a stage's object list. it is an unseen shape
 * that offers the player inside it an action. a player who takes the action
 * lets out a small animal */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/hidden_animal.h"
#include "system/task.h"

/* the bit of ang.z that picks the shape, ELLIPSOID or BOX */
#define SHAPE_BIT 0x1
#define ELLIPSOID 0
#define BOX 1
/* the bits of ang.z that hold the animal's release kind */
#define KIND_BITS 0xf0
#define KIND_SHIFT 4
/* the shape's half size along each of its axes is scl's part on that axis
 * plus SIZE_ADD */
#define SIZE_ADD 10.0F
/* the action the shape offers. a player inside lets the animal out by taking
 * it, which puts it in the player's action_taken */
#define ACTION_HIDDEN_ANIMAL PLAYER_MODE_UNK_2A

static void hidden_animal_destroy(struct task *task);
static void hidden_animal_step(struct task *task);
static void hidden_animal_draw(struct task *task);

void hidden_animal_init(struct task *task)
{
    if (task->placement != NULL && placement_unk_20(task)) {
        placement_kill(task);
        return;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = hidden_animal_draw;
    task->step = hidden_animal_step;
    task->destructor = hidden_animal_destroy;
}

static void hidden_animal_destroy(struct task *task)
{
}

static void hidden_animal_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 point;
    int player;
    Bool inside;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a readout no path reaches. SHC still places its
     * strings in this source's constants, and the image holds the code of its
     * switch's cases */
    if (0) {
        njPrintC(NJM_LOCATION(37, 15), "<- ANGLE X");
        njPrintC(NJM_LOCATION(37, 16), "<- ANGLE Y");
        njPrintC(NJM_LOCATION(37, 17), "<- LEV( ) / SHAPE");
        switch (work->ang.z & SHAPE_BIT) {
        case ELLIPSOID:
            njPrintC(NJM_LOCATION(37, 18), "SPHERE");
            break;
        case BOX:
            njPrintC(NJM_LOCATION(37, 18), "RECTANGLE");
            break;
        }
        return;
    }
    player = player_nearest(&work->pos);
    if (player_work[player] == NULL) {
        return;
    }
    /* @hack spelling: inside set to FALSE here, where an initializer would
     * do */
    inside = FALSE;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njScale(NULL, 1.0F / (work->scl.x + SIZE_ADD),
            1.0F / (work->scl.y + SIZE_ADD), 1.0F / (work->scl.z + SIZE_ADD));
    njRotateY(NULL, -work->ang.y);
    njRotateX(NULL, -work->ang.x);
    njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
    njCalcPoint(NULL, &player_work[player]->pos, &point);
    njPopMatrix(1);
    /* @hack spelling: a default label on ELLIPSOID's case, where the case
     * alone would do: SHAPE_BIT leaves no other value */
    switch (work->ang.z & SHAPE_BIT) {
    default:
    case ELLIPSOID:
        if (njScalor(&point) < 1.0F) {
            inside = TRUE;
        }
        break;
    case BOX:
        if (njAbs(point.x) < 1.0F && njAbs(point.y) < 1.0F &&
            njAbs(point.z) < 1.0F) {
            inside = TRUE;
        }
        break;
    }
    if (inside) {
        if (player_records[player]->action_taken == ACTION_HIDDEN_ANIMAL) {
            animal_release(work->pos.x, work->pos.y, work->pos.z,
                           ((work->ang.z & KIND_BITS) >> KIND_SHIFT) %
                               ANIMAL_RELEASE_KINDS);
            if (task->placement != NULL) {
                placement_set_unk_20(task);
                placement_kill(task);
            } else {
                task_kill(task);
            }
        } else {
            player_records[player]->action = ACTION_HIDDEN_ANIMAL;
        }
    }
}

static void hidden_animal_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct angles ang;
    NJS_VECTOR scl;
    int model;

    /* @hack redundant: a debug draw no path reaches. the image holds the code
     * of its switch's cases and of its call */
    if (0) {
        scl = work->scl;
        switch (work->ang.z & SHAPE_BIT) {
        case ELLIPSOID:
            model = DEBUG_MODEL_SPHERE;
            break;
        case BOX:
            model = DEBUG_MODEL_BOX;
            break;
        }
        scl.x += SIZE_ADD;
        scl.y += SIZE_ADD;
        scl.z += SIZE_ADD;
        ang = work->ang;
        ang.z = 0;
        debug_draw_model(&work->pos, &ang, &scl, model);
    }
}
