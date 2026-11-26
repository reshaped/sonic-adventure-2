/* the board, BOARD in City Escape's object list: a board a player rides */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "stg13/board.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "stg13/board/text.inc"

/* the board's modes, in its work's mode */
enum board_mode {
    MODE_WAIT, /* the board stands where it was placed */
    MODE_RIDDEN, /* its player plays a board animation */
    MODE_FALL /* its player has stopped riding, and the board falls */
};

/* the player's board animations, ANIM_BOARD_FIRST to ANIM_BOARD_LAST. the
 * board's own motion for animation n lies in
 * motion_table[n + BOARD_MOTION_OFFSET] */
#define ANIM_BOARD_FIRST 121
#define ANIM_BOARD_LAST 135
#define BOARD_MOTION_OFFSET 15
/* the slot of the board's player, 0 or 1, in its work's submode. the
 * SLOT_BIT of ang.x picks it */
#define SLOT(work) ((work)->submode)
#define SLOT_BIT 0x1

/* the board's view of its TASK_MOTION record. spd and acc lie where struct
 * task_motion has them, which fall_step_small reads. anim and frame take the
 * first 8 bytes of task_motion's unk_18 */
struct board_motion {
    NJS_VECTOR spd;
    NJS_VECTOR acc;
    /* the player's anim.current and anim.frame, as board_step most recently
     * copied them. it copies them only while the player plays a board animation
     * with anim.mode above PLAYER_ANIM_BLEND. while the player blends into
     * another board animation, the draw blends the board's motion from this one
     */
    Sint32 anim;
    Float frame;
};

/* the matrix the board's shadow draws under */
static NJS_MATRIX board_shadow_matrix;

static void board_step(struct task *task);
static void board_draw(struct task *task);
static void board_destroy(struct task *task);

static NJS_TEXNAME board_texnames[3] = {
#include "stg13/board/board_texnames.inc"
};
static NJS_TEXLIST board_texlist = {
#include "stg13/board/board_texlist.inc"
};
static Sint16 board_polygons[281] = {
#include "stg13/board/board_polygons.inc"
};
static Sint32 board_vertices[299] = {
#include "stg13/board/board_vertices.inc"
};
static NJS_CNK_MODEL board_model = {
#include "stg13/board/board_model.inc"
};
static NJS_CNK_OBJECT board_object = {
#include "stg13/board/board_object.inc"
};
/* board_object's parent, which turns it a quarter turn about y */
static NJS_CNK_OBJECT board_root = {
#include "stg13/board/board_root.inc"
};
/* the board's shadow, a modifier volume */
/* clang-format off */
static Sint16 board_shadow_polygons[59] = {
#include "stg13/board/board_shadow_polygons.inc"
};
/* clang-format on */
struct board_shadow_vertex_list {
    Sint32 head[2];
    NJS_POINT3 points[11];
    Sint32 end;
};
static struct board_shadow_vertex_list board_shadow_vertices = {
#include "stg13/board/board_shadow_vertices.inc"
};
static NJS_CNK_MODEL board_shadow_model = {
#include "stg13/board/board_shadow_model.inc"
};
static NJS_CNK_OBJECT board_shadow_object = {
#include "stg13/board/board_shadow_object.inc"
};
NJS_TEXLIST *board_texlists[2] = {
#include "stg13/board/board_texlists.inc"
};

void board_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its string, which
     * SHC still places in this source's constants */
    if (0) {
        (void)"-> PLAYER";
    }
    SLOT(work) = work->ang.x & SLOT_BIT;
    /* @hack spelling: the draw set last, where struct task's order would set
     * it before the destructor */
    task->step = board_step;
    task->destructor = board_destroy;
    task->draw = board_draw;
}

static void board_step(struct task *task)
{
    struct task_work *work = task->work;
    struct board_motion *motion = task->motion.p;
    struct task_work *player_task_work = player_work[SLOT(work)];
    struct player_motion *player_spd = player_motion[SLOT(work)];
    struct player *player = player_records[SLOT(work)];
    int anim;
    Float *frame;

    if (player_task_work == NULL) {
        return;
    }
    anim = player->anim.current;
    if (anim < ANIM_BOARD_FIRST || anim > ANIM_BOARD_LAST) {
        if (work->mode == MODE_RIDDEN) {
            work->mode = MODE_FALL;
        }
        if (work->mode == MODE_FALL) {
            fall_step_small(task);
            placement_try_leave(task);
        }
    } else {
        work->pos = player_task_work->pos;
        work->ang.x = player_task_work->ang.x;
        work->ang.y = HALF_TURN - player_task_work->ang.y;
        work->ang.z = player_task_work->ang.z;
        motion->spd = player_spd->spd;
        work->mode = MODE_RIDDEN;
        if (player->anim.mode > PLAYER_ANIM_BLEND) {
            motion->anim = player->anim.current;
            /* @hack spelling: frame, a pointer to the record's frame, where
             * motion->frame stored in place would do */
            frame = &motion->frame;
            *frame = player->anim.frame;
        }
    }
}

/* njCnkSetMotionCallback's callback, which a motion draw calls at each node.
 * board_shadow_matrix ends as the last node's matrix */
static void board_keep_matrix(NJS_CNK_OBJECT *object)
{
    njGetMatrix(&board_shadow_matrix);
}

static void board_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct board_motion *motion = task->motion.p;
    struct task_work *player_task_work = player_work[SLOT(work)];
    struct player *player = player_records[SLOT(work)];
    NJS_CNK_OBJECT *object;
    int last;
    int anim;
    NJS_MOTION *board_motion;
    NJS_CNK_OBJECT *model;
    NJS_MOTION_LINK link;
    Float *frame;
    NJS_MOTION *from;

    if (player_task_work == NULL) {
        return;
    }
    anim = player->anim.current;
    njCnkSetMotionCallback(board_keep_matrix);
    njSetTexture(&board_texlist);
    njPushMatrixEx();
    object = &board_root;
    if (work->mode != MODE_RIDDEN) {
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y);
        njCnkSimpleDrawObject(object);
        njRotateEx(object->ang, XYZ_TURN);
        njGetMatrix(&board_shadow_matrix);
    } else {
        /* @hack spelling: last set in two statements, where last =
         * motion->anim + BOARD_MOTION_OFFSET would do */
        last = motion->anim;
        last += BOARD_MOTION_OFFSET;
        anim += BOARD_MOTION_OFFSET;
        board_motion = motion_table[anim].motion;
        if (board_motion != NULL) {
            njTranslateEx(&work->pos);
            njRotateZ(NULL, work->ang.z);
            njRotateX(NULL, work->ang.x);
            njRotateY(NULL, work->ang.y);
            /* @hack spelling: model, a copy of object chosen by a conditional
             * whose dead arm reads the player's anim twice, where object
             * passed in place would do */
            model = 1.0F ? object
                         : (player->anim.mode, player->anim.current, object);
            if (player->anim.mode != PLAYER_ANIM_BLEND) {
                njCnkSimpleDrawMotion(model, board_motion, player->anim.frame);
            } else if (last >= ANIM_BOARD_FIRST + BOARD_MOTION_OFFSET &&
                       last <= ANIM_BOARD_LAST + BOARD_MOTION_OFFSET) {
                /* @hack spelling: frame, a pointer to the record's frame taken
                 * before from is read, where motion->frame read in place would
                 * do */
                frame = &motion->frame;
                from = motion_table[last].motion;
                link.motion[0] = from;
                link.motion[1] = board_motion;
                link.frame[0] = *frame;
                link.frame[1] = 0.0F;
                njCnkSimpleDrawMotionLink(object, &link, player->anim.frame);
            } else {
                njCnkSimpleDrawObject(model);
                njRotateEx(object->ang, XYZ_TURN);
                njGetMatrix(&board_shadow_matrix);
            }
        }
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetMatrix(NULL, &board_shadow_matrix);
    njCnkModDrawObject(&board_shadow_object);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
    njCnkSetMotionCallback(NULL);
}

static void board_destroy(struct task *task)
{
}
