/* the effects and effect textures of Sonic and Shadow, and of Amy and Metal
 * Sonic in their place */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/sonic_effect.h"
#include "effect/sonic_effect_assets.h"
#include "object/set.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* njFogDisable under a second name, which symbols.txt places at its address.
 * it is declared to take the record flame_ring_draw passes, which it doesn't
 * read */
void fog_disable_record(struct player *record);

/* the task levels of the effects, of the textures' tasks and of the Chaos
 * Control */
#define EFFECT_LEVEL 3
#define TEXTURES_LEVEL 0
#define CHAOS_CONTROL_LEVEL 0

/* the indices of S_EFFTEX's textures, and of SH_EFFTEX's. SH_EFFTEX's names
 * take sh_ for s_, and lcharge03 and lcharge04 for lcharge01 and lcharge02 */
#define TEX_BALL_B 1 /* s_ball_b, after s_ball_a */
#define TEX_DASH 2 /* s_dash01, the first of s_dash01 to s_dash23 */
#define TEX_TRAIL 25 /* lcharge01, a light's trail */
#define TEX_LIGHT 26 /* lcharge02, a light's head */
#define TEX_SPARK 28 /* fire_ss02, the first a flame spark shows */
/* cc_glight, a flash's glow. cc_particle0, cc_ring and ef_firesum follow it */
#define TEX_GLOW 35

/* the model_table slots of Sonic's and Shadow's ball models, dash streaks and
 * Chaos Control rings */
#define SONIC_BALL_MODEL 19
#define SHADOW_BALL_MODEL 84
#define SONIC_DASH_MODEL 20
#define SHADOW_DASH_MODEL 85
#define SONIC_CHAOS_MODEL 29
#define SHADOW_CHAOS_MODEL 102

/* a ball's, a dash's or a flash's record, in its task's extra */
struct model_effect {
    Sint8 mode; /* a dash's DASH_ set, or a flash's FLASH_ mode */
    Sint8 player; /* the player's slot */
    /* the steps a brief ball or a dash has taken, or a flash has left */
    Sint16 timer;
    /* a jitter or a follow ball's offset from the player's point, or the
     * effect's place */
    NJS_POINT3 pos;
    NJS_CNK_OBJECT *object; /* the model it draws, a dash's own copy */
    /* the one ball texture a ball draws with, or a flash's effect textures */
    NJS_TEXLIST texlist;
    Float scale;
};

/* a flame ring spark's record, in its task's extra */
struct flame_spark {
    Sint8 unk_00; /* 0, which nothing reads */
    Sint8 player;
    Sint16 timer;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    Float scale;
};

/* a light's trail holds TRAIL_PIECES pieces, laid PIECES_PER_STEP to a step */
#define TRAIL_PIECES 32
#define PIECES_PER_STEP 2
/* a light circles the player's middle. the middle lies halfway from the
 * player's pos to its first collision shape's center. a sparkle light's circle
 * starts at a radius of SPARKLE_RADIUS_MIN up to SPARKLE_RADIUS_SPAN more.
 * each step multiplies the radius by SPARKLE_CLOSE. a spin charge light's
 * starts at CHARGE_RADIUS_MIN up to CHARGE_RADIUS_SPAN more, and grows by
 * CHARGE_OPEN instead */
#define SPARKLE_RADIUS_MIN 25.0F
#define SPARKLE_RADIUS_SPAN 15.0F
#define SPARKLE_CLOSE 0.85F
#define CHARGE_RADIUS_MIN 3.0F
#define CHARGE_RADIUS_SPAN 1.5F
#define CHARGE_OPEN 1.01F
/* a circle's tilt starts at LIGHT_TILT_MIN up to LIGHT_TILT_SPAN more. its
 * heading starts anywhere. each step turns it by the light's turn,
 * LIGHT_TURN_MIN up to LIGHT_TURN_SPAN more */
#define LIGHT_TILT_MIN (-0x800)
#define LIGHT_TILT_SPAN 0x3000
#define LIGHT_TURN_MIN 0x400
#define LIGHT_TURN_SPAN 0x600
/* the second piece of each pair lies nearer the middle by the trail's width.
 * the first pair's is TRAIL_START_WIDTH, and the rest's SPARKLE_TRAIL_WIDTH or
 * CHARGE_TRAIL_WIDTH */
#define TRAIL_START_WIDTH 1.5F
#define SPARKLE_TRAIL_WIDTH 0.8F
#define CHARGE_TRAIL_WIDTH 0.4F
/* a spin charge light's middle rises against world_gravity by CHARGE_RISE for
 * each piece laid */
#define CHARGE_RISE 0.2F

/* a light's work. submode holds the player's slot, where player_slot_of
 * reads it for a WORK_ID_PLAYER_PROXY. scl.x holds the circle's radius, ang.x
 * its tilt, ang.y its heading and ang.z the heading's turn a step. tick
 * counts the pieces laid, and timer the pieces to lay, TRAIL_PIECES */
#define LIGHT_PLAYER(work) ((work)->submode)
#define LIGHT_RADIUS(work) ((work)->scl.x)
#define LIGHT_TURN(work) ((work)->ang.z)

/* the Chaos Control's record, in its task's extra */
struct chaos_control {
    Sint8 player;
    Uint8 unk_01;
    Sint16 timer; /* the steps left */
    /* the player's anim frame, rounded down, at the last draw at or past
     * CHAOS_CONTROL_START. nothing reads it */
    Float frame;
};

/* a jet's work, in place of its task's task_work. offset lies where
 * task_work's ang does, and life, life_step and scale where its scl does */
struct jet_work {
    Sint8 kind; /* JET_ */
    Sint8 player;
    Uint8 unk_02;
    Uint8 texture; /* the jet draws texture JET_FIRST_TEXTURE + texture */
    Sint16 unk_04;
    Uint16 timer; /* the steps that faded it. nothing reads it */
    NJS_VECTOR offset; /* from the player's unk_230 */
    NJS_POINT3 pos;
    /* from 1 down to 0, the jet's brightness and opacity */
    Float life;
    Float life_step; /* what each step that fades the jet takes from life */
    Float scale;
};
#define JET(task) ((struct jet_work *)(task)->work)

/* a vertex of a dash's streak: a point, and a word the dash leaves alone */
struct streak_vertex {
    NJS_POINT3 pos;
    Uint32 unk_0c;
};
/* a streak's vertex list, in words: one vertex chunk, then NJD_CE, the
 * list's end. the chunk's head is HEAD_WORDS long. its word COUNT_WORD holds
 * the count of vertices in its high halfword, COUNT_MASK, from bit
 * COUNT_SHIFT. OTHER_WORDS counts the head and the NJD_CE */
#define HEAD_WORDS 2
#define COUNT_WORD 1
#define COUNT_MASK 0xffff0000
#define COUNT_SHIFT 16
#define VERTEX_WORDS (sizeof(struct streak_vertex) / sizeof(Sint32))
#define OTHER_WORDS (HEAD_WORDS + 1)

/* Each step of a Sonic or Shadow dash draws with its own texture view. */
#define DASH_STEPS 20
#define DASH_SECOND_STEPS 19
static NJS_TEXLIST s_dash_textures[DASH_STEPS] = {
#include "effect/sonic_effect/s_dash_textures.inc"
};
static NJS_TEXLIST s_dash_second_textures[DASH_SECOND_STEPS] = {
#include "effect/sonic_effect/s_dash_second_textures.inc"
};
static NJS_TEXLIST sh_dash_textures[DASH_STEPS] = {
#include "effect/sonic_effect/sh_dash_textures.inc"
};
static NJS_TEXLIST sh_dash_second_textures[DASH_SECOND_STEPS] = {
#include "effect/sonic_effect/sh_dash_second_textures.inc"
};

/* the task that holds each effect texture list loaded, or null while the
 * list isn't */
static struct task *s_efftex_task = NULL;
static struct task *sh_efftex_task = NULL;

/* a spark's sprite: the whole of a 32 by 32 texture about its middle. its
 * draw sets the list, the texture, the place and the scale */
/* @hack type: arrays of one, where objects would do */
static NJS_TEXANIM flame_spark_cell[1] = {
#include "effect/sonic_effect/flame_spark_cell.inc"
};
static NJS_SPRITE flame_spark_sprite[1] = {
#include "effect/sonic_effect/flame_spark_sprite.inc"
};

/* a light's head: a 32 by 32 cell of TEX_LIGHT about its middle. each kind of
 * light has a sprite of it, whose list and place its draws set */
static NJS_TEXANIM light_cell = {
#include "effect/sonic_effect/light_cell.inc"
};
/* @hack type: arrays of one, where objects would do */
static NJS_SPRITE sparkle_light_head[1] = {
#include "effect/sonic_effect/sparkle_light_head.inc"
};
static NJS_SPRITE spin_charge_light_head[1] = {
#include "effect/sonic_effect/spin_charge_light_head.inc"
};

/* clang-format off */
static struct collision_shape sparkle_light_shape = {
    0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
    SHAPE_ATTACK_LEVEL(1) | SHAPE_GUARD_LEVEL(3) | SHAPE_HURTS_ENEMIES,
    SHAPE_UNTURNED | SHAPE_HURTABLE, {0.0F, 0.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0,
    {0, 0, 0},
};
/* clang-format on */

/* a half-gray quad over the whole screen, which the Chaos Control draws */
static NJS_POLYGON_VTX chaos_control_screen[4] = {
#include "effect/sonic_effect/chaos_control_screen.inc"
};

/* the glow of a Chaos Control's flash, and a jet's flame, each a square
 * about its middle. a jet's draw sets the flame's color */
#define QUAD_VERTICES 4
static NJS_TEXTURE_VTX flash_glow[QUAD_VERTICES] = {
#include "effect/sonic_effect/flash_glow.inc"
};
static NJS_TEXTURE_VTX jet_flame[QUAD_VERTICES] = {
#include "effect/sonic_effect/jet_flame.inc"
};

static void s_efftex_destroy(struct task *task)
{
    njReleaseTexture(&s_efftex_texlist);
    s_efftex_task = NULL;
}

static void sh_efftex_destroy(struct task *task)
{
    njReleaseTexture(&sh_efftex_texlist);
    sh_efftex_task = NULL;
}

static void efftex_hold(struct task *task)
{
}

#ifdef NONMATCHING
/* the textures' holder task, or null where task_create fails or the textures
 * don't load. the matching loads write the destructor through the null
 * task_create can give */
static struct task *efftex_load(const char *name, NJS_TEXLIST *textures,
                                void (*destroy)(struct task *))
{
    struct task *holder;

    holder = task_create(0, TEXTURES_LEVEL, efftex_hold, "ManTex");
    if (holder == NULL) {
        return NULL;
    }
    if (!texture_load_pvm(name, textures)) {
        task_destroy(holder);
        return NULL;
    }
    holder->destructor = destroy;
    return holder;
}
#endif

void s_efftex_load(void)
{
    if (s_efftex_task == NULL) {
#ifdef NONMATCHING
        /* this side leaves the holder null where the load fails, so a later
         * call loads again */
        s_efftex_task =
            efftex_load("S_EFFTEX", &s_efftex_texlist, s_efftex_destroy);
#else
        texture_load_pvm("S_EFFTEX", &s_efftex_texlist);
        s_efftex_task = task_create(0, TEXTURES_LEVEL, efftex_hold, "ManTex");
        s_efftex_task->destructor = s_efftex_destroy;
#endif
    }
}

void sh_efftex_load(void)
{
    if (sh_efftex_task == NULL) {
#ifdef NONMATCHING
        /* this side leaves the holder null where the load fails, so a later
         * call loads again */
        sh_efftex_task =
            efftex_load("SH_EFFTEX", &sh_efftex_texlist, sh_efftex_destroy);
#else
        texture_load_pvm("SH_EFFTEX", &sh_efftex_texlist);
        sh_efftex_task = task_create(0, TEXTURES_LEVEL, efftex_hold, "ManTex");
        sh_efftex_task->destructor = sh_efftex_destroy;
#endif
    }
}

void amy_efftex_load(void)
{
    if (s_efftex_task == NULL) {
#ifdef NONMATCHING
        /* this side leaves the holder null where the load fails, so a later
         * call loads again */
        s_efftex_task =
            efftex_load("AMY_EFFTEX", &s_efftex_texlist, s_efftex_destroy);
#else
        texture_load_pvm("AMY_EFFTEX", &s_efftex_texlist);
        s_efftex_task = task_create(0, TEXTURES_LEVEL, efftex_hold, "ManTex");
        s_efftex_task->destructor = s_efftex_destroy;
#endif
    }
}

/* a task named exec that runs step, with an uncleared struct model_effect for
 * its extra. the caller fills the record. it returns null where the heap can't
 * give both */
static struct task *model_effect_task_create(void (*step)(struct task *task))
{
    struct task *task;
    struct model_effect *effect;

    task = task_create(0, EFFECT_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((effect = syMalloc(sizeof(struct model_effect))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = effect;
    }
    return task;
}

/* model_effect_task_create with a struct flame_spark */
static struct task *flame_spark_task_create(void (*step)(struct task *task))
{
    struct task *task;
    struct flame_spark *spark;

    task = task_create(0, EFFECT_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((spark = syMalloc(sizeof(struct flame_spark))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = spark;
    }
    return task;
}

/* model_effect_task_create with a work, and room for TRAIL_PIECES pieces of a
 * trail in its extra */
static struct task *trail_task_create(void (*step)(struct task *task))
{
    struct task *task;
    NJS_TEXTURE_VTX *pieces;

    task = task_create(TASK_WORK, EFFECT_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((pieces = syMalloc(TRAIL_PIECES * sizeof(NJS_TEXTURE_VTX))) ==
            NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = pieces;
    }
    return task;
}

/* set *pos to the center of the player's first collision shape. *pos stays
 * as it was where the slot has no work, or the work no collision record */
static void player_collision_center(int player, NJS_POINT3 *pos)
{
    struct task_work *work = player_work[player];

    if (work != NULL && work->collision != NULL) {
        *pos = work->collision->shapes[0].center;
    }
}

/* set *pos to the point the player's effects follow. it is unk_218 of a mech's
 * record, and unk_230 of any other's. the slot must hold a record, unchecked */
static void effect_player_point(int player, NJS_POINT3 *pos)
{
    struct player *record = player_records[player];

    /* @hack redundant: the arms for PLAYER_SONIC, PLAYER_SHADOW and
     * PLAYER_KNUCKLES, which the default arm takes too */
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
    case PLAYER_TAILS_MECH:
        *pos = record->unk_218;
        break;
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    default:
        *pos = record->unk_230;
        break;
    }
}

/* each ball's scale at its start, and what each step takes from it. a
 * follow ball takes JITTER_STEP too. a brief ball's step kills it once its
 * timer passes BRIEF_LAST */
#define JITTER_SCALE 1.4F
#define FOLLOW_SCALE 1.2F
#define SHRINK_SCALE 1.2F
#define LARGE_SCALE 3.0F
#define JITTER_STEP 0.2F
#define SHRINK_STEP 0.1F
#define BRIEF_STEP 0.3F
#define LARGE_STEP 0.3F
#define BRIEF_LAST 2
/* a jitter ball lies JITTER_REACH_MIN up to 1 more from the player's point. a
 * follow ball lies FOLLOW_REACH_MIN up to FOLLOW_REACH_SPAN more */
#define JITTER_REACH_MIN 0.2F
#define FOLLOW_REACH_MIN 1.0F
#define FOLLOW_REACH_SPAN 2.0F
/* a ball draws with TEX_BALL_B where rand() * RAND_SCALE is BALL_B_SHARE or
 * less. it draws with the first ball texture otherwise */
#define BALL_B_SHARE 0.5F
/* a jitter ball's model lies BALL_DROP below its place, and is stretched by
 * BALL_STRETCH_X, _Y and _Z */
#define BALL_DROP 1.0F
#define BALL_STRETCH_X 0.7F
#define BALL_STRETCH_Y 1.1F
#define BALL_STRETCH_Z 0.8F

/* give ball the character's ball model, and one of its two ball textures at
 * random */
#define BALL_SET_MODEL(ball)                                                   \
    do {                                                                       \
        (ball)->texlist.nbTexture = 1;                                         \
        if (player_records[(ball)->player]->character == PLAYER_SONIC) {       \
            (ball)->object = model_table[SONIC_BALL_MODEL].object;             \
            if (rand() * RAND_SCALE > BALL_B_SHARE) {                          \
                (ball)->texlist.textures = s_efftex_texlist.textures;          \
            } else {                                                           \
                (ball)->texlist.textures =                                     \
                    &s_efftex_texlist.textures[TEX_BALL_B];                    \
            }                                                                  \
        } else {                                                               \
            (ball)->object = model_table[SHADOW_BALL_MODEL].object;            \
            if (rand() * RAND_SCALE > BALL_B_SHARE) {                          \
                (ball)->texlist.textures = sh_efftex_texlist.textures;         \
            } else {                                                           \
                (ball)->texlist.textures =                                     \
                    &sh_efftex_texlist.textures[TEX_BALL_B];                   \
            }                                                                  \
        }                                                                      \
    } while (0)

static void ball_jitter_draw(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    struct task_work *work = player_work[ball->player];
    NJS_VECTOR pos;

    /* @bug memory: reads the player's work unchecked, which
     * ball_jitter_set_up tests for null at 0x8c075a9a. the turns read it at
     * 0x8c0759c2 */
#ifdef NONMATCHING
    /* this side repairs the tag above: it draws nothing where the slot has no
     * work */
    if (work == NULL) {
        return;
    }
#endif
    njFogDisable();
    njSetTexture(&ball->texlist);
    effect_player_point(ball->player, &pos);
    njAddVector(&pos, &ball->pos);
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, -work->ang.y);
    pos.x = 0.0F;
    pos.y = -BALL_DROP;
    pos.z = 0.0F;
    njTranslateEx(&pos);
    njRotateZ(NULL, -EIGHTH_TURN);
    /* @hack spelling: the scale set ahead of the half turn, not next to its
     * use */
    pos.x = BALL_STRETCH_X;
    pos.y = BALL_STRETCH_Y;
    pos.z = BALL_STRETCH_Z;
    njRotateY(NULL, HALF_TURN);
    njScaleEx(&pos);
    njScale(NULL, ball->scale, ball->scale, ball->scale);
    njCnkEasyDrawObject(ball->object);
    njPopMatrixEx();
    njFogEnable();
}

static void ball_jitter_step(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    ball->scale -= JITTER_STEP;
    if (ball->scale <= 0.0F) {
        task_kill(task);
        task->draw = NULL;
    }
}

static void ball_jitter_set_up(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct player *record;
    struct task_work *work;
    NJS_VECTOR offset;

    /* @hack redundant: the record read beside the work, which nothing
     * uses */
    /* @hack spelling: the record and the work read in one statement, where
     * two would do */
    /* @hack spelling: the work read from the table's first element, where
     * the table would do */
    record = player_records[ball->player],
    work = (&player_work[0])[ball->player];
    if (work != NULL) {
        /* @hack spelling: offset's x and z set in one statement, where two
         * would do */
        offset.x = offset.z = 0.0F;
        offset.y = (rand() * RAND_SCALE) + JITTER_REACH_MIN;
        matrix_push_unit();
        njRotateZ(NULL, RANDOM_TURN());
        njRotateX(NULL, RANDOM_TURN());
        njRotateY(NULL, RANDOM_TURN());
        njCalcVector(NULL, &offset, &ball->pos);
        njPopMatrixEx();
    }
    ball->scale = JITTER_SCALE;
    BALL_SET_MODEL(ball);
    task->draw = ball_jitter_draw;
}

void ball_jitter_create(int player)
{
    struct task *task;
    struct model_effect *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(ball_jitter_step)) != NULL) {
        ball = task->extra.p;
        ball->player = player;
        ball_jitter_set_up(task);
    }
}

static void ball_follow_draw(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* @hack spelling: pos declared ahead of ang, not in the order the code
     * first uses them */
    NJS_VECTOR pos;
    struct angles ang;

    if (player_get_past(ball->player, 1, NULL, &ang)) {
        njFogDisable();
        effect_player_point(ball->player, &pos);
        njAddVector(&pos, &ball->pos);
        njSetTexture(&ball->texlist);
        njPushMatrixEx();
        njTranslateEx(&pos);
        njRotateZ(NULL, ang.z);
        njRotateX(NULL, ang.x);
        njRotateY(NULL, HALF_TURN - ang.y);
        njScale(NULL, ball->scale, ball->scale, ball->scale);
        njCnkEasyDrawObject(ball->object);
        njPopMatrixEx();
        njFogEnable();
    }
}

static void ball_follow_step(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    ball->scale -= JITTER_STEP;
    if (ball->scale <= 0.0F) {
        task_kill(task);
        task->draw = NULL;
    }
}

static void ball_follow_set_up(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct player *record;
    struct task_work *work;
    NJS_VECTOR offset;

    /* @hack redundant: the record read ahead of the work, which nothing
     * uses */
    record = player_records[ball->player];
    work = player_work[ball->player];
    if (work != NULL) {
        /* @hack spelling: offset's x and z set in one statement, where two
         * would do */
        offset.x = offset.z = 0.0F;
        offset.y =
            (rand() * (FOLLOW_REACH_SPAN * RAND_SCALE)) + FOLLOW_REACH_MIN;
        matrix_push_unit();
        njRotateZ(NULL, RANDOM_TURN());
        njRotateX(NULL, RANDOM_TURN());
        njRotateY(NULL, work->ang.y);
        njCalcVector(NULL, &offset, &ball->pos);
        njPopMatrixEx();
    }
    ball->scale = FOLLOW_SCALE;
    BALL_SET_MODEL(ball);
    task->draw = ball_follow_draw;
}

void ball_follow_create(int player)
{
    struct task *task;
    struct model_effect *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(ball_follow_step)) != NULL) {
        ball = task->extra.p;
        ball->player = player;
        ball_follow_set_up(task);
    }
}

static void ball_shrink_draw(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* @hack redundant: a vector nothing uses */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    struct angles ang;

    if (player_get_past(ball->player, 1, NULL, &ang)) {
        njFogDisable();
        njSetTexture(&ball->texlist);
        njPushMatrixEx();
        njTranslateEx(&ball->pos);
        njScale(NULL, ball->scale, ball->scale, ball->scale);
        njCnkEasyDrawObject(ball->object);
        njPopMatrixEx();
        njFogEnable();
    }
}

static void ball_shrink_step(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    ball->scale -= SHRINK_STEP;
    if (ball->scale <= 0.0F) {
        task_kill(task);
        task->draw = NULL;
    }
}

static void ball_brief_step(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    if (ball->timer > 0) {
        ball->scale -= BRIEF_STEP;
    }
    /* @hack spelling: the timer counted inside the test, where a statement
     * of its own would do */
    if (ball->timer++ > BRIEF_LAST) {
        task_kill(task);
        task->draw = NULL;
    }
}

static void ball_shrink_set_up(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* @hack redundant: the record read here, which nothing uses */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct player *record = player_tasks[ball->player]->extra.p;

    player_collision_center(ball->player, &ball->pos);
    ball->scale = SHRINK_SCALE;
    BALL_SET_MODEL(ball);
    task->draw = ball_shrink_draw;
}

void ball_shrink_create(int player)
{
    struct task *task;
    struct model_effect *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(ball_shrink_step)) != NULL) {
        ball = task->extra.p;
        ball->player = player;
        ball_shrink_set_up(task);
    }
}

void ball_brief_create(int player)
{
    struct task *task;
    struct model_effect *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(ball_brief_step)) != NULL) {
        ball = task->extra.p;
        ball->player = player;
        ball_shrink_set_up(task);
        ball->timer = 0;
    }
}

static void ball_large_draw(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    njFogDisable();
    njSetTexture(&ball->texlist);
    njPushMatrixEx();
    njTranslateEx(&ball->pos);
    njScale(NULL, ball->scale, ball->scale, ball->scale);
    njCnkEasyDrawObject(ball->object);
    njPopMatrixEx();
    njFogEnable();
}

static void ball_large_step(struct task *task)
{
    struct model_effect *ball = task->extra.p;

    ball->scale -= LARGE_STEP;
    if (ball->scale <= 0.0F) {
        task_kill(task);
        task->draw = NULL;
    }
}

static void ball_large_set_up(struct task *task)
{
    struct model_effect *ball = task->extra.p;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    struct player *record;
    struct task_work *work;

    /* @hack redundant: the record read beside the work, which nothing
     * uses */
    /* @hack spelling: the record and the work read in one statement, where
     * two would do */
    /* @hack spelling: the work read from the table's first element, where
     * the table would do */
    record = player_records[ball->player],
    work = (&player_work[0])[ball->player];
    /* @bug uninitialized: ball_large_draw reads a pos nothing set where the
     * slot has no work. the test at 0x8c0761f8 skips the place, and the draw
     * reads pos as syMalloc left it in the call at 0x8c076186 */
    if (work != NULL) {
        player_collision_center(ball->player, &ball->pos);
    }
    BALL_SET_MODEL(ball);
    ball->scale = LARGE_SCALE;
    task->draw = ball_large_draw;
}

void ball_large_create(int player)
{
    struct task *task;
    struct model_effect *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(ball_large_step)) != NULL) {
        ball = task->extra.p;
        ball->player = player;
        ball_large_set_up(task);
    }
}

/* a dash's mode: the set of textures it draws. a dash that draws the second
 * set ends a step sooner */
#define DASH_FIRST_SET 0
#define DASH_SECOND_SET 2

/* the streak's scale for each unit of the way from the player's center to
 * where the dash started. a dash whose scale reaches STREAK_SCALE_MAX ends
 * instead */
#define STREAK_SCALE 0.125F
#define STREAK_SCALE_MAX 5.0F
/* the streak model's radius at a scale of 1 */
#define STREAK_RADIUS 12.0F

/* set ang to the turns that take the x axis to way. ang.y turns it to way's
 * heading and ang.z to its rise, and ang.x is 0. a zero way leaves ang as it
 * was */
static void dash_angles(NJS_VECTOR *way, struct angles *ang)
{
    Float length = njScalor(way);

    if (length != 0.0F) {
        ang->y = atan2f(-way->z, way->x) * ANGLE_PER_RADIAN;
        ang->z = asinf(way->y / length) * ANGLE_PER_RADIAN;
        ang->x = 0;
    }
}

static void dash_draw(struct task *task)
{
    struct model_effect *dash = task->extra.p;

    if (dash->mode == DASH_SECOND_SET) {
        if (player_records[dash->player]->character == PLAYER_SONIC) {
            njSetTexture(&s_dash_second_textures[dash->timer - 1]);
        } else {
            njSetTexture(&sh_dash_second_textures[dash->timer - 1]);
        }
    } else {
        if (player_records[dash->player]->character == PLAYER_SONIC) {
            njSetTexture(&s_dash_textures[dash->timer - 1]);
        } else {
            njSetTexture(&sh_dash_textures[dash->timer - 1]);
        }
    }
    njFogDisable();
    njCnkEasyDrawObject(dash->object);
    njFogEnable();
}

/* the first step stretches the streak's x axis from the player's center to
 * where the dash started. it then moves the dash to that center. the second
 * lays every other vertex of the model's streak again. those run from the
 * center as it is then back to where the first step left it */
static void dash_step(struct task *task)
{
    struct model_effect *dash = task->extra.p;
    NJS_VECTOR offset;
    NJS_POINT3 pos;
    struct streak_vertex *vertex;
    int count;
    int i;
    struct player *record;
    struct task_work *work;
    struct streak_vertex *source;

    /* @hack spelling: the timer counted inside the test, where a statement
     * of its own would do */
    if (dash->timer++ >= DASH_STEPS) {
        task->draw = NULL;
        task_kill(task);
        return;
    }
    if (dash->mode == DASH_SECOND_SET && dash->timer >= DASH_SECOND_STEPS) {
        task_kill(task);
        return;
    }
    if (dash->timer <= 1) {
        /* @hack type: an array of one, where an object would do */
        struct angles ang[1];

        offset = dash->pos;
        /* @bug uninitialized: where the slot has no work, pos stays unset,
         * and the call at 0x8c0764ae reads it. the second step tests the
         * work first */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the slot has no work, the
         * dash ends. player_unregister nulls the slot's record with its work,
         * and dash_draw reads the record, so the draw ends too */
        if (player_work[dash->player] == NULL) {
            task->draw = NULL;
            task_kill(task);
            return;
        }
#endif
        player_collision_center(dash->player, &pos);
        njSubVector(&offset, &pos);
        /* @bug uninitialized: the turns read ang unset where the center
         * hasn't moved since dash_set_up. dash_angles then returns at
         * 0x8c07632a without setting it, and the loads at 0x8c0764fa and
         * 0x8c076502 pass it to njRotateY and njRotateZ. the second step
         * tests the offset's length first */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the center hasn't moved,
         * the turns are 0 */
        ang->y = 0;
        ang->z = 0;
#endif
        dash_angles(&offset, ang);
        dash->scale = njScalor(&offset) * STREAK_SCALE;
        if (dash->scale >= STREAK_SCALE_MAX) {
            task_kill(task);
            return;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&pos);
        njRotateY(NULL, ang->y);
        njRotateZ(NULL, ang->z);
        njScale(NULL, dash->scale, 1.0F, 1.0F);
        vertex =
            (struct streak_vertex *)(dash->object->model->vlist + HEAD_WORDS);
        /* @hack spelling: the count's halfword masked before its shift,
         * where the shift would do */
        count =
            (Sint16)((dash->object->model->vlist[COUNT_WORD] & COUNT_MASK) >>
                     COUNT_SHIFT);
        /* @hack spelling: i = i + 1, where i++ would do */
        for (i = 0; i < count; i = i + 1) {
            njCalcPoint(NULL, &vertex->pos, &vertex->pos);
            vertex++;
        }
        /* @hack redundant: a test of the counter that never runs */
        if (0) {
            if (i) {
            }
        }
        njPopMatrixEx();
        record = player_tasks[dash->player]->extra.p;
        /* @hack spelling: the bits tested in two arms, where one test of
         * both would do */
        if (((record->by_character.sonic.dash_steps & SONIC_DASH_STEP(2)) !=
                 0 &&
             (record->by_character.sonic.dash_steps & SONIC_DASH_STEP(1)) ==
                 0) ||
            (record->by_character.sonic.dash_steps & SONIC_DASH_STEP(2)) == 0) {
            dash->mode = DASH_SECOND_SET;
        }
        dash->pos = pos;
        dash->object->model->center = dash->pos;
        dash->object->model->r = dash->scale * STREAK_RADIUS;
    } else if (dash->timer == 2) {
        work = player_work[dash->player];
        if (work != NULL) {
            player_collision_center(dash->player, &pos);
            offset = dash->pos;
            njSubVector(&offset, &pos);
            if (njScalor(&offset) != 0.0F) {
                /* @hack type: an array of one, where an object would do */
                struct angles ang[1];

                dash_angles(&offset, ang);
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateEx(&pos);
                njRotateY(NULL, ang->y);
                njRotateZ(NULL, ang->z);
                njScale(NULL, dash->scale, 1.0F, 1.0F);
                /* @hack spelling: the count's halfword masked before its
                 * shift, where the shift would do */
                count =
                    (Sint16)((Sint32)(dash->object->model->vlist[COUNT_WORD] &
                                      COUNT_MASK) >>
                             (COUNT_SHIFT + 1));
                vertex = (struct streak_vertex *)(dash->object->model->vlist +
                                                  HEAD_WORDS);
                if (player_records[dash->player]->character == PLAYER_SONIC) {
                    source =
                        (struct streak_vertex *)(model_table[SONIC_DASH_MODEL]
                                                     .object->model->vlist +
                                                 HEAD_WORDS);
                } else {
                    source =
                        (struct streak_vertex *)(model_table[SHADOW_DASH_MODEL]
                                                     .object->model->vlist +
                                                 HEAD_WORDS);
                }
                for (i = 0; i < count; i++) {
                    /* @hack spelling: the pointers stepped inside the call,
                     * where statements of their own would do */
                    njCalcPoint(NULL, &(++source)->pos, &(++vertex)->pos);
                    vertex++;
                    source++;
                }
                njPopMatrixEx();
            }
        }
    }
}

/* free the dash's copy of the streak */
static void dash_destroy(struct task *task)
{
    struct model_effect *dash = task->extra.p;

    /* @bug memory: reads through the object and its model, at 0x8c076728
     * and 0x8c07672a, ahead of the tests below for null */
    if (dash->object->model->vlist != NULL) {
        syFree(dash->object->model->vlist);
        dash->object->model->vlist = NULL;
    }
    if (dash->object->model != NULL) {
        syFree(dash->object->model);
        dash->object->model = NULL;
    }
    if (dash->object != NULL) {
        syFree(dash->object);
        dash->object = NULL;
    }
}

/* copy the character's streak into blocks of the Shinobi heap: its object, its
 * model and its vertex list. then start the dash at the player's center. where
 * a block can't be had, it frees those it has and kills the task */
static void dash_set_up(struct task *task)
{
    struct model_effect *dash = task->extra.p;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *source;
    int count;
    int size;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((object = syMalloc(sizeof(NJS_CNK_OBJECT))) == NULL) {
        task_kill(task);
        return;
    }
    if (player_records[dash->player]->character == PLAYER_SONIC) {
        source = model_table[SONIC_DASH_MODEL].object;
    } else {
        source = model_table[SHADOW_DASH_MODEL].object;
    }
    *object = *source;
    dash->object = object;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((object->model = syMalloc(sizeof(NJS_CNK_MODEL))) == NULL) {
        syFree(object);
        task_kill(task);
        return;
    }
    *object->model = *source->model;
    /* @hack spelling: the count's halfword masked before its shift, where the
     * shift would do */
    count = (Sint16)((source->model->vlist[COUNT_WORD] & COUNT_MASK) >>
                     COUNT_SHIFT);
    size =
        (count * sizeof(struct streak_vertex)) + (OTHER_WORDS * sizeof(Sint32));
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((object->model->vlist = syMalloc(size)) == NULL) {
        syFree(object->model);
        syFree(object);
        task_kill(task);
        return;
    }
    njMemCopy4(object->model->vlist, source->model->vlist,
               (count * VERTEX_WORDS) + OTHER_WORDS);
    player_collision_center(dash->player, &dash->pos);
    task->destructor = dash_destroy;
    task->draw = dash_draw;
}

void dash_create(int player)
{
    struct task *task;
    struct model_effect *dash;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(dash_step)) != NULL) {
        dash = task->extra.p;
        dash->player = player;
        dash->timer = 0;
        dash->mode = DASH_FIRST_SET;
        dash_set_up(task);
    }
}

static void flame_spark_draw(struct task *task)
{
    struct flame_spark *spark = task->extra.p;

    if (player_records[spark->player]->character == PLAYER_SONIC) {
        flame_spark_sprite->tlist = &s_efftex_texlist;
    } else {
        flame_spark_sprite->tlist = &sh_efftex_texlist;
    }
    flame_spark_cell->texid = (spark->timer >> 1) + TEX_SPARK;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(flame_spark_sprite->tlist);
    /* @hack spelling: sx and sy set in one statement, where two would do */
    flame_spark_sprite->sx = flame_spark_sprite->sy = spark->scale;
    flame_spark_sprite->p = spark->pos;
    njDrawSprite3D(flame_spark_sprite, 0, NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
}

static void flame_spark_destroy(struct task *task)
{
}

/* a spark starts within SPARK_REACH of the player's unk_230 along each axis.
 * its scale starts at SPARK_SCALE_MIN up to SPARK_SCALE_SPAN more */
#define SPARK_REACH 1.5F
#define SPARK_SCALE_MIN 0.12F
#define SPARK_SCALE_SPAN 0.08F
/* the drift is SPARK_DRIFT of the unit vector of the player's motion. each of
 * its parts then gains SPARK_DRIFT_GAIN_MAX, less up to
 * SPARK_DRIFT_GAIN_SPAN */
#define SPARK_DRIFT (-0.3F)
#define SPARK_DRIFT_GAIN_MAX 0.1F
#define SPARK_DRIFT_GAIN_SPAN 0.05F
/* what a step multiplies the scale and the drift by */
#define SPARK_GROWTH 1.015F
#define SPARK_FRICTION 0.95F
/* a spark's timer starts at random below SPARK_START_STEPS. the step that
 * takes it to SPARK_STEPS ends the spark */
#define SPARK_START_STEPS 4
#define SPARK_STEPS 14

static void flame_spark_step(struct task *task)
{
    struct flame_spark *spark = task->extra.p;

    /* @hack spelling: the timer counted inside the test, where a statement
     * of its own would do */
    if (++spark->timer >= SPARK_STEPS) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    spark->scale *= SPARK_GROWTH;
    njAddVector(&spark->pos, &spark->spd);
    spark->spd.x *= SPARK_FRICTION;
    spark->spd.y *= SPARK_FRICTION;
    spark->spd.z *= SPARK_FRICTION;
}

void flame_spark_create(int player)
{
    struct task *task;
    struct flame_spark *spark;

    task = flame_spark_task_create(flame_spark_step);
    if (task == NULL) {
        return;
    }
    spark = task->extra.p;
    spark->player = player;
    spark->timer = rand() * (SPARK_START_STEPS * RAND_SCALE);
    spark->unk_00 = 0;
    spark->pos = player_records[player]->unk_230;
    spark->pos.x += SPARK_REACH - (rand() * (2 * SPARK_REACH * RAND_SCALE));
    spark->pos.y += SPARK_REACH - (rand() * (2 * SPARK_REACH * RAND_SCALE));
    spark->pos.z += SPARK_REACH - (rand() * (2 * SPARK_REACH * RAND_SCALE));
    spark->scale = (rand() * (SPARK_SCALE_SPAN * RAND_SCALE)) + SPARK_SCALE_MIN;
    spark->spd = player_motion[player]->spd;
    njUnitVector(&spark->spd);
    spark->spd.x *= SPARK_DRIFT;
    spark->spd.y *= SPARK_DRIFT;
    spark->spd.z *= SPARK_DRIFT;
    spark->spd.x +=
        SPARK_DRIFT_GAIN_MAX - (rand() * (SPARK_DRIFT_GAIN_SPAN * RAND_SCALE));
    spark->spd.y +=
        SPARK_DRIFT_GAIN_MAX - (rand() * (SPARK_DRIFT_GAIN_SPAN * RAND_SCALE));
    spark->spd.z +=
        SPARK_DRIFT_GAIN_MAX - (rand() * (SPARK_DRIFT_GAIN_SPAN * RAND_SCALE));
    task->destructor = flame_spark_destroy;
    task->draw = flame_spark_draw;
}

/* a piece's color as it is laid, opaque white. each step of a trail's fade
 * takes PIECE_FADE from it, 8 of its alpha */
#define PIECE_COLOR 0xffffffff
#define PIECE_FADE 0x08000000

/* a light's step once its trail is laid, which fades each piece by
 * PIECE_FADE. it kills the light once a piece's color is PIECE_FADE or
 * less */
static void trail_fade(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXTURE_VTX *piece = task->extra.p;
    int i;

    /* @hack spelling: the pieces counted down from tick, where a count up
     * to it would do */
    for (i = work->tick; i > 0; i--) {
        if (piece->col > PIECE_FADE) {
            piece->col -= PIECE_FADE;
        } else {
            task_kill(task);
            task->draw = NULL;
            return;
        }
        piece++;
    }
}

/* a light draws its head while it lays its trail. it draws the trail once more
 * than TRAIL_HIDDEN_PIECES of the pieces are laid */
#define TRAIL_HIDDEN_PIECES 3

/* a light's draw adds its head and its trail to what lies behind them */
static void sparkle_light_draw(struct task *task)
{
    NJS_TEXTURE_VTX *pieces = task->extra.p;
    struct task_work *work = task->work;

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    if (player_character(LIGHT_PLAYER(work)) == PLAYER_SONIC) {
        sparkle_light_head[0].tlist = &s_efftex_texlist;
    } else {
        sparkle_light_head[0].tlist = &sh_efftex_texlist;
    }
    njSetTexture(sparkle_light_head[0].tlist);
    if (work->tick < work->timer) {
        sparkle_light_head[0].p = work->pos;
        njDrawSprite3D(sparkle_light_head, 0,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    }
    if (work->tick > TRAIL_HIDDEN_PIECES) {
        njSetTexture(sparkle_light_head[0].tlist);
        njSetTextureNum(TEX_TRAIL);
        njDrawTexture3DExStart(1);
        njDrawTexture3DExSetData(pieces, work->tick);
        njDrawTexture3DExEnd();
    }
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void spin_charge_light_draw(struct task *task)
{
    NJS_TEXTURE_VTX *pieces = task->extra.p;
    struct task_work *work = task->work;

    njFogDisable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    if (player_character(LIGHT_PLAYER(work)) == PLAYER_SONIC) {
        spin_charge_light_head[0].tlist = &s_efftex_texlist;
    } else {
        spin_charge_light_head[0].tlist = &sh_efftex_texlist;
    }
    njSetTexture(spin_charge_light_head[0].tlist);
    if (work->tick < work->timer) {
        spin_charge_light_head[0].p = work->pos;
        njDrawSprite3D(spin_charge_light_head, 0,
                       NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE);
    }
    if (work->tick > TRAIL_HIDDEN_PIECES) {
        njSetTexture(spin_charge_light_head[0].tlist);
        njSetTextureNum(TEX_TRAIL);
        njDrawTexture3DExStart(1);
        njDrawTexture3DExSetData(pieces, work->tick);
        njDrawTexture3DExEnd();
    }
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* a trail's first pair lies at a u of 0, and its last at 1. the pairs
 * between lie at PIECE_U_A and PIECE_U_B in turn, by tick's two low bits */
#define PIECE_U_A 0.4375F
#define PIECE_U_B 0.5625F

static void sparkle_light_step(struct task *task)
{
    NJS_TEXTURE_VTX *pieces = task->extra.p;
    struct task_work *work = task->work;
    struct task_work *charger;
    NJS_POINT3 middle;
    Float radius;
    Angle ang_x;
    Angle ang_y;
    Float sin_x;
    Float sin_y;
    Float cos_x;
    Float cos_y;
    NJS_TEXTURE_VTX *piece;
    int i;

    charger = player_work[LIGHT_PLAYER(work)];
    if (charger == NULL) {
        task_kill(task);
        return;
    }
    middle.x = (charger->pos.x + charger->collision->shapes->center.x) * 0.5F;
    middle.y = (charger->pos.y + charger->collision->shapes->center.y) * 0.5F;
    middle.z = (charger->pos.z + charger->collision->shapes->center.z) * 0.5F;
    /* @hack spelling: the product written out, where *= would do */
    LIGHT_RADIUS(work) = LIGHT_RADIUS(work) * SPARKLE_CLOSE;
    radius = LIGHT_RADIUS(work);
    ang_x = work->ang.x;
    /* @hack spelling: ang.y set in the same statement as ang_y, where two
     * statements would do */
    ang_y = work->ang.y = work->ang.y + LIGHT_TURN(work);
    sin_x = njSin(ang_x);
    sin_y = njSin(ang_y);
    cos_x = njCos(ang_x);
    cos_y = njCos(ang_y);
    work->pos.x = middle.x + (radius * sin_y * cos_x);
    work->pos.z = middle.z + (radius * cos_x * cos_y);
    work->pos.y = middle.y + (radius * sin_x);
    piece = &pieces[work->tick];
    for (i = 0; i < PIECES_PER_STEP; i++) {
        if (work->tick & 1) {
            radius -= SPARKLE_TRAIL_WIDTH;
        }
        piece->x = middle.x + (radius * sin_y * cos_x);
        piece->z = middle.z + (radius * cos_x * cos_y);
        piece->y = middle.y + (radius * sin_x);
        piece->col = PIECE_COLOR;
        if (work->tick == work->timer - 1) {
            piece->u = 1.0F;
            piece->v = 1.0F;
        } else if (work->tick == work->timer - 2) {
            piece->u = 1.0F;
            piece->v = 0.0F;
        } else {
            /* @hack spelling: the cases in the order 2, 3, 0, 1, where the
             * order 0 to 3 would do */
            switch (work->tick & 3) {
            case 2:
                piece->u = PIECE_U_A;
                piece->v = 0.0F;
                break;
            case 3:
                piece->u = PIECE_U_A;
                piece->v = 1.0F;
                break;
            case 0:
                piece->u = PIECE_U_B;
                piece->v = 0.0F;
                break;
            case 1:
                piece->u = PIECE_U_B;
                piece->v = 1.0F;
                break;
            }
        }
        work->tick++;
        piece++;
    }
    /* @hack spelling: the return in the arm, where an else for the
     * registration would do */
    if (work->tick >= work->timer) {
        task->step = trail_fade;
        return;
    }
    /* @hack spelling: task->work read again, where work would do */
    if (task->work->collision != NULL) {
        collision_register(task);
    }
}

void sparkle_light_create(int player)
{
    struct task *task;
    NJS_TEXTURE_VTX *pieces;
    struct task_work *work;
    struct task_work *charger;
    Float radius;
    Angle ang_x;
    Angle ang_y;
    /* @hack spelling: cos_y declared ahead of cos_x, where the reverse would
     * do */
    Float sin_x;
    Float sin_y;
    Float cos_y;
    Float cos_x;
    NJS_POINT3 middle;
    NJS_TEXTURE_VTX *piece;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = trail_task_create(sparkle_light_step)) == NULL) {
        return;
    }
    pieces = task->extra.p;
    work = task->work;
    LIGHT_PLAYER(work) = player;
    charger = player_work[player];
    work->id = WORK_ID_PLAYER_PROXY;
    /* @hack spelling: radius, ang_x and ang_y set in the work's statements,
     * where statements of their own would do */
    radius = LIGHT_RADIUS(work) =
        (rand() * (SPARKLE_RADIUS_SPAN * RAND_SCALE)) + SPARKLE_RADIUS_MIN;
    ang_x = work->ang.x =
        (Angle)(rand() * (LIGHT_TILT_SPAN * RAND_SCALE)) + LIGHT_TILT_MIN;
    ang_y = work->ang.y = (Angle)RANDOM_TURN();
    sin_x = njSin(ang_x);
    sin_y = njSin(ang_y);
    cos_x = njCos(ang_x);
    cos_y = njCos(ang_y);
    LIGHT_TURN(work) =
        (Angle)(rand() * (LIGHT_TURN_SPAN * RAND_SCALE)) + LIGHT_TURN_MIN;
    middle.x = (charger->pos.x + charger->collision->shapes->center.x) * 0.5F;
    middle.y = (charger->pos.y + charger->collision->shapes->center.y) * 0.5F;
    middle.z = (charger->pos.z + charger->collision->shapes->center.z) * 0.5F;
    work->pos.x = middle.x + (radius * sin_y * cos_x);
    work->pos.z = middle.z + (radius * cos_x * cos_y);
    work->pos.y = middle.y + (radius * sin_x);
    piece = pieces;
    piece->x = work->pos.x;
    piece->y = work->pos.y;
    piece->z = work->pos.z;
    piece->u = 0.0F;
    piece->v = 0.0F;
    piece->col = PIECE_COLOR;
    piece++;
    radius -= TRAIL_START_WIDTH;
    piece->x = middle.x + (radius * sin_y * cos_x);
    piece->z = middle.z + (radius * cos_x * cos_y);
    piece->y = middle.y + (radius * sin_x);
    piece->u = 0.0F;
    piece->v = 1.0F;
    piece->col = PIECE_COLOR;
    work->mode = 0;
    work->tick = PIECES_PER_STEP;
    work->timer = TRAIL_PIECES;
    collision_init(task, &sparkle_light_shape, 1, COLLISION_ATTACK);
    task->draw = sparkle_light_draw;
}

static void spin_charge_light_step(struct task *task)
{
    NJS_TEXTURE_VTX *pieces = task->extra.p;
    struct task_work *work = task->work;
    struct task_work *charger;
    Float rise;
    NJS_POINT3 middle;
    Float radius;
    Angle ang_x;
    Angle ang_y;
    Float sin_x;
    Float sin_y;
    Float cos_x;
    Float cos_y;
    NJS_TEXTURE_VTX *piece;
    int i;

    charger = player_work[LIGHT_PLAYER(work)];
    if (charger == NULL) {
        task_kill(task);
        return;
    }
    rise = work->tick * CHARGE_RISE;
    middle.x =
        ((charger->pos.x + charger->collision->shapes->center.x) * 0.5F) -
        (world_gravity.x * rise);
    middle.y =
        ((charger->pos.y + charger->collision->shapes->center.y) * 0.5F) -
        (world_gravity.y * rise);
    middle.z =
        ((charger->pos.z + charger->collision->shapes->center.z) * 0.5F) -
        (world_gravity.z * rise);
    /* @hack spelling: the product written out, where *= would do */
    LIGHT_RADIUS(work) = LIGHT_RADIUS(work) * CHARGE_OPEN;
    radius = LIGHT_RADIUS(work);
    ang_x = work->ang.x;
    /* @hack spelling: ang.y set in the same statement as ang_y, where two
     * statements would do */
    ang_y = work->ang.y = work->ang.y + LIGHT_TURN(work);
    sin_x = njSin(ang_x);
    sin_y = njSin(ang_y);
    cos_x = njCos(ang_x);
    cos_y = njCos(ang_y);
    work->pos.x = middle.x + (radius * sin_y * cos_x);
    work->pos.z = middle.z + (radius * cos_x * cos_y);
    work->pos.y = middle.y + (radius * sin_x);
    piece = &pieces[work->tick];
    for (i = 0; i < PIECES_PER_STEP; i++) {
        if (work->tick & 1) {
            radius -= CHARGE_TRAIL_WIDTH;
        }
        piece->x = middle.x + (radius * sin_y * cos_x);
        piece->z = middle.z + (radius * cos_x * cos_y);
        piece->y = middle.y + (radius * sin_x);
        piece->col = PIECE_COLOR;
        if (work->tick == work->timer - 1) {
            piece->u = 1.0F;
            piece->v = 1.0F;
        } else if (work->tick == work->timer - 2) {
            piece->u = 1.0F;
            piece->v = 0.0F;
        } else {
            /* @hack spelling: the cases in the order 2, 3, 0, 1, where the
             * order 0 to 3 would do */
            switch (work->tick & 3) {
            case 2:
                piece->u = PIECE_U_A;
                piece->v = 0.0F;
                break;
            case 3:
                piece->u = PIECE_U_A;
                piece->v = 1.0F;
                break;
            case 0:
                piece->u = PIECE_U_B;
                piece->v = 0.0F;
                break;
            case 1:
                piece->u = PIECE_U_B;
                piece->v = 1.0F;
                break;
            }
        }
        work->tick++;
        piece++;
    }
    if (work->tick >= work->timer) {
        task->step = trail_fade;
    }
}

void spin_charge_light_create(int player)
{
    struct task *task;
    NJS_TEXTURE_VTX *pieces;
    struct task_work *work;
    struct task_work *charger;
    Float radius;
    Angle ang_x;
    Angle ang_y;
    /* @hack spelling: cos_y declared ahead of cos_x, where the reverse would
     * do */
    Float sin_x;
    Float sin_y;
    Float cos_y;
    Float cos_x;
    NJS_POINT3 middle;
    NJS_TEXTURE_VTX *piece;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = trail_task_create(spin_charge_light_step)) == NULL) {
        return;
    }
    pieces = task->extra.p;
    work = task->work;
    LIGHT_PLAYER(work) = player;
    charger = player_work[player];
    /* @hack spelling: radius, ang_x and ang_y set in the work's statements,
     * where statements of their own would do */
    radius = LIGHT_RADIUS(work) =
        (rand() * (CHARGE_RADIUS_SPAN * RAND_SCALE)) + CHARGE_RADIUS_MIN;
    ang_x = work->ang.x =
        (Angle)(rand() * (LIGHT_TILT_SPAN * RAND_SCALE)) + LIGHT_TILT_MIN;
    ang_y = work->ang.y = (Angle)RANDOM_TURN();
    sin_x = njSin(ang_x);
    sin_y = njSin(ang_y);
    cos_x = njCos(ang_x);
    cos_y = njCos(ang_y);
    LIGHT_TURN(work) =
        (Angle)(rand() * (LIGHT_TURN_SPAN * RAND_SCALE)) + LIGHT_TURN_MIN;
    if (player_character(player) == PLAYER_SHADOW) {
        LIGHT_TURN(work) = -LIGHT_TURN(work);
    }
    middle.x = (charger->pos.x + charger->collision->shapes->center.x) * 0.5F;
    middle.y = (charger->pos.y + charger->collision->shapes->center.y) * 0.5F;
    middle.z = (charger->pos.z + charger->collision->shapes->center.z) * 0.5F;
    work->pos.x = middle.x + (radius * sin_y * cos_x);
    work->pos.z = middle.z + (radius * cos_x * cos_y);
    work->pos.y = middle.y + (radius * sin_x);
    piece = pieces;
    piece->x = work->pos.x;
    piece->y = work->pos.y;
    piece->z = work->pos.z;
    piece->u = 0.0F;
    piece->v = 0.0F;
    piece->col = PIECE_COLOR;
    piece++;
    radius -= TRAIL_START_WIDTH;
    piece->x = middle.x + (radius * sin_y * cos_x);
    piece->z = middle.z + (radius * cos_x * cos_y);
    piece->y = middle.y + (radius * sin_x);
    piece->u = 0.0F;
    piece->v = 1.0F;
    piece->col = PIECE_COLOR;
    work->mode = 0;
    work->tick = PIECES_PER_STEP;
    work->timer = TRAIL_PIECES;
    task->draw = spin_charge_light_draw;
}

/* a Chaos Control's model and a flash draw from frame 0 to CHAOS_LAST_FRAME.
 * a frame less than NEAR_WHOLE past a whole one counts as whole */
#define CHAOS_LAST_FRAME 60.0F
#define NEAR_WHOLE 0.1F

/* draw the Chaos Control's rings at pos, at frame of their animation.
 * each ring grows in x and z from its first frame by its own rate a frame. it
 * shows until its last frame. a character other than Sonic and Shadow draws
 * nothing */
static void chaos_control_draw_model(struct player *record, NJS_POINT3 *pos,
                                     Float frame)
{
    NJS_CNK_OBJECT *object;

    if (frame < 0.0F || frame > CHAOS_LAST_FRAME) {
        return;
    }
    if (record->character == PLAYER_SONIC) {
        object = model_table[SONIC_CHAOS_MODEL].object;
    } else if (record->character == PLAYER_SHADOW) {
        object = model_table[SHADOW_CHAOS_MODEL].object;
    } else {
        return;
    }
    njFogDisable();
    njPushMatrixEx();
    if (record->character == PLAYER_SONIC) {
        njSetTexture(&s_efftex_texlist);
    } else {
        njSetTexture(&sh_efftex_texlist);
    }
    njTranslateEx(pos);
    if (_builtin_fabsf(njFraction(frame)) < NEAR_WHOLE) {
        /* the quad's half gray, scaled by 1 less the screen's color, replaces
         * the screen. that is the screen inverted at half brightness */
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_INVDESTCOLOR);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ZERO);
        njDrawPolygon(chaos_control_screen, 4, 1);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    if (frame > 0.0F && frame < 5.0F) {
        /* @hack redundant: the 0 taken from the first ring's frame */
        Float scale = (frame - 0.0F) * 0.02F;

        njRotateZ(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 3.0F && frame < 11.0F) {
        Float scale = (frame - 3.0F) * 0.04F;

        njRotateX(NULL, EIGHTH_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 8.0F && frame < 16.0F) {
        Float scale = (frame - 8.0F) * 0.07F;

        njRotateX(NULL, EIGHTH_TURN);
        njRotateY(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 11.0F && frame < 16.0F) {
        Float scale = (frame - 11.0F) * 0.03F;

        njRotateZ(NULL, QUARTER_TURN - (EIGHTH_TURN / 2));
        njRotateY(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    njPopMatrixEx();
    njFogEnable();
}

static void chaos_control_step(struct task *task)
{
    /* @hack spelling: the record read through task->extra at each use, where
     * a local would do */
    struct player *record =
        player_records[((struct chaos_control *)task->extra.p)->player];

    if ((record->flags & PLAYER_MISSED) ||
        ((struct chaos_control *)task->extra.p)->timer-- < 0) {
        task_kill(task);
    }
}

/* the frame of the player's animation at which the rings start */
#define CHAOS_CONTROL_START 16.0F
static void chaos_control_draw(struct task *task)
{
    struct player *record;
    Float frame;

    /* @hack spelling: the record read through task->extra at each use, where
     * a local would do */
    record =
        player_tasks[((struct chaos_control *)task->extra.p)->player]->extra.p;
    frame = record->anim.frame;
    /* @hack spelling: the test of the kind as an else after the return,
     * where a test of its own would do */
    if (frame < CHAOS_CONTROL_START) {
        return;
    } else if (record->kind == PLAYER_KIND_AMY) {
        chaos_control_draw_model(record, &record->unk_224,
                                 frame - CHAOS_CONTROL_START);
    } else if (record->character == PLAYER_SHADOW) {
        chaos_control_draw_model(record, &record->unk_1f4,
                                 frame - CHAOS_CONTROL_START);
    } else {
        chaos_control_draw_model(record, &record->unk_1e8,
                                 frame - CHAOS_CONTROL_START);
    }
    ((struct chaos_control *)task->extra.p)->frame = floorf(frame);
}

#define CHAOS_CONTROL_STEPS 60
void chaos_control_create(int player)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, CHAOS_CONTROL_LEVEL, chaos_control_step,
                            "SonicEffectChaosControlEffectExecutor")) != NULL) {
        task->draw = chaos_control_draw;
        /* @hack spelling: the record read through task->extra at each use,
         * where a local would do */
        ((struct chaos_control *)task->extra.p)->player = player;
        ((struct chaos_control *)task->extra.p)->timer = CHAOS_CONTROL_STEPS;
    }
}

/* a flash's modes: it follows the player's unk_230, or stays where its
 * creator put it */
#define FLASH_FOLLOWS 0
#define FLASH_FIXED 1
/* a flash's timer starts at FLASH_STEPS, and its frame is FLASH_STEPS less
 * the timer. each step gives the glow a scale of GLOW_SCALE_MIN up to
 * GLOW_SCALE_SPAN more */
#define FLASH_STEPS 60
#define GLOW_SCALE_MIN 0.7F
#define GLOW_SCALE_SPAN 0.6F
/* the glow's turn for each unit of its scale, about 19.8 turns. a random
 * scale thus gives the glow a turn that is in effect random */
#define GLOW_TURN_PER_SCALE 1299544.0F

static void flash_draw_model(struct model_effect *flash)
{
    Float frame;
    NJS_CNK_OBJECT *object;

    frame = FLASH_STEPS - flash->timer;
    if (frame < 0.0F || frame > CHAOS_LAST_FRAME) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((object = flash->object) == NULL) {
        return;
    }
    njFogDisable();
    njPushMatrixEx();
    njSetTexture(&flash->texlist);
    njTranslateEx(&flash->pos);
    if (_builtin_fabsf(njFraction(frame)) < NEAR_WHOLE) {
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njPushMatrixEx();
        njSetTextureNum(TEX_GLOW);
        njRotateY(NULL, -camera_stacks[0].place.ang.y);
        njRotateX(NULL, -camera_stacks[0].place.ang.x);
        njRotateZ(NULL, -camera_stacks[0].place.ang.z);
        njRotateZ(NULL, (Angle)(flash->scale * GLOW_TURN_PER_SCALE));
        njScale(NULL, flash->scale, flash->scale, flash->scale);
        njDrawTexture3DEx(flash_glow, QUAD_VERTICES, 1);
        njPopMatrixEx();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
    }
    njPushMatrixEx();
    if (frame > 0.0F && frame < 5.0F) {
        /* @hack redundant: the 0 taken from the first ring's frame */
        Float scale = (frame - 0.0F) * 0.2F;

        njRotateZ(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 3.0F && frame < 11.0F) {
        Float scale = (frame - 3.0F) * 0.4F;

        njRotateX(NULL, EIGHTH_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 8.0F && frame < 16.0F) {
        Float scale = (frame - 8.0F) * 0.7F;

        njRotateX(NULL, EIGHTH_TURN);
        njRotateY(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 11.0F && frame < 16.0F) {
        Float scale = (frame - 11.0F) * 0.3F;

        njRotateZ(NULL, QUARTER_TURN - (EIGHTH_TURN / 2));
        njRotateY(NULL, QUARTER_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 15.0F && frame < 25.0F) {
        Float scale = (frame - 15.0F) * 0.3F;

        njRotateZ(NULL, QUARTER_TURN - (EIGHTH_TURN / 4));
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 20.0F && frame < 30.0F) {
        Float scale = (frame - 20.0F) * 0.2F;

        njRotateZ(NULL, FULL_TURN - EIGHTH_TURN);
        njRotateY(NULL, EIGHTH_TURN);
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    if (frame > 35.0F && frame < 50.0F) {
        Float scale = (frame - 35.0F) * 0.5F;

        njRotateZ(NULL, QUARTER_TURN);
        njRotateY(NULL, FULL_TURN - (EIGHTH_TURN / 2));
        njScale(NULL, scale, 1.0F, scale);
        njCnkSimpleDrawObject(object);
        matrix_load_parent();
    }
    njPopMatrix(2);
    njFogEnable();
}

static void flash_step(struct task *task)
{
    struct model_effect *flash = task->extra.p;
    /* @hack spelling: the record in a local, where its one use could read
     * it */
    struct player *record = player_records[flash->player];

    if ((record->flags & PLAYER_MISSED) || flash->timer-- < 0) {
        task_kill(task);
        return;
    }
    flash->scale = (rand() * (GLOW_SCALE_SPAN * RAND_SCALE)) + GLOW_SCALE_MIN;
}

static void flash_draw(struct task *task)
{
    struct model_effect *flash = task->extra.p;
    struct player *record;

    if (flash->mode == FLASH_FOLLOWS) {
        record = player_tasks[flash->player]->extra.p;
        flash->pos = record->unk_230;
    }
    flash_draw_model(flash);
}

void chaos_control_flash_create(int player, NJS_POINT3 *pos)
{
    struct task *task;
    struct player *record;
    struct model_effect *flash;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = model_effect_task_create(flash_step)) != NULL) {
        record = player_tasks[player]->extra.p;
        flash = task->extra.p;
        task->draw = flash_draw;
        flash->player = player;
        /* @bug uninitialized: for a null pos, mode stays as syMalloc left
         * it, and flash_draw tests it at 0x8c077eba */
        if (pos != NULL) {
            flash->mode = FLASH_FIXED;
            flash->pos = *pos;
        }
        flash->timer = FLASH_STEPS;
        if (record->character == PLAYER_SONIC) {
            flash->object = model_table[SONIC_CHAOS_MODEL].object;
        } else if (record->character == PLAYER_SHADOW) {
            flash->object = model_table[SHADOW_CHAOS_MODEL].object;
        } else {
            flash->object = NULL;
        }
        if (record->character == PLAYER_SONIC) {
            flash->texlist = s_efftex_texlist;
        } else {
            flash->texlist = sh_efftex_texlist;
        }
        flash->scale = 1.0F;
    }
}

/* a jet's offset starts at its scale plus JET_START_REACH along x. each
 * metal_jet_aim lays it along the player's unk_2cc, and lengthens it by
 * JET_REACH_PER_SCALE of the scale */
#define JET_START_REACH 1.75F
#define JET_REACH_PER_SCALE 0.75F
/* a jet draws texture JET_FIRST_TEXTURE + texture of the player's list. texture
 * is rand() times JET_TEXTURE_SPAN * RAND_SCALE, 0 to 3 */
#define JET_FIRST_TEXTURE 8
#define JET_TEXTURE_SPAN 3.99F
/* each byte of a jet's color is its life times JET_BRIGHTEST */
#define JET_BRIGHTEST 255.0F
/* each kind's fade is what a step takes from its life. its scale runs from a
 * least up to a span more. JET_SCALED's scale is a share of the scale
 * metal_jet_create takes. OTHER_ is JET_LASTING's, and any other kind's */
#define SCALED_FADE 0.25F
#define SCALED_SHARE_MIN 0.07F
#define SCALED_SHARE_SPAN 0.015F
#define SLOW_FADE 0.19F
#define SLOW_SCALE_MIN 0.04F
#define SLOW_SCALE_SPAN 0.02F
#define FAST_FADE 0.14F
#define FAST_SCALE_MIN 0.07F
#define FAST_SCALE_SPAN 0.024F
#define OTHER_FADE 0.1F
#define OTHER_SCALE_MIN 0.05F
#define OTHER_SCALE_SPAN 0.02F

static void metal_jet_follow(struct task *task)
{
    struct jet_work *jet = JET(task);
    struct player *record = player_tasks[jet->player]->extra.p;

    jet->pos = record->unk_230;
    njAddVector(&jet->pos, &jet->offset);
}

static void metal_jet_aim(struct task *task)
{
    struct jet_work *jet = JET(task);
    /* @hack spelling: the task in a local of its own, where its one use
     * could read it */
    struct task *player_task = player_tasks[jet->player];
    struct player *record = player_task->extra.p;
    NJS_VECTOR *offset = &jet->offset;
    Float length;

    length = njScalor(offset) + (jet->scale * JET_REACH_PER_SCALE);
    offset->x = record->unk_2cc.x * length;
    offset->y = record->unk_2cc.y * length;
    offset->z = record->unk_2cc.z * length;
}

/* a run of jets among their parent's children draws as one batch. the first
 * turns the fog off, and sets an additive blend and the record's texture list.
 * it then pushes the matrix. each later one starts from the pushed matrix
 * again. the last clears the constant material, turns the fog on and pops the
 * matrix. it leaves the source blend at NJD_COLOR_BLENDING_SRCALPHA and the
 * destination's at NJD_COLOR_BLENDING_INVSRCALPHA */
static void metal_jet_draw(struct task *task)
{
    struct jet_work *jet = JET(task);
    Float scale = jet->scale;
    struct player *record = player_tasks[jet->player]->extra.p;
    Uint32 color;
    NJS_TEXTURE_VTX *vtx;
    int i;

    if (task->parent->child == task || task->prev->draw != metal_jet_draw) {
        njFogDisable();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(record->by_character.sonic.texlist);
        njPushMatrixEx();
    }
    metal_jet_follow(task);
    color = (Uint32)(jet->life * JET_BRIGHTEST);
    color = (color << 24) | (color << 16) | (color << 8) | color;
    /* @hack spelling: the counter set ahead of the for, where its first
     * clause would do */
    vtx = jet_flame;
    i = QUAD_VERTICES;
    for (; i > 0; i--) {
        vtx->col = color;
        vtx++;
    }
    njSetTextureNum(jet->texture + JET_FIRST_TEXTURE);
    njTranslateEx(&jet->pos);
    njScale(NULL, scale, scale, scale);
    njRotateEx(&camera_views[camera_current_index]->ang.x, 0);
    njDrawTexture3DEx(jet_flame, QUAD_VERTICES, 1);
    if (task->parent->child->prev == task ||
        task->next->draw != metal_jet_draw) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
        njPopMatrixEx();
    } else {
        matrix_load_parent();
    }
}

static void metal_jet_destroy(struct task *task)
{
}

/* a jet's step destroys it once its life is JET_GONE or less */
#define JET_GONE 0.05F
static void metal_jet_step(struct task *task)
{
    struct jet_work *jet = JET(task);

    if (task->parent == NULL && placement_try_leave(task)) {
        return;
    }
    if (game_state.winner_posed == 1) {
        task_kill(task);
        task->draw = NULL;
        return;
    }
    if (jet->life <= JET_GONE) {
        task_destroy(task);
        return;
    }
    if (game_state.time_stopped & TIME_STOPPED_BIT(jet->player)) {
        return;
    }
    jet->life -= jet->life_step;
    if (jet->life < 0.0F) {
        jet->life = 0.0F;
    }
    metal_jet_aim(task);
    jet->timer++;
}

static void metal_jet_set_up(struct task *task)
{
    struct jet_work *jet = JET(task);

    jet->life = 1.0F;
    switch (jet->kind) {
    case JET_SCALED:
        jet->life_step = SCALED_FADE;
        /* @hack spelling: the product written out, where *= would do */
        jet->scale =
            ((rand() * (SCALED_SHARE_SPAN * RAND_SCALE)) + SCALED_SHARE_MIN) *
            jet->scale;
        break;
    case JET_SLOW:
        jet->life_step = SLOW_FADE;
        jet->scale = (rand() * (SLOW_SCALE_SPAN * RAND_SCALE)) + SLOW_SCALE_MIN;
        break;
    case JET_FAST:
        jet->life_step = FAST_FADE;
        jet->scale = (rand() * (FAST_SCALE_SPAN * RAND_SCALE)) + FAST_SCALE_MIN;
        break;
    case JET_LASTING:
    default:
        jet->life_step = OTHER_FADE;
        jet->scale =
            (rand() * (OTHER_SCALE_SPAN * RAND_SCALE)) + OTHER_SCALE_MIN;
        break;
    }
    task->destructor = metal_jet_destroy;
    task->draw = metal_jet_draw;
}

void metal_jet_create(int player, int kind, Float scale)
{
    struct task *parent;
    struct task *task;
    struct jet_work *jet;
    NJS_VECTOR *offset;

    parent = player_tasks[player];
    if (parent != NULL) {
        task = task_create_child(TASK_WORK, metal_jet_step, parent);
    } else {
        /* @bug memory: a jet made here reads through two nulls.
         * metal_jet_aim reads the slot's task, which this test found null,
         * at 0x8c07801a, as metal_jet_follow and metal_jet_draw do.
         * metal_jet_draw reads the jet's parent, null, at 0x8c07807e and
         * 0x8c078080, though metal_jet_step tests it at 0x8c0781e8 */
        task = task_create(TASK_WORK, EFFECT_LEVEL, metal_jet_step,
                           "MSJetExecutor");
    }
    if (task != NULL) {
        jet = JET(task);
        jet->player = player;
        /* @hack spelling: offset set ahead of the scale's store, not next
         * to its first use */
        offset = &jet->offset;
        jet->scale = scale;
        jet->kind = kind;
        metal_jet_set_up(task);
        offset->x = jet->scale + JET_START_REACH;
        offset->y = 0.0F;
        offset->z = 0.0F;
        metal_jet_aim(task);
        jet->texture = rand() * (JET_TEXTURE_SPAN * RAND_SCALE);
    }
}

void flame_ring_draw(int player, NJS_MATRIX *matrix)
{
    struct task_work *work;

#ifdef NONMATCHING
    /* this side keeps the tag's result: it calls njFogDisable with nothing, by
     * its own name. this side's objects don't define fog_disable_record,
     * which only symbols.txt places */
    njFogDisable();
#else
    /* @bug mismatched-call: passes the player's record to njFogDisable,
     * which takes nothing and reads no argument (MOV.L in the JSR's slot at
     * 0x8c078494) */
    fog_disable_record(player_tasks[player]->extra.p);
#endif
    if (player_character(player) == PLAYER_SONIC) {
        njSetTexture(&s_efftex_texlist);
    } else {
        njSetTexture(&sh_efftex_texlist);
    }
    work = player_work[player];
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, HALF_TURN - work->ang.y);
    texture_anim_apply(&flame_ring_model, &flame_ring_anim, game_state.steps);
    njCnkEasyDrawObject(&flame_ring_object);
    njFogEnable();
    njPopMatrixEx();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}
