/* the wind, WIND in a stage's object list: rising air that lifts the
 * players and carries stones up */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/wind.h"
#include "shared/wind_stone.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* how far below its base and above its top the wind reaches */
#define REACH 2.0F
/* how much narrower than scl.x the column of a wind without WIND_CLEAR is */
#define COLUMN_MARGIN 20.0F
/* a cone's lift points toward its tip, with its part across scaled by
 * PULL */
#define PULL 2.5F
/* the lift's pulse turns by PULSE_STEP a step, a period of 64 steps. it
 * scales the lift by PULSE_LEAST + PULSE_SCALE * (1 + its cosine), from
 * PULSE_LEAST to 1 */
#define PULSE_STEP 0x400
#define PULSE_LEAST 0.5F
#define PULSE_SCALE 0.25F
/* the lift at the wind's base is BASE_LIFT times lift, and falls by lift over
 * the wind's height */
#define BASE_LIFT 1.5F

/* a wind throws stones up from a disc STONE_DEPTH below its base. the
 * disc's radius is scl.z less STONE_MARGIN, or scl.z where that is no more
 * than STONE_MARGIN */
#define STONE_DEPTH 40.0F
#define STONE_MARGIN 5.0F

/* each player counts its steps in the wind, up to COUNT_MAX. player 0 keeps its
 * count in COUNT_0, and player 1 in COUNT_1. a step in the wind counts where
 * the player jumps, dives, or glides slower than glide_count_speed. any other
 * step in the wind zeroes the count, unless the player holds the A button. a
 * step out of the wind zeroes it whatever the player holds. IN_WIND holds a
 * bit, 1 << slot, for each player in the wind */
#define COUNT_0(work) ((work)->submode)
#define COUNT_1(work) ((work)->tick)
#define COUNT_MAX 127
#define IN_WIND(work) ((work)->ang.z)

/* the task's extra: each player's y as the wind's last step left it. before_0
 * and before_1 keep each player's y before it last changed */
struct wind_heights {
    Float before_0;
    Float height_0;
    Float before_1;
    Float height_1;
};

#define HEIGHTS(task) ((struct wind_heights *)(task)->extra.p)

static Float lift = 0.175F;
/* a player in the wind rises at most rise_speed_limit a step. its acc.y
 * stops at rise_accel_limit, and its y at height_limit */
static Float rise_speed_limit = 3.0F;
static Float rise_accel_limit = 0.175F;
static Float height_limit = 1110.0F;
/* the odds of a stone each step, in a one- and a two-player game */
static Float stone_odds = 0.2F;
static Float stone_odds_two_players = 0.15F;
/* the wind requests PLAYER_REQUEST_UPDRAFT once a player's speed in y passes
 * updraft_speed, or its count reaches updraft_count. the request zeroes the
 * count, unless the player holds the A button */
static Float updraft_speed = 2.0F;
/* a falling player that rose less than stall_rise since the wind's last step
 * takes a shake */
static Float stall_rise = 2.0F;
/* the count at which the wind requests PLAYER_REQUEST_UPDRAFT */
static Sint32 updraft_count = 30;
/* a glide counts only below glide_count_speed */
static Float glide_count_speed = 0.3F;
static Float glide_lift = 1.1F; /* a glide takes glide_lift times the lift */
/* a dive loses its speed in x and z, and keeps dive_brake of its speed in y */
static Float dive_brake = 0.9F;
/* the shake of a stalled player's pad, as canyon_vib_slight_create takes it */
static Sint32 stall_power = 2;
static Sint32 stall_freq = 20;
static Sint32 stall_interval = 20;
static Sint32 stall_count = 3;
/* the shake as a player enters the wind, as vibration_start takes it */
static Sint32 enter_power = 6;
static Sint32 enter_freq = 10;
static Sint32 enter_time = 0;

static void wind_step(struct task *task);
static void wind_destroy(struct task *task);
static void wind_draw(struct task *task);

void wind_init(struct task *task)
{
    if (placement_try_leave(task)) {
        return;
    }
    task->draw = wind_draw;
    task->step = wind_step;
    task->destructor = wind_destroy;
    task->force.p = NULL;
    WIND_STONE_COUNT(task) = 0;
    WIND_STONE_PIECE_COUNT(task) = 0;
    COUNT_0(task->work) = 0;
    COUNT_1(task->work) = 0;
    IN_WIND(task->work) = 0;
    HEIGHTS(task)->before_0 = player_work[0]->pos.y;
    HEIGHTS(task)->height_0 = player_work[0]->pos.y;
    if (game_state.two_players != 0) {
        HEIGHTS(task)->before_1 = player_work[1]->pos.y;
        HEIGHTS(task)->height_1 = player_work[1]->pos.y;
    }
}

static void wind_destroy(struct task *task)
{
    while (task->force.p != NULL) {
        wind_stone_free(task->force.p);
    }
}

/* the lift on the player in slot. it falls with the player's height in the
 * wind, and pulses between half and all of itself */
static Float wind_lift(struct task *task, int slot)
{
    Float lift_here;

    lift_here =
        lift * (BASE_LIFT - ((player_work[slot]->pos.y - task->work->pos.y) /
                             task->work->scl.y));
    return lift_here *
           (((njCos(game_state.steps * PULSE_STEP) + 1.0F) * PULSE_SCALE) +
            PULSE_LEAST);
}

/* lift each player in the wind, then throw up stones and move them */
static void wind_step(struct task *task)
{
    /* @hack spelling: task->work read again for all but pos, where work
     * would do */
    struct task_work *work = task->work;
    int i;
    Uint8 *count;
    int bit;
    Float distance;
    Float narrowing;
    Float tip;
    Float lift_here;
    Float pull;
    Float pull_x;
    Float pull_z;
    Float radius;
    /* @hack type: heights and heights2 volatile, kept only for their
     * stores, where no locals would do */
    struct wind_heights *volatile heights;
    struct wind_heights *volatile heights2;
    Angle ang;
    struct wind_stone *stone;
    /* @hack redundant: center, the wind's middle, which nothing reads */
    NJS_POINT3 center;
    /* @hack spelling: vec, one local for the way to a cone's tip and for a
     * stone's start, where a local for each would do */
    NJS_VECTOR vec;

    if (placement_try_leave(task)) {
        return;
    }
    center = work->pos;
    center.y += 0.5F * task->work->scl.y;
    for (i = 0; i <= 1; i++) {
        if (player_work[i] == NULL) {
            continue;
        }
        if (i == 1 && game_state.two_players == 0) {
            continue;
        }
        count = i == 0 ? (Uint8 *)&COUNT_0(task->work) : &COUNT_1(task->work);
        distance = njSqrt(((player_work[i]->pos.x - work->pos.x) *
                           (player_work[i]->pos.x - work->pos.x)) +
                          ((player_work[i]->pos.z - work->pos.z) *
                           (player_work[i]->pos.z - work->pos.z)));
        narrowing = task->work->scl.x - task->work->scl.z;
        bit = 1 << i;
        if ((!(task->work->ang.x & WIND_CLEAR) &&
             HALF_LINE > player_work[i]->pos.y && narrowing != 0.0F) ||
            ((task->work->ang.x & WIND_CLEAR) && narrowing != 0.0F)) {
            if (task->work->scl.y + work->pos.y > HALF_LINE) {
                tip =
                    task->work->scl.x * ((HALF_LINE - work->pos.y) / narrowing);
            } else {
                tip = task->work->scl.x * (task->work->scl.y / narrowing);
            }
            if (work->pos.y - REACH <= player_work[i]->pos.y &&
                player_work[i]->pos.y <=
                    work->pos.y + task->work->scl.y + REACH &&
                task->work->scl.x / (tip + work->pos.y) *
                        (tip + work->pos.y - player_work[i]->pos.y) >
                    distance) {
                if (!(IN_WIND(task->work) & bit)) {
                    /* @bug logic: shakes player 0's pad at 0x32a of the
                     * object's code, as any player enters the cone. a stall's
                     * shake at 0x56c shakes the stalled player's own pad */
                    vibration_start(0, enter_power, enter_freq, enter_time);
                }
                IN_WIND(task->work) |= bit;
                lift_here = wind_lift(task, i);
                vec.x = work->pos.x - player_work[i]->pos.x;
                vec.y = work->pos.y + tip - player_work[i]->pos.y;
                vec.z = work->pos.z - player_work[i]->pos.z;
                njUnitVector(&vec);
                pull = lift_here / vec.y;
                pull_x = pull * vec.x * PULL;
                pull_z = pull * vec.z * PULL;
                if (player_work[i]->mode == PLAYER_MODE_HUNTER_DIVE) {
                    player_set_speed(
                        i, 0.0F, player_motion[i]->spd.y * dive_brake, 0.0F);
                }
                if (player_work[i]->mode == PLAYER_MODE_JUMP ||
                    player_work[i]->mode == PLAYER_MODE_HUNTER_DIVE ||
                    (player_work[i]->mode == PLAYER_MODE_HUNTER_GLIDE &&
                     player_records[i]->spd.x < glide_count_speed)) {
                    if (*count < COUNT_MAX) {
                        (*count)++;
                    }
                } else if (!(player_pads[i]->on & PDD_DGT_TA)) {
                    *count = 0;
                }
                if (player_motion[i]->spd.y > updraft_speed ||
                    *count >= updraft_count) {
                    player_work[i]->flags |= PLAYER_WORK_REQUEST;
                    player_work[i]->submode = PLAYER_REQUEST_UPDRAFT;
                    if (!(player_pads[i]->on & PDD_DGT_TA)) {
                        *count = 0;
                    }
                }
                if (player_work[i]->mode == PLAYER_MODE_HUNTER_GLIDE &&
                    player_work[i]->submode != PLAYER_REQUEST_UPDRAFT) {
                    player_set_acc(i, 0.0F, lift_here * glide_lift, 0.0F);
                } else {
                    player_set_acc(i, pull_x, lift_here, pull_z);
                    if (player_work[i]->mode == PLAYER_MODE_FALL) {
                        /* @bug logic: shakes player 1's pad at each step of
                         * a stall, where player 0's shakes only as its fall's
                         * animation starts. player 1 skips that test at 0x538
                         * of the object's code */
                        if ((!(player_records[i]->anim.last_frame > 0.0F) &&
                             player_records[i]->anim.frame > 0.0F && i == 0 &&
                             stall_rise > player_work[0]->pos.y -
                                              HEIGHTS(task)->height_0) ||
                            (i == 1 &&
                             stall_rise > player_work[1]->pos.y -
                                              HEIGHTS(task)->height_1)) {
                            canyon_vib_slight_create(i, stall_power, stall_freq,
                                                     stall_interval,
                                                     stall_count);
                        }
                    }
                }
                /* @hack redundant: the tests of the player's records and of
                 * its slot in the three limits, which the loop passed
                 * already */
                if (player_motion[i] != NULL &&
                    player_motion[i]->spd.y > rise_speed_limit &&
                    (i == 0 || (i == 1 && game_state.two_players != 0))) {
                    player_motion[i]->spd.y = rise_speed_limit;
                }
                if (player_motion[i] != NULL &&
                    player_motion[i]->acc.y > rise_accel_limit &&
                    (i == 0 || (i == 1 && game_state.two_players != 0))) {
                    player_motion[i]->acc.y = rise_accel_limit;
                }
                if (player_work[i] != NULL &&
                    player_work[i]->pos.y > height_limit &&
                    (i == 0 || (i == 1 && game_state.two_players != 0))) {
                    player_work[i]->pos.y = height_limit;
                }
            } else {
                IN_WIND(task->work) &= ~bit;
            }
        } else {
            if (work->pos.y - REACH <= player_work[i]->pos.y &&
                player_work[i]->pos.y <=
                    work->pos.y + task->work->scl.y + REACH) {
                if ((!(task->work->ang.x & WIND_CLEAR) &&
                     task->work->scl.x - COLUMN_MARGIN > distance) ||
                    ((task->work->ang.x & WIND_CLEAR) &&
                     task->work->scl.x > distance)) {
                    if (!(IN_WIND(task->work) & bit)) {
                        /* @bug logic: shakes player 0's pad at 0x706 of the
                         * object's code, as any player enters the column */
                        vibration_start(0, enter_power, enter_freq, enter_time);
                    }
                    IN_WIND(task->work) |= bit;
                    lift_here = wind_lift(task, i);
                    if (player_work[i]->mode == PLAYER_MODE_HUNTER_DIVE) {
                        player_set_speed(i, 0.0F,
                                         player_motion[i]->spd.y * dive_brake,
                                         0.0F);
                    }
                    if (player_work[i]->mode == PLAYER_MODE_JUMP ||
                        player_work[i]->mode == PLAYER_MODE_HUNTER_DIVE ||
                        (player_work[i]->mode == PLAYER_MODE_HUNTER_GLIDE &&
                         player_records[i]->spd.x < glide_count_speed)) {
                        if (*count < COUNT_MAX) {
                            (*count)++;
                        }
                    } else if (!(player_pads[i]->on & PDD_DGT_TA)) {
                        *count = 0;
                    }
                    if (player_motion[i]->spd.y > updraft_speed ||
                        *count >= updraft_count) {
                        player_work[i]->flags |= PLAYER_WORK_REQUEST;
                        player_work[i]->submode = PLAYER_REQUEST_UPDRAFT;
                        if (!(player_pads[i]->on & PDD_DGT_TA)) {
                            *count = 0;
                        }
                    }
                    if (player_work[i]->mode == PLAYER_MODE_HUNTER_GLIDE &&
                        player_work[i]->submode != PLAYER_REQUEST_UPDRAFT) {
                        player_set_acc(i, 0.0F, lift_here * glide_lift, 0.0F);
                    } else {
                        player_set_acc(i, 0.0F, lift_here, 0.0F);
                        if (player_work[i]->mode == PLAYER_MODE_FALL) {
                            /* @bug logic: shakes player 1's pad at each step
                             * of a stall, as in the cone. player 1 skips the
                             * test of its fall's animation at 0x88c of the
                             * object's code */
                            if ((player_records[i]->anim.frame == 0.0F &&
                                 i == 0 &&
                                 stall_rise > player_work[0]->pos.y -
                                                  HEIGHTS(task)->height_0) ||
                                (i == 1 &&
                                 stall_rise > player_work[1]->pos.y -
                                                  HEIGHTS(task)->height_1)) {
                                canyon_vib_slight_create(
                                    i, stall_power, stall_freq, stall_interval,
                                    stall_count);
                            }
                        }
                    }
                    /* @hack redundant: the tests of the player's records and of
                     * its slot in the three limits, which the loop passed
                     * already */
                    if (player_motion[i] != NULL &&
                        player_motion[i]->spd.y > rise_speed_limit &&
                        (i == 0 || (i == 1 && game_state.two_players != 0))) {
                        player_motion[i]->spd.y = rise_speed_limit;
                    }
                    if (player_motion[i] != NULL &&
                        player_motion[i]->acc.y > rise_accel_limit &&
                        (i == 0 || (i == 1 && game_state.two_players != 0))) {
                        player_motion[i]->acc.y = rise_accel_limit;
                    }
                    if (player_work[i] != NULL &&
                        player_work[i]->pos.y > height_limit &&
                        (i == 0 || (i == 1 && game_state.two_players != 0))) {
                        player_work[i]->pos.y = height_limit;
                    }
                } else {
                    /* @hack type: bit cast to unsigned for its complement,
                     * here and in the else below, where ~bit would do */
                    IN_WIND(task->work) &= ~(unsigned)bit;
                }
            } else {
                IN_WIND(task->work) &= ~(unsigned)bit;
            }
        }
        if (!(IN_WIND(task->work) & bit)) {
            *count = 0;
        }
    }
    if (!(task->work->ang.x & WIND_CLEAR) &&
        WIND_STONE_COUNT(task) - WIND_STONE_PIECE_COUNT(task) <
            WIND_STONE_WHOLE_MAX &&
        (((Float)rand() * RAND_SCALE < stone_odds &&
          game_state.two_players == 0) ||
         ((Float)rand() * RAND_SCALE < stone_odds_two_players &&
          game_state.two_players != 0))) {
        if (task->work->scl.z > STONE_MARGIN) {
            radius = task->work->scl.z - STONE_MARGIN;
        } else {
            radius = task->work->scl.z;
        }
        radius = njCos((Angle)((Float)rand() * (QUARTER_TURN * RAND_SCALE))) *
                 radius;
        ang = (Angle)((Float)rand() * (FULL_TURN * RAND_SCALE));
        vec.x = work->pos.x + (radius * njCos(ang));
        vec.y = work->pos.y - STONE_DEPTH;
        vec.z = work->pos.z + (radius * njSin(ang));
        wind_stone_create(task, &vec);
    }
    /* @bug memory: reads a stone's next at 0xade of the object's code, after
     * wind_stone_step may have freed the stone. a stone frees itself once it
     * has faded out, and when it breaks into pieces */
#ifdef NONMATCHING
    /* this side repairs the tag above: it reads each stone's next before the
     * stone's step, which may free the stone. a step frees no stone but its
     * own */
    {
        struct wind_stone *next;

        for (stone = WIND_STONES(task); stone != NULL; stone = next) {
            next = stone->next;
            wind_stone_step(stone);
        }
    }
#else
    for (stone = WIND_STONES(task); stone != NULL; stone = stone->next) {
        wind_stone_step(stone);
    }
#endif
    if (HEIGHTS(task)->height_0 != player_work[0]->pos.y) {
        HEIGHTS(task)->before_0 = (heights = task->extra.p)->height_0;
        HEIGHTS(task)->height_0 = player_work[0]->pos.y;
    }
    if (game_state.two_players != 0 &&
        HEIGHTS(task)->height_1 != player_work[1]->pos.y) {
        HEIGHTS(task)->before_1 = (heights2 = task->extra.p)->height_1;
        HEIGHTS(task)->height_1 = player_work[1]->pos.y;
    }
}

static void wind_draw(struct task *task)
{
    struct wind_stone *stone;

    for (stone = WIND_STONES(task); stone != NULL; stone = stone->next) {
        wind_stone_draw(stone);
    }
}
