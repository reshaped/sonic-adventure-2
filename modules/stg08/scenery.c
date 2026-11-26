/* DOBJF in Security Hall's object list: scenery that players stand on. scl.x
 * picks the kind, clamped to the kinds there are. the placement's ang.y turns
 * the scenery, and it shows only on camera 0's floor */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "stg08/scenery/text.inc"
#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/model_kind.h"
#include "stg08/background.h"
#include "stg08/floor_object.h"
#include "stg08/solid_props.h"
#include "system/areas.h"
#include "system/task.h"

/* world_add classes an object's copy of its kind's solid by the copy's
 * radius + SIZE_MARGIN. players meet the copy only while one is within that
 * sum of the copy's center */
#define SIZE_MARGIN 15.0F

/* shared/model_kind.h's model_kind, with solid after object. solid is the
 * model that the object puts in the world, or null for none */
struct scenery_kind {
    Uint32 flags;
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
    NJS_OBJECT *solid;
    struct collision_shape *shapes;
    Sint32 shape_count;
    void (*draw)(struct task *task);
    void (*step)(struct task *task, struct task_work *work,
                 struct scenery_kind *kind);
    struct model_alternate *alternate;
    Sint32 unk_24;
    NJS_DIRECT_HEAD *built;
};

struct scenery_table {
    struct scenery_kind *kinds;
    Sint32 count;
};

/* an object's record, in task->extra */
struct scenery_record {
    /* the copy of its kind's solid that the collision world holds, placed
     * where the object lies */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the copy's center in the world */
};

/* the set editor's labels for scl.x and ang.y */
static const char editor_model[16] = {
#include "stg08/scenery/editor_model.inc"
};
static const char editor_angle_y[12] = {
#include "stg08/scenery/editor_angle_y.inc"
};

/* each header defines a kind's models. the object's data holds them in the
 * order of these includes */
#include "stg08/safe/scenery_0.h"
#include "stg08/safe/scenery_1.h"
#include "stg08/safe/scenery_2.h"
#include "stg08/safe/scenery_3.h"
#include "stg08/safe/scenery_4.h"

NJS_TEXLIST *security_scenery_textures[6] = {
#include "stg08/scenery/security_scenery_textures.inc"
};

static void scenery_start(struct task *task);
static void scenery_destroy(struct task *task);
static void scenery_step(struct task *task);
static void scenery_draw(struct task *task);

static struct scenery_kind kinds[5] = {
#include "stg08/scenery/kinds.inc"
};
static struct scenery_table scenery = {
#include "stg08/scenery/scenery.inc"
};

#define TABLE(task) ((struct scenery_table *)(task)->motion.p)
#define KIND(task, work) (&TABLE(task)->kinds[(int)(work)->scl.x])
#define RECORD(task) ((struct scenery_record *)(task)->extra.p)

void security_scenery_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct scenery_record));
    if (task->extra.p) {
        task->motion.p = &scenery;
        scenery_start(task);
    }
}

Uint8 *security_scenery_build(Uint8 *buffer)
{
    struct scenery_table *list = &scenery;
    int i;
    struct scenery_kind *kind;
    NJS_CNK_OBJECT copy;
    Uint8 *end;

    /* @hack spelling: a do loop under a test of the count, where a for loop
     * would do */
    i = 0;
    if (list->count > 0) {
        do {
            kind = &list->kinds[i];
            if ((kind->flags & KIND_BUILD) && !(kind->flags & KIND_COMPILED)) {
                copy = *kind->object;
                if (!(kind->flags & KIND_TREE)) {
                    copy.sibling = NULL;
                    copy.child = NULL;
                    copy.evalflags |= NJD_EVAL_BREAK;
                }
                njSetTexture(kind->texlist);
                end = buffer + njCnkDirectObjectCompileSize(&copy);
                if (end < DEMO_AREA) {
                    njCnkDirectObjectCompile(&copy, buffer);
                    kind->built = (NJS_DIRECT_HEAD *)buffer;
                    buffer = end;
                    kind->flags |= KIND_COMPILED;
                }
            }
        } while (++i < list->count);
    }
    return buffer;
}

static void scenery_start(struct task *task)
{
    struct task_work *work = task->work;
    struct scenery_kind *kind;
    NJS_OBJECT *object;
    Float radius;
    Uint32 flags;
    int i;

    task->step = scenery_step;
    task->destructor = scenery_destroy;
    if (work->scl.x < 0.0F) {
        work->scl.x = 0.0F;
    }
    if ((int)work->scl.x > TABLE(task)->count - 1) {
        work->scl.x = TABLE(task)->count - 1;
    }
    kind = KIND(task, work);
    if (kind->shapes) {
        if (kind->flags & KIND_DOUBLE_SHAPES) {
            collision_init(task, kind->shapes, kind->shape_count * 2,
                           COLLISION_OBJECT);
            for (i = 0; i < kind->shape_count; i++) {
                work->collision->shapes[i + kind->shape_count].flags |=
                    SHAPE_OFF;
            }
        } else {
            collision_init(task, kind->shapes, kind->shape_count,
                           COLLISION_OBJECT);
        }
    }
    if (kind->solid) {
        object = &RECORD(task)->object;
        *object = *kind->solid;
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->child = NULL;
        object->sibling = NULL;
        object->ang[1] = work->ang.y;
        object->ang[2] = 0;
        object->ang[0] = 0;
        object->pos[0] = work->pos.x;
        object->pos[1] = work->pos.y;
        object->pos[2] = work->pos.z;
        radius = object->model->r + SIZE_MARGIN;
        flags = WORLD_SOLID;
        CLASSIFY_SOLID(flags, radius);
        world_add(flags, task, object);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        /* @hack spelling: the copy read through RECORD(task), where object
         * would do */
        njTranslateEx((NJS_VECTOR *)RECORD(task)->object.pos);
        njRotateY(NULL, RECORD(task)->object.ang[1]);
        njCalcPoint(NULL, &RECORD(task)->object.model->center,
                    &RECORD(task)->center);
        njPopMatrixEx();
    }
    task->draw = kind->draw;
    if (task->placement) {
        work->submode = placement_get_state(task);
    }
}

static void scenery_destroy(struct task *task)
{
    struct task_work *work = task->work;

    if (task->extra.p) {
        if (RECORD(task)->object.model) {
            world_remove(task, &RECORD(task)->object);
        }
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
    task->motion.p = NULL;
    if (task->placement) {
        placement_set_state(task, work->submode);
    }
}

static void scenery_step(struct task *task)
{
    struct task_work *work = task->work;
    struct scenery_kind *kind;
    struct scenery_record *record;
    Float height;
    int inactive_mask;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: ~WORLD_ADDED and security_floor_height held in locals,
     * where the two would do in place */
    inactive_mask = ~WORLD_ADDED;
    height = security_floor_height;
    SELECT_FLOOR(work, height, inactive_mask);
    kind = KIND(task, work);
    if (kind->step) {
        kind->step(task, work, kind);
    } else if (work->collision) {
        collision_register(task);
    }
    if (kind->solid) {
        record = task->extra.p;
        /* @hack spelling: the center read through RECORD(task), and the
         * copy's model through record, where record would do for both */
        if (player_near(&RECORD(task)->center,
                        record->object.model->r + SIZE_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= inactive_mask;
        }
    }
}

static void scenery_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct scenery_kind *kind;

    if (work->tick) {
        kind = KIND(task, work);
        njSetTexture(kind->texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        if (kind->flags & KIND_COMPILED) {
            njDirectDrawObject(kind->built);
        } else {
            njCnkEasyDrawModel(kind->object->model);
        }
        njPopMatrixEx();
    }
}
