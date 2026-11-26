/* the fire of fire.h. the fireball is the task BogyFireBowlExec, and a wave
 * BogyFireBreathBExec. a wall's lowest flame is BogyFireWallExec where it has
 * no parent. the breath, its drops and the flames above a wall's lowest are
 * unnamed children */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include "boss_bigbogy/boss.h"
#include "boss_bigbogy/arena.h"
#include "boss_bigbogy/fire.h"
#include "boss_bigbogy/stage.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#define FIRE_LEVEL 3 /* task_create's level */
#define FIRE_PRIORITY 0
#define FIRE_VOLUME 120
/* ids in bigbogy_sounds */
#define RELEASE_SOUND 11
#define IMPACT_SOUND 12
#define BREATH_SOUND 15
/* the sprites' attributes. a wave draws in white, and the others in the
 * constant material their draws set */
#define FIRE_SPRITE (NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA)
#define FIRE_MATERIAL (FIRE_SPRITE | NJD_SPRITE_COLOR)
#define ADDITIVE(blend)                                                        \
    do {                                                                       \
        (blend)(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);                \
        (blend)(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);                \
    } while (0)
#define NORMAL_BLEND(blend)                                                    \
    do {                                                                       \
        (blend)(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);                \
        (blend)(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);        \
    } while (0)
#define SQUARE(value) ((value) * (value))

/* a flame hurts player 0 within FLAME_REACH of its pos. it knocks player 0 at
 * KNOCK_SPEED along the heading of the boss's circle at the flame, and
 * KNOCK_RISE up */
#define FLAME_REACH 15.0F
#define KNOCK_SPEED 5.0F
#define KNOCK_RISE 1.0F
/* a wave, a drop or a fading flame goes once its scale falls below
 * GONE_SCALE */
#define GONE_SCALE 0.1F
/* the ball and a drop show BALL_CELLS cells in turn, one a step */
#define BALL_CELLS 10

/* a ball's record, in its task's motion. in flight, aim is the share of its
 * speed that heads for target. the rest goes along the heading of the boss's
 * circle at the ball. past an aim of 1 the ball keeps its last velocity */
struct fireball {
    Float aim_step; /* what aim gains a step */
    Float aim;
    Float speed;
    const NJS_POINT3 *hand; /* the point a held ball follows */
    Uint8 unused[24];
    NJS_POINT3 target;
    NJS_VECTOR velocity;
};
/* a drop's record, in its task's motion. a wave and the breath take one too,
 * and use none of it */
struct fire_drop {
    Uint8 unused[8];
    NJS_POINT3 target; /* it lands once it falls below target.y */
    NJS_VECTOR velocity;
};

/* a held ball grows by GROWTH a step to FULL_SIZE, HAND_HEIGHT above its
 * hand. nothing reads SHRINKAGE: ball_shrink takes SHRINK_STEP */
enum ball_setting { FULL_SIZE, GROWTH, SHRINKAGE, HAND_HEIGHT };
static Float ball_settings[4] = {0.5F, 0.02F, 0.05F, 30.0F};
static Float ball_alpha = 1.0F;
static Float drop_frames = 20.0F; /* the steps a drop takes to fall */
/* how far ahead of the breath its drops and its fronts of waves aim */
/* @hack type: one-element arrays, where Floats would do */
static Float drop_reach[1] = {100.0F};
static Float wave_reach[1] = {80.0F};
static Float unused[2] = {0};
static Angle sweep_step = 0x100; /* the breath's sweep a step */

static NJS_TEXANIM ball_cells[BALL_CELLS] = {
#include "boss_bigbogy/fire/ball_cells.inc"
};
/* @hack type: a one-element array, where the sprite itself would do */
static NJS_SPRITE ball_sprite[1] = {
    {{0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, 0, &bigbogy_effect_texlist, ball_cells}};
static struct task *wave_create(const NJS_POINT3 *position, Angle travelled);
static void breath_destroy(struct task *task);

static void hurt_player(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR velocity;
    /* @hack redundant: rotation, three angles set to 0 that nothing reads */
    struct angles rotation;
    Angle heading;
    /* @hack redundant: the test of WORK_HURT before it is set */
    if (!(player_work[0]->flags & WORK_HURT)) {
        player_work[0]->flags |= WORK_HURT;
    }
    heading = bigbogy_arena_tangent(task, &work->pos);
    velocity.x = njSin(heading) * KNOCK_SPEED;
    velocity.y = KNOCK_RISE;
    velocity.z = njCos(heading) * KNOCK_SPEED;
    rotation.x = rotation.y = rotation.z = 0;
    player_set_speed(0, velocity.x, velocity.y, velocity.z);
}

/* hurt player 0 where it lies within radius of task's pos. hurt_in_cylinder
 * hurts it within radius in x and z, and within height above or below pos.
 * each returns TRUE where it hurts player 0. neither hurts in BIGBOGY_DEFEAT */
static Bool hurt_in_sphere(struct task *task, Float radius)
{
    struct task_work *work = task->work;
    NJS_VECTOR offset;
    Float distance;
    /* @hack type: bigbogy_mode() cast to Sint8, where its int would do */
    if ((Sint8)bigbogy_mode() == BIGBOGY_DEFEAT) {
        return FALSE;
    }
    offset.x = player_work[0]->pos.x - work->pos.x;
    offset.y = player_work[0]->pos.y - work->pos.y;
    offset.z = player_work[0]->pos.z - work->pos.z;
    distance = njScalor(&offset);
    if (radius > distance) {
        hurt_player(task);
        return TRUE;
    }
    return FALSE;
}

static Bool hurt_in_cylinder(struct task *task, Float radius, Float height)
{
    struct task_work *work = task->work;
    NJS_VECTOR offset;
    Float distance;
    /* @hack type: bigbogy_mode() cast to Sint8, where its int would do */
    if ((Sint8)bigbogy_mode() == BIGBOGY_DEFEAT) {
        return FALSE;
    }
    offset.x = player_work[0]->pos.x - work->pos.x;
    offset.y = player_work[0]->pos.y - work->pos.y;
    offset.z = player_work[0]->pos.z - work->pos.z;
    distance = njSqrt((offset.x * offset.x) + (offset.z * offset.z));
    if (distance < radius && height > _builtin_fabsf(offset.y)) {
        hurt_player(task);
        return TRUE;
    }
    return FALSE;
}

/* a ball's modes, in its work's mode. nothing sets TIMED. a ball's scl.x is
 * its scale, and scl.y its change a step. ang.x is its cell */
enum ball_mode { HELD, THROWN, SHRINKING, TIMED };
/* a ball shrinks by SHRINK_STEP a step until less than a step is left */
#define SHRINK_STEP 0.05F
static void ball_held(struct task *task)
{
    struct task_work *work = task->work;
    struct fireball *ball = task->motion.p;
    switch (work->submode) {
    case 0:
        work->scl.y = ball_settings[GROWTH];
        work->submode++;
        /* fall through */
    case 1:
        if (work->scl.x < ball_settings[FULL_SIZE]) {
            work->scl.x += work->scl.y;
            if (work->scl.x >= ball_settings[FULL_SIZE]) {
                work->scl.x = ball_settings[FULL_SIZE];
                work->submode++;
            }
        }
        break;
    /* @hack redundant: an empty case for the grown ball */
    case 2:
        break;
    }
    work->pos = *ball->hand;
    work->pos.y += ball_settings[HAND_HEIGHT];
}

static void ball_shrink(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->submode) {
    case 0:
        work->scl.y = SHRINK_STEP;
        work->submode++;
        /* fall through */
    case 1:
        work->scl.x -= work->scl.y;
        if (work->scl.x < SHRINK_STEP) {
            task_kill(task);
        }
        break;
    }
}

/* a thrown ball flies at BALL_SPEED. where it goes out it raises a wall of
 * BALL_WALL flames, the lowest at BALL_WALL_SCALE. where it goes out inside
 * BIGBOGY_BALL_OUTER, the pad rumbles at IMPACT_POWER and IMPACT_FREQ for
 * IMPACT_TIME */
#define BALL_SPEED 10.0F
#define BALL_WALL 8
#define BALL_WALL_SCALE 1.2F
#define IMPACT_POWER 2
#define IMPACT_FREQ 7
#define IMPACT_TIME 1
/* one axis of ball's velocity. the share aim of its speed runs along direct,
 * toward its target, and the rest along tangent */
#define FLIGHT_VELOCITY(ball, direct, tangent, axis)                           \
    (((direct).axis * (ball)->speed * (ball)->aim) +                           \
     ((tangent).axis * (ball)->speed * (1.0F - (ball)->aim)))
#define IMPACT(work)                                                           \
    sound_play_at(IMPACT_SOUND, NULL, FIRE_PRIORITY, FIRE_VOLUME, &(work)->pos)
#define FIRE_WALL(work)                                                        \
    bigbogy_fire_wall(NULL, &(work)->pos, BALL_WALL, BALL_WALL_SCALE)
#define RUMBLE() vibration_start(0, IMPACT_POWER, IMPACT_FREQ, IMPACT_TIME)
#define SHRINK(work)                                                           \
    do {                                                                       \
        (work)->mode = SHRINKING;                                              \
        (work)->submode = 0;                                                   \
    } while (0)

static void ball_flight(struct task *task)
{
    struct task_work *work = task->work;
    struct fireball *ball = task->motion.p;
    /* @hack redundant: unused_frame, which only sizes the frame */
    Float unused_frame[10];
    NJS_VECTOR direct;
    NJS_VECTOR tangent;
    Float distance;
    Angle heading;
    (void)unused_frame;
    switch (work->submode) {
    case 0:
        /* aim reaches 1 in the steps a straight flight to target takes */
        distance = njSqrt(SQUARE(ball->target.x - work->pos.x) +
                          SQUARE(ball->target.y - work->pos.y) +
                          SQUARE(ball->target.z - work->pos.z));
        ball->speed = BALL_SPEED;
        ball->aim = 0.0F;
        ball->aim_step = ball->speed / distance;
        work->submode++;
        /* fall through */
    case 1:
        direct.x = ball->target.x - work->pos.x;
        direct.y = ball->target.y - work->pos.y;
        direct.z = ball->target.z - work->pos.z;
        njUnitVector(&direct);
        heading = bigbogy_arena_tangent(NULL, &work->pos);
        tangent.x = njSin(heading);
        tangent.y = 0.0F;
        tangent.z = njCos(heading);
        ball->velocity.x = FLIGHT_VELOCITY(ball, direct, tangent, x);
        ball->velocity.y = FLIGHT_VELOCITY(ball, direct, tangent, y);
        ball->velocity.z = FLIGHT_VELOCITY(ball, direct, tangent, z);
        njAddVector(&work->pos, &ball->velocity);
        ball->aim += ball->aim_step;
        if (ball->aim > 1.0F) {
            work->submode++;
        }
        break;
    case 2:
        njAddVector(&work->pos, &ball->velocity);
        break;
    }
    if (work->pos.y < bigbogy_arena_center.y) {
        IMPACT(work);
        FIRE_WALL(work);
        RUMBLE();
        SHRINK(work);
    }
    distance = bigbogy_arena_distance(&work->pos, NULL);
    if (BIGBOGY_BALL_INNER > distance) {
        FIRE_WALL(work);
        RUMBLE();
        IMPACT(work);
        SHRINK(work);
    }
    if (distance > BIGBOGY_BALL_OUTER) {
        IMPACT(work);
        FIRE_WALL(work);
        SHRINK(work);
    }
    if (hurt_in_sphere(task, FLAME_REACH) == TRUE) {
        FIRE_WALL(work);
        IMPACT(work);
        RUMBLE();
        SHRINK(work);
    }
}

void bigbogy_fireball_throw(struct task *ball)
{
    struct task_work *work = ball->work;
    struct fireball *flight = ball->motion.p;
    NJS_POINT3 player;
    NJS_POINT3 ahead;
    Angle tangent;
    Angle gap;
    player = player_work[0]->pos;
    tangent = bigbogy_arena_tangent(ball, &work->pos);
    gap = angle_distance(tangent, (Angle)(atan2f(player.x - work->pos.x,
                                                 player.z - work->pos.z) *
                                          ANGLE_PER_RADIAN));
    if (gap < THIRD_TURN) {
        flight->target = player;
    } else {
        bigbogy_arena_advance(ball, &work->pos, -THIRD_TURN, &ahead);
        flight->target = ahead;
    }
    sound_play_at(RELEASE_SOUND, NULL, FIRE_PRIORITY, FIRE_VOLUME, &work->pos);
    work->mode = THROWN;
    work->submode = 0;
}

void bigbogy_fireball_dismiss(struct task *ball)
{
    struct task_work *work = ball->work;
    SHRINK(work);
}

static void ball_timer(struct task *task)
{
    struct task_work *work = task->work;
    if (work->timer-- == 0) {
        bigbogy_fireball_dismiss(task);
        bigbogy_fire_wall(NULL, &work->pos, BALL_WALL, 1.0F);
    }
}

static void ball_step(struct task *task)
{
    struct task_work *work = task->work;
    if (game_state.time_stopped) {
        return;
    }
    switch (work->mode) {
    case HELD:
        ball_held(task);
        break;
    case SHRINKING:
        ball_shrink(task);
        break;
    case THROWN:
        ball_flight(task);
        break;
    case TIMED:
        ball_timer(task);
        break;
    }
    work->ang.x++;
    if (work->ang.x == BALL_CELLS) {
        work->ang.x = 0;
    }
}

static void ball_draw(struct task *task)
{
    struct task_work *work = task->work;
    void (*blend)(Int, Int);
    ball_sprite->sx = ball_sprite->sy = work->scl.x;
    ball_sprite->p = work->pos;
    /* @hack spelling: blend, a pointer to njColorBlendingMode, where calling
     * it by name would do */
    blend = njColorBlendingMode;
    ADDITIVE(blend);
    constant_material_set(ball_alpha, 1, 1, 1);
    njSetTexture(&bigbogy_effect_texlist);
    njDrawSprite3D(ball_sprite, work->ang.x, FIRE_MATERIAL);
    constant_material_set(0, 0, 0, 0);
    NORMAL_BLEND(blend);
}
static void ball_destroy(struct task *task)
{
}

struct task *bigbogy_fireball_create(struct task *boss, const NJS_POINT3 *hand)
{
    struct task *task;
    struct task_work *work;
    struct fireball *ball;
    if (boss) {
        task =
            task_create(TASK_WORK, FIRE_LEVEL, ball_step, "BogyFireBowlExec");
        if (!task) {
            return NULL;
        }
        work = task->work;
        ball = syCalloc(1, sizeof(*ball));
        if (!ball) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = ball;
        work->scl.x = work->scl.y = work->scl.z = 0.0F;
        work->ang.z = (Angle)boss; /* nothing reads it */
        ball->hand = hand;
        task->draw = ball_draw;
        task->destructor = ball_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where boss is null. the
     * branch at 0x8c507b50 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* a wall's flame. timer counts its steps. submode holds the flames still to
 * come above it, scl.x its scale, and scl.z its alpha. ang.x is its cell,
 * from 0 to WALL_LAST_CELL, a cell each two steps. cell n shows the texture
 * WALL_TEXTURE + n */
/* in its work's flags: it has raised the next flame */
#define NEXT_MADE 0x100
/* a flame raises the next as its timer passes NEXT_STEP. the next stands
 * NEXT_RISE of this flame's scales above it, at NEXT_SCALE of its scale */
#define NEXT_STEP 2
#define NEXT_RISE 12.0F
#define NEXT_SCALE 0.95F
/* a flame lasts FLAME_STEPS, and its alpha falls evenly to 0 over them */
#define FLAME_STEPS 32
#define WALL_TEXTURE 10
#define WALL_LAST_CELL 15
/* a flame with more than SOUND_ABOVE flames to come above it plays
 * IMPACT_SOUND as it starts */
#define SOUND_ABOVE 5
static void wall_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 next;
    if (!game_state.time_stopped && !game_state.unk_09) {
        if (work->timer++ == NEXT_STEP && !(work->flags & NEXT_MADE)) {
            work->flags |= NEXT_MADE;
            next = work->pos;
            next.y += work->scl.x * NEXT_RISE;
            bigbogy_fire_wall(task, &next, work->submode,
                              work->scl.x * NEXT_SCALE);
        }
        if (work->timer >= FLAME_STEPS) {
            task_kill(task);
            return;
        }
        work->ang.x = work->timer >> 1;
        if (work->ang.x > WALL_LAST_CELL) {
            work->ang.x = WALL_LAST_CELL;
        }
        work->scl.z = 1.0F - (work->timer * (1.0F / FLAME_STEPS));
        if (work->scl.z < 0.0F) {
            work->scl.z = 0.0F;
            task_kill(task);
        }
        if (hurt_in_sphere(task, FLAME_REACH) == TRUE) {
            task_kill(task);
        }
    }
}

static void wall_draw(struct task *task)
{
    struct task_work *work = task->work;
    static const NJS_TEXANIM first_cell = {
#include "boss_bigbogy/fire/first_cell.inc"
    };
    static const NJS_SPRITE wall = {{0, 0, 0}, 0, 0, 0, &bigbogy_effect_texlist,
                                    NULL};
    NJS_TEXANIM cell;
    NJS_SPRITE sprite;
    NJS_TEXANIM *frame;
    /* @hack spelling: image, a pointer to sprite, where sprite itself would
     * do */
    NJS_SPRITE *image = &sprite;
    cell = first_cell;
    /* @hack spelling: frame, a pointer to cell taken before the sprite's
     * copy, where &cell would do */
    frame = &cell;
    sprite = wall;
    image->sx = image->sy = work->scl.x;
    image->p = work->pos;
    image->tanim = frame;
    image->tanim->texid = work->ang.x + WALL_TEXTURE;
    ADDITIVE(njColorBlendingMode);
    constant_material_set(work->scl.z, work->scl.z, work->scl.z, work->scl.z);
    njSetTexture(&bigbogy_effect_texlist);
    njDrawSprite3D(image, 0, FIRE_MATERIAL);
    constant_material_set(0, 0, 0, 0);
    NORMAL_BLEND(njColorBlendingMode);
}

static void wall_destroy(struct task *task)
{
}

struct task *bigbogy_fire_wall(struct task *parent, const NJS_POINT3 *position,
                               Sint8 count, Float scale)
{
    struct task *task;
    struct task_work *work;
    if (count <= 0) {
        return NULL;
    }
    if (parent) {
        task = task_create_child(TASK_WORK, wall_step, parent);
    } else {
        task =
            task_create(TASK_WORK, FIRE_LEVEL, wall_step, "BogyFireWallExec");
    }
    if (!task) {
        return NULL;
    }
    work = task->work;
    work->pos = *position;
    work->submode = count - 1;
    work->timer = 0;
    work->scl.x = scale;
    work->scl.z = 1.0F;
    if (work->submode > SOUND_ABOVE) {
        IMPACT(work);
    }
    task->draw = wall_draw;
    task->destructor = wall_destroy;
    return task;
}

/* each step a fading flame shrinks by FADE_SHRINK. until its parent's
 * submode reaches FADE_END, it first grows by FADE_GROWTH, below FADE_SIZE */
#define FADE_SHRINK 0.05F
#define FADE_END 2
#define FADE_GROWTH 0.01F
#define FADE_SIZE 1.5F
void bigbogy_flame_fade(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: phase, a local for the parent's submode, where the
     * test of the submode would do */
    Sint8 phase;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    switch (work->submode) {
    case 0:
        work->submode++;
        /* fall through */
    case 1:
        if (work->scl.x < FADE_SIZE) {
            work->scl.x += FADE_GROWTH;
        }
        phase = task->parent->work->submode;
        if (phase == FADE_END) {
            work->submode++;
        }
        /* fall through */
    case 2:
        work->scl.x -= FADE_SHRINK;
        if (work->scl.x < GONE_SCALE) {
            task_kill(task);
        }
        break;
    }
    work->ang.x++;
    if (work->ang.x == BALL_CELLS) {
        work->ang.x = 0;
    }
}

/* a wave: a flame of the breath that grows and rises in place, waits, then
 * shrinks and sinks. as its scale falls below WAVE_NEXT in BIGBOGY_BREATH, it
 * starts the next wave. the next lies WAVE_TURN further about the center, the
 * way the boss goes. ang.z is the turn its front has run from its first wave. a
 * front starts no wave at HALF_TURN or past it. scl.x is a wave's scale, scl.y
 * its change a step, and scl.z its largest. ang.x is its cell, below WAVE_CELLS
 */
#define WAVE_NEXT 0.4F
#define WAVE_TURN 0x800
#define WAVE_CELLS 15
#define WAVE_HOLD 1 /* a wave waits WAVE_HOLD + 1 steps at its largest */
/* a wave's pos.y: WAVE_BASE above the floor, and higher by WAVE_RISE times
 * its scale squared */
#define WAVE_BASE 10.0F
#define WAVE_RISE 8.0F
#define WAVE_HEIGHT(scale)                                                     \
    (bigbogy_arena_center.y + ((scale) * (scale) * WAVE_RISE) + WAVE_BASE)
/* a wave hurts player 0 within WAVE_REACH of its pos. it also hurts it within
 * WAVE_RADIUS in x and z and WAVE_TALL above or below pos */
#define WAVE_REACH 50.0F
#define WAVE_RADIUS 30.0F
#define WAVE_TALL 100.0F
/* a wave starts at a scale of WAVE_START. its largest is WAVE_LARGEST less
 * a random 0 to WAVE_SPREAD. it grows and shrinks by WAVE_GROWTH of the
 * difference a step */
#define WAVE_START 0.2F
#define WAVE_LARGEST 3.0F
#define WAVE_SPREAD 0.5F
#define WAVE_GROWTH 0.1F
#define BREATH_PRIORITY 1
#define WAVE_SOUND_STEPS 15
static void wave_step(struct task *task)
{
    struct task_work *work = task->work;
    Sint8 mode;
    int active;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    switch (work->submode) {
    case 0:
        work->submode++;
        /* fall through */
    case 1:
        work->scl.x += work->scl.y;
        work->pos.y = WAVE_HEIGHT(work->scl.x);
        if (work->scl.x < work->scl.z) {
            break;
        }
        work->scl.x = work->scl.z;
        work->timer = WAVE_HOLD;
        work->submode++;
        break;
    case 2:
        if (work->timer-- == 0) {
            work->submode++;
        }
        break;
    case 3:
        work->scl.x -= work->scl.y;
        work->pos.y = WAVE_HEIGHT(work->scl.x);
        if (work->scl.x < WAVE_NEXT) {
            mode = bigbogy_mode();
            /* @hack spelling: active, the test's result compared with 1,
             * where testing mode == BIGBOGY_BREATH would do */
            active = mode == BIGBOGY_BREATH;
            if (active == 1) {
                wave_create(&work->pos, work->ang.z + WAVE_TURN);
            }
            work->submode++;
        }
        break;
    case 4:
        work->scl.x -= work->scl.y;
        work->pos.y = WAVE_HEIGHT(work->scl.x);
        if (work->scl.x < GONE_SCALE) {
            task_kill(task);
        }
        break;
    }
    /* @hack redundant: the tests of hurt_in_sphere's and hurt_in_cylinder's
     * results, which choose nothing */
    if (hurt_in_sphere(task, WAVE_REACH) == TRUE) {
    }
    if (hurt_in_cylinder(task, WAVE_RADIUS, WAVE_TALL) == TRUE) {
    }
    work->ang.x++;
    if (work->ang.x == WAVE_CELLS) {
        work->ang.x = 0;
    }
}

static void wave_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_TEXANIM wave_cells[16] = {
#include "boss_bigbogy/fire/wave_cells.inc"
    };
    static const NJS_SPRITE wave = {{0, 0, 0}, 0, 0, 0, &bigbogy_effect_texlist,
                                    NULL};
    NJS_SPRITE sprite;
    /* @hack spelling: image, a pointer to sprite, where sprite itself would
     * do */
    NJS_SPRITE *image = &sprite;
    void (*blend)(Int, Int);
    sprite = wave;
    image->sx = image->sy = work->scl.x;
    image->p = work->pos;
    image->tanim = wave_cells;
    /* @hack spelling: blend, a pointer to njColorBlendingMode, where calling
     * it by name would do */
    blend = njColorBlendingMode;
    ADDITIVE(blend);
    njSetTexture(&bigbogy_effect_texlist);
    njDrawSprite3D(image, work->ang.x, FIRE_SPRITE);
    constant_material_set(0, 0, 0, 0);
    NORMAL_BLEND(blend);
}

static struct task *wave_create(const NJS_POINT3 *position, Angle travelled)
{
    struct task *task;
    struct task_work *work;
    struct fire_drop *drop;
    NJS_POINT3 next;
    NJS_VECTOR radial;
    Float radius;
    if (travelled >= HALF_TURN) {
        return NULL;
    }
    task = task_create(TASK_WORK, FIRE_LEVEL, wave_step, "BogyFireBreathBExec");
    if (!task) {
        return NULL;
    }
    work = task->work;
    drop = syCalloc(1, sizeof(*drop));
    if (!drop) {
        task_kill(task);
        return NULL;
    }
    task->motion.p = drop;
    if (travelled) {
        /* the next wave keeps position's distance from the center */
        bigbogy_arena_advance(task, position, -WAVE_TURN, &next);
        radius = bigbogy_arena_distance(position, NULL);
        bigbogy_arena_distance(&next, &radial);
        bigbogy_arena_along(&radial, radius, &next);
        work->pos = next;
        work->pos.y = bigbogy_arena_center.y + WAVE_BASE;
    } else {
        work->pos = *position;
    }
    work->ang.z = travelled;
    work->scl.x = WAVE_START;
    work->scl.z = WAVE_LARGEST - (rand() * (WAVE_SPREAD * RAND_SCALE));
    work->scl.y = (work->scl.z - work->scl.x) * WAVE_GROWTH;
    /* the key, wave_step's address plus the turn run, gives the waves at
     * each turn one sound between them */
    sound_play_at_for(
        BREATH_SOUND, (const void *)((Sint32)task->step + work->ang.z),
        BREATH_PRIORITY, FIRE_VOLUME, WAVE_SOUND_STEPS, &work->pos);
    task->draw = wave_draw;
    task->destructor = breath_destroy;
    return task;
}

/* a drop of the breath's fire. it starts at a scale of DROP_START, and grows
 * by DROP_GROWTH a step to 1 as it falls. it lands at DROP_LANDED, then
 * shrinks by DROP_GROWTH a step. a drop that reaches player 0 raises a wall
 * of DROP_WALL flames and goes */
#define DROP_GROWTH 0.02F
#define DROP_LANDED 2.0F
#define DROP_HOLD 300
#define DROP_HOLD_GROWTH 0.01F
#define DROP_WALL 3
#define DROP_START 0.3F
/* a drop's phases, in its work's submode */
enum drop_phase { DROP_NEW, DROP_FALLING, DROP_HOLDING, DROP_SHRINKING };
static void drop_step(struct task *task)
{
    struct task_work *work = task->work;
    struct fire_drop *drop = task->motion.p;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    switch (work->submode) {
    case DROP_NEW:
        work->submode++;
        /* fall through */
    case DROP_FALLING:
        work->scl.x += DROP_GROWTH;
        if (work->scl.x > 1.0F) {
            work->scl.x = 1.0F;
        }
        if (work->pos.y < drop->target.y) {
            work->pos.y = drop->target.y;
            work->scl.x = DROP_LANDED;
            work->timer = DROP_HOLD;
            /* @bug logic: a landed drop shrinks at once, past DROP_HOLDING's
             * hold of DROP_HOLD steps. the landing sets DROP_SHRINKING, at
             * 0x8c5083ba */
            work->submode = DROP_SHRINKING;
        } else {
            njAddVector(&work->pos, &drop->velocity);
        }
        break;
    case DROP_HOLDING:
        if ((work->scl.x += DROP_HOLD_GROWTH) > DROP_LANDED) {
            work->scl.x = DROP_LANDED;
        }
        if (work->timer-- == 0) {
            work->submode++;
        }
        break;
    case DROP_SHRINKING:
        work->scl.x -= DROP_GROWTH;
        if (work->scl.x < GONE_SCALE) {
            task_kill(task);
        }
        break;
    }
    if (hurt_in_sphere(task, FLAME_REACH) == TRUE) {
        bigbogy_fire_wall(NULL, &work->pos, DROP_WALL, 1.0F);
        task_kill(task);
    }
    work->ang.x++;
    if (work->ang.x == BALL_CELLS) {
        work->ang.x = 0;
    }
}

static struct task *drop_create(struct task *parent, const NJS_POINT3 *target,
                                Sint8 kind)
{
    struct task *task;
    struct task_work *work;
    struct fire_drop *drop;
    NJS_VECTOR delta;
    Float rate;
    if (parent) {
        task = task_create_child(TASK_WORK, drop_step, parent);
        if (!task) {
            return NULL;
        }
        work = task->work;
        drop = syCalloc(1, sizeof(*drop));
        if (!drop) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = drop;
        drop->target = *target;
        work->pos = parent->work->pos;
        work->tick = kind; /* nothing reads it */
        /* @hack spelling: drop_frames read through *&, where drop_frames
         * would do */
        work->timer = *&drop_frames;
        delta.x = drop->target.x - work->pos.x;
        delta.y = drop->target.y - work->pos.y;
        delta.z = drop->target.z - work->pos.z;
        rate = 1.0F / work->timer;
        drop->velocity.x = delta.x * rate;
        drop->velocity.y = delta.y * rate;
        drop->velocity.z = delta.z * rate;
        work->scl.x = DROP_START;
        work->ang.z = (Angle)&parent->work->submode; /* nothing reads it */
        task->draw = ball_draw;
        task->destructor = breath_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where parent is null.
     * the branch at 0x8c5084a6 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* at its FIRST_FRONT and SECOND_FRONT steps the breath sends a front of
 * waves. the front aims at the point wave_reach ahead of the breath, along
 * the circle's heading. its waves lie on that point's bearing, FRONT_GAP
 * apart. the first lies FIRST_FRONT_RADIUS or SECOND_FRONT_RADIUS from the
 * center, and the last short of FRONT_END. each is made FRONT_LIFT above the
 * floor, a height its first step changes */
#define FIRST_FRONT 84.0F
#define SECOND_FRONT 86.0F
#define FIRST_FRONT_RADIUS 130.0F
#define SECOND_FRONT_RADIUS 155.0F
#define FRONT_LIFT 40.0F
#define FRONT_GAP 50.0F
#define FRONT_END 260.0F
/* while the sweep runs, a drop falls each DROP_PERIOD steps before
 * DROP_STEPS. it falls toward the point drop_reach ahead of the breath, along
 * the sweep and DROP_LIFT above the floor */
#define DROP_PERIOD 10
#define DROP_STEPS 180
#define DROP_LIFT 25.0F
/* a drop's kind is 1 each KIND_PERIOD steps, but in submode KINDLESS, which
 * nothing gives the breath. nothing reads a drop's kind */
#define KIND_PERIOD 30
#define KINDLESS 2
/* send a front of task's waves, the first start from the center. it works in
 * breath_step's heading, target, radial, radius and source */
#define SEND_FRONT(task, work, start)                                          \
    do {                                                                       \
        heading = bigbogy_arena_tangent((task), &(work)->pos);                 \
        target.x = (work)->pos.x + (njSin(heading) * wave_reach[0]);           \
        target.z = (work)->pos.z + (njCos(heading) * wave_reach[0]);           \
        bigbogy_arena_distance(&target, &radial);                              \
        radius = (start);                                                      \
        do {                                                                   \
            bigbogy_arena_along(&radial, radius, &source);                     \
            source.y = bigbogy_arena_center.y + FRONT_LIFT;                    \
            wave_create(&source, 0);                                           \
            radius += FRONT_GAP;                                               \
        } while (radius < FRONT_END);                                          \
    } while (0)

/* the breath. timer counts its steps. ang.y is its sweep, a turn off the
 * heading of the boss's circle at it. ang.z is the sweep's change a step. the
 * sweep runs from -EIGHTH_TURN to EIGHTH_TURN and back, then stops */
static void breath_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 target;
    NJS_VECTOR radial;
    NJS_POINT3 source;
    Angle heading;
    Angle tangent;
    Float radius;
    Sint8 mode;
    int active;
    if (game_state.time_stopped) {
        return;
    }
    if (game_state.unk_09) {
        return;
    }
    work->timer++;
    /* @hack type: the timer compared with Floats, where integers would do */
    if (work->timer == FIRST_FRONT) {
        SEND_FRONT(task, work, FIRST_FRONT_RADIUS);
    } else if (work->timer == SECOND_FRONT) {
        SEND_FRONT(task, work, SECOND_FRONT_RADIUS);
    }
    work->ang.y += work->ang.z;
    if (work->ang.y > EIGHTH_TURN) {
        work->ang.y = EIGHTH_TURN;
        work->ang.z = -sweep_step;
    } else if (work->ang.y < -EIGHTH_TURN) {
        /* the sweep is over. with ang.z 0 no drop falls, so nothing shows
         * ang.y's jump to the far side */
        work->ang.y = EIGHTH_TURN;
        work->ang.z = 0;
    }
    if (work->timer % DROP_PERIOD == 0 && work->timer < DROP_STEPS &&
        work->ang.z != 0) {
        tangent = bigbogy_arena_tangent(task, &work->pos);
        heading = tangent + work->ang.y;
        target.x = work->pos.x + (njSin(heading) * drop_reach[0]);
        target.y = bigbogy_arena_center.y + DROP_LIFT;
        target.z = work->pos.z + (njCos(heading) * drop_reach[0]);
        if (work->timer % KIND_PERIOD == 0 && work->submode != KINDLESS) {
            drop_create(task, &target, 1);
        } else {
            drop_create(task, &target, 0);
        }
    }
    mode = bigbogy_mode();
    /* @hack spelling: active, the test's result spelled 1 or 0 and compared
     * with 0, where testing mode != BIGBOGY_BREATH would do */
    active = mode == BIGBOGY_BREATH ? 1 : 0;
    if (active == 0) {
        task_kill(task);
    }
}

static void breath_destroy(struct task *task)
{
}

struct task *bigbogy_breath_create(struct task *boss,
                                   const NJS_POINT3 *position)
{
    struct task *task;
    struct task_work *work;
    struct fire_drop *drop;
    Angle heading;
    if (boss) {
        task = task_create_child(TASK_WORK, breath_step, boss);
        if (!task) {
            return NULL;
        }
        work = task->work;
        drop = syCalloc(1, sizeof(*drop));
        if (!drop) {
            task_kill(task);
            return NULL;
        }
        task->motion.p = drop;
        work->pos = *position;
        heading = bigbogy_arena_tangent(task, &work->pos);
        work->pos.x += njSin(heading) * BIGBOGY_BREATH_LEAD;
        work->pos.z += njCos(heading) * BIGBOGY_BREATH_LEAD;
        work->ang.y = -EIGHTH_TURN;
        work->ang.z = sweep_step;
        task->destructor = breath_destroy;
        return task;
    }
    /* @bug uninitialized: the call returns task unset where boss is null. the
     * branch at 0x8c508874 skips every assignment of it */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: it returns null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */
