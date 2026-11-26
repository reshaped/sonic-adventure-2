/* the hammer, HAMMER in City Escape's object list: a solid column the
 * players can ride. it strokes along y to its pos and back, and rests at each
 * end */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "stg13/hammer.h"
#include "stg13/hammer/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* a hammer's modes, in its work's mode. MODE_TO_POS rests at the far end,
 * then strokes to pos. MODE_BACK rests at pos, then strokes back */
enum hammer_mode { MODE_TO_POS, MODE_BACK };

/* the hammer's settings, as hammer.h lists them */
#define HIGH_BYTE 0xff00
#define REST(work) ((Uint8)(work)->ang.x)
#define STROKES_DOWN(work) ((work)->ang.x & HIGH_BYTE)
#define TURN(work) ((work)->ang.y & HIGH_BYTE)
#define PHASE(work) ((Uint8)(work)->ang.y)
#define SPEED(work) ((Uint16)(work)->ang.z)
#define WIDTH(work) ((work)->scl.x + 1.0F)
#define HEIGHT(work) ((work)->scl.y + 1.0F)
/* PHASE(work) counts a cycle in PHASE_STEPS parts */
#define PHASE_STEPS 256

/* the collision model's half width and half height, about its origin. the
 * hammer's radius reaches a top corner of the model scaled by HEIGHT along y
 * alone */
#define HALF_WIDTH 25.0F
#define HALF_HEIGHT 50.0F
/* a player within the hammer's radius plus REACH_MARGIN of its center lets
 * the world collect the copy */
#define REACH_MARGIN 10.0F

/* how far the hammer lies above its pos, below 0 for below it. task->extra
 * holds the Float in place of a pointer. task_destroy would free extra as a
 * record, so the destructor nulls it */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define OFFSET(task) (*(Float *)&(task)->extra)
/* the hammer's radius, in task->unk_3c */
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define RADIUS(task) (*(Float *)&(task)->unk_3c)
/* the TASK_FORCE record at index, of two */
#define FORCE(task, index) (&((struct task_force *)(task)->force.p)[index])
/* a stroke's share done, from 0 to 1, given its cosine, from 1 to -1 */
#define STROKE_SHARE(cosine) (0.5F + ((cosine) * 0.5F))

static NJS_TEXNAME hammer_texture_names[3] = {
#include "stg13/hammer/hammer_texture_names.inc"
};

static NJS_TEXLIST hammer_texlist = {
#include "stg13/hammer/hammer_texlist.inc"
};

static Sint16 hammer_display_polygons[429] = {
#include "stg13/hammer/hammer_display_polygons.inc"
};

static Sint32 hammer_display_vertices[271] = {
#include "stg13/hammer/hammer_display_vertices.inc"
};

static NJS_CNK_MODEL hammer_model = {
#include "stg13/hammer/hammer_model.inc"
};

NJS_CNK_OBJECT hammer_object = {
#include "stg13/hammer/hammer_object.inc"
};

static NJS_MATERIAL hammer_collision_materials[1] = {
#include "stg13/hammer/hammer_collision_materials.inc"
};

static Sint16 hammer_collision_indices[35] = {
#include "stg13/hammer/hammer_collision_indices.inc"
};

static NJS_MESHSET hammer_collision_meshsets[1] = {
#include "stg13/hammer/hammer_collision_meshsets.inc"
};

static NJS_POINT3 hammer_collision_points[12] = {
#include "stg13/hammer/hammer_collision_points.inc"
};

static NJS_VECTOR hammer_collision_normals[12] = {
#include "stg13/hammer/hammer_collision_normals.inc"
};

static NJS_MODEL hammer_collision_model = {
#include "stg13/hammer/hammer_collision_model.inc"
};

static NJS_OBJECT hammer_collision = {
#include "stg13/hammer/hammer_collision.inc"
};

NJS_TEXLIST *hammer_texlists[2] = {
#include "stg13/hammer/hammer_texlists.inc"
};

static void hammer_destroy(struct task *task);
static void hammer_step(struct task *task);
static void hammer_draw(struct task *task);
static void hammer_set_phase(struct task *task);
static Uint32 hammer_size_class(Float radius);

void hammer_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Uint32 flags;
    Float height;
    Float area;

    if (placement_try_leave(task)) {
        return;
    }
    task->draw = hammer_draw;
    task->step = hammer_step;
    task->destructor = hammer_destroy;
    hammer_set_phase(task);
    OFFSET(task) = 0.0F;
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. the image holds none of
     * its code. its store is a stand-in, without which SHC builds the code
     * below otherwise */
    if (0) {
        (void)"<- UPDOWN / REST(   )";
        (void)"(0:UP not0:DOWN)<    >";
        (void)"DOWN";
        (void)" UP ";
        (void)"<- ANG Y / RUG(  %)";
        (void)"<- UPDOWN SPEED";
        (void)"<- WIDTH SCALE(1.0+x)";
        (void)"<- HEIGHT SCALE(1.0+x)";
        (void)"<- HOVER HEIGHT";
        task->work->ang.x |= task->work->ang.y;
    }
    object = world_pool_alloc();
    task->motion.p = object;
    /* @bug memory: copies hammer_collision through null where
     * world_pool_alloc has none to lend (MOV.L at 0x8c50599e).
     * hammer_destroy tests task->motion for null (TST at 0x8c505a7a).
     * hammer_step reads through it untested (FMOV.S at 0x8c505b82) */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the pool has no object, the init
     * destroys the hammer, and set.c makes it again on a later pass */
    if (object == NULL) {
        task_destroy(task);
        return;
    }
#endif
    *object = hammer_collision;
    object->evalflags &=
        ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL);
    /* @hack spelling: task->work read again for the scales, the turn and the
     * height, where work would do */
    object->scl[0] = WIDTH(task->work);
    object->scl[1] = HEIGHT(task->work);
    object->scl[2] = WIDTH(task->work);
    object->ang[1] = TURN(task->work);
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y;
    object->pos[2] = work->pos.z;
    flags = WORLD_MOVING | WORLD_SOLID;
    /* @hack spelling: height set and then raised by 1, where HEIGHT(work)
     * in one statement would do */
    height = task->work->scl.y;
    height += 1.0F;
    /* @hack spelling: area, a local for the product, where the product
     * written in the call would do */
    area = height * height * (HALF_HEIGHT * HALF_HEIGHT);
    RADIUS(task) = njSqrt(area + (HALF_WIDTH * HALF_WIDTH));
    flags |= hammer_size_class(RADIUS(task));
    world_add(flags, task, object);
}

static void hammer_destroy(struct task *task)
{
    if (task->motion.p != NULL) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->extra.p = NULL;
    task->motion.p = NULL;
}

static void hammer_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Angle ang;
    Float stroke;

    if (placement_try_leave(task)) {
        return;
    }
    hammer_set_phase(task);
    /* @hack spelling: task->work read again for the timer, the speed and the
     * stroke's way, where work would do */
    ang = (Uint16)(task->work->timer * SPEED(task->work));
    ang += work->mode != MODE_TO_POS ? HALF_TURN : 0;
    stroke = njSin(ang + QUARTER_TURN);
    /* @hack redundant: a store to stroke that no path reaches */
    if (0) {
        stroke = 0.0F;
    }
    if (STROKES_DOWN(task->work)) {
        OFFSET(task) = STROKE_SHARE(stroke) * work->scl.z;
    } else {
        OFFSET(task) = -(STROKE_SHARE(stroke) * work->scl.z);
    }
    object = task->motion.p;
    FORCE(task, 0)->speed.y = FORCE(task, 1)->speed.y =
        work->pos.y + OFFSET(task) - object->pos[1];
    object->pos[1] = work->pos.y + OFFSET(task);
    if (player_near((NJS_POINT3 *)object->pos, RADIUS(task) + REACH_MARGIN)) {
        work->flags |= WORLD_ADDED;
    } else {
        work->flags &= ~WORLD_ADDED;
    }
}

static void hammer_draw(struct task *task)
{
    struct task_work *work = task->work;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + OFFSET(task), work->pos.z);
    /* @hack spelling: task->work read again for the turn and the scale,
     * where work would do */
    njRotateY(NULL, TURN(task->work));
    njScale(NULL, WIDTH(task->work), HEIGHT(task->work), WIDTH(task->work));
    njSetTexture(&hammer_texlist);
    njCnkEasyDrawModel(&hammer_model);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

/* set the work's mode, timer and tick to the hammer's place in its cycle,
 * from game_state.steps. timer counts a stroke's steps, and is 0 through a
 * rest. tick counts the steps left of a rest, and is 0 through a stroke */
static void hammer_set_phase(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 stroke;
    Uint32 rest;
    Uint32 half;
    Uint32 cycle;
    Uint32 place;

    /* @hack spelling: task->work read again for all but the mode, where work
     * would do */
    stroke = HALF_TURN / SPEED(task->work);
    rest = REST(task->work);
    half = stroke + rest;
    cycle = half * 2;
    if (cycle != 0) {
        place = (Sint32)((Float)game_state.steps +
                         ((Float)cycle * (Float)PHASE(task->work) *
                          (1.0F / PHASE_STEPS))) %
                cycle;
    } else {
        place = 0;
    }
    task->work->tick = 0;
    if (place >= half) {
        task->work->timer = place - half;
        work->mode = MODE_BACK;
        if (task->work->timer > stroke) {
            task->work->tick = half - task->work->timer;
            task->work->timer = 0;
            work->mode = MODE_TO_POS;
        }
    } else {
        task->work->timer = place;
        work->mode = MODE_TO_POS;
        if (task->work->timer > stroke) {
            task->work->tick = half - task->work->timer;
            task->work->timer = 0;
            work->mode = MODE_BACK;
        }
    }
}

/* the WORLD_SIZE_CLASS flags of the class radius falls in */
static Uint32 hammer_size_class(Float radius)
{
    /* @hack spelling: an else after each return, where each test after the
     * if before it would do */
    if (radius > WORLD_SIZE_CLASS_1_MAX) {
        return WORLD_SIZE_CLASS(0);
    } else if (radius > WORLD_SIZE_CLASS_2_MAX) {
        return WORLD_SIZE_CLASS(1);
    } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
        return WORLD_SIZE_CLASS(2);
    } else {
        return WORLD_SIZE_CLASS(3);
    }
}
