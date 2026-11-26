/* emerald pieces in their eleven kinds, and the sign, as emerald.h declares
 * them */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/emerald.h"
#include "shared/ring_sparkle.h"
#include "shared/sparkle.h"
#include "stage/hunt.h"
#include "stage/path.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#include "shared/emerald/text.inc"

/* bits 8 to 15 of a piece's ang.x hold its kind, and bits 0 to 7 its id */
#define KIND_SHIFT 8
#define BYTE_BITS 0xff
#define KIND_BYTE(work) (((work)->ang.x >> KIND_SHIFT) & BYTE_BITS)
#define KIND(work) (emerald_kinds[KIND_BYTE(work) % EMERALD_KINDS])
#define ID(work) ((Sint8)(work)->ang.x)
/* the radius of a piece's shape, RADIUS_BIAS above scl.y */
#define RADIUS(work) ((work)->scl.y + RADIUS_BIAS)
#define RADIUS_BIAS 1.0F
/* how far a piece has turned about y */
#define TURN(task) ((task)->unk_3c.l)

/* a piece's mode. a piece from an object list starts in MODE_PLACED, and one
 * from emerald_create in MODE_CREATED. a child piece starts in MODE_SHOWN.
 * emerald_set_shown sets MODE_SHOWN or MODE_HIDDEN. a hidden piece is neither
 * drawn nor taken. only a placed piece tests whether it has left range */
#define MODE_PLACED 0
#define MODE_CREATED 3
#define MODE_SHOWN 4
#define MODE_HIDDEN 5

/* a piece's submode: whether stage/hunt.h's hunt holds a piece of its kind and
 * id. only a PIECE_SOUGHT piece is drawn and can be taken. an
 * EMERALD_KIND_INENEMY piece's submode is instead the EMERALD_PIECE_ value it
 * is drawn as */
#define PIECE_NEW 0
#define PIECE_ASKING 1 /* asks on each step until a hunt runs */
#define PIECE_SOUGHT 2 /* the hunt holds it */
#define PIECE_UNSOUGHT 3 /* the hunt holds none */

/* the lights a piece is drawn by, as indexes in lights. a look with
 * EMERALD_LOOK_PULSE takes LIGHT_PULSE, which is LIGHT_BASE brightened by the
 * pulse */
#define LIGHT_BASE 4
#define LIGHT_PULSE 7

/* emerald_draw_mark's mark pulses by an angle that gains MARK_PULSE a step.
 * its scale along x and z swings MARK_WIDTH_SWING about MARK_WIDTH, and along
 * y MARK_HEIGHT_SWING about MARK_HEIGHT */
#define MARK_PULSE 0x300
#define MARK_WIDTH 1.5F
#define MARK_WIDTH_SWING 0.5F
#define MARK_HEIGHT 4.0F
#define MARK_HEIGHT_SWING 2.0F

/* the bits of _nj_control_3d_flag_ a glow is drawn with */
#define GLOW_CONTROL (DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE)

/* a sign's growth, turns and burst */
#define SIGN_GROWTH 0.1F /* the size a sign gains each step */
#define SIGN_STEPS 120 /* the steps for which SIGN_SIZE caps its size */
#define SIGN_SIZE 1.2F
#define SIGN_TURN 91 /* its turn a step for those steps */
#define SIGN_SPIN 5097 /* its turn a step after them, with a sparkle a step */
#define SIGN_END_SIZE 4.0F /* the size past which it bursts and ends */
#define SIGN_BURST 8 /* the sparkles of the burst */
#define SIGN_HEIGHT 2.0F /* its height above its player's first shape */
#define SIGN_LIFT 3.8F /* the height each unit of its size adds */
/* the task levels of a piece emerald_create makes, and of a sign */
#define PIECE_LEVEL 1
#define SIGN_LEVEL 3
/* the players a sign can show over */
#define SIGN_PLAYERS 2
/* a sign's sparkle that falls keeps SPARKLE_FALL of its speed along y */
#define SPARKLE_FALL 0.3F
/* a sign's mode: SIGN_NEW until its first step sets it up */
#define SIGN_NEW 0
#define SIGN_SHOWING 1
/* the EMERALD_PIECE_ value a sign shows */
#define SIGN_PIECE(task) ((task)->unk_3c.l)

/* the bits of a seed that pick a place's direction, distance and height. the
 * shifts bring the distance's and the height's bits down to bit 0 */
#define SEED_DIRECTION_BITS 0x0f /* one of sixteen */
#define SEED_DISTANCE_BITS 0xf0 /* 0 to 15 fifteenths of the way out */
#define SEED_HEIGHT_BITS 0x2c /* bits of both the others */
#define SEED_DISTANCE_SHIFT 4
#define SEED_HEIGHT_SHIFT 2
/* the shift that makes a direction's number k sixteenths of a turn */
#define DIRECTION_SHIFT 12

/* the radius of an EMERALD_KIND_INENEMY piece's shape, and of its collision */
#define ENEMY_RADIUS 7.0F

/* a kind's calls for its pieces, its name, and the piece it draws */
struct emerald_kind {
    int kind; /* the entry's own index, which the hunt's calls take */
    const char *name; /* as emerald_debug prints it */
    void (*init)(struct task *task);
    void (*debug)(struct task *task);
    void (*unk_10)(struct task *task); /* which nothing calls */
    void (*step)(struct task *task);
    void (*draw)(struct task *task);
    void (*destructor)(struct task *task);
    /* the EMERALD_PIECE_ value its pieces are drawn as. the draws of
     * EMERALD_KIND_FINAL and EMERALD_KIND_INENEMY pick their own */
    int piece;
};

/* a piece on a path keeps this record in its extra */
struct emerald_path_place {
    NJS_POINT3 pos; /* its place in the world */
    NJS_POINT3 center; /* the middle of the box its path's points fill */
};

/* a piece on a path keeps its place in two of its task's words. force holds
 * the index of the first point of its segment. motion holds a Float, how far
 * along that segment it lies, from 0 to 1 */
#define PATH_SEGMENT(task) (&(task)->force.l)
#define PATH_FRACTION(task) (&(task)->motion.f)
#define PATH_PLACE(task) ((struct emerald_path_place *)(task)->extra.p)
/* the path a piece's work's scl.x picks, and how far the piece moves a step */
#define PATH(work) (emerald_paths[(Uint32)(work)->scl.x % emerald_path_count])
#define PATH_SPEED(work) ((work)->scl.z + PATH_SPEED_BIAS)
#define PATH_SPEED_BIAS 3.0F
#define PATH_POINTS(work) ((NJS_POINT3 *)PATH(work)->points)

/* the table of paths emerald_set_paths gave, or null */
static struct path **emerald_paths;

/* nothing calls these, and no header declares them. they are external only
 * because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* emerald_create, but the piece is parent's child and is shown. returns the
 * task, or null where task_create_child fails */
struct task *emerald_create_child(struct task *parent, NJS_POINT3 *pos,
                                  Uint8 kind, Uint8 id);
/* a piece's debug view, whose address no image that places this source
 * holds. it prints the piece's kind, id and kind name, and calls its kind's
 * debug view. its label "<- COLSIZE(+3)" says 3 is added to the collision's
 * size, where RADIUS_BIAS is 1 */
void emerald_debug(struct task *task);
/* NOLINTEND(misc-use-internal-linkage) */

static void emerald_plain_init(struct task *task);
static void emerald_plain_debug(struct task *task);
static void emerald_plain_step(struct task *task);
static void emerald_plain_draw(struct task *task);
static void emerald_plain_destroy(struct task *task);
static void emerald_spread_init(struct task *task);
static void emerald_spread_debug(struct task *task);
static void emerald_spread_step(struct task *task);
static void emerald_spread_draw(struct task *task);
static void emerald_spread_destroy(struct task *task);
static void emerald_path_init(struct task *task);
static void emerald_path_debug(struct task *task);
static void emerald_path_reset(struct task *task);
static void emerald_path_step(struct task *task);
static void emerald_path_draw(struct task *task);
static void emerald_path_destroy(struct task *task);
static void emerald_final_draw(struct task *task);
static void emerald_enemy_init(struct task *task);
static void emerald_enemy_step(struct task *task);
static void emerald_enemy_draw(struct task *task);
static void emerald_enemy_destroy(struct task *task);
static void emerald_sign_step(struct task *task);
static void emerald_sign_draw(struct task *task);
static void emerald_draw_glow(int piece);

static NJS_TEXNAME emerald_texname_b[1] = {
#include "shared/emerald/emerald_texname_b.inc"
};
static NJS_TEXLIST emerald_texlist_b = {
#include "shared/emerald/emerald_texlist_b.inc"
};
static NJS_TEXNAME emerald_texname_a[1] = {
#include "shared/emerald/emerald_texname_a.inc"
};
static NJS_TEXLIST emerald_texlist_a = {
#include "shared/emerald/emerald_texlist_a.inc"
};
static NJS_TEXNAME emerald_texname_c[1] = {
#include "shared/emerald/emerald_texname_c.inc"
};
static NJS_TEXLIST emerald_texlist_c = {
#include "shared/emerald/emerald_texlist_c.inc"
};

/* the model each piece and sign takes in the look they start with */
#define EMERALD_POINTS 22
static Sint16 emerald_polygons[75] = {
#include "shared/emerald/emerald_polygons.inc"
};
static Sint32 emerald_vertices[135] = {
#include "shared/emerald/emerald_vertices.inc"
};
static NJS_CNK_MODEL emerald_model = {
#include "shared/emerald/emerald_model.inc"
};
static NJS_CNK_OBJECT emerald_object = {
#include "shared/emerald/emerald_object.inc"
};

NJS_TEXLIST *emerald_texlists[6] = {
#include "shared/emerald/emerald_texlists.inc"
};

/* the count of paths in emerald_paths, or -1 while there is no table */
static int emerald_path_count = -1;

/* the look the pieces start with, which emerald_reset_look puts back */
static struct emerald_look emerald_look_default = {
#include "shared/emerald/emerald_look_default.inc"
};
struct emerald_look emerald_look = {
#include "shared/emerald/emerald_look.inc"
};

static struct emerald_kind emerald_kinds[EMERALD_KINDS] = {
#include "shared/emerald/emerald_kinds.inc"
};

/* a glow's brightness, which repeats every emerald_glow_period steps of
 * game_state.steps. it rises from 0 to 1 over the first emerald_glow_rise
 * steps. it then falls from 25/24 to 1/24 over the rest */
static int emerald_glow_period = 30;
static int emerald_glow_rise = 5;

/* the pulse that brightens LIGHT_PULSE, a level that pulse_level gives for
 * these counts of steps. LIGHT_PULSE's ambient is LIGHT_BASE's times a gain,
 * 1 + emerald_pulse_gain * level. its intensity is LIGHT_BASE's times the
 * mean of 1 and the gain */
static Float emerald_pulse_gain = 10.0F;
static int emerald_pulse_hold = 8;
static int emerald_pulse_rest = 70;
static int emerald_pulse_rise = 10;
static int emerald_pulse_fall = 50;

static Float emerald_sign_sparkle_speed = 2.5F;
static Float emerald_sign_sparkle_scale = 1.2F;

#define SHAPE_FLAGS (SHAPE_UNTURNED | SHAPE_IGNORE_ALL_BUT_PLAYERS)
/* clang-format off */
/* a piece's shape, whose radius each piece sets */
static struct collision_shape emerald_shape[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL), 0, SHAPE_FLAGS, {0.0F, 0.0F, 0.0F},
     3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */
static NJS_POINT3 emerald_origin = {0.0F, 0.0F, 0.0F};
/* clang-format off */
/* the shape of a piece on a path, whose center the piece sets in the world */
static struct collision_shape emerald_path_shape[1] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL), 0, SHAPE_FLAGS | SHAPE_IN_WORLD,
     {0.0F, 0.0F, 0.0F}, 3.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

void emerald_reset_look(void)
{
    emerald_look = emerald_look_default;
}

void emerald_set_paths(struct path **paths, int count)
{
    emerald_paths = paths;
    emerald_path_count = count;
}

void emerald_clear_paths(void)
{
    emerald_path_count = -1;
    emerald_paths = NULL;
}

/* whether player may take a piece of kind. a piece of an underground kind
 * needs its player to be digging out. it needs nothing in four stages: Wild
 * Canyon, its 2P stage, Dry Lagoon and Security Hall. it needs nothing where
 * player has no work either */
static Bool emerald_can_take(int kind, int player)
{
    switch (kind) {
    case EMERALD_KIND_A_UNDERGND:
    case EMERALD_KIND_B_UNDERGND:
    case EMERALD_KIND_A_2P_UNDGND:
        if (player_work[player] != NULL) {
            switch (player_work[player]->mode) {
            case PLAYER_MODE_HUNTER_DIG_OUT:
            case PLAYER_MODE_HUNTER_WALL_DIG_OUT:
                return TRUE;
            }
            switch (stage_current_id()) {
            case STAGE_WILD_CANYON:
            case STAGE_WILD_CANYON_2P:
            case STAGE_DRY_LAGOON:
            case STAGE_SECURITY_HALL:
                return TRUE;
            }
            return FALSE;
        }
        break;
    }
    return TRUE;
}

struct task *emerald_create(NJS_POINT3 *pos, Uint8 kind, Uint8 id)
{
    struct task *task =
        task_create(TASK_WORK, PIECE_LEVEL, emerald_init, EMERALD_OBJECT_NAME);
    if (task != NULL) {
        struct task_work *work = task->work;
        work->pos = *pos;
        /* @hack spelling: task->work read again for ang.x and mode, where
         * the local work would do */
        task->work->ang.x = (kind << KIND_SHIFT) | id;
        task->work->mode = MODE_CREATED;
    }
    return task;
}

void emerald_set_shown(struct task *task, int shown)
{
    if (shown) {
        task->work->mode = MODE_SHOWN;
        /* @hack redundant: the test that submode isn't PIECE_SOUGHT, which
         * the test before it settles */
        if (task->work->submode == PIECE_UNSOUGHT &&
            task->work->submode != PIECE_SOUGHT) {
            task->work->submode = PIECE_ASKING;
        }
    } else {
        task->work->mode = MODE_HIDDEN;
    }
}

struct task *emerald_create_child(struct task *parent, NJS_POINT3 *pos,
                                  Uint8 kind, Uint8 id)
{
    struct task *task = task_create_child(TASK_WORK, emerald_init, parent);
    if (task != NULL) {
        struct task_work *work = task->work;
        work->pos = *pos;
        /* @hack spelling: task->work read again for ang.x and mode, where
         * the local work would do */
        task->work->ang.x = (kind << KIND_SHIFT) | id;
        task->work->mode = MODE_SHOWN;
    }
    return task;
}

struct task *emerald_create_plain_child(struct task *parent, NJS_POINT3 *pos,
                                        Uint8 kind, Uint8 id)
{
    struct task *task =
        task_create_child(TASK_WORK, emerald_plain_step, parent);
    if (task != NULL) {
        struct task_work *work = task->work;
        work->pos = *pos;
        /* @hack spelling: task->work read again for ang.x and mode, where
         * the local work would do */
        task->work->ang.x = (kind << KIND_SHIFT) | id;
        task->work->mode = MODE_SHOWN;
        task->draw = emerald_plain_draw;
        task->destructor = emerald_plain_destroy;
        emerald_plain_init(task);
    }
    return task;
}

/* set LIGHT_PULSE to LIGHT_BASE brightened by the pulse, and apply it */
static void emerald_apply_pulse_light(void)
{
    struct light light;
    Float gain;

    light = lights[LIGHT_BASE];
    gain = 1.0F + (pulse_level(emerald_pulse_hold, emerald_pulse_rest,
                               emerald_pulse_rise, emerald_pulse_fall) *
                   emerald_pulse_gain);
    light.ambient *= gain;
    light.intensity *= 0.5F + (0.5F * gain);
    light_set(LIGHT_PULSE, &light);
    light_apply(LIGHT_PULSE);
}

void emerald_debug(struct task *task)
{
    struct task_work *work = task->work;

    njPrintC(NJM_LOCATION(17, 16), EMERALD_DEBUG_KIND_ID);
    njPrintH(NJM_LOCATION(24, 16), KIND_BYTE(work) % EMERALD_KINDS, 1);
    njPrintH(NJM_LOCATION(30, 16), work->ang.x & BYTE_BITS, 2);
    njPrintC(NJM_LOCATION(20, 15), KIND(work).name);
    njPrintC(NJM_LOCATION(17, 20), EMERALD_DEBUG_RADIUS);
    KIND(work).debug(task);
    task->destructor = KIND(work).destructor;
}

void emerald_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: step set after draw and destructor, not in the
     * members' order */
    task->draw = KIND(work).draw;
    task->destructor = KIND(work).destructor;
    task->step = KIND(work).step;
    KIND(work).init(task);
}

/* draw piece, an EMERALD_PIECE_ value, at pos by emerald_look, turned by ang.
 * fog is on when the call returns, whatever held before. the call ends by
 * applying light_after_draw's light */
static void emerald_draw(NJS_POINT3 *pos, struct angles *ang, int piece)
{
    njFogDisable();
    if (emerald_look.flags & EMERALD_LOOK_PULSE) {
        emerald_apply_pulse_light();
    } else {
        light_apply(LIGHT_BASE);
    }
    njPushMatrixEx();
    if (emerald_look.bob_speed) {
        njTranslate(NULL, pos->x,
                    pos->y + (njSin(game_state.steps * emerald_look.bob_speed) *
                              emerald_look.bob_height),
                    pos->z);
    } else {
        njTranslateEx(pos);
    }
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, ang->y);
    njSetTexture(emerald_look.texlists[piece]);
    njCnkSimpleDrawModel(emerald_look.models[piece]);
    emerald_draw_glow(piece);
    njPopMatrixEx();
    light_apply(light_after_draw);
    njFogEnable();
}

static void emerald_draw_turned(NJS_POINT3 *pos, Angle ang_y, int piece)
{
    struct angles ang;

    ang.x = 0;
    ang.y = ang_y;
    ang.z = 0;
    emerald_draw(pos, &ang, piece);
}

/* emerald_draw, but the piece is stretched into a pulsing mark. the kinds'
 * debug views draw the mark at a piece's place */
static void emerald_draw_mark(NJS_POINT3 *pos, struct angles *ang, int piece)
{
    njFogDisable();
    if (emerald_look.flags & EMERALD_LOOK_PULSE) {
        emerald_apply_pulse_light();
    } else {
        light_apply(LIGHT_BASE);
    }
    njPushMatrixEx();
    if (emerald_look.bob_speed) {
        njTranslate(NULL, pos->x,
                    pos->y + (njSin(game_state.steps * emerald_look.bob_speed) *
                              emerald_look.bob_height),
                    pos->z);
    } else {
        njTranslateEx(pos);
    }
    njRotateZ(NULL, ang->z);
    njRotateX(NULL, ang->x);
    njRotateY(NULL, ang->y);
    njScale(
        NULL,
        MARK_WIDTH + (njCos(game_state.steps * MARK_PULSE) * MARK_WIDTH_SWING),
        MARK_HEIGHT +
            (njSin(game_state.steps * MARK_PULSE) * MARK_HEIGHT_SWING),
        MARK_WIDTH + (njCos(game_state.steps * MARK_PULSE) * MARK_WIDTH_SWING));
    njSetTexture(emerald_look.texlists[piece]);
    njCnkSimpleDrawModel(emerald_look.models[piece]);
    emerald_draw_glow(piece);
    njPopMatrixEx();
    light_apply(light_after_draw);
    njFogEnable();
}

static void emerald_draw_mark_turned(NJS_POINT3 *pos, Angle ang_y, int piece)
{
    struct angles ang;

    ang.x = 0;
    ang.y = ang_y;
    ang.z = 0;
    emerald_draw_mark(pos, &ang, piece);
}

struct task *emerald_sign_create(int player, int piece)
{
    struct task *task;

    /* NOLINTNEXTLINE(bugprone-infinite-loop) */
    while (player < 0 || player >= SIGN_PLAYERS) {
    }
    task = task_create(TASK_WORK, SIGN_LEVEL, emerald_sign_step,
                       EMERALD_SIGN_NAME);
    if (task != NULL) {
        /* @hack spelling: work, a local for task->work, where task->work->tick
         * would do */
        struct task_work *work = task->work;
        work->tick = player;
        SIGN_PIECE(task) = piece;
    }
    return task;
}

static void emerald_sign_sparkle(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;

    spd.x = emerald_sign_sparkle_speed * ((rand() * RAND_SCALE) - 0.5F);
    spd.z = emerald_sign_sparkle_speed * ((rand() * RAND_SCALE) - 0.5F);
    spd.y = emerald_sign_sparkle_speed * ((rand() * RAND_SCALE) - 0.5F);
    if (spd.y < 0.0F) {
        spd.y *= SPARKLE_FALL;
    }
    sparkle_create(&work->pos, &spd, emerald_sign_sparkle_scale);
}

static void emerald_sign_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player = player_work[work->tick];
    int i;

    if (player == NULL) {
        task_kill(task);
        return;
    }
    switch (work->mode) {
    case SIGN_NEW:
        work->mode = SIGN_SHOWING;
        work->scl.x = 0.0F;
        work->timer = 0;
        task->draw = emerald_sign_draw;
        break;
    case SIGN_SHOWING:
        work->pos = player->collision->shapes->center;
        work->pos.y += player->collision->shapes->radius +
                       (work->scl.x * SIGN_LIFT) + SIGN_HEIGHT;
        work->scl.x += SIGN_GROWTH;
        work->timer++;
        if (work->timer > SIGN_STEPS) {
            work->ang.y += SIGN_SPIN;
            if (work->scl.x > SIGN_END_SIZE) {
                for (i = 0; i < SIGN_BURST; i++) {
                    emerald_sign_sparkle(task);
                }
                task_kill(task);
                break;
            }
            emerald_sign_sparkle(task);
        } else {
            work->ang.y += SIGN_TURN;
            if (work->scl.x > SIGN_SIZE) {
                work->scl.x = SIGN_SIZE;
            }
        }
        break;
    }
}

static void emerald_sign_draw(struct task *task)
{
    struct task_work *work = task->work;

    njFogDisable();
    if (emerald_look.flags & EMERALD_LOOK_PULSE) {
        emerald_apply_pulse_light();
    } else {
        light_apply(LIGHT_BASE);
    }
    njPushMatrixEx();
    if (emerald_look.bob_speed) {
        njTranslate(NULL, work->pos.x,
                    work->pos.y +
                        (njSin(game_state.steps * emerald_look.bob_speed) *
                         emerald_look.bob_height),
                    work->pos.z);
    } else {
        njTranslateEx(&work->pos);
    }
    njRotateY(NULL, work->ang.y);
    njSetTexture(emerald_look.texlists[SIGN_PIECE(task)]);
    njScale(NULL, work->scl.x, work->scl.x, work->scl.x);
    njCnkSimpleDrawModel(emerald_look.models[SIGN_PIECE(task)]);
    emerald_draw_glow(SIGN_PIECE(task));
    njPopMatrixEx();
    light_apply(light_after_draw);
    njFogEnable();
}

/* draw the debug sphere that reaches radius from pos */
static void emerald_draw_radius(NJS_POINT3 *pos, Float radius)
{
    NJS_VECTOR scl;

    scl.x = radius;
    scl.y = radius;
    scl.z = radius;
    debug_draw_model(pos, NULL, &scl, DEBUG_MODEL_SPHERE);
}

/* the plain calls: the kinds whose pieces stay where they are put share
 * them */

static void emerald_plain_debug(struct task *task)
{
    struct task_work *work = task->work;

    emerald_draw_mark_turned(&work->pos, TURN(task), KIND(work).piece);
    emerald_draw_radius(&work->pos, RADIUS(work));
}

static void emerald_plain_init(struct task *task)
{
    struct task_work *work = task->work;

    hunt_add_place(KIND(work).kind, ID(work), &work->pos);
    if (work->mode != MODE_PLACED || !placement_try_leave(task)) {
        collision_init(task, emerald_shape, 1, COLLISION_PICKUP);
        work->collision->shapes->radius = RADIUS(work);
    }
}

static void emerald_plain_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

/* a touch by a player's or an attack's record takes a sought piece that isn't
 * hidden. emerald_can_take must allow the player. on STAGE_EGG_QUARTERS, only a
 * touch of the record's first shape takes an EMERALD_KIND_FINAL piece. a shown
 * or hidden piece, which a parent may move, tells the hunt its place on each
 * step */
static void emerald_plain_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (work->mode == MODE_PLACED && placement_try_leave(task)) {
        return;
    }
    if (work->submode <= PIECE_ASKING) {
        if (work->submode == PIECE_ASKING) {
            switch (hunt_find_piece(KIND(work).kind, ID(work), NULL)) {
            case HUNT_NOT_FOUND:
                work->submode = PIECE_UNSOUGHT;
                break;
            case HUNT_FOUND(HUNT_SLOT_C):
            case HUNT_FOUND(HUNT_SLOT_B):
            case HUNT_FOUND(HUNT_SLOT_A):
                work->submode = PIECE_SOUGHT;
                break;
            }
        } else {
            work->submode = PIECE_ASKING;
        }
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN &&
        (work->collision->flags & COLLISION_TOUCHED)) {
        if (!(work->collision->hit_shape != 0 &&
              stage_current_id() == STAGE_EGG_QUARTERS &&
              KIND(work).kind == EMERALD_KIND_FINAL) &&
            (work->collision->hit->kind == COLLISION_PLAYER ||
             work->collision->hit->kind == COLLISION_ATTACK) &&
            emerald_can_take(KIND(work).kind,
                             player =
                                 player_slot_of(work->collision->hit->owner))) {
            /* @hack redundant: a second player_slot_of call for player, which
             * the test has set from the first */
            player = player_slot_of(work->collision->hit->owner);
            hunt_take_piece(KIND(work).kind, ID(work), player, &work->pos);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            return;
        }
    }
    if ((work->mode == MODE_SHOWN || work->mode == MODE_HIDDEN) &&
        work->submode >= PIECE_SOUGHT) {
        hunt_find_piece(KIND(work).kind, ID(work), &work->pos);
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN) {
        collision_register(task);
        TURN(task) += emerald_look.spin;
    }
}

static void emerald_plain_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: the kind table and its count read through locals,
     * where KIND(work) would do */
    int kind_count = EMERALD_KINDS;
    struct emerald_kind *kinds = emerald_kinds;

    /* @hack spelling: a while whose body returns, where an if would do */
    while (work->submode != PIECE_SOUGHT || work->mode == MODE_HIDDEN) {
        return;
    }
    if (work->mode == MODE_SHOWN && task->parent != NULL &&
        work->tick == EMERALD_TICK_PARENT_ANGLES) {
        struct angles ang;
        /* @hack spelling: parent_ang, a local for the parent's angles, where
         * task->parent->work->ang read in place would do */
        struct angles *parent_ang = &task->parent->work->ang;

        ang.x = parent_ang->x;
        ang.y = TURN(task);
        ang.z = parent_ang->z;
        emerald_draw(&work->pos, &ang,
                     kinds[KIND_BYTE(work) % kind_count].piece);
    } else {
        emerald_draw_turned(&work->pos, TURN(task),
                            kinds[KIND_BYTE(work) % kind_count].piece);
    }
}

/* the spread calls, EMERALD_KIND_A_2P_UNDGND's. a piece lies at a place in the
 * region about its work's pos, which a random seed picks. a placed piece keeps
 * its seed in its placement's state, and so comes back to the same place. a
 * state of 0 is no seed, and the piece picks a new one */

/* a random seed, 0 to 255 */
static Uint32 emerald_random_seed(void)
{
    Uint32 seed = rand() * (256.0F * RAND_SCALE);

    if (seed > 255) {
        seed = 255;
    }
    return seed;
}

/* set out to the place seed picks in the region about pos. the region is an
 * ellipse with radii scl->x along x and scl->z along z. ang->y and ang->z turn
 * it. the place lies from scl->y below the ellipse's plane to 7/15 of scl->y
 * above it */
static void emerald_spread_point(NJS_VECTOR *scl, NJS_POINT3 *pos,
                                 struct angles *ang, NJS_POINT3 *out,
                                 Uint32 seed)
{
    Angle turn;
    /* @hack type: direction, an int, where the unsigned expression in the
     * product would do */
    int direction = (seed & SEED_DIRECTION_BITS) << DIRECTION_SHIFT;
    Float radius;
    Float height;
    NJS_POINT3 point;

    /* @bug logic: the sixteen directions fall in eight pairs 8 degrees apart,
     * where they should lie 22.5 degrees from each other. direction is k
     * sixteenths of a turn in Angle units, for k from 0 to 15. the product
     * takes it for degrees. directions k and k + 8 then differ by 32768
     * degrees, 8 past whole turns. the product is FMUL FR2,FR3 at 0xf18 of the
     * object's code */
    turn = direction * ANGLE_PER_DEGREE;
    radius =
        ((seed & SEED_DISTANCE_BITS) >> SEED_DISTANCE_SHIFT) * (1.0F / 15.0F);
    height = ((seed & SEED_HEIGHT_BITS) >> SEED_HEIGHT_SHIFT) * (1.0F / 15.0F);
    point.x = njCos(turn) * radius * scl->x;
    point.y = (height - 0.5F) * scl->y * 2.0F;
    point.z = njSin(turn) * radius * scl->z;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(pos);
    njRotateY(NULL, ang->y);
    njRotateZ(NULL, ang->z);
    njCalcPoint(NULL, &point, out);
    njPopMatrixEx();
}

/* EMERALD_KIND_A_2P_UNDGND's debug view, which only emerald_debug calls. it
 * draws the region at the origin, not at the piece's pos. it draws the mark
 * at a new random place in the region about pos */
static void emerald_spread_debug(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scl;
    struct angles ang;
    NJS_POINT3 pos;
    /* @hack spelling: seed, a local for emerald_random_seed's result, where
     * the call as the argument would do */
    Uint32 seed;

    njPrintC(NJM_LOCATION(17, 17), EMERALD_DEBUG_YAW);
    njPrintC(NJM_LOCATION(17, 18), EMERALD_DEBUG_ROLL);
    njPrintC(NJM_LOCATION(17, 19), EMERALD_DEBUG_SCALE_X);
    njPrintC(NJM_LOCATION(17, 21), EMERALD_DEBUG_SCALE_Z);
    scl.x = work->scl.x;
    scl.y = 1.0F;
    scl.z = work->scl.z;
    ang.x = 0;
    ang.y = work->ang.y;
    ang.z = work->ang.z;
    njPushMatrixEx();
    njTranslateEx(&emerald_origin);
    njRotateY(NULL, ang.y);
    njRotateZ(NULL, ang.z);
    debug_draw_model(&emerald_origin, NULL, &scl, DEBUG_MODEL_CYLINDER);
    njPopMatrixEx();
    seed = emerald_random_seed();
    emerald_spread_point(&scl, &work->pos, &ang, &pos, seed);
    emerald_draw_mark_turned(&pos, TURN(task), KIND(work).piece);
    emerald_draw_radius(&pos, RADIUS(work));
}

/* move the piece to the place its seed picks, in the plane of its region.
 * then set it up as emerald_plain_init does, but with a COLLISION_OBJECT
 * record */
static void emerald_spread_init(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 seed = 0;
    NJS_VECTOR scl;
    NJS_POINT3 pos;
    struct angles ang;

    if (task->placement != NULL) {
        seed = (Uint8)placement_get_state(task);
    }
    if (!seed) {
        seed = emerald_random_seed();
    }
    if (task->placement != NULL) {
        placement_set_state(task, seed);
    }
    scl.x = work->scl.x;
    scl.y = 0.0F;
    scl.z = work->scl.z;
    ang.x = 0;
    ang.y = work->ang.y;
    ang.z = work->ang.z;
    emerald_spread_point(&scl, &work->pos, &ang, &pos, seed);
    work->pos = pos;
    hunt_add_place(KIND(work).kind, ID(work), &work->pos);
    if (work->mode != MODE_PLACED || !placement_try_leave(task)) {
        collision_init(task, emerald_shape, 1, COLLISION_OBJECT);
        work->collision->shapes->radius = RADIUS(work);
    }
}

static void emerald_spread_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

/* emerald_plain_step, but the piece never tells the hunt where it is */
static void emerald_spread_step(struct task *task)
{
    struct task_work *work = task->work;
    int player;

    if (work->mode == MODE_PLACED && placement_try_leave(task)) {
        return;
    }
    if (work->submode <= PIECE_ASKING) {
        if (work->submode == PIECE_ASKING) {
            switch (hunt_find_piece(KIND(work).kind, ID(work), NULL)) {
            case HUNT_NOT_FOUND:
                work->submode = PIECE_UNSOUGHT;
                break;
            case HUNT_FOUND(HUNT_SLOT_C):
            case HUNT_FOUND(HUNT_SLOT_B):
            case HUNT_FOUND(HUNT_SLOT_A):
                work->submode = PIECE_SOUGHT;
                break;
            }
        } else {
            work->submode = PIECE_ASKING;
        }
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN &&
        (work->collision->flags & COLLISION_TOUCHED)) {
        if (!(work->collision->hit_shape != 0 &&
              stage_current_id() == STAGE_EGG_QUARTERS &&
              KIND(work).kind == EMERALD_KIND_FINAL) &&
            (work->collision->hit->kind == COLLISION_PLAYER ||
             work->collision->hit->kind == COLLISION_ATTACK) &&
            emerald_can_take(KIND(work).kind,
                             player =
                                 player_slot_of(work->collision->hit->owner))) {
            /* @hack redundant: a second player_slot_of call for player, which
             * the test has set from the first */
            player = player_slot_of(work->collision->hit->owner);
            hunt_take_piece(KIND(work).kind, ID(work), player, &work->pos);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            return;
        }
    }
    /* @hack redundant: a test of mode with nothing under it */
    if (work->mode == MODE_SHOWN) {
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN) {
        collision_register(task);
        TURN(task) += emerald_look.spin;
    }
}

static void emerald_spread_draw(struct task *task)
{
    /* @hack spelling: task copied to a local, where task itself would do */
    struct task *piece = task;
    struct task_work *work = piece->work;

    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN) {
        emerald_draw_turned(&work->pos, TURN(piece), KIND(work).piece);
    }
}

/* the path calls, EMERALD_KIND_A_PATHMOVE's. a piece goes round a path of the
 * table emerald_set_paths gave. the middle of the box the path's points fill
 * lies at the work's pos. the work's ang.y and ang.z turn the path about that
 * middle */

/* the heading about y from the point from to the point to */
static Angle emerald_path_heading(NJS_POINT3 *from, NJS_POINT3 *to)
{
    /* @hack spelling: dx and dz, locals for the differences, where atan2f's
     * arguments written out would do */
    Float dx = from->x - to->x;
    Float dz = from->z - to->z;
    /* @hack spelling: the heading in a local, where returning the conversion
     * would do */
    Angle heading = (Angle)(atan2f(-dx, -dz) * ANGLE_PER_RADIAN);

    return heading;
}

/* move a piece travel along its path: forward for a travel above 0, else
 * back. the path is taken as closed, though its flags of 0 say it isn't.
 * *segment is the index of the point that starts the piece's segment.
 * *fraction is how far along that segment the piece lies, from 0 to 1. the
 * call moves both, and sets *out to the piece's new place.
 *
 * *heading becomes the segment's heading about y, turned toward the next
 * segment's. the turn is *fraction of the shorter turn between the two.
 *
 * returns whether the piece passed the path's first point. a path with flags
 * other than 0 returns FALSE and sets nothing */
static Bool emerald_path_advance(struct task_work *work, Sint32 *segment,
                                 Float *fraction, Float travel, NJS_POINT3 *out,
                                 Angle *heading)
{
    Bool wrapped = FALSE;
    NJS_POINT3 *points;
    /* @hack spelling: point, a local for &points[i], where points[i] read in
     * place would do */
    NJS_POINT3 *point;
    int i;
    int next;
    int after;
    /* @hack type: heading_here, heading_next and share, Sint16s, where Angles
     * would give the same turns */
    Sint16 heading_here;
    Sint16 heading_next;
    Sint16 share;
    Float length;
    Float part;
    Float room;
    Float weight_here;
    Float weight_next;
    Float turn;
    Float past;

    if (PATH(work)->flags == 0) {
        points = PATH_POINTS(work);
        if (travel > 0.0F) {
            i = *segment;
            part = *fraction;
            for (;;) {
                i %= PATH(work)->count;
                next = (i + 1) % PATH(work)->count;
                after = (i + 2) % PATH(work)->count;
                length = njDistanceP2P(&points[i], &points[next]);
                /* @hack spelling: the product stored in room before it is
                 * taken from length, where length - length * part would do */
                room = length * part;
                room = length - room;
                if (room < 0.0F) {
                    room = 0.0F;
                }
                if (travel > room) {
                    travel -= room;
                } else {
                    weight_here = (room - travel) / length;
                    weight_next = 1.0F - weight_here;
                    if (weight_next < 0.0F) {
                        weight_next = 0.0F;
                        weight_here = 1.0F;
                    }
                    *segment = i;
                    *fraction = weight_next;
                    point = &points[i];
                    out->x = (point->x * weight_here) +
                             (points[next].x * weight_next);
                    out->y = (point->y * weight_here) +
                             (points[next].y * weight_next);
                    out->z = (point->z * weight_here) +
                             (points[next].z * weight_next);
                    heading_here = emerald_path_heading(point, &points[next]);
                    heading_next =
                        emerald_path_heading(&points[next], &points[after]);
                    /* @hack spelling: turn set and then scaled, where one
                     * expression would do */
                    turn = (Sint16)(heading_next - heading_here);
                    turn *= weight_next;
                    share = (Sint16)turn;
                    *heading = share + heading_here;
                    return wrapped;
                }
                part = 0.0F;
                i++;
                if (i >= PATH(work)->count) {
                    wrapped = TRUE;
                }
            }
        } else {
            i = *segment;
            part = *fraction;
            for (;;) {
                i %= PATH(work)->count;
                next = (i + 1) % PATH(work)->count;
                after = (i + 2) % PATH(work)->count;
                length = njDistanceP2P(&points[i], &points[next]);
                room = length * part;
                /* @hack spelling: past, a local for length - room, where the
                 * test on length - room itself would do */
                past = length - room;
                if (past < 0.0F) {
                    room = length;
                }
                if (-travel > room) {
                    travel += room;
                } else {
                    room += travel;
                    /* @bug arithmetic: weight_next is 0 / 0 where a travel of
                     * 0 stops on a segment of length 0. njDistanceP2P gives
                     * that length for points whose distance squared is under
                     * 0.025. a travel below 0 passes such a segment, whose
                     * room is 0. the division is FDIV FR5,FR14 at 0x1750 of
                     * the object's code */
                    weight_next = room / length;
                    weight_here = 1.0F - weight_next;
                    if (weight_here < 0.0F) {
                        weight_next = 1.0F;
                        weight_here = 0.0F;
                    }
                    *segment = i;
                    *fraction = weight_next;
                    point = &points[i];
                    out->x = (point->x * weight_here) +
                             (points[next].x * weight_next);
                    out->y = (point->y * weight_here) +
                             (points[next].y * weight_next);
                    out->z = (point->z * weight_here) +
                             (points[next].z * weight_next);
                    heading_here = emerald_path_heading(point, &points[next]);
                    heading_next =
                        emerald_path_heading(&points[next], &points[after]);
                    /* @hack spelling: turn set and then scaled, where one
                     * expression would do */
                    turn = (Sint16)(heading_next - heading_here);
                    turn *= weight_next;
                    share = (Sint16)turn;
                    *heading = share + heading_here;
                    return wrapped;
                }
                i--;
                part = 1.0F;
                if (i < 0) {
                    i = PATH(work)->count - 1;
                    wrapped = TRUE;
                }
            }
        }
    }
    /* @bug uninitialized: emerald_path_init and emerald_path_step pass
     * njCalcPoint a pos that nothing set, on a path whose flags aren't 0. this
     * return, at 0x1812 of the object's code, leaves *out unset for their calls
     * at 0x1ce8 and 0x1e9a */
    return FALSE;
}

/* set out to the middle of the box that holds the points of task's path */
static void emerald_path_center(struct task *task, NJS_POINT3 *out)
{
    struct task_work *work = task->work;
    NJS_POINT3 *points = PATH_POINTS(work);
    Float max_x;
    Float max_y;
    Float max_z;
    Float min_x;
    Float min_y;
    Float min_z;
    Float x;
    Float y;
    Float z;
    int i;

#ifdef NONMATCHING
    /* the first point sets each bound, which GCC can't see. only a path of no
     * points reads these values */
    max_x = max_y = max_z = min_x = min_y = min_z = 0.0F;
#endif
    for (i = 0; i < PATH(work)->count; i++) {
        if (i == 0) {
            max_x = min_x = points[i].x;
            max_y = min_y = points[i].y;
            max_z = min_z = points[i].z;
        } else {
            /* @hack spelling: x, y and z, locals for the point's parts, where
             * points[i] read in place would do */
            x = points[i].x;
            y = points[i].y;
            z = points[i].z;
            if (max_x < x) {
                max_x = x;
            }
            if (max_y < y) {
                max_y = y;
            }
            if (max_z < z) {
                max_z = z;
            }
            if (min_x > x) {
                min_x = x;
            }
            if (min_y > y) {
                min_y = y;
            }
            if (min_z > z) {
                min_z = z;
            }
        }
    }
    out->x = (min_x + max_x) * 0.5F;
    out->y = (min_y + max_y) * 0.5F;
    out->z = (min_z + max_z) * 0.5F;
}

/* EMERALD_KIND_A_PATHMOVE's debug view, which only emerald_debug calls. it
 * moves the piece's segment and fraction on a step, as the piece's step does.
 * its label "<- MOVE SPD(+1)" says 1 is added to the speed, where
 * PATH_SPEED_BIAS is 3 */
static void emerald_path_debug(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 *points;
    NJS_POINT3 pos;
    NJS_POINT3 center;
    Angle heading;
    char text[256];
    /* @hack type: i, a Sint32, where an int would do */
    Sint32 i;

    if (emerald_path_count < 0) {
        return;
    }
    points = PATH_POINTS(work);
    njPrintC(NJM_LOCATION(17, 17), EMERALD_DEBUG_YAW);
    njPrintC(NJM_LOCATION(17, 18), EMERALD_DEBUG_ROLL);
    njPrintC(NJM_LOCATION(17, 19), EMERALD_DEBUG_PATH_ID);
    njPrintC(NJM_LOCATION(17, 21), EMERALD_DEBUG_SPEED);
    njPrintD(NJM_LOCATION(28, 19), (Uint32)work->scl.x % emerald_path_count, 2);
    emerald_path_advance(work, PATH_SEGMENT(task), PATH_FRACTION(task),
                         PATH_SPEED(work), &pos, &heading);
    emerald_path_center(task, &center);
    sprintf(text, EMERALD_DEBUG_CENTER, center.x, center.y, center.z);
    njPrintC(NJM_LOCATION(1, 24), text);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njTranslate(NULL, -center.x, -center.y, -center.z);
    for (i = 0; i < PATH(work)->count - 1; i++) {
        line_draw_points(&points[i], &points[i + 1]);
    }
    emerald_draw_mark_turned(&pos, TURN(task), KIND(work).piece);
    emerald_draw_radius(&pos, RADIUS(work));
    njPopMatrixEx();
}

/* EMERALD_KIND_A_PATHMOVE's unk_10, which nothing calls. it sets the piece's
 * segment and fraction to 0, and its extra to null without freeing it */
static void emerald_path_reset(struct task *task)
{
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

/* set the piece up on its path, from the segment its placement's state holds.
 * without a table of paths, or where syCalloc fails, the piece gets no extra.
 * its step and draw then do nothing */
static void emerald_path_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack redundant: a local nothing uses */
    unsigned char unused[12];
    Angle heading;

    if (emerald_path_count < 0) {
        return;
    }
    task->extra.p = syCalloc(1, sizeof(struct emerald_path_place));
    if (task->extra.p == NULL) {
        return;
    }
    if (task->placement != NULL) {
        *PATH_SEGMENT(task) = placement_get_state(task);
    }
    emerald_path_advance(work, PATH_SEGMENT(task), PATH_FRACTION(task),
                         PATH_SPEED(work), &pos, &heading);
    emerald_path_center(task, &PATH_PLACE(task)->center);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njTranslate(NULL, -PATH_PLACE(task)->center.x, -PATH_PLACE(task)->center.y,
                -PATH_PLACE(task)->center.z);
    njCalcPoint(NULL, &pos, &PATH_PLACE(task)->pos);
    njPopMatrixEx();
    hunt_add_place(KIND(work).kind, ID(work), &PATH_PLACE(task)->pos);
    if (work->mode == MODE_PLACED && placement_try_leave(task)) {
        return;
    }
    collision_init(task, emerald_path_shape, 1, COLLISION_PICKUP);
    work->collision->shapes->radius = RADIUS(work);
    work->collision->shapes->center = PATH_PLACE(task)->pos;
    collision_fit_record_radius(work->collision);
}

/* keep the piece's segment in its placement's state, and free its extra */
static void emerald_path_destroy(struct task *task)
{
    if (task->placement != NULL) {
        placement_set_state(task, *PATH_SEGMENT(task));
    }
    if (task->extra.p != NULL) {
        syFree(task->extra.p);
    }
    /* @hack redundant: extra cleared here and again below */
    task->extra.p = NULL;
    task->motion.p = NULL;
    task->force.p = NULL;
    task->extra.p = NULL;
}

/* emerald_plain_step for a piece on a path. the piece moves on each step while
 * it isn't PIECE_UNSOUGHT, hidden or not. it tells the hunt its place on each
 * step while it is drawn */
static void emerald_path_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    Angle heading;
    int player;

    if (work->mode == MODE_PLACED && placement_try_leave(task)) {
        return;
    }
    if (task->extra.p == NULL) {
        return;
    }
    if (work->submode != PIECE_UNSOUGHT) {
        emerald_path_advance(work, PATH_SEGMENT(task), PATH_FRACTION(task),
                             PATH_SPEED(work), &pos, &heading);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, work->ang.z);
        njTranslate(NULL, -PATH_PLACE(task)->center.x,
                    -PATH_PLACE(task)->center.y, -PATH_PLACE(task)->center.z);
        njCalcPoint(NULL, &pos, &PATH_PLACE(task)->pos);
        njPopMatrixEx();
    }
    if (work->submode <= PIECE_ASKING) {
        if (work->submode == PIECE_ASKING) {
            switch (hunt_find_piece(KIND(work).kind, ID(work), NULL)) {
            case HUNT_NOT_FOUND:
                work->submode = PIECE_UNSOUGHT;
                break;
            case HUNT_FOUND(HUNT_SLOT_C):
            case HUNT_FOUND(HUNT_SLOT_B):
            case HUNT_FOUND(HUNT_SLOT_A):
                work->submode = PIECE_SOUGHT;
                break;
            }
        } else {
            work->submode = PIECE_ASKING;
        }
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN &&
        (work->collision->flags & COLLISION_TOUCHED)) {
        if (!(work->collision->hit_shape != 0 &&
              stage_current_id() == STAGE_EGG_QUARTERS &&
              KIND(work).kind == EMERALD_KIND_FINAL) &&
            (work->collision->hit->kind == COLLISION_PLAYER ||
             work->collision->hit->kind == COLLISION_ATTACK) &&
            emerald_can_take(KIND(work).kind,
                             player =
                                 player_slot_of(work->collision->hit->owner))) {
            /* @hack redundant: a second player_slot_of call for player, which
             * the test has set from the first */
            player = player_slot_of(work->collision->hit->owner);
            hunt_take_piece(KIND(work).kind, ID(work), player,
                            &PATH_PLACE(task)->pos);
            if (task->placement != NULL) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            return;
        }
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN) {
        work->collision->shapes->radius = RADIUS(work);
        work->collision->shapes->center = PATH_PLACE(task)->pos;
        collision_fit_record_radius(work->collision);
        hunt_find_piece(KIND(work).kind, ID(work), &PATH_PLACE(task)->pos);
        collision_register(task);
        TURN(task) += emerald_look.spin;
    }
}

static void emerald_path_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (task->extra.p != NULL && work->submode == PIECE_SOUGHT &&
        work->mode != MODE_HIDDEN) {
        emerald_draw_turned(&PATH_PLACE(task)->pos, TURN(task),
                            KIND(work).piece);
    }
}

/* draw piece's glow model, where the look has one, added to what lies under it.
 * the call puts back Ninja's constant attributes and control flags. it leaves
 * Ninja's constant material at 0 */
static void emerald_draw_glow(int piece)
{
    int phase;
    Float level;

    if (emerald_look.glow_models[piece] != NULL) {
        phase = game_state.steps % emerald_glow_period;
        if (phase < emerald_glow_rise) {
            level = (Float)phase / (emerald_glow_rise - 1);
        } else {
            level = (Float)(emerald_glow_period - phase) /
                    (emerald_glow_period - emerald_glow_rise - 1);
        }
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(GLOW_CONTROL);
        njSetConstantAttr(CHUNK_TYPE_BITS,
                          NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
        njSetCnkBlendMode(NJD_FBS_ONE | NJD_FBD_ONE);
        constant_material_set(level, level, level, level);
        njCnkSimpleDrawModel(emerald_look.glow_models[piece]);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
}

/* the enemy calls, EMERALD_KIND_INENEMY's */
static void emerald_enemy_init(struct task *task)
{
    struct task_work *work = task->work;

    collision_init(task, emerald_shape, 1, COLLISION_PICKUP);
    work->collision->shapes->radius = ENEMY_RADIUS;
    work->collision->radius = ENEMY_RADIUS;
    switch (hunt_find_piece(EMERALD_KIND_INENEMY, ID(work), NULL)) {
    case HUNT_FOUND(HUNT_SLOT_C):
        work->submode = EMERALD_PIECE_C;
        break;
    case HUNT_FOUND(HUNT_SLOT_B):
        work->submode = EMERALD_PIECE_B;
        break;
    default:
        task_kill(task);
        break;
    }
}

/* a player who touches the piece takes it, whatever the piece's mode */
static void emerald_enemy_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *hit;
    int player;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((hit = collision_next_player(task)) != NULL) {
        /* @hack spelling: player in a local, where the call as the argument
         * would do */
        player = player_slot_of(hit);
        hunt_take_piece(EMERALD_KIND_INENEMY, ID(work), player, &work->pos);
        task_kill(task);
    } else {
        collision_register(task);
        TURN(task) += emerald_look.spin;
    }
}

/* @hack redundant: register on task, where none would do */
static void emerald_enemy_draw(register struct task *task)
{
    /* @hack redundant: register on work, where none would do */
    register struct task_work *work = task->work;

    emerald_draw_turned(&work->pos, TURN(task), work->submode);
}

static void emerald_enemy_destroy(struct task *task)
{
}

/* EMERALD_KIND_FINAL's draw: emerald_plain_draw, but the piece is drawn as
 * the piece of the hunt slot its id names */
static void emerald_final_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: parent_ang, a local for the parent's angles, where
     * task->parent->work->ang read in place would do */
    struct angles *parent_ang;
    struct angles ang;
    /* @hack type: piece, a Uint32, where the int the draws take would do */
    Uint32 piece;

    switch ((Uint8)work->ang.x) {
    case HUNT_SLOT_C:
        piece = EMERALD_PIECE_C;
        break;
    case HUNT_SLOT_B:
        piece = EMERALD_PIECE_B;
        break;
    /* @hack redundant: case HUNT_SLOT_A, which default takes anyway */
    case HUNT_SLOT_A:
    default:
        piece = EMERALD_PIECE_A;
        break;
    }
    if (work->submode == PIECE_SOUGHT && work->mode != MODE_HIDDEN) {
        if (work->mode == MODE_SHOWN && task->parent != NULL &&
            work->tick == EMERALD_TICK_PARENT_ANGLES) {
            parent_ang = &task->parent->work->ang;
            ang.x = parent_ang->x;
            ang.y = TURN(task);
            ang.z = parent_ang->z;
            emerald_draw(&work->pos, &ang, piece);
        } else {
            emerald_draw_turned(&work->pos, TURN(task), piece);
        }
    }
}
