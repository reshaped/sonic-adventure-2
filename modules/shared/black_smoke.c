/* black smoke: dark puffs of the dust textures, the mud a player kicks up,
 * and the smoke of a broken-down player */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "collision/world.h"
#include "effect/particle.h"
#include "effect/powder.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/dust.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the players who can have a mud task and a smoke task: 0 and 1 */
#define BLACK_SMOKE_PLAYERS 2
/* the task level of a mud or smoke task made without a parent */
#define BLACK_SMOKE_TASK_LEVEL 1
#define GENERATOR_TASK_LEVEL 5

/* a puff starts turned up to TURN degrees either way. it turns up to SPIN
 * degrees a step either way */
#define TURN 10.0F
#define SPIN 0.5F
/* the puffs' color: near black, at an alpha of 0xb0 */
#define SMOKE_COLOR 0xb0080810

/* black_smoke_rising_move and black_smoke_sinking_move swing a puff's
 * edge_angle up to SWING degrees either side of a quarter turn. a swing from
 * one side to the other takes SWING_STEPS steps. the swing's phase, in degrees,
 * starts at random below 2 * SWING */
#define SWING 8
#define SWING_STEPS 60
#define SWING_RATE (2.0F * SWING / SWING_STEPS)
/* a puff keeps its swing's phase in depth_bias, which the draw reads only under
 * PARTICLE_DEPTH. neither group sets that flag */
#define SWING_PHASE(particle) ((particle)->depth_bias)

/* a burst's puff starts with the speed spd, plus RISE times scale upward. it
 * also moves out from the ring's center at RISING_OUT times scale. a sinking
 * burst's puff moves out at SINKING_OUT times scale instead */
#define RISE 0.03F
#define RISING_OUT 0.05F
#define SINKING_OUT 0.07F

/* a mud or smoke task's work keeps its player's slot in tick. a mud task's
 * timer counts the steps since its last splash. its flags keep the player's
 * work's flags of the step before */
#define SLOT(work) ((work)->tick)
/* the player's animation frame of the step before */
#define LAST_FRAME(work) ((work)->scl.x)
/* the player's vertical speed of the step before, without its sign */
#define LAST_FALL_SPEED(work) ((work)->scl.y)

/* a splash of mud is a sinking burst of MUD_PUFFS puffs of size MUD_SCALE. its
 * ring's radius is MUD_RADIUS. the burst's spd is MUD_SPEED_SHARE of the
 * player's speed in x and z, and MUD_KICK times LAST_FALL_SPEED up */
#define MUD_PUFFS 7
#define MUD_SCALE 2.1F
#define MUD_RADIUS 1.1F
#define MUD_SPEED_SHARE 0.3F
#define MUD_KICK 0.1F
/* a mud task splashes as its player lands. it also splashes as a grounded
 * player's run or brake animation starts over, its frame falling below
 * LAST_FRAME. a running splash needs the player at MUD_SPEED or more along its
 * own x. a braking splash throws MUD_BIG_PUFFS puffs at twice
 * MUD_SPEED_SHARE */
#define MUD_SPEED 0.1F
#define MUD_BIG_PUFFS 10
/* a splash needs more than MUD_WAIT steps since the last splash, or since the
 * task began */
#define MUD_WAIT 12

/* a generator's work keeps its player's slot in submode, and the puffs it has
 * left in tick */
#define GENERATOR_SLOT(work) ((work)->submode)
#define PUFFS_LEFT(work) ((work)->tick)
/* a generator's place modes, in its work's mode. each puts the generator at the
 * place on the mech its name gives, which the mech's draws keep in the
 * player's record. any other mode puts it at the cockpit's place, unk_218, plus
 * the work's scl */
enum black_smoke_place_mode {
    MODE_AT_UPPER_BODY = 0,
    MODE_AT_FOOT_0 = 3,
    MODE_AT_FOOT_1 = 4,
    MODE_AT_GUN_MUZZLE = 7,
    MODE_AT_CANNON_MUZZLE = 8,
    MODE_AT_AIM_START = 9,
    MODE_AT_NOZZLE_0 = 10,
    MODE_AT_NOZZLE_1 = 11
};
/* a generator's scl starts at random below PLACE_SPREAD along x and z, and
 * at 0 along y */
#define PLACE_SPREAD 2.0F

/* a generator's mode also counts down the steps to its next puff. it starts at
 * random below FIRST_PUFF_STEPS, and after each puff at PUFF_WAIT plus a random
 * part below PUFF_WAIT_RANGE */
#define FIRST_PUFF_STEPS 16
#define PUFF_WAIT 8
#define PUFF_WAIT_RANGE 24
/* a generator's puffs are sinking puffs of the size PUFF_SCALE. each starts
 * upward at PUFF_RISE */
#define PUFF_SCALE 0.2F
#define PUFF_RISE 0.45F
/* a generator throws no powder at the chance NO_POWDER_CHANCE. otherwise it
 * throws POWDER_SPECKS specks upward at POWDER_RISE as it starts */
#define NO_POWDER_CHANCE 0.9F
#define POWDER_SPECKS 8
#define POWDER_RISE 0.5F

/* a smoke task splits game_state.steps into slots of SLOT_STEPS steps, and the
 * slots into cycles of CYCLE_SLOTS. each slot that smokes starts a generator of
 * SMOKE_PUFFS puffs. let gauge be the mech's gauge in the player's record. the
 * first (SMOKELESS_GAUGE - gauge) * CYCLE_SLOTS / SMOKELESS_GAUGE slots of
 * each cycle smoke, cut toward 0. so every slot smokes at a gauge of 0, and
 * none above 9.375 */
#define SLOT_STEPS 16
#define CYCLE_SLOTS 16
#define SMOKELESS_GAUGE 10.0F
#define SMOKE_PUFFS 2

static int black_smoke_rising_move(struct particle_group *group,
                                   struct particle *particle);
static int black_smoke_sinking_move(struct particle_group *group,
                                    struct particle *particle);
static void muddy_forget(struct task *task);
static void muddy_step(struct task *task);
static void black_smoke_generator_step(struct task *task);
static void broken_down_smoke_forget(struct task *task);
static void broken_down_smoke_step(struct task *task);

/* what particle_create copies into both groups' puffs, which gives them
 * SMOKE_COLOR */
/* clang-format off */
static struct particle black_smoke_template = {
    0, 1.0F, {0.0F, 0.0F, 0.0F}, SMOKE_COLOR, 0.0F, QUARTER_TURN,
    {0.0F, 0.0F, 0.0F}, 0, NULL, 0, 0.0F,
};
static struct particle_group black_smoke_rising_group = {
    PARTICLE_BLEND_ALPHA, &dust_texlist, 0, DUST_FRAMES,
    0.11F, 0.98F, 0.0065F, 0.03F, black_smoke_rising_move, 75000.0F,
    &black_smoke_template, 0, NULL, NULL,
};
static struct particle_group black_smoke_sinking_group = {
    PARTICLE_BLEND_ALPHA, &dust_texlist, 0, DUST_FRAMES,
    0.06F, 0.98F, -0.001F, 0.05F, black_smoke_sinking_move, 22500.0F,
    &black_smoke_template, 0, NULL, NULL,
};
/* clang-format on */
/* each player's mud task, or null */
static struct task *muddy_tasks[BLACK_SMOKE_PLAYERS] = {NULL, NULL};
static Float dat_black_smoke_0[2] = {1.0F, 0.5F};
/* each player's smoke task, or null */
static struct task *broken_down_smoke_tasks[BLACK_SMOKE_PLAYERS] = {NULL, NULL};

struct particle *black_smoke_rising_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                           Float scale)
{
    struct particle *puff;

    puff = particle_create(&black_smoke_rising_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2 * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

struct particle *black_smoke_sinking_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale)
{
    struct particle *puff;

    puff = particle_create(&black_smoke_sinking_group);
    if (puff != NULL) {
        puff->pos = *pos;
        puff->spd = *spd;
        puff->scale = scale;
        puff->ang = RANDOM_ANGLE(TURN);
        puff->ang_spd = RANDOM_ANGLE(SPIN);
        SWING_PHASE(puff) = (Float)rand() * (2 * SWING * RAND_SCALE);
        puff->frame += (Float)rand() * RAND_SCALE;
    }
    return puff;
}

/* the distance from x to the nearest multiple of 2 * half. as x grows, the
 * distance climbs from 0 to half and falls back. half must not be 0 */
static Uint32 triangle_wave(Uint32 x, Uint32 half)
{
    Uint32 period;
    Uint32 distance;

    period = half * 2;
    distance = x % period;
    if (distance >= half) {
        distance = period - distance;
    }
    return distance;
}

/* particle_move with the swing, but the puff stays at its last texture as it
 * fades. each time the frame reaches the group's frames, the step halves the
 * alpha. it sets the frame back to frames - 1. it ends the puff once the alpha
 * is 0 */
static int black_smoke_rising_move(struct particle_group *group,
                                   struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    particle->edge_angle =
        (Angle)((Sint32)(triangle_wave((Uint32)SWING_PHASE(particle),
                                       2 * SWING) -
                         SWING) *
                ANGLE_PER_DEGREE) +
        QUARTER_TURN;
    SWING_PHASE(particle) += SWING_RATE;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        particle->color = (particle->color & ~COLOR_ALPHA) |
                          ((particle->color >> 1) & COLOR_ALPHA);
        if (!(particle->color & COLOR_ALPHA)) {
            return 0;
        }
    }
    return 1;
}

/* particle_move with the swing */
static int black_smoke_sinking_move(struct particle_group *group,
                                    struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    particle->edge_angle =
        (Angle)((Sint32)(triangle_wave((Uint32)SWING_PHASE(particle),
                                       2 * SWING) -
                         SWING) *
                ANGLE_PER_DEGREE) +
        QUARTER_TURN;
    SWING_PHASE(particle) += SWING_RATE;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}

void black_smoke_rising_burst(NJS_POINT3 *pos, NJS_VECTOR *spd, Float radius,
                              Float scale, int count)
{
    NJS_VECTOR out = {0.0F, RISE, RISING_OUT};
    NJS_POINT3 offset = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 point;
    NJS_VECTOR puff_spd;
    Angle gap;
    Angle ang;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    gap = FULL_TURN / count;
    ang = 0;
    offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    out.y *= scale;
    out.z *= scale;
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        /* @bug logic: the puffs don't lie a gap apart around the ring, since
         * the turns add up twice. ang already sums the gaps, and each
         * njRotateY adds ang to the matrix's turn, which the loop never sets
         * back (the JSR to njRotateY at 0x4ae of the object's code) */
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &offset, &point);
        njCalcVector(NULL, &out, &puff_spd);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        puff_spd.x += spd->x;
        puff_spd.y += spd->y;
        puff_spd.z += spd->z;
        black_smoke_rising_create(&point, &puff_spd, scale);
        ang += gap;
    }
    njPopMatrixEx();
}

/* black_smoke_rising_burst, but of black_smoke_sinking_create's puffs, which
 * move out at SINKING_OUT times scale. ang gains a random part below half a gap
 * before each puff */
static void black_smoke_sinking_burst(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float radius, Float scale, int count)
{
    NJS_VECTOR out = {0.0F, RISE, SINKING_OUT};
    NJS_POINT3 offset = {0.0F, 0.0F, 0.0F};
    NJS_POINT3 point;
    NJS_VECTOR puff_spd;
    Angle gap;
    Angle ang;
    /* @hack type: i, a Uint8, where an int would do for a count below 256 */
    Uint8 i;

    gap = FULL_TURN / count;
    ang = 0;
    offset.z = radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    out.y *= scale;
    out.z *= scale;
    /* NOLINTNEXTLINE(bugprone-too-small-loop-variable) */
    for (i = 0; i < count; i++) {
        ang += (Float)gap * (Float)rand() * (0.5F * RAND_SCALE);
        /* @bug logic: the turns add up twice, as in black_smoke_rising_burst
         * (the JSR to njRotateY at 0x63e of the object's code) */
        njRotateY(NULL, ang);
        njCalcPoint(NULL, &offset, &point);
        njCalcVector(NULL, &out, &puff_spd);
        point.x += pos->x;
        point.y += pos->y;
        point.z += pos->z;
        puff_spd.x += spd->x;
        puff_spd.y += spd->y;
        puff_spd.z += spd->z;
        black_smoke_sinking_create(&point, &puff_spd, scale);
        ang += gap;
    }
    njPopMatrixEx();
}

struct task *muddy_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < BLACK_SMOKE_PLAYERS && muddy_tasks[player] == NULL) {
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, muddy_step, parent);
        } else {
            task = task_create(TASK_WORK, BLACK_SMOKE_TASK_LEVEL, muddy_step,
                               "MuddyExec");
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = muddy_forget;
            SLOT(work) = player;
            muddy_tasks[player] = task;
            return task;
        }
    }
    return NULL;
}

/* empty the player's entry of muddy_tasks where it holds task, so the player
 * can have another. broken_down_smoke_forget does the same for
 * broken_down_smoke_tasks */
static void muddy_forget(struct task *task)
{
    if (muddy_tasks[SLOT(task->work)] == task) {
        muddy_tasks[SLOT(task->work)] = NULL;
    }
}

static void muddy_step(struct task *task)
{
    struct task_work *work = task->work;
    int player = SLOT(work);
    struct task_work *player_task_work = player_work[player];
    Float frame;
    /* @hack redundant: last_frame, a read of LAST_FRAME(work) that nothing
     * uses */
    Float last_frame = LAST_FRAME(work);
    NJS_VECTOR spd;

    if (player_task_work == NULL) {
        task_destroy(task);
        return;
    }
    /* @bug memory: reads the player's record with no test for null, where
     * broken_down_smoke_step tests it (MOV.L @(R0,R4),R2 at 0x806 of the
     * object's code) */
    frame = player_records[player]->anim.frame;
    if ((player_records[player]->ahead.above_flags & WORLD_FLAT) &&
        (player_records[player]->ahead.below_flags & WORLD_DUSTY)) {
        if (((work->flags ^ player_task_work->flags) & player_task_work->flags &
             PLAYER_WORK_ON_GROUND) &&
            work->timer > MUD_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= MUD_SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * MUD_KICK;
            spd.z *= MUD_SPEED_SHARE;
            black_smoke_sinking_burst(&player_task_work->pos, &spd, MUD_RADIUS,
                                      MUD_SCALE, MUD_PUFFS);
            work->timer = 0;
        }
        if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
            player_task_work->mode == PLAYER_MODE_RUN &&
            player_records[player]->spd.x >= MUD_SPEED && frame >= 0.0F &&
            LAST_FRAME(work) > frame && work->timer > MUD_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= MUD_SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * MUD_KICK;
            spd.z *= MUD_SPEED_SHARE;
            black_smoke_sinking_burst(&player_task_work->pos, &spd, MUD_RADIUS,
                                      MUD_SCALE, MUD_PUFFS);
            work->timer = 0;
        } else if ((player_task_work->flags & PLAYER_WORK_ON_GROUND) &&
                   player_task_work->mode == PLAYER_MODE_BRAKE &&
                   frame >= 0.0F && LAST_FRAME(work) > frame &&
                   work->timer > MUD_WAIT) {
            spd = player_motion[player]->spd;
            spd.x *= 2.0F * MUD_SPEED_SHARE;
            spd.y = LAST_FALL_SPEED(work) * MUD_KICK;
            spd.z *= 2.0F * MUD_SPEED_SHARE;
            black_smoke_sinking_burst(&player_task_work->pos, &spd, MUD_RADIUS,
                                      MUD_SCALE, MUD_BIG_PUFFS);
            work->timer = 0;
        }
    }
    work->flags = player_task_work->flags;
    LAST_FALL_SPEED(work) = njAbs(player_motion[player]->spd.y);
    /* @hack spelling: the increment written out, where work->timer++ would
     * do */
    work->timer = work->timer + 1;
    LAST_FRAME(work) = frame;
}

/* put the generator at the place its mode names, in the record of the player
 * in GENERATOR_SLOT. that record is player_tasks[player]->extra, which
 * player_records[player] holds too. every path here tests
 * player_records[player] for null first */
static void black_smoke_generator_place(struct task_work *work)
{
    /* @hack spelling: player, a local for GENERATOR_SLOT(work), where
     * GENERATOR_SLOT(work) read in place would do */
    int player = GENERATOR_SLOT(work);
    struct player *record = player_tasks[player]->extra.p;

    switch (work->mode) {
    case MODE_AT_UPPER_BODY:
        work->pos = record->unk_230;
        break;
    case MODE_AT_FOOT_0:
        work->pos = record->unk_200;
        break;
    case MODE_AT_FOOT_1:
        work->pos = record->unk_20c;
        break;
    case MODE_AT_GUN_MUZZLE:
        work->pos = record->unk_23c;
        break;
    case MODE_AT_CANNON_MUZZLE:
        work->pos = record->unk_248;
        break;
    case MODE_AT_AIM_START:
        work->pos = record->unk_254;
        break;
    case MODE_AT_NOZZLE_0:
        work->pos = record->by_character.mech.nozzle0_pos;
        break;
    case MODE_AT_NOZZLE_1:
        work->pos = record->by_character.mech.nozzle1_pos;
        break;
    default:
        work->pos = record->unk_218;
        njAddVector(&work->pos, &work->scl);
        break;
    }
}

static void black_smoke_generator_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_VECTOR spd;
    /* @hack spelling: count, a local the test decrements, where
     * PUFFS_LEFT(work) <= 1 tested before PUFFS_LEFT(work)-- would do */
    int count;
    /* @hack spelling: player, a local for GENERATOR_SLOT(work), where
     * GENERATOR_SLOT(work) read in place would do */
    int player = GENERATOR_SLOT(work);

    if (player_records[player] == NULL) {
        task_destroy(task);
        return;
    }
    if (game_state.time_stopped) {
        return;
    }
    if (work->mode <= 0) {
        /* @bug logic: every puff rises from the mech's upper body, not from
         * the place black_smoke_generator_create picked. mode counts down to
         * the puff, so it is 0 here, which names MODE_AT_UPPER_BODY. only the
         * powder leaves from the create's place (the BSR at 0xb36 of the
         * object's code) */
        black_smoke_generator_place(work);
        spd.x = 0.0F;
        spd.y = PUFF_RISE;
        spd.z = 0.0F;
        black_smoke_sinking_create(&work->pos, &spd, PUFF_SCALE);
        count = PUFFS_LEFT(work);
        if (--count <= 0) {
            task_kill(task);
            return;
        }
        PUFFS_LEFT(work) = count;
        work->mode =
            ((Float)rand() * (PUFF_WAIT_RANGE * RAND_SCALE)) + PUFF_WAIT;
    } else {
        work->mode--;
    }
}

/* make a generator of count puffs from player's record. its random first mode
 * picks the place any powder leaves from */
static void black_smoke_generator_create(int player_index, int count)
{
    /* @hack spelling: player, a copy of the parameter, where the parameter
     * read in place would do */
    int player = player_index;
    struct task *task;
    struct task_work *work;
    NJS_VECTOR spd;
    /* @hack spelling: pos, a copy of the work's pos, where &work->pos passed
     * in place would do */
    NJS_POINT3 pos;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, GENERATOR_TASK_LEVEL,
                            black_smoke_generator_step,
                            "BlackSmokeGeneratorExec")) != NULL) {
        work = task->work;
        GENERATOR_SLOT(work) = player;
        PUFFS_LEFT(work) = count;
        work->mode = (Float)rand() * (FIRST_PUFF_STEPS * RAND_SCALE);
        work->scl.x = (Float)rand() * (PLACE_SPREAD * RAND_SCALE);
        work->scl.z = (Float)rand() * (PLACE_SPREAD * RAND_SCALE);
        black_smoke_generator_place(work);
        if ((Float)rand() * RAND_SCALE >= NO_POWDER_CHANCE) {
            pos = work->pos;
            spd.x = 0.0F;
            spd.y = POWDER_RISE;
            spd.z = 0.0F;
            powder_create(player, POWDER_SPECKS, &pos, &spd);
        }
    }
}

struct task *broken_down_smoke_create(int player, struct task *parent)
{
    struct task *task;
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work;

    if (player < BLACK_SMOKE_PLAYERS &&
        broken_down_smoke_tasks[player] == NULL) {
        if (parent != NULL) {
            task = task_create_child(TASK_WORK, broken_down_smoke_step, parent);
        } else {
            task = task_create(TASK_WORK, BLACK_SMOKE_TASK_LEVEL,
                               broken_down_smoke_step, "BrokenDownSmokeExec");
        }
        if (task != NULL) {
            work = task->work;
            task->destructor = broken_down_smoke_forget;
            SLOT(work) = player;
            broken_down_smoke_tasks[player] = task;
            return task;
        }
    }
    return NULL;
}

static void broken_down_smoke_forget(struct task *task)
{
    if (broken_down_smoke_tasks[SLOT(task->work)] == task) {
        broken_down_smoke_tasks[SLOT(task->work)] = NULL;
    }
}

static void broken_down_smoke_step(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: player, a local for SLOT(work), where SLOT(work) read in
     * place would do */
    int player = SLOT(work);
    /* @hack spelling: record, a local for the player's record, where
     * player_records[player] read in place would do */
    struct player *record = player_records[player];
    int slots;

    if (record == NULL) {
        task_destroy(task);
        return;
    }
    if (game_state.time_stopped) {
        return;
    }
    slots = (SMOKELESS_GAUGE - record->gauge) * (CYCLE_SLOTS / SMOKELESS_GAUGE);
    if (slots <= 0) {
        return;
    }
    if ((game_state.steps % SLOT_STEPS) == 0 &&
        ((game_state.steps % (CYCLE_SLOTS * SLOT_STEPS)) / SLOT_STEPS) <
            (Uint32)slots) {
        black_smoke_generator_create(player, SMOKE_PUFFS);
    }
}
