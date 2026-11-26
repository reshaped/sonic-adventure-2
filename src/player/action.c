/* the steps the characters share, and the small tasks a player starts */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/motion.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "effect/powder.h"
#include "effect/stars.h"
#include "enemy/enemy.h"
#include "object/fall.h"
#include "player/action.h"
#include "player/knuckles.h"
#include "player/mystic_melody.h"
#include "player/player.h"
#include "player/pose_camera.h"
#include "player/walker_weapons.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a ledge whose surface has any of these flags holds no player */
#define LEDGE_REFUSES                                                          \
    (WORLD_MOVING | WORLD_NO_CLIMB | WORLD_FLAT | GROUND_NO_LANDING |          \
     GROUND_DEFLECTS | GROUND_NO_LEDGE)
/* player_ground_step lands no player on ground with any of these flags */
#define SNAP_REFUSES (WORLD_FLAT | WORLD_UPRIGHT | GROUND_CLING)
/* gravity points straight down where world_gravity.y is below
 * -GRAVITY_STRAIGHT, and straight up where it is above GRAVITY_STRAIGHT */
#define GRAVITY_STRAIGHT 0.99999F
/* the cosine of 10 degrees. a ledge, or ground a player runs off, that tilts
 * less is level */
#define NEAR_LEVEL 0.9848F
/* how far a pettable task's pos may lie from the player's along y */
#define PET_HEIGHT 1.499F

struct player_hooks player_hooks = {0};

/* how far a look may turn a player's head and eyes from their nodes' own
 * angles. both eyes take eyes_max_z and eyes_min_z, so eye0_max_z and
 * eye0_min_z go unread */
struct look_limits {
    Angle head_max_x, head_min_x;
    Angle head_max_z, head_min_z;
    Angle eye0_max_y, eye0_min_y;
    Angle eye0_max_z, eye0_min_z;
    Angle eye1_max_y, eye1_min_y;
    Angle eyes_max_z, eyes_min_z;
};

/* the limits, by the record's character */
static struct look_limits look_limits[11] = {
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0xc00, -0x200, 0x400, -0xd00, 0x200,
     -0xc00, 0x400, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
    {0x2000, -0x2000, 0x400, -0x1800, 0x1b00, -0x200, 0x800, -0xd00, 0x200,
     -0x1b00, 0x800, -0xd00},
};

/* the petting's shape, which the checks against players pass over */
static struct collision_shape chaonade_shape[1] = {
#include "player/action/chaonade_shape.inc"
};

/* a pose before a two-player stage starts, by the player's kind. a kind
 * without a motion doesn't pose */
struct start_pose {
    Sint32 anim;
    NJS_MOTION *motion; /* the camera's, about the player */
    Float speed; /* the camera motion's */
    /* the player's line in a match's first round. a later round's line is
     * won_voice where its character won the last round, and lost_voice
     * otherwise */
    Sint32 voice;
    Sint32 won_voice;
    Sint32 lost_voice;
};

static struct start_pose start_poses[17] = {
    {0x77, &sonic_versus_camera, 1.0F, 0x5f7, 0x5fd, 0x5ff},
    {0xa5, &shadow_versus_camera, 1.0F, 0x5f6, 0x5fc, 0x5fe},
    {0, NULL, 0.5F, 0, 0, 0},
    {0, NULL, 0.5F, 0, 0, 0},
    {0x88, &knuckles_versus_camera, 1.0F, 0x4e4, 0x4ea, 0x4ec},
    {0xb3, &rouge_versus_camera, 1.0F, 0x4e5, 0x4eb, 0x4ed},
    {0xbb, &tails_mech_versus_camera, 1.0F, 0x669, 0x66f, 0x671},
    {0x88, &eggman_mech_versus_camera, 1.0F, 0x668, 0x66e, 0x670},
    {0x77, &amy_versus_camera, 0.7F, 0xa67, 0xa6c, 0xa6d},
    {0, NULL, 0.5F, 0, 0, 0},
    {0, NULL, 0.5F, 0, 0, 0},
    {0, NULL, 0.5F, 0, 0, 0},
    {0xa5, &metal_sonic_versus_camera, 1.0F, 0, 0, 0},
    {0xbb, &chao_walker_versus_camera, 1.0F, 0, 0, 0},
    {0x88, &eggman_mech_versus_camera, 1.0F, 0, 0, 0},
    {0x88, &tikal_versus_camera, 1.0F, 0, 0, 0},
    {0xb3, &rouge_versus_camera, 1.0F, 0, 0, 0},
};

/* each character's animation while the two players wait for play to start,
 * by the record's character */
static Sint32 waiting_anims[9] = {
    0x78, 0xa6, 0x10, 0x10, 0x89, 0xb4, 0xbc, 0x89, 0x78,
};

/* the pairs of lines a long idle says, by kind and stage or by kind alone.
 * the table by stage ends at a stage of STAGE_NONE. the table by kind ends at
 * a kind of -1 */
struct stage_voice {
    Sint8 kind;
    Sint8 stage;
    Sint16 voices[2];
};
struct kind_voice {
    Sint16 kind;
    Sint16 voices[2];
};

/* clang-format off */
static struct stage_voice stage_idle_voices[40] = {
    {PLAYER_SONIC, STAGE_CITY_ESCAPE, {0x712, 0x713}},
    {PLAYER_SONIC, STAGE_METAL_HARBOR, {0x786, 0x787}},
    {PLAYER_SONIC, STAGE_GREEN_FOREST, {0x76c, 0x76d}},
    {PLAYER_SONIC, STAGE_PYRAMID_CAVE, {0x7cd, 0x7ce}},
    {PLAYER_SONIC, STAGE_CRAZY_GADGET, {0x6d6, 0x6d7}},
    {PLAYER_SONIC, STAGE_FINAL_RUSH, {0x772, 0x774}},
    {PLAYER_SONIC, STAGE_RELAY_SONIC, {0x6f9, 0x6fe}},
    {PLAYER_SHADOW, STAGE_RADICAL_HIGHWAY, {0x790, 0x791}},
    {PLAYER_SHADOW, STAGE_WHITE_JUNGLE, {0x794, 0x795}},
    {PLAYER_SHADOW, STAGE_SKY_RAIL, {0x7f9, 0x7fa}},
    {PLAYER_SHADOW, STAGE_FINAL_CHASE, {0x76a, 0x76b}},
    {PLAYER_KNUCKLES, STAGE_WILD_CANYON, {0x71e, 0x71f}},
    {PLAYER_KNUCKLES, STAGE_PUMPKIN_HILL, {0x7db, 0x7dc}},
    {PLAYER_KNUCKLES, STAGE_AQUATIC_MINE, {0x5c8, 0x5c9}},
    {PLAYER_KNUCKLES, STAGE_DEATH_CHAMBER, {0x733, 0x734}},
    {PLAYER_KNUCKLES, STAGE_METEOR_HERD, {0x7bd, 0x7be}},
    {PLAYER_KNUCKLES, STAGE_RELAY_KNUCKLES, {0x6f7, 0x6fc}},
    {PLAYER_ROUGE, STAGE_DRY_LAGOON, {0x72a, 0x72b}},
    {PLAYER_ROUGE, STAGE_EGG_QUARTERS, {0x73d, 0x73e}},
    {PLAYER_ROUGE, STAGE_SECURITY_HALL, {0x7f5, 0x7f6}},
    {PLAYER_ROUGE, STAGE_MAD_SPACE, {0x7af, 0x7b0}},
    {PLAYER_ROUGE, STAGE_RELAY_ROUGE, {0x6f8, 0x6fd}},
    {PLAYER_TAILS_MECH, STAGE_PRISON_LANE, {0x7e1, 0x7e2}},
    {PLAYER_TAILS_MECH, STAGE_MISSION_STREET, {0x7c5, 0x7c6}},
    {PLAYER_TAILS_MECH, STAGE_HIDDEN_BASE, {0x78b, 0x78c}},
    {PLAYER_TAILS_MECH, STAGE_ETERNAL_ENGINE, {0x764, 0x765}},
    {PLAYER_TAILS_MECH, STAGE_RELAY_TAILS, {0x6fa, 0x6ff}},
    {PLAYER_EGGMAN_MECH, STAGE_IRON_GATE, {0x77e, 0x77f}},
    {PLAYER_EGGMAN_MECH, STAGE_SAND_OCEAN, {0x7fd, 0x7fe}},
    {PLAYER_EGGMAN_MECH, STAGE_LOST_COLONY, {0x79f, 0x7a0}},
    {PLAYER_EGGMAN_MECH, STAGE_WEAPONS_BED, {0x811, 0x812}},
    {PLAYER_EGGMAN_MECH, STAGE_COSMIC_WALL, {0x80e, 0x80f}},
    {PLAYER_EGGMAN_MECH, STAGE_RELAY_EGGMAN, {0x6f6, 0x6fb}},
    {PLAYER_SONIC, STAGE_CHAO, {0x8ee, 0x8ee}},
    {PLAYER_SHADOW, STAGE_CHAO, {0x8ed, 0x8ed}},
    {PLAYER_KNUCKLES, STAGE_CHAO, {0x8eb, 0x8eb}},
    {PLAYER_ROUGE, STAGE_CHAO, {0x8ec, 0x8ec}},
    {PLAYER_EGGMAN, STAGE_CHAO, {0x8ea, 0x8ea}},
    {PLAYER_TAILS, STAGE_CHAO, {0x8ef, 0x8ef}},
    {-1, STAGE_NONE, {0, 0}},
};
static struct kind_voice kind_idle_voices[9] = {
    {PLAYER_SONIC, {0x5bc, 0x5b6}},
    {PLAYER_KNUCKLES, {0x5b3, 0x5b9}},
    {PLAYER_TAILS_MECH, {0x5b7, 0x5bd}},
    {PLAYER_TAILS, {0x5b7, 0x5bd}},
    {PLAYER_SHADOW, {0x5bb, 0x5b5}},
    {PLAYER_ROUGE, {0x5b4, 0x5ba}},
    {PLAYER_EGGMAN_MECH, {0x5b8, 0x5b2}},
    {PLAYER_EGGMAN, {0x5b2, 0x5b8}},
    {-1, {-1, 0}},
};
/* clang-format on */

/* the flute that flute_draw is drawing, and null outside the draw */
static struct flute *flute_drawing = NULL;
/* the inverse of the view as flute_draw begins. it takes a place in the view
 * back to the world */
static NJS_MATRIX view_to_world;

struct look;

static void look_turn_head(struct look *look);
static void look_turn_eyes(struct look *look);
static void look_destroy(struct task *task);
static void look_step(struct task *task);
static void afterimage_step(struct task *task);
static void afterimage_draw(struct task *task);
static void chaonade_start(int slot);
static void pose_step(struct task *task);
static void start_pose_show(struct task_work *work, struct player *record,
                            Bool frozen);
static void start_pose_step(struct task *task);
static void idle_voice(struct task_work *work, struct player *record, int idle);
static void note_draw(struct task *task);
static void flute_create(int slot);

/* a ledge's top must lie from LEDGE_LOW below the player's head up to
 * LEDGE_WINDOW above that. for Tails the lowest is TAILS_LEDGE_LOW below. the
 * ground must lie LEDGE_DROP or more below the player */
#define LEDGE_LOW 3.0F
#define TAILS_LEDGE_LOW 5.0F
#define LEDGE_WINDOW 4.0F
#define LEDGE_DROP 5.0F

/* TRUE where the ledge ahead is in reach, as the ledges' comment in
 * action.h says */
static Bool ledge_in_reach(struct task_work *work, struct player *record)
{
    struct ground_probe *ahead = &record->ahead;
    /* @hack spelling: ground, a local for &record->ground set before its one
     * use, where record->ground.below_y would do */
    struct ground_probe *ground;
    NJS_VECTOR up;
    Float top;
    Float ledge;

    if (!(world_gravity.y < -GRAVITY_STRAIGHT)) {
        return FALSE;
    }
    if (work->flags & PLAYER_WORK_BALL) {
        return FALSE;
    }
    if (ahead->below_flags & LEDGE_REFUSES) {
        return FALSE;
    }
    ledge = ahead->below_y;
    switch (record->character) {
    case PLAYER_EGGMAN:
        return FALSE;
    case PLAYER_TAILS:
        top = (work->pos.y + record->consts.height) - TAILS_LEDGE_LOW;
        break;
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
    default:
        top = (work->pos.y + record->consts.height) - LEDGE_LOW;
        break;
    }
    if (top > ledge) {
        return FALSE;
    }
    top += LEDGE_WINDOW;
    if (ledge > top) {
        return FALSE;
    }
    up.x = 0.0F;
    up.y = 1.0F;
    up.z = 0.0F;
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, ahead->below_ang_z);
    njRotateX(NULL, ahead->below_ang_x);
    njCalcVector(NULL, &up, &up);
    njPopMatrixEx();
    if (up.y < NEAR_LEVEL) {
        return FALSE;
    }
    ground = &record->ground;
    if (work->pos.y - ground->below_y < LEDGE_DROP) {
        return FALSE;
    }
    return TRUE;
}

/* the damage a hit does to a mech's gauge. a walker's laser does LASER_DAMAGE,
 * and a mech's strike STRIKE_DAMAGE. WORLD_HURTS ground and any other hit do
 * HIT_DAMAGE */
#define LASER_DAMAGE 4.9F
#define STRIKE_DAMAGE 1.0F
#define HIT_DAMAGE 3.0F
/* a hit of LIGHT_DAMAGE or less plays SOUND_LIGHT_HIT, and any other
 * SOUND_HEAVY_HIT */
#define LIGHT_DAMAGE 2.0F
#define SOUND_LIGHT_HIT 0x2014
#define SOUND_HEAVY_HIT 0x2015

/* take the hit's damage from the mech's gauge, with a sound by its size. the
 * mech misses where the gauge runs out */
static void gauge_take_hit(struct task_work *work, struct player *record)
{
    struct collision_work *hit;
    struct enemy_work *enemy;
    Float damage;

    /* @bug memory: reads a null record's slot, at 0x8c04cb20, where the hit is
     * no item's or enemy's. the test at 0x8c04ca7e lets the null through */
    if (record != NULL && (record->ground_flags & WORLD_HURTS)) {
        damage = HIT_DAMAGE;
    } else if ((hit = work->collision->hit) != NULL &&
               (hit->kind == COLLISION_ITEM || hit->kind == COLLISION_ENEMY) &&
               (enemy = hit->owner->motion.p) != NULL) {
        damage = enemy->damage;
    } else if (collision_first_with_id(player_tasks[record->slot],
                                       WALKER_LASER_SHAPE)) {
        damage = LASER_DAMAGE;
    } else if (collision_first_with_id(player_tasks[record->slot],
                                       MECH_STRIKE_SHAPE)) {
        damage = STRIKE_DAMAGE;
    } else {
        damage = HIT_DAMAGE;
    }
    if (damage <= LIGHT_DAMAGE) {
        sound_play(SOUND_LIGHT_HIT, NULL, 0, 0);
    } else {
        sound_play(SOUND_HEAVY_HIT, NULL, 0, 0);
    }
    player_add_gauge(record->slot, -damage);
    if (record->gauge <= 0.0F) {
        player_miss(record->slot);
    }
}

/* a player within LINE_REACH of a SHAPE_ID_LINE shape's line may stand upright
 * on it. one farther off may stand tilted to world_gravity. the way to the line
 * must then lie within 60 degrees of world_gravity. LINE_GRAVITY_COS is the
 * cosine of 60 degrees */
#define LINE_REACH 5.0F
#define LINE_GRAVITY_COS 0.5F

void player_ground_step(struct task *task, struct task_work *work,
                        struct player_motion *motion, struct player *record)
{
    struct task *line_task;
    struct ground_probe *ground;
    Bool falling;
    Angle ang_x;
    Angle ang_z;
    NJS_POINT3 near;
    NJS_LINE line;
    NJS_VECTOR side;
    NJS_VECTOR ahead;

#ifdef NONMATCHING
    /* GCC can't see that every path sets ang_x and ang_z. the up axis is one of
     * the six that each switch below has a case for */
    ang_x = 0;
    ang_z = 0;
#endif
    falling = FALSE;
    record->action_count = 0;
    record->hold_offer = NULL;
    switch (collision_get_up_axis()) {
    case COLLISION_UP_NEG_X:
        falling = motion->spd.x >= 0.0F;
        break;
    case COLLISION_UP_X:
        falling = motion->spd.x <= 0.0F;
        break;
    case COLLISION_UP_NEG_Y:
        falling = motion->spd.y >= 0.0F;
        break;
    case COLLISION_UP_Y:
        falling = motion->spd.y <= 0.0F;
        break;
    case COLLISION_UP_NEG_Z:
        falling = motion->spd.z >= 0.0F;
        break;
    case COLLISION_UP_Z:
        falling = motion->spd.z <= 0.0F;
        break;
    }
    if (game_state.cleared != 1 && game_state.mini_event_running != 1 &&
        work->mode != PLAYER_MODE_EVENT) {
        line_task = collision_first_with_id(task, SHAPE_ID_LINE);
    } else {
        line_task = NULL;
    }
    if (line_task) {
        line.px = line_task->work->pos.x;
        line.py = line_task->work->pos.y;
        line.pz = line_task->work->pos.z;
        near.x = 0.0F;
        near.y = 1.0F;
        near.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, line_task->work->ang.y);
        njRotateZ(NULL, line_task->work->ang.z);
        njCalcVector(NULL, &near, &near);
        njPopMatrixEx();
        line.vx = near.x;
        line.vy = near.y;
        line.vz = near.z;
        njDistanceP2L(&work->pos, &line, &near);
        njSubVector(&near, &work->pos);
        if (njScalor(&near) < LINE_REACH) {
            if (game_state.cleared != 1 && (work->flags & WORK_ON_TOP) &&
                falling == TRUE) {
                switch (collision_get_up_axis()) {
                case COLLISION_UP_NEG_X:
                    ang_x = 0;
                    ang_z = QUARTER_TURN;
                    break;
                case COLLISION_UP_X:
                    ang_x = 0;
                    ang_z = HALF_TURN + QUARTER_TURN;
                    break;
                case COLLISION_UP_NEG_Y:
                    ang_x = 0;
                    ang_z = HALF_TURN;
                    break;
                case COLLISION_UP_Y:
                    ang_x = 0;
                    ang_z = 0;
                    break;
                case COLLISION_UP_NEG_Z:
                    ang_x = HALF_TURN + QUARTER_TURN;
                    ang_z = 0;
                    break;
                case COLLISION_UP_Z:
                    ang_x = QUARTER_TURN;
                    ang_z = 0;
                    break;
                }
                motion->aim_x = work->ang.x = ang_x;
                motion->aim_z = work->ang.z = ang_z;
                work->flags |= PLAYER_WORK_ON_SHAPE;
            }
        } else {
            njUnitVector(&near);
            if (njInnerProduct(&near, &world_gravity) > LINE_GRAVITY_COS) {
                work->flags |= PLAYER_WORK_ON_SHAPE;
                side.x = 1.0F;
                side.y = 0.0F;
                side.z = 0.0F;
                turn_to_world(work, &side);
                motion->aim_x = work->ang.x = world_gravity_ang_x;
                motion->aim_z = work->ang.z = world_gravity_ang_z;
                ahead.x = 1.0F;
                ahead.y = 0.0F;
                ahead.z = 0.0F;
                turn_to_world(work, &ahead);
                if (njInnerProduct(&side, &ahead) < 0.0F) {
                    motion->aim_y += HALF_TURN;
                    work->ang.y = HALF_TURN + work->ang.y;
                }
            }
        }
    } else {
        /* @bug logic: the test that keeps a floating or swimming hunter off
         * a shape never holds. it asks for PLAYER_MODE_HUNTER_FLOAT and
         * PLAYER_MODE_HUNTER_SWIM at once, at 0x8c04cf36 and 0x8c04cf3c */
        if (game_state.cleared != 1 && (work->flags & WORK_ON_TOP) &&
            falling == TRUE &&
            (!collision_next_item_or_enemy(task) ||
             collision_next_object(task)) &&
            !collision_first_with_id(task, SHAPE_ID_NO_STAND) &&
            !((record->character == PLAYER_KNUCKLES ||
               record->character == PLAYER_ROUGE) &&
              work->mode == PLAYER_MODE_HUNTER_FLOAT &&
              work->mode == PLAYER_MODE_HUNTER_SWIM)) {
            record->stood_on = collision_next_below(task, 0);
            if (!(work->collision->shapes->flags & SHAPE_HURT_NO_PUSH) ||
                !(record->stood_on->work->collision->shapes->flags &
                  SHAPE_HURT_NO_PUSH)) {
                /* @hack redundant: a store that never runs */
                if (0) {
                    work->mode = PLAYER_MODE_STAND;
                }
                switch (collision_get_up_axis()) {
                case COLLISION_UP_NEG_X:
                    ang_x = 0;
                    ang_z = QUARTER_TURN;
                    break;
                case COLLISION_UP_X:
                    ang_x = 0;
                    ang_z = HALF_TURN + QUARTER_TURN;
                    break;
                case COLLISION_UP_NEG_Y:
                    ang_x = 0;
                    ang_z = HALF_TURN;
                    break;
                case COLLISION_UP_Y:
                    ang_x = 0;
                    ang_z = 0;
                    break;
                case COLLISION_UP_NEG_Z:
                    ang_x = HALF_TURN + QUARTER_TURN;
                    ang_z = 0;
                    break;
                case COLLISION_UP_Z:
                    ang_x = QUARTER_TURN;
                    ang_z = 0;
                    break;
                }
                work->ang.x = ang_x;
                motion->aim_x = ang_x;
                work->ang.z = ang_z;
                motion->aim_z = ang_z;
                work->flags |= PLAYER_WORK_ON_SHAPE;
                goto snap;
            }
        }
        work->flags &= ~PLAYER_WORK_ON_SHAPE;
        record->stood_on = NULL;
    }
snap:
    if (!(work->flags & PLAYER_WORK_GROUNDED) &&
        work->mode != PLAYER_MODE_UNK_35 &&
        work->mode != PLAYER_MODE_PLAY_ANIM &&
        !((record->character == PLAYER_KNUCKLES ||
           record->character == PLAYER_ROUGE) &&
          work->mode == PLAYER_MODE_HUNTER_CLIMB) &&
        pad_player_is_enabled(record->slot) &&
        world_gravity.y < -GRAVITY_STRAIGHT &&
        !(work->pos.y - record->consts.gravity > record->ground.below_y) &&
        !(record->ground.below_flags & SNAP_REFUSES)) {
        ground = &record->ground;
        /* @hack type: line's point and vector serve as the world's up and the
         * ground's, where two vectors of their own would do */
        line.px = -world_gravity.x;
        line.py = -world_gravity.y;
        line.pz = -world_gravity.z;
        near.x = 0.0F;
        near.y = 1.0F;
        near.z = 0.0F;
        line.vx = 0.0F;
        line.vy = 1.0F;
        line.vz = 0.0F;
        turn_to_world(work, &near);
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, ground->below_ang_z);
        njRotateX(NULL, ground->below_ang_x);
        njCalcVector(NULL, (NJS_VECTOR *)&line.vx, (NJS_VECTOR *)&line.vx);
        njPopMatrixEx();
        if (njInnerProduct(&near, (NJS_VECTOR *)&line.px) >=
                njCos(EIGHTH_TURN) &&
            njInnerProduct(&near, (NJS_VECTOR *)&line.vx) >=
                njCos(EIGHTH_TURN)) {
            if (game_state.mini_event_running == 0) {
                work->pos.y = ground->below_y;
            }
            record->ground_flags = ground->below_flags;
            if (angle_distance(work->ang.x, ground->below_ang_x) >
                    QUARTER_TURN &&
                angle_distance(work->ang.z, ground->below_ang_z) <
                    QUARTER_TURN) {
                ground->below_ang_x += HALF_TURN;
            } else if (angle_distance(work->ang.z, ground->below_ang_z) >
                           QUARTER_TURN &&
                       angle_distance(work->ang.x, ground->below_ang_x) <
                           QUARTER_TURN) {
                ground->below_ang_z += HALF_TURN;
            }
            motion->aim_x = work->ang.x = ground->below_ang_x;
            motion->aim_z = work->ang.z = ground->below_ang_z;
            work->flags |= PLAYER_WORK_ON_GROUND;
        }
    }
    if (work->submode == PLAYER_REQUEST_RESET) {
        work->flags &= ~PLAYER_RESET_WORK_FLAGS;
        work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_CHARGED |
                         PLAYER_WORK_ATTACK | PLAYER_WORK_HOLDING);
        work->flags &= ~(PLAYER_WORK_ON_PATH | PLAYER_WORK_NO_CONTROL |
                         PLAYER_WORK_UNK_8000);
        record->flags &= ~PLAYER_RESET_FLAGS;
        player_anim_end_blend(&record->anim);
        /* @hack spelling: the record read again through player_records, where
         * record would do */
        player_records[record->slot]->anim.flags |= PLAYER_ANIM_NO_BLEND;
        player_stand(work, motion, record);
        player_anim_step(&record->anim);
    }
    if (work->mode == PLAYER_MODE_MISS &&
        work->submode == PLAYER_REQUEST_ANIM &&
        (record->request_arg == ANIM_WIN ||
         (record->request_arg == ANIM_LOSE && game_state.two_players != 0))) {
        work->mode = PLAYER_MODE_PLAY_ANIM;
        record->anim.next = record->request_arg;
        player_stop(motion, record);
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        work->timer = 0;
    }
}

/* WORLD_HURTS ground hurts no player whose first shape guards at
 * INVINCIBLE_GUARD, a shape's highest level */
#define INVINCIBLE_GUARD 3
/* a hit throws a player away from a task of SHAPE_ID_THROWS at THROWN_SPEED.
 * the player then takes no stick for THROWN_STICK_LOCK steps */
#define THROWN_SPEED 8.0F
#define THROWN_STICK_LOCK 60
/* a hit's vibration_start_fading power, freq and length */
#define HIT_VIBRATION_POWER 7
#define HIT_VIBRATION_FREQ 59
#define HIT_VIBRATION_LENGTH 24

Bool player_take_hit(struct task_work *work, struct player *record)
{
    struct task *task;
    struct task *attack;
    struct task *thrower;
    int foreign; /* the last attack's: 1 another's, 0 the mech's, -1 none */
    int slot;
    int i;
    int stage;
    int id; /* the voice the hit plays, or in one case its sound */
    Float gauge;
    Float before;
    NJS_VECTOR away;

    id = -1;
    if (game_state.cleared != 0) {
        return FALSE;
    }
    if (work->id == WORK_ID_NO_HIT) {
        return FALSE;
    }
    switch (work->mode) {
    case PLAYER_MODE_MISS:
    case PLAYER_MODE_DROWN:
    case PLAYER_MODE_UNK_38:
        return FALSE;
    }
    if (work->flags & WORK_HURT) {
        /* @bug memory: reads record's character under WORK_HURT, at
         * 0x8c04d314, before the test for a null record at 0x8c04d3f6 */
        /* @hack type: the second test reads character again through a
         * volatile record, where record->character would do */
        if (record->character == PLAYER_TAILS_MECH ||
            ((volatile struct player *)record)->character ==
                PLAYER_EGGMAN_MECH) {
            task = player_tasks[record->slot];
            if (!collision_next_item_or_enemy(task) &&
                !collision_next_player(task)) {
                foreign = -1;
                /* @bug logic: the last attack scanned decides alone, so a
                 * mech's own attack scanned after another's spares it the
                 * other's hit. each sets foreign anew, at 0x8c04d348 and
                 * 0x8c04d3a2 */
                while ((attack = collision_next_attack(task)) != NULL) {
                    if (attack->parent == task) {
                        foreign = 0;
                    } else {
                        foreign = 1;
                    }
                }
                if (foreign == 0) {
                    work->flags &= ~WORK_HURT;
                }
            }
        }
        if ((work->flags & WORK_HURT) &&
            stage_current_id() == STAGE_VERSUS_SONIC_2) {
            task = player_tasks[record->slot];
            if (collision_next_object(task) ||
                collision_next_item_or_enemy(task)) {
                work->flags &= ~WORK_HURT;
            }
        }
    }
    if (!(work->flags & WORK_HURT)) {
        if (record == NULL ||
            collision_get_guard(player_tasks[record->slot], 0) ==
                INVINCIBLE_GUARD ||
            !(record->ground_flags & WORLD_HURTS)) {
            return FALSE;
        }
    }
    work->timer = PLAYER_HIT_STEPS;
    if (stage_current_id() == STAGE_BOSS_LAST2) {
        vibration_start_fading(0, HIT_VIBRATION_POWER, HIT_VIBRATION_FREQ,
                               HIT_VIBRATION_LENGTH);
    } else {
        vibration_start_fading(record->slot, HIT_VIBRATION_POWER,
                               HIT_VIBRATION_FREQ, HIT_VIBRATION_LENGTH);
    }
    if (record->upgrades & UPGRADE_BIT(UPGRADE_UNK_15)) {
        record->spd.x *= 0.5F;
        return FALSE;
    } else if (!(record->flags & PLAYER_MISSED)) {
        if (work->mode != PLAYER_MODE_UNK_38) {
            work->mode = PLAYER_MODE_HURT;
            record->anim.next = ANIM_HURT;
            work->flags &= ~WORK_HURT;
            record->spd.y = record->consts.jump_speed;
            record->spd.x = -record->consts.running_speed;
        }
        work->flags &=
            ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        if (record->flags & (PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER)) {
            record->flags &= ~(PLAYER_BARRIER | PLAYER_MAGNETIC_BARRIER);
        } else {
            player_hooks.drop_rings(record->slot);
        }
        game_state.ring_levels[record->slot][0] = 0;
        game_state.ring_levels[record->slot][1] = 0;
        game_state.ring_levels[record->slot][2] = 0;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((thrower = collision_first_with_id(player_tasks[record->slot],
                                               SHAPE_ID_THROWS)) != NULL) {
            slot = player_slot_of(thrower);
            if (slot == -1) {
                crash_stars_burst(record->slot);
                crash_stars_burst(record->slot);
                crash_stars_burst(record->slot);
                crash_stars_burst(record->slot);
                powder_create(record->slot, 48, &record->unk_218,
                              &record->unk_2b4);
            } else if (player_records[slot]->character == PLAYER_EGGMAN_MECH) {
                crash_stars_burst(record->slot);
                crash_stars_burst(record->slot);
            } else {
                powder_create(record->slot, 48, &record->unk_218,
                              &record->unk_2b4);
            }
            away = work->pos;
            njSubVector(&away, &thrower->work->pos);
            njUnitVector(&away);
            player_face_along(record->slot, -away.x, 0.0F, -away.z);
            away.x *= THROWN_SPEED;
            away.y *= THROWN_SPEED;
            away.z *= THROWN_SPEED;
            player_motion[record->slot]->spd = away;
            turn_from_world(work, &away);
            record->spd = away;
            record->stick_lock = THROWN_STICK_LOCK;
            work->mode = PLAYER_MODE_UNK_43;
            record->anim.next = ANIM_CRASH;
        }
        if (record->character == PLAYER_EGGMAN_MECH ||
            record->character == PLAYER_TAILS_MECH) {
            before = record->gauge;
            gauge_take_hit(work, record);
            if (work->id != WORK_ID_RIVAL) {
                if (record->gauge > 0.0F) {
                    if (before > 3.0F && record->gauge <= 3.0F) {
                        if (record->kind == PLAYER_TAILS_MECH) {
                            id = 1663;
                        } else if (record->kind == PLAYER_EGGMAN_MECH) {
                            id = 1662;
                        }
                    } else if (before > 6.0F && record->gauge <= 6.0F) {
                        if (record->kind == PLAYER_TAILS_MECH) {
                            id = 1661;
                        } else if (record->kind == PLAYER_EGGMAN_MECH) {
                            id = 1660;
                        }
                    }
                }
                if (id != -1) {
                    voice_play_priority_2(id);
                }
            } else {
                switch (stage_current_id()) {
                case STAGE_VERSUS_MECH_1:
                    if (before >= 5.0F && record->gauge < 5.0F) {
                        if (record->kind == PLAYER_TAILS_MECH) {
                            id = 2264;
                        } else if (record->kind == PLAYER_EGGMAN_MECH) {
                            id = 2072;
                        }
                    }
                    break;
                case STAGE_VERSUS_MECH_2:
                    if (before >= 5.0F && record->gauge < 5.0F) {
                        if (record->kind == PLAYER_TAILS_MECH) {
                            id = 2272;
                        } else if (record->kind == PLAYER_EGGMAN_MECH) {
                            id = 2081;
                        }
                    }
                    break;
                }
                if (id != -1) {
                    for (i = 0; i < VOICES; i++) {
                        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                            voice_stop(i);
                        }
                    }
                    voice_play(id, RIVAL_VOICE_PRIORITY);
                }
            }
        } else if (work->id == WORK_ID_RIVAL) {
            before = record->gauge;
            record->gauge = float_approach(record->gauge, 0.0F, 1.0F);
            if (record->gauge <= 0.0F) {
                player_miss(record->slot);
            } else if (before > 1.0F && record->gauge <= 1.0F) {
                switch (stage_current_id()) {
                case STAGE_VERSUS_SONIC_1:
                    if (record->kind == PLAYER_SONIC) {
                        id = 2239;
                    } else if (record->kind == PLAYER_SHADOW) {
                        id = 2215;
                    }
                    break;
                case STAGE_VERSUS_SONIC_2:
                    if (record->kind == PLAYER_SONIC) {
                        id = 2249;
                    } else if (record->kind == PLAYER_SHADOW) {
                        id = 2226;
                    }
                    break;
                case STAGE_VERSUS_KNUCKLES:
                    if (record->kind == PLAYER_KNUCKLES) {
                        id = 2110;
                    } else if (record->kind == PLAYER_ROUGE) {
                        id = 2197;
                    }
                    break;
                }
                if (id != -1) {
                    for (i = 0; i < VOICES; i++) {
                        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                            voice_stop(i);
                        }
                    }
                    voice_play(id, RIVAL_VOICE_PRIORITY);
                }
            }
        }
        if (((work->flags & PLAYER_WORK_REQUEST) &&
             work->submode == PLAYER_REQUEST_MISS) ||
            (work->id == WORK_ID_RIVAL && !(record->gauge > 0.0F) &&
             !(player_records[0]->flags & PLAYER_MISSED))) {
            player_release_held(work, player_motion[record->slot], record);
            if (id == -1) {
                if (work->id != WORK_ID_RIVAL) {
                    switch (record->kind) {
                    case PLAYER_SONIC:
                    case PLAYER_SUPER_SONIC:
                        id = 1567;
                        break;
                    case PLAYER_SHADOW:
                    case PLAYER_SUPER_SHADOW:
                        id = 1564;
                        break;
                    case PLAYER_KNUCKLES:
                        id = 1300;
                        break;
                    case PLAYER_ROUGE:
                        id = 1301;
                        break;
                    case PLAYER_EGGMAN:
                    case PLAYER_EGGMAN_MECH:
                        id = 1676;
                        break;
                    case PLAYER_TAILS:
                    case PLAYER_TAILS_MECH:
                        id = 1677;
                        break;
                    case PLAYER_KIND_AMY:
                        id = 2667;
                        break;
                    }
                    if (id != -1) {
                        voice_play_priority_2(id);
                    }
                } else {
                    stage = stage_current_id();
                    switch (record->kind) {
                    case PLAYER_SONIC:
                        if (stage == STAGE_VERSUS_SONIC_1) {
                            id = 2242;
                        } else if (stage == STAGE_VERSUS_SONIC_2) {
                            id = 2252;
                        }
                        break;
                    case PLAYER_SHADOW:
                        if (stage == STAGE_VERSUS_SONIC_1) {
                            id = 2218;
                        } else if (stage == STAGE_VERSUS_SONIC_2) {
                            id = 2227;
                        }
                        break;
                    case PLAYER_KNUCKLES:
                        id = 2111;
                        break;
                    case PLAYER_ROUGE:
                        id = 2198;
                        break;
                    case PLAYER_EGGMAN_MECH:
                        if (stage == STAGE_VERSUS_MECH_1) {
                            id = 2073;
                        } else if (stage == STAGE_VERSUS_MECH_2) {
                            id = 2082;
                        }
                        break;
                    case PLAYER_TAILS_MECH:
                        if (stage == STAGE_VERSUS_MECH_1) {
                            id = 2265;
                        } else if (stage == STAGE_VERSUS_MECH_2) {
                            id = 2273;
                        }
                        break;
                    }
                    if (id != -1) {
                        for (i = 0; i < VOICES; i++) {
                            if (voice_can_start(RIVAL_VOICE_PRIORITY, i) ==
                                TRUE) {
                                voice_stop(i);
                            }
                        }
                        voice_play(id, RIVAL_VOICE_PRIORITY);
                    }
                }
            }
            if (work->id == WORK_ID_RIVAL && record->gauge > 0.0F) {
                return TRUE;
            }
            work->mode = PLAYER_MODE_MISS;
            record->anim.next = ANIM_MISS;
            work->flags &=
                ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
        } else if (id == -1) {
            if (work->id != WORK_ID_RIVAL) {
                id = -1;
                switch (record->kind) {
                case PLAYER_SONIC:
                case PLAYER_SUPER_SONIC:
                    id = 0x300b;
                    break;
                case PLAYER_SHADOW:
                case PLAYER_SUPER_SHADOW:
                    id = 0x301d;
                    break;
                case PLAYER_KNUCKLES:
                    id = 0x300a;
                    break;
                case PLAYER_ROUGE:
                    id = 0x301b;
                    break;
                case PLAYER_EGGMAN:
                case PLAYER_EGGMAN_MECH:
                    id = 0x300f;
                    break;
                case PLAYER_TAILS:
                case PLAYER_TAILS_MECH:
                    id = 0x3005;
                    break;
                case PLAYER_KIND_AMY:
                    voice_play_priority_2(2676);
                    break;
                }
                if (id != -1) {
                    sound_play(id, NULL, 0, 0);
                }
            } else {
                stage = stage_current_id();
                gauge = record->gauge;
                switch (record->kind) {
                case PLAYER_SONIC:
                    if (stage == STAGE_VERSUS_SONIC_1) {
                        if (gauge > 2.0F) {
                            id = 2237;
                        } else {
                            id = 2238;
                        }
                    } else if (stage == STAGE_VERSUS_SONIC_2) {
                        if (gauge > 2.0F) {
                            id = 2237;
                        } else {
                            id = 2248;
                        }
                    }
                    break;
                case PLAYER_SHADOW:
                    if (stage == STAGE_VERSUS_SONIC_1) {
                        if (gauge > 2.0F) {
                            id = 2213;
                        } else {
                            id = 2214;
                        }
                    } else if (stage == STAGE_VERSUS_SONIC_2) {
                        if (gauge > 2.0F) {
                            id = 2213;
                        } else {
                            id = 2225;
                        }
                    }
                    break;
                case PLAYER_KNUCKLES:
                    if (gauge >= 3.0F) {
                        id = 2108;
                    } else {
                        id = 2109;
                    }
                    break;
                case PLAYER_ROUGE:
                    if (gauge >= 3.0F) {
                        id = 2195;
                    } else {
                        id = 2196;
                    }
                    break;
                case PLAYER_EGGMAN_MECH:
                    if (gauge > 6.0F) {
                        id = 2068;
                    } else {
                        id = 2069;
                    }
                    break;
                case PLAYER_TAILS_MECH:
                    if (stage == STAGE_VERSUS_MECH_1) {
                        if (gauge > 6.0F) {
                            id = 2260;
                        } else {
                            id = 2261;
                        }
                    } else if (stage == STAGE_VERSUS_MECH_2) {
                        if (gauge > 6.0F) {
                            id = 2260;
                        } else {
                            id = 2271;
                        }
                    }
                    break;
                }
                if (id != -1) {
                    for (i = 0; i < VOICES; i++) {
                        if (voice_can_start(RIVAL_VOICE_PRIORITY, i) == TRUE) {
                            voice_stop(i);
                        }
                    }
                    voice_play(id, RIVAL_VOICE_PRIORITY);
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

Bool player_test_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    int grade;

    if (ledge_in_reach(work, record) && motion->spd.y <= 0.0F &&
        record->stick_lock == 0) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((grade = player_stick_into_wall(work, record)) >=
            PLAYER_WALL_ASKEW) {
            return TRUE;
        }
        if (grade != PLAYER_WALL_AWAY && grade != PLAYER_WALL_ACROSS &&
            player_face_wall(work, record) >= PLAYER_WALL_ASKEW) {
            return TRUE;
        }
    }
    return FALSE;
}

/* a hanging player lies HANG_REACH of its radius from the wall, or for Tails
 * TAILS_HANG_REACH. its head lies at the ledge's top, Eggman's
 * EGGMAN_HANG_DROP below it, and Tails's TAILS_HANG_RISE above it */
#define HANG_REACH 0.65F
#define TAILS_HANG_REACH 0.7F
#define EGGMAN_HANG_DROP 1.0F
#define TAILS_HANG_RISE 4.0F

void player_hang_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    /* @hack redundant: a vector nothing reads */
    NJS_VECTOR unused; /* NOLINT(clang-diagnostic-unused-variable) */
    Float reach;
    int sound;

    if (njScalor(&record->wall_normal) == 0.0F) {
        return;
    }
    motion->aim_y = work->ang.y =
        (Angle)(atan2f(-record->wall_normal.z, -record->wall_normal.x) *
                ANGLE_PER_RADIAN);
#ifdef NONMATCHING
    /* GCC can't see that every path sets reach. only the characters on foot
     * hang from a ledge, and the switch has a case for each */
    reach = 0.0F;
#endif
    switch (record->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        reach = record->consts.radius * HANG_REACH;
        break;
    case PLAYER_EGGMAN:
        return;
    case PLAYER_TAILS:
        reach = record->consts.radius * TAILS_HANG_REACH;
        break;
    }
    work->pos.x += -record->wall_normal.x * reach;
    work->pos.y = (-record->wall_normal.y * reach) + record->ahead.below_y;
    switch (record->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        work->pos.y -= record->consts.height;
        break;
    case PLAYER_EGGMAN:
        work->pos.y -= record->consts.height + EGGMAN_HANG_DROP;
        break;
    case PLAYER_TAILS:
        work->pos.y -= record->consts.height - TAILS_HANG_RISE;
        break;
    }
    work->pos.z += -record->wall_normal.z * reach;
    player_stop(motion, record);
    work->mode = PLAYER_MODE_LEDGE_HANG;
    record->anim.next = ANIM_LEDGE_HANG;
    sound = -1;
    switch (record->kind) {
    case PLAYER_SONIC:
        sound = 0x3003;
        break;
    case PLAYER_SHADOW:
        sound = 0x3015;
        break;
    case PLAYER_KNUCKLES:
        sound = 0x3003;
        break;
    case PLAYER_ROUGE:
        sound = 0x3014;
        break;
    case PLAYER_TAILS:
        voice_play_priority_2(1719);
        break;
    case PLAYER_EGGMAN:
        voice_play_priority_2(1716);
        break;
    case PLAYER_KIND_AMY:
        voice_play_priority_2(2665);
        break;
    }
    if (sound != -1) {
        sound_play_at(sound, NULL, 0, SOUND_LOUDEST, &work->pos);
    }
}

/* how far a climb moves a player ahead onto the ledge, by its character */
#define CLIMB_STEP 3.12F
#define EGGMAN_CLIMB_STEP 4.34F
#define TAILS_CLIMB_STEP 3.76F

void player_onto_ledge(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_VECTOR step;

    switch (record->character) {
    case PLAYER_SONIC:
    case PLAYER_SHADOW:
    case PLAYER_KNUCKLES:
    case PLAYER_ROUGE:
        step.x = CLIMB_STEP;
        break;
    case PLAYER_EGGMAN:
        step.x = EGGMAN_CLIMB_STEP;
        break;
    case PLAYER_TAILS:
        step.x = TAILS_CLIMB_STEP;
        break;
    }
    step.y = record->ahead.below_y - work->pos.y;
    step.z = 0.0F;
    turn_to_world(work, &step);
    njAddVector(&work->pos, &step);
}

/* a player that falls from a ledge takes no stick for LEDGE_FALL_STICK_LOCK
 * steps */
#define LEDGE_FALL_STICK_LOCK 4

Bool player_test_ledge_hang(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    int grade;

    if (!ledge_in_reach(work, record)) {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        return TRUE;
    }
    if (work->mode == PLAYER_MODE_LEDGE_CLIMB) {
        return FALSE;
    }
    if (player_jump_pressed(work, record)) {
        work->mode = PLAYER_MODE_LEDGE_CLIMB;
        record->anim.next = ANIM_LEDGE_JUMP;
        return TRUE;
    }
    grade = player_stick_into_wall(work, record);
    if (grade >= PLAYER_WALL_ASKEW) {
        work->mode = PLAYER_MODE_LEDGE_CLIMB;
        record->anim.next = ANIM_LEDGE_CLIMB;
        return TRUE;
    }
    if (grade) {
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        record->stick_lock = LEDGE_FALL_STICK_LOCK;
        return TRUE;
    }
    return FALSE;
}

void player_stand(struct task_work *work, struct player_motion *motion,
                  struct player *record)
{
    if (work->mode == PLAYER_MODE_EVENT &&
        (game_state.solo_player & (1 << record->slot))) {
        return;
    }
    work->mode = PLAYER_MODE_STAND;
    record->mode_steps = 0;
    player_stand_anim(work, motion, record);
}

/* how long a player stands still, in steps, before it plays an idle */
#define IDLE_STEPS 900
/* how long a still landing holds, in steps, before the player stands */
#define LAND_STILL_STEPS 180
/* ground tilted less than this is level to a standing player */
#define LEVEL_TILT ((Angle)(25 * ANGLE_PER_DEGREE))
/* how far a standing player's aim may lie from its heading before it walks in
 * place */
#define AIM_SLACK 0x200

/* the first of each character's three idles. those of Tails, Eggman, Knuckles
 * and Eggman's mech start at ANIM_IDLE */
#define ANIM_IDLE 87
#define ANIM_SONIC_IDLE 88
#define ANIM_SHADOW_IDLE 158
#define ANIM_ROUGE_IDLE 141
#define ANIM_TAILS_MECH_IDLE 139
/* a pick from 0 up to IDLE_PICK rounds to one of the three idles */
#define IDLE_PICK 1.99F
/* the anim_speed of a walk in place, as a standing player turns */
#define TURN_IN_PLACE_PACE 0.1F
/* whether anim is one of character's idles */
#define IS_IDLE(character, anim)                                               \
    (((character) == PLAYER_SONIC && (anim) >= ANIM_SONIC_IDLE &&              \
      (anim) <= ANIM_SONIC_IDLE + 2) ||                                        \
     ((character) == PLAYER_SHADOW && (anim) >= ANIM_SHADOW_IDLE &&            \
      (anim) <= ANIM_SHADOW_IDLE + 2) ||                                       \
     ((character) == PLAYER_TAILS && (anim) >= ANIM_IDLE &&                    \
      (anim) <= ANIM_IDLE + 2) ||                                              \
     ((character) == PLAYER_EGGMAN && (anim) >= ANIM_IDLE &&                   \
      (anim) <= ANIM_IDLE + 2) ||                                              \
     ((character) == PLAYER_KNUCKLES && (anim) >= ANIM_IDLE &&                 \
      (anim) <= ANIM_IDLE + 2) ||                                              \
     ((character) == PLAYER_ROUGE && (anim) >= ANIM_ROUGE_IDLE &&              \
      (anim) <= ANIM_ROUGE_IDLE + 2) ||                                        \
     ((character) == PLAYER_EGGMAN_MECH && (anim) >= ANIM_IDLE &&              \
      (anim) <= ANIM_IDLE + 2) ||                                              \
     ((character) == PLAYER_TAILS_MECH && (anim) >= ANIM_TAILS_MECH_IDLE &&    \
      (anim) <= ANIM_TAILS_MECH_IDLE + 2))

void player_stand_anim(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_PLANE plane;
    NJS_VECTOR ahead;
    NJS_VECTOR down;
    NJS_VECTOR slope;
    Float level;
    Angle ang;
    int anim;
    int character;
    int idle;
    Float pick;

    if (record->anim.next == ANIM_STAND_UP ||
        record->anim.next == ANIM_LEDGE_CLIMB ||
        record->anim.next == ANIM_LEDGE_JUMP ||
        record->anim.next == ANIM_LAND || record->anim.next == ANIM_LAND_FAST ||
        record->anim.next == ANIM_TURN ||
        ((record->character == PLAYER_KNUCKLES ||
          record->character == PLAYER_ROUGE) &&
         record->anim.next == ANIM_DIVE_LAND)) {
        return;
    }
    level = -njInnerProduct(&world_gravity, &record->ground_normal);
    if (level > njCos(LEVEL_TILT) || (record->ground_flags & WORLD_UPRIGHT)) {
        anim = record->anim.next;
        if (record->character == PLAYER_ROUGE &&
            motion_table[ANIM_ROUGE_STAND_216].motion != NULL &&
            player_hooks.in_shadow != NULL && player_hooks.in_shadow() != 0) {
            record->anim.next = ANIM_ROUGE_STAND_216;
        } else if (!(anim == ANIM_STAND || anim == ANIM_LAND_STILL ||
                     anim == ANIM_UNK_2 || anim == ANIM_UNK_3 ||
                     IS_IDLE(record->character, anim))) {
            record->anim.next = ANIM_STAND;
        } else if (record->anim.next == ANIM_LAND_STILL &&
                   record->mode_steps > LAND_STILL_STEPS) {
            record->mode_steps = 0;
            record->anim.next = ANIM_STAND;
        } else if (record->mode_steps > IDLE_STEPS) {
            /* @hack spelling: the scale in two statements, where one
             * expression would do */
            pick = (Float)rand() * RAND_SCALE;
            pick *= IDLE_PICK;
            idle = (int)njRoundOff(pick);
            record->mode_steps = 0;
            switch (record->character) {
            case PLAYER_SONIC:
                record->anim.next = idle + ANIM_SONIC_IDLE;
                break;
            case PLAYER_SHADOW:
                record->anim.next = idle + ANIM_SHADOW_IDLE;
                break;
            case PLAYER_TAILS:
                record->anim.next = idle + ANIM_IDLE;
                break;
            case PLAYER_EGGMAN:
                record->anim.next = idle + ANIM_IDLE;
                break;
            case PLAYER_KNUCKLES:
                record->anim.next = idle + ANIM_IDLE;
                break;
            case PLAYER_ROUGE:
                record->anim.next = idle + ANIM_ROUGE_IDLE;
                break;
            case PLAYER_EGGMAN_MECH:
                record->anim.next = idle + ANIM_IDLE;
                break;
            case PLAYER_TAILS_MECH:
                record->anim.next = idle + ANIM_TAILS_MECH_IDLE;
                break;
            }
            idle_voice(work, record, idle);
            record->anim.after = ANIM_STAND;
        }
    } else if (!(record->ground_flags & WORLD_UPRIGHT) &&
               level > njCos(EIGHTH_TURN)) {
        if (angle_distance(motion->aim_y, work->ang.y) > AIM_SLACK) {
            record->anim.next = ANIM_WALK;
            record->anim_speed = TURN_IN_PLACE_PACE;
        } else {
            plane.vx = record->ground_normal.x;
            plane.vy = record->ground_normal.y;
            plane.vz = record->ground_normal.z;
            plane.pz = 0.0F;
            plane.py = 0.0F;
            plane.px = 0.0F;
            ahead.x = 1.0F;
            ahead.z = 0.0F;
            ahead.y = 0.0F;
            turn_to_world(work, &ahead);
            njDistanceP2PL(&ahead, &plane, &slope);
            njUnitVector(&slope);
            njDistanceP2PL(&world_gravity, &plane, &down);
            njUnitVector(&down);
            ang = vector_angle(&slope, &down, &record->ground_normal);
            if (ang > HALF_TURN + QUARTER_TURN || ang < QUARTER_TURN) {
                record->anim.next = ANIM_STAND_DOWNHILL;
            } else {
                record->anim.next = ANIM_STAND_UPHILL;
            }
        }
    }
    anim = record->anim.next;
    character = record->character;
    if (!IS_IDLE(character, anim)) {
        record->mode_steps++;
    } else if (game_state.steps & 1) {
        record->mode_steps++;
    }
}

void player_hold_stand_anim(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    NJS_PLANE plane;
    NJS_VECTOR ahead;
    NJS_VECTOR down;
    NJS_VECTOR slope;
    Float level;
    Angle ang;

    if (record->anim.next == ANIM_HOLD_BRAKE ||
        record->anim.next == ANIM_HOLD_TURN ||
        record->anim.next == ANIM_HOLD_LAND_FAST ||
        record->anim.next == ANIM_PULL_LIFT) {
        return;
    }
    level = -njInnerProduct(&world_gravity, &record->ground_normal);
    if (level > njCos(LEVEL_TILT)) {
        /* @hack redundant: the test of ANIM_HOLD_STAND before it is stored,
         * where the store alone would do */
        if (record->anim.next == ANIM_HOLD_STAND) {
            return;
        }
        record->anim.next = ANIM_HOLD_STAND;
        return;
    }
    if (!(record->ground_flags & WORLD_UPRIGHT) && level > njCos(EIGHTH_TURN)) {
        plane.vx = record->ground_normal.x;
        plane.vy = record->ground_normal.y;
        plane.vz = record->ground_normal.z;
        plane.pz = 0.0F;
        plane.py = 0.0F;
        plane.px = 0.0F;
        ahead.x = 1.0F;
        ahead.z = 0.0F;
        ahead.y = 0.0F;
        turn_to_world(work, &ahead);
        njDistanceP2PL(&ahead, &plane, &slope);
        njUnitVector(&slope);
        njDistanceP2PL(&world_gravity, &plane, &down);
        njUnitVector(&down);
        ang = vector_angle(&slope, &down, &record->ground_normal);
        if (ang > HALF_TURN + QUARTER_TURN || ang < QUARTER_TURN) {
            record->anim.next = ANIM_HOLD_STAND_DOWNHILL;
        } else {
            record->anim.next = ANIM_HOLD_STAND_UPHILL;
        }
    }
}

/* the weight of the record's slip in the speed that picks a run's animation,
 * and in its anim_speed */
#define SLIP_SPEED 5.0F
#define SLIP_PACE 20.0F
/* a running player leans toward four times its turn over the last LEAN_PAST
 * steps, at most LEAN_MOST. each step closes a quarter of the way.
 * LEAN_GAIN_SHIFT and LEAN_EASE_SHIFT are those factors as shifts */
#define LEAN_PAST 3
#define LEAN_MOST 0x1d00
#define LEAN_GAIN_SHIFT 2
#define LEAN_EASE_SHIFT 2

void player_run_anim(struct task_work *work, struct player_motion *motion,
                     struct player *record)
{
    Float speed = record->spd.x + (SLIP_SPEED * record->slip);
    struct angles ang;
    Angle lean;

    /* @hack spelling: an else after the return, where the if alone would
     * do */
    if (record->anim.next == ANIM_STAND_UP || record->anim.next == ANIM_TURN ||
        record->anim.next == ANIM_HOLD_TURN) {
        return;
    } else if (work->flags & PLAYER_WORK_HOLDING) {
        if (speed > record->consts.running_speed) {
            record->anim.next = ANIM_HOLD_RUN;
        } else {
            record->anim.next = ANIM_HOLD_WALK;
        }
    } else if (speed > record->consts.full_speed) {
        record->anim.next = ANIM_FULL_SPEED;
    } else if (speed > record->consts.sprint_speed) {
        record->anim.next = ANIM_SPRINT;
    } else if (speed > record->consts.running_speed) {
        record->anim.next = ANIM_RUN;
    } else if (speed > record->consts.trot_speed) {
        record->anim.next = ANIM_TROT;
    } else {
        record->anim.next = ANIM_WALK;
    }
    if (record->spd.x >= record->consts.running_speed) {
        player_get_past(record->slot, LEAN_PAST, NULL, &ang);
        lean = angle_approach(
            0, angle_difference(work->ang.y, ang.y) << LEAN_GAIN_SHIFT,
            LEAN_MOST);
    } else {
        lean = 0;
    }
    record->lean =
        angle_approach(record->lean, lean,
                       angle_distance(record->lean, lean) >> LEAN_EASE_SHIFT);
    record->anim_speed = record->spd.x + (SLIP_PACE * record->slip);
}

Bool player_test_stand(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    int flags;

    if (player_read_stick(work, record, NULL, NULL) == FALSE &&
        record->spd.x == 0.0F) {
        if (work->flags & PLAYER_WORK_HOLDING) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((flags = player_action_flags(work, record)) & ACTION_FLAG_800) {
                work->mode = PLAYER_MODE_UNK_17;
                record->anim.next = ANIM_GRAB_STAND;
            } else if (flags & ACTION_FLAG_400) {
                work->mode = PLAYER_MODE_GRAB_PUSHABLE;
                record->anim.next = ANIM_GRAB_STAND;
            } else {
                work->mode = PLAYER_MODE_STAND_HOLDING;
                record->anim.next = ANIM_HOLD_STAND;
            }
        } else {
            player_stand(work, motion, record);
        }
        return TRUE;
    }
    return FALSE;
}

/* a player teeters over a drop of TEETER_DROP or more. it goes back to where it
 * stood TEETER_BACK steps before, or a mech MECH_TEETER_BACK */
#define TEETER_DROP 60.0F
#define TEETER_BACK 2
#define MECH_TEETER_BACK 3

Bool player_test_fall(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    int sound;

    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        if (pad_player_is_enabled(record->slot) &&
            !(record->last_ground_flags & WORLD_MOVING) &&
            record->character != PLAYER_TAILS_MECH &&
            record->character != PLAYER_EGGMAN_MECH &&
            (work->mode == PLAYER_MODE_RUN ||
             ((record->character == PLAYER_SONIC ||
               record->character == PLAYER_SHADOW) &&
              work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
              work->mode <= PLAYER_MODE_SONIC_COMBO_RUNNING_3)) &&
            record->stick_lock == 0 &&
            njInnerProduct(&record->ground_normal, &world_gravity) <
                -NEAR_LEVEL &&
            record->spd.x >= record->consts.trot_speed &&
            njScalor(&record->wall_normal) == 0.0F) {
            if (((world_gravity.y < -GRAVITY_STRAIGHT &&
                  record->ground.below_y <= work->pos.y - TEETER_DROP) ||
                 (world_gravity.y > GRAVITY_STRAIGHT &&
                  record->ground.above_y >= work->pos.y + TEETER_DROP)) &&
                !(record->ground.above_flags & WORLD_FLAT)) {
                work->mode = PLAYER_MODE_TEETER;
                record->anim.next = ANIM_TEETER;
                player_stop(motion, record);
                if (record->character == PLAYER_EGGMAN_MECH ||
                    record->character == PLAYER_TAILS_MECH) {
                    player_get_past(record->slot, MECH_TEETER_BACK, &work->pos,
                                    &work->ang);
                } else {
                    player_get_past(record->slot, TEETER_BACK, &work->pos,
                                    &work->ang);
                }
                record->mode_steps = 0;
                sound = -1;
                switch (record->kind) {
                case PLAYER_SONIC:
                    sound = 0x3002;
                    break;
                case PLAYER_SHADOW:
                    sound = 0x3014;
                    break;
                case PLAYER_KNUCKLES:
                    sound = 0x3002;
                    break;
                case PLAYER_ROUGE:
                    sound = 0x3013;
                    break;
                case PLAYER_EGGMAN_MECH:
                    sound = 0x300c;
                    break;
                case PLAYER_EGGMAN:
                    voice_play_priority_2(1630);
                    break;
                case PLAYER_TAILS_MECH:
                    sound = 0x3002;
                    break;
                case PLAYER_TAILS:
                    voice_play_priority_2(1631);
                    break;
                case PLAYER_KIND_AMY:
                    voice_play_priority_2(2674);
                    break;
                }
                if (sound != -1) {
                    sound_play_at(sound, NULL, 0, SOUND_LOUDEST, &work->pos);
                }
                return TRUE;
            }
        } else if (work->flags & PLAYER_WORK_BALL) {
            work->mode = PLAYER_MODE_JUMP;
            return TRUE;
        } else if (work->flags & PLAYER_WORK_HOLDING) {
            if (player_action_flags(work, record) & ACTION_FLAG_200) {
                work->mode = PLAYER_MODE_FALL_HOLDING;
                record->anim.next = ANIM_HOLD_FALL;
                return TRUE;
            }
            player_release_held(work, motion, record);
        }
        work->mode = PLAYER_MODE_FALL;
        record->anim.next = ANIM_FALL;
        return TRUE;
    }
    return FALSE;
}

Bool player_test_slow(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    if (record->spd.x < record->consts.trot_speed) {
        if (record->spd.x <= 0.0F) {
            work->mode = PLAYER_MODE_STAND;
            record->mode_steps = 0;
        } else {
            work->mode = PLAYER_MODE_RUN;
        }
        record->anim.next = ANIM_STAND_UP;
        work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
        return TRUE;
    }
    return FALSE;
}

Bool player_test_push(struct task_work *work, struct player_motion *motion,
                      struct player *record)
{
    int grade;
    int voice;

    if (!(player_pad_records[record->slot].press & pad_grab_buttons)) {
        /* @hack type: record passed as the address of its first field */
        grade = player_stick_into_offer(work, (struct player *)&record->slot);
        if (grade) {
            if (player_action_flags(work, record) & ACTION_FLAG_800) {
                work->mode = PLAYER_MODE_UNK_22;
                record->anim.next = ANIM_SHAKE;
                voice = -1;
                /* @bug logic: picks the voice by the record's slot where its
                 * character is meant. the tests at 0x8c04ed32 and 0x8c04ed38
                 * give Eggman's 1728 to slot 3 and Tails's 1731 to slot 2,
                 * PLAYER_EGGMAN's and PLAYER_TAILS's numbers, as eggman.c and
                 * tails.c play them */
                switch (record->slot) {
                case 3:
                    voice = 1728;
                    break;
                case 2:
                    voice = 1731;
                    break;
                }
                if (voice != -1) {
                    voice_play_priority_2(voice);
                }
            } else if (grade == PLAYER_WALL_HEAD_ON) {
                work->mode = PLAYER_MODE_UNK_0E;
                record->anim.next = ANIM_PUSH_START;
            } else {
                work->mode = PLAYER_MODE_UNK_20;
                record->anim.next = ANIM_DRAG;
            }
            return TRUE;
        }
    }
    return FALSE;
}

Bool player_test_hold_end(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    if (!(work->flags & PLAYER_WORK_HOLDING)) {
        if (work->flags & PLAYER_WORK_GROUNDED) {
            player_stand(work, motion, record);
        } else {
            work->mode = PLAYER_MODE_FALL;
            record->anim.next = ANIM_FALL;
        }
        if (record->held) {
            record->held->work->flags &= ~WORK_HELD_BY_PLAYER;
            record->held = NULL;
        }
        return TRUE;
    }
    return FALSE;
}

/* a task the player lets go of with ACTION_FLAG_200 flies off ahead and up. it
 * flies at RELEASE_THROW along each, in the player's axes */
#define RELEASE_THROW 1.0F

void player_release_held(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    NJS_VECTOR throw;
    struct task_work *held;
    int flags;

    if (work->flags & PLAYER_WORK_HOLDING) {
        held = record->held->work;
        if (held) {
            held->flags &= ~WORK_HELD_BY_PLAYER;
        }
        work->flags &= ~PLAYER_WORK_HOLDING;
        flags = player_action_flags(work, record);
        if (flags & ACTION_FLAG_200) {
            throw.x = RELEASE_THROW;
            throw.y = RELEASE_THROW;
            throw.z = 0.0F;
            turn_to_world(work, &throw);
            TASK_MOTION_OF(record->held)->spd = throw;
        } else if (flags & ACTION_FLAG_400) {
            player_set_down(work, record->held->work, record);
        }
        record->held = NULL;
    }
}

/* the steps a pull lasts before it gives up, unless the action button ends it
 * sooner */
#define PULL_STEPS 300

Bool player_test_pull_heavy(struct task_work *work,
                            struct player_motion *motion, struct player *record)
{
    player_approach_touched(work, motion, record);
    if (record->mode_steps++ >= PULL_STEPS ||
        pad_action_pressed[record->slot]) {
        if (record->anim.next == ANIM_PULL) {
            record->anim.next = ANIM_PULL_END;
            record->anim.after = ANIM_STAND;
        }
    }
    if (record->anim.next == ANIM_STAND) {
        player_stand(work, motion, record);
    }
    return FALSE;
}

/* player_test_shake_held counts mode_steps from SHAKE_COUNT_START, as
 * player_pick_up sets it, and is TRUE from SHAKE_COUNT_END on.
 * player_test_shake_end sets the count back to SHAKE_COUNT_START as it runs
 * out */
#define SHAKE_COUNT_START 60
#define SHAKE_COUNT_END 120

Bool player_test_shake_held(struct task_work *work, struct player *record)
{
    if (!(player_pad_records[record->slot].on & pad_grab_buttons)) {
        record->mode_steps = 0;
        return FALSE;
    }
    if (record->held->work->id != FALL_KIND_NORMAL) {
        return FALSE;
    }
    if (player_read_stick(work, record, NULL, NULL)) {
        record->mode_steps = 0;
        return TRUE;
    }
    if (record->mode_steps >= SHAKE_COUNT_START &&
        record->mode_steps++ >= SHAKE_COUNT_END) {
        return TRUE;
    }
    return FALSE;
}

Bool player_test_shake_end(struct task_work *work, struct player *record)
{
    if (record->mode_steps == 0) {
        if (!player_read_stick(work, record, NULL, NULL)) {
            return TRUE;
        }
    } else if (!(player_pad_records[record->slot].on & pad_grab_buttons)) {
        record->mode_steps = 0;
        return TRUE;
    } else {
        record->mode_steps--;
        if (record->mode_steps < 0) {
            record->mode_steps = SHAKE_COUNT_START;
            return TRUE;
        }
    }
    return FALSE;
}

/* a petting turns and steps the player toward the pet over its first
 * PET_TURN_STEPS. the aim turns by PET_AIM_TURN a step at most, and the heading
 * by PET_TURN. the player steps toward PET_NEAR from the pet, or for Tails
 * TAILS_PET_NEAR */
#define PET_TURN_STEPS 60
#define PET_AIM_TURN 0x800
#define PET_TURN 0xc00
#define PET_NEAR 4.5F
#define TAILS_PET_NEAR 5.5F

Bool player_test_petting(struct task_work *work, struct player_motion *motion,
                         struct player *record)
{
    struct task *task;
    NJS_VECTOR way;
    Float near;

    if (record->mode_steps < PET_TURN_STEPS) {
        record->mode_steps++;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = collision_nearest_with_id_within(
             player_tasks[record->slot], QUARTER_TURN - work->ang.y,
             EIGHTH_TURN, SHAPE_ID_PET)) != NULL) {
        /* @hack spelling: an empty arm for a task beyond PET_HEIGHT, where the
         * else's test negated would do */
        if (_builtin_fabsf(task->work->pos.y - work->pos.y) > PET_HEIGHT) {
        } else if (pad_action_held[record->slot] != 0 ||
                   record->mode_steps < PET_TURN_STEPS) {
            if (record->mode_steps < PET_TURN_STEPS) {
                way = task->work->pos;
                njSubVector(&way, &work->pos);
                motion->aim_y = angle_approach(
                    motion->aim_y,
                    (Angle)(atan2f(way.z, way.x) * ANGLE_PER_RADIAN),
                    PET_AIM_TURN);
                work->ang.y =
                    angle_approach(work->ang.y, motion->aim_y, PET_TURN);
                if (record->character == PLAYER_TAILS) {
                    near = TAILS_PET_NEAR;
                } else {
                    near = PET_NEAR;
                }
                if (njScalor(&way) >= near) {
                    if (njScalor(&way) >= near + 1.0F) {
                        njUnitVector(&way);
                    } else {
                        near = njScalor(&way) - near;
                        njUnitVector(&way);
                        way.x *= near;
                        way.y *= near;
                        way.z *= near;
                    }
                    work->pos.x += way.x;
                    work->pos.y += way.y;
                    work->pos.z += way.z;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

Bool player_offer_hint(struct task_work *work, struct player *record)
{
    int mode;

    if (work->flags & PLAYER_WORK_GROUNDED) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((mode = work->mode) == PLAYER_MODE_STAND ||
            mode == PLAYER_MODE_RUN) {
            if (record->action == PLAYER_MODE_ASK_HINT) {
                OFFER(record, PLAYER_MODE_ASK_HINT);
                return TRUE;
            }
        }
    }
    return FALSE;
}

Bool player_offer_flute(struct task_work *work, struct player *record)
{
    if (work->mode == PLAYER_MODE_STAND) {
        if (!((record->character == PLAYER_SONIC &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_SONIC_MELODY))) ||
              (record->character == PLAYER_SHADOW &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_SHADOW_MELODY))) ||
              (record->character == PLAYER_KNUCKLES &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_KNUCKLES_MELODY))) ||
              (record->character == PLAYER_ROUGE &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_ROUGE_MELODY))) ||
              (record->character == PLAYER_EGGMAN_MECH &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_EWALKER_MELODY))) ||
              (record->character == PLAYER_TAILS_MECH &&
               (record->upgrades & UPGRADE_BIT(UPGRADE_TWALKER_MELODY))))) {
            if (record->action == PLAYER_MODE_PLAY_FLUTE ||
                record->action == PLAYER_MODE_CANT_PLAY_FLUTE) {
                OFFER(record, PLAYER_MODE_CANT_PLAY_FLUTE);
                return TRUE;
            }
        } else if (record->action == PLAYER_MODE_PLAY_FLUTE_AGAIN) {
            OFFER(record, PLAYER_MODE_PLAY_FLUTE_AGAIN);
            return TRUE;
        } else if (record->action == PLAYER_MODE_PLAY_FLUTE) {
            OFFER(record, PLAYER_MODE_PLAY_FLUTE);
            return TRUE;
        }
    }
    return FALSE;
}

Bool player_offer_fall(struct task_work *work, struct player *record)
{
    if (work->flags & PLAYER_WORK_GROUNDED) {
        return FALSE;
    }
    OFFER(record, PLAYER_MODE_FALL);
    return TRUE;
}

Bool player_offer_target_actions(struct task_work *work,
                                 struct player_motion *motion,
                                 struct player *record)
{
    struct task *task;
    int flags;
    Bool offered;

    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        return FALSE;
    }
    if (work->flags & PLAYER_WORK_HOLDING) {
        return FALSE;
    }
    if (work->mode >= PLAYER_MODE_PLAY_ANIM &&
        work->mode < PLAYER_MODE_UNK_3B) {
        return FALSE;
    }
    if ((record->character == PLAYER_KNUCKLES ||
         record->character == PLAYER_ROUGE) &&
        work->mode >= PLAYER_MODE_HUNTER_DIG &&
        work->mode <= PLAYER_MODE_HUNTER_RUN_ATTACK_3) {
        return FALSE;
    }
    if ((record->ground.above_flags & WORLD_FLAT_THIN) &&
        _builtin_fabsf(record->ground.above_y - record->ground.below_y) >
            WATER_DEEP &&
        stage_current_id() != STAGE_CHAO) {
        return FALSE;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((flags = player_action_flags(work, record)) == 0) {
        return FALSE;
    }
    offered = FALSE;
    switch (player_face_offer(work, motion, record)) {
    case PLAYER_WALL_HEAD_ON:
    case PLAYER_WALL_ASKEW:
    case PLAYER_WALL_ACROSS:
    case PLAYER_WALL_AWAY:
        if (flags & ACTION_FLAG_400) {
            offered = TRUE;
            OFFER(record, PLAYER_MODE_GRAB_PUSHABLE);
        }
        if (flags & ACTION_FLAG_200) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((task = collision_nearest_with_id_within(
                     player_tasks[record->slot], QUARTER_TURN - work->ang.y,
                     EIGHTH_TURN, SHAPE_ID_PET)) != NULL) {
                if (_builtin_fabsf(task->work->pos.y - work->pos.y) <=
                    PET_HEIGHT) {
                    OFFER(record, PLAYER_MODE_PET_CHAO);
                }
                OFFER(record, PLAYER_MODE_PICK_UP);
            } else if (collision_nearest_unk_9_within(
                           player_tasks[record->slot],
                           QUARTER_TURN - work->ang.y, EIGHTH_TURN)) {
                OFFER(record, PLAYER_MODE_PICK_UP);
            } else if (player_normal_kind_flag(work, record)) {
                OFFER(record, PLAYER_MODE_PICK_UP);
            } else {
                OFFER(record, PLAYER_MODE_PULL_HEAVY);
            }
            offered = TRUE;
        }
        if (flags & ACTION_FLAG_800) {
            offered = TRUE;
            OFFER(record, PLAYER_MODE_UNK_17);
        }
        if (flags & ACTION_FLAG_1000) {
            offered = TRUE;
            OFFER(record, PLAYER_MODE_PULL_BURIED);
        }
        return offered;
    }
    return FALSE;
}

Bool player_offer_release(struct task_work *work, struct player *record)
{
    if (work->mode == PLAYER_MODE_GRAB_PUSHABLE ||
        work->mode == PLAYER_MODE_UNK_17) {
        if (player_action_flags(work, record) &
            (ACTION_FLAG_400 | ACTION_FLAG_800)) {
            OFFER(record, PLAYER_MODE_LET_GO);
            return TRUE;
        }
    }
    return FALSE;
}

Bool player_offer_held_actions(struct task_work *work, struct player *record)
{
    if (!(work->flags & PLAYER_WORK_HOLDING)) {
        return FALSE;
    }
    if (!(player_action_flags(work, record) & ACTION_FLAG_200)) {
        return FALSE;
    }
    /* @hack type: the held task's shape's id read as a Uint8, where the
     * Sint8 it is would do */
    if (!(work->flags & PLAYER_WORK_GROUNDED)) {
        OFFER(record, PLAYER_MODE_AIR_THROW);
    } else if ((Uint8)record->held->work->collision->shapes->id ==
               SHAPE_ID_NO_SET_DOWN) {
        OFFER(record, PLAYER_MODE_THROW);
    } else if (record->spd.x < record->consts.trot_speed) {
        OFFER(record, PLAYER_MODE_UNK_14);
    } else {
        OFFER(record, PLAYER_MODE_THROW);
    }
    return TRUE;
}

Bool player_offer_call(struct task_work *work, struct player *record)
{
    if (work->mode != PLAYER_MODE_STAND) {
        return FALSE;
    }
    if (record->action_count == 0) {
        OFFER(record, ACTION_SKIPPED);
    }
    OFFER(record, PLAYER_MODE_UNK_2A);
    return TRUE;
}

void player_start_push(struct task_work *work, struct player_motion *motion,
                       struct player *record)
{
    NJS_VECTOR away;

    work->mode = PLAYER_MODE_UNK_0E;
    record->anim.next = ANIM_PUSH_START;
    work->flags &=
        ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
    if (!player_dir_from_offer(work, record, &away)) {
        collision_get_normal(work, &away);
    }
    motion->aim_y = (Angle)(atan2f(-away.z, -away.x) * ANGLE_PER_RADIAN);
}

void player_ask_hint(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_ASK_HINT;
    record->anim.next = ANIM_STAND;
    work->flags &=
        ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
}

void player_play_flute(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_PLAY_FLUTE;
    record->anim.next = ANIM_FLUTE;
    record->anim.after = ANIM_STAND;
    work->flags &=
        ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
    flute_create(record->slot);
}

void player_play_flute_again(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_PLAY_FLUTE_AGAIN;
    record->anim.next = ANIM_FLUTE;
    record->anim.after = ANIM_FLUTE_AGAIN;
    work->flags &=
        ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
    flute_create(record->slot);
}

void player_cant_play_flute(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_CANT_PLAY_FLUTE;
    record->anim.next = ANIM_CANT_PLAY_FLUTE;
    record->anim.after = ANIM_STAND;
    work->flags &=
        ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK | PLAYER_WORK_ON_PATH);
}

void player_fall(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_FALL;
    player_stop(NULL, record);
    record->anim.next = ANIM_FALL;
    record->anim_speed = 0.0F;
    work->flags &= ~(PLAYER_WORK_BALL | PLAYER_WORK_ATTACK);
}

void player_hold(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_GRAB_PUSHABLE;
    record->anim.next = ANIM_GRAB;
    if (record->hold_offer) {
        record->held = record->hold_offer;
    } else {
        record->held = work->collision->hit->owner;
    }
    work->flags |= PLAYER_WORK_HOLDING;
}

void player_pick_up(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_PICK_UP;
    record->anim.next = ANIM_PICK_UP;
    record->anim.after = ANIM_HOLD_STAND;
    record->mode_steps = SHAKE_COUNT_START;
    if (!(record->spd.x < record->consts.trot_speed)) {
        record->spd.x = record->consts.trot_speed;
    }
}

void player_pull_buried(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_PULL_BURIED;
    record->anim.next = ANIM_PULL_START;
    record->mode_steps = 0;
    if (!(record->spd.x < record->consts.trot_speed)) {
        record->spd.x = record->consts.trot_speed;
    }
}

void player_grab_shakable(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_UNK_17;
    record->anim.next = ANIM_GRAB;
}

void player_air_throw_held(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_AIR_THROW;
    if (player_has_target(record->slot)) {
        record->anim.next = ANIM_AIR_THROW_AT;
    } else {
        record->anim.next = ANIM_AIR_THROW;
    }
}

void player_throw_held(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_THROW;
    if (player_has_target(record->slot)) {
        record->anim.next = ANIM_THROW_AT;
    } else {
        record->anim.next = ANIM_THROW;
    }
}

void player_throw_at_touch(struct task_work *work, struct player *record)
{
    work->mode = PLAYER_MODE_THROW_AT_TOUCH;
    record->anim.next = ANIM_THROW_AT_TOUCH;
}

void player_set_down_held(struct task_work *work, struct player_motion *motion,
                          struct player *record)
{
    work->mode = PLAYER_MODE_UNK_14;
    record->anim.next = ANIM_SET_DOWN;
    player_stop(motion, record);
}

void player_release_and_stand(struct task_work *work,
                              struct player_motion *motion,
                              struct player *record)
{
    player_release_held(work, motion, record);
    player_stand(work, motion, record);
}

/* the call, PLAYER_MODE_UNK_2A, plays its character's sound CALL_SOUND_WAIT
 * steps after it starts, or EGGMAN_CALL_SOUND_WAIT steps for Eggman */
#define CALL_SOUND_WAIT 20
#define EGGMAN_CALL_SOUND_WAIT 42

void player_call(struct task_work *work, struct player *record)
{
    int sound;
    int wait;

    work->mode = PLAYER_MODE_UNK_2A;
    record->anim.next = ANIM_CALL;
#ifdef NONMATCHING
    /* GCC can't see that every path sets sound and wait. a record's character
     * is one of the eight the switch has cases for */
    sound = 0x2005;
    wait = CALL_SOUND_WAIT;
#endif
    /* @hack spelling: a case for each character, where the cases that share
     * a body would do */
    switch (record->character) {
    case PLAYER_SONIC:
        sound = 0x2005;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_SHADOW:
        sound = 0x2006;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_KNUCKLES:
        sound = 0x2005;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_ROUGE:
        sound = 0x2006;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_EGGMAN_MECH:
        sound = 0x2006;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_TAILS_MECH:
        sound = 0x2005;
        wait = CALL_SOUND_WAIT;
        break;
    case PLAYER_EGGMAN:
        sound = 0x2006;
        wait = EGGMAN_CALL_SOUND_WAIT;
        break;
    case PLAYER_TAILS:
        sound = 0x2005;
        wait = CALL_SOUND_WAIT;
        break;
    }
    player_sound_after(sound, wait);
}

void player_pull_heavy(struct task_work *work, struct player *record)
{
    record->mode_steps = 0;
    work->mode = PLAYER_MODE_PULL_HEAVY;
    record->anim.next = ANIM_PULL_START;
    if (!(record->spd.x < record->consts.trot_speed)) {
        record->spd.x = record->consts.trot_speed;
    }
}

void player_pet_chao(struct task_work *work, struct player *record)
{
    record->mode_steps = 0;
    work->mode = PLAYER_MODE_PET_CHAO;
    record->anim.next = ANIM_PET_CHAO;
    chaonade_start(record->slot);
}

/* a look task's record, which the task's motion points to. the head turns about
 * x and z, and each eye about y and z. each turn moves toward its aim by up to
 * a step of it, within the limits. a node's angle is then the angle it had as
 * the look started, plus its turn */
struct look {
    Sint8 mode; /* a LOOK_ mode */
    Sint8 slot;
    Uint8 unk_02[2];
    Angle head_aim_x, head_aim_z;
    Angle head_x, head_z;
    Angle eyes_aim_y, eyes_aim_z;
    Angle eye0_y, eye0_z;
    Angle eye1_y, eye1_z;
    struct look_limits limits;
    NJS_CNK_OBJECT *head;
    NJS_CNK_OBJECT *eye0;
    NJS_CNK_OBJECT *eye1;
    /* the nodes' own angles, which the look's end puts back */
    Angle head_ang[3];
    Angle eye0_ang[3];
    Angle eye1_ang[3];
};

/* a look's modes. look_step takes LOOK_END once the player's task is gone. it
 * takes LOOK_EVENT while the record's look_mode isn't PLAYER_LOOK_NONE, and
 * LOOK_AHEAD otherwise. it never takes LOOK_CAMERA, which looks at the camera
 */
#define LOOK_AHEAD 1
#define LOOK_CAMERA 2
#define LOOK_EVENT 3
#define LOOK_END 4

/* how far the head and the eyes turn toward their aims in a step */
#define HEAD_STEP 0x1000
#define EYES_STEP 0x800
/* the bits of an Angle above its low 16. setting them makes an angle past
 * HALF_TURN the same angle below 0 */
#define ANGLE_SIGN_BITS 0xffff0000

static void look_turn_head(struct look *look)
{
    Angle ang;

    ang = angle_approach(look->head_x, look->head_aim_x, HEAD_STEP);
    if (ang > HALF_TURN) {
        ang |= ANGLE_SIGN_BITS;
    }
    if (ang > look->limits.head_max_x) {
        ang = look->limits.head_max_x;
    } else if (ang < look->limits.head_min_x) {
        ang = look->limits.head_min_x;
    }
    look->head_x = ang;
    /* @hack type: the sums cut to Uint16, where plain sums would do */
    look->head->ang[0] = (Uint16)(look->head_x + look->head_ang[0]);
    ang = angle_approach(look->head_z, look->head_aim_z, HEAD_STEP);
    if (ang > HALF_TURN) {
        ang |= ANGLE_SIGN_BITS;
    }
    if (ang > look->limits.head_max_z) {
        ang = look->limits.head_max_z;
    } else if (ang < look->limits.head_min_z) {
        ang = look->limits.head_min_z;
    }
    look->head_z = ang;
    look->head->ang[2] = (Uint16)(look->head_z + look->head_ang[2]);
}

static void look_turn_eyes(struct look *look)
{
    Angle ang;

    ang = angle_approach(look->eye0_y, look->eyes_aim_y, EYES_STEP);
    if (ang & HALF_TURN) {
        ang |= ANGLE_SIGN_BITS;
    }
    if (ang > look->limits.eye0_max_y) {
        ang = look->limits.eye0_max_y;
    } else if (ang < look->limits.eye0_min_y) {
        ang = look->limits.eye0_min_y;
    }
    look->eye0_y = ang;
    look->eye0->ang[1] = look->eye0_y + look->eye0_ang[1];
    ang = angle_approach(look->eye1_y, look->eyes_aim_y, EYES_STEP);
    if (ang & HALF_TURN) {
        ang |= ANGLE_SIGN_BITS;
    }
    if (ang < look->limits.eye1_min_y) {
        ang = look->limits.eye1_min_y;
    } else if (ang > look->limits.eye1_max_y) {
        ang = look->limits.eye1_max_y;
    }
    look->eye1_y = ang;
    look->eye1->ang[1] = look->eye1_y + look->eye1_ang[1];
    ang = angle_approach(look->eye0_z, look->eyes_aim_z, EYES_STEP);
    if (ang & HALF_TURN) {
        ang |= ANGLE_SIGN_BITS;
    }
    if (ang < look->limits.eyes_min_z) {
        ang = look->limits.eyes_min_z;
    } else if (ang > look->limits.eyes_max_z) {
        ang = look->limits.eyes_max_z;
    }
    look->eye0_z = ang;
    look->eye0->ang[2] = look->eye0_z + look->eye0_ang[2];
    look->eye1_z = ang;
    look->eye1->ang[2] = look->eye1_z + look->eye1_ang[2];
}

Bool player_look_start(int slot, NJS_CNK_OBJECT **nodes)
{
    struct task *task;
    struct look *look;
    Angle *limit;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create_child(0, look_step, player_tasks[slot])) != NULL) {
        if ((look = syCalloc(1, sizeof(struct look))) == NULL) {
            task_kill(task);
        } else {
            task->motion.p = look;
            look->slot = slot;
            look->head = nodes[0];
            look->eye0 = nodes[1];
            look->eye1 = nodes[2];
            look->mode = LOOK_AHEAD;
            look->head_ang[0] = look->head->ang[0];
            look->head_ang[1] = look->head->ang[1];
            look->head_ang[2] = look->head->ang[2];
            look->eye0_ang[0] = look->eye0->ang[0];
            look->eye0_ang[1] = look->eye0->ang[1];
            look->eye0_ang[2] = look->eye0->ang[2];
            look->eye1_ang[0] = look->eye1->ang[0];
            look->eye1_ang[1] = look->eye1->ang[1];
            look->eye1_ang[2] = look->eye1->ang[2];
            /* @hack spelling: the limits copied an angle at a time through
             * limit, where a struct assignment would do */
            limit = &look_limits[player_records[slot]->character].head_max_x;
            look->limits.head_max_x = *limit++;
            look->limits.head_min_x = *limit++;
            look->limits.head_max_z = *limit++;
            look->limits.head_min_z = *limit++;
            look->limits.eye0_max_y = *limit++;
            look->limits.eye0_min_y = *limit++;
            look->limits.eye0_max_z = *limit++;
            look->limits.eye0_min_z = *limit++;
            look->limits.eye1_max_y = *limit++;
            look->limits.eye1_min_y = *limit++;
            look->limits.eyes_max_z = *limit++;
            look->limits.eyes_min_z = *limit;
            look->head->evalflags &= ~NJD_EVAL_UNIT_ANG;
            look->eye0->evalflags &= ~NJD_EVAL_UNIT_ANG;
            look->eye1->evalflags &= ~NJD_EVAL_UNIT_ANG;
            task->destructor = look_destroy;
            /* @hack spelling: a jump to the TRUE return, where the return
             * itself would do */
            goto started;
        }
    }
    return FALSE;
started:
    return TRUE;
}

static void look_destroy(struct task *task)
{
    struct look *look = task->motion.p;

    look->head->ang[0] = look->head_ang[0];
    look->head->ang[1] = look->head_ang[1];
    look->head->ang[2] = look->head_ang[2];
    look->eye1->ang[0] = look->eye1_ang[0];
    look->eye1->ang[1] = look->eye1_ang[1];
    look->eye1->ang[2] = look->eye1_ang[2];
    look->eye0->ang[0] = look->eye0_ang[0];
    look->eye0->ang[1] = look->eye0_ang[1];
    look->eye0->ang[2] = look->eye0_ang[2];
}

/* LOOK_AHEAD turns the head toward the player's aim. LOOK_EVENT turns the head
 * and the eyes as the record's look_mode says. they turn by the angles in
 * look_ang, toward look_pos, or toward look_pos in look_task's axes. it lasts
 * look_steps steps, or while look_steps is below 0 until look_mode changes */
static void look_step(struct task *task)
{
    struct look *look = task->motion.p;
    struct task_work *work = player_work[look->slot];
    struct player_motion *motion = player_motion[look->slot];
    struct player *record = player_records[look->slot];
    struct task_work *target;
    struct collision_shape *shape;
    Angle ang[3];
    NJS_POINT3 eyes;
    NJS_VECTOR ahead;

    if (task->parent == NULL) {
        look->mode = LOOK_END;
    } else if (record->look_mode) {
        look->mode = LOOK_EVENT;
    } else {
        look->mode = LOOK_AHEAD;
        record->look_mode = PLAYER_LOOK_NONE;
    }
    switch (look->mode) {
    case LOOK_AHEAD:
        look->head_aim_x = -angle_difference(work->ang.y, motion->aim_y);
        look->head_aim_z = 0;
        look_turn_head(look);
        look->eyes_aim_y = 0;
        look->eyes_aim_z = 0;
        look_turn_eyes(look);
        break;
    case LOOK_CAMERA:
        shape = work->collision->shapes;
        ahead.y = ahead.z = 0.0F;
        ahead.x = 1.0F;
        turn_to_world(work, &ahead);
        if (camera_views[look->slot]) {
            eyes = camera_views[look->slot]->pos;
        } else {
            eyes = camera_views[0]->pos;
        }
        njSubVector(&eyes, &shape->center);
        vector_get_angles(&eyes, &ahead, ang);
        look->head_aim_x = -ang[1];
        look->head_aim_z = ang[2];
        look_turn_head(look);
        look->eyes_aim_y = ang[1] + look->head_x;
        look->eyes_aim_z = ang[2] - look->head_z;
        look_turn_eyes(look);
        break;
    case LOOK_EVENT:
        if (record->look_steps == 0) {
            look->mode = LOOK_AHEAD;
            record->look_mode = PLAYER_LOOK_NONE;
            break;
        } else if (record->look_mode == PLAYER_LOOK_NONE) {
            look->mode = LOOK_AHEAD;
            break;
        }
        /* @hack spelling: an empty else after the breaks */
        else {
        }
        shape = work->collision->shapes;
        if (record->look_steps >= 0) {
            record->look_steps--;
        }
        switch (record->look_mode) {
        case PLAYER_LOOK_ANGLES:
            ang[1] = record->look_ang.y;
            ang[2] = record->look_ang.z;
            break;
        case PLAYER_LOOK_POINT:
            ahead.y = ahead.z = 0.0F;
            ahead.x = 1.0F;
            turn_to_world(work, &ahead);
            eyes = record->look_pos;
            njSubVector(&eyes, &shape->center);
            vector_get_angles(&eyes, &ahead, ang);
            break;
        case PLAYER_LOOK_TASK:
            /* @bug uninitialized: turns the head and the eyes by ang[1] and
             * ang[2] unset, at 0x8c0500ec and 0x8c0500f4, where look_task
             * is gone, ending or without a work. the break at 0x8c05000e
             * leaves only this switch */
#ifdef NONMATCHING
            /* this side repairs the tag above: where look_task is gone, the
             * head and the eyes take ang[1] and ang[2] as 0, as in the
             * default case. the image's angles are two words of the frame
             * that no store in this step writes, so they hold what the calls
             * before it left there. only this step aims the head and the
             * eyes by them, since look_mode is now PLAYER_LOOK_NONE */
            ang[1] = 0;
            ang[2] = 0;
#endif
            if (record->look_task == NULL ||
                record->look_task->step == task_destroy) {
                record->look_mode = PLAYER_LOOK_NONE;
                break;
            }
            target = record->look_task->work;
            if (target == NULL) {
                record->look_mode = PLAYER_LOOK_NONE;
                break;
            }
            ahead.y = ahead.z = 0.0F;
            ahead.x = 1.0F;
            turn_to_world(work, &ahead);
            njPushMatrix(&_nj_unit_matrix_);
            njRotateZ(NULL, target->ang.z);
            njRotateX(NULL, target->ang.x);
            njRotateY(NULL, target->ang.y);
            njCalcVector(NULL, &record->look_pos, &eyes);
            njPopMatrixEx();
            njAddVector(&eyes, &target->pos);
            njSubVector(&eyes, &shape->center);
            vector_get_angles(&eyes, &ahead, ang);
            break;
        default:
            ang[1] = 0;
            ang[2] = 0;
            break;
        }
        look->head_aim_x = -ang[1];
        look->head_aim_z = ang[2];
        look_turn_head(look);
        look->eyes_aim_y = ang[1] + look->head_x;
        look->eyes_aim_z = ang[2] - look->head_z;
        look_turn_eyes(look);
        break;
    default:
        task_kill(task);
        break;
    }
}

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC aligns NJS_MATRIX to 8, and a record takes that only under pack(8). the
 * matrix comes first, and the record's 88 bytes are a multiple of 8. so no
 * member moves, and the size stays SHC's */
#pragma pack(push, 8)
#endif
#endif
/* an afterimage task's extra: a fading copy of a player */
struct afterimage {
    NJS_MATRIX matrix; /* where it draws */
    Sint8 slot; /* the player's. the copy ends once the slot has no work */
    /* its constant material's alpha, which falls from AFTERIMAGE_ALPHA by
     * AFTERIMAGE_FADE a step. the copy ends at GONE_ALPHA */
    Float alpha;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion; /* null to draw object unposed */
    NJS_TEXLIST *texlist;
    Float frame;
};
#ifdef DREAMCAST
#ifdef NONMATCHING
/* ends the pack(8) the record above takes */
#pragma pack(pop)
#endif
#endif

/* an afterimage's first alpha, and its fall in a step */
#define AFTERIMAGE_ALPHA (-0.2F)
#define AFTERIMAGE_FADE 0.04F
/* a constant material's alpha that leaves nothing of what it draws */
#define GONE_ALPHA (-1.0F)

static void afterimage_draw(struct task *task)
{
    struct afterimage *image = task->extra.p;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    draw_attr_save();
    draw_control_save();
    constant_material_set(image->alpha, 0.0F, 0.0F, 0.0F);
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    draw_control_clear(NJD_CONTROL_3D_CONSTANT_ATTR);
    draw_attr_set(0, NJD_FST_UA);
    draw_attr_clear(0, STRIP_FLAGS_BUT_UA);
    njSetTexture(image->texlist);
    njPushMatrixEx();
    njMultiMatrix(NULL, &image->matrix);
    if (image->motion) {
        njCnkSimpleDrawMotion(image->object, image->motion, image->frame);
    } else {
        njCnkSimpleDrawObject(image->object);
    }
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
}

static void afterimage_step(struct task *task)
{
    struct afterimage *image = task->extra.p;

    if (!player_work[image->slot]) {
        task_destroy(task);
        return;
    }
    image->alpha -= AFTERIMAGE_FADE;
    if (image->alpha <= GONE_ALPHA) {
        task_destroy(task);
    }
}

static void afterimage_create(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              NJS_TEXLIST *texlist, Float frame,
                              NJS_MATRIX *matrix, int slot)
{
    struct task *task;
    struct afterimage *image;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(0, 1, afterimage_step,
                            "ExecutePlayerDrawMotionWithAlphaReducing")) !=
        NULL) {
#ifdef DREAMCAST
#ifdef NONMATCHING
        /* under pack(8), the record takes SHC's 88 bytes */
        _Static_assert(sizeof(struct afterimage) == 88,
                       "struct afterimage changed its size");
#endif
#endif
        if ((image = syMalloc(sizeof(struct afterimage))) == NULL) {
            task_destroy(task);
            return;
        }
        task->extra.p = image;
        image->slot = slot;
        if (matrix == NULL) {
            njGetMatrix(&image->matrix);
        } else {
            njSetMatrix(&image->matrix, matrix);
        }
        image->alpha = AFTERIMAGE_ALPHA;
        image->frame = frame;
        image->object = object;
        image->motion = motion;
        image->texlist = texlist;
        task->draw = afterimage_draw;
    }
}

void player_afterimage(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                       NJS_TEXLIST *texlist, Float frame, NJS_MATRIX *matrix,
                       int slot)
{
    if (game_state.two_players == 0) {
        afterimage_create(object, motion, texlist, frame, matrix, slot);
    }
}

/* a moving player ripples the water where a random pick from 0 up to 1 passes
 * RIPPLE_CHANCE. the ripple starts at RIPPLE_SCALE of the player's radius. it
 * grows by RIPPLE_GROWTH a step, less RIPPLE_SINK of the player's spd.y. it
 * fades once its size passes RIPPLE_FADE_SIZE */
#define RIPPLE_CHANCE 0.85F
#define RIPPLE_SCALE 0.1F
#define RIPPLE_GROWTH 0.03F
#define RIPPLE_SINK 0.005F
#define RIPPLE_FADE_SIZE 0.85F

void player_ripple(int slot, NJS_POINT3 *pos, struct angles *ang)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    NJS_VECTOR growth;
    /* @hack spelling: drift, a local for the negated product, where the sum
     * in growth.x would do */
    Float drift;

    if (player_hooks.ripple == NULL) {
        return;
    }
    if (pos->y > work->pos.y + record->consts.height) {
        return;
    }
    if (njScalor(&record->spd) == 0.0F) {
        return;
    }
    if (!((Float)rand() * RAND_SCALE > RIPPLE_CHANCE)) {
        return;
    }
    drift = -(record->spd.y * RIPPLE_SINK);
    growth.x = drift + RIPPLE_GROWTH;
    growth.y = 0.0F;
    growth.z = RIPPLE_FADE_SIZE;
    player_hooks.ripple(pos, &growth, record->consts.radius * RIPPLE_SCALE);
}

/* a player splashes only faster than SPLASH_SLOWEST. within SPLASH_HEIGHT of
 * the ground below it, the splash's size is 0. the splash's speed is
 * SPLASH_SPEED of the motion's */
#define SPLASH_SLOWEST 1.0F
#define SPLASH_HEIGHT 8.0F
#define SPLASH_SPEED 0.5F
#define SOUND_SPLASH 0x7001

void player_splash(int slot, NJS_POINT3 *pos, struct angles *ang)
{
    Float size;
    Float speed;

    if (player_hooks.splash == NULL) {
        return;
    }
    if (!(njScalor(&player_records[slot]->spd) > SPLASH_SLOWEST)) {
        return;
    }
    if (_builtin_fabsf(player_work[slot]->pos.y -
                       player_records[slot]->ground.below_y) > SPLASH_HEIGHT) {
#ifdef NONMATCHING
        /* GCC can't see that every path sets size. a record's character is one
         * of the eight the switch has cases for */
        size = 1.0F;
#endif
        switch (player_records[slot]->character) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            size = 1.0F;
            break;
        case PLAYER_TAILS:
            size = 0.8F;
            break;
        case PLAYER_EGGMAN:
            size = 2.0F;
            break;
        case PLAYER_EGGMAN_MECH:
        case PLAYER_TAILS_MECH:
            size = 3.0F;
            break;
        }
    } else {
        size = 0.0F;
    }
    speed = njScalor(&player_motion[slot]->spd) * SPLASH_SPEED;
    player_hooks.splash(pos, ang, size, speed);
    sound_play_at(SOUND_SPLASH, NULL, 0, 0, pos);
}

/* a waiting task's extra: a sound or a voice to play once steps runs out */
struct waiting {
    Sint32 id;
    Sint32 steps; /* the steps left to wait */
};

static void sound_wait_step(struct task *task)
{
    struct waiting *waiting = task->extra.p;

    if (--waiting->steps == 0) {
        sound_play(waiting->id, NULL, 0, 0);
        task_destroy(task);
    }
}

void player_sound_after(int id, int delay)
{
    struct task *task =
        task_create(TASK_EXTRA, 3, sound_wait_step, "PSubCallSEWithWait");

    if (task) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct waiting *)task->extra.p)->id = id;
        ((struct waiting *)task->extra.p)->steps = delay;
    }
}

/* a player_sound_in_mode task's extra: the sound, and the slot and mode it
 * plays for */
struct sound_in_mode {
    Sint32 id;
    Sint32 slot;
    Sint32 mode;
};

/* player_sound_in_mode plays its sound at IN_MODE_PRIORITY. each step plays it
 * again for IN_MODE_STEPS, as sound_play_at_for counts a duration */
#define IN_MODE_PRIORITY 1
#define IN_MODE_STEPS 2

static void sound_in_mode_step(struct task *task)
{
    struct sound_in_mode *sound = task->extra.p;
    /* @hack type: kept, a second pointer to the extra through its first
     * field, where sound would do */
    struct sound_in_mode *kept = (struct sound_in_mode *)&sound->id;
    /* @hack redundant: slot declared register, where a plain local would do */
    register int slot = kept->slot;
    struct task_work *work = player_work[slot];

    if (work && work->mode == kept->mode) {
        sound_play_at_for(sound->id, task, IN_MODE_PRIORITY, 0, IN_MODE_STEPS,
                          &work->pos);
    } else {
        task_destroy(task);
    }
}

void player_sound_in_mode(int id, int slot, Sint16 mode)
{
    struct task *task =
        task_create(TASK_EXTRA, 3, sound_in_mode_step, "LoopSECallForTheMode");

    if (task) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct sound_in_mode *)task->extra.p)->id = id;
        ((struct sound_in_mode *)task->extra.p)->slot = slot;
        ((struct sound_in_mode *)task->extra.p)->mode = mode;
    }
}

static void voice_wait_step(struct task *task)
{
    struct waiting *waiting = task->extra.p;

    if (--waiting->steps == 0) {
        voice_play_priority_2(waiting->id);
        task_destroy(task);
    }
}

void player_voice_after(int id, int delay)
{
    struct task *task =
        task_create(TASK_EXTRA, 3, voice_wait_step, "PSubSerif_PlayWithWait");

    if (task) {
        /* @hack spelling: the extra cast at each use, where a local of its
         * type would do */
        ((struct waiting *)task->extra.p)->id = id;
        ((struct waiting *)task->extra.p)->steps = delay;
    }
}

/* the petting's work timer counts its steps from 0. the player says its line
 * at the step that finds the timer at PET_LINE_TIMER, the 31st */
#define PET_LINE_TIMER 30

/* the petting's shape follows the player's unk_1e8 while the player stays in
 * PLAYER_MODE_PET_CHAO. the work's submode holds the player's slot */
static void chaonade_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player = player_work[work->submode];
    struct player *record = player_records[work->submode];
    int voice;

    if (!player || player->mode != PLAYER_MODE_PET_CHAO) {
        task_kill(task);
        return;
    }
    /* @hack spelling: the record read again through player_records, where
     * record would do */
    work->pos = player_records[work->submode]->unk_1e8;
    collision_register(task);
    if (work->timer++ == PET_LINE_TIMER) {
        voice = -1;
        switch (record->kind) {
        case PLAYER_SONIC:
            voice = 1517;
            break;
        case PLAYER_SHADOW:
            voice = 1516;
            break;
        case PLAYER_TAILS:
            voice = 1729;
            break;
        case PLAYER_EGGMAN:
            voice = 1726;
            break;
        case PLAYER_KNUCKLES:
            voice = 1242;
            break;
        case PLAYER_ROUGE:
            voice = 1243;
            break;
        }
        if (voice != -1) {
            voice_play_priority_2(voice);
        }
    }
}

static void chaonade_start(int slot)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, 2, chaonade_step, "ChaonadeExec")) !=
        NULL) {
        collision_init(task, chaonade_shape, 1, COLLISION_PICKUP);
        task->work->submode = slot;
    }
}

/* a player_pose task's extra: the slot and the animation of the pose */
struct pose {
    Sint8 slot;
    Sint32 anim;
};

static void pose_step(struct task *task)
{
    struct pose *pose = task->extra.p;
    int slot = pose->slot;
    int anim = pose->anim;
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];

    if (work->mode != PLAYER_MODE_EVENT ||
        (record->anim.current != anim && record->anim.last == anim) ||
        (record->anim.last == anim &&
         (record->anim.flags & PLAYER_ANIM_DONE))) {
        task_destroy(task);
        game_state.solo_player = 0;
        game_state.single_view = 0;
        player_request(slot, PLAYER_REQUEST_RELEASE, 0);
        game_state.hidden_cameras &= ~HIDDEN_CAMERA_0;
    }
}

/* the speed of every camera motion of player_pose */
#define POSE_CAMERA_SPEED 0.96F

void player_pose(int slot, int anim)
{
    struct task_work *work = player_work[slot];
    struct player *record = player_records[slot];
    struct task *task;
    struct camera_motion camera;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, 0, pose_step, "PoseEffectMan")) ==
        NULL) {
        return;
    }
    /* @hack spelling: the record read again through player_records, where
     * record would do */
    player_anim_end_blend(&player_records[slot]->anim);
    record->anim.next = anim;
    work->mode = PLAYER_MODE_EVENT;
    /* @hack spelling: the extra cast at each use, where a local of its type
     * would do */
    ((struct pose *)task->extra.p)->slot = slot;
    ((struct pose *)task->extra.p)->anim = anim;
    if (slot == 0) {
        game_state.solo_player = 1;
    } else {
        game_state.solo_player = 2;
    }
    game_state.single_view = 1;
    game_state.hidden_cameras = HIDDEN_CAMERA_0;
    /* @hack spelling: a loop of one pass from the camera's setup on, where
     * the code without the loop and an if for the motion would do */
    do {
        camera.origin = &work->pos;
        camera.ang = HALF_TURN - work->ang.y;
        camera.motion = NULL;
        camera.flags = CAMERA_MOTION_FOLLOWS | CAMERA_MOTION_TURNS;
        camera.done = NULL;
        switch (record->character) {
        case PLAYER_SONIC:
            if (record->kind == PLAYER_KIND_AMY) {
                camera.motion = &amy_pose_camera;
            } else {
                camera.motion = &sonic_pose_camera;
            }
            camera.speed = POSE_CAMERA_SPEED;
            break;
        case PLAYER_SHADOW:
            camera.motion = &shadow_pose_camera;
            camera.speed = POSE_CAMERA_SPEED;
            break;
        case PLAYER_ROUGE:
            camera.motion = &rouge_pose_camera;
            camera.speed = POSE_CAMERA_SPEED;
            break;
        case PLAYER_TAILS_MECH:
            camera.motion = &tails_mech_pose_camera;
            camera.speed = POSE_CAMERA_SPEED;
            break;
        case PLAYER_EGGMAN_MECH:
            camera.motion = &eggman_mech_pose_camera;
            camera.speed = POSE_CAMERA_SPEED;
            break;
        case PLAYER_KNUCKLES:
            if (record->kind == PLAYER_KIND_TICAL) {
                camera.motion = &tikal_pose_camera;
            } else {
                camera.motion = &knuckles_pose_camera;
            }
            camera.speed = POSE_CAMERA_SPEED;
            break;
        }
        /* @hack redundant: a store that never runs */
        if (0) {
            camera.motion = NULL;
        }
        if (!camera.motion) {
            break;
        }
        camera_stop_shake(0);
        if (camera_top(0) > CAMERA_MOTION_SLOT) {
            camera_remove_mode(0, camera_top(0));
        }
        camera_motion_start(0, &camera);
    } while (0);
    /* @hack redundant: a store that never runs */
    if (0) {
        camera.ang = HALF_TURN;
    }
}

/* start the camera motion of the pose of record's kind. a frozen pose holds the
 * camera at the motion's first frame. otherwise the player says the pose's line
 * and plays its animation too */
static void start_pose_show(struct task_work *work, struct player *record,
                            Bool frozen)
{
    struct camera_motion camera;
    int kind = record->kind;
    int voice;

    camera.origin = &work->pos;
    camera.ang = HALF_TURN - work->ang.y;
    camera.motion = NULL;
    camera.flags =
        CAMERA_MOTION_HOLD | CAMERA_MOTION_FOLLOWS | CAMERA_MOTION_TURNS;
    camera.done = NULL;
    camera.motion = start_poses[kind].motion;
    if (frozen == TRUE) {
        camera.speed = 0.0F;
    } else {
        camera.speed = start_poses[kind].speed;
    }
    if (camera.motion) {
        if (camera_top(0) > CAMERA_MOTION_SLOT) {
            camera_remove_mode(0, camera_top(0));
        }
        camera_motion_start(0, &camera);
        if (frozen == FALSE) {
            if (game_state.round_wins[0] == 0 &&
                game_state.round_wins[1] == 0) {
                voice = start_poses[kind].voice;
            } else if (record->character == game_state.round_winner) {
                voice = start_poses[kind].won_voice;
            } else {
                voice = start_poses[kind].lost_voice;
            }
            if (voice > 0) {
                voice_play_priority_2(voice);
            }
            record->anim.next = start_poses[kind].anim;
        }
    }
}

/* a start pose ends at the step its count, from 0, passes POSE_COUNT_LIMIT:
 * its 122nd. Amy's count starts at -AMY_EXTRA_STEPS unless she won the last
 * round */
#define POSE_COUNT_LIMIT 120
#define AMY_EXTRA_STEPS 60

/* a player_pose_two_start task's extra */
struct start_turn {
    /* the slot of the player who poses, or -1 before the first pose */
    Sint8 slot;
    Sint16 steps; /* that pose's count, one a step */
};

static void start_pose_step(struct task *task)
{
    struct start_turn *turn = task->extra.p;
    struct task_work *work;
    struct player *record;

    player_work[0]->mode = PLAYER_MODE_EVENT;
    player_work[1]->mode = PLAYER_MODE_EVENT;
    if (stage_get_mode() != STAGE_PLAY) {
        return;
    }
    switch (turn->slot) {
    case -1:
        work = player_work[0];
        record = player_records[0];
        turn->slot = 0;
        start_pose_show(work, record, FALSE);
        turn->steps = 0;
        if (record->kind == PLAYER_KIND_AMY) {
            if ((game_state.round_wins[0] == 0 &&
                 game_state.round_wins[1] == 0) ||
                record->character != game_state.round_winner) {
                turn->steps = -AMY_EXTRA_STEPS;
            }
        }
        break;
    case 0:
        if (turn->steps++ > POSE_COUNT_LIMIT) {
            game_state.solo_player = 2;
            work = player_work[1];
            record = player_records[1];
            turn->slot = 1;
            start_pose_show(work, record, FALSE);
            turn->steps = 0;
            if (record->kind == PLAYER_KIND_AMY) {
                if ((game_state.round_wins[0] == 0 &&
                     game_state.round_wins[1] == 0) ||
                    record->character != game_state.round_winner) {
                    turn->steps = -AMY_EXTRA_STEPS;
                }
            }
        }
        break;
    case 1:
        if (turn->steps++ > POSE_COUNT_LIMIT) {
            game_state.solo_player = 0;
            game_state.single_view = 0;
            player_request(0, PLAYER_REQUEST_RELEASE, 0);
            player_request(1, PLAYER_REQUEST_RELEASE, 0);
            stage_place_player(0);
            stage_place_player(1);
            camera_restart();
            task_destroy(task);
        }
        break;
    }
}

void player_pose_two_start(void)
{
    struct task *task;
    struct player *record;

    if (game_state.two_players == 0) {
        return;
    }
    if (stage_current_id() > STAGE_STG_END) {
        return;
    }
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_EXTRA, 0, start_pose_step,
                            "PoseEffect2PStartMan")) == NULL) {
        return;
    }
    player_work[0]->mode = PLAYER_MODE_EVENT;
    player_work[1]->mode = PLAYER_MODE_EVENT;
    player_work[1]->submode = 0;
    player_work[0]->timer = 0;
    player_work[1]->timer = 0;
    player_work[0]->flags = 0;
    player_work[1]->flags = 0;
    record = player_records[0];
    player_anim_end_blend(&record->anim);
    record->anim.next = waiting_anims[record->character];
    record = player_records[1];
    player_anim_end_blend(&record->anim);
    record->anim.next = waiting_anims[record->character];
    stage_place_player_intro(0);
    stage_place_player_intro(1);
    ((struct start_turn *)task->extra.p)->slot = -1;
    game_state.solo_player = 1;
    game_state.single_view = 1;
    start_pose_show(player_work[0], player_records[0], TRUE);
}

/* say an idle's line for player 0: at random, one of its kind's two or its
 * stage's two. idle's low bit picks between the kind's. game_state.steps,
 * scaled by the same random pick, picks between the stage's. it says none in a
 * two-player game, in a mini event or in STAGE_CHAO. so the rows of
 * stage_idle_voices for STAGE_CHAO never play */
/* an idle's line is its kind's where the random pick passes KIND_LINE_CHANCE,
 * and its stage's otherwise */
#define KIND_LINE_CHANCE 0.5F

static void idle_voice(struct task_work *work, struct player *record, int idle)
{
    int kind = record->kind;
    int stage = stage_current_id();
    Sint16 voice = -1;
    Float chance;
    struct kind_voice *by_kind;
    struct stage_voice *by_stage;
    int pick;

    if (record->slot != 0) {
        return;
    }
    /* @hack type: game_state's two_players read as game_state_two_players,
     * where game_state.two_players would do */
    if (game_state_two_players != 0) {
        return;
    }
    if (game_state.mini_event_running != 0) {
        return;
    }
    /* @hack spelling: a switch of one case, where an if would do */
    switch (stage) {
    case STAGE_CHAO:
        return;
    }
    chance = (Float)rand() * RAND_SCALE;
    if (chance > KIND_LINE_CHANCE) {
        by_kind = kind_idle_voices;
        while (by_kind->kind != -1) {
            if (by_kind->kind == kind) {
                if (idle & 1) {
                    voice = by_kind->voices[1];
                } else {
                    voice = by_kind->voices[0];
                }
                break;
            }
            by_kind++;
        }
    } else {
        by_stage = stage_idle_voices;
        while (by_stage->stage != STAGE_NONE) {
            if (by_stage->stage == stage && by_stage->kind == kind) {
                pick = (int)((Float)game_state.steps * chance);
                if (pick & 1) {
                    voice = by_stage->voices[0];
                } else {
                    voice = by_stage->voices[1];
                }
                break;
            }
            by_stage++;
        }
    }
    if (voice != -1) {
        voice_play_priority_2(voice);
    }
}

/* a note task's extra: a note the flute gives off */
struct note {
    Sint8 mode;
    Sint8 kind; /* its bit 0 picks one of the two notes' models */
    Sint16 steps; /* the steps since it began */
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Float scale;
    /* its constant material. alpha falls from 0 to GONE_ALPHA as the note
     * fades. red, green and blue each lie from 0 down to -1 */
    Float alpha;
    Float red, green, blue;
};

/* the steps a note lasts before it fades */
#define NOTE_STEPS 120
/* a note starts at NOTE_SCALE, and grows to 1 by NOTE_GROWTH a step. it fades
 * by NOTE_FADE a step */
#define NOTE_SCALE 0.1F
#define NOTE_GROWTH 0.04F
#define NOTE_FADE 0.05F
/* each step keeps NOTE_KEEP of a note's speed, and turns NOTE_DRAG of the
 * speed against world_gravity */
#define NOTE_KEEP 0.996F
#define NOTE_DRAG 0.003F
/* a note's speed lies from NOTE_SPEED up to NOTE_SPEED_SPREAD more. its lift
 * lies from NOTE_LIFT up to NOTE_LIFT_SPREAD more */
#define NOTE_SPEED 0.05F
#define NOTE_SPEED_SPREAD 0.1F
#define NOTE_LIFT 0.01F
#define NOTE_LIFT_SPREAD 0.05F

static void note_draw(struct task *task)
{
    struct note *note = task->extra.p;
    NJS_CNK_MODEL *model;

    if (note->kind & 1) {
        model = &mystic_note_odd_model;
    } else {
        model = &mystic_note_even_model;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(note->alpha, note->red, note->green, note->blue);
    njPushMatrixEx();
    njTranslateEx(&note->pos);
    njRotateY(NULL, camera_views[camera_current_index]->ang.y);
    njScale(NULL, note->scale, note->scale, note->scale);
    njCnkSimpleDrawModel(model);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* a note grows to its full size and drifts, slowing and rising against
 * world_gravity. after NOTE_STEPS it fades, and it ends once faded */
static void note_step(struct task *task)
{
    struct note *note = task->extra.p;
    Float drag;

    if (note->steps++ > NOTE_STEPS) {
        note->alpha = float_approach(note->alpha, GONE_ALPHA, NOTE_FADE);
    }
    if (note->alpha <= GONE_ALPHA) {
        task_kill(task);
        return;
    }
    note->scale = float_approach(note->scale, 1.0F, NOTE_GROWTH);
    njAddVector(&note->pos, &note->spd);
    drag = njScalor(&note->spd) * NOTE_DRAG;
    note->spd.x = (note->spd.x * NOTE_KEEP) - (world_gravity.x * drag);
    note->spd.y = (note->spd.y * NOTE_KEEP) - (world_gravity.y * drag);
    note->spd.z = (note->spd.z * NOTE_KEEP) - (world_gravity.z * drag);
}

/* start a note at pos, thrown along dir and lifted, each at a random speed.
 * slot is unused */
static void note_create(int slot, NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    struct task *task;
    struct note *note;
    Float speed;
    Float lift;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(0, 4, note_step, "psExecuteNote")) == NULL) {
        return;
    }
    if ((note = syCalloc(1, sizeof(struct note))) == NULL) {
        task_kill(task);
        return;
    }
    task->extra.p = note;
    note->pos = *pos;
    speed = NOTE_SPEED + ((Float)rand() * (NOTE_SPEED_SPREAD * RAND_SCALE));
    lift = NOTE_LIFT + ((Float)rand() * (NOTE_LIFT_SPREAD * RAND_SCALE));
    /* @bug logic: lifts spd.y and spd.z against world_gravity.x, where
     * note_step's drag takes each axis's own. the reads at 0x8c051124 and
     * 0x8c051134 take the x */
    note->spd.x = (dir->x * speed) - (world_gravity.x * lift);
    note->spd.y = (dir->y * speed) - (world_gravity.x * lift);
    note->spd.z = (dir->z * speed) - (world_gravity.x * lift);
    note->scale = NOTE_SCALE;
    note->alpha = 0.0F;
    note->red = -((Float)rand() * RAND_SCALE);
    note->green = -((Float)rand() * RAND_SCALE);
    note->blue = -((Float)rand() * RAND_SCALE);
    note->kind = (Sint8)game_state.steps;
    task->draw = note_draw;
}

/* a flute task's extra: the flute a player plays */
struct flute {
    Sint8 shown; /* 1 while the player plays it */
    Sint8 slot;
    Sint16 wait; /* the steps until the next note */
    /* the flute node's origin and x axis, as the last draw found them */
    NJS_POINT3 pos;
    NJS_VECTOR dir;
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
    Float alpha; /* its fade, from 0 to 1 */
    Uint8 unk_28[4];
};

/* flute_draw's motion callback. at the flute's node, it takes the node's
 * origin and x axis to the world */
static void flute_locate(NJS_CNK_OBJECT *object)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = pos.y = pos.z = 0.0F;
    if (flute_drawing != NULL) {
        if (object == &mystic_flute_object) {
            dir.x = 1.0F;
            dir.y = dir.z = 0.0F;
            njCalcPoint(NULL, &pos, &pos);
            njCalcPoint(&view_to_world, &pos, &flute_drawing->pos);
            njCalcVector(NULL, &dir, &dir);
            njCalcVector(&view_to_world, &dir, &flute_drawing->dir);
        }
    }
}

/* the flute model's vertex list. its first word is the chunk's header, and its
 * second holds the count of vertices in VERTEX_COUNT. each vertex then takes
 * VERTEX_WORDS words, its color last */
#define VERTEX_COUNT 0xffff0000
#define VERTEX_COUNT_SHIFT 16
#define VERTEX_WORDS 4
#define VERTEX_COLOR 3
/* a vertex color's alpha and the rest. a color with FULL_ALPHA in its alpha
 * takes the flute's alpha, and any other half of it */
#define COLOR_ALPHA 0xff000000
#define COLOR_RGB 0x00ffffff
#define FULL_ALPHA 0x01000000

/* draw the flute in the player's hands, at the player's frame. each vertex
 * color with an alpha takes the flute's, or half of it */
static void flute_draw(struct task *task)
{
    struct flute *flute = task->extra.p;
    struct task_work *work = player_work[flute->slot];
    struct player *record = player_records[flute->slot];
    Uint32 alpha;
    Uint32 *vertex;
    Sint32 count;
    Uint32 color;

    if (flute->shown == 1) {
        flute_drawing = flute;
        njCnkSetMotionCallback(flute_locate);
        njGetMatrix(&view_to_world);
        njInvertMatrix(&view_to_world);
        alpha = (Uint32)(flute->alpha * 255.0F);
        if (alpha > 255) {
            alpha = 255;
        }
        alpha <<= 24;
        vertex = (Uint32 *)flute->object->model->vlist + 1;
        count = (Sint32)((*vertex++ & VERTEX_COUNT) >> VERTEX_COUNT_SHIFT);
        for (; count > 0; count--, vertex += VERTEX_WORDS) {
            color = vertex[VERTEX_COLOR];
#ifdef NONMATCHING
            /* All six vertices of this model take the current fade. */
#else
            /* @bug logic: the two half-alpha tip vertices reach zero at
             * frame zero, and the original test (TST at 0x8c051302) skips
             * them on later draws. */
            if (color & COLOR_ALPHA)
#endif
            {
                if (color & FULL_ALPHA) {
                    color = (color & COLOR_RGB) | alpha | FULL_ALPHA;
                } else {
                    color = (color & COLOR_RGB) |
                            ((alpha >> 1) & (COLOR_ALPHA & ~FULL_ALPHA));
                }
                vertex[VERTEX_COLOR] = color;
            }
        }
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, HALF_TURN - work->ang.y);
        njScaleEx(&work->scl);
        njCnkSimpleDrawMotion(flute->object, flute->motion, record->anim.frame);
        njPopMatrixEx();
        flute_drawing = NULL;
        njCnkSetMotionCallback(NULL);
    }
}

/* the frames the flute fades in and out over */
#define FLUTE_FADE 5.0F

/* between its fades, the flute gives off a note every 1 to NOTE_WAIT steps, at
 * random. a note starts NOTE_AHEAD ahead of the flute's node, along its x
 * axis */
#define NOTE_WAIT 18.0F
#define NOTE_AHEAD 2.0F

/* the flute fades in over its motion's first FLUTE_FADE frames and out over the
 * last. it gives off notes between. the flute ends once ANIM_FLUTE is neither
 * the player's animation nor its next */
static void flute_step(struct task *task)
{
    struct flute *flute = task->extra.p;
    /* @hack redundant: work, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-variable) */
    struct task_work *work = player_work[flute->slot];
    struct player *record = player_records[flute->slot];
    Float fade;
    Float frames;
    NJS_POINT3 pos;

    if (record->anim.current == ANIM_FLUTE ||
        record->anim.current == ANIM_FLUTE_AGAIN) {
        flute->shown = 1;
    } else {
        flute->shown = 0;
    }
    /* @hack spelling: a case for each mech and the rest, each with the same
     * fade, where the fade alone would do */
    switch (record->character) {
    case PLAYER_EGGMAN_MECH:
        fade = FLUTE_FADE;
        break;
    case PLAYER_TAILS_MECH:
        fade = FLUTE_FADE;
        break;
    default:
        fade = FLUTE_FADE;
        break;
    }
    frames = (Float)flute->motion->nbFrame;
    if (record->anim.frame < fade) {
        flute->alpha = record->anim.frame / fade;
    } else if (record->anim.frame > frames - fade - 1.0F) {
        flute->alpha = (frames - record->anim.frame - 1.0F) / fade;
    } else {
        flute->alpha = 1.0F;
        if (flute->wait-- <= 0) {
            flute->wait = (Sint16)((Float)rand() * (NOTE_WAIT * RAND_SCALE));
            pos = flute->pos;
            pos.x += flute->dir.x * NOTE_AHEAD;
            pos.y += flute->dir.y * NOTE_AHEAD;
            pos.z += flute->dir.z * NOTE_AHEAD;
            note_create(flute->slot, &pos, &flute->dir);
        }
    }
    /* @bug logic: ends the flute as the player goes on from ANIM_FLUTE, so
     * player_play_flute_again's ANIM_FLUTE_AGAIN plays without it. the test
     * that sets shown takes both animations, at 0x8c05143e and 0x8c051446,
     * but this one takes ANIM_FLUTE alone, at 0x8c051528 and 0x8c051534 */
#ifdef NONMATCHING
    /* this side repairs the tag above: the flute stays through
     * ANIM_FLUTE_AGAIN */
    if (record->anim.current == ANIM_FLUTE_AGAIN ||
        record->anim.next == ANIM_FLUTE_AGAIN) {
        return;
    }
#endif
    if (record->anim.current != ANIM_FLUTE && record->anim.next != ANIM_FLUTE) {
        task_kill(task);
    }
}

static void flute_create(int slot)
{
    struct task *task;
    struct flute *flute;

    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if ((task = task_create(0, 4, flute_step, "psExecuteFlute")) == NULL) {
        return;
    }
    if ((flute = syCalloc(1, sizeof(struct flute))) == NULL) {
        task_kill(task);
        return;
    }
    task->extra.p = flute;
    flute->slot = slot;
    flute->alpha = 0.0F;
    flute->shown = 0;
    flute->object = &mystic_flute_object;
    switch (player_records[slot]->character) {
    case PLAYER_EGGMAN_MECH:
        flute->motion = &eggman_mech_flute_motion;
        break;
    case PLAYER_TAILS_MECH:
        flute->motion = &tails_mech_flute_motion;
        break;
    default:
        flute->motion = &mystic_flute_motion;
        break;
    }
    task->draw = flute_draw;
}
