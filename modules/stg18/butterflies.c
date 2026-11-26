/* the butterflies, BUTTERFLY in Dry Lagoon's object list: a flock that
 * flutters about a point */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "object/set.h"
#include "shared/common_object.h"
#include "shared/object_builds.h"
#include "stg18/butterflies.h"
#include "system/port.h"
#include "stg18/butterflies/text.inc"
#include "stg18/lagoon_util.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/task.h"
#include "system/util.h"

enum butterfly_kind { KIND_BLUE, KIND_WHITE, KIND_YELLOW, KINDS };
/* a wing beat's models, each drawn for two steps */
#define FRAMES 3
#define BUILDS (KINDS * FRAMES)

/* a butterfly of a flock, from syMalloc. the flock's task->extra holds the
 * first of its list */
struct butterfly {
    Sint32 kind;
    /* added to game_state.steps to time its flight, and to half of it to time
     * its wing beat */
    Sint32 phase;
    NJS_POINT3 pos;
    /* the turns about x and about y that head it along its last move */
    Angle ang_x;
    Angle ang_y;
    Uint8 unk_1c[4];
    Float scale;
    /* how far the sines that swing its radius, its pitch and its yaw turn
     * each step, as Angles. pitch_rate and yaw_rate may be negative */
    Sint32 radius_rate;
    Sint32 pitch_rate;
    Sint32 yaw_rate;
    struct butterfly *prev;
    struct butterfly *next;
    struct task *owner;
};

/* a butterfly's distance from pos swings about RADIUS_MID of scl.x, by
 * RADIUS_SWING of scl.x either way */
#define RADIUS_MID 0.667F
#define RADIUS_SWING 0.333F
/* a new butterfly's scale and rates each lie from a base up to the base plus
 * a spread. the scale's base is 1 */
#define SCALE_SPREAD 0.25F
#define RADIUS_RATE 32.0F
#define RADIUS_SPREAD 16.0F
#define TURN_RATE 48.0F
#define TURN_SPREAD 48.0F
/* a new butterfly's phase lies from 0 up to PHASE_SPREAD * RAND_MAX. it
 * scales rand() itself, where the spreads above scale rand() * RAND_SCALE */
#define PHASE_SPREAD 2.0F

/* Three wing poses for each butterfly color. */

static NJS_TEXNAME butterfly_blue_texlist_0_names[1] = {
#include "stg18/butterflies/butterfly_blue_texlist_0_names.inc"
};

static NJS_TEXLIST butterfly_blue_texlist_0 = {
#include "stg18/butterflies/butterfly_blue_texlist_0.inc"
};

static Sint16 butterfly_blue_object_0_polygons[31] = {
#include "stg18/butterflies/butterfly_blue_object_0_polygons.inc"
};

static Sint32 butterfly_blue_object_0_vertices[27] = {
#include "stg18/butterflies/butterfly_blue_object_0_vertices.inc"
};

static NJS_CNK_MODEL butterfly_blue_model_0 = {
#include "stg18/butterflies/butterfly_blue_model_0.inc"
};

static NJS_CNK_OBJECT butterfly_blue_object_0 = {
#include "stg18/butterflies/butterfly_blue_object_0.inc"
};

static NJS_TEXNAME butterfly_blue_texlist_1_names[1] = {
#include "stg18/butterflies/butterfly_blue_texlist_1_names.inc"
};

static NJS_TEXLIST butterfly_blue_texlist_1 = {
#include "stg18/butterflies/butterfly_blue_texlist_1.inc"
};

static Sint16 butterfly_blue_object_1_polygons[31] = {
#include "stg18/butterflies/butterfly_blue_object_1_polygons.inc"
};

static Sint32 butterfly_blue_object_1_vertices[27] = {
#include "stg18/butterflies/butterfly_blue_object_1_vertices.inc"
};

static NJS_CNK_MODEL butterfly_blue_model_1 = {
#include "stg18/butterflies/butterfly_blue_model_1.inc"
};

static NJS_CNK_OBJECT butterfly_blue_object_1 = {
#include "stg18/butterflies/butterfly_blue_object_1.inc"
};

static NJS_TEXNAME butterfly_blue_texlist_2_names[1] = {
#include "stg18/butterflies/butterfly_blue_texlist_2_names.inc"
};

static NJS_TEXLIST butterfly_blue_texlist_2 = {
#include "stg18/butterflies/butterfly_blue_texlist_2.inc"
};

static Sint16 butterfly_blue_object_2_polygons[31] = {
#include "stg18/butterflies/butterfly_blue_object_2_polygons.inc"
};

static Sint32 butterfly_blue_object_2_vertices[27] = {
#include "stg18/butterflies/butterfly_blue_object_2_vertices.inc"
};

static NJS_CNK_MODEL butterfly_blue_model_2 = {
#include "stg18/butterflies/butterfly_blue_model_2.inc"
};

static NJS_CNK_OBJECT butterfly_blue_object_2 = {
#include "stg18/butterflies/butterfly_blue_object_2.inc"
};

static NJS_TEXNAME butterfly_white_texlist_0_names[1] = {
#include "stg18/butterflies/butterfly_white_texlist_0_names.inc"
};

static NJS_TEXLIST butterfly_white_texlist_0 = {
#include "stg18/butterflies/butterfly_white_texlist_0.inc"
};

static Sint16 butterfly_white_object_0_polygons[31] = {
#include "stg18/butterflies/butterfly_white_object_0_polygons.inc"
};

static Sint32 butterfly_white_object_0_vertices[27] = {
#include "stg18/butterflies/butterfly_white_object_0_vertices.inc"
};

static NJS_CNK_MODEL butterfly_white_model_0 = {
#include "stg18/butterflies/butterfly_white_model_0.inc"
};

static NJS_CNK_OBJECT butterfly_white_object_0 = {
#include "stg18/butterflies/butterfly_white_object_0.inc"
};

static NJS_TEXNAME butterfly_white_texlist_1_names[1] = {
#include "stg18/butterflies/butterfly_white_texlist_1_names.inc"
};

static NJS_TEXLIST butterfly_white_texlist_1 = {
#include "stg18/butterflies/butterfly_white_texlist_1.inc"
};

static Sint16 butterfly_white_object_1_polygons[31] = {
#include "stg18/butterflies/butterfly_white_object_1_polygons.inc"
};

static Sint32 butterfly_white_object_1_vertices[27] = {
#include "stg18/butterflies/butterfly_white_object_1_vertices.inc"
};

static NJS_CNK_MODEL butterfly_white_model_1 = {
#include "stg18/butterflies/butterfly_white_model_1.inc"
};

static NJS_CNK_OBJECT butterfly_white_object_1 = {
#include "stg18/butterflies/butterfly_white_object_1.inc"
};

static NJS_TEXNAME butterfly_white_texlist_2_names[1] = {
#include "stg18/butterflies/butterfly_white_texlist_2_names.inc"
};

static NJS_TEXLIST butterfly_white_texlist_2 = {
#include "stg18/butterflies/butterfly_white_texlist_2.inc"
};

static Sint16 butterfly_white_object_2_polygons[31] = {
#include "stg18/butterflies/butterfly_white_object_2_polygons.inc"
};

static Sint32 butterfly_white_object_2_vertices[27] = {
#include "stg18/butterflies/butterfly_white_object_2_vertices.inc"
};

static NJS_CNK_MODEL butterfly_white_model_2 = {
#include "stg18/butterflies/butterfly_white_model_2.inc"
};

static NJS_CNK_OBJECT butterfly_white_object_2 = {
#include "stg18/butterflies/butterfly_white_object_2.inc"
};

static NJS_TEXNAME butterfly_yellow_texlist_0_names[1] = {
#include "stg18/butterflies/butterfly_yellow_texlist_0_names.inc"
};

static NJS_TEXLIST butterfly_yellow_texlist_0 = {
#include "stg18/butterflies/butterfly_yellow_texlist_0.inc"
};

static Sint16 butterfly_yellow_object_0_polygons[31] = {
#include "stg18/butterflies/butterfly_yellow_object_0_polygons.inc"
};

static Sint32 butterfly_yellow_object_0_vertices[27] = {
#include "stg18/butterflies/butterfly_yellow_object_0_vertices.inc"
};

static NJS_CNK_MODEL butterfly_yellow_model_0 = {
#include "stg18/butterflies/butterfly_yellow_model_0.inc"
};

static NJS_CNK_OBJECT butterfly_yellow_object_0 = {
#include "stg18/butterflies/butterfly_yellow_object_0.inc"
};

static NJS_TEXNAME butterfly_yellow_texlist_1_names[1] = {
#include "stg18/butterflies/butterfly_yellow_texlist_1_names.inc"
};

static NJS_TEXLIST butterfly_yellow_texlist_1 = {
#include "stg18/butterflies/butterfly_yellow_texlist_1.inc"
};

static Sint16 butterfly_yellow_object_1_polygons[31] = {
#include "stg18/butterflies/butterfly_yellow_object_1_polygons.inc"
};

static Sint32 butterfly_yellow_object_1_vertices[27] = {
#include "stg18/butterflies/butterfly_yellow_object_1_vertices.inc"
};

static NJS_CNK_MODEL butterfly_yellow_model_1 = {
#include "stg18/butterflies/butterfly_yellow_model_1.inc"
};

static NJS_CNK_OBJECT butterfly_yellow_object_1 = {
#include "stg18/butterflies/butterfly_yellow_object_1.inc"
};

static NJS_TEXNAME butterfly_yellow_texlist_2_names[1] = {
#include "stg18/butterflies/butterfly_yellow_texlist_2_names.inc"
};

static NJS_TEXLIST butterfly_yellow_texlist_2 = {
#include "stg18/butterflies/butterfly_yellow_texlist_2.inc"
};

static Sint16 butterfly_yellow_object_2_polygons[31] = {
#include "stg18/butterflies/butterfly_yellow_object_2_polygons.inc"
};

static Sint32 butterfly_yellow_object_2_vertices[27] = {
#include "stg18/butterflies/butterfly_yellow_object_2_vertices.inc"
};

static NJS_CNK_MODEL butterfly_yellow_model_2 = {
#include "stg18/butterflies/butterfly_yellow_model_2.inc"
};

static NJS_CNK_OBJECT butterfly_yellow_object_2 = {
#include "stg18/butterflies/butterfly_yellow_object_2.inc"
};

static NJS_CNK_MODEL *butterfly_blue_models[FRAMES] = {
#include "stg18/butterflies/butterfly_blue_models.inc"
};
static NJS_CNK_MODEL *butterfly_white_models[FRAMES] = {
#include "stg18/butterflies/butterfly_white_models.inc"
};
static NJS_CNK_MODEL *butterfly_yellow_models[FRAMES] = {
#include "stg18/butterflies/butterfly_yellow_models.inc"
};
KEPT static NJS_CNK_MODEL **butterfly_models[KINDS] = {
#include "stg18/butterflies/butterfly_models.inc"
};
/* a kind's three texture lists name one texture, so the draw sets the first
 * for all three */
NJS_TEXLIST *butterflies_texlists[4] = {
#include "stg18/butterflies/butterflies_texlists.inc"
};
static Bool butterflies_built = FALSE;
/* by kind, then by frame */
static struct model_build butterflies_builds[BUILDS] = {
#include "stg18/butterflies/butterflies_builds.inc"
};

static void butterflies_destroy(struct task *task);
static void butterflies_step(struct task *task);
static void butterflies_draw(struct task *task);
static struct butterfly *butterfly_add(struct task *task);
static void butterfly_remove(struct butterfly *butterfly);
static void butterfly_fly(struct butterfly *butterfly);
static void butterfly_draw(struct butterfly *butterfly);

Uint8 *butterflies_build(Uint8 *buf)
{
    if (objects_fog_off) {
        njFogDisable();
    }
    if (model_builds_compile(&buf, butterflies_builds, BUILDS) == BUILDS) {
        butterflies_built = TRUE;
    }
    if (objects_fog_off) {
        njFogEnable();
    }
    return buf;
}

void butterflies_init(struct task *task)
{
    int i;

    if (!placement_try_leave(task)) {
        /* @hack spelling: the draw set before the step, where struct task's
         * order would set the step first */
        task->draw = butterflies_draw;
        task->step = butterflies_step;
        task->destructor = butterflies_destroy;
        for (i = 0; i < task->work->ang.z; i++) {
            butterfly_add(task);
        }
    }
}

static void butterflies_destroy(struct task *task)
{
    while (task->extra.p != NULL) {
        butterfly_remove(task->extra.p);
    }
}

static void butterflies_step(struct task *task)
{
    struct butterfly *butterfly = task->extra.p;

    if (!placement_try_leave(task)) {
        while (butterfly != NULL) {
            butterfly_fly(butterfly);
            butterfly = butterfly->next;
        }
    }
}

static void butterflies_draw(struct task *task)
{
    struct butterfly *butterfly = task->extra.p;

    while (butterfly != NULL) {
        butterfly_draw(butterfly);
        butterfly = butterfly->next;
    }
}

/* add a new butterfly at the head of task's list, and return it. it returns
 * null where syMalloc can't give one. the call leaves pos as the heap left
 * it */
static struct butterfly *butterfly_add(struct task *task)
{
    struct butterfly *butterfly;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((butterfly = syMalloc(sizeof(struct butterfly))) == NULL) {
        return NULL;
    }
    butterfly->prev = NULL;
    butterfly->next = task->extra.p;
    if (butterfly->next != NULL) {
        butterfly->next->prev = butterfly;
    }
    butterfly->owner = task;
    task->extra.p = butterfly;
    butterfly->kind = (Sint32)(rand() * (KINDS * RAND_SCALE));
    butterfly->phase = (Sint32)(rand() * PHASE_SPREAD);
    butterfly->scale = 1.0F + (rand() * (SCALE_SPREAD * RAND_SCALE));
    butterfly->radius_rate =
        (Sint32)(RADIUS_RATE + (rand() * (RADIUS_SPREAD * RAND_SCALE)));
    butterfly->pitch_rate =
        (Sint32)(TURN_RATE + (rand() * (TURN_SPREAD * RAND_SCALE)));
    butterfly->yaw_rate =
        (Sint32)(TURN_RATE + (rand() * (TURN_SPREAD * RAND_SCALE)));
    if (butterfly->pitch_rate & 1) {
        butterfly->pitch_rate = -butterfly->pitch_rate;
    }
    if (butterfly->yaw_rate & 1) {
        butterfly->yaw_rate = -butterfly->yaw_rate;
    }
#ifdef NONMATCHING
    /* this side repairs the tag in butterfly_fly: a new butterfly starts at
     * its flock's pos, so its first step heads it outward from there */
    butterfly->pos = task->work->pos;
#endif
    return butterfly;
}

/* take butterfly out of its owner's list and free it. a null butterfly
 * changes nothing */
static void butterfly_remove(struct butterfly *butterfly)
{
    if (butterfly != NULL) {
        if (butterfly->next != NULL) {
            butterfly->next->prev = butterfly->prev;
        }
        if (butterfly->prev != NULL) {
            butterfly->prev->next = butterfly->next;
        } else if (butterfly->owner != NULL) {
            butterfly->owner->extra.p = butterfly->next;
        }
        syFree(butterfly);
    }
}

static void butterfly_fly(struct butterfly *butterfly)
{
    struct task *task = butterfly->owner;
    struct task_work *work = task->work;
    Uint32 time = game_state.steps + butterfly->phase;
    Float radius;
    Angle pitch;
    Angle yaw;
    NJS_POINT3 offset;
    NJS_POINT3 last;

    /* @hack spelling: task->work read again for scl.x and ang.x, where work
     * would do */
    radius = ((njSin(time * butterfly->radius_rate) * task->work->scl.x) *
              RADIUS_SWING) +
             (task->work->scl.x * RADIUS_MID);
    pitch = njSin(time * butterfly->pitch_rate) * task->work->ang.x;
    yaw = njSin(time * butterfly->yaw_rate) * task->work->ang.x;
    offset.x = offset.z = 0.0F;
    offset.y = radius;
    /* @bug uninitialized: heads a butterfly on its first step by its move
     * from pos as the heap left it, which butterfly_add never sets. the read
     * is at 0x8c509d36 */
    last = butterfly->pos;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, yaw);
    njRotateX(NULL, pitch);
    njCalcPoint(NULL, &offset, &butterfly->pos);
    njPopMatrixEx();
    direction_angles_xy(butterfly->pos.x - last.x, butterfly->pos.y - last.y,
                        butterfly->pos.z - last.z, &butterfly->ang_x,
                        &butterfly->ang_y);
}

static void butterfly_draw(struct butterfly *butterfly)
{
    int frame = ((game_state.steps >> 1) + butterfly->phase) % FRAMES;

    njPushMatrixEx();
    njTranslateEx(&butterfly->pos);
    njRotateY(NULL, butterfly->ang_y);
    njRotateX(NULL, butterfly->ang_x);
    njSetTexture(butterflies_texlists[butterfly->kind]);
    if (butterflies_built) {
        lagoon_direct_draw_scaled(
            butterflies_builds[(butterfly->kind * FRAMES) + frame].built,
            butterfly->scale);
    } else {
        lagoon_model_draw_scaled(butterfly_models[butterfly->kind][frame],
                                 butterfly->scale);
    }
    njPopMatrixEx();
}
