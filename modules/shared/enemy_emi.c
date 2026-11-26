/* the emi, an enemy that a stage's object list names "E EMI". most kinds
 * walk about their home, and charge at a player who comes near home */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/helpers.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/placement.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_bomb.h"
#include "shared/enemy_emi.h"
#include "shared/enemy_emi_art.h"
#include "shared/enemy_jet.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* the emi's work->mode, which skips 10: no code sets it. work->submode holds
 * its emi_kind */
enum emi_mode {
    MODE_SETUP = 0,
    MODE_WALK = 1, /* a walking kind walks along its heading */
    MODE_TURN = 2, /* a walking kind turns in place toward aim_y */
    MODE_AIM = 3, /* a walking kind turns toward the nearest player */
    MODE_CHARGE = 4, /* a walking kind runs toward its target */
    MODE_BRAKE = 5, /* a walking kind slows to a stop after a charge */
    MODE_FIRE = 6, /* a cannon's only mode */
    MODE_STAND = 7, /* a dool's only mode */
    MODE_RECOIL = 8, /* an armored emi slows to a stop after a knock back */
    MODE_FALL = 9, /* a walking kind falls until it lands */
    /* a hit has beaten the emi, which its next step breaks into pieces and
     * kills */
    MODE_BROKEN = 11
};

/* the indexes of emi_actions. a dool plays ACTION_NORMAL alone. a walking
 * kind plays its _AIM action from its aim to the end of its charge. a cannon
 * plays ACTION_CANNON_SHOT once for each bomb. it never aims, so nothing
 * plays ACTION_CANNON_AIM */
#define ACTION_NORMAL 0
#define ACTION_NORMAL_AIM 1
#define ACTION_CANNON 2
#define ACTION_CANNON_SHOT 3
#define ACTION_CANNON_AIM 4
#define ACTION_ARMORED 5
#define ACTION_ARMORED_AIM 6
#define ACTION_SPIKED 7
#define ACTION_SPIKED_AIM 8

/* a step moves ACTION_CANNON_SHOT's frame by SHOT_ACTION_SPEED. the bomb
 * leaves in the one step that finds the frame from SHOT_FRAME up to
 * SHOT_FRAME_END */
#ifdef NONMATCHING
/* this side takes the speed the shot's frame steps by from the action table.
 * the matching side spells the table's value */
#define SHOT_ACTION_SPEED (emi_actions[ACTION_CANNON_SHOT].speed)
#else
#define SHOT_ACTION_SPEED 0.14F
#endif
#define SHOT_FRAME 3.0F
#define SHOT_FRAME_END (SHOT_FRAME + SHOT_ACTION_SPEED)

/* the emi's own fields of its enemy_work */
/* a cannon's bomb_kind, and its interval in steps. a shot waits for
 * work->timer to pass the interval */
#define BOMB_KIND(enemy) ((enemy)->unk_047)
#define INTERVAL(enemy) ((enemy)->unk_048)
/* what the draw adds to each v of the kind's _scroll_model */
#define SCROLL_V(enemy) ((enemy)->unk_050)

/* the range is scl.z + RANGE. sight_distance2 and near_distance2 both hold
 * the range squared */
#define RANGE 80.0F
/* a target lies level with home, at random in a square about it. the
 * square's side is TARGET_SPREAD times the range */
#define TARGET_SPREAD 0.2F
/* a walk toward home turns in place once ang.y lies more than TURN_SLACK
 * off aim_y. the turn ends once the signed turn left lies below TURN_SLACK */
#define TURN_SLACK 0x800
/* the most ang.y turns in a step, in Angle units. it turns TURN_SPEED in place,
 * AIM_TURN_SPEED toward the nearest player, and CHARGE_TURN_SPEED toward the
 * target in a charge */
#define TURN_SPEED 0xf0
#define AIM_TURN_SPEED 0x300
#define CHARGE_TURN_SPEED 64
/* an aim ends in a charge once work->timer passes AIM_STEPS. a charge ends
 * once it passes CHARGE_STEPS, or once player 0 lies behind the emi */
#define AIM_STEPS 100
#define CHARGE_STEPS 180
/* an emi moves along its heading at spd.x, and each move adds spd.z to
 * spd.x. a walk caps spd.x at WALK_SPEED before its move. each move holds
 * spd.x within TOP_SPEED of 0 */
#define WALK_SPEED 0.3F
#define TOP_SPEED 3.4F
/* spd.z in each mode, where a recoil takes BRAKE_ACCEL too. an aim backs the
 * emi away within the range of home, and stops it beyond */
#define WALK_ACCEL 0.02F
#define AIM_ACCEL (-0.5F)
#define CHARGE_ACCEL 0.1F
#define BRAKE_ACCEL (-0.15F)
/* a brake or a recoil stops the emi once spd.x lies within STOP_SPEED of 0 */
#define STOP_SPEED 0.5F
/* the spd.x an attack knocks an armored emi back at */
#define RECOIL_SPEED (-8.0F)
/* each move takes AIR_DRAG times spd.y, and GRAVITY, off spd.y */
#define AIR_DRAG 0.01F
#define GRAVITY 0.4F
/* a fall ends on a floor tilted no more than STEEPEST from level. an armored
 * emi falls from a steeper one */
#define STEEPEST 0x1000
/* the length of the move that starts a fall */
#define FALL_STEP 2.0F
/* the steps an armored emi's attack stays off after it throws a player.
 * work->tick counts them down */
#define DISARMED_STEPS 5
/* an armored emi throws a player back at THROW_SPEED along the player's own
 * x, and THROW_LIFT up */
#define THROW_SPEED (-2.5F)
#define THROW_LIFT 1.8F
/* a spotted player's target is one of enemy_set_target_near_player's
 * TARGET_OFFSETS offsets, picked at random. that holds where the emi lies
 * outside the upper screen */
#define TARGET_OFFSETS 4
/* each walking kind's two jets leave from (_JET_X, _JET_Y, _JET_Z or
 * -_JET_Z) in the emi's own axes. the normal kind's blow at
 * (NORMAL_JET_SPEED_X, NORMAL_JET_SPEED_Y), and the others' at
 * SMALL_JET_SPEED along x */
#define NORMAL_JET_X (-17.0F)
#define NORMAL_JET_Y 12.0F
#define NORMAL_JET_Z 8.0F
#define NORMAL_JET_SPEED_X (-1.2F)
#define NORMAL_JET_SPEED_Y 0.5F
#define ARMORED_JET_X (-15.0F)
#define ARMORED_JET_Y 7.5F
#define ARMORED_JET_Z 8.8F
#define SPIKED_JET_X (-14.9F)
#define SPIKED_JET_Y 7.1F
#define SPIKED_JET_Z 8.6F
#define SMALL_JET_SPEED (-0.5F)
/* a cannon's muzzle, in the cannon's own axes, which turn by its heading
 * alone */
#define MUZZLE_X 8.7F
#define MUZZLE_Y 25.1F
/* what SCROLL_V loses in a step, for each unit of spd.x */
#define SCROLL_RATE 32.0F

/* the points score_add_enemy adds for a beaten emi: SCORE, or SCORE_KNOCK
 * where player_touch_attacks returns TRUE */
#define SCORE 100
#define SCORE_KNOCK 200
/* a beaten emi's ball, under ENEMY_IN_MAGIC_HAND, grows to BALL_SCALE. it
 * draws the emi's model off its center by BALL_OFFSET_Y in y */
#define BALL_SCALE 0.13F
#define BALL_OFFSET_Y (-10.0F)
#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.2F
/* the radius of the sphere about pos that emi_draw tests against the view */
#define VIEW_RADIUS 30.0F
/* the index in lights of the light the emi draws in */
#define EMI_LIGHT 8
/* the texture tasks' level */
#define TEXTURE_LEVEL 0

#define SOUND_SPOT 0x401d
#define SOUND_CHARGE 0x401e /* from a charge's start to its end */
#define SOUND_SHOT 0x4003
#define SOUND_THROW 0x401f
#define SPOT_VOLUME (-10)
#define CHARGE_VOLUME (-10)
#define SHOT_VOLUME 90
#define THROW_VOLUME 10
/* a throw shakes the player's pad */
#define VIBRATION_POWER 3
#define VIBRATION_FREQ 7

/* EMI_SPIKED takes the first SPIKED_SHAPES of emi_shapes, and EMI_ARMORED
 * the one at ARMORED_SHAPE. every other kind takes the first SHAPES. each
 * kind's first shape is its body. the second sphere is at the front and
 * EMI_SPIKED adds one at the top */
#define SHAPES 2
#define SPIKED_SHAPES 3
#define ARMORED_SHAPE 3

static struct collision_shape emi_shapes[ARMORED_SHAPE + 1] = {
#include "shared/enemy_emi/emi_shapes.inc"
};

/* the models enemy_break breaks a beaten emi into, null-ended. a dool breaks
 * into emi_pieces, as EMI_NORMAL does */
static NJS_CNK_MODEL *emi_pieces[7] = {
#include "shared/enemy_emi/emi_pieces.inc"
};
static NJS_CNK_MODEL *emi_spiked_pieces[7] = {
#include "shared/enemy_emi/emi_spiked_pieces.inc"
};
static NJS_CNK_MODEL *emi_cannon_pieces[8] = {
#include "shared/enemy_emi/emi_cannon_pieces.inc"
};
static NJS_CNK_MODEL *emi_armored_pieces[7] = {
#include "shared/enemy_emi/emi_armored_pieces.inc"
};

static struct enemy_action emi_actions[ACTION_SPIKED_AIM + 1] = {
#include "shared/enemy_emi/emi_actions.inc"
};

/* the tasks that keep emi_texlist and emi_spiked_texlist loaded, or null
 * while none does. each one's destructor releases its list */
static struct task *emi_texture_task = NULL;
static struct task *emi_spiked_texture_task = NULL;

static void emi_texture_step(struct task *task);
static void emi_draw(struct task *task);
static void emi_destroy(struct task *task);

static void emi_texture_destroy(struct task *task)
{
    njReleaseTexture(&emi_texlist);
    emi_texture_task = NULL;
}

static void emi_texture_step(struct task *task)
{
}

static void emi_spiked_texture_destroy(struct task *task)
{
    njReleaseTexture(&emi_spiked_texlist);
    emi_spiked_texture_task = NULL;
}

static void emi_setup(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    struct angles ang;
    Float y;

    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= EMI_KINDS || work->submode < 0) {
        work->submode = EMI_NORMAL;
    }
    if (work->submode == EMI_ARMORED) {
        enemy = enemy_work_alloc(task);
    } else {
        enemy = enemy_work_alloc_grouped(task);
    }
    if (work->submode == EMI_SPIKED) {
        collision_init(task, emi_shapes, SPIKED_SHAPES, COLLISION_ENEMY);
    } else if (work->submode == EMI_ARMORED) {
        collision_init(task, &emi_shapes[ARMORED_SHAPE], 1, COLLISION_ENEMY);
    } else {
        collision_init(task, emi_shapes, SHAPES, COLLISION_ENEMY);
    }
    if (work->submode == EMI_CANNON) {
        BOMB_KIND(enemy) = work->ang.x;
        /* @hack spelling: the test against the last bomb kind, where >=
         * BOMB_KINDS would do */
        if (BOMB_KIND(enemy) > BOMB_KINDS - 1 || BOMB_KIND(enemy) < 0) {
            BOMB_KIND(enemy) = 0;
        }
        INTERVAL(enemy) = work->scl.y;
    }
    if (work->submode == EMI_DOOL) {
        work->mode = MODE_STAND;
    } else if (work->submode == EMI_CANNON) {
        work->mode = MODE_FIRE;
    } else {
        work->mode = MODE_WALK;
    }
    work->timer = 0;
    /* @bug logic: a walking emi never holds at a ledge, since it loses
     * ENEMY_HOLD_AT_LEDGE at its first move. no enemy_meet_stage has set
     * ENEMY_ON_FLOOR before that emi_move, so the move starts a fall. the fall
     * clears the flag (AND at 0x1186 of the object's code). the flag stays
     * only where game_state.time_stopped skips the first emi_move (TST at
     * 0x1072) */
    enemy->flags |= ENEMY_HOLD_AT_LEDGE | ENEMY_TILT | ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    enemy->sight_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = 0.0F;
    enemy->spd.z = 0.0F;
    enemy->probe_offset.x = 0.0F;
    enemy->probe_offset.y = 8.0F;
    enemy->probe_offset.z = 0.0F;
    enemy->top = 6.0F;
    enemy->radius = 18.0F;
    enemy->bottom = -9.0F;
    enemy->friction = 1.0F;
    enemy->floor_bounce = 0.2F;
    if (work->submode == EMI_ARMORED) {
        enemy->step_down = 1.0F;
    }
    enemy_set_home(work, enemy);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((y = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z,
                                 &ang)) == NO_FLOOR) {
        y = work->pos.y;
    }
    work->pos.y = y;
    /* @hack spelling: ang.z cleared ahead of ang.x, not in their order */
    work->ang.z = 0;
    work->ang.x = 0;
    enemy->motion.table = emi_actions;
    if (work->submode == EMI_SPIKED) {
        enemy->motion.object = &emi_spiked_object;
        enemy->motion.next = ACTION_SPIKED;
    } else if (work->submode == EMI_CANNON) {
        enemy->motion.object = &emi_cannon_object;
        enemy->motion.next = ACTION_CANNON;
        enemy_bomb_load();
    } else if (work->submode == EMI_ARMORED) {
        enemy->motion.object = &emi_armored_object;
        enemy->motion.next = ACTION_ARMORED;
    } else {
        enemy->motion.object = &emi_object;
        enemy->motion.next = ACTION_NORMAL;
    }
    enemy_motion_step(&enemy->motion);
    enemy->flags |= ENEMY_BURST;
    if (work->submode != EMI_CANNON && work->submode != EMI_DOOL) {
        enemy_jet_load_texture();
    }
    if (work->submode == EMI_SPIKED) {
        if (emi_spiked_texture_task == NULL) {
            texture_load_pvm("E_T_EMITEX", &emi_spiked_texlist);
            emi_spiked_texture_task =
                task_create(0, TEXTURE_LEVEL, emi_texture_step, "ManTex");
            emi_spiked_texture_task->destructor = emi_spiked_texture_destroy;
        }
    } else {
        if (emi_texture_task == NULL) {
            texture_load_pvm("E_EMITEX", &emi_texlist);
            emi_texture_task =
                task_create(0, TEXTURE_LEVEL, emi_texture_step, "ManTex");
            emi_texture_task->destructor = emi_texture_destroy;
        }
    }
    task->draw = emi_draw;
    task->destructor = emi_destroy;
}

static void emi_pick_target(struct task_work *work, struct enemy_work *enemy)
{
    Float range = njSqrt(enemy->near_distance2);

    enemy->target.x =
        enemy->home.x +
        (TARGET_SPREAD * (range * ((rand() * RAND_SCALE) - 0.5F)));
    enemy->target.y = enemy->home.y;
    enemy->target.z =
        enemy->home.z +
        (TARGET_SPREAD * (range * ((rand() * RAND_SCALE) - 0.5F)));
    enemy->aim_y = (Angle)(atan2f(enemy->target.x - work->pos.x,
                                  enemy->target.z - work->pos.z) *
                           ANGLE_PER_RADIAN) -
                   QUARTER_TURN;
}

static void emi_walk(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack redundant: players, which only the test below reads */
    struct task_work **players = player_work;
    struct collision_work *collision;
    struct collision_work *hit;

    /* @hack redundant: a test of players' first entry, which never runs */
    if (0) {
        if (players[0]) {
        }
    }
    collision = work->collision;
    /* @hack spelling: work->collision read again, where collision->hit would
     * do */
    hit = work->collision->hit;
    enemy->spd.z = WALK_ACCEL;
    if (enemy->spd.x > WALK_SPEED) {
        enemy->spd.x = WALK_SPEED;
    }
    enemy->turn_speed = 0;
    if ((collision->flags & COLLISION_TOUCHED) &&
        hit->kind == COLLISION_OBJECT) {
        emi_pick_target(work, enemy);
        work->mode = MODE_TURN;
        work->timer = 0;
        work->pos.x = enemy->last_pos.x;
        work->pos.z = enemy->last_pos.z;
    }
}

static void emi_turn(struct task_work *work, struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        enemy->spd.x = 0.0F;
        work->ang.y = angle_approach(work->ang.y, enemy->aim_y, TURN_SPEED);
        enemy->turn_speed = TURN_SPEED;
        /* @bug logic: a turn in place ends at once where the short turn from
         * ang.y to aim_y is negative, however long that turn is. the test
         * compares the signed turn with TURN_SLACK, where its size would do
         * (CMP/GE at 0x4da of the object's code) */
        if (angle_difference(work->ang.y, enemy->aim_y) < TURN_SLACK) {
            work->mode = MODE_WALK;
            work->timer = 0;
        }
    }
}

static void emi_break(struct task_work *work, struct enemy_work *enemy)
{
    if (work->submode == EMI_SPIKED) {
        enemy_break(work, emi_spiked_pieces, &emi_spiked_texlist);
    } else if (work->submode == EMI_CANNON) {
        enemy_break(work, emi_cannon_pieces, &emi_texlist);
    } else if (work->submode == EMI_ARMORED) {
        enemy_break(work, emi_armored_pieces, &emi_texlist);
    } else {
        enemy_break(work, emi_pieces, &emi_texlist);
    }
}

static void emi_check_hit(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    int score;
    int player;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_BROKEN;
        enemy->unk_09c = 0.35F;
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
        if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
            NJS_VECTOR offset;
            NJS_VECTOR scl;

            player = enemy->shot_by;
            offset.x = 0.0F;
            offset.y = BALL_OFFSET_Y;
            offset.z = 0.0F;
            /* @hack spelling: one assignment to the three parts, where a
             * statement for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            switch (work->submode) {
            case EMI_NORMAL:
            case EMI_DOOL:
                enemy_ball_create(player, &emi_object, &emi_texlist, &scl,
                                  &offset);
                break;
            case EMI_SPIKED:
                enemy_ball_create(player, &emi_spiked_object,
                                  &emi_spiked_texlist, &scl, &offset);
                break;
            case EMI_CANNON:
                enemy_ball_create(player, &emi_cannon_object, &emi_texlist,
                                  &scl, &offset);
                break;
            case EMI_ARMORED:
                enemy_ball_create(player, &emi_armored_object, &emi_texlist,
                                  &scl, &offset);
                break;
            }
        }
    } else if (work->submode == EMI_ARMORED) {
        if (work->collision->hit) {
            struct collision_work *hit = work->collision->hit;

            player = player_slot_of(hit->owner);
            if (player != -1) {
                struct player *record = player_records[player];
                struct task_work *player_task_work = player_work[player];

                if (((record->character == PLAYER_SONIC ||
                      record->character == PLAYER_SHADOW) &&
                     player_task_work->mode >= PLAYER_MODE_SONIC_COMBO_1 &&
                     player_task_work->mode <=
                         PLAYER_MODE_SONIC_COMBO_RUNNING_3) ||
                    (record->character == PLAYER_KNUCKLES &&
                     player_task_work->mode >= PLAYER_MODE_HUNTER_ATTACK_1 &&
                     player_task_work->mode <=
                         PLAYER_MODE_HUNTER_RUN_ATTACK_3)) {
                    enemy->spd.x = RECOIL_SPEED;
                    work->mode = MODE_RECOIL;
                    /* @bug logic: a recoil turns the emi to a heading from 0
                     * to 0x8000, not toward the player. ang.y takes
                     * enemy_aim_at_player's result, which is how far ang.y
                     * lay from the heading toward the player. the call leaves
                     * that heading in aim_y (MOV.L at 0x77a of the object's
                     * code) */
                    work->ang.y = enemy_aim_at_player(work, enemy, player);
                    return;
                }
                if (!(player_task_work->flags &
                      (PLAYER_WORK_ON_GROUND | PLAYER_WORK_ON_SHAPE)) &&
                    (player == 0 || player == 1)) {
                    NJS_VECTOR spd;
                    struct angles ang;

                    spd.x = THROW_SPEED;
                    spd.y = THROW_LIFT;
                    spd.z = 0.0F;
                    ang.x = 0;
                    ang.y = work->ang.y + (HALF_TURN + QUARTER_TURN);
                    ang.z = 0;
                    player_launch_speed(player, &spd, &ang, 0);
                    work->tick = DISARMED_STEPS;
                    sound_play_at(SOUND_THROW, NULL, 0, THROW_VOLUME,
                                  &work->pos);
                    vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                                    VIBRATION_UNTIMED);
                }
            }
        }
        /* @hack redundant: a second test of the kind, which the else if has
         * just made */
        if (work->submode == EMI_ARMORED) {
            struct collision_shape *shape = work->collision->shapes;

            if (work->tick > 0) {
                work->tick--;
                /* @hack spelling: the mask written out, where &= would do */
                shape->levels = shape->levels & ~SHAPE_ATTACK;
            } else {
                shape->levels =
                    (shape->levels & ~SHAPE_ATTACK) | SHAPE_ATTACK_LEVEL(1);
            }
        }
    }
}

static void emi_watch(struct task_work *work, struct enemy_work *enemy)
{
    Float distance = enemy_home_distance2(work, enemy);
    int player;

    if ((enemy->flags & ENEMY_LEDGE_HOLD_OFF) ||
        distance > enemy->near_distance2) {
        if (work->submode != EMI_ARMORED ||
            enemy->floor_normal.y > njCos(STEEPEST)) {
            enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
        }
        enemy_turn_to_home(work, enemy);
        if (angle_distance(enemy->aim_y, work->ang.y) > TURN_SLACK) {
            work->mode = MODE_TURN;
        }
        return;
    }
    player = player_nearest(&work->pos);
    if (player == 0 || player == 1) {
        struct task_work *player_task_work = player_work[player];
        Float height = work->pos.y - player_task_work->pos.y;

        if (enemy->sight_distance2 > height * height) {
            if (enemy_find_player_near_home(work, enemy)) {
                work->timer = 0;
                work->mode = MODE_AIM;
                if (work->submode == EMI_SPIKED) {
                    enemy->motion.next = ACTION_SPIKED_AIM;
                } else if (work->submode == EMI_ARMORED) {
                    enemy->motion.next = ACTION_ARMORED_AIM;
                } else if (work->submode == EMI_CANNON) {
                    enemy->motion.next = ACTION_CANNON_AIM;
                } else {
                    enemy->motion.next = ACTION_NORMAL_AIM;
                }
                if (point_in_upper_screen(&work->pos)) {
                    enemy->target = player_task_work->pos;
                } else {
                    enemy_set_target_near_player(
                        work, enemy, player,
                        rand() * (TARGET_OFFSETS * RAND_SCALE));
                }
                sound_play_at(SOUND_SPOT, NULL, 0, SPOT_VOLUME, &work->pos);
            }
        }
    }
}

/* move pos from the emi's own axes, in which x points ahead and y up, into
 * the world */
static void emi_point_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

static void emi_vector_to_world(struct task_work *work, NJS_VECTOR *vec)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vec, vec);
    njPopMatrixEx();
}

/* send a jet from each side of the emi's back as it aims and charges. the
 * normal kind's are large, and blow back and up. the spiked and armored
 * kinds' are small, and blow back */
static void emi_send_jets(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (!game_state.time_stopped) {
        if (work->submode == EMI_NORMAL) {
            pos.x = NORMAL_JET_X;
            pos.y = NORMAL_JET_Y;
            pos.z = -NORMAL_JET_Z;
            emi_point_to_world(work, &pos);
            spd.x = NORMAL_JET_SPEED_X;
            spd.y = NORMAL_JET_SPEED_Y;
            spd.z = 0.0F;
            emi_vector_to_world(work, &spd);
            enemy_jet_create(task, ENEMY_JET_LARGE, &pos, &spd);
            pos.x = NORMAL_JET_X;
            pos.y = NORMAL_JET_Y;
            pos.z = NORMAL_JET_Z;
            emi_point_to_world(work, &pos);
            enemy_jet_create(task, ENEMY_JET_LARGE, &pos, &spd);
        } else if (work->submode == EMI_ARMORED) {
            pos.x = ARMORED_JET_X;
            pos.y = ARMORED_JET_Y;
            pos.z = -ARMORED_JET_Z;
            emi_point_to_world(work, &pos);
            spd.x = SMALL_JET_SPEED;
            spd.y = 0.0F;
            spd.z = 0.0F;
            emi_vector_to_world(work, &spd);
            enemy_jet_create(task, ENEMY_JET_SMALL, &pos, &spd);
            pos.x = ARMORED_JET_X;
            pos.y = ARMORED_JET_Y;
            pos.z = ARMORED_JET_Z;
            emi_point_to_world(work, &pos);
            enemy_jet_create(task, ENEMY_JET_SMALL, &pos, &spd);
        } else if (work->submode == EMI_SPIKED) {
            pos.x = SPIKED_JET_X;
            pos.y = SPIKED_JET_Y;
            pos.z = -SPIKED_JET_Z;
            emi_point_to_world(work, &pos);
            spd.x = SMALL_JET_SPEED;
            spd.y = 0.0F;
            spd.z = 0.0F;
            emi_vector_to_world(work, &spd);
            enemy_jet_create(task, ENEMY_JET_SMALL, &pos, &spd);
            pos.x = SPIKED_JET_X;
            pos.y = SPIKED_JET_Y;
            pos.z = SPIKED_JET_Z;
            emi_point_to_world(work, &pos);
            enemy_jet_create(task, ENEMY_JET_SMALL, &pos, &spd);
        }
    }
}

static void emi_aim(struct task *task, struct task_work *work,
                    struct enemy_work *enemy)
{
    Float distance;
    int player;

    if (!game_state.time_stopped) {
        distance = enemy_home_distance2(work, enemy);
        if (distance > enemy->near_distance2) {
            /* @hack spelling: spd.z cleared ahead of spd.x, not in their
             * order */
            enemy->spd.z = 0.0F;
            enemy->spd.x = 0.0F;
        } else {
            enemy->spd.z = AIM_ACCEL;
        }
        enemy->turn_speed = AIM_TURN_SPEED;
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
        if (work->timer > AIM_STEPS) {
            work->timer = 0;
            work->mode = MODE_CHARGE;
            sound_play_task_held_at(SOUND_CHARGE, task, 0, CHARGE_VOLUME);
        }
    }
}

/* the step that asks for ACTION_CANNON_SHOT returns early, and checks no
 * hit */
static void emi_fire(struct task *task, struct task_work *work,
                     struct enemy_work *enemy)
{
    NJS_POINT3 muzzle;
    int player;

    enemy->turn_speed = AIM_TURN_SPEED;
    player = player_nearest(&work->pos);
    if (player == 0 || player == 1) {
        enemy_turn_to_player(work, enemy, player);
    }
    if (work->timer > INTERVAL(enemy)) {
        if (enemy->motion.action != ACTION_CANNON_SHOT) {
            enemy->motion.next = ACTION_CANNON_SHOT;
            return;
        }
        if (enemy->motion.frame >= SHOT_FRAME &&
            enemy->motion.frame < SHOT_FRAME_END) {
            muzzle.x = MUZZLE_X;
            muzzle.y = MUZZLE_Y;
            muzzle.z = 0.0F;
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            njCalcPoint(NULL, &muzzle, &muzzle);
            njPopMatrixEx();
            enemy_bomb_create(NULL, BOMB_KIND(enemy), BOMB_THROWN, &muzzle);
            work->timer = 0;
            sound_play_at(SOUND_SHOT, NULL, 0, SHOT_VOLUME, &work->pos);
        }
    }
    emi_check_hit(task, work, enemy);
}

static void emi_charge(struct task_work *work, struct enemy_work *enemy)
{
    struct task_work *player_task_work = player_work[0];
    NJS_VECTOR ahead;
    NJS_VECTOR to_player;
    Float along;

    if (!game_state.time_stopped) {
        enemy->turn_speed = CHARGE_TURN_SPEED;
        enemy_turn_to_target(work, enemy);
        enemy->spd.z = CHARGE_ACCEL;
        ahead.x = njCos(work->ang.y);
        ahead.y = 0.0F;
        ahead.z = -njSin(work->ang.y);
        /* @bug memory: a charge reads player 0's work through null where
         * player_work[0] is null. emi_step tests that pointer (TST at 0x1608
         * of the object's code), and the charge reads its pos.x untested
         * (FMOV.S at 0xe86) */
#ifdef NONMATCHING
        /* this side repairs the tag above: without player 0, the charge
         * takes the emi's own pos for the player's. along is then 0, and the
         * charge brakes once it has run CHARGE_STEPS */
        if (player_task_work == NULL) {
            player_task_work = work;
        }
#endif
        to_player.x = player_task_work->pos.x - work->pos.x;
        to_player.y = 0.0F;
        to_player.z = player_task_work->pos.z - work->pos.z;
        along = njInnerProduct(&ahead, &to_player);
        if (along < 0.0F || work->timer > CHARGE_STEPS) {
            work->mode = MODE_BRAKE;
            work->timer = 0;
            if (work->submode == EMI_SPIKED) {
                enemy->motion.next = ACTION_SPIKED;
            } else if (work->submode == EMI_ARMORED) {
                enemy->motion.next = ACTION_ARMORED;
            } else if (work->submode == EMI_CANNON) {
                enemy->motion.next = ACTION_CANNON;
            } else {
                enemy->motion.next = ACTION_NORMAL;
            }
        }
    }
}

static void emi_brake(struct task_work *work, struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        enemy->turn_speed = 0;
        enemy->spd.z = BRAKE_ACCEL;
        if (enemy->spd.x < STOP_SPEED && enemy->spd.x > -STOP_SPEED) {
            enemy->spd.x = 0.0F;
            enemy->spd.z = 0.0F;
            emi_pick_target(work, enemy);
            work->mode = MODE_TURN;
            work->timer = 0;
        }
    }
}

static void emi_recoil(struct task_work *work, struct enemy_work *enemy)
{
    if (!game_state.time_stopped) {
        enemy->turn_speed = 0;
        enemy->spd.z = BRAKE_ACCEL;
        if (enemy->spd.x < STOP_SPEED && enemy->spd.x > -STOP_SPEED) {
            enemy->spd.x = 0.0F;
            enemy->spd.z = 0.0F;
            if (enemy->flags & ENEMY_LEDGE_HOLD_OFF) {
                enemy->flags &= ~ENEMY_HOLD_AT_LEDGE;
                /* @hack redundant: a return, where the if's end would do */
                return;
            } else {
                work->mode = MODE_WALK;
            }
        }
    }
}

/* an emi off the floor starts a fall in place of the move. so does an
 * armored emi on a floor steeper than STEEPEST */
static void emi_move(struct task_work *work, struct enemy_work *enemy)
{
    Float last = enemy->spd.x;
    NJS_VECTOR push;
    NJS_LINE line;
    NJS_PLANE plane;

    if (game_state.time_stopped) {
        return;
    }
    if ((work->submode == EMI_ARMORED &&
         njCos(STEEPEST) > enemy->floor_normal.y &&
         njScalor(&enemy->floor_normal) != 0.0F &&
         (enemy->flags & ENEMY_ON_FLOOR)) ||
        !(enemy->flags & ENEMY_ON_FLOOR)) {
        push = enemy->floor_normal;
        push.y = 0.0F;
        /* @bug arithmetic: makes NaNs of push off the floor. there
         * floor_normal is (0, 1, 0), or zero before the first
         * enemy_meet_stage, so njUnitVector takes a zero push (JSR at 0x1098
         * of the object's code) */
        njUnitVector(&push);
        /* @hack spelling: line's and plane's points cleared in turn, where
         * each record's three in a row would do */
        line.px = 0.0F;
        plane.px = 0.0F;
        line.py = 0.0F;
        plane.py = 0.0F;
        line.pz = 0.0F;
        plane.pz = 0.0F;
        line.vx = push.x;
        line.vy = push.y;
        line.vz = push.z;
        plane.vx = enemy->floor_normal.x;
        plane.vy = enemy->floor_normal.y;
        plane.vz = enemy->floor_normal.z;
        njDistanceL2PL(&line, &plane, &push);
        /* @bug logic: the start of a fall always moves the emi FALL_STEP
         * down, since this test never passes. line and plane both pass through
         * the origin. so on a floor too steep for EMI_ARMORED, njDistanceL2PL
         * sets push to the origin. off the floor, push holds NaNs or zeros.
         * none of these passes the test (FCMP/GT at 0x1100 of the object's
         * code) */
        if (njScalor(&push) > 0.0F) {
            njUnitVector(&push);
            push.x *= FALL_STEP;
            push.y *= FALL_STEP;
            push.z *= FALL_STEP;
        } else {
            push.x = 0.0F;
            push.y = -FALL_STEP;
            push.z = 0.0F;
        }
        njAddVector(&work->pos, &push);
        work->mode = MODE_FALL;
        enemy->flags &= ~ENEMY_HOLD_AT_LEDGE;
        return;
    }
    if (enemy->spd.x < 0.0F && enemy->spd.z < 0.0F) {
        enemy->spd.z = -enemy->spd.z;
    }
    enemy->spd.x += enemy->spd.z;
    if (enemy->spd.x < 0.0F && last > 0.0F) {
        enemy->spd.x = 0.0F;
    }
    if (enemy->spd.x > TOP_SPEED) {
        enemy->spd.x = TOP_SPEED;
    }
    if (enemy->spd.x < -TOP_SPEED) {
        enemy->spd.x = -TOP_SPEED;
    }
    enemy->spd.y += (-enemy->spd.y * AIR_DRAG) - GRAVITY;
    work->pos.x += enemy->spd.x * njCos(work->ang.y);
    work->pos.y += enemy->spd.y;
    work->pos.z -= enemy->spd.x * njSin(work->ang.y);
}

static void emi_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    int kind;
    const char *label;
    int bomb;
    struct angles ang;
    NJS_VECTOR scl;

    /* @hack spelling: the result compared with 1, where the result alone
     * would do */
    if (sphere_out_of_view(&work->pos, VIEW_RADIUS) == 1) {
        return;
    }
    light_apply(EMI_LIGHT);
    if (work->submode == EMI_SPIKED) {
        njSetTexture(&emi_spiked_texlist);
    } else {
        njSetTexture(&emi_texlist);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    /* @hack redundant: an empty loop that never runs */
    while (0) {
    }
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    if (work->submode == EMI_SPIKED) {
        texture_anim_scroll(&emi_spiked_scroll_model, &emi_spiked_scroll_uv, 0,
                            SCROLL_V(enemy));
    } else if (work->submode == EMI_CANNON) {
        texture_anim_scroll(&emi_cannon_scroll_model, &emi_cannon_scroll_uv, 0,
                            SCROLL_V(enemy));
    } else if (work->submode == EMI_ARMORED) {
        texture_anim_scroll(&emi_armored_scroll_model, &emi_armored_scroll_uv,
                            0, SCROLL_V(enemy));
    } else {
        texture_anim_scroll(&emi_scroll_model, &emi_scroll_uv, 0,
                            SCROLL_V(enemy));
    }
    if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
    } else {
        njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                  enemy->motion.frame);
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njPopMatrixEx();
    /* @hack redundant: a readout of the emi's settings that never runs. the
     * image holds its strings, and its code from the switch to the end,
     * which a branch skips. the lines before the switch leave no code */
    if (1) {
    } else {
        kind = work->submode;
        if (kind >= EMI_KINDS || kind < 0) {
            kind = EMI_NORMAL;
        }
        label = "-> TYPE( 0 - 4)";
        njPrintD(NJM_LOCATION(17, 18), kind, 1);
        switch (kind) {
        case EMI_NORMAL:
        case EMI_SPIKED:
        case EMI_ARMORED:
            njPrintC(NJM_LOCATION(17, 18), label);
            njPrintC(NJM_LOCATION(17, 21), "-> RANGE       ");
            break;
        case EMI_DOOL:
            njPrintC(NJM_LOCATION(17, 18), label);
            break;
        case EMI_CANNON:
            njPrintC(NJM_LOCATION(17, 16), "-> PHASE & BOMB ");
            bomb = (Uint8)work->ang.x;
            if (bomb >= BOMB_KINDS || bomb < 0) {
                bomb = 0;
            }
            njPrintC(NJM_LOCATION(34, 16), emi_bomb_names[bomb]);
            njPrintC(NJM_LOCATION(17, 18), label);
            njPrintC(NJM_LOCATION(17, 20), "-> INTERVAL(FRAME)");
            njPrintC(NJM_LOCATION(17, 21), "-> RANGE       ");
            break;
        }
        njPrintC(NJM_LOCATION(17, 17), "-> ANG Y ");
        njPrintH(NJM_LOCATION(26, 17), work->ang.y & 0xff00, 4);
        if (kind != EMI_ARMORED) {
            njPrintC(NJM_LOCATION(30, 17), " EME ");
            njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
        }
        ang.x = ang.y = ang.z = 0;
        scl.z = scl.x = work->scl.z + RANGE;
        scl.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

static void emi_destroy(struct task *task)
{
    enemy_destroy(task);
    sound_stop_key(task);
    /* @hack redundant: the return after sound_stop_key, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void emi_move_collide(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    emi_move(work, enemy);
    enemy_meet_stage(work, enemy);
    emi_check_hit(task, work, enemy);
    /* @hack redundant: the return after emi_check_hit, where the function's
     * end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void emi_fall(struct task *task, struct task_work *work,
                     struct enemy_work *enemy)
{
    emi_move(work, enemy);
    enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
    enemy_meet_stage(work, enemy);
    emi_check_hit(task, work, enemy);
    if (enemy->floor_normal.y >= njCos(STEEPEST) &&
        njScalor(&enemy->floor_normal) != 0.0F &&
        (enemy->flags & ENEMY_ON_FLOOR)) {
        work->mode = MODE_WALK;
    }
}

/* @hack spelling: a function of its own for the dool's step, which SHC
 * expands in place, where emi_check_hit called in its case would do */
static void emi_stand(struct task *task, struct task_work *work,
                      struct enemy_work *enemy)
{
    emi_check_hit(task, work, enemy);
    /* @hack redundant: the return after the dool's one call, where the
     * function's end would do */
    return; /* NOLINT(readability-redundant-control-flow) */
}

void emi_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    Bool stay = FALSE;

    /* a player within the placement's leave range of home keeps the emi in
     * range. enemy is null until the setup */
    if (enemy != NULL) {
        Float range = njSqrt(task->placement->leave.range);

        if (player_work[0] != NULL) {
            if (range > njDistanceP2P(&enemy->home, &player_work[0]->pos)) {
                stay = TRUE;
            }
        }
        if (player_work[1] != NULL) {
            if (range > njDistanceP2P(&enemy->home, &player_work[1]->pos)) {
                stay = TRUE;
            }
        }
    }
    /* @hack spelling: stay compared with TRUE, where !stay would do */
    if (work->mode != MODE_SETUP && stay != TRUE && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_SETUP:
#ifdef NONMATCHING
        /* this side removes an enemy whose normal action doesn't loop, as the
         * disc-free build's zeros leave it. the setup below plays it */
        if (emi_actions[ACTION_NORMAL].mode < ENEMY_MOTION_LOOP) {
            placement_kill(task);
            return;
        }
#endif
        emi_setup(task);
        return;
    case MODE_WALK:
        emi_walk(work, enemy);
        emi_watch(work, enemy);
        emi_move_collide(task, work, enemy);
        break;
    case MODE_TURN:
        emi_turn(work, enemy);
        emi_move_collide(task, work, enemy);
        if (work->submode == EMI_ARMORED && work->mode == MODE_TURN &&
            (enemy->flags & ENEMY_LEDGE_HOLD_OFF)) {
            work->mode = MODE_FALL;
            enemy->flags &= ~ENEMY_HOLD_AT_LEDGE;
        }
        break;
    case MODE_AIM:
        emi_aim(task, work, enemy);
        emi_move_collide(task, work, enemy);
        emi_send_jets(task, work, enemy);
        break;
    case MODE_CHARGE:
        emi_charge(work, enemy);
        emi_move_collide(task, work, enemy);
        if (work->mode != MODE_CHARGE) {
            sound_stop_key(task);
        }
        emi_send_jets(task, work, enemy);
        break;
    case MODE_BRAKE:
        emi_brake(work, enemy);
        emi_move_collide(task, work, enemy);
        break;
    /* @hack spelling: the case of MODE_BROKEN after MODE_BRAKE's, not in the
     * modes' order */
    case MODE_BROKEN:
        emi_break(work, enemy);
        placement_kill(task);
        return;
    case MODE_FIRE:
        emi_fire(task, work, enemy);
        break;
    case MODE_STAND:
        emi_stand(task, work, enemy);
        break;
    case MODE_RECOIL:
        emi_recoil(work, enemy);
        emi_move_collide(task, work, enemy);
        break;
    case MODE_FALL:
        emi_fall(task, work, enemy);
        break;
    }
    work->timer++;
    /* while a kind other than EMI_ARMORED turns in place, players, enemies
     * and objects can't push its body */
    if (work->mode == MODE_TURN && work->submode != EMI_ARMORED) {
        work->collision->shapes->push |= PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
    } else {
        work->collision->shapes->push &= ~PUSH_FIRM(PUSH_ALL_BUT_PICKUPS);
    }
    /* @hack spelling: one test inside the other, where && would do */
    if (work->mode != MODE_RECOIL && work->mode != MODE_WALK &&
        work->mode != MODE_TURN) {
        if (work->submode != EMI_ARMORED ||
            enemy->floor_normal.y > njCos(STEEPEST)) {
            enemy->flags &= ~ENEMY_LEDGE_HOLD_OFF;
        }
    }
    if (!game_state.time_stopped) {
        enemy_save_pos(work, enemy);
        SCROLL_V(enemy) -= enemy->spd.x * SCROLL_RATE;
        enemy_motion_step(&enemy->motion);
    }
    collision_register(task);
}
