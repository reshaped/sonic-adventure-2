/* the board, BOARD in Metal Harbor's object list: a board a player rides */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "stg10/board.h"
#include "stg10/board/text.inc"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

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
    /* the player's anim.current and anim.frame, as the last step whose
     * player's anim.mode was above PLAYER_ANIM_BLEND left them. while the
     * player blends into another animation, the draw blends the board's motion
     * from this one */
    Sint32 anim;
    Float frame;
};

/* the matrix the board's shadow draws under */
static NJS_MATRIX harbor_board_shadow_matrix;

static void board_step(struct task *task);
static void board_draw(struct task *task);
static void board_destroy(struct task *task);

/* The escape board and the modifier volume drawn beneath it. */

struct harbor_board_shadow_vertices {
    Sint32 head[2];
    NJS_POINT3 vertices[11];
    Sint32 end;
};

static NJS_TEXNAME harbor_board_textures[1] = {
#include "stg10/board/harbor_board_textures.inc"
};

static NJS_TEXLIST harbor_board_texlist = {
#include "stg10/board/harbor_board_texlist.inc"
};

static Sint16 harbor_board_deck_polygons[239] = {
#include "stg10/board/harbor_board_deck_polygons.inc"
};

static Sint32 harbor_board_deck_vertices[231] = {
#include "stg10/board/harbor_board_deck_vertices.inc"
};

static NJS_CNK_MODEL harbor_board_deck_mesh = {
#include "stg10/board/harbor_board_deck_mesh.inc"
};

static NJS_CNK_OBJECT harbor_board_deck = {
#include "stg10/board/harbor_board_deck.inc"
};

static NJS_CNK_OBJECT harbor_board_root = {
#include "stg10/board/harbor_board_root.inc"
};

static Sint16 harbor_board_shadow_polygons[59] = {
#include "stg10/board/harbor_board_shadow_polygons.inc"
};

static struct harbor_board_shadow_vertices harbor_board_shadow_vertices = {
#include "stg10/board/harbor_board_shadow_vertices.inc"
};

static NJS_CNK_MODEL harbor_board_shadow_mesh = {
#include "stg10/board/harbor_board_shadow_mesh.inc"
};

static NJS_CNK_OBJECT harbor_board_shadow_object = {
#include "stg10/board/harbor_board_shadow_object.inc"
};

NJS_TEXLIST *harbor_board_texlists[2] = {
#include "stg10/board/harbor_board_texlists.inc"
};

void harbor_board_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for its SET editor
     * caption, which SHC still places in this source's constants */
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
 * harbor_board_shadow_matrix ends as the last node's matrix */
static void board_keep_matrix(NJS_CNK_OBJECT *object)
{
    njGetMatrix(&harbor_board_shadow_matrix);
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
    njSetTexture(&harbor_board_texlist);
    njPushMatrixEx();
    object = &harbor_board_root;
    if (work->mode != MODE_RIDDEN) {
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njCnkSimpleDrawObject(object);
        njRotateEx(object->ang, 0);
        njGetMatrix(&harbor_board_shadow_matrix);
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
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
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
                njRotateEx(object->ang, 0);
                njGetMatrix(&harbor_board_shadow_matrix);
            }
        }
    }
    draw_control_set(DRAW_CONTROL_SHADOW);
    njSetMatrix(NULL, &harbor_board_shadow_matrix);
    njCnkModDrawObject(&harbor_board_shadow_object);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    njPopMatrixEx();
    njCnkSetMotionCallback(NULL);
}

static void board_destroy(struct task *task)
{
}
