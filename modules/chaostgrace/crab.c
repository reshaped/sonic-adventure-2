/* a crab that walks sideways. it goes as far as its placement's scale.x to its
 * left and scale.y to its right */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "chaostgrace/crab_art.h"
#include "chaostgrace/props.h"
#include "collision/collision.h"
#include "draw/motion.h"
#include "object/set.h"
#include "system/task.h"
#include "system/util.h"

enum crab_mode { CRAB_STILL, CRAB_RIGHT, CRAB_LEFT, CRAB_WAIT_MOTION };
enum { CRAB_IDLE_MOTION, CRAB_WALK_MOTION };
static struct motion_table_entry crab_motions[3] = {
#include "chaostgrace/crab/crab_motions.inc"
};
/* clang-format off */
static const struct collision_shape crab_shape = {
    (Sint8)197, SHAPE_SPHERE,
    PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK | SHAPE_GUARD, SHAPE_UNTURNED | SHAPE_IN_WORLD,
    {0.0F, 0.0F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}};
/* clang-format on */

/* a still crab starts to walk at a chance of START_CHANCE every
 * START_PERIOD frames, right or left alike. a walking one stops at a chance
 * of STOP_CHANCE every STOP_PERIOD frames */
#define START_PERIOD 60
#define START_CHANCE 0.3F
#define STOP_PERIOD 45
#define STOP_CHANCE 0.3F
#define WALK_SPEED 0.25F
#define LEFT_LIMIT(work) ((work)->scl.x)
#define RIGHT_LIMIT(work) ((work)->scl.y)
/* how far right of its placement the crab stands */
#define CRAB_OFFSET(work) ((work)->scl.z)

#define CRAB_STOP(work, motion)                                                \
    do {                                                                       \
        (work)->mode = CRAB_STILL;                                             \
        (work)->submode = 0;                                                   \
        motion_link((motion), CRAB_IDLE_MOTION);                               \
    } while (0)

static void crab_step(struct task *task)
{
    struct task_work *work = task->work;
    struct motion_control *motion = task->motion.p;
    NJS_POINT3 offset;
    if (!placement_try_leave(task)) {
        switch (work->mode) {
        case CRAB_STILL:
            if (++work->timer % START_PERIOD == 0) {
                if (rand() * RAND_SCALE < START_CHANCE) {
                    if (rand() * RAND_SCALE < 0.5F) {
                        work->mode = CRAB_RIGHT;
                        work->submode = 0;
                    } else {
                        work->mode = CRAB_LEFT;
                        work->submode = 0;
                    }
                    motion_link(motion, CRAB_WALK_MOTION);
                    work->timer = 0;
                }
            }
            break;
        case CRAB_RIGHT:
            CRAB_OFFSET(work) += WALK_SPEED;
            if (CRAB_OFFSET(work) > RIGHT_LIMIT(work)) {
                CRAB_OFFSET(work) = RIGHT_LIMIT(work);
                CRAB_STOP(work, motion);
            }
            if (++work->timer % STOP_PERIOD == 0 &&
                rand() * RAND_SCALE < STOP_CHANCE) {
                CRAB_STOP(work, motion);
            }
            break;
        case CRAB_LEFT:
            CRAB_OFFSET(work) -= WALK_SPEED;
            if (CRAB_OFFSET(work) < -LEFT_LIMIT(work)) {
                CRAB_OFFSET(work) = -LEFT_LIMIT(work);
                CRAB_STOP(work, motion);
            }
            if (++work->timer % STOP_PERIOD == 0 &&
                rand() * RAND_SCALE < STOP_CHANCE) {
                CRAB_STOP(work, motion);
            }
            break;
        case CRAB_WAIT_MOTION:
            if (motion_held(motion)) {
                CRAB_STOP(work, motion);
            }
            break;
        }
        /* @hack redundant: a second switch on the mode, whose cases do
         * nothing */
        switch (work->mode) {
        case CRAB_STILL:
            break;
        case CRAB_RIGHT:
            break;
        case CRAB_LEFT:
            break;
        }
        offset.x = CRAB_OFFSET(work);
        offset.y = 0.0F;
        offset.z = 0.0F;
        njPushMatrix(&_nj_unit_matrix_);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njCalcPoint(NULL, &offset, &offset);
        njPopMatrixEx();
        collision_set_shape_center(task, 0, &offset);
        motion_step(motion);
        collision_register(task);
    }
}

static void crab_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct motion_control *motion = task->motion.p;
    /* @hack redundant: frame_space, 12 bytes that only size the frame */
    Uint8 frame_space[12];
    (void)frame_space;
    njSetTexture(&race_fruit_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njTranslate(NULL, CRAB_OFFSET(work), 0.0F, 0.0F);
    motion_draw(&race_crab_model, motion);
    njPopMatrixEx();
    /* @hack redundant: a matrix push and a copy of the scales under if (0),
     * which never runs */
    if (0) {
        Float left = LEFT_LIMIT(work);
        Float right = RIGHT_LIMIT(work);
        njPushMatrixEx();
        LEFT_LIMIT(work) = left;
        RIGHT_LIMIT(work) = right;
    }
}

static void crab_destroy(struct task *task)
{
}

void race_crab_init(struct task *task)
{
#ifdef NONMATCHING
    /* a build without the disc's data has zeros in crab_motions. motion_link
     * would divide by their link_frames, so the crab removes itself */
    if (crab_motions[0].link_frames == 0) {
        task_kill(task);
        return;
    }
#endif
    task->step = crab_step;
    task->draw = crab_draw;
    task->destructor = crab_destroy;
    task->motion.p = syCalloc(1, sizeof(struct motion_control));
    motion_init(task->motion.p, crab_motions);
    motion_link(task->motion.p, CRAB_IDLE_MOTION);
    collision_init(task, &crab_shape, 1, COLLISION_OBJECT);
}

/* the labels the placement editor gives a crab's fields */
struct crab_editor_labels {
    char angle_x[12], yaw[20], increment[16];
    char left_limit[24], right_limit[24], scale_z[12];
};
static const struct crab_editor_labels crab_editor_labels = {
#include "chaostgrace/crab/crab_editor_labels.inc"
};
