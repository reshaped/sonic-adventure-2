/* the lobby's exit from the Chao world */
#include <ninja.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "chao/garden.h"
#include "chaostglobby/gate.h"
#include "chaostglobby/gate_art.h"
#include "chaostglobby/lobby.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "player/player.h"
#include "stage/stage.h"
#include "system/math.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

enum { EXIT_INIT = 0, EXIT_WAIT = 1, EXIT_CHOICE = 3, EXIT_LEAVING = 4 };
/* timer counts the exit's first START_STEPS steps, which it spends in
 * EXIT_INIT. it then holds the answer selected, an enum lobby_answer */
#define START_STEPS 6

/* the exit's first two shapes, spheres about its middle. it asks the player
 * who touches INNER_SHAPE */
#define INNER_SHAPE 0
#define OUTER_SHAPE 1

#define EXIT_SHAPES 10

/* in the work's flags: the exit asks no one while it is set. it is set at
 * creation and at each question. the step clears it once no player touches
 * OUTER_SHAPE */
#define EXIT_OCCUPIED 0x100

/* the exit pulls player 0 toward its middle while it holds the player. each
 * step takes PULL of the distance away, in x and z */
#define PULL 0.2F

#define BEAMS 4
#define BEAM_FRAMES 8
/* tick holds the beams' frame, and scl.x the part of a frame past it. each
 * step adds BEAM_SPEED frames */
#define BEAM_SPEED 0.8F
#define BEAM_SCALE_X 0.8F
#define BEAM_SCALE_Y 1.1F

/* ang.x holds the disc's turn about y, which falls by DISC_TURN a step */
#define DISC_TURN 0xc0

static struct collision_shape exit_shapes[EXIT_SHAPES] = {
#include "chaostglobby/exit/exit_shapes.inc"
};

#define EXIT_BEAM_ATTR (NJD_SPRITE_COLOR | NJD_SPRITE_ALPHA)

static NJS_TEXANIM beam_cells[BEAM_FRAMES] = {
#include "chaostglobby/exit/beam_cells.inc"
};
static NJS_POINT3 beam_positions[BEAMS] = {
#include "chaostglobby/exit/beam_positions.inc"
};

static void exit_step(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case EXIT_INIT:
        if (work->timer++ > START_STEPS - 1) {
            work->mode = EXIT_WAIT;
            work->timer = LOBBY_ANSWER_YES;
        }
        break;
    case EXIT_WAIT:
        if (work->flags & EXIT_OCCUPIED) {
            if (!collision_next_player_by_shape(task, OUTER_SHAPE)) {
                work->flags &= ~EXIT_OCCUPIED;
            }
        } else if (collision_next_player_by_shape(task, INNER_SHAPE)) {
            work->flags |= EXIT_OCCUPIED;
            pad_disable();
            pause_disable();
            lobby_exit_prompt();
            lobby_select_answer(work->timer);
            work->mode = EXIT_CHOICE;
        }
        break;
    case EXIT_CHOICE:
        if ((player_pads[0]->press & PDD_DGT_KU) ||
            (player_pads[0]->press & PDD_DGT_KD)) {
            if (work->timer == LOBBY_ANSWER_YES) {
                work->timer = LOBBY_ANSWER_NO;
            } else {
                work->timer = LOBBY_ANSWER_YES;
            }
        }
        lobby_select_answer(work->timer);
        if (player_pads[0]->press & PDD_DGT_TA) {
            lobby_close_prompt();
            if (work->timer == LOBBY_ANSWER_YES) {
                chao_leave();
                work->mode = EXIT_LEAVING;
            } else {
                pause_enable();
                pad_enable();
                work->mode = EXIT_WAIT;
            }
        }
        break;
    /* @hack redundant: a case for EXIT_LEAVING, which does nothing */
    case EXIT_LEAVING:
        break;
    }
    if (collision_next_player(task) && !pad_is_enabled()) {
        /* @hack spelling: player, a copy of player_work[0] for its one use,
         * where player_work[0] would do */
        struct task_work *player = player_work[0];
        struct player_motion *motion = player_motion[0];
        NJS_POINT3 *pos = &player->pos;
        motion->spd.x = 0.0F;
        motion->spd.z = 0.0F;
        motion->acc.x = 0.0F;
        motion->acc.z = 0.0F;
        pos->x += (work->pos.x - pos->x) * PULL;
        pos->z += (work->pos.z - pos->z) * PULL;
    }
    work->scl.x += BEAM_SPEED;
    if (work->scl.x >= 1.0F) {
        work->tick++;
        if (work->tick > BEAM_FRAMES - 1) {
            work->tick = 0;
        }
        work->scl.x -= 1.0F;
    }
    work->ang.x -= DISC_TURN;
    collision_reset(task);
    /* @hack spelling: a collision_register on each path through nested tests,
     * where one call under work->mode != EXIT_INIT || chao_previous_area() > 0
     * would do */
    if (work->mode == EXIT_INIT) {
        if (chao_previous_area() <= 0) {
            return;
        } else {
            collision_register(task);
        }
    } else {
        collision_register(task);
    }
}

static void exit_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_SPRITE sprite;
    NJS_VECTOR view;
    Angle pitch;
    Angle yaw;
    int i;

    sprite.tlist = &lobby_texlist;
    sprite.tanim = beam_cells;
    sprite.ang = 0;
    sprite.sx = BEAM_SCALE_X;
    sprite.sy = BEAM_SCALE_Y;
    sprite.p.x = 0.0F;
    sprite.p.y = 0.0F;
    sprite.p.z = 0.0F;
    GATE_DRAW_BEGIN(work);
    njCnkSimpleDrawObject(&lobby_exit_base);
    njCnkSimpleDrawObject(&lobby_exit_frame);
    constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    for (i = 0; i < BEAMS; i++) {
        NJS_POINT3 *pos = &beam_positions[i];
        /* @hack spelling: camera, a pointer to the camera's position for its
         * one copy, where camera_views[0]->pos would do */
        NJS_POINT3 *camera = &camera_views[0]->pos;
        view = *camera;
        njSubVector(&view, pos);
        direction_angles_xy(view.x, view.y, view.z, &pitch, &yaw);
        njPushMatrixEx();
        njTranslateEx(pos);
        njRotateY(NULL, yaw - work->ang.y);
        njDrawSprite3D(&sprite,
                       (work->tick + (i * (BEAM_FRAMES / BEAMS))) % BEAM_FRAMES,
                       EXIT_BEAM_ATTR | NJD_SPRITE_ANGLE);
        njPopMatrixEx();
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njRotateY(NULL, work->ang.x);
    njCnkSimpleDrawObject(&lobby_exit_disc);
    njPopMatrixEx();
    draw_control_clear(GATE_SHADOW);
}

static void exit_destroy(struct task *task)
{
}

struct task *lobby_exit_create(const NJS_POINT3 *pos, Angle yaw)
{
    struct task *task =
        task_create(TASK_WORK, 2, exit_step, "ALO_LobbyExitExecutor");
    if (task) {
        struct task_work *work = task->work;
        GATE_INSTALL(task, work, pos, yaw, exit_shapes, EXIT_SHAPES, exit_draw,
                     exit_destroy);
        work->flags |= EXIT_OCCUPIED;
    }
    return task;
}

/* nothing calls it. it isn't static, since SHC leaves out a static function
 * that nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void fun_0c9c0b3c(void)
{
}
