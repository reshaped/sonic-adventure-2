/* the technique link, LINKLINK in a stage's object list: an unseen volume,
 * one numbered link of a row. a player who touches a row's links in order
 * earns the bonus of the last link touched */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "hud/hud.h"
#include "object/set.h"
#include "shared/technique_link.h"
#include "system/task.h"

/* the bit of ang.z that picks a link's shape. LINK_BOX and LINK_SPHERE are
 * the shapes' indexes in technique_link_shapes */
#define LINK_SHAPE_BIT 0x1
#define LINK_BOX 0
#define LINK_SPHERE 1
#define LINK_SHAPES 2

/* the fields of ang.y: a link's number in its row, and its bonus. a bonus
 * past LAST_BONUS counts as 0 */
#define NUMBER_BITS 0xf0
#define NUMBER_SHIFT 4
#define BONUS_BITS 0xf
#define LAST_BONUS 10
/* the low bytes of ang.y and ang.z, which hold a link's settings until
 * technique_link_init clears them */
#define SETTINGS_BITS 0xff

/* the work's fields that keep a link's number, bonus and time. the time
 * counts down while the link is its row's latest */
#define LINK_NUMBER(work) ((work)->submode)
#define LINK_BONUS(work) ((work)->tick)
#define LINK_TIME(work) ((work)->timer)

/* technique_link_latest's value while no row runs */
#define NO_LINK (-1)
/* the placement's state once its link has ended */
#define STATE_ENDED 1

/* a link waits for the touch that extends its row, then is the row's
 * latest */
enum technique_link_mode { MODE_WAIT, MODE_LATEST };

static void technique_link_draw(struct task *task);
static void technique_link_destroy(struct task *task);
static void technique_link_step(struct task *task);

/* the checks pass a link over against items, enemies, objects and pickups */
#define LINK_FLAGS                                                             \
    (SHAPE_IGNORE_ENEMIES | SHAPE_IGNORE_OBJECTS | SHAPE_IGNORE_PICKUPS |      \
     SHAPE_UNK_2000)
/* a link's shapes, a box and a sphere. technique_link_init sizes its
 * record's copy of one by scl */
/* clang-format off */
static struct collision_shape technique_link_shapes[LINK_SHAPES] = {
    {0, SHAPE_BOX2, PUSH_FIRM(PUSH_ALL), 0, LINK_FLAGS, {0.0F, 0.0F, 0.0F},
     1.0F, 1.0F, 1.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL), 0, LINK_FLAGS, {0.0F, 0.0F, 0.0F},
     1.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
/* the points each bonus gives through technique_score_start */
static Sint16 technique_link_bonuses[LAST_BONUS + 1] = {
    0, 100, 200, 300, 400, 500, 600, 800, 1000, 1500, 2000,
};
/* the number of the running row's latest link, or NO_LINK */
static Sint8 technique_link_latest = NO_LINK;
/* each shape's name */
static char *technique_link_shape_names[LINK_SHAPES] = {"CUBE", "SPHERE"};
/* the debug_models entry that shows each shape */
static Sint8 technique_link_models[LINK_SHAPES] = {DEBUG_MODEL_BOX,
                                                   DEBUG_MODEL_SPHERE};

void technique_link_init(struct task *task)
{
    struct task_work *work = task->work;
    int shape;

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack spelling: the shape picked by an if and an else, where the masked
     * bit would do */
    if (work->ang.z & LINK_SHAPE_BIT) {
        shape = LINK_SPHERE;
    } else {
        shape = LINK_BOX;
    }
    LINK_TIME(work) = work->ang.x;
    work->ang.x = 0;
    LINK_NUMBER(work) = (work->ang.y & NUMBER_BITS) >> NUMBER_SHIFT;
    LINK_BONUS(work) = work->ang.y & BONUS_BITS;
    if (LINK_BONUS(work) > LAST_BONUS) {
        LINK_BONUS(work) = 0;
    }
    work->ang.y &= ~SETTINGS_BITS;
    /* @hack spelling: the mask written ahead of ang.z, where &= would do */
    work->ang.z = ~SETTINGS_BITS & work->ang.z;
    collision_init(task, &technique_link_shapes[shape], 1, COLLISION_OBJECT);
    work->collision->shapes->radius = work->scl.x;
    work->collision->shapes->size_y = work->scl.y;
    work->collision->shapes->size_z = work->scl.z;
    if (shape == LINK_BOX) {
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_record_radius(work->collision);
    task->step = technique_link_step;
    task->draw = technique_link_draw;
    task->destructor = technique_link_destroy;
}

static void technique_link_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    NJS_POINT3 scale;

    /* @hack redundant: statements behind a return, kept for the strings SHC
     * places in the object, and for the registers and frame SHC sets up for
     * their code. the image holds none of their code, and the calls after
     * the strings stand in for the original's */
    if (1) {
        return;
    }
    (void)"TIME:";
    (void)"<- TIME:";
    (void)"FRAMES";
    (void)"<- ANGY ID ";
    (void)" TP ";
    (void)"<- ANGZ & SHAPE:CUBE";
    (void)"<- SCLX";
    (void)"<- SCLY";
    (void)"<- SCLZ";
    (void)"<- ID ";
    (void)"<- SHAPE:SPHERE";
    (void)"<- SCL";
    pos = work->pos;
    scale = work->scl;
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njPopMatrixEx();
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njScaleEx(&scale);
    njPopMatrixEx();
}

static void technique_link_destroy(struct task *task)
{
    /* @hack spelling: one test inside the other, where && would do */
    if (task->work->mode == MODE_LATEST) {
        if (technique_link_latest == LINK_NUMBER(task->work)) {
            technique_link_latest = NO_LINK;
        }
    }
}

static void technique_link_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: set_state, a local for placement_set_state, where the
     * function called by name would do */
    void (*set_state)(struct task *task, Sint8 state);

    /* @hack spelling: set_state set in an else after the return, where a
     * statement after the if would do */
    if (work->mode != MODE_LATEST && placement_try_leave(task)) {
        return;
    } else {
        set_state = placement_set_state;
    }
    if (placement_get_state(task)) {
        return;
    }
    if (work->mode == MODE_WAIT) {
        /* @hack spelling: one test inside the other, where && would do */
        if (collision_next_player(task) != NULL) {
            if (technique_link_latest == LINK_NUMBER(work) - 1) {
                technique_link_latest = LINK_NUMBER(work);
                work->mode = MODE_LATEST;
            }
        }
    } else if (work->mode == MODE_LATEST) {
        if (technique_link_latest != LINK_NUMBER(work)) {
            task_kill(task);
            set_state(task, STATE_ENDED);
            return;
        }
        /* @hack spelling: the unsigned time tested as at most 0, where == 0
         * would do */
        if (LINK_TIME(work)-- <= 0) {
            task_kill(task);
            set_state(task, STATE_ENDED);
            technique_link_latest = NO_LINK;
            if (technique_link_bonuses[LINK_BONUS(work)] != 0) {
                technique_score_start(technique_link_bonuses[LINK_BONUS(work)]);
            }
            return;
        }
    }
    collision_register(task);
}
