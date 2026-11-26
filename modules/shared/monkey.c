/* the monkey, E SARU in a stage's object list. it hops around the point it
 * was placed at. the normal kind throws bombs at a player in range */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "shared/enemy_bomb.h"
#include "shared/monkey.h"
#include "shared/ring.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/snapshot.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "shared/monkey/text.inc"
#include "shared/monkey_art/models.h"

/* a monkey's modes, in its work's mode */
enum monkey_mode {
    MODE_INIT,
    MODE_CIRCLE,
    MODE_THROW, /* a MONKEY_NORMAL's, from when a player comes in range */
    MODE_DOLL, /* either doll kind's, which never throws */
    MODE_DEFEATED
};

/* in the work's flags */
#define MONKEY_ALERT 0x1000 /* a player is in range */
/* the monkey has asked for a held bomb, its task's child, and not yet thrown
 * it */
#define MONKEY_HOLDING 0x2000

/* the bits of ang.x that ask for the dust and for the move down to the
 * floor */
#define DUST_BITS 0xf00
#define GROUND_BITS 0xf000

/* the monkey's state in the unk_ fields of enemy_work, which enemy.h leaves
 * to each kind of enemy */
#define DUST(enemy) ((enemy)->unk_046) /* 1 where the monkey raises dust */
/* the kind of bomb a MONKEY_NORMAL holds and throws */
#define BOMB(enemy) ((enemy)->unk_047)
/* the steps the work's timer must pass before the monkey throws */
#define INTERVAL(enemy) ((enemy)->unk_048)
/* the monkey's bearing from home, and how far the bearing moves in a step */
#define BEARING(enemy) ((enemy)->unk_01c)
#define BEARING_SPEED(enemy) ((enemy)->unk_028)
/* the steps of circling left before the monkey picks another speed */
#define CIRCLE_STEPS(enemy) ((enemy)->unk_04e)

/* the last bomb the monkey held or threw, or null, in its task's unk_3c */
#define LAST_BOMB(task) ((task)->unk_3c.task)

/* the indexes of monkey_actions */
enum monkey_action {
    ACTION_HOP_A,
    ACTION_HOP_B,
    ACTION_THROW,
    ACTION_TURN_TO_A, /* the turn from ACTION_HOP_B to ACTION_HOP_A */
    ACTION_TURN_TO_B, /* that turn played backward, to ACTION_HOP_B */
    ACTION_HEAVY_HOLD,
    ACTION_HEAVY_THROW,
    ACTION_HEAVY_FUMBLE,
    ACTION_HOP_TO_THROW
};

/* frames of a hop's motion. the monkey rises above home.y from HOP_START,
 * is highest at HOP_PEAK and is back at home.y by HOP_END. past HOP_PEAK,
 * its lift falls HOP_FALL a frame */
#define HOP_START 0.5F
#define HOP_PEAK 2.0F
#define HOP_END 2.5F
#define HOP_FALL 1.5F
/* a hop raises dust in its frames below DUST_FRAME. the dust's speed is
 * DUST_SPEED times the monkey's */
#define DUST_FRAME 0.14F
#define DUST_SPEED 0.2F
#define DUST_SCALE 1.0F

/* the monkey circles home scl.y + CIRCLE_RADIUS away. CIRCLE_SPEED is its
 * speed either way around, in units a step. it picks a speed again once it
 * has circled CIRCLE_TIME up to CIRCLE_TIME + CIRCLE_TIME_RANGE steps */
#define CIRCLE_RADIUS 40.0F
#define CIRCLE_SPEED 0.18F
#define CIRCLE_TIME 300.0F
#define CIRCLE_TIME_RANGE 10.0F
/* how far BEARING_SPEED moves toward turn_speed in a step */
#define BEARING_ACCEL 64
/* how far ang.y turns toward the monkey's heading in a step */
#define TURN_STEP 0x400
/* a pick goes one of PICKS ways at random. the monkey circles with ACTION_HOP_A
 * or with ACTION_HOP_B, or hops in place. a pick that stops the monkey plays
 * ACTION_TURN_TO_A or ACTION_TURN_TO_B. it plays the first where a random
 * number from 0 up to 1 passes TURN_TO_A_THRESHOLD. between picks, a hop near
 * its end may turn the monkey to its other hop. it does where such a number
 * passes HOP_SWITCH_THRESHOLD */
#define PICKS 3.0F
enum monkey_pick { PICK_HOP_A, PICK_HOP_B, PICK_IN_PLACE };
#define TURN_TO_A_THRESHOLD 0.5F
#define HOP_SWITCH_THRESHOLD 0.9F

/* a player within scl.z + RANGE of the monkey, across x and z, is in range */
#define RANGE 80.0F
/* the turn_speed of a throwing monkey, in Angle units a step */
#define THROW_TURN_SPEED 0x300
/* the held bomb leaves in the step whose ACTION_THROW frame lies from
 * THROW_FRAME up to THROW_FRAME_END. a BOMB_HEAVY's throw or fumble starts in
 * the step whose ACTION_HEAVY_THROW frame lies from HEAVY_THROW_FRAME up to
 * HEAVY_THROW_FRAME_END */
#define THROW_FRAME 3.87F
#define THROW_FRAME_END 4.0F
#define HEAVY_THROW_FRAME 5.86F
#define HEAVY_THROW_FRAME_END 6.0F
/* a BOMB_HEAVY's throw is fumbled where a random number from 0 up to 1
 * passes FUMBLE_THRESHOLD */
#define FUMBLE_THRESHOLD 0.5F
/* how far a held bomb lies above the place its motion gives, in units of
 * its scl.y */
#define HELD_LIFT 1.0F
#define HEAVY_HELD_LIFT 5.0F

/* a held bomb that draws a ring becomes a scattered ring, placed as
 * enemy_bomb_create places one */
#define RING_DROP 10.0F
#define RING_LEVEL 2

/* the points score_add_enemy adds for a beaten monkey: SCORE, or SCORE_KNOCK
 * where player_touch_attacks returns TRUE */
#define SCORE 100
#define SCORE_KNOCK 200
/* the ball a beaten monkey leaves under ENEMY_IN_MAGIC_HAND */
#define BALL_SCALE 0.55F
#define BALL_DROP 3.0F

#define RECORD_RADIUS 8.0F
#define SHADOW_SIZE 2.6F
#define SHADOW_RATIO 1.0F
/* the index in lights of the light the monkey draws under */
#define MONKEY_LIGHT 8
/* the task level of the task, with no records, whose end releases the
 * monkeys' textures */
#define TEXTURES_LEVEL 0
/* the sound a monkey plays once its last bomb's blast touches a player */
#define BOMB_HIT_SOUND 0x400a
#define BOMB_HIT_VOLUME 60

/* the readout's names. they are defined after monkey_step, so that SHC
 * places their strings after the texture names */
static char *monkey_titles[MONKEY_KINDS];
static char *monkey_bomb_names[BOMB_KINDS];

/* the collision shapes: a MONKEY_DOLL_NO_PUSH_OUT takes the second, and any
 * other kind the first. the second pushes only players */
/* clang-format off */
static struct collision_shape monkey_shapes[2] = {
    {0, SHAPE_SPHERE, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 3.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_SPHERE, PUSH_PLAYERS | PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_ATTACK_LEVEL(1) | SHAPE_HURTS_PLAYERS, SHAPE_HURTABLE,
     {0.0F, 3.0F, 0.0F}, 5.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static NJS_CNK_MODEL *monkey_pieces[16] = {
#include "shared/monkey/monkey_pieces.inc"
};

static struct enemy_action monkey_actions[9] = {
#include "shared/monkey/monkey_actions.inc"
};

/* the task that keeps monkey_texlist loaded while it lives, or null */
static struct task *monkey_textures = NULL;

static void monkey_textures_release(struct task *task)
{
    njReleaseTexture(&monkey_texlist);
    monkey_textures = NULL;
}

static void monkey_textures_step(struct task *task)
{
}

static void monkey_draw(struct task *task);
static void monkey_destroy(struct task *task);

/* the monkey's first step, which sets it up as monkey.h says */
static void monkey_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    struct angles ang;
    Float y;

    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= MONKEY_KINDS || work->submode < 0) {
        work->submode = MONKEY_NORMAL;
    }
    enemy = enemy_work_alloc_grouped(task);
    if (work->submode == MONKEY_DOLL_NO_PUSH_OUT) {
        collision_init(task, &monkey_shapes[1], 1, COLLISION_ENEMY);
    } else {
        collision_init(task, monkey_shapes, 1, COLLISION_ENEMY);
    }
    collision_set_radius(task, RECORD_RADIUS);
    if (work->ang.x & DUST_BITS) {
        DUST(enemy) = 1;
    } else {
        DUST(enemy) = 0;
    }
    if (work->submode == MONKEY_NORMAL) {
        BOMB(enemy) = work->ang.x;
        if (BOMB(enemy) > BOMB_RING || BOMB(enemy) < 0) {
            BOMB(enemy) = BOMB_NORMAL;
        }
        INTERVAL(enemy) = work->ang.z;
    }
    switch (work->submode) {
    case MONKEY_DOLL:
    case MONKEY_DOLL_NO_PUSH_OUT:
        work->mode = MODE_DOLL;
        break;
    default:
        work->mode = MODE_CIRCLE;
        break;
    }
    work->timer = 0;
    enemy->flags |= ENEMY_RELEASES_ANIMAL;
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
    if (work->ang.x & GROUND_BITS) {
        y = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z, &ang);
        if (y != NO_FLOOR) {
            work->pos.y = y;
        }
    }
    work->ang.z = 0;
    work->ang.x = 0;
    enemy_set_home(work, enemy);
    enemy->motion.table = monkey_actions;
    /* @hack spelling: object and next set in both arms, where one store of
     * each ahead of the test would do */
    if (work->submode == MONKEY_NORMAL) {
        enemy->motion.object = monkey_object;
        enemy->motion.next = ACTION_HOP_A;
        enemy_bomb_load();
    } else {
        enemy->motion.object = monkey_object;
        enemy->motion.next = ACTION_HOP_A;
    }
    enemy_motion_step(&enemy->motion);
    enemy->flags |= ENEMY_BURST;
    LAST_BOMB(task) = NULL;
    if (monkey_textures == NULL) {
        texture_load_pvm(MONKEY_TEXTURE_ARCHIVE, &monkey_texlist);
        monkey_textures = task_create(0, TEXTURES_LEVEL, monkey_textures_step,
                                      MONKEY_TEXTURE_TASK_NAME);
        monkey_textures->destructor = monkey_textures_release;
    }
    task->draw = monkey_draw;
    task->destructor = monkey_destroy;
}

static void monkey_check_hit(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    NJS_VECTOR offset;
    NJS_VECTOR scl;
    int score;
    /* @hack spelling: player set to shot_by, where shot_by passed in place
     * would do */
    int player;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_DEFEATED;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        enemy_break(work, monkey_pieces, &monkey_texlist);
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
        if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
            player = enemy->shot_by;
            offset.x = 0.0F;
            offset.y = -BALL_DROP;
            /* @bug uninitialized: enemy_ball_create reads offset.z, which
             * nothing sets. offset.x is stored twice, the second time at
             * 0x2de of the object's code. enemy_ball_create copies offset.z
             * into the ball's record at 0x8c023c92 */
#ifdef NONMATCHING
            /* this side repairs the tag above: offset.z is 0, as the other
             * enemies' balls take it */
            offset.z = 0.0F;
#endif
            offset.x = 0.0F;
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            /* @hack spelling: the call in a switch on submode of a default
             * alone, where the call alone would do */
            switch (work->submode) {
            default:
                enemy_ball_create(player, monkey_object, &monkey_texlist, &scl,
                                  &offset);
                break;
            }
        }
    }
}

/* move pos from the monkey's axes, which ang.y turns, to the world's */
static void monkey_point_to_world(struct task_work *work, NJS_POINT3 *pos)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, pos, pos);
    njPopMatrixEx();
}

static void monkey_mark_player_near(struct task_work *work,
                                    struct enemy_work *enemy)
{
    if (enemy_find_player_near(work, enemy)) {
        work->flags |= MONKEY_ALERT;
    } else {
        work->flags &= ~MONKEY_ALERT;
    }
}

static void monkey_throw(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    int action;
    int player;
    int kind;
    struct task *ring;
    struct task_work *ring_work;
    struct task_work *target;
    struct task_work *bomb;
    NJS_MOTION *motion;
    NJS_POINT3 pos;
    Float lift;

    action = enemy->motion.action;
    monkey_mark_player_near(work, enemy);
    enemy->turn_speed = THROW_TURN_SPEED;
    player = player_nearest(&work->pos);
    /* @bug memory: the turn toward the nearest player reads slot 0 of
     * player_work through null. player_nearest returns 0 where it finds no
     * player, even where slot 0 is empty. monkey_throw tests the nearest
     * player's slot for null at 0x5e2 of the object's code. the call at 0x468
     * reaches enemy_aim_at_player, whose read at 0x8c022510 meets the null */
#ifdef NONMATCHING
    /* this side repairs the tag above: the monkey turns toward no empty
     * slot */
    if ((player == 0 || player == 1) && player_work[player] != NULL) {
        enemy_turn_to_player(work, enemy, player);
    }
#else
    if (player == 0 || player == 1) {
        enemy_turn_to_player(work, enemy, player);
    }
#endif
    if (work->timer > INTERVAL(enemy)) {
        /* @hack spelling: BOMB tested against BOMB_HEAVY in both arms, where
         * an else for the second would do */
        if (BOMB(enemy) != BOMB_HEAVY) {
            if (action != ACTION_THROW && action != ACTION_HOP_TO_THROW) {
                enemy->motion.next = ACTION_HOP_TO_THROW;
                return;
            }
            if (action == ACTION_THROW &&
                enemy->motion.last == ACTION_HOP_TO_THROW) {
                if (work->flags & MONKEY_HOLDING) {
                    return;
                }
                LAST_BOMB(task) =
                    enemy_bomb_create(task, BOMB(enemy), BOMB_HELD, &work->pos);
                work->flags |= MONKEY_HOLDING;
                return;
            }
            if (enemy->motion.frame >= THROW_FRAME &&
                enemy->motion.frame < THROW_FRAME_END) {
                pos = work->pos;
                kind = BOMB(enemy);
                if (task->child != NULL) {
                    pos = task->child->work->pos;
                    kind = task->child->work->submode;
                    task->child->work->flags &= ~WORK_HELD_BY_PLAYER;
                    task_kill(task->child);
                    /* @hack spelling: the assignment inside the test, where a
                     * statement of its own would do */
                    if (kind == BOMB_RING &&
                        (ring = task_create(TASK_WORK | TASK_EXTRA, RING_LEVEL,
                                            scattered_ring_step,
                                            MONKEY_DEBRIS_TASK_NAME)) != NULL) {
                        ring_work = ring->work;
                        game_state.ring_total++;
                        snapshot_current->ring_total++;
                        ring_work->pos = pos;
                        ring_work->pos.y -= RING_DROP;
                        target = player_work[player];
                        if (target != NULL) {
                            ring_work->ang.y =
                                (Angle)(atan2f(target->pos.x - pos.x,
                                               target->pos.z - pos.z) *
                                        ANGLE_PER_RADIAN);
                        }
                        /* @bug memory: keeps as LAST_BOMB the held bomb
                         * passed to task_kill above. while
                         * game_state.time_stopped isn't 0, monkey_step skips
                         * its test of the bomb's step. once time moves, it
                         * reads the bomb after its free. the branches at 0x5e4
                         * and 0x612 of the object's code skip the store at
                         * 0x624, and the read is at 0xfaa */
#ifdef NONMATCHING
                        /* this side repairs the tag above: the monkey keeps
                         * no bomb once it has thrown a ring */
                        LAST_BOMB(task) = NULL;
#endif
                        /* @hack spelling: a goto past the throw, where an
                         * else around the throw would do */
                        goto thrown;
                    }
                }
                /* @bug memory: keeps a scattered ring's task as LAST_BOMB,
                 * and reads the task after the ring frees it. a BOMB_RING
                 * thrown here can roll a ring, and enemy_bomb_create then
                 * returns the ring's task. the store at 0x624 of the object's
                 * code keeps it. a scattered ring destroys its own task
                 * without task_kill, so monkey_step's test of the task's step,
                 * at 0xfaa, can read the freed block. monkey_step also tests
                 * the address of the ring's chain record for BOMB_HIT_PLAYER,
                 * at 0xfb8 */
#ifdef NONMATCHING
                /* this side repairs the tag above: the monkey keeps no
                 * scattered ring as LAST_BOMB, so it plays no BOMB_HIT_SOUND
                 * for one. the image's monkey plays it for a live ring whose
                 * chain entry's address holds the bit BOMB_HIT_PLAYER */
                {
                    struct task *made =
                        enemy_bomb_create(NULL, kind, BOMB_THROWN, &pos);

                    LAST_BOMB(task) =
                        made != NULL && made->step == scattered_ring_step
                            ? NULL
                            : made;
                }
#else
                LAST_BOMB(task) =
                    enemy_bomb_create(NULL, kind, BOMB_THROWN, &pos);
#endif
            thrown:
                work->timer = 0;
                work->flags &= ~MONKEY_HOLDING;
            }
        } else if (BOMB(enemy) == BOMB_HEAVY) {
            if (action == ACTION_HEAVY_FUMBLE) {
                if (enemy->motion.flags & ENEMY_MOTION_ENDED) {
                    pos = work->pos;
                    if (task->child != NULL) {
                        pos = task->child->work->pos;
                        task->child->work->flags &= ~WORK_HELD_BY_PLAYER;
                        task_kill(task->child);
                    }
                    enemy_bomb_create(NULL, BOMB_HEAVY, BOMB_BURSTING, &pos);
                    work->timer = 0;
                    /* WORK_HURT makes the monkey_check_hit below beat the
                     * monkey */
                    work->flags |= WORK_HURT;
                    work->flags &= ~MONKEY_HOLDING;
                }
            } else if (action != ACTION_HEAVY_THROW) {
                enemy->motion.next = ACTION_HEAVY_THROW;
            } else if (enemy->motion.frame >= HEAVY_THROW_FRAME &&
                       enemy->motion.frame < HEAVY_THROW_FRAME_END) {
                if ((Float)rand() * RAND_SCALE > FUMBLE_THRESHOLD) {
                    enemy->motion.next = ACTION_HEAVY_FUMBLE;
                } else {
                    pos = work->pos;
                    kind = BOMB(enemy);
                    if (task->child != NULL) {
                        pos = task->child->work->pos;
                        kind = task->child->work->submode;
                        task->child->work->flags &= ~WORK_HELD_BY_PLAYER;
                        task_kill(task->child);
                    }
                    LAST_BOMB(task) =
                        enemy_bomb_create(NULL, kind, BOMB_THROWN, &pos);
                    work->timer = 0;
                    work->flags &= ~MONKEY_HOLDING;
                }
            }
        }
    } else if (!(work->flags & MONKEY_ALERT) && action != ACTION_HEAVY_HOLD) {
        /* @bug logic: a monkey back from MODE_THROW circles at
         * THROW_TURN_SPEED until it next picks a speed. monkey_move moves
         * BEARING_SPEED toward turn_speed, and the change to MODE_CIRCLE leaves
         * turn_speed at THROW_TURN_SPEED. the throw stores it at 0x450 of the
         * object's code, and monkey_move reads it at 0xace */
        work->mode = MODE_CIRCLE;
    } else if (BOMB(enemy) == BOMB_HEAVY && action != ACTION_HEAVY_HOLD &&
               action != ACTION_HEAVY_THROW && action != ACTION_HEAVY_FUMBLE) {
        enemy->motion.next = ACTION_HEAVY_HOLD;
        if (!(work->flags & MONKEY_HOLDING)) {
            LAST_BOMB(task) =
                enemy_bomb_create(task, BOMB(enemy), BOMB_HELD, &work->pos);
            work->flags |= MONKEY_HOLDING;
        }
    }
    if ((work->flags & MONKEY_HOLDING) && task->child != NULL) {
        motion = NULL;
        switch (action) {
        case ACTION_HEAVY_HOLD:
            motion = &monkey_heavy_hold_bomb_motion;
            break;
        case ACTION_HEAVY_THROW:
            motion = &monkey_heavy_throw_bomb_motion;
            break;
        case ACTION_HEAVY_FUMBLE:
            motion = &monkey_heavy_fumble_bomb_motion;
            break;
        case ACTION_THROW:
            motion = &monkey_throw_bomb_motion;
            break;
        }
        if (motion != NULL) {
            bomb = task->child->work;
            if (bomb->submode == BOMB_HEAVY) {
                lift = HEAVY_HELD_LIFT;
            } else {
                lift = HELD_LIFT;
            }
            njSetCnkCurrentMotion(motion, enemy->motion.frame);
            njGetCnkMotionTranslate(&pos.x, 0);
            /* turn the place from the model's axes to the monkey's.
             * monkey_draw turns the model by the same quarter turn about y */
            bomb->pos.x = pos.z;
            bomb->pos.y = pos.y + (bomb->scl.y * lift);
            bomb->pos.z = -pos.x;
            monkey_point_to_world(work, &bomb->pos);
        }
    }
    monkey_check_hit(task, work, enemy);
}

/* move the monkey around home. turn_speed here is the bearing's speed that
 * BEARING_SPEED eases toward, in Angle units a step */
static void monkey_move(struct task_work *work, struct enemy_work *enemy)
{
    Float radius;
    Float circumference;
    Angle heading;

    if (game_state.time_stopped) {
        return;
    }
    radius = work->scl.y + CIRCLE_RADIUS;
    if (CIRCLE_STEPS(enemy) <= 0 &&
        (enemy->motion.flags & ENEMY_MOTION_ENDING)) {
        CIRCLE_STEPS(enemy) =
            ((Float)rand() * (CIRCLE_TIME_RANGE * RAND_SCALE)) + CIRCLE_TIME;
        circumference = radius * (2.0F * NJD_PI);
        switch ((int)njRoundOff((Float)rand() * (PICKS * RAND_SCALE))) {
        case PICK_HOP_A:
            enemy->turn_speed = (CIRCLE_SPEED * FULL_TURN) / circumference;
            if (enemy->motion.action == ACTION_HOP_B) {
                enemy->motion.next = ACTION_TURN_TO_A;
            }
            break;
        case PICK_HOP_B:
            enemy->turn_speed = (-CIRCLE_SPEED * FULL_TURN) / circumference;
            if (enemy->motion.action == ACTION_HOP_A) {
                enemy->motion.next = ACTION_TURN_TO_B;
            }
            break;
        case PICK_IN_PLACE:
        /* @hack redundant: the label of PICK_IN_PLACE + 1, a value the switch
         * never takes */
        case PICK_IN_PLACE + 1:
            enemy->turn_speed = 0;
            if ((Float)rand() * RAND_SCALE > TURN_TO_A_THRESHOLD) {
                enemy->motion.next = ACTION_TURN_TO_A;
            } else {
                enemy->motion.next = ACTION_TURN_TO_B;
            }
            break;
        }
    } else if (CIRCLE_STEPS(enemy) > 0) {
        CIRCLE_STEPS(enemy)--;
        if ((enemy->motion.flags & ENEMY_MOTION_ENDING) &&
            (enemy->motion.action == ACTION_HOP_A ||
             enemy->motion.action == ACTION_HOP_B)) {
            if ((Float)rand() * RAND_SCALE > HOP_SWITCH_THRESHOLD) {
                /* @hack redundant: the test of ACTION_HOP_A, which is always
                 * true where it stands */
                if (enemy->motion.action == ACTION_HOP_B) {
                    enemy->motion.next = ACTION_TURN_TO_A;
                } else if (enemy->motion.action == ACTION_HOP_A) {
                    enemy->motion.next = ACTION_TURN_TO_B;
                }
            }
        }
    }
    BEARING_SPEED(enemy) =
        angle_approach(BEARING_SPEED(enemy), enemy->turn_speed, BEARING_ACCEL);
    heading = BEARING(enemy);
    /* @hack spelling: angle_difference called again for each test, where its
     * result kept from one call would do */
    /* @hack redundant: the third test, which is always true where it stands */
    if (angle_difference(0, enemy->turn_speed) > 0) {
        heading += QUARTER_TURN;
    } else if (angle_difference(0, enemy->turn_speed) < 0) {
        heading += HALF_TURN + QUARTER_TURN;
    } else if (angle_difference(0, enemy->turn_speed) == 0) {
        heading += HALF_TURN;
    }
    work->ang.y = angle_approach(work->ang.y, heading, TURN_STEP);
    BEARING(enemy) += BEARING_SPEED(enemy);
    work->pos.x = enemy->home.x + (radius * njCos(BEARING(enemy)));
    /* @hack spelling: the sine's part negated and added to home.z, where
     * home.z less it would do */
    work->pos.z = -(radius * njSin(BEARING(enemy))) + enemy->home.z;
}

static void monkey_circle(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    monkey_move(work, enemy);
    monkey_mark_player_near(work, enemy);
    if ((work->flags & MONKEY_ALERT) && work->submode == MONKEY_NORMAL) {
        work->mode = MODE_THROW;
        work->timer = 0;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    monkey_check_hit(task, work, enemy);
    /* @hack redundant: a return as the last statement */
    return; /* NOLINT(readability-redundant-control-flow) */
}

static void monkey_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles ang;
    NJS_VECTOR scl;
    int bomb;
    int kind;
    Angle bits;

    light_apply(MONKEY_LIGHT);
    njSetTexture(&monkey_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y + QUARTER_TURN);
    if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
        njCnkSimpleDrawMotion(enemy->motion.object,
                              enemy->motion.table[enemy->motion.action].motion,
                              enemy->motion.frame);
    } else {
        njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                  enemy->motion.frame);
    }
    njPopMatrixEx();
    enemy_draw_shadow(work, enemy);
    /* @hack redundant: a readout that no path reaches. the image holds its
     * strings, and its code from the case label on, which a branch skips */
    if (0) {
        /* @hack redundant: the locals bits and kind, and the prints ahead of
         * the switch, which leave no code */
        bits = work->ang.x;
        if (bits & DUST_BITS) {
            njPrintC(NJM_LOCATION(17, 22), MONKEY_DEBUG_SMOKE);
        } else {
            njPrintC(NJM_LOCATION(17, 22), MONKEY_DEBUG_CLEAR);
        }
        if (bits & GROUND_BITS) {
            njPrintC(NJM_LOCATION(17, 23), MONKEY_DEBUG_GROUND_CHECK);
        } else {
            njPrintC(NJM_LOCATION(17, 23), MONKEY_DEBUG_NO_GROUND_CHECK);
        }
        kind = work->submode;
        njPrintC(NJM_LOCATION(4, 16), monkey_titles[kind]);
        switch (kind) {
        case MONKEY_NORMAL:
            njPrintC(NJM_LOCATION(25, 16), MONKEY_DEBUG_BOMB);
            bomb = (Uint8)work->ang.x;
            if (bomb >= BOMB_KINDS || bomb < 0) {
                bomb = BOMB_NORMAL;
            }
            njPrintC(NJM_LOCATION(32, 16), monkey_bomb_names[bomb]);
            njPrintC(NJM_LOCATION(17, 18), MONKEY_DEBUG_INTERVAL);
            break;
        }
        njPrintC(NJM_LOCATION(17, 19), MONKEY_DEBUG_KIND);
        njPrintC(NJM_LOCATION(17, 20), MONKEY_DEBUG_MOVE_RANGE);
        njPrintC(NJM_LOCATION(17, 21), MONKEY_DEBUG_SEARCH_RANGE);
        njPrintC(NJM_LOCATION(17, 17), MONKEY_DEBUG_YAW);
        njPrintH(NJM_LOCATION(26, 17), work->ang.y & 0xff00, 4);
        njPrintC(NJM_LOCATION(30, 17), MONKEY_DEBUG_EMERALD);
        njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
        /* @hack spelling: the chained assignments to ang and to scl.z and
         * scl.x, where a statement for each field would do */
        ang.x = ang.y = ang.z = 0;
        scl.z = scl.x = work->scl.z + RANGE;
        scl.y = 1.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
        /* @hack spelling: the chained assignments to ang and to scl.z and
         * scl.x, where a statement for each field would do */
        ang.x = ang.y = ang.z = 0;
        scl.z = scl.x = work->scl.y + CIRCLE_RADIUS;
        scl.y = 15.0F;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

static void monkey_destroy(struct task *task)
{
    enemy_destroy(task);
}

void monkey_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct task *bomb;
    NJS_VECTOR spd;
    Float lift;
    /* @hack type: action, a Sint16 local for motion.action, where
     * motion.action read at each test would do */
    Sint16 action;

    if (work->mode != MODE_INIT && task->child == NULL &&
        placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        monkey_init(task);
        return;
    case MODE_CIRCLE:
        monkey_circle(task, work, enemy);
        break;
    case MODE_THROW:
        monkey_throw(task, work, enemy);
        break;
    /* @hack spelling: MODE_DOLL's case written apart, where its label beside
     * MODE_CIRCLE's would do */
    case MODE_DOLL:
        monkey_circle(task, work, enemy);
        break;
    case MODE_DEFEATED:
        placement_kill(task);
        return;
    }
    work->timer++;
    /* @hack spelling: the empty arm and the else, where one arm under
     * !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else {
        enemy_motion_step(&enemy->motion);
        lift = 0.0F;
        if (enemy->motion.action == ACTION_HOP_A ||
            enemy->motion.action == ACTION_HOP_B) {
            if (enemy->motion.frame <= HOP_PEAK) {
                /* @hack redundant: the frame multiplied by 1 */
                lift = (enemy->motion.frame * 1.0F) - HOP_START;
            } else {
                lift = (HOP_END - enemy->motion.frame) * HOP_FALL;
            }
        }
        if (lift < 0.0F) {
            lift = 0.0F;
        }
        work->pos.y = enemy->home.y + lift;
        if (DUST(enemy) == 1) {
            action = enemy->motion.action;
            /* @hack spelling: the frame tested in each arm, where one test
             * after the tests of action would do */
            if ((action == ACTION_HOP_A && enemy->motion.frame < DUST_FRAME) ||
                (action == ACTION_HOP_B && enemy->motion.frame < DUST_FRAME)) {
                spd = work->pos;
                njSubVector(&spd, &enemy->last_pos);
                spd.x *= DUST_SPEED;
                spd.y *= DUST_SPEED;
                spd.z *= DUST_SPEED;
                dust_create(&work->pos, &spd, DUST_SCALE);
            }
        }
        enemy_save_pos(work, enemy);
        bomb = LAST_BOMB(task);
        if (bomb != NULL) {
            if (bomb->step == task_destroy) {
                LAST_BOMB(task) = NULL;
            } else if (bomb->unk_3c.l & BOMB_HIT_PLAYER) {
                /* @hack type: unk_3c cleared as an integer, where LAST_BOMB
                 * set to null would do */
                task->unk_3c.l = 0;
                sound_play_at(BOMB_HIT_SOUND, NULL, 0, BOMB_HIT_VOLUME,
                              &work->pos);
            }
        }
    }
    collision_register(task);
}

/* SHC preserves the two original SET readout tables. */
static char *monkey_titles[MONKEY_KINDS] = {
#include "shared/monkey/monkey_titles.inc"
};

static char *monkey_bomb_names[BOMB_KINDS] = {
#include "shared/monkey/monkey_bomb_names.inc"
};
