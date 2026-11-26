/* the kyoko, an enemy that a stage's object list names "E KYOKO". it walks or
 * stands by its kind, and most kinds fire at a player near their home */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/helpers.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/capturing_bullet.h"
#include "shared/dust.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_kyoko.h"
#include "shared/enemy_kyoko_art.h"
#include "shared/enemy_laser.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/cache.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* draw_control_set under a second name, which config/1st_read/symbols.txt
 * places at its address. it is declared with the Float that kyoko_draw
 * passes after the bits */
void draw_control_set_float(Uint32 bits, Float unused);

/* the kyoko's work->mode. work->submode holds its kyoko_kind */
enum kyoko_mode {
    MODE_INIT,
    MODE_STAND,
    MODE_WALK,
    MODE_ENTER, /* an entering kind's wait at home, and its climb */
    MODE_DEFEATED, /* a hit has beaten the kyoko, which its next step kills */
    MODE_DEAD
};

/* the fields of the set entry's ang.x, and the laser length's shift */
#define KIND_BITS 0xf
#define DUST_BIT 0x10
#define BULLETS_PASS_BIT 0x20
#define LASER_LENGTH_BITS 0xff00
#define LASER_LENGTH_SHIFT 8

/* the kyoko's bits in work->flags */
/* the kyoko lies farther than scl.z + RANGE from home, across x and z. only
 * a walk sets or clears the bit */
#define KYOKO_AWAY 0x1000
/* a player is in range. in MODE_ENTER, the bit stays set once a player has
 * come in range */
#define KYOKO_ALERT 0x2000
#define KYOKO_DUST 0x4000 /* DUST_BIT, kept once the setup clears ang.x */

/* the kyoko's own fields of its enemy_work. SHOTS and LASER_LENGTH share
 * one */
#define BULLET_KIND(enemy) ((enemy)->unk_046)
/* a gunner's shots in a burst. the work's tick counts the shots fired */
#define SHOTS(enemy) ((enemy)->unk_047)
#define LASER_LENGTH(enemy) ((enemy)->unk_047)
/* the steps the work's timer must exceed before the kyoko fires */
#define INTERVAL(enemy) ((enemy)->unk_048)
#define SHOT_SPEED(enemy) ((enemy)->unk_094) /* a copy of scl.x */
/* how high the gun stands above its lowest, from 0 to GUN_TRAVEL */
#define GUN_HEIGHT(enemy) ((enemy)->unk_098)
#define ANIM_FRAME(enemy) ((enemy)->unk_04e) /* kyoko_body_anim's frame */
#define SPIN(enemy) ((enemy)->unk_050) /* kyoko_spinner's angle about y */
#define SPIN_STEP 0x1000 /* what SPIN gains a step, in Angle units */
/* a walk's acceleration: x along the heading, z across it. nothing sets
 * ACC.z, which keeps the zero enemy_work_alloc gives it */
#define ACC(enemy) ((enemy)->unk_00c)

/* the indexes of kyoko_actions. no step asks for kyoko_actions[1] or
 * kyoko_actions[2] */
#define ACTION_STAND 0
#define ACTION_TURN 3
#define ACTION_FIRE 4
#define ACTION_CHARGE 5 /* holds still while a laser charges */
#define ACTION_JETS_ON 6
#define ACTION_JETS 7
#define ACTION_JETS_OFF 8

/* a kyoko's range, from home across x and z, is scl.z + RANGE */
#define RANGE 80.0F
#define SIGHT_ANGLE ((Angle)(160 * ANGLE_PER_DEGREE))
#define TURN_SPEED 0x300 /* the most the kyoko turns a step, in Angle units */
/* a standing kyoko plays ACTION_TURN in each step that turns it
 * TURN_ACTION_MIN Angle units or more */
#define TURN_ACTION_MIN 32
/* a walk's top speed a step, ahead or back. a player's touch sets spd.x to
 * -WALK_SPEED, which walks the kyoko back */
#define WALK_SPEED 2.0F
/* a walk's step that turns the kyoko more than SLOW_TURN Angle units slows
 * it. the step multiplies spd.x by TURN_SLOWDOWN where spd.x is above
 * SLOW_SPEED */
#define SLOW_TURN 128
#define SLOW_SPEED 0.4F
#define TURN_SLOWDOWN 0.9F
/* ACC.x in a walk, positive ahead. beyond the range, a walk takes SPEED_UP.
 * within it, a walk takes BRAKE while no player is in range. with a player in
 * range, it takes RECOVER while spd.x lies below -SLOW_SPEED, and SPEED_UP
 * otherwise */
#define SPEED_UP 0.1F
#define RECOVER 0.02F
#define BRAKE (-0.1F)

/* INTERVAL's values, in steps. a gunner's or catcher's INTERVAL becomes
 * REST_INTERVAL after each burst and each catch */
#define GUNNER_INTERVAL 60
#define CATCHER_INTERVAL 180
#define REST_INTERVAL 120
/* a shot leaves in a step that finds ACTION_FIRE's frame from FIRE_FRAME up
 * to FIRE_FRAME_END */
#define FIRE_FRAME 0.9F
#define FIRE_FRAME_END 1.05F
/* the most a shot pitches from level, in Angle units */
#define AIM_PITCH 0x3000

/* the gun moves by GUN_STEP a step, bringing the capture gun's muzzle toward
 * the nearest player's pos.y. every kind measures from that muzzle. the gun
 * holds while the muzzle lies within GUN_SLACK of the player's pos.y. it
 * rises at most GUN_TRAVEL above its lowest */
#define GUN_TRAVEL 9.0F
#define GUN_STEP 0.2F
#define GUN_SLACK 1.0F
#define GUN_BASE (-16.0F) /* a gun object's pos[1] at GUN_HEIGHT 0 */
/* each gun's muzzle in the kyoko's frame, in which x points ahead and y up.
 * a muzzle's y is GUN_HEIGHT plus its _MUZZLE_Y, but a walking gunner's
 * stays at WALKING_MUZZLE_Y */
#define BULLET_MUZZLE_X 13.4F
#define BULLET_MUZZLE_Y 14.0F
#define WALKING_MUZZLE_X 11.4F
#define WALKING_MUZZLE_Y 13.0F
#define LASER_MUZZLE_X 14.4F
#define LASER_MUZZLE_Y 14.0F
#define CAPTURE_MUZZLE_X 11.4F
#define CAPTURE_MUZZLE_Y 13.0F

/* a climb's spd.y nears CLIMB_FAST while its goal lies more than FAR_ABOVE
 * above the kyoko or FAR_BELOW below. it nears CLIMB_SLOW while the goal lies
 * nearer. each is signed toward the goal, and spd.y holds while the goal is
 * level with the kyoko. an entering kyoko's goal is the top of its climb, and
 * any other's the nearest player's height. with no player in range, a kyoko
 * that isn't entering takes spd.y toward 0. spd.y moves by CLIMB_STEP a step,
 * but toward CLIMB_SLOW by CLIMB_SLOW_STEP where the kyoko stands or enters */
#define FAR_ABOVE 18.0F
#define FAR_BELOW 5.0F
#define CLIMB_FAST 2.0F
#define CLIMB_SLOW 0.5F
#define CLIMB_STEP 0.05F
#define CLIMB_SLOW_STEP 0.02F

/* the ring of dust a kyoko raises under DUST_BIT, centered DUST_X along x in
 * the kyoko's frame */
#define DUST_X (-15.0F)
#define DUST_RADIUS 19.0F
#define DUST_SCALE 2.5F
#define DUST_PUFFS 2
/* a walking kyoko sends out two jets at each step of ACTION_JETS. in the
 * kyoko's frame, each leaves from (JET_X, JET_Y, JET_Z or -JET_Z) at
 * JET_SPEED along x */
#define JET_X (-16.6F)
#define JET_Y 13.2F
#define JET_Z 13.5F
#define JET_SPEED (-0.7F)

/* the points score_add_enemy adds for a beaten kyoko: SCORE, or SCORE_KNOCK
 * where player_touch_attacks returns TRUE */
#define SCORE 200
#define SCORE_KNOCK 400
/* a beaten kyoko's ball, under ENEMY_IN_MAGIC_HAND, grows to BALL_SCALE. it
 * draws the kyoko's model off its center by BALL_OFFSET_X in x and
 * BALL_OFFSET_Y in y */
#define BALL_SCALE 0.09F
#define BALL_OFFSET_X 20.0F
#define BALL_OFFSET_Y (-22.0F)

/* the index in lights of the light the kyoko draws in */
#define KYOKO_LIGHT 8
/* the radius of the sphere about pos that kyoko_draw tests against the
 * view */
#define VIEW_RADIUS 40.0F
/* the sound a kyoko holds from its setup until its task is destroyed */
#define KYOKO_SOUND 0x4010
#define KYOKO_VOLUME 8
#define TEXTURE_LEVEL 0 /* the texture task's level */

#define SHADOW_SIZE 20.0F
#define SHADOW_RATIO 1.0F
/* the kyoko draws three shadows SHADOW_LIFT above pos. the body's lies
 * SHADOW_OFFSET behind pos, and the other two lie SHADOW_OFFSET to either
 * side of pos */
#define SHADOW_LIFT 0.01F
#define SHADOW_OFFSET 13.0F
#define SHADOW_BODY_SCALE 20.0F
#define SHADOW_SIDE_SCALE 8.0F

#define KYOKO_LEVELS (SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS)

/* the collision shapes, in the kyoko's frame */
#define KYOKO_SHAPES 4
/* clang-format off */
static struct collision_shape kyoko_shapes[KYOKO_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS, KYOKO_LEVELS,
     SHAPE_HURTABLE,
     {-15.0F, 30.0F, 0.0F}, 15.0F, 4.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_ALL_BUT_PICKUPS, KYOKO_LEVELS,
     SHAPE_UNTURNED | SHAPE_HURTABLE,
     {0.0F, 15.0F, 0.0F}, 6.0F, 8.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, KYOKO_LEVELS,
     SHAPE_HURTABLE,
     {-6.0F, 12.0F, 13.0F}, 9.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_ALL_BUT_PICKUPS, KYOKO_LEVELS,
     SHAPE_HURTABLE,
     {-6.0F, 12.0F, -13.0F}, 9.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

/* the models enemy_break breaks a beaten kyoko into, null-ended. each kind
 * with a gun breaks into its gun's model too */
static NJS_CNK_MODEL *kyoko_pieces[10] = {
#include "shared/enemy_kyoko/kyoko_pieces.inc"
};
static NJS_CNK_MODEL *kyoko_bullet_pieces[11] = {
#include "shared/enemy_kyoko/kyoko_bullet_pieces.inc"
};
static NJS_CNK_MODEL *kyoko_laser_pieces[11] = {
#include "shared/enemy_kyoko/kyoko_laser_pieces.inc"
};
static NJS_CNK_MODEL *kyoko_capture_pieces[11] = {
#include "shared/enemy_kyoko/kyoko_capture_pieces.inc"
};

static struct enemy_action kyoko_actions[ACTION_JETS_OFF + 1] = {
#include "shared/enemy_kyoko/kyoko_actions.inc"
};

/* the task that keeps kyoko_texlist loaded, or null while none does. its
 * destructor releases the list */
static struct task *kyoko_textures = NULL;

static void kyoko_textures_release(struct task *task)
{
    njReleaseTexture(&kyoko_texlist);
    kyoko_textures = NULL;
}

static void kyoko_textures_step(struct task *task)
{
}

static void kyoko_draw(struct task *task);
static void kyoko_destroy(struct task *task);

static void kyoko_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    NJS_CNK_OBJECT *object;

    enemy = enemy_work_alloc_grouped(task);
    work->submode = work->ang.x & KIND_BITS;
    /* @hack redundant: the test of submode below 0, which can't hold */
    if (work->submode >= KYOKO_KINDS || work->submode < 0) {
        work->submode = KYOKO_WALKER;
    }
    if (work->ang.x & DUST_BIT) {
        work->flags |= KYOKO_DUST;
    }
    collision_init(task, kyoko_shapes, KYOKO_SHAPES, COLLISION_ENEMY);
    /* @hack spelling: object set in the switch's comma, where &kyoko_object
     * stored at each case would do */
    switch (object = &kyoko_object, work->submode) {
    case KYOKO_WALKER:
    case KYOKO_ENTERING_WALKER:
        enemy->motion.object = object;
        break;
    case KYOKO_CATCHER:
    case KYOKO_ENTERING_CATCHER:
        INTERVAL(enemy) = CATCHER_INTERVAL;
        SHOT_SPEED(enemy) = work->scl.x;
        enemy->motion.object = object;
        capturing_bullet_load_texture();
        break;
    /* @hack spelling: the two gunners' cases written apart, where one case
     * with all four labels would do */
    case KYOKO_GUNNER:
    case KYOKO_ENTERING_GUNNER:
        if (work->ang.x & BULLETS_PASS_BIT) {
            BULLET_KIND(enemy) = ENEMY_BULLET_PASSES;
        } else {
            BULLET_KIND(enemy) = ENEMY_BULLET_STOPS;
        }
        SHOTS(enemy) = work->ang.z;
        INTERVAL(enemy) = GUNNER_INTERVAL;
        SHOT_SPEED(enemy) = work->scl.x;
        enemy->motion.object = object;
        enemy_bullet_load();
        break;
    case KYOKO_WALKING_GUNNER:
    case KYOKO_ENTERING_WALKING_GUNNER:
        if (work->ang.x & BULLETS_PASS_BIT) {
            BULLET_KIND(enemy) = ENEMY_BULLET_PASSES;
        } else {
            BULLET_KIND(enemy) = ENEMY_BULLET_STOPS;
        }
        SHOTS(enemy) = work->ang.z;
        INTERVAL(enemy) = GUNNER_INTERVAL;
        SHOT_SPEED(enemy) = work->scl.x;
        enemy->motion.object = object;
        enemy_bullet_load();
        break;
    case KYOKO_LASER:
    case KYOKO_ENTERING_LASER:
        LASER_LENGTH(enemy) =
            (work->ang.x & LASER_LENGTH_BITS) >> LASER_LENGTH_SHIFT;
        INTERVAL(enemy) = work->ang.z;
        SHOT_SPEED(enemy) = work->scl.x;
        enemy->motion.object = object;
        enemy_laser_load();
        break;
    }
    work->ang.x = 0;
    work->ang.z = 0;
    switch (work->submode) {
    case KYOKO_WALKER:
    case KYOKO_WALKING_GUNNER:
        work->mode = MODE_WALK;
        break;
    case KYOKO_GUNNER:
    case KYOKO_LASER:
    case KYOKO_CATCHER:
        work->mode = MODE_STAND;
        break;
    case KYOKO_ENTERING_WALKER:
    case KYOKO_ENTERING_GUNNER:
    case KYOKO_ENTERING_WALKING_GUNNER:
    case KYOKO_ENTERING_LASER:
    case KYOKO_ENTERING_CATCHER:
        work->mode = MODE_ENTER;
        break;
    }
    enemy->sight_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->sight_angle = SIGHT_ANGLE;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = WALK_SPEED;
    enemy->turn_speed = TURN_SPEED;
    enemy->floor_bounce = 0.0F;
    enemy->friction = 0.9999F;
    enemy_set_home(work, enemy);
    enemy->motion.table = kyoko_actions;
    enemy->motion.next = ACTION_STAND;
    enemy->bottom = -7.0F;
    enemy->radius = 25.0F;
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = kyoko_draw;
    task->destructor = kyoko_destroy;
    enemy_jet_load_texture();
    sound_play_task_held_at(KYOKO_SOUND, task, 0, KYOKO_VOLUME);
    if (kyoko_textures == NULL) {
        texture_load_pvm("E_KYOKOTEX", &kyoko_texlist);
        kyoko_textures =
            task_create(0, TEXTURE_LEVEL, kyoko_textures_step, "ManTex");
        kyoko_textures->destructor = kyoko_textures_release;
    }
}

/* move pos from the kyoko's frame into the world */
static void kyoko_point_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

/* turn vec from the kyoko's frame into the world */
static void kyoko_vector_to_world(struct task_work *work, NJS_VECTOR *vec)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vec, vec);
    njPopMatrixEx();
}

static void kyoko_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & KYOKO_AWAY) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & KYOKO_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}

static void kyoko_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local distance, where enemy_home_distance2_flat's
     * result compared in place would do */
    Float distance;

    if (game_state.time_stopped) {
        return;
    }
    distance = enemy_home_distance2_flat(work, enemy);
    if (distance > powf(work->scl.z + RANGE, 2.0F)) {
        work->flags |= KYOKO_AWAY;
    } else {
        work->flags &= ~KYOKO_AWAY;
    }
}

static void kyoko_check_hit(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    int score;
    int player;
    NJS_VECTOR offset;
    NJS_VECTOR scl;

    if (!enemy_take_hit(work, enemy)) {
        return;
    }
    work->mode = MODE_DEFEATED;
    work->collision->shapes->flags |= SHAPE_OFF;
    work->timer = 0;
    enemy_set_home(work, enemy);
    switch (work->submode) {
    case KYOKO_WALKER:
    case KYOKO_ENTERING_WALKER:
        enemy_break(work, kyoko_pieces, &kyoko_texlist);
        break;
    case KYOKO_GUNNER:
    case KYOKO_WALKING_GUNNER:
    case KYOKO_ENTERING_GUNNER:
    case KYOKO_ENTERING_WALKING_GUNNER:
        enemy_break(work, kyoko_bullet_pieces, &kyoko_texlist);
        break;
    case KYOKO_LASER:
    case KYOKO_ENTERING_LASER:
        enemy_break(work, kyoko_laser_pieces, &kyoko_texlist);
        break;
    case KYOKO_CATCHER:
    case KYOKO_ENTERING_CATCHER:
        enemy_break(work, kyoko_capture_pieces, &kyoko_texlist);
        break;
    }
    score = SCORE;
    if (player_touch_attacks(task)) {
        score = SCORE_KNOCK;
    }
    score_add_enemy(score);
    enemy_defeat(work, enemy);
    if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
        /* @hack spelling: player set to shot_by, where shot_by passed in
         * place would do */
        player = enemy->shot_by;
        offset.x = BALL_OFFSET_X;
        offset.y = BALL_OFFSET_Y;
        offset.z = 0.0F;
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        scl.x = scl.y = scl.z = BALL_SCALE;
        /* @hack spelling: the call under a switch on submode with only a
         * default, where the call alone would do */
        switch (work->submode) {
        default:
            enemy_ball_create(player, &kyoko_object, &kyoko_texlist, &scl,
                              &offset);
        }
    }
}

static void kyoko_mark_player_near_home(struct task_work *work,
                                        struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= KYOKO_ALERT;
    } else {
        work->flags &= ~KYOKO_ALERT;
    }
}

/* move a walking kyoko by spd, and climb it toward the nearest player */
static void kyoko_move(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    int player;
    /* @hack spelling: the local nearest, where player_work[player] read in
     * place would do */
    struct task_work *nearest;
    NJS_POINT3 pos;
    Float dy;
    Float low;
    Float high;

    if (game_state.time_stopped) {
        return;
    }
    if (collision_next_player(task)) {
        enemy->spd.x = -WALK_SPEED;
    } else {
        enemy->spd.x += ACC(enemy).x;
        if (ACC(enemy).x <= 0.0F &&
            njAbs(enemy->spd.x) <= njAbs(ACC(enemy).x)) {
            enemy->spd.x = 0.0F;
        }
        enemy->spd.z += ACC(enemy).z;
    }
    if (enemy->spd.x > WALK_SPEED) {
        enemy->spd.x = WALK_SPEED;
    } else if (enemy->spd.x < -WALK_SPEED) {
        enemy->spd.x = -WALK_SPEED;
    }
    work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                   (enemy->spd.z * njSin(work->ang.y));
    work->pos.y += enemy->spd.y;
    work->pos.z += (-enemy->spd.x * njSin(work->ang.y)) +
                   (enemy->spd.z * njCos(work->ang.y));
    if (work->scl.y < 0.0F) {
        low = work->scl.y + enemy->home.y;
        high = enemy->home.y;
    } else {
        low = enemy->home.y;
        high = work->scl.y + enemy->home.y;
    }
    if (work->pos.y > high) {
        work->pos.y = high;
    } else if (work->pos.y < low) {
        work->pos.y = low;
    }
    enemy_meet_stage(work, enemy);
    /* @bug logic: player_nearest's result is never -1, so this test always
     * holds (at 0x770 of the object's code) */
    if ((work->flags & KYOKO_ALERT) &&
        (player = player_nearest(&work->pos)) != -1) {
        nearest = player_work[player];
        dy = nearest->pos.y - work->pos.y;
        if (dy < 0.0F) {
            if (dy < -FAR_BELOW) {
                enemy->spd.y =
                    float_approach(enemy->spd.y, -CLIMB_FAST, CLIMB_STEP);
            } else {
                enemy->spd.y =
                    float_approach(enemy->spd.y, -CLIMB_SLOW, CLIMB_STEP);
            }
        } else if (dy > 0.0F) {
            if (dy > FAR_ABOVE) {
                enemy->spd.y =
                    float_approach(enemy->spd.y, CLIMB_FAST, CLIMB_STEP);
            } else {
                enemy->spd.y =
                    float_approach(enemy->spd.y, CLIMB_SLOW, CLIMB_STEP);
            }
        }
    } else {
        enemy->spd.y = float_approach(enemy->spd.y, 0.0F, CLIMB_STEP);
    }
    enemy_keep_near_home_flat(work, enemy, work->scl.z + RANGE);
    if ((enemy->flags & ENEMY_ON_FLOOR) && (work->flags & KYOKO_DUST)) {
        pos.x = DUST_X;
        pos.y = 0.0F;
        pos.z = 0.0F;
        kyoko_point_to_world(work, &pos);
        dust_short_ring(&pos, DUST_RADIUS, DUST_SCALE, DUST_PUFFS);
    }
}

/* climb a standing kyoko toward the nearest player */
static void kyoko_climb(struct task_work *work, struct enemy_work *enemy)
{
    int player;
    /* @hack spelling: the local nearest, where player_work[player] read in
     * place would do */
    struct task_work *nearest;
    NJS_POINT3 pos;
    Float dy;
    Float low;
    Float high;

    if (game_state.time_stopped) {
        return;
    }
    work->pos.y += enemy->spd.y;
    if (work->scl.y < 0.0F) {
        low = work->scl.y + enemy->home.y;
        high = enemy->home.y;
    } else {
        low = enemy->home.y;
        high = work->scl.y + enemy->home.y;
    }
    if (work->pos.y > high) {
        work->pos.y = high;
    } else if (work->pos.y < low) {
        work->pos.y = low;
    }
    /* @bug logic: player_nearest's result is never -1, so this test always
     * holds (at 0x908 of the object's code) */
    if ((work->flags & KYOKO_ALERT) &&
        (player = player_nearest(&work->pos)) != -1) {
        nearest = player_work[player];
        dy = nearest->pos.y - work->pos.y;
        if (dy < 0.0F) {
            if (dy < -FAR_BELOW) {
                enemy->spd.y =
                    float_approach(enemy->spd.y, -CLIMB_FAST, CLIMB_STEP);
            } else {
                enemy->spd.y =
                    float_approach(enemy->spd.y, -CLIMB_SLOW, CLIMB_SLOW_STEP);
            }
        } else if (dy > 0.0F) {
            if (dy > FAR_ABOVE) {
                enemy->spd.y =
                    float_approach(enemy->spd.y, CLIMB_FAST, CLIMB_STEP);
            } else {
                enemy->spd.y =
                    float_approach(enemy->spd.y, CLIMB_SLOW, CLIMB_SLOW_STEP);
            }
        }
    } else {
        enemy->spd.y = float_approach(enemy->spd.y, 0.0F, CLIMB_STEP);
    }
    if ((work->flags & KYOKO_DUST) && work->pos.y <= low) {
        pos.x = DUST_X;
        pos.y = 0.0F;
        pos.z = 0.0F;
        kyoko_point_to_world(work, &pos);
        dust_short_ring(&pos, DUST_RADIUS, DUST_SCALE, DUST_PUFFS);
    }
}

/* climb an entering kyoko toward home.y + scl.y, once a player has come in
 * range. it returns TRUE at the step whose climb passes that height, and
 * clamps pos.y to it there. it returns FALSE at every other step. with scl.y
 * 0, pos.y never passes home.y */
static Bool kyoko_enter_climb(struct task_work *work, struct enemy_work *enemy)
{
    Float low;
    Float high;
    Float target;
    Float dy;

    if (game_state.time_stopped) {
        return FALSE;
    }
    if (!(work->flags & KYOKO_ALERT)) {
        return FALSE;
    }
    work->pos.y += enemy->spd.y;
    if (work->scl.y < 0.0F) {
        low = work->scl.y + enemy->home.y;
        high = enemy->home.y;
        target = low;
    } else {
        low = enemy->home.y;
        /* @hack spelling: high taken as scl.y plus low, where scl.y plus
         * home.y would do */
        high = work->scl.y + low;
        target = high;
    }
    if (work->pos.y > high) {
        work->pos.y = high;
        if (target == high) {
            return TRUE;
        }
    } else if (work->pos.y < low) {
        work->pos.y = low;
        if (target == low) {
            return TRUE;
        }
    }
    dy = target - work->pos.y;
    if (dy < 0.0F) {
        if (dy < -FAR_BELOW) {
            enemy->spd.y =
                float_approach(enemy->spd.y, -CLIMB_FAST, CLIMB_STEP);
        } else {
            enemy->spd.y =
                float_approach(enemy->spd.y, -CLIMB_SLOW, CLIMB_SLOW_STEP);
        }
    } else if (dy > 0.0F) {
        if (dy > FAR_ABOVE) {
            enemy->spd.y = float_approach(enemy->spd.y, CLIMB_FAST, CLIMB_STEP);
        } else {
            enemy->spd.y =
                float_approach(enemy->spd.y, CLIMB_SLOW, CLIMB_SLOW_STEP);
        }
    }
    /* @hack redundant: a store of CLIMB_SLOW_STEP that no path reaches */
    if (0) {
        enemy->spd.y = CLIMB_SLOW_STEP;
    }
    return FALSE;
}

/* turn dir, a shot's speed in the kyoko's frame, into the world, pitched
 * toward the nearest player. the pitch aims at the center of that player's
 * first collision shape, as seen from pos. it is AIM_PITCH at most */
static void kyoko_aim(struct task_work *work, NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: the local dy, where the difference used in place would
     * do */
    Float dy;
    Angle pitch;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    dy = target.y - pos->y;
    if (distance != 0.0F) {
        pitch = asinf(dy / distance) * ANGLE_PER_RADIAN;
    } else {
        pitch = 0;
    }
    pitch = angle_approach(0, pitch, AIM_PITCH);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, pitch);
    njCalcVector(NULL, dir, dir);
    njPopMatrixEx();
}

static void kyoko_fire_bullet(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    if (work->submode == KYOKO_WALKING_GUNNER ||
        work->submode == KYOKO_ENTERING_WALKING_GUNNER) {
        pos.x = WALKING_MUZZLE_X;
        pos.y = WALKING_MUZZLE_Y;
        pos.z = 0.0F;
    } else {
        pos.x = BULLET_MUZZLE_X;
        pos.y = GUN_HEIGHT(enemy) + BULLET_MUZZLE_Y;
        pos.z = 0.0F;
    }
    kyoko_point_to_world(work, &pos);
    dir.x = SHOT_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    kyoko_aim(work, &pos, &dir);
    enemy_bullet_fire(task, BULLET_KIND(enemy), &dir, &pos);
}

static void kyoko_fire_laser(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = LASER_MUZZLE_X;
    pos.y = GUN_HEIGHT(enemy) + LASER_MUZZLE_Y;
    pos.z = 0.0F;
    kyoko_point_to_world(work, &pos);
    dir.x = SHOT_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    kyoko_aim(work, &pos, &dir);
    enemy_laser_fire(task, LASER_LENGTH(enemy), &dir, &pos);
}

/* aim the charging laser again from the muzzle, as kyoko_fire_laser aimed
 * it */
static void kyoko_aim_laser(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = LASER_MUZZLE_X;
    pos.y = GUN_HEIGHT(enemy) + LASER_MUZZLE_Y;
    pos.z = 0.0F;
    kyoko_point_to_world(work, &pos);
    dir.x = SHOT_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    kyoko_aim(work, &pos, &dir);
    enemy_laser_aim(task, &dir, &pos);
}

static void kyoko_fire_capture(struct task *task, struct task_work *work,
                               struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    pos.x = CAPTURE_MUZZLE_X;
    pos.y = GUN_HEIGHT(enemy) + CAPTURE_MUZZLE_Y;
    pos.z = 0.0F;
    kyoko_point_to_world(work, &pos);
    dir.x = SHOT_SPEED(enemy);
    dir.y = 0.0F;
    dir.z = 0.0F;
    kyoko_aim(work, &pos, &dir);
    capturing_bullet_fire(task, &dir, &pos);
}

static void kyoko_move_gun(struct task_work *work, struct enemy_work *enemy)
{
    int player;
    /* @hack spelling: the local nearest, where player_work[player] read at
     * each use would do */
    struct task_work *nearest;
    NJS_POINT3 pos;
    Float dy;

    if (game_state.time_stopped) {
        return;
    }
    player = player_nearest(&work->pos);
    if (player == 0 || player == 1) {
        nearest = player_work[player];
        if (nearest) {
            pos.x = CAPTURE_MUZZLE_X;
            pos.y = GUN_HEIGHT(enemy) + CAPTURE_MUZZLE_Y;
            pos.z = 0.0F;
            kyoko_point_to_world(work, &pos);
            dy = nearest->pos.y - pos.y;
            if (njAbs(dy) >= GUN_SLACK) {
                dy += GUN_HEIGHT(enemy);
                if (dy > GUN_TRAVEL) {
                    dy = GUN_TRAVEL;
                } else if (dy < 0.0F) {
                    dy = 0.0F;
                }
                GUN_HEIGHT(enemy) =
                    float_approach(GUN_HEIGHT(enemy), dy, GUN_STEP);
            }
        }
    }
}

static void kyoko_destroy(struct task *task)
{
    enemy_destroy(task);
    sound_stop_key(task);
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void kyoko_stand(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    Angle ang;

    kyoko_mark_player_near_home(work, enemy);
    if (enemy->motion.action == ACTION_STAND ||
        enemy->motion.action == ACTION_TURN) {
        ang = work->ang.y;
        kyoko_turn(work, enemy);
        if (angle_distance(ang, work->ang.y) >= TURN_ACTION_MIN) {
            enemy->motion.next = ACTION_TURN;
        } else {
            enemy->motion.next = ACTION_STAND;
        }
    }
    kyoko_climb(work, enemy);
    kyoko_move_gun(work, enemy);
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    /* @bug memory: reads the nearest player's work through null where
     * player_nearest finds no player and slot 0 is empty. kyoko_move_gun
     * tests that work for null, at 0xe7c of the object's code. kyoko_stand
     * reads it untested at 0xfdc, and kyoko_walk at 0x12b2 and 0x1370 */
#ifdef NONMATCHING
    /* this side repairs the tag above. with no player to read, the stand only
     * takes hits. so does the walk of each kind but a walker. a walker's walk
     * reads no player */
    if (player_work[player_nearest(&work->pos)] == NULL) {
        kyoko_check_hit(task, work, enemy);
        return;
    }
#endif
    if (player_work[player_nearest(&work->pos)]->mode == PLAYER_MODE_CAUGHT &&
        (work->submode == KYOKO_CATCHER ||
         work->submode == KYOKO_ENTERING_CATCHER)) {
        work->mode = MODE_WALK;
        work->timer = 0;
    } else if (work->submode == KYOKO_GUNNER ||
               work->submode == KYOKO_CATCHER ||
               work->submode == KYOKO_ENTERING_GUNNER ||
               work->submode == KYOKO_ENTERING_CATCHER) {
        if (work->flags & CAPTURING_BULLET_CAUGHT) {
            work->flags &= ~CAPTURING_BULLET_CAUGHT;
            work->timer = 0;
            INTERVAL(enemy) = REST_INTERVAL;
        }
        if (!(work->flags & KYOKO_ALERT)) {
            work->timer = 0;
            work->tick = 0;
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_STAND ||
                enemy->motion.action == ACTION_TURN) {
                enemy->motion.next = ACTION_FIRE;
            } else if (enemy->motion.action == ACTION_FIRE) {
                if (enemy->motion.frame >= FIRE_FRAME &&
                    enemy->motion.frame < FIRE_FRAME_END) {
                    if (work->submode == KYOKO_GUNNER ||
                        work->submode == KYOKO_ENTERING_GUNNER) {
                        kyoko_fire_bullet(task, work, enemy);
                        work->tick++;
                    } else {
                        /* a catcher's burst is a single shot */
                        kyoko_fire_capture(task, work, enemy);
                        work->tick = SHOTS(enemy);
                    }
                    if (work->tick >= SHOTS(enemy)) {
                        work->tick = 0;
                        work->timer = 0;
                        INTERVAL(enemy) = REST_INTERVAL;
                    }
                }
            }
        }
    } else if (work->submode == KYOKO_LASER ||
               work->submode == KYOKO_ENTERING_LASER) {
        if (!(work->flags & KYOKO_ALERT)) {
            work->timer = 0;
            work->flags &= ~ENEMY_LASER_CHARGING;
        } else if (work->flags & ENEMY_LASER_CHARGING) {
            /* the laser sets ENEMY_LASER_CHARGING again at each step it
             * charges */
            work->flags &= ~ENEMY_LASER_CHARGING;
            kyoko_aim_laser(task, work, enemy);
        } else if (work->timer > INTERVAL(enemy)) {
            if (enemy->motion.action == ACTION_STAND ||
                enemy->motion.action == ACTION_TURN) {
                enemy->motion.next = ACTION_CHARGE;
                kyoko_fire_laser(task, work, enemy);
            } else if (enemy->motion.action == ACTION_CHARGE) {
                enemy->motion.next = ACTION_FIRE;
                work->timer = 0;
            }
        }
    }
    kyoko_check_hit(task, work, enemy);
}

static void kyoko_walk(struct task *task, struct task_work *work,
                       struct enemy_work *enemy)
{
    Angle ang;

    kyoko_mark_away(work, enemy);
    kyoko_mark_player_near_home(work, enemy);
    ang = work->ang.y;
    kyoko_turn(work, enemy);
    if (angle_distance(ang, work->ang.y) > SLOW_TURN &&
        enemy->spd.x > SLOW_SPEED) {
        enemy->spd.x *= TURN_SLOWDOWN;
    }
    if (work->flags & KYOKO_AWAY) {
        ACC(enemy).x = SPEED_UP;
    } else if (!(work->flags & KYOKO_ALERT)) {
        ACC(enemy).x = BRAKE;
    } else if (enemy->spd.x < -SLOW_SPEED) {
        ACC(enemy).x = RECOVER;
    } else {
        ACC(enemy).x = SPEED_UP;
    }
    kyoko_move(task, work, enemy);
    kyoko_move_gun(work, enemy);
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
#ifdef NONMATCHING
    /* this side repairs the tag in kyoko_stand for the walk's reads. with no
     * player to read, a kyoko other than a walker only takes hits. a walker's
     * test reads no player */
    if (work->submode != KYOKO_WALKER &&
        work->submode != KYOKO_ENTERING_WALKER &&
        player_work[player_nearest(&work->pos)] == NULL) {
        kyoko_check_hit(task, work, enemy);
        return;
    }
#endif
    /* @bug logic: an entering catcher that walks sends out no jets. the test
     * names KYOKO_CATCHER twice, where the test below, which ends a catcher's
     * walk, names both catchers (CMP/EQ at 0x129a and 0x12a0 of the object's
     * code) */
    if (work->submode == KYOKO_WALKER ||
        work->submode == KYOKO_ENTERING_WALKER ||
        /* NOLINTNEXTLINE(misc-redundant-expression) */
        ((work->submode == KYOKO_CATCHER || work->submode == KYOKO_CATCHER) &&
         player_work[player_nearest(&work->pos)]->mode == PLAYER_MODE_CAUGHT)) {
        if (!(work->flags & KYOKO_ALERT)) {
            if (enemy->motion.action == ACTION_JETS ||
                enemy->motion.action == ACTION_JETS_ON) {
                enemy->motion.next = ACTION_JETS_OFF;
            }
        } else if (enemy->motion.action != ACTION_JETS &&
                   enemy->motion.action != ACTION_JETS_ON) {
            enemy->motion.next = ACTION_JETS_ON;
        }
    } else if (player_work[player_nearest(&work->pos)]->mode !=
                   PLAYER_MODE_CAUGHT &&
               (work->submode == KYOKO_CATCHER ||
                work->submode == KYOKO_ENTERING_CATCHER)) {
        work->mode = MODE_STAND;
        work->timer = 0;
        if (enemy->motion.action == ACTION_JETS ||
            enemy->motion.action == ACTION_JETS_ON) {
            enemy->motion.next = ACTION_JETS_OFF;
        }
    } else if (work->submode == KYOKO_WALKING_GUNNER ||
               work->submode == KYOKO_ENTERING_WALKING_GUNNER) {
        if (!(work->flags & KYOKO_ALERT)) {
            work->timer = 0;
            work->tick = 0;
        } else if (work->timer > INTERVAL(enemy)) {
            /* @hack spelling: ACTION_TURN tested ahead of ACTION_STAND, not
             * in kyoko_stand's order */
            if (enemy->motion.action == ACTION_TURN ||
                enemy->motion.action == ACTION_STAND) {
                enemy->motion.next = ACTION_FIRE;
            } else if (enemy->motion.action == ACTION_FIRE) {
                if (enemy->motion.frame >= FIRE_FRAME &&
                    enemy->motion.frame < FIRE_FRAME_END) {
                    kyoko_fire_bullet(task, work, enemy);
                    work->tick++;
                    if (work->tick >= SHOTS(enemy)) {
                        /* @hack spelling: tick cleared after timer and
                         * INTERVAL, not ahead of them as in kyoko_stand */
                        work->timer = 0;
                        INTERVAL(enemy) = REST_INTERVAL;
                        work->tick = 0;
                    }
                }
            }
        }
    }
    kyoko_check_hit(task, work, enemy);
}

static void kyoko_enter(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    if (enemy_find_player_near_home(work, enemy)) {
        work->flags |= KYOKO_ALERT;
    }
    kyoko_turn(work, enemy);
    if (kyoko_enter_climb(work, enemy)) {
        switch (work->submode) {
        case KYOKO_ENTERING_WALKER:
        case KYOKO_ENTERING_WALKING_GUNNER:
            work->mode = MODE_WALK;
            break;
        /* @hack spelling: the standing kinds' labels in the reverse of their
         * order */
        case KYOKO_ENTERING_CATCHER:
        case KYOKO_ENTERING_LASER:
        case KYOKO_ENTERING_GUNNER:
            work->mode = MODE_STAND;
            break;
        }
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    kyoko_check_hit(task, work, enemy);
    /* @hack redundant: a return at the end */
    return; /* NOLINT(readability-redundant-control-flow) */
}

/* draw the kyoko with its kind's gun, then its shadows */
static void kyoko_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles ang;
    NJS_VECTOR scl;
    NJS_POINT3 pos;
    /* @hack spelling: rotate_y, a local for njRotateY, where the call by its
     * name would do */
    void (*rotate_y)(NJS_MATRIX *matrix, Angle ang);
    /* @hack spelling: push, a local for njPushMatrixEx, where the call by its
     * name would do */
    Bool (*push)(void);

    /* @hack redundant: a read of njPrintC, which leaves no code */
    (void)njPrintC;
    /* @hack redundant: a read of shadow_object's address, which leaves no
     * code */
    (void)&shadow_object;
    if (sphere_out_of_view(&work->pos, VIEW_RADIUS)) {
        return;
    }
    light_apply(KYOKO_LIGHT);
    rotate_y = njRotateY;
    push = njPushMatrixEx;
    switch (work->submode) {
    case KYOKO_WALKER:
    case KYOKO_ENTERING_WALKER:
        kyoko_capture_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_laser_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_bullet_gun->evalflags |= NJD_EVAL_HIDE;
        break;
    case KYOKO_GUNNER:
    case KYOKO_WALKING_GUNNER:
    case KYOKO_ENTERING_GUNNER:
    case KYOKO_ENTERING_WALKING_GUNNER:
        kyoko_capture_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_laser_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_bullet_gun->evalflags &= ~NJD_EVAL_HIDE;
        kyoko_bullet_gun->evalflags &= ~NJD_EVAL_UNIT_POS;
        kyoko_bullet_gun->pos[1] = GUN_HEIGHT(enemy) + GUN_BASE;
        break;
    case KYOKO_LASER:
    case KYOKO_ENTERING_LASER:
        kyoko_capture_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_laser_gun->evalflags &= ~NJD_EVAL_HIDE;
        kyoko_bullet_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_laser_gun->evalflags &= ~NJD_EVAL_UNIT_POS;
        kyoko_laser_gun->pos[1] = GUN_HEIGHT(enemy) + GUN_BASE;
        break;
    case KYOKO_CATCHER:
    case KYOKO_ENTERING_CATCHER:
        kyoko_capture_gun->evalflags &= ~NJD_EVAL_HIDE;
        kyoko_laser_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_bullet_gun->evalflags |= NJD_EVAL_HIDE;
        kyoko_capture_gun->evalflags &= ~NJD_EVAL_UNIT_POS;
        kyoko_capture_gun->pos[1] = GUN_HEIGHT(enemy) + GUN_BASE;
        /* @hack redundant: a store of GUN_BASE that no path reaches */
        if (0) {
            pos.y = GUN_BASE;
        }
        break;
    }
    push();
    njTranslateEx(&work->pos);
    rotate_y(NULL, work->ang.y + QUARTER_TURN);
    njSetTexture(&kyoko_texlist);
    texture_anim_apply(&kyoko_body_model, &kyoko_body_anim, ANIM_FRAME(enemy));
    kyoko_spinner->evalflags &= ~NJD_EVAL_UNIT_ANG;
    kyoko_spinner->ang[1] = SPIN(enemy);
    if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
    } else {
        njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                  enemy->motion.frame);
    }
    njPopMatrixEx();
#ifdef NONMATCHING
    /* this side keeps the tag's result, as the callee reads only the bits. it
     * calls draw_control_set by its own name. only symbols.txt places the
     * second name, which this side's objects don't define */
    draw_control_set(DRAW_CONTROL_SHADOW);
#else
    /* @bug mismatched-call: passes shadow_size after the bits, where
     * draw_control_set takes the bits alone. the callee reads only the bits,
     * so the call sets them as one with the bits alone would. the FMOV.S in
     * the JSR's slot at 0x16ec of the object's code passes the Float */
    draw_control_set_float(DRAW_CONTROL_SHADOW, enemy->shadow_size);
#endif
    push();
    njTranslate(NULL, work->pos.x, work->pos.y + SHADOW_LIFT, work->pos.z);
    rotate_y(NULL, work->ang.y);
    push();
    njTranslate(NULL, -SHADOW_OFFSET, 0.0F, 0.0F);
    njScale(NULL, SHADOW_BODY_SCALE, 1.0F, SHADOW_BODY_SCALE);
    /* @hack redundant: a store of -SHADOW_OFFSET that no path reaches */
    if (0) {
        pos.x = -SHADOW_OFFSET;
    }
    /* @hack type: the address of shadow_object's first field, cast to the
     * object, where &shadow_object would do */
    njCnkModDrawObject((NJS_CNK_OBJECT *)&shadow_object.evalflags);
    matrix_load_parent();
    njTranslate(NULL, 0.0F, 0.0F, -SHADOW_OFFSET);
    njScale(NULL, SHADOW_SIDE_SCALE, 1.0F, SHADOW_SIDE_SCALE);
    njCnkModDrawObject((NJS_CNK_OBJECT *)&shadow_object.evalflags);
    matrix_load_parent();
    njTranslate(NULL, 0.0F, 0.0F, SHADOW_OFFSET);
    njScale(NULL, SHADOW_SIDE_SCALE, 1.0F, SHADOW_SIDE_SCALE);
    njCnkModDrawObject((NJS_CNK_OBJECT *)&shadow_object.evalflags);
    matrix_load_parent();
    /* @hack redundant: stores of 1 to pos.y and pos.z that no path reaches */
    if (0) {
        pos.y = 1.0F;
        pos.z = 1.0F;
    }
    njPopMatrix(2);
    draw_control_clear(DRAW_CONTROL_SHADOW);
    /* @hack redundant: a readout that no path reaches, which leaves its
     * cases' code and its strings in the object */
    if (0) {
        /* @hack redundant: the local kind and its clamp to the first five
         * kinds, which leave no code */
        int kind;

        kind = work->submode;
        if (kind >= KYOKO_KINDS) {
            kind = KYOKO_ENTERING_CATCHER;
        }
        if (kind >= KYOKO_ENTERING_WALKER) {
            kind -= KYOKO_ENTERING_WALKER;
        }
        /* @hack redundant: four prints of kind, which leave no code */
        njPrintD(0, kind, 0);
        njPrintD(0, kind, 0);
        njPrintD(0, kind, 0);
        njPrintD(0, kind, 0);
        switch (kind) {
        case KYOKO_WALKER:
        case KYOKO_ENTERING_WALKER:
            njPrintC(NJM_LOCATION(17, 21), "-> MOVE RANGE");
            break;
        case KYOKO_CATCHER:
        case KYOKO_ENTERING_CATCHER:
            njPrintC(NJM_LOCATION(17, 19), "-> BALL SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> MOVE RANGE");
            break;
        case KYOKO_GUNNER:
        case KYOKO_ENTERING_GUNNER:
            if (work->ang.x & BULLETS_PASS_BIT) {
                njPrintC(NJM_LOCATION(21, 15), "BULLET IGNORE LAND COLLI");
            } else {
                njPrintC(NJM_LOCATION(21, 15), "NORMAL BULLET");
            }
            njPrintC(NJM_LOCATION(17, 18), "-> RAPID ");
            njPrintD(NJM_LOCATION(26, 18), (Uint8)work->ang.z, 3);
            njPrintC(NJM_LOCATION(17, 19), "-> BULLET SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> VIEW RANGE");
            break;
        case KYOKO_WALKING_GUNNER:
        case KYOKO_ENTERING_WALKING_GUNNER:
            if (work->ang.x & BULLETS_PASS_BIT) {
                njPrintC(NJM_LOCATION(21, 15), "BULLET IGNORE LAND COLLI");
            } else {
                njPrintC(NJM_LOCATION(21, 15), "NORMAL BULLET");
            }
            njPrintC(NJM_LOCATION(17, 18), "-> RAPID ");
            njPrintD(NJM_LOCATION(26, 18), (Uint8)work->ang.z, 3);
            njPrintC(NJM_LOCATION(17, 19), "-> BULLET SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> MOVE RANGE");
            break;
        case KYOKO_LASER:
        case KYOKO_ENTERING_LASER:
            njPrintC(NJM_LOCATION(19, 15), "LASER=");
            njPrintD(NJM_LOCATION(26, 15),
                     (work->ang.x & LASER_LENGTH_BITS) >> LASER_LENGTH_SHIFT,
                     3);
            njPrintC(NJM_LOCATION(29, 15), "0cm");
            njPrintC(NJM_LOCATION(17, 18), "-> INTERVAL ");
            njPrintD(NJM_LOCATION(29, 18), work->ang.z, 5);
            njPrintC(NJM_LOCATION(34, 18), "FRAMES");
            njPrintC(NJM_LOCATION(17, 19), "-> LASER SPEED");
            njPrintC(NJM_LOCATION(17, 21), "-> VIEW RANGE");
            break;
        }
        njPrintC(NJM_LOCATION(17, 20), "-> UP or DOWN RANGE");
        njPrintC(NJM_LOCATION(17, 16), (work->ang.x & DUST_BIT)
                                           ? "-> SMOKE ON  TYPE(0-4)"
                                           : "-> SMOKE OFF TYPE(0-4)");
        njPrintC(NJM_LOCATION(17, 17), "-> ROT&EME.ID ");
        njPrintD(NJM_LOCATION(31, 17), (Uint8)work->ang.y, 3);
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        ang.x = ang.y = ang.z = 0;
        /* @hack spelling: one assignment to scl.z and scl.x, where a
         * statement for each would do */
        scl.z = scl.x = work->scl.z + RANGE;
        scl.y = 10.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
        /* @hack spelling: one assignment to the three, where a statement for
         * each would do */
        ang.x = ang.y = ang.z = 0;
        /* @hack spelling: one assignment to scl.z and scl.x, where a
         * statement for each would do */
        scl.z = scl.x = 2.0F;
        scl.y = work->scl.y;
        pos = work->pos;
        pos.y += 0.5F * work->scl.y;
        debug_draw_model_unlit(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

void kyoko_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_POINT3 pos;
    NJS_VECTOR dir;

    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
#ifdef NONMATCHING
        /* this side removes an enemy whose stand action doesn't loop, as the
         * disc-free build's zeros leave it. the init below plays it */
        if (kyoko_actions[ACTION_STAND].mode < ENEMY_MOTION_LOOP) {
            placement_kill(task);
            return;
        }
#endif
        kyoko_init(task);
        return;
    case MODE_STAND:
        kyoko_stand(task, work, enemy);
        break;
    case MODE_WALK:
        kyoko_walk(task, work, enemy);
        if (enemy->motion.action == ACTION_JETS) {
            pos.x = JET_X;
            pos.y = JET_Y;
            pos.z = JET_Z;
            kyoko_point_to_world(work, &pos);
            dir.x = JET_SPEED;
            /* @hack spelling: one assignment to dir.y and dir.z, where a
             * statement for each would do */
            dir.y = dir.z = 0.0F;
            kyoko_vector_to_world(work, &dir);
            enemy_jet_create(task, ENEMY_JET_LARGE, &pos, &dir);
            pos.x = JET_X;
            pos.y = JET_Y;
            pos.z = -JET_Z;
            kyoko_point_to_world(work, &pos);
            enemy_jet_create(task, ENEMY_JET_LARGE, &pos, &dir);
        }
        break;
    case MODE_ENTER:
        kyoko_enter(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        return;
    /* @hack redundant: a default that only breaks */
    default:
        break;
    }
    /* @hack spelling: the empty arm and the else, where one arm under
     * !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
        ANIM_FRAME(enemy)++;
        SPIN(enemy) += SPIN_STEP;
    }
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    collision_register(task);
    /* @hack redundant: a call of kyoko_init that no path reaches, under a
     * float test */
    if (0.0F) {
        switch (work->mode) {
        case MODE_INIT:
            kyoko_init(task);
            break;
        }
    }
}
