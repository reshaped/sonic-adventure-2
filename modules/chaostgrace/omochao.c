/* the Omochao spectators. each has its own copy of race_omo_model's nodes, and
 * so its own models at its hands. its placement's ang.x chooses its mode, one
 * of the first OMO_PLACED_MODES */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "chao/model.h"
#include "chao/movement.h"
#include "chaostgrace/omo_motion.h"
#include "chaostgrace/props.h"
#include "chaostgrace/race.h"
#include "draw/motion.h"
#include "system/task.h"
#include "system/util.h"

enum omo_mode {
    OMO_BOTH_HANDS,
    OMO_ACCESSORY_A_RIGHT,
    OMO_ACCESSORY_A_LEFT,
    OMO_ACCESSORY_B_RIGHT,
    OMO_ACCESSORY_B_LEFT,
    OMO_STARTER,
    OMO_AWAIT_START,
    OMO_AFTER_START,
    OMO_CHEER
};
enum { OMO_PLACED_MODES = 6 };
/* omo_motions' entries. OMO_READY_MOTION and OMO_GO_MOTION play the two halves
 * of the starter's motion, omo_start, and OMO_GO_MOTION goes on to
 * OMO_STARTER_MOTION */
enum {
    OMO_IDLE_MOTION,
    OMO_RIGHT_MOTION,
    OMO_LEFT_MOTION,
    OMO_STARTER_MOTION,
    OMO_READY_MOTION,
    OMO_GO_MOTION,
    OMO_CHEER_MOTION,
    OMO_MOTIONS
};
/* the hands' nodes, counted depth first from the root */
enum { OMO_LEFT_HAND = 6, OMO_RIGHT_HAND = 8 };
/* an Omochao's cheer starts and ends at a chance of CHEER_CHANCE on a frame
 * CHEER_PERIOD picks. the starter takes OMO_STARTER again more than
 * STARTER_REST frames after the start */
#define CHEER_PERIOD 60
#define CHEER_CHANCE 0.3F
#define STARTER_REST 120
/* the 16 bits of an angle in the placement's 32-bit ang.x */
#define ANGLE_BITS 0xffff
/* an Omochao draws while the box VISIBLE_HALF_SIZE either way of it is in
 * view */
#define VISIBLE_HALF_SIZE 5.0F
#define OMO_SCALE 1.25F
extern NJS_CNK_OBJECT race_omo_accessory_a[], race_omo_accessory_b[];
extern NJS_CNK_OBJECT race_omo_both_hands[], race_omo_starter_accessory[];
static struct motion_table_entry omo_motions[OMO_MOTIONS] = {
#include "chaostgrace/omochao/omo_motions.inc"
};
static struct task *starter = NULL;
/* the nodes attach_at_node and clear_node have passed, counted depth first */
static Uint16 node_index;

static NJS_CNK_OBJECT *copy_nodes(NJS_CNK_OBJECT *source)
{
    NJS_CNK_OBJECT *copy;
    if (!source) {
        return NULL;
    }
    copy = syCalloc(1, sizeof(NJS_CNK_OBJECT));
    if (!copy) {
        return NULL;
    }
    njMemCopy(copy, source, sizeof(NJS_CNK_OBJECT));
    if (source->child) {
        copy->child = copy_nodes(source->child);
    }
    if (source->sibling) {
        copy->sibling = copy_nodes(source->sibling);
    }
    return copy;
}

static void free_nodes(NJS_CNK_OBJECT *node)
{
    if (node) {
        if (node->child) {
            free_nodes(node->child);
            node->child = NULL;
        }
        if (node->sibling) {
            free_nodes(node->sibling);
            node->sibling = NULL;
        }
        syFree(node);
    }
}

static void omo_step(struct task *task)
{
    /* @hack redundant: frame_space, 24 bytes that only size the frame, and
     * its address in frame_address */
    Uint8 frame_space[24];
    void *frame_address = frame_space;
    struct task_work *work = task->work;
    struct motion_control *motion = task->motion.p;
    (void)frame_address;
    /* @hack redundant: three stores under if (0), which never runs */
    if (0) {
        work->mode = OMO_AWAIT_START;
        work->submode = 0;
        work->timer = 6;
    }
    work->ang.x &= ANGLE_BITS;
    switch (work->mode) {
    case OMO_BOTH_HANDS:
    /* @hack redundant: the labels check_idle, check_cheer and check_starter,
     * which no goto names */
    check_idle: /* NOLINT(clang-diagnostic-unused-label) */
        /* @bug logic: starts a cheer on each frame but every CHEER_PERIOD-th,
         * at 0x0c9c3bf6, where OMO_CHEER's test, at 0x0c9c3c90, takes every
         * CHEER_PERIOD-th frame alone */
        if (++work->timer % CHEER_PERIOD &&
            rand() * RAND_SCALE < CHEER_CHANCE) {
            work->mode = OMO_CHEER;
            work->submode = 0;
            motion_link(motion, OMO_CHEER_MOTION);
        }
        break;
    case OMO_CHEER:
    check_cheer: /* NOLINT(clang-diagnostic-unused-label) */
        if (++work->timer % CHEER_PERIOD == 0 &&
            rand() * RAND_SCALE < CHEER_CHANCE) {
            /* @bug logic: keeps OMO_CHEER as it links OMO_IDLE_MOTION, at
             * 0x0c9c3cb0, so an Omochao that has cheered never takes
             * OMO_BOTH_HANDS again */
            work->mode = OMO_CHEER;
            work->submode = 0;
            motion_link(motion, OMO_IDLE_MOTION);
        }
        break;
    case OMO_ACCESSORY_A_LEFT:
        break;
    case OMO_ACCESSORY_A_RIGHT:
        break;
    case OMO_ACCESSORY_B_LEFT:
        break;
    case OMO_ACCESSORY_B_RIGHT:
        break;
    case OMO_STARTER:
        if (race_mode() == RACE_READY) {
            work->mode = OMO_AWAIT_START;
            work->submode = 0;
            motion_link(motion, OMO_READY_MOTION);
        }
        break;
    case OMO_AWAIT_START:
        if (race_mode() != RACE_READY) {
            work->mode = OMO_AFTER_START;
            work->submode = 0;
            motion_link(motion, OMO_GO_MOTION);
            work->timer = 0;
        }
        break;
    case OMO_AFTER_START:
    check_starter: /* NOLINT(clang-diagnostic-unused-label) */
        if (work->timer++ > STARTER_REST) {
            work->mode = OMO_STARTER;
            work->submode = 0;
            motion_link(motion, OMO_STARTER_MOTION);
        }
        break;
    }
    motion_step(motion);
}

static void omo_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct motion_control *motion = task->motion.p;
    if (garden_visible_box(task, VISIBLE_HALF_SIZE, VISIBLE_HALF_SIZE,
                           VISIBLE_HALF_SIZE)) {
        njSetTexture(&chao_body_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njScale(NULL, OMO_SCALE, OMO_SCALE, OMO_SCALE);
        motion_draw(task->extra.p, motion);
        njPopMatrixEx();
    }
}

static void attach_at_node(NJS_CNK_OBJECT *node, Uint16 index,
                           NJS_CNK_MODEL *model)
{
    if (node_index == index && !node->model) {
        node->model = model;
        node->evalflags &= ~NJD_EVAL_HIDE;
    }
    node_index++;
    if (node->child) {
        attach_at_node(node->child, index, model);
    }
    if (node->sibling) {
        attach_at_node(node->sibling, index, model);
    }
}

void race_omo_attach(NJS_CNK_OBJECT *root, Uint16 index, NJS_CNK_MODEL *model)
{
    node_index = 0;
    attach_at_node(root, index, model);
}

static void clear_node(NJS_CNK_OBJECT *node, Uint16 index)
{
    if (node_index == index && node->model) {
        node->model = NULL;
        node->evalflags |= NJD_EVAL_HIDE;
    }
    node_index++;
    if (node->child) {
        clear_node(node->child, index);
    }
    if (node->sibling) {
        clear_node(node->sibling, index);
    }
}

void race_omo_detach(NJS_CNK_OBJECT *root, Uint16 index)
{
    node_index = 0;
    clear_node(root, index);
}

static void omo_destroy(struct task *task)
{
    struct task_work *work = task->work;
    race_omo_detach(task->extra.p, OMO_LEFT_HAND);
    race_omo_detach(task->extra.p, OMO_RIGHT_HAND);
    free_nodes(task->extra.p);
    task->extra.p = NULL;
    /* @hack redundant: a test of work->mode & 0, which never holds, and the
     * clear of starter under it */
    if (work->mode & 0) {
        starter = NULL;
    }
}

void race_omochao_init(struct task *task)
{
    struct task_work *work = task->work;
    struct motion_control *motion;
    NJS_CNK_OBJECT *root;
#ifdef NONMATCHING
    /* a build without the disc's data has zeros in omo_motions. motion_link
     * would divide by their link_frames, so the Omochao removes itself */
    if (omo_motions[0].link_frames == 0) {
        task_kill(task);
        return;
    }
#endif
    task->step = omo_step;
    task->draw = omo_draw;
    task->destructor = omo_destroy;
    task->motion.p = syCalloc(1, sizeof(struct motion_control));
    if (work->ang.x < 0) {
        work->mode =
            (10 - ((-work->ang.x) % OMO_PLACED_MODES)) % OMO_PLACED_MODES;
        work->submode = 0;
    } else {
        work->mode = work->ang.x % OMO_PLACED_MODES;
        work->submode = 0;
    }
    motion_init(task->motion.p, omo_motions);
    motion = task->motion.p;
    root = copy_nodes(&race_omo_model);
    task->extra.p = root;
    switch (work->mode) {
    case OMO_BOTH_HANDS:
    default:
        race_omo_attach(root, OMO_LEFT_HAND, race_omo_both_hands->model);
        race_omo_attach(root, OMO_RIGHT_HAND, race_omo_both_hands->model);
        motion_link(motion, OMO_IDLE_MOTION);
        break;
    case OMO_ACCESSORY_A_LEFT:
        race_omo_attach(root, OMO_LEFT_HAND, race_omo_accessory_a->model);
        motion_link(motion, OMO_LEFT_MOTION);
        break;
    case OMO_ACCESSORY_A_RIGHT:
        race_omo_attach(root, OMO_RIGHT_HAND, race_omo_accessory_a->model);
        motion_link(motion, OMO_RIGHT_MOTION);
        break;
    case OMO_ACCESSORY_B_LEFT:
        race_omo_attach(root, OMO_LEFT_HAND, race_omo_accessory_b->model);
        motion_link(motion, OMO_LEFT_MOTION);
        break;
    case OMO_ACCESSORY_B_RIGHT:
        race_omo_attach(root, OMO_RIGHT_HAND, race_omo_accessory_b->model);
        motion_link(motion, OMO_RIGHT_MOTION);
        break;
    case OMO_STARTER:
        race_omo_attach(root, OMO_LEFT_HAND, race_omo_starter_accessory->model);
        starter = task;
        motion_link(motion, OMO_STARTER_MOTION);
        break;
    }
}

/* the captions the placement editor gives the placed modes */
typedef const char *omo_mode_caption;
struct omo_mode_names {
    char cheer[8], left[8], right[8], batu_left[12], batu_right[12], start[8];
};
static const struct omo_mode_names mode_names;
static const omo_mode_caption omo_editor_modes[OMO_PLACED_MODES] = {
#include "chaostgrace/omochao/omo_editor_modes.inc"
};
/* the labels the placement editor gives an Omochao's fields */
struct omochao_editor_labels {
    char motion[20], yaw[20], angle_z[12];
};
static const struct omochao_editor_labels omo_editor_labels = {
#include "chaostgrace/omochao/omo_editor_labels.inc"
};
static const struct omo_mode_names mode_names = {
#include "chaostgrace/omochao/mode_names.inc"
};
