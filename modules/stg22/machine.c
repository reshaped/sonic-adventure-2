#include "stg22/fixture_models.h"
#include <sg_xpt.h>
#include <stdlib.h>
#include <ninja.h>
#include <shinobi.h>

#include "object/set.h"
#include "stg22/crazy_gadget.h"
#include "stg22/prop_textures.h"
#include "stg22/scenery.h"
#include "system/cache.h"
#include "system/task.h"
#include "system/util.h"

/* each piston's extension runs from 0 to PISTON_TRAVEL. the first
 * PISTONS_PER_SIDE pistons stand PISTON_MOUNT plus their extension along the
 * machine's +z. the rest stand as far along its -z */
#define PISTONS 6
#define PISTONS_PER_SIDE 3
#define PISTON_TRAVEL 20.0F
#define PISTON_MOUNT 25.0F

/* the pistons' record, at task->motion. bit i of the work's submode is set
 * while piston i extends */
struct piston_motion {
    Float speed[PISTONS];
    Float extension[PISTONS];
};
#define PISTON_MOTION(task) ((struct piston_motion *)(task)->motion.p)

NJS_TEXLIST *gadget_machine_texlists[] = {&gadget_machine_texlist, NULL};
/* a piston starts extending where a draw from 0 to 1 passes EXTEND_ODDS. it
 * starts at one of START_PLACES places: 0, half or all of PISTON_TRAVEL. the
 * place is a whole number drawn below START_DRAW, mod START_PLACES. a
 * piston's speed is PISTON_TRAVEL / 2 / steps_per_stroke a step, times a
 * factor. the init draws the factor from SPEED_LOW up to SPEED_LOW + 1 */
#define EXTEND_ODDS 0.5F
#define START_PLACES 3
#define START_DRAW 103.0F
#define SPEED_LOW 0.5F
static int steps_per_stroke = 10;

static void machine_destroy(struct task *owner);
static void machine_step(struct task *task);
static void machine_draw(struct task *task);

void gadget_machine_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct piston_motion));
    if (!task->motion.p) {
        return;
    }
    task->draw = machine_draw;
    task->step = machine_step;
    task->destructor = machine_destroy;
    work->submode = 0;
    for (i = 0; i < PISTONS; i++) {
        if (RAND_SCALE * rand() > EXTEND_ODDS) {
            work->submode |= 1 << i;
        }
        PISTON_MOTION(task)->extension[i] =
            (int)((START_DRAW * RAND_SCALE) * rand()) % START_PLACES *
            (PISTON_TRAVEL / 2);
        PISTON_MOTION(task)->speed[i] = (PISTON_TRAVEL / 2) / steps_per_stroke *
                                        ((RAND_SCALE * rand()) + SPEED_LOW);
    }
}

static void machine_destroy(struct task *owner)
{
    /* @hack spelling: task, a copy of owner, where the parameter would do */
    struct task *task = owner;

    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void machine_step(struct task *task)
{
    struct task_work *work = task->work;
    int i;
#ifdef NONMATCHING
#else
    /* @hack redundant: motion, a copy of the record that only blocks no path
     * reaches use */
    struct piston_motion *motion;

    /* @hack redundant: a store of the record to motion, in a block no path
     * reaches */
    if (0) {
        motion = PISTON_MOTION(task);
    }
#endif

    if (!placement_try_leave(task)) {
#ifdef NONMATCHING
#else
        /* @hack redundant: a test of motion, in a block no path reaches */
        if (0) {
            /* NOLINTNEXTLINE(misc-redundant-expression) */
            if (motion || motion) {
                (void)0;
            }
        }
#endif
        for (i = 0; i < PISTONS; i++) {
            if (work->submode & (1 << i)) {
                PISTON_MOTION(task)->extension[i] +=
                    PISTON_MOTION(task)->speed[i];
                if (PISTON_MOTION(task)->extension[i] > PISTON_TRAVEL) {
                    PISTON_MOTION(task)->extension[i] = PISTON_TRAVEL;
                    work->submode &= ~(1 << i);
                }
            } else {
                PISTON_MOTION(task)->extension[i] -=
                    PISTON_MOTION(task)->speed[i];
                if (PISTON_MOTION(task)->extension[i] < 0.0F) {
                    PISTON_MOTION(task)->extension[i] = 0.0F;
                    work->submode |= 1 << i;
                }
            }
        }
    }
    /* @hack redundant: the set editor's labels, in a block no path reaches */
    if (0) {
        (void)"<- ROT X";
        (void)"<- ROT Y";
        (void)"<- ROT Z";
    }
}

static void machine_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *part;
    int i;

    if (gadget_visible_from_camera(&work->pos)) {
        part = &gadget_machine_object;
        njSetTexture(&gadget_machine_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateEx(&work->ang.x, 0);
        njCnkEasyDrawModel(part->model);
        part = part->child;
        njPushMatrixEx();
        for (i = 0; i < PISTONS && part; i++, part = part->sibling) {
            if (i >= PISTONS_PER_SIDE) {
                njTranslate(NULL, part->pos[0], part->pos[1],
                            -PISTON_MOTION(task)->extension[i] - PISTON_MOUNT);
            } else {
                njTranslate(NULL, part->pos[0], part->pos[1],
                            PISTON_MOTION(task)->extension[i] + PISTON_MOUNT);
            }
            njCnkEasyDrawModel(part->model);
            matrix_load_parent();
        }
        njPopMatrix(2);
    }
}
