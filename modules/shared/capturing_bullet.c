/* the capturing bullet, a shot an enemy fires. it holds the player it
 * catches, then breaks into pieces */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/capturing_bullet.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a bullet's modes, in its work's mode */
enum capturing_bullet_mode {
    MODE_FLY,
    MODE_HOLD,
    MODE_BURST, /* shrinks and fades where it burst */
    MODE_DONE /* ended: its task is killed in the same step */
};

#define BULLET_LEVEL 3
#define TEXTURE_LEVEL 0

/* the slot of the player a held bullet holds */
#define CAUGHT_PLAYER(work) ((work)->submode)
/* a held bullet's scale in x, and a burst's in x, y and z */
#define SIZE(work) ((work)->scl.x)
/* the alpha a burst sets the constant material to, from 0 down to BURST_END */
#define BURST_ALPHA(work) ((work)->scl.y)
/* the angle of a held bullet's wobble. enemy.h leaves the field to each kind
 * of enemy */
#define WOBBLE(enemy) ((enemy)->unk_018)

/* a bullet is armed once its work's timer passes ARMED_STEPS */
#define ARMED_STEPS 5
/* how far a flying bullet turns about z in a step */
#define SPIN 0x1400
/* how far a bullet moves along its speed's direction as it bursts */
#define BURST_AHEAD 3.0F
/* how far each step of a burst moves SIZE toward 0 and BURST_ALPHA toward
 * BURST_END. the burst ends once BURST_ALPHA reaches BURST_END */
#define BURST_STEP 0.02F
#define BURST_END (-1.0F)

/* a hold's scale in x starts at HOLD_WIDTH, or at HOLD_WIDTH_MECH about a
 * mech. its scale in y is HOLD_HEIGHT */
#define HOLD_WIDTH 1.0F
#define HOLD_WIDTH_MECH 3.0F
#define HOLD_HEIGHT 2.0F
/* the shake a catch gives the caught player's pad */
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7

#define WOBBLE_SIZE 0.02F /* the most a hold's wobble moves SIZE in a step */
#define WOBBLE_STEP 0x400 /* how far WOBBLE turns in a step of a hold */

#define COLLISION_RADIUS 40.0F
/* how far a bullet's probe of the stage reaches from its point, in each
 * direction */
#define PROBE_SIZE 3.0F

#define FIRE_SOUND 0x4000
#define HOLD_SOUND 0x4007
/* the fire sound's volume, and the hold sound's volume and its duration, as
 * sound_play_at_for takes it */
#define FIRE_VOLUME 15
#define HOLD_VOLUME 10
#define HOLD_SOUND_DURATION 2

/* the pieces a bullet breaks into. their speeds fan out about y, an eighth
 * of a turn apart */
#define PIECES 8
/* a piece's speed in x and y before the fan turns it, and its scale.
 * RANDOM_FROM gives each from its value up to its value plus its _SPREAD */
#define PIECE_SPD_X 0.4F
#define PIECE_SPD_X_SPREAD 0.6F
#define PIECE_SPD_Y (-0.6F)
#define PIECE_SPD_Y_SPREAD 0.4F
#define PIECE_SCALE 0.8F
#define PIECE_SCALE_SPREAD 0.4F
/* the share of world_gravity that each step adds to a piece's speed */
#define PIECE_GRAVITY 0.04F
/* how far each step moves a piece's scale toward 0, and its alpha toward
 * PIECE_END, in that order. the piece ends once its alpha reaches PIECE_END */
#define PIECE_SHRINK 0.025F
#define PIECE_FADE 0.05F
#define PIECE_END (-1.0F)

/* a random Float from least up to, but not including, least + spread */
#define RANDOM_FROM(least, spread)                                             \
    ((rand() * ((spread) * RAND_SCALE)) + (least))

/* a piece of a broken bullet, which its task's extra points to */
struct capturing_bullet_piece {
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float scale;
    /* the alpha it sets the constant material to, from 1 down to PIECE_END */
    Float alpha;
};

#include "shared/capturing_bullet/text.inc"

static NJS_TEXNAME capturing_bullet_texname[1] = {
#include "shared/capturing_bullet/capturing_bullet_texname.inc"
};
static NJS_TEXLIST capturing_bullet_texlist = {
#include "shared/capturing_bullet/capturing_bullet_texlist.inc"
};

/* the model a flying bullet draws, a chunk model of FLY_POINTS points, each
 * with its color */
#define FLY_POINTS 15
static Sint16 capturing_bullet_fly_polygons[149] = {
#include "shared/capturing_bullet/capturing_bullet_fly_polygons.inc"
};
static Sint32 capturing_bullet_fly_vertices[63] = {
#include "shared/capturing_bullet/capturing_bullet_fly_vertices.inc"
};
static NJS_CNK_MODEL capturing_bullet_fly_model = {
#include "shared/capturing_bullet/capturing_bullet_fly_model.inc"
};
static NJS_CNK_OBJECT capturing_bullet_fly_object = {
#include "shared/capturing_bullet/capturing_bullet_fly_object.inc"
};

/* the model a burst and the pieces draw, a chunk model of BURST_POINTS
 * points, each with its normal */
#define BURST_POINTS 32
static Sint16 capturing_bullet_burst_polygons[291] = {
#include "shared/capturing_bullet/capturing_bullet_burst_polygons.inc"
};
static Sint32 capturing_bullet_burst_vertices[195] = {
#include "shared/capturing_bullet/capturing_bullet_burst_vertices.inc"
};
static NJS_CNK_MODEL capturing_bullet_burst_model = {
#include "shared/capturing_bullet/capturing_bullet_burst_model.inc"
};
static NJS_CNK_OBJECT capturing_bullet_burst_object = {
#include "shared/capturing_bullet/capturing_bullet_burst_object.inc"
};

/* the model a held bullet draws, a chunk model of HOLD_POINTS points, each
 * with its color */
#define HOLD_POINTS 42
static Sint16 capturing_bullet_hold_polygons[357] = {
#include "shared/capturing_bullet/capturing_bullet_hold_polygons.inc"
};
static Sint32 capturing_bullet_hold_vertices[171] = {
#include "shared/capturing_bullet/capturing_bullet_hold_vertices.inc"
};
static NJS_CNK_MODEL capturing_bullet_hold_model = {
#include "shared/capturing_bullet/capturing_bullet_hold_model.inc"
};
static NJS_CNK_OBJECT capturing_bullet_hold_object = {
#include "shared/capturing_bullet/capturing_bullet_hold_object.inc"
};

/* the shape hurts nothing, despite SHAPE_HURTS_PLAYERS. a touch hurts only
 * where its attack level is above the other shape's guard level. this shape's
 * attack level is 0, the lowest. nothing hurts the shape either, since it lacks
 * SHAPE_HURTABLE */
/* clang-format off */
static struct collision_shape capturing_bullet_shape = {
#include "shared/capturing_bullet/capturing_bullet_shape.inc"
};
/* clang-format on */
/* the task that holds the textures, or null while none does. the initializer
 * places the pointer in the object's data, where each image has it, not in its
 * bss */
static struct task *capturing_bullet_texture_task = NULL;

static void capturing_bullet_break(NJS_POINT3 *pos, struct angles *ang);

static void capturing_bullet_move(struct task_work *work,
                                  struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    njAddVector(&work->pos, &enemy->spd);
    enemy->unk_00c.x = 0.0F;
    enemy->unk_00c.y = 0.0F;
    enemy->unk_00c.z = 0.0F;
}

static void capturing_bullet_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR scl;

    /* @hack spelling: an if for each mode, in the order MODE_HOLD, MODE_BURST
     * and MODE_FLY, where a switch in the modes' order would do */
    if (work->mode == MODE_HOLD) {
        scl.x = SIZE(work);
        /* @hack spelling: scl.z set ahead of scl.y, where the order x, y, z
         * would do */
        scl.z = njInvertSqrt(SIZE(work));
        scl.y = HOLD_HEIGHT;
        njSetTexture(&capturing_bullet_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        njScaleEx(&scl);
        njCnkSimpleDrawModel(&capturing_bullet_hold_model);
        njPopMatrixEx();
    } else if (work->mode == MODE_BURST) {
        /* @hack spelling: one assignment to the three, in the order z, x, y,
         * where a statement for each would do */
        scl.z = scl.x = scl.y = SIZE(work);
        constant_material_set(BURST_ALPHA(work), 0.0F, 0.0F, 0.0F);
        njSetTexture(&capturing_bullet_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        njScaleEx(&scl);
        njCnkSimpleDrawModel(&capturing_bullet_burst_model);
        njPopMatrixEx();
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    } else if (work->mode == MODE_FLY) {
        njSetTexture(&capturing_bullet_texlist);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njRotateX(NULL, work->ang.x);
        njRotateZ(NULL, work->ang.z);
        njCnkSimpleDrawModel(&capturing_bullet_fly_model);
        njPopMatrixEx();
    }
}

static void capturing_bullet_check_hit(struct task_work *work,
                                       struct enemy_work *enemy)
{
    struct collision_work *collision = work->collision;
    NJS_VECTOR dir;
    Float width;
    /* @hack spelling: burst, a local set by the test of the flags and
     * compared with 1, where the test alone would do */
    int burst = 0;
    int player;

    if (collision->flags & COLLISION_TOUCHED) {
        player = player_slot_of(collision->hit->owner);
        if (player != -1) {
            player_request(player, PLAYER_REQUEST_CAPTURE, 0);
            work->mode = MODE_HOLD;
            CAUGHT_PLAYER(work) = player;
            if (collision->owner->parent != NULL) {
                collision->owner->parent->work->flags |=
                    CAPTURING_BULLET_CAUGHT;
            }
            work->pos = player_work[player]->collision->shapes->center;
#ifdef NONMATCHING
            /* GCC can't see that every path sets width: the caught player is
             * never PLAYER_EGGMAN, as capturing_bullet.h requires */
            width = HOLD_WIDTH;
#endif
            /* @hack spelling: the characters' cases out of the characters'
             * order, where that order would do */
            switch (player_records[player]->character) {
            case PLAYER_SONIC:
            case PLAYER_SHADOW:
            case PLAYER_KNUCKLES:
            case PLAYER_TAILS:
            case PLAYER_ROUGE:
                width = HOLD_WIDTH;
                break;
            case PLAYER_EGGMAN_MECH:
            case PLAYER_TAILS_MECH:
                width = HOLD_WIDTH_MECH;
                break;
            }
            SIZE(work) = width;
            work->ang = player_work[player]->ang;
            vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ, 0);
            return;
        }
    }
    if (work->flags & (WORK_UNK_1 | WORK_HURT | WORK_UNK_8)) {
        burst = 1;
    }
    if (burst == 1) {
        work->mode = MODE_BURST;
        SIZE(work) = 1.0F;
        BURST_ALPHA(work) = 0.0F;
        /* @hack spelling: work->collision read again, where collision would
         * do */
        work->collision->shapes->flags |= SHAPE_OFF;
        dir = enemy->spd;
        njUnitVector(&dir);
        work->pos.x += dir.x * BURST_AHEAD;
        work->pos.y += dir.y * BURST_AHEAD;
        work->pos.z += dir.z * BURST_AHEAD;
    }
}

static void capturing_bullet_fly(struct task_work *work,
                                 struct enemy_work *enemy)
{
    NJS_VECTOR dir;

    if (work->timer > ARMED_STEPS) {
        capturing_bullet_check_hit(work, enemy);
    }
    capturing_bullet_move(work, enemy);
    enemy_meet_stage(work, enemy);
    if (enemy->flags & (ENEMY_ON_FLOOR | ENEMY_AT_WALL)) {
        work->mode = MODE_BURST;
        SIZE(work) = 1.0F;
        BURST_ALPHA(work) = 0.0F;
        work->collision->shapes->flags |= SHAPE_OFF;
        dir = enemy->spd;
        njUnitVector(&dir);
        work->pos.x += dir.x * BURST_AHEAD;
        work->pos.y += dir.y * BURST_AHEAD;
        work->pos.z += dir.z * BURST_AHEAD;
    }
    if (!game_state.time_stopped) {
        work->ang.z += SPIN;
    }
}

static void capturing_bullet_hold(struct task_work *work,
                                  struct enemy_work *enemy)
{
    struct task_work *player_task_work;
    struct player *record;

    /* @bug memory: reads the caught player's work at 0x3fe of the object's
     * code, with no test for null. player_task_work, the same work, is
     * tested at 0x424 */
#ifdef NONMATCHING
    /* this side repairs the tag above: the bullet breaks once the caught
     * player's slot is empty, as once the player is free */
    if (player_work[CAUGHT_PLAYER(work)] == NULL ||
        player_work[CAUGHT_PLAYER(work)]->mode != PLAYER_MODE_CAUGHT) {
#else
    if (player_work[CAUGHT_PLAYER(work)]->mode != PLAYER_MODE_CAUGHT) {
#endif
        work->mode = MODE_DONE;
        capturing_bullet_break(&work->pos, &work->ang);
        return;
    }
    player_task_work = player_work[CAUGHT_PLAYER(work)];
    record = player_records[CAUGHT_PLAYER(work)];
    if (player_task_work != NULL) {
        work->pos = record->unk_230;
        work->ang = player_task_work->ang;
    }
    SIZE(work) += njSin(WOBBLE(enemy)) * WOBBLE_SIZE;
    WOBBLE(enemy) += WOBBLE_STEP;
}

static void capturing_bullet_fade(struct task_work *work,
                                  struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    SIZE(work) = float_approach(SIZE(work), 0.0F, BURST_STEP);
    BURST_ALPHA(work) =
        float_approach(BURST_ALPHA(work), BURST_END, BURST_STEP);
    if (BURST_ALPHA(work) <= BURST_END) {
        work->mode = MODE_DONE;
    }
}

static void capturing_bullet_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void capturing_bullet_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_FLY:
        capturing_bullet_fly(work, enemy);
        break;
    case MODE_HOLD:
        capturing_bullet_hold(work, enemy);
        if (work->mode != MODE_HOLD) {
            task_kill(task);
            return;
        }
        sound_play_at_for(HOLD_SOUND, work, 1, HOLD_VOLUME, HOLD_SOUND_DURATION,
                          &work->pos);
        break;
    case MODE_BURST:
        capturing_bullet_fade(work, enemy);
        if (work->mode != MODE_BURST) {
            task_kill(task);
            return;
        }
        break;
    }
    collision_register(task);
    if (!game_state.time_stopped) {
        work->timer++;
    }
}

static void capturing_bullet_init(struct task *task, NJS_VECTOR *spd)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    /* @hack redundant: unused, a local that only sizes the stack frame */
    NJS_VECTOR unused;

    collision_init(task, &capturing_bullet_shape, 1, COLLISION_ENEMY);
    /* @hack spelling: task->work read again, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, COLLISION_RADIUS);
    enemy = enemy_work_alloc(task);
    enemy->spd = *spd;
    /* the work's scale starts at 1. a hold sets SIZE, and a burst SIZE and
     * BURST_ALPHA, before reading them */
    work->scl.x = 1.0F;
    work->scl.y = 1.0F;
    work->ang.x =
        asinf(-enemy->spd.y / njScalor(&enemy->spd)) * ANGLE_PER_RADIAN;
    work->ang.y = atan2f(enemy->spd.x, enemy->spd.z) * ANGLE_PER_RADIAN;
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 0.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = PROBE_SIZE;
    enemy->radius = PROBE_SIZE;
    enemy->bottom = -PROBE_SIZE;
    enemy->wall_bounce = 0.0F;
    enemy->floor_bounce = 0.0F;
    enemy->unk_0e4 = 0.0F;
    enemy->wall_turn = 0;
    enemy_set_home(work, enemy);
    task->destructor = capturing_bullet_destroy;
    task->draw = capturing_bullet_draw;
}

static void capturing_bullet_texture_destroy(struct task *task)
{
    njReleaseTexture(&capturing_bullet_texlist);
    capturing_bullet_texture_task = NULL;
}

static void capturing_bullet_texture_step(struct task *task)
{
}

void capturing_bullet_load_texture(void)
{
    if (capturing_bullet_texture_task != NULL) {
        return;
    }
    texture_load_pvm("E_CAPTEX", &capturing_bullet_texlist);
    capturing_bullet_texture_task =
        task_create(0, TEXTURE_LEVEL, capturing_bullet_texture_step, "ManTex");
    capturing_bullet_texture_task->destructor =
        capturing_bullet_texture_destroy;
}

void capturing_bullet_fire(struct task *parent, NJS_VECTOR *spd,
                           NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, capturing_bullet_step, parent);
    } else {
        task = task_create(TASK_WORK, BULLET_LEVEL, capturing_bullet_step,
                           "EnemyCapturingBulletExecutor");
    }
    if (task == NULL) {
        return;
    }
    task->work->pos = *pos;
    capturing_bullet_init(task, spd);
    capturing_bullet_load_texture();
    sound_play_at(FIRE_SOUND, NULL, 0, FIRE_VOLUME, pos);
}

static void capturing_bullet_piece_draw(struct task *task)
{
    struct capturing_bullet_piece *piece = task->extra.p;
    NJS_VECTOR scl;

    /* @hack spelling: one assignment to the three, in the order z, x, y,
     * where a statement for each would do */
    scl.z = scl.x = scl.y = piece->scale;
    constant_material_set(piece->alpha, 0.0F, 0.0F, 0.0F);
    njSetTexture(&capturing_bullet_texlist);
    njPushMatrixEx();
    njTranslateEx(&piece->pos);
    njScaleEx(&scl);
    njCnkSimpleDrawModel(&capturing_bullet_burst_model);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

static void capturing_bullet_piece_step(struct task *task)
{
    struct capturing_bullet_piece *piece = task->extra.p;

    if (game_state.time_stopped) {
        return;
    }
    njAddVector(&piece->pos, &piece->spd);
    piece->spd.x += world_gravity.x * PIECE_GRAVITY;
    piece->spd.y += world_gravity.y * PIECE_GRAVITY;
    piece->spd.z += world_gravity.z * PIECE_GRAVITY;
    piece->scale = float_approach(piece->scale, 0.0F, PIECE_SHRINK);
    piece->alpha = float_approach(piece->alpha, PIECE_END, PIECE_FADE);
    if (piece->alpha <= PIECE_END) {
        task_kill(task);
    }
}

/* ang turns the pieces' fan of speeds. each piece starts at pos plus its
 * speed before any turn */
static void capturing_bullet_break(NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task;
    struct capturing_bullet_piece *piece;
    NJS_VECTOR spd;
    int i;

    njPushMatrixEx();
    njRotateY(NULL, ang->y);
    njRotateX(NULL, ang->x);
    njRotateZ(NULL, ang->z);
    for (i = 0; i < PIECES; i++) {
        /* @hack spelling: the task's assignment inside the test, where a
         * statement of its own would do */
        if ((task = task_create(0, BULLET_LEVEL, capturing_bullet_piece_step,
                                "EnemyCapturingBulletPiecesExecutor")) ==
            NULL) {
            break;
        }
        /* @hack spelling: the piece's assignment inside the test, where a
         * statement of its own would do */
        if ((piece = syCalloc(1, sizeof(struct capturing_bullet_piece))) ==
            NULL) {
            task_destroy(task);
            break;
        }
        task->extra.p = piece;
        piece->pos = *pos;
        njRotateY(NULL, EIGHTH_TURN);
        spd.x = RANDOM_FROM(PIECE_SPD_X, PIECE_SPD_X_SPREAD);
        spd.y = RANDOM_FROM(PIECE_SPD_Y, PIECE_SPD_Y_SPREAD);
        spd.z = 0.0F;
        njCalcVector(NULL, &spd, &piece->spd);
        njAddVector(&piece->pos, &spd);
        piece->scale = RANDOM_FROM(PIECE_SCALE, PIECE_SCALE_SPREAD);
        piece->alpha = 1.0F;
        task->draw = capturing_bullet_piece_draw;
    }
    njPopMatrixEx();
}
