/* the electric pillar ELEPIL, whose arc shows a cell of
 * electric_arc_texlist's texture a step */
#include <ninja.h>
#include <sg_xpt.h>
#ifdef NONMATCHING
/* memcpy, with which this side's draw copies the arc's polygon list */
#include <string.h>
#endif

#include "camera/camera.h"
#include "collision/collision.h"
#include "object/set.h"
#include "stg34/electric_pillar.h"
#include "stg34/scenery.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the arc's texture holds its cells in rows of ARC_COLUMNS. a cell is
 * CELL_WIDTH wide in u and CELL_HEIGHT tall in v. the arc shows each of
 * ARC_CELLS cells for a step, a row at a time */
#define ARC_COLUMNS 16
#define ARC_CELLS 64
#define CELL_WIDTH 16
#define CELL_HEIGHT 64
/* electric_pillar_cell_edge rounds u and v alike to a multiple of CELL_WIDTH.
 * each edge of a cell is one, in v as in u. a coordinate up to EDGE_SLACK below
 * an edge rounds up to it */
#define EDGE_SLACK 2
/* the shorts of the arc's polygon list ahead of its strip's length */
#define ARC_HEAD 11
/* the copy of the arc's polygon list that the draw changes */
#define ARC_COPY ((Sint16 *)WORK_AREA)

static void pillar_step(struct task *task);
static void pillar_destroy(struct task *task);
static void electric_pillar_draw(struct task *task);

static struct collision_shape pillar_shape;

void electric_pillar_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = electric_pillar_draw;
        task->step = pillar_step;
        task->destructor = pillar_destroy;
        collision_init(task, &pillar_shape, PILLAR_SHAPES, COLLISION_OBJECT);
    }
}

#include "stg34/pillar_impl.h"

static struct collision_shape pillar_shape = PILLAR_SHAPE(10.0F, 350.0F);

NJS_TEXLIST *electric_pillar_texlists[] = {&electric_pillar_texlist,
                                           &electric_arc_texlist, NULL};

/* @hack spelling: an external function, where a static one would do */
int electric_pillar_cell_edge(int coordinate)
{
    return ((coordinate + EDGE_SLACK) / CELL_WIDTH) * CELL_WIDTH;
}

static void electric_pillar_draw(struct task *task)
{
    struct task_work *work = task->work;
    int column = game_state.steps % ARC_COLUMNS;
    int row = game_state.steps % ARC_CELLS / ARC_COLUMNS;
#ifdef NONMATCHING
    /* the draw changes a copy of the model and of its polygon list. the model
     * stays as it is. the list's one strip holds VERTICES vertices from
     * FIRST_VERTEX on, past ARC_HEAD shorts and the strip's length. each vertex
     * takes STRIDE shorts: an index, u and v. NJD_CE ends the list */
    enum { FIRST_VERTEX = ARC_HEAD + 1, VERTICES = 4, STRIDE = 3 };
    Sint16 polygon[FIRST_VERTEX + VERTICES * STRIDE + 1];
    NJS_CNK_MODEL arc = *electric_arc_model;
    int i;

    memcpy(polygon, arc.plist, sizeof(polygon));
    for (i = FIRST_VERTEX + 1; i < FIRST_VERTEX + VERTICES * STRIDE;
         i += STRIDE) {
        polygon[i] =
            electric_pillar_cell_edge(polygon[i]) + column * CELL_WIDTH;
        polygon[i + 1] =
            electric_pillar_cell_edge(polygon[i + 1]) + row * CELL_HEIGHT;
    }
    arc.plist = polygon;
#else
    Sint16 *src;
    Sint16 *saved = src = electric_arc_model->plist;
    Sint16 *dst = ARC_COPY;
    Sint16 count;
    int i;

    for (i = 0; i < ARC_HEAD; ++i) {
        *dst++ = *src++;
    }
    for (count = *dst++ = *src++; count != NJD_CE; count = *dst++ = *src++) {
        for (i = 0; i < njAbs(count); ++i) {
            *dst++ = *src++;
            *dst++ = electric_pillar_cell_edge(*src++) + (column * CELL_WIDTH);
            *dst++ = electric_pillar_cell_edge(*src++) + (row * CELL_HEIGHT);
        }
    }
    /* the model points at the copy until the draw ends */
    electric_arc_model->plist = ARC_COPY;
#endif
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njSetTexture(&electric_pillar_texlist);
    njCnkEasyDrawModel(&electric_pillar_model);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    draw_control_save();
    draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ISA);
    njSetTexture(&electric_arc_texlist);
#ifdef NONMATCHING
    /* this side draws its copy. the matching side puts the model's list back */
    njCnkEasyDrawModel(&arc);
    draw_control_restore();
    njPopMatrixEx();
#else
    njCnkEasyDrawModel(electric_arc_model);
    draw_control_restore();
    njPopMatrixEx();
    electric_arc_model->plist = saved;
#endif
}
