/* what the enemies share, as enemy.h declares it */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "effect/burst.h"
#include "enemy/enemy.h"
#include "enemy/scrap_models.h"
#include "object/animal.h"
#include "object/chaos_drive.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "player/walker.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

#ifdef NONMATCHING
/* port_view_x, the window this side's test of the screen takes */
#include "engine/dc/port.h"
#endif

#define GROUPS 256 /* each value of the Sint8 group, taken unsigned */
/* the bits of a grouped enemy's ang.y that hold its heading. the low byte
 * holds its group until a grouped enemy_work_alloc call takes it */
#define HEADING_BITS 0xff00

/* the share of an overlap with a ceiling or a floor that a call of
 * enemy_meet_stage undoes */
#define OVERLAP_UNDONE 0.3F
/* the share of spd by which a hold at a ledge steps the enemy back from
 * last_pos */
#define LEDGE_STEP_BACK 0.4F
/* under ENEMY_TILT, how far a landing turns ang.x and ang.z toward the
 * floor's tilt in a step */
#define TILT_STEP 0x80
/* a surface on a side is a wall where its normal's part along the side lies
 * below -WALL_FACING. the normal then points within 60 degrees of straight
 * back */
#define WALL_FACING 0.5F
/* the share of an overlap with a wall that a call of enemy_meet_stage
 * undoes */
#define WALL_UNDONE 0.35F

#define SHADOW_LIFT 0.01F

/* a player under either bit bounces off no enemy */
#define BOUNCE_BLOCKING_BITS (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)
/* a bounce's speed against gravity, in units of world_gravity */
#define BOUNCE_SPEED 2.0F
#define BOUNCE_VIBRATION_POWER 6
#define BOUNCE_VIBRATION_FREQ 15

/* the squash_spd enemy_take_hit gives a hurt enemy */
#define HIT_SQUASH_SPD 0.35F
/* each enemy_squash_step takes from squash_spd SQUASH_DAMPING of itself, and
 * SQUASH_PULL of scl.y less 1 */
#define SQUASH_DAMPING 0.2F
#define SQUASH_PULL 0.32F

/* a piece flies, then fades, and a scrap only flies. under
 * game_state.time_stopped, each holds still in PIECE_FLY and PIECE_FADE, and
 * moves in the _UNHELD modes. one made while time_stopped is set flies in
 * PIECE_FLY_UNHELD. a piece leaves either flight for PIECE_FADE, so nothing
 * sets PIECE_FADE_UNHELD */
enum piece_mode { PIECE_FLY, PIECE_FADE, PIECE_FLY_UNHELD, PIECE_FADE_UNHELD };
#define PIECE_LEVEL 3 /* a piece's task's level, and a scrap's */
/* a flying piece's or scrap's timer counts its steps. the step that finds
 * the timer at PIECE_STEPS ends a scrap. it moves a piece once more, and
 * turns it to fading. a fading piece's alpha, in 255ths, starts at
 * PIECE_ALPHA and falls by PIECE_FADE_STEP a step */
#define PIECE_STEPS 120
#define PIECE_ALPHA 255
#define PIECE_FADE_STEP 10
/* a piece starts with spd.x and spd.z each within half PIECE_SPREAD of 0.
 * its spd.y starts from PIECE_RISE up to PIECE_RISE plus PIECE_RISE_SPAN.
 * each part of its spin lies within half PIECE_SPIN of 0 */
#define PIECE_SPREAD 2.0F
#define PIECE_RISE 0.9F
#define PIECE_RISE_SPAN 2
#define PIECE_SPIN (20 * ANGLE_PER_DEGREE)
/* a scrap's, as for a piece. each part of its spin lies from SCRAP_SPIN up to
 * twice it */
#define SCRAP_SPREAD 3.0F
#define SCRAP_RISE 0.1F
#define SCRAP_RISE_SPAN 3
#define SCRAP_SPIN (50 * ANGLE_PER_DEGREE)
/* scraps_throw picks a scrap's model from the first SCRAP_MODELS of
 * scrap_models */
#define SCRAP_MODELS 2
/* how far a piece's spd.y falls in a step, and a scrap's */
#define PIECE_GRAVITY 0.06F
#define SCRAP_GRAVITY (PIECE_GRAVITY * 1.5F)
/* a piece looks for a floor on each step its count passes
 * PIECE_FLOOR_COUNT. that step starts the count again at 0. the count rises
 * by 1 a step, so a search comes every PIECE_FLOOR_COUNT + 2 steps */
#define PIECE_FLOOR_COUNT 9
/* the search for a floor starts PIECE_FLOOR_PROBE above the piece's pos */
#define PIECE_FLOOR_PROBE 4.0F
/* a falling piece bounces where the floor it found lies within
 * PIECE_BOUNCE_GAP of its pos in y. the bounce reverses spd.y, and keeps
 * PIECE_BOUNCE of it */
#define PIECE_BOUNCE_GAP 1.0F
#define PIECE_BOUNCE 0.8F
/* piece i starts its count at i % PIECE_COUNT_SPREAD, so a call's pieces
 * search on different steps */
#define PIECE_COUNT_SPREAD 10
/* a flying piece may make a burst on a step where its timer has no bit of
 * PIECE_BURST_MASK. it does where a random number from 0 up to 1 lies above
 * PIECE_BURST_THRESHOLD */
#define PIECE_BURST_MASK 0xf
#define PIECE_BURST_THRESHOLD 0.9F
#define PIECE_BURST_SCALE 5.0F

#define BREAK_SCRAPS 16

/* the size of enemy_defeat's burst, and the sound it plays with it */
#define DEFEAT_BURST_SCALE 16.0F
#define BURST_SOUND 0x8015
#define BURST_VOLUME 127
/* a dmyEnemy task's level. its timer starts at DUMMY_FIRST_WAIT, and later
 * at DUMMY_WAIT, each plus a random part below DUMMY_WAIT_SPAN. its submode
 * starts at DUMMY_WAITS plus a random part below DUMMY_WAITS_SPAN */
#define DUMMY_LEVEL 3
#define DUMMY_FIRST_WAIT 4.0F
#define DUMMY_WAIT 6.0F
#define DUMMY_WAIT_SPAN 8
#define DUMMY_WAITS 1.0F
#define DUMMY_WAITS_SPAN 2

/* the radius of the sphere an enemy ball draws, and its center's height
 * above the work's pos. both match fall_step_small's sphere */
#define BALL_SPHERE_RADIUS 2.5F
#define BALL_LEVEL 2
#define BALL_GROWTH 0.01F
#define BALL_TURN 0x200
/* an enemy ball looks for its holder in the player slots below
 * BALL_HOLDER_SLOTS */
#define BALL_HOLDER_SLOTS 4
/* a ball bursts on a step that finds either bit in its work's flags.
 * fall_step_small clears FALL_UNK_8, one of FALL_STEP_BITS, unless a player
 * holds the ball */
#define BALL_BURST_FLAGS (FALL_LANDED | FALL_UNK_8)
#define BALL_BURST_SCALE 2.0F
#define BALL_SHAPES 2

/* the height in pixels, from the screen's top, of the part
 * point_in_upper_screen accepts */
#define UPPER_SCREEN_HEIGHT 400.0F

#define NEAR_PLAYER_OFFSETS 4

/* debug_draw_model_unlit's constant material: its alpha, then its red,
 * green and blue */
#define UNLIT_ALPHA 0.3F
#define UNLIT_RED 0.2F
#define UNLIT_GREEN 0.1F
#define UNLIT_BLUE 0.4F

int (*enemy_hit_hook)(NJS_POINT3 *pos) = NULL;
Sint16 enemy_switch_bits = 0;

/* the number of enemies each group counts */
static Sint8 enemy_group_counts[GROUPS];

/* the offsets enemy_set_target_near_player picks from, before the camera's
 * heading turns them */
static NJS_POINT3 near_player_offsets[NEAR_PLAYER_OFFSETS] = {
    {40.0F, 0.0F, -20.0F},
    {20.0F, 0.0F, -40.0F},
    {-20.0F, 0.0F, -40.0F},
    {-40.0F, 0.0F, -20.0F},
};
/* scraps_throw never picks the third */
static NJS_CNK_MODEL *scrap_models[3] = {
#include "enemy/enemy/scrap_models.inc"
};
/* clang-format off */
static struct collision_shape enemy_ball_shapes[BALL_SHAPES] = {
    {6, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_HURTS_ALL,
     SHAPE_UNTURNED | SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0,
     {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS,
     SHAPE_ATTACK_LEVEL(3) | SHAPE_HURTS_ENEMIES,
     SHAPE_UNTURNED | SHAPE_UNK_2000, {0.0F, 2.0F, 0.0F}, 0.2F, 4.5F, 0.0F, 0,
     {0, 0, 0}},
};
/* clang-format on */

/* a piece or a scrap, in its task's extra */
struct piece {
    Sint8 mode; /* an enum piece_mode */
    /* the count toward the piece's next search for a floor */
    Sint8 count;
    Sint16 timer; /* its steps of flight, then its alpha as it fades */
    struct angles spin; /* what ang turns by in a step */
    struct angles ang;
    NJS_VECTOR spd;
    NJS_POINT3 pos;
    NJS_TEXLIST *texlist; /* a piece's texture list. a scrap's is null */
    NJS_CNK_MODEL *model;
};

/* an enemy ball's record, in its task's motion */
struct enemy_ball {
    /* first, for fall_step_small reads the task's motion as a task_motion */
    struct task_motion motion;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *texlist;
    NJS_VECTOR scl; /* the scale the work's scl moves toward */
    /* where object is drawn from the sphere's center, turned and scaled
     * with the ball */
    NJS_VECTOR offset;
};

/* no header declares these. as statics, SHC would inline enemy_restore_pos
 * at its one call, and leave out the rest, which nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void enemy_restore_pos(struct task_work *work, struct enemy_work *enemy);
Float enemy_target_distance(struct task_work *work, struct enemy_work *enemy);
Float enemy_target_distance2(struct task_work *work, struct enemy_work *enemy);
Float enemy_home_distance(struct task_work *work, struct enemy_work *enemy);
Float enemy_home_distance_flat(struct task_work *work,
                               struct enemy_work *enemy);
/* enemy_turn_to_player, turning away: aim_y becomes the heading away from
 * player. it returns what enemy_aim_at_player returns */
Angle enemy_turn_from_player(struct task_work *work, struct enemy_work *enemy,
                             int player);
/* NOLINTEND(misc-use-internal-linkage) */

void enemy_groups_clear(void)
{
    int i;

    for (i = 0; i < GROUPS; i++) {
        enemy_group_counts[i] = 0;
    }
    enemy_switch_bits = 0;
}

int enemy_group_empty(int group)
{
    /* @hack spelling: the test's result returned through an if, where
     * returning the comparison would do */
    if (enemy_group_counts[(Uint8)group] == 0) {
        return 1;
    }
    return 0;
}

struct enemy_work *enemy_work_alloc(struct task *task)
{
    struct enemy_work *enemy;

    enemy = syCalloc(1, sizeof(struct enemy_work));
    if (enemy == NULL) {
        task_kill(task);
        return NULL;
    }
    task->motion.p = enemy;
    enemy->home = task->work->pos;
    enemy->radius = 15.0F;
    enemy->probe_offset.y = 5.0F;
    enemy->top = 10.0F;
    enemy->bottom = -1.0F;
    enemy->step_down = 50.0F;
    enemy->floor_bounce = 0.9F;
    enemy->friction = 0.8F;
    enemy->unk_0e4 = 0.1F;
    enemy->wall_bounce = 0.7F;
    enemy->wall_turn = 0x400;
    enemy->damage = 3.0F;
    enemy->shot_by = -1;
    enemy->group = ENEMY_NO_GROUP;
    enemy->sight_angle = (Angle)(80 * ANGLE_PER_DEGREE);
    enemy->sight_distance2 = 150.0F * 150.0F;
    enemy->near_distance2 = 35.0F * 35.0F;
    enemy->turn_speed = 0x100;
    enemy->flags |= ENEMY_LEDGE_HOLD_OFF;
    /* @hack redundant: the clear of ENEMY_HIT and ENEMY_PROBE_FLAT in the
     * zeroed work */
    enemy->flags &= ~(ENEMY_HIT | ENEMY_PROBE_FLAT);
    return enemy;
}

struct enemy_work *enemy_work_alloc_grouped(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    enemy = enemy_work_alloc(task);
    if (enemy == NULL) {
        return NULL;
    }
    enemy->group = work->ang.y;
    work->ang.y &= HEADING_BITS;
    if (enemy->group != ENEMY_NO_GROUP) {
        hunt_add_place(EMERALD_KIND_INENEMY, enemy->group, &work->pos);
    }
    enemy_group_counts[(Uint8)enemy->group]++;
    enemy->flags |= ENEMY_COUNTED;
    return enemy;
}

struct enemy_work *enemy_work_alloc_grouped_no_hunt(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    enemy = enemy_work_alloc(task);
    if (enemy == NULL) {
        return NULL;
    }
    enemy->group = work->ang.y;
    work->ang.y &= HEADING_BITS;
    enemy_group_counts[(Uint8)enemy->group]++;
    enemy->flags |= ENEMY_COUNTED;
    return enemy;
}

void enemy_destroy(struct task *task)
{
    struct enemy_work *enemy = task->motion.p;

#ifdef NONMATCHING
    if (enemy == NULL) {
        /* Sound channels use the task work as their key. They outlive a
         * failed enemy work allocation until the task is destroyed. */
        sound_stop_key(task->work);
        return;
    }
#endif
    if (enemy->lock_task != NULL) {
        lockon_release(enemy);
    }
    if (enemy->flags & ENEMY_COUNTED) {
        enemy_group_counts[(Uint8)enemy->group]--;
        enemy->flags &= ~ENEMY_COUNTED;
    }
    sound_stop_key(task->work);
    /* @hack redundant: the return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static int enemies_may_look(void)
{
    if (game_state.mini_event_running == 1 || game_state.cleared == 1) {
        return 0;
    }
    return 1;
}

int enemy_find_player(struct task_work *work, struct enemy_work *enemy)
{
    int i;
    struct task_work *player_task_work;
    Float distance;
    Angle heading;
    /* @hack type: off declared Uint16, where Angle would do */
    Uint16 off;

    /* @hack spelling: the result compared with 1, where the result alone
     * would do */
    if (enemies_may_look() != 1) {
        return 0;
    }
    for (i = 0; i < GAME_PLAYERS; i++) {
        player_task_work = player_work[i];
        if (player_task_work != NULL) {
            distance = ((player_task_work->pos.x - work->pos.x) *
                        (player_task_work->pos.x - work->pos.x)) +
                       ((player_task_work->pos.y - work->pos.y) *
                        (player_task_work->pos.y - work->pos.y)) +
                       ((player_task_work->pos.z - work->pos.z) *
                        (player_task_work->pos.z - work->pos.z));
            if (distance < enemy->near_distance2) {
                return i + 1;
            }
            if (distance < enemy->sight_distance2) {
                /* @hack spelling: heading, a local for ang.y read before the
                 * arctangent, where ang.y in place would do */
                heading = work->ang.y;
                off = angle_distance(
                    (Angle)(atan2f(player_task_work->pos.x - work->pos.x,
                                   player_task_work->pos.z - work->pos.z) *
                            ANGLE_PER_RADIAN) -
                        QUARTER_TURN,
                    heading);
                if (off < enemy->sight_angle) {
                    return i + 1;
                }
            }
        }
    }
    return 0;
}

int enemy_find_player_near(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: range, a local for near_distance2 read before the
     * loop, where near_distance2 in place would do */
    Float range = enemy->near_distance2;
    int found = 0;
    int i;
    struct task_work *player_task_work;
    Float distance;

    /* @hack spelling: the result compared with 1, where the result alone
     * would do */
    if (enemies_may_look() != 1) {
        return 0;
    }
    for (i = 0; i < GAME_PLAYERS; i++) {
        player_task_work = player_work[i];
        if (player_task_work != NULL) {
            distance = ((player_task_work->pos.x - work->pos.x) *
                        (player_task_work->pos.x - work->pos.x)) +
                       ((player_task_work->pos.z - work->pos.z) *
                        (player_task_work->pos.z - work->pos.z));
            if (distance < range) {
                found = i + 1;
            }
        }
    }
    return found;
}

int enemy_find_player_near_home(struct task_work *work,
                                struct enemy_work *enemy)
{
    /* @hack spelling: range, a local for near_distance2 read before the
     * loop, where near_distance2 in place would do */
    Float range = enemy->near_distance2;
    int found = 0;
    int i;
    struct task_work *player_task_work;
    Float distance;

    /* @hack spelling: the result compared with 1, where the result alone
     * would do */
    if (enemies_may_look() != 1) {
        return 0;
    }
    for (i = 0; i < GAME_PLAYERS; i++) {
        player_task_work = player_work[i];
        if (player_task_work != NULL) {
            distance = ((player_task_work->pos.x - enemy->home.x) *
                        (player_task_work->pos.x - enemy->home.x)) +
                       ((player_task_work->pos.z - enemy->home.z) *
                        (player_task_work->pos.z - enemy->home.z));
            if (distance < range) {
                found = i + 1;
            }
        }
    }
    return found;
}

int enemy_find_player_near_offset(struct task_work *work,
                                  struct enemy_work *enemy)
{
    /* @hack spelling: range, a local for near_distance2 read before the
     * loop, where near_distance2 in place would do */
    Float range = enemy->near_distance2;
    int found = 0;
    int i;
    struct task_work *player_task_work;
    Float distance;

    /* @hack spelling: the result compared with 1, where the result alone
     * would do */
    if (enemies_may_look() != 1) {
        return 0;
    }
    for (i = 0; i < GAME_PLAYERS; i++) {
        player_task_work = player_work[i];
        if (player_task_work != NULL) {
            distance =
                ((player_task_work->pos.x - enemy->home.x - enemy->offset.x) *
                 (player_task_work->pos.x - enemy->home.x - enemy->offset.x)) +
                ((player_task_work->pos.z - enemy->home.z - enemy->offset.z) *
                 (player_task_work->pos.z - enemy->home.z - enemy->offset.z));
            if (distance < range) {
                found = i + 1;
            }
        }
    }
    return found;
}

static void enemy_probe_stage(struct task_work *work, struct enemy_work *enemy)
{
    struct surface_probe *probe = &enemy->probe;
    /* @hack redundant: unused, a zeroed vector that nothing reads */
    NJS_VECTOR unused = {0.0F, 0.0F, 0.0F};
    NJS_VECTOR turned;
    NJS_VECTOR offset;

    /* @hack spelling: offset, a copy of probe_offset, where probe_offset
     * passed in place would do */
    offset = enemy->probe_offset;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, &offset, &turned);
    njPopMatrixEx();
    probe->pos.x = work->pos.x + turned.x;
    probe->pos.y = work->pos.y + turned.y;
    probe->pos.z = work->pos.z + turned.z;
    if (enemy->flags & ENEMY_PROBE_FLAT) {
        if (surface_probe_sides_flat(probe)) {
            /* @hack spelling: the bit set by an assignment of an or, where
             * |= would do */
            enemy->flags = enemy->flags | ENEMY_ON_FLAT;
        } else {
            enemy->flags &= ~ENEMY_ON_FLAT;
        }
    } else {
        surface_probe_sides(probe);
    }
}

static void enemy_meet_ceiling(struct task_work *work, struct enemy_work *enemy)
{
    Float top = enemy->probe.pos.y + enemy->top;
    struct surface_side *ceiling = &enemy->probe.sides[SURFACE_POS_Y];

    enemy->flags &= ~ENEMY_UNDER_CEILING;
    if (top > ceiling->pos) {
        enemy->spd.y = 0.0F;
        work->pos.y -= (top - ceiling->pos) * OVERLAP_UNDONE;
        enemy->flags |= ENEMY_UNDER_CEILING;
    }
}

/* land the enemy on the floor the probe found, or hold it at a ledge */
static void enemy_meet_floor(struct task_work *work, struct enemy_work *enemy)
{
    Float bottom = enemy->probe.pos.y + enemy->bottom;
    struct surface_side *floor = &enemy->probe.sides[SURFACE_NEG_Y];

    if ((enemy->flags & ENEMY_HOLD_AT_LEDGE) &&
        !(enemy->flags & ENEMY_LEDGE_HOLD_OFF) &&
        (bottom - enemy->step_down > floor->pos || !floor->found)) {
        enemy_restore_pos(work, enemy);
        enemy->flags |= ENEMY_LEDGE_HOLD_OFF | ENEMY_ON_FLOOR;
        work->pos.x -= enemy->spd.x * LEDGE_STEP_BACK;
        work->pos.z -= enemy->spd.z * LEDGE_STEP_BACK;
        enemy->spd.x = 0.0F;
        enemy->spd.y = 0.0F;
        enemy->spd.z = 0.0F;
        enemy->floor_normal.x = 0.0F;
        enemy->floor_normal.y = 1.0F;
        enemy->floor_normal.z = 0.0F;
    } else {
        enemy->flags &= ~ENEMY_ON_FLOOR;
        if (floor->pos > bottom) {
            enemy->spd.y *= -enemy->floor_bounce;
            enemy->spd.x *= enemy->friction;
            enemy->spd.z *= enemy->friction;
            work->pos.y += (floor->pos - bottom) * OVERLAP_UNDONE;
            if (enemy->flags & ENEMY_TILT) {
                work->ang.x =
                    angle_approach(work->ang.x, floor->ang_x, TILT_STEP);
                work->ang.z =
                    angle_approach(work->ang.z, floor->ang_z, TILT_STEP);
            }
            enemy->floor_normal = floor->normal;
            enemy->flags |= ENEMY_ON_FLOOR;
            enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
        } else {
            enemy->floor_normal.x = 0.0F;
            enemy->floor_normal.y = 1.0F;
            enemy->floor_normal.z = 0.0F;
        }
    }
}

/* meet the wall the probe found on side, if any. (dir_x, 0, dir_z) points
 * toward the side. the wall's plane passes through (plane_x, point.y, plane_z),
 * and plane_x and plane_z may read wall. the macro uses enemy_meet_walls'
 * locals */
#define MEET_WALL(side, dir_x, dir_z, plane_x, plane_z)                        \
    do {                                                                       \
        if (probe->sides[side].found) {                                        \
            NJS_VECTOR dir = {dir_x, 0.0F, dir_z};                             \
            NJS_POINT3 nearest;                                                \
            NJS_VECTOR push;                                                   \
            NJS_VECTOR axis;                                                   \
            NJS_VECTOR reflect;                                                \
                                                                               \
            wall = &probe->sides[side];                                        \
            if (njInnerProduct(&dir, &wall->normal) < -WALL_FACING) {          \
                plane.px = plane_x;                                            \
                plane.py = point.y;                                            \
                plane.pz = plane_z;                                            \
                plane.vx = wall->normal.x;                                     \
                plane.vy = wall->normal.y;                                     \
                plane.vz = wall->normal.z;                                     \
                overlap = radius - njDistanceP2PL(&point, &plane, &nearest);   \
                if (overlap > 0.0F) {                                          \
                    push.x = wall->normal.x;                                   \
                    push.y = 0.0F;                                             \
                    push.z = wall->normal.z;                                   \
                    push.x *= overlap * WALL_UNDONE;                           \
                    push.y *= overlap * WALL_UNDONE;                           \
                    push.z *= overlap * WALL_UNDONE;                           \
                    njAddVector(&work->pos, &push);                            \
                    enemy->flags |= ENEMY_AT_WALL;                             \
                    if (njInnerProduct(&enemy->spd, &wall->normal) < 0.0F) {   \
                        reflect = wall->normal;                                \
                        if (njUnitVector(&reflect) == 0.0F) {                  \
                            reflect.x = 1.0F;                                  \
                            reflect.y = 0.0F;                                  \
                            reflect.z = 0.0F;                                  \
                        }                                                      \
                        scale = -(njInnerProduct(&enemy->spd, &reflect) *      \
                                  enemy->wall_bounce * 2.0F);                  \
                        reflect.x *= scale;                                    \
                        reflect.y *= scale;                                    \
                        reflect.z *= scale;                                    \
                        njAddVector(&enemy->spd, &reflect);                    \
                        njOuterProduct(&reflect, &enemy->spd, &axis);          \
                        if (axis.y > 0.0F) {                                   \
                            work->ang.y -= enemy->wall_turn;                   \
                        } else {                                               \
                            work->ang.y += enemy->wall_turn;                   \
                        }                                                      \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while (0)

static void enemy_meet_walls(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 point;
    NJS_PLANE plane;
    struct surface_probe *probe = &enemy->probe;
    struct surface_side *wall;
    Float radius = enemy->radius;
    Float overlap;
    Float scale;

    point = probe->pos;
    enemy->flags &= ~ENEMY_AT_WALL;
    /* @hack spelling: the meeting expanded for each of the four sides, where a
     * function called for each would do */
    MEET_WALL(SURFACE_NEG_X, -1.0F, 0.0F, wall->pos, point.z);
    MEET_WALL(SURFACE_POS_X, 1.0F, 0.0F, wall->pos, point.z);
    MEET_WALL(SURFACE_NEG_Z, 0.0F, -1.0F, point.x, wall->pos);
    MEET_WALL(SURFACE_POS_Z, 0.0F, 1.0F, point.x, wall->pos);
}

void enemy_draw_shadow(struct task_work *work, struct enemy_work *enemy)
{
    Float sx = enemy->shadow_size;
    /* @hack spelling: shadow_size read again for sz, where sx would do */
    Float sz = enemy->shadow_size * enemy->shadow_ratio;

    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    njTranslate(NULL, work->pos.x, work->pos.y + SHADOW_LIFT, work->pos.z);
    njRotateY(NULL, work->ang.y);
    njScale(NULL, sx, 1.0F, sz);
    njCnkModDrawObject(&shadow_object);
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

void enemy_meet_stage(struct task_work *work, struct enemy_work *enemy)
{
    enemy_probe_stage(work, enemy);
    enemy_meet_ceiling(work, enemy);
    enemy_meet_floor(work, enemy);
    enemy_meet_walls(work, enemy);
}

void enemy_set_home(struct task_work *work, struct enemy_work *enemy)
{
    enemy->home = work->pos;
}

void enemy_save_pos(struct task_work *work, struct enemy_work *enemy)
{
    enemy->last_pos = work->pos;
    if (enemy->group != ENEMY_NO_GROUP) {
        hunt_find_piece(EMERALD_KIND_INENEMY, enemy->group, &work->pos);
    }
}

void enemy_restore_pos(struct task_work *work, struct enemy_work *enemy)
{
    work->pos = enemy->last_pos;
}

Float enemy_target_distance(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared =
        ((enemy->target.x - work->pos.x) * (enemy->target.x - work->pos.x)) +
        ((enemy->target.y - work->pos.y) * (enemy->target.y - work->pos.y)) +
        ((enemy->target.z - work->pos.z) * (enemy->target.z - work->pos.z));

    return njSqrt(squared);
}

Float enemy_target_distance2(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: squared, a local for the sum, where returning the
     * sum would do */
    Float squared =
        ((enemy->target.x - work->pos.x) * (enemy->target.x - work->pos.x)) +
        ((enemy->target.y - work->pos.y) * (enemy->target.y - work->pos.y)) +
        ((enemy->target.z - work->pos.z) * (enemy->target.z - work->pos.z));

    return squared;
}

static Angle enemy_aim_at_target(struct task_work *work,
                                 struct enemy_work *enemy)
{
    /* @hack spelling: heading, a local for ang.y read before the
     * arctangent, where ang.y in place would do */
    Angle heading = work->ang.y;
    /* @hack spelling: off, a local for the result, where returning
     * angle_distance's would do */
    Angle off;

    enemy->aim_y = (Angle)(atan2f(enemy->target.x - work->pos.x,
                                  enemy->target.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
    off = angle_distance(enemy->aim_y, heading);
    return off;
}

Angle enemy_turn_to_target(struct task_work *work, struct enemy_work *enemy)
{
    Angle off = enemy_aim_at_target(work, enemy);

    work->ang.y = angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    return off;
}

Float enemy_home_distance(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared =
        ((enemy->home.x - work->pos.x) * (enemy->home.x - work->pos.x)) +
        ((enemy->home.y - work->pos.y) * (enemy->home.y - work->pos.y)) +
        ((enemy->home.z - work->pos.z) * (enemy->home.z - work->pos.z));

    return njSqrt(squared);
}

Float enemy_home_distance_flat(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: squared, a local for the sum, where passing the sum
     * to njSqrt would do */
    Float squared =
        ((enemy->home.x - work->pos.x) * (enemy->home.x - work->pos.x)) +
        ((enemy->home.z - work->pos.z) * (enemy->home.z - work->pos.z));

    return njSqrt(squared);
}

Float enemy_home_distance2(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: squared, a local for the sum, where returning the
     * sum would do */
    Float squared =
        ((enemy->home.x - work->pos.x) * (enemy->home.x - work->pos.x)) +
        ((enemy->home.y - work->pos.y) * (enemy->home.y - work->pos.y)) +
        ((enemy->home.z - work->pos.z) * (enemy->home.z - work->pos.z));

    return squared;
}

Float enemy_home_distance2_flat(struct task_work *work,
                                struct enemy_work *enemy)
{
    return ((enemy->home.x - work->pos.x) * (enemy->home.x - work->pos.x)) +
           ((enemy->home.z - work->pos.z) * (enemy->home.z - work->pos.z));
}

static Angle enemy_aim_at_home(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: heading, a local for ang.y read before the
     * arctangent, where ang.y in place would do */
    Angle heading = work->ang.y;
    /* @hack spelling: off, a local for the result, where returning
     * angle_distance's would do */
    Angle off;

    enemy->aim_y = (Angle)(atan2f(enemy->home.x - work->pos.x,
                                  enemy->home.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
    off = angle_distance(enemy->aim_y, heading);
    return off;
}

Angle enemy_turn_to_home(struct task_work *work, struct enemy_work *enemy)
{
    Angle off = enemy_aim_at_home(work, enemy);

    work->ang.y = angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    return off;
}

void enemy_keep_near_home(struct task_work *work, struct enemy_work *enemy,
                          Float range)
{
    NJS_VECTOR away;
    NJS_POINT3 center;
    /* @hack spelling: distance, a local for njDistanceP2P's result, where the
     * call compared in place would do */
    Float distance;

    distance = njDistanceP2P(&enemy->home, &work->pos);
    if (distance >= range) {
        away = work->pos;
        njSubVector(&away, &enemy->home);
        njUnitVector(&away);
        away.x *= range;
        away.y = 0.0F;
        away.z *= range;
        center = enemy->home;
        center.y = 0.0F;
        njAddVector(&away, &center);
        work->pos.x = away.x;
        work->pos.z = away.z;
    }
}

void enemy_keep_near_home_flat(struct task_work *work, struct enemy_work *enemy,
                               Float range)
{
    NJS_VECTOR away;
    NJS_POINT3 center;
    /* @hack spelling: distance, a local for njScalor's result, where the call
     * compared in place would do */
    Float distance;

    away = work->pos;
    njSubVector(&away, &enemy->home);
    away.y = 0.0F;
    distance = njScalor(&away);
    if (distance >= range) {
        njUnitVector(&away);
        away.x *= range;
        away.y = 0.0F;
        away.z *= range;
        center = enemy->home;
        center.y = 0.0F;
        njAddVector(&away, &center);
        work->pos.x = away.x;
        work->pos.z = away.z;
    }
}

Float enemy_player_distance2(struct task_work *work, int player)
{
    struct task_work *player_task_work = player_work[player];
    /* @bug memory: reads player's work through null where player's slot is
     * empty. enemy_bounce_player tests its player's work for null at
     * 0x8c022e7c. enemy_player_distance2 reads through the null at
     * 0x8c0224ca, enemy_aim_at_player at 0x8c022510 and
     * enemy_set_target_near_player at 0x8c022694 */
    /* @hack spelling: squared, a local for the sum, where returning the
     * sum would do */
    Float squared = ((player_task_work->pos.x - work->pos.x) *
                     (player_task_work->pos.x - work->pos.x)) +
                    ((player_task_work->pos.y - work->pos.y) *
                     (player_task_work->pos.y - work->pos.y)) +
                    ((player_task_work->pos.z - work->pos.z) *
                     (player_task_work->pos.z - work->pos.z));

    return squared;
}

Angle enemy_aim_at_player(struct task_work *work, struct enemy_work *enemy,
                          int player)
{
    struct task_work *player_task_work = player_work[player];
    /* @hack spelling: heading, a local for ang.y read before the
     * arctangent, where ang.y in place would do */
    Angle heading = work->ang.y;
    /* @hack spelling: off, a local for the result, where returning
     * angle_distance's would do */
    Angle off;

    enemy->aim_y = (Angle)(atan2f(player_task_work->pos.x - work->pos.x,
                                  player_task_work->pos.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
    off = angle_distance(enemy->aim_y, heading);
    return off;
}

Angle enemy_turn_to_player(struct task_work *work, struct enemy_work *enemy,
                           int player)
{
    Angle off = enemy_aim_at_player(work, enemy, player);

    work->ang.y = angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    return off;
}

Angle enemy_turn_from_player(struct task_work *work, struct enemy_work *enemy,
                             int player)
{
    Angle off = enemy_aim_at_player(work, enemy, player);

    enemy->aim_y += HALF_TURN;
    work->ang.y = angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
    return off;
}

int point_in_upper_screen(NJS_POINT3 *pos)
{
    NJS_POINT2 screen;

    njProjectScreen(NULL, pos, &screen);
#ifdef NONMATCHING
    /* this side takes the port's window, which 16:9 widens outside a demo
     * (engine/dc/port.h) */
    screen.x = port_view_x(screen.x);
#endif
    if (screen.x >= 0.0F && screen.x <= SCREEN_WIDTH && screen.y >= 0.0F &&
        screen.y <= UPPER_SCREEN_HEIGHT) {
        return 1;
    }
    return 0;
}

void enemy_set_target_near_player(struct task_work *work,
                                  struct enemy_work *enemy, int player,
                                  int index)
{
    struct task_work *player_task_work = player_work[player];
    NJS_VECTOR offset;

    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, camera_task_work->ang.y);
    njCalcPoint(NULL, &near_player_offsets[index], &offset);
    njPopMatrixEx();
    enemy->target.x = player_task_work->pos.x + offset.x;
    enemy->target.y = player_task_work->pos.y + offset.y;
    enemy->target.z = player_task_work->pos.z + offset.z;
}

static struct task *piece_create(void (*step)(struct task *task))
{
    struct task *task;
    struct piece *piece;

    task = task_create(0, PIECE_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if (!(piece = syCalloc(1, sizeof(struct piece)))) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = piece;
    }
    return task;
}

static void piece_move(struct piece *piece)
{
    struct angles ang;
    Float floor;
    Float gap;

    piece->spd.y -= PIECE_GRAVITY;
    if (piece->count++ > PIECE_FLOOR_COUNT) {
        piece->count = 0;
        floor = surface_probe_floor(
            piece->pos.x, piece->pos.y + PIECE_FLOOR_PROBE, piece->pos.z, &ang);
        gap = _builtin_fabsf(floor - piece->pos.y);
        if (piece->spd.y < 0.0F && gap < PIECE_BOUNCE_GAP) {
            piece->spd.y = -(piece->spd.y * PIECE_BOUNCE);
        }
    }
    piece->pos.x += piece->spd.x;
    piece->pos.y += piece->spd.y;
    piece->pos.z += piece->spd.z;
    piece->ang.x += piece->spin.x;
    piece->ang.y += piece->spin.y;
    piece->ang.z += piece->spin.z;
}

static void scrap_move(struct piece *scrap)
{
    scrap->spd.y -= SCRAP_GRAVITY;
    scrap->pos.x += scrap->spd.x;
    scrap->pos.y += scrap->spd.y;
    scrap->pos.z += scrap->spd.z;
    scrap->ang.x += scrap->spin.x;
    scrap->ang.y += scrap->spin.y;
    scrap->ang.z += scrap->spin.z;
}

static void piece_draw(struct task *task)
{
    struct piece *piece = task->extra.p;
    NJS_CNK_MODEL *model = piece->model;
    NJS_POINT3 *center = &model->center;

    if (piece->mode == PIECE_FADE || piece->mode == PIECE_FADE_UNHELD) {
        draw_attr_save();
        draw_attr_set(0, NJD_FST_UA);
        constant_material_set(piece->timer * (1.0F / PIECE_ALPHA), 0.0F, 0.0F,
                              0.0F);
    }
    njSetTexture(piece->texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &piece->pos);
    njRotateZ(NULL, piece->ang.z);
    njRotateX(NULL, piece->ang.x);
    njRotateY(NULL, piece->ang.y);
    njTranslate(NULL, -center->x, -center->y, -center->z);
    njCnkSimpleDrawModel(model);
    njPopMatrix(1);
    if (piece->mode == PIECE_FADE || piece->mode == PIECE_FADE_UNHELD) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
    }
}

static void scrap_draw(struct task *task)
{
    struct piece *scrap = task->extra.p;

    njPushMatrixEx();
    njTranslateEx(&scrap->pos);
    njRotateZ(NULL, scrap->ang.z);
    njRotateX(NULL, scrap->ang.x);
    njRotateY(NULL, scrap->ang.y);
    njCnkSimpleDrawModel(scrap->model);
    njPopMatrixEx();
}

static void piece_step(struct task *task)
{
    struct piece *piece = task->extra.p;

    switch (piece->mode) {
    case PIECE_FLY:
        if (game_state.time_stopped) {
            break;
        }
        /* fall through */
    case PIECE_FLY_UNHELD:
        if (piece->timer++ >= PIECE_STEPS) {
            piece->timer = PIECE_ALPHA;
            piece->mode = PIECE_FADE;
        }
        piece_move(piece);
        if (!(piece->timer & PIECE_BURST_MASK) &&
            rand() * RAND_SCALE > PIECE_BURST_THRESHOLD) {
            burst_create_wobbling(&piece->pos, &piece->spd, PIECE_BURST_SCALE);
        }
        break;
    case PIECE_FADE:
        if (game_state.time_stopped) {
            break;
        }
        /* fall through */
    case PIECE_FADE_UNHELD:
        if (piece->timer < PIECE_FADE_STEP) {
            piece->timer = PIECE_ALPHA;
            piece->model = NULL;
            piece->texlist = NULL;
            task_destroy(task);
        } else {
            /* @hack spelling: the subtraction assigned whole, where -= would
             * do */
            piece->timer = piece->timer - PIECE_FADE_STEP;
            piece_move(piece);
        }
        break;
    /* @hack redundant: the default's return */
    default:
        return;
    }
}

static void scrap_step(struct task *task)
{
    struct piece *scrap = task->extra.p;

    switch (scrap->mode) {
    case PIECE_FLY:
        if (game_state.time_stopped) {
            break;
        }
        /* fall through */
    case PIECE_FLY_UNHELD:
        if (scrap->timer++ >= PIECE_STEPS) {
            scrap->model = NULL;
            scrap->texlist = NULL;
            task_destroy(task);
        } else {
            scrap_move(scrap);
        }
        break;
    }
}

void enemy_break(struct task_work *work, NJS_CNK_MODEL **models,
                 NJS_TEXLIST *texlist)
{
    struct task *task;
    struct piece *piece;
    int i;

    for (i = 0; models[i]; i++) {
        task = piece_create(piece_step);
        if (task != NULL) {
            piece = task->extra.p;
            if (game_state.time_stopped) {
                piece->mode = PIECE_FLY_UNHELD;
            } else {
                piece->mode = PIECE_FLY;
            }
            task->draw = piece_draw;
            piece->pos = work->pos;
            piece->spd.x = ((rand() * RAND_SCALE) - 0.5F) * PIECE_SPREAD;
            piece->spd.y =
                (rand() * (PIECE_RISE_SPAN * RAND_SCALE)) + PIECE_RISE;
            piece->spd.z = ((rand() * RAND_SCALE) - 0.5F) * PIECE_SPREAD;
            piece->ang = work->ang;
            piece->count = i % PIECE_COUNT_SPREAD;
            piece->timer = 0;
            piece->model = models[i];
            piece->spin.x =
                (Angle)((0.5F - (rand() * RAND_SCALE)) * PIECE_SPIN);
            piece->spin.y =
                (Angle)((0.5F - (rand() * RAND_SCALE)) * PIECE_SPIN);
            piece->spin.z =
                (Angle)((0.5F - (rand() * RAND_SCALE)) * PIECE_SPIN);
            piece->texlist = texlist;
        }
    }
    scraps_throw(BREAK_SCRAPS, &work->pos, &work->ang);
    /* @hack redundant: the return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void scraps_throw(int count, NJS_POINT3 *pos, struct angles *ang)
{
    struct task *task;
    struct piece *scrap;
    int i;

    for (i = 0; i < count; i++) {
        task = piece_create(scrap_step);
        if (task != NULL) {
            scrap = task->extra.p;
            if (game_state.time_stopped) {
                scrap->mode = PIECE_FLY_UNHELD;
            } else {
                scrap->mode = PIECE_FLY;
            }
            task->draw = scrap_draw;
            scrap->pos = *pos;
            scrap->spd.x = ((rand() * RAND_SCALE) - 0.5F) * SCRAP_SPREAD;
            scrap->spd.y =
                (rand() * (SCRAP_RISE_SPAN * RAND_SCALE)) + SCRAP_RISE;
            scrap->spd.z = ((rand() * RAND_SCALE) - 0.5F) * SCRAP_SPREAD;
            scrap->ang = *ang;
            scrap->count = i % PIECE_COUNT_SPREAD;
            scrap->timer = 0;
            scrap->model =
                scrap_models[(int)(rand() * (SCRAP_MODELS * RAND_SCALE))];
            scrap->texlist = NULL;
            scrap->spin.x =
                (Angle)(((rand() * RAND_SCALE) + 1.0F) * SCRAP_SPIN);
            scrap->spin.y =
                (Angle)(((rand() * RAND_SCALE) + 1.0F) * SCRAP_SPIN);
            scrap->spin.z =
                (Angle)(((rand() * RAND_SCALE) + 1.0F) * SCRAP_SPIN);
        }
    }
}

void enemy_squash_step(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: stretch, a local for scl.y less 1 read before squash_spd,
     * where the difference in place would do */
    Float stretch = work->scl.y - 1.0F;

    /* @hack spelling: the change added as a negated difference, where
     * subtracting the sum would do */
    enemy->squash_spd +=
        -(enemy->squash_spd * SQUASH_DAMPING) - (stretch * SQUASH_PULL);
    work->scl.y += enemy->squash_spd;
    if (work->scl.y > ENEMY_SQUASH_MAX) {
        work->scl.y = ENEMY_SQUASH_MAX;
    } else if (work->scl.y < ENEMY_SQUASH_MIN) {
        work->scl.y = ENEMY_SQUASH_MIN;
    }
    work->scl.x = 2.0F - work->scl.y;
}

void enemy_bounce_player(int player)
{
    struct task_work *player_task_work = player_work[player];
    int character;
    /* @hack spelling: x, y and z, locals for the bounce's speed, where the
     * products passed in place would do */
    Float x;
    Float y;
    Float z;

    if (player_task_work != NULL) {
        if ((player_task_work->flags & PLAYER_WORK_ATTACK) &&
            !(player_task_work->flags & BOUNCE_BLOCKING_BITS)) {
            character = player_records[player]->character;
            if (character == PLAYER_SONIC || character == PLAYER_SHADOW ||
                ((character == PLAYER_KNUCKLES || character == PLAYER_ROUGE) &&
                 (player_task_work->mode == PLAYER_MODE_JUMP ||
                  player_task_work->mode == PLAYER_MODE_HUNTER_RUN_ATTACK_3))) {
                x = -(world_gravity.x * BOUNCE_SPEED);
                y = -(world_gravity.y * BOUNCE_SPEED);
                z = -(world_gravity.z * BOUNCE_SPEED);
                player_set_speed(player, x, y, z);
                vibration_start(player, BOUNCE_VIBRATION_POWER,
                                BOUNCE_VIBRATION_FREQ, VIBRATION_UNTIMED);
            }
        }
    }
}

int enemy_take_hit(struct task_work *work, struct enemy_work *enemy)
{
    struct task *own;
    struct task *task;
    int player;

    if (work->flags & WORK_HURT) {
        /* @hack redundant: the test of work, which the read of its flags has
         * already used */
        if (work && work->collision) {
            own = work->collision->owner;
        } else {
            own = NULL;
        }
        if (own) {
            collision_restart_scans();
            /* @hack spelling: the two scans in an if chain with empty
             * bodies, where the second under a test of the first would do */
            if ((task = collision_next_player(own)) != NULL) {
            } else if ((task = collision_next_attack(own)) != NULL) {
            }
        } else {
            task = NULL;
        }
        if (task) {
            enemy->flags |= ENEMY_HIT;
            player = player_slot_of(task);
            if (player != -1) {
                enemy_bounce_player(player);
            }
        }
        enemy->squash_spd = HIT_SQUASH_SPD;
        return 1;
    }
    if (enemy_hit_hook && enemy_hit_hook(&work->pos)) {
        return 1;
    }
    return 0;
}

/* a dmyEnemy task only waits. each time its timer runs out, the timer starts
 * again, and the task takes 1 from submode. it kills itself where submode was
 * already 0 */
static void dummy_enemy_step(struct task *task)
{
    struct task_work *work = task->work;

    if (work->timer-- > 0) {
        return;
    }
    /* @hack spelling: the timer written through task->work, where work
     * would do */
    task->work->timer = (rand() * (DUMMY_WAIT_SPAN * RAND_SCALE)) + DUMMY_WAIT;
    if (work->submode-- > 0) {
        return;
    }
    task_kill(task);
}

#ifdef NONMATCHING
/* set pos to the center of work's first shape, or to work's pos where it has
 * no record */
static void defeat_center(struct task_work *work, NJS_POINT3 *pos)
{
    if (work->collision == NULL) {
        *pos = work->pos;
        return;
    }
    collision_get_shape_center(work, work->collision->shapes, pos);
}
#endif

void enemy_defeat(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    struct task *task;
    int player;

    collision_restart_scans();
    /* @bug memory: reads the collision record through null where an enemy
     * without one is beaten. enemy_hit_hook can hurt such an enemy, since it
     * takes only the enemy's pos. enemy_take_hit tests the record for null at
     * 0x8c022f16. enemy_defeat reads through the null at 0x8c023070 */
#ifdef NONMATCHING
    /* this side repairs the tag above: where the enemy has no record, its
     * defeat credits no player. defeat_center then gives the enemy's pos for
     * its shape's center */
    task = work->collision != NULL
               ? collision_next_player(work->collision->owner)
               : NULL;
    if (task != NULL) {
#else
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = collision_next_player(work->collision->owner)) != NULL) {
#endif
        player = player_slot_of(task);
    } else {
        player = -1;
    }
    collision_restart_scans();
    if (enemy->group != ENEMY_NO_GROUP) {
#ifdef NONMATCHING
        /* this side reads no shape of a missing record, as above */
        defeat_center(work, &pos);
#else
        collision_get_shape_center(work, work->collision->shapes, &pos);
#endif
        hunt_take_piece(EMERALD_KIND_INENEMY, enemy->group, player, &pos);
    }
    lockon_count_blast(work, enemy);
    if (enemy->flags & ENEMY_COUNTED) {
        enemy_group_counts[(Uint8)enemy->group]--;
        enemy->flags &= ~ENEMY_COUNTED;
    }
    work->flags |= WORLD_ADDED;
    if (enemy->flags &
        (ENEMY_BURST | ENEMY_RELEASES_ANIMAL | ENEMY_LEAVES_CHAOS_DRIVE)) {
#ifdef NONMATCHING
        /* this side reads no shape of a missing record, as above */
        defeat_center(work, &pos);
#else
        collision_get_shape_center(work, work->collision->shapes, &pos);
#endif
        if (enemy->flags & ENEMY_BURST) {
            burst_create_wobbling(&pos, &enemy->spd, DEFEAT_BURST_SCALE);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((task = task_create(TASK_WORK, DUMMY_LEVEL, dummy_enemy_step,
                                    "dmyEnemy")) != NULL) {
                sound_play_at(BURST_SOUND, NULL, 0, BURST_VOLUME, &pos);
                task->work->timer = (rand() * (DUMMY_WAIT_SPAN * RAND_SCALE)) +
                                    DUMMY_FIRST_WAIT;
                task->work->submode =
                    (rand() * (DUMMY_WAITS_SPAN * RAND_SCALE)) + DUMMY_WAITS;
            }
        }
        if (!game_state.two_players) {
            if (enemy->flags & ENEMY_LEAVES_CHAOS_DRIVE) {
                chaos_drive_create(&pos);
            }
            if (enemy->flags & ENEMY_RELEASES_ANIMAL) {
                animal_bubble_release(pos.x, pos.y, pos.z);
            }
        }
    }
}

static void enemy_motion_follow(struct enemy_motion *motion, Sint8 ended);
static void enemy_motion_enter(struct enemy_motion *motion);
static void enemy_motion_link(struct enemy_motion *motion);
static void enemy_motion_unlink(struct enemy_motion *motion);

void enemy_motion_step(struct enemy_motion *motion)
{
    Sint8 next = motion->next;
    NJS_MOTION *played;
    Uint32 frames;
    Uint32 next_frames;

    motion->last = motion->action;
    motion->flags = 0;
    if (motion->action != next && motion->mode != ENEMY_MOTION_BLENDING &&
        motion->mode != ENEMY_MOTION_START &&
        motion->mode != ENEMY_MOTION_FROZEN) {
        motion->mode = ENEMY_MOTION_CHANGE;
        motion->steps = 0;
        if (motion->table[motion->next].mode == ENEMY_MOTION_LOOP_SCALED &&
            motion->table[motion->action].mode == ENEMY_MOTION_LOOP_SCALED) {
            frames = motion->table[motion->action].motion->nbFrame;
            next_frames = motion->table[motion->next].motion->nbFrame;
            motion->next_frame =
                ((Float)next_frames * ((Float)frames - motion->frame) /
                 (Float)frames) +
                ((Float)next_frames * ENEMY_LOOP_LEAD);
            if (motion->next_frame >= (Float)next_frames) {
                motion->next_frame -= (Float)next_frames;
            }
        }
    } else {
        motion->steps++;
    }
    switch (motion->mode) {
    case ENEMY_MOTION_START:
        /* @hack redundant: the first store of frame, which the last
         * repeats */
        motion->frame = 0.0F;
        motion->action = next;
        motion->mode = ENEMY_MOTION_CHANGE;
        motion->next_frame = 0.0F;
        motion->frame = 0.0F;
        break;
    case ENEMY_MOTION_CHANGE:
        if (motion->table[next].blend != 1.0F) {
            enemy_motion_link(motion);
            motion->frame = 0.0F;
        } else {
            motion->frame = 1.0F;
        }
        motion->action = next;
        motion->mode = ENEMY_MOTION_BLENDING;
        /* fall through */
    case ENEMY_MOTION_BLENDING:
        motion->frame += motion->table[motion->action].blend;
        if (motion->frame >= 1.0F) {
            motion->mode = motion->table[motion->action].mode;
            enemy_motion_unlink(motion);
            enemy_motion_enter(motion);
        }
        break;
    case ENEMY_MOTION_LOOP:
        played = motion->table[next].motion;
        motion->frame += motion->table[next].speed;
        /* @hack spelling: the last frame on the left of the test, where frame +
         * speed >= nbFrame - 1 would do */
        if ((Float)played->nbFrame - 1.0F <=
            motion->frame + motion->table[next].speed) {
            motion->flags |= ENEMY_MOTION_ENDING;
        }
        while (motion->frame >= (Float)played->nbFrame) {
            motion->frame -= (Float)played->nbFrame;
        }
        /* @hack spelling: 0 on the left, where frame < 0 would do */
        while (0.0F > motion->frame) {
            motion->frame += (Float)played->nbFrame;
        }
        break;
    case ENEMY_MOTION_LOOP_SCALED:
        played = motion->table[next].motion;
        motion->frame += motion->speed * motion->table[next].speed;
        while (motion->frame >= (Float)played->nbFrame) {
            motion->frame -= (Float)played->nbFrame;
        }
        /* @hack spelling: 0 on the left, where frame < 0 would do */
        while (0.0F > motion->frame) {
            motion->frame += (Float)played->nbFrame;
        }
        break;
    case ENEMY_MOTION_ONCE_SCALED:
        played = motion->table[next].motion;
        motion->frame += motion->speed;
        if (motion->frame >= (Float)played->nbFrame - 1.0F) {
            enemy_motion_follow(motion, next);
        }
        break;
    case ENEMY_MOTION_ONCE:
        played = motion->table[next].motion;
        motion->frame += motion->table[next].speed;
        /* @hack spelling: the else's test with the last frame on the left,
         * where frame + speed >= nbFrame - 1 would do */
        if (motion->frame >= (Float)played->nbFrame - 1.0F) {
            enemy_motion_follow(motion, next);
        } else if ((Float)played->nbFrame - 1.0F <=
                   motion->frame + motion->table[next].speed) {
            motion->flags |= ENEMY_MOTION_ENDING;
        }
        break;
    case ENEMY_MOTION_BACK:
        /* @hack redundant: played loaded, which the mode doesn't read */
        played = motion->table[next].motion;
        motion->frame -= motion->table[next].speed;
        /* @hack spelling: 0 on the left, where frame < 0 would do */
        if (0.0F > motion->frame) {
            enemy_motion_follow(motion, next);
        }
        break;
    case ENEMY_MOTION_ONCE_HOLD:
        played = motion->table[next].motion;
        motion->frame += motion->table[next].speed;
        /* @hack spelling: the else's test with the last frame on the left,
         * where frame + speed >= nbFrame - 1 would do */
        if (motion->frame >= (Float)played->nbFrame - 1.0F) {
            motion->frame = (Float)played->nbFrame - 1.0F;
            motion->flags |= ENEMY_MOTION_ENDED;
        } else if ((Float)played->nbFrame - 1.0F <=
                   motion->frame + motion->table[next].speed) {
            motion->flags |= ENEMY_MOTION_ENDING;
        }
        break;
    case ENEMY_MOTION_BACK_HOLD:
        /* @hack redundant: played loaded, which the mode doesn't read */
        played = motion->table[next].motion;
        motion->frame -= motion->table[next].speed;
        /* @hack spelling: 0 on the left, where frame < 0 would do */
        if (0.0F > motion->frame) {
            motion->frame = 0.0F;
            motion->flags |= ENEMY_MOTION_ENDED;
        }
        break;
    case ENEMY_MOTION_STILL:
        played = motion->table[next].motion;
        while (motion->frame >= (Float)played->nbFrame) {
            motion->frame -= (Float)played->nbFrame;
        }
        /* @hack spelling: 0 on the left, where frame < 0 would do */
        while (0.0F > motion->frame) {
            motion->frame += (Float)played->nbFrame;
        }
        break;
    case ENEMY_MOTION_FROZEN:
    default:
        enemy_motion_unlink(motion);
        break;
    }
}

/* play the action that table[ended].next names, from where its mode starts */
static void enemy_motion_follow(struct enemy_motion *motion, Sint8 ended)
{
    motion->next = motion->table[ended].next;
    motion->action = motion->next;
    motion->mode = motion->table[motion->action].mode;
    motion->after_next = motion->table[motion->next].next;
    motion->flags |= ENEMY_MOTION_ENDED;
    enemy_motion_enter(motion);
}

/* set frame to where the action's mode starts it */
static void enemy_motion_enter(struct enemy_motion *motion)
{
    if (motion->mode == ENEMY_MOTION_BACK ||
        motion->mode == ENEMY_MOTION_BACK_HOLD) {
        motion->frame =
            (Float)motion->table[motion->action].motion->nbFrame - 1.0F;
    } else if (motion->mode == ENEMY_MOTION_LOOP ||
               motion->mode == ENEMY_MOTION_LOOP_SCALED ||
               motion->mode == ENEMY_MOTION_STILL) {
        motion->frame = motion->next_frame;
        motion->next_frame = 0.0F;
    } else {
        motion->frame = 0.0F;
    }
}

/* start a blend from the action that plays to next */
static void enemy_motion_link(struct enemy_motion *motion)
{
    motion->link.motion[0] = motion->table[motion->action].motion;
    motion->link.motion[1] = motion->table[motion->next].motion;
    motion->link.frame[0] = motion->frame;
    motion->link.frame[1] = motion->next_frame;
}

static void enemy_motion_unlink(struct enemy_motion *motion)
{
    motion->link.motion[0] = NULL;
    motion->link.motion[1] = NULL;
    motion->link.frame[0] = 0.0F;
    motion->link.frame[1] = 0.0F;
}

void enemy_set_offset(struct task_work *work, struct enemy_work *enemy)
{
    NJS_VECTOR place;
    /* @hack spelling: x, a local for the distance, where storing it to place.x
     * would do */
    Float x;

    x = _builtin_fabsf(njFraction(work->scl.z) * ENEMY_OFFSET_SCALE);
    place.x = x;
    place.y = 0.0F;
    place.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, &place, &enemy->offset);
    njPopMatrixEx();
    work->scl.z = njRoundOff(work->scl.z);
}

void enemy_set_offset_behind(struct task_work *work, struct enemy_work *enemy)
{
    NJS_VECTOR place;
    /* @hack spelling: x, a local for the distance, where storing it to place.x
     * would do */
    Float x;

    x = _builtin_fabsf(njFraction(work->scl.z) * ENEMY_OFFSET_SCALE);
    place.x = x;
    place.y = 0.0F;
    place.z = 0.0F;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, HALF_TURN);
    njCalcVector(NULL, &place, &enemy->offset);
    njPopMatrixEx();
    work->scl.z = njRoundOff(work->scl.z);
}

/* while a player holds the ball, the sphere tilts with that player's ang.x
 * and ang.z */
static void enemy_ball_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_ball *ball = task->motion.p;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    struct task_work *holder;
    int i;

    pos.x = work->pos.x;
    pos.y = work->pos.y + BALL_SPHERE_RADIUS;
    pos.z = work->pos.z;
    ang.x = 0;
    ang.y = 0;
    ang.z = 0;
    scl.x = BALL_SPHERE_RADIUS;
    scl.y = BALL_SPHERE_RADIUS;
    scl.z = BALL_SPHERE_RADIUS;
    holder = NULL;
    for (i = 0; i < BALL_HOLDER_SLOTS; i++) {
        struct player *record = player_records[i];

        if (record != NULL && record->held == task) {
            holder = player_work[i];
            break;
        }
    }
    if (holder != NULL) {
        ang.x = holder->ang.x;
        ang.z = holder->ang.z;
    }
    debug_draw_model(&pos, &ang, &scl, DEBUG_MODEL_SPHERE);
    njSetTexture(ball->texlist);
    njPushMatrixEx();
    njTranslateEx(&pos);
    njRotateY(NULL, work->ang.y);
    njScaleEx(&work->scl);
    njTranslateEx(&ball->offset);
    njCnkSimpleDrawObject(ball->object);
    njPopMatrixEx();
}

static void enemy_ball_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_ball *ball = task->motion.p;

    if (placement_try_leave(task)) {
        return;
    }
    if (work->flags & BALL_BURST_FLAGS) {
        task_kill(task);
        burst_create_wobbling(&work->pos, &ball->motion.spd, BALL_BURST_SCALE);
        return;
    }
    fall_step_small(task);
    work->scl.x = float_approach(work->scl.x, ball->scl.x, BALL_GROWTH);
    work->scl.y = float_approach(work->scl.y, ball->scl.y, BALL_GROWTH);
    work->scl.z = float_approach(work->scl.z, ball->scl.z, BALL_GROWTH);
    work->ang.y += BALL_TURN;
}

static void enemy_ball_destroy(struct task *task)
{
}

struct task *enemy_ball_create(int player, NJS_CNK_OBJECT *object,
                               NJS_TEXLIST *texlist, NJS_VECTOR *scl,
                               NJS_VECTOR *offset)
{
    struct task *task;
    struct enemy_ball *ball;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, BALL_LEVEL, enemy_ball_step,
                            "EnemyBallExec")) == NULL) {
        return NULL;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((ball = syCalloc(1, sizeof(struct enemy_ball))) == NULL) {
        task_destroy(task);
        return NULL;
    }
    task->motion.p = ball;
    collision_init(task, enemy_ball_shapes, BALL_SHAPES, COLLISION_OBJECT);
    ball->scl = *scl;
    if (offset) {
        ball->offset = *offset;
    }
    ball->object = object;
    ball->texlist = texlist;
    task->work->pos = player_records[player]->unk_1e8;
    player_hand_task(player, task);
    /* @hack spelling: task->motion passed, where &ball->motion would do */
    fall_set_kind(task->work, task->motion.p, FALL_KIND_NORMAL);
    /* @hack spelling: destructor set ahead of draw, where draw first, in
     * struct task's order, would do */
    task->destructor = enemy_ball_destroy;
    task->draw = enemy_ball_draw;
    return task;
}

void debug_draw_model_unlit(NJS_POINT3 *pos, struct angles *ang,
                            NJS_VECTOR *scl, int model)
{
    Uint32 flags;
    Uint32 and_attr;
    Uint32 or_attr;

    njPushMatrixEx();
    njTranslateEx(pos);
    if (ang) {
        njRotateZ(NULL, ang->z);
        njRotateX(NULL, ang->x);
        njRotateY(NULL, ang->y);
    }
    if (scl) {
        njScaleEx(scl);
    }
    and_attr = _nj_constant_attr_and_;
    or_attr = _nj_constant_attr_or_;
    flags = _nj_control_3d_flag_;
    draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL);
    draw_control_set(DRAW_CONTROL_CONSTANT);
    draw_attr_set(0, NJD_FST_IL | NJD_FST_IS | NJD_FST_IA | NJD_FST_UA);
    constant_material_set(UNLIT_ALPHA, UNLIT_RED, UNLIT_GREEN, UNLIT_BLUE);
    njCnkSimpleDrawObject(debug_models[model]);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    _nj_control_3d_flag_ = flags;
    _nj_constant_attr_and_ = and_attr;
    _nj_constant_attr_or_ = or_attr;
    njPopMatrixEx();
}
