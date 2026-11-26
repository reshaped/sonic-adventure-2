#include <ninja.h>

#include "event/scene.h"
#include "event/tails.h"
#include "system/util.h"

#define TAILS 2
#define BEND_ANGLE 0x3000
#define BEND_STEP 0x700
/* the jiggle's param, which JIGGLE_BEND doesn't read */
#define BEND_PARAM 0.4F

/* @hack type: the two pointers as the one row of a two-dimensional array,
 * where struct jiggle *tails[TAILS] would do */
static struct jiggle *tails[1][TAILS];

static void make_tails(void);
static void free_tails(void);
static void move_tails(void);

void event_tails_start(void)
{
    if (event_scene_file->tails[0]) {
        make_tails();
    }
}

void event_tails_stop(void)
{
    if (event_scene_file->tails[0]) {
        free_tails();
    }
}

void event_tails_step(void)
{
    if (event_scene_file->tails[0]) {
        move_tails();
    }
}

static void make_tails(void)
{
    struct jiggle *tail;
    int i;

    tails[0][0] = jiggle_create(event_scene_file->tails[0]->child);
    tails[0][1] = jiggle_create(event_scene_file->tails[0]->child->sibling);
    for (i = 0; i < TAILS; i++) {
        tail = tails[0][i];
        tail->kind = JIGGLE_BEND;
        tail->param = BEND_PARAM;
        tail->by_kind.angle = BEND_ANGLE;
        tail->phase_step = BEND_STEP;
        tail->shape_a = NULL;
        tail->copy->evalflags &= ~NJD_EVAL_UNIT_ANG;
    }
    event_scene_file->tails[0]->child = tails[0][0]->copy;
    event_scene_file->tails[0]->child->sibling = tails[0][1]->copy;
    tails[0][1]->copy->scl[0] = -1.0F;
    tails[0][1]->copy->scl[2] = -1.0F;
    tails[0][1]->copy->evalflags &= ~NJD_EVAL_UNIT_SCL;
}

static void free_tails(void)
{
    jiggle_free(tails[0][0]);
    jiggle_free(tails[0][1]);
}

static void move_tails(void)
{
    jiggle_step(tails[0][0]);
    jiggle_step(tails[0][1]);
}
