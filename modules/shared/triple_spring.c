/* the triple spring, 3SPRING in a stage's object list: a slanted board on
 * three springs. it throws a player who touches it */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/triple_spring.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "vmu/vibration.h"
#include "shared/triple_spring/text.inc"

/* a throw sends the player along its own y at THROW_SPEED plus scl.x */
#define THROW_SPEED 5.0F
/* the steps a thrown player's stick is locked for, off a path */
#define THROW_LOCK 30
#define THROW_SOUND 0x1000
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15
#define VIBRATION_TIME 0
/* a throw first moves the player in the spring's space. the player rises
 * THROW_LIFT, and moves toward a z of THROW_Z, keeping THROW_KEEP of its
 * distance from it */
#define THROW_LIFT 5.0F
#define THROW_Z 8.0F
#define THROW_KEEP 0.3F
/* a touch throws a player only WAIT_STEPS or more steps after its last */
#define WAIT_STEPS 10
/* the players the record keeps a wait for, slots 0 and 1 */
#define SPRING_PLAYERS 2

/* a spring's record, in task->motion. offset is how far the board has
 * bounced, and speed what each step adds to it. the board draws moved by
 * njAbs(offset) along the spring's y, and as far along its z. wait[player]
 * counts the steps until a touch can throw the player again */
struct triple_spring {
    Float speed;
    Float offset;
    Sint8 wait[SPRING_PLAYERS];
};

#define SPRING(task) ((struct triple_spring *)(task)->motion.p)

static void triple_spring_destroy(struct task *task);
static void triple_spring_step(struct task *task);
static void triple_spring_draw(struct task *task);

static NJS_TEXNAME triple_spring_texnames[4] = {
#include "shared/triple_spring/triple_spring_texnames.inc"
};
static NJS_TEXLIST triple_spring_texlist = {
#include "shared/triple_spring/triple_spring_texlist.inc"
};
/* the board's model: a chunk model of BOARD_POINTS points, each with its
 * normal */
#define BOARD_POINTS 43
static Sint16 triple_spring_board_polygons[397] = {
#include "shared/triple_spring/triple_spring_board_polygons.inc"
};
static Sint32 triple_spring_board_vertices[261] = {
#include "shared/triple_spring/triple_spring_board_vertices.inc"
};
static NJS_CNK_MODEL triple_spring_board_model = {
#include "shared/triple_spring/triple_spring_board_model.inc"
};
static NJS_CNK_OBJECT triple_spring_board_object = {
#include "shared/triple_spring/triple_spring_board_object.inc"
};
/* the base's model: a chunk model of BASE_POINTS points, each with its
 * normal */
#define BASE_POINTS 38
static Sint16 triple_spring_base_polygons[285] = {
#include "shared/triple_spring/triple_spring_base_polygons.inc"
};
static Sint32 triple_spring_base_vertices[231] = {
#include "shared/triple_spring/triple_spring_base_vertices.inc"
};
static NJS_CNK_MODEL triple_spring_base_model = {
#include "shared/triple_spring/triple_spring_base_model.inc"
};
/* the spring's nodes, the base with the board as its child */
static NJS_CNK_OBJECT triple_spring_base_object = {
#include "shared/triple_spring/triple_spring_base_object.inc"
};
NJS_TEXLIST *triple_spring_texlists[2] = {
#include "shared/triple_spring/triple_spring_texlists.inc"
};
/* the spring's shape, a cylinder that its quarter turn lays along the
 * spring's x */
/* clang-format off */
static struct collision_shape triple_spring_shape = {
#include "shared/triple_spring/triple_spring_shape.inc"
};
/* clang-format on */
/* the board's bounce. each step takes offset times triple_spring_stiffness
 * from speed, then scales speed by triple_spring_damping. a throw sets speed
 * to triple_spring_kick, and offset to 0 */
static Float triple_spring_stiffness = 0.25F;
static Float triple_spring_kick = 1.0F;
static Float triple_spring_damping = 0.95F;

/* @hack type: player a Sint8, where the int that player_slot_of returns would
 * do */
static void triple_spring_throw(struct task *task, Sint8 player)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd = {0.0F, THROW_SPEED, 0.0F};
    struct angles ang;
    NJS_POINT3 pos;

    player_get_past(player, 0, &pos, NULL);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njPushMatrixEx();
    njInvertMatrix(NULL);
    njCalcPoint(NULL, &pos, &pos);
    pos.z = ((pos.z - THROW_Z) * THROW_KEEP) + THROW_Z;
    pos.y += THROW_LIFT;
    njPopMatrixEx();
    njCalcPoint(NULL, &pos, &pos);
    njPopMatrixEx();
    player_set_pos(player, pos.x, pos.y, pos.z);
    spd.y += work->scl.x;
    ang.x = work->ang.x;
    ang.y = work->ang.y + HALF_TURN;
    ang.z = work->ang.z;
    fun_8c049304(player, &spd, &ang, THROW_LOCK);
    sound_play(THROW_SOUND, NULL, 0, 0);
    vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ, VIBRATION_TIME);
}

void triple_spring_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct triple_spring));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: the draw set before the step, where struct task's
     * order would set the step first */
    task->draw = triple_spring_draw;
    task->step = triple_spring_step;
    task->destructor = triple_spring_destroy;
    /* @hack redundant: the submode cleared, though task_create zeroed the
     * work */
    work->submode = 0;
    collision_init(task, &triple_spring_shape, 1, COLLISION_OBJECT);
    work->collision->flags |= COLLISION_TARGET;
    /* @hack redundant: the bounce cleared, though syCalloc zeroed the
     * record */
    SPRING(task)->speed = 0.0F;
    SPRING(task)->offset = 0.0F;
}

static void triple_spring_destroy(struct task *task)
{
    /* @hack spelling: owner, a local for task, where task read in place
     * would do */
    struct task *owner = task;

    syFree(owner->motion.p);
    owner->motion.p = NULL;
}

static void triple_spring_step(struct task *task)
{
    struct task *hit;
    int player;

    if (placement_try_leave(task)) {
        return;
    }
    hit = collision_next_player(task);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (hit != NULL && (player = player_slot_of(hit)) >= 0) {
        /* @hack spelling: the wait tested with !, where == 0 would do */
        if (!SPRING(task)->wait[player]) {
            SPRING(task)->speed = triple_spring_kick;
            SPRING(task)->offset = 0.0F;
            triple_spring_throw(task, player);
        }
        SPRING(task)->wait[player] = WAIT_STEPS;
    }
    /* @hack spelling: player reused as the loop's counter, where a counter
     * of its own would do */
    for (player = 0; player < SPRING_PLAYERS; player++) {
        if (SPRING(task)->wait[player] != 0) {
            SPRING(task)->wait[player]--;
        }
    }
    collision_register(task);
    SPRING(task)->speed = (SPRING(task)->speed -
                           (SPRING(task)->offset * triple_spring_stiffness)) *
                          triple_spring_damping;
    SPRING(task)->offset += SPRING(task)->speed;
}

static void triple_spring_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float lift;

    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object. the image holds none of its code */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
        (void)"<- ADD YSPD";
    }
    njSetTexture(&triple_spring_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(&triple_spring_base_model);
    lift = njAbs(SPRING(task)->offset);
    njTranslate(NULL, 0.0F, lift, lift);
    njCnkEasyDrawModel(&triple_spring_board_model);
    njPopMatrixEx();
}
