/* the beetle, E KUMI in a stage's object list: an enemy that bobs as it
 * hovers. it comes in eight kinds. every kind but the bomber turns toward a
 * player in range */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "draw/helpers.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/barrier.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/beetle.h"
#include "shared/beetle_art.h"
#include "shared/beetle_status.h"
#include "shared/enemy_bomb.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_light.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

/* a beetle's modes, in its work's mode. nothing sets MODE_VANISH, so a warp
 * beetle that has appeared never vanishes */
enum beetle_mode {
    MODE_INIT,
    MODE_HOVER,
    MODE_HIDDEN, /* a warp beetle, unseen until a player comes in range */
    MODE_APPEAR,
    MODE_VANISH,
    MODE_DISCHARGE,
    MODE_CIRCLE,
    MODE_GUNNER,
    MODE_DEFEATED,
    MODE_DEAD
};

/* a beetle's bits in its work's flags */
#define BEETLE_AWAY 0x1000 /* farther than scl.z + RANGE from home */
#define BEETLE_ALERT 0x2000 /* a player is in range */
/* the beetle draws its kind's compiled model, by njDirectDrawObject */
#define BEETLE_DIRECT_DRAW 0x4000
/* a gunner's bullets fly through the stage, as enemy_bullet_fire's kind 0
 * does. without the bit, they are kind 1, which bursts on the stage */
#define BEETLE_BULLETS_PASS 0x8000

/* the bit of ang.z with which a placement asks for BEETLE_DIRECT_DRAW, for a
 * kind that draws beetle_object. a warp beetle clears it */
#define ASKS_DIRECT_DRAW 1
/* in a gunner's ang.x: the bits that count a burst's shots, and those that
 * set BEETLE_BULLETS_PASS */
#define SHOT_BITS 0xf
#define PASS_BITS 0xf0
/* an angle's high byte. the init clears the low byte of ang.x and of a
 * gunner's ang.z. a bomber's circle starts from ang.y's high byte */
#define HIGH_BYTE 0xff00

/* what a beetle keeps in the fields of its enemy_work that enemy.h leaves
 * to each kind of enemy */
/* the steps the work's timer must pass before a beetle discharges, fires or
 * drops a bomb */
#define INTERVAL(enemy) ((enemy)->unk_048)
/* a gunner's shots in a burst, and their speed */
#define SHOTS(enemy) ((enemy)->unk_047)
#define BULLET_SPEED(enemy) ((enemy)->unk_094)
#define BOMB_KIND(enemy) ((enemy)->unk_047)
/* the bob's angle, which the beetle keeps and nothing reads */
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* the heading a bomber's circle starts from */
#define CIRCLE_START(enemy) ((enemy)->unk_01c)
/* the upward speed a bumper throws a player at */
#define BUMP_SPEED(enemy) ((enemy)->unk_098)
/* a warp beetle's scale in x and z, which grows from 0 to 1 as it appears */
#define WARP_SCALE(enemy) ((enemy)->unk_098)
/* the steps a warp beetle has hovered, which its bob counts in place of
 * game_state.steps */
#define BOB_STEPS(enemy) ((enemy)->unk_1cc)
/* the constant material a warp beetle draws under as it appears or
 * vanishes. as it appears, the alpha rises from WARP_UNSEEN_ALPHA to 0. red,
 * green and blue then fall from 1 to 0. as it vanishes, the three rise to 1
 * first, and then the alpha falls to WARP_UNSEEN_ALPHA */
#define WARP_COLOR(enemy) ((enemy)->unk_0ac)
/* the index in discharge_texlists of the list a discharge draws with */
#define DISCHARGE_TEXLIST(enemy) ((enemy)->unk_046)
/* the frame of the discharge's texture animation */
#define DISCHARGE_FRAME(enemy) ((enemy)->unk_04e)
/* a moving beetle's acceleration: x along its heading, z across it. nothing
 * sets ACCEL, which keeps the zero enemy_work_alloc gives it */
#define ACCEL(enemy) ((enemy)->unk_00c)

/* a discharge lasts until the work's timer passes DISCHARGE_STEPS. it draws
 * with each of the BEETLE_DISCHARGE_FRAMES lists of discharge_texlists in turn,
 * one a step */
#define DISCHARGE_STEPS 300

/* a moving beetle's speed along its heading, and the bounds beetle_move
 * holds it in. ACCEL stays zero, so the speed stays SPEED */
#define SPEED 0.4F
#define SPEED_MAX 0.4F
#define SPEED_MIN 0.1F
/* a sinking beetle's spd.y falls by SINK_STEP a step, down to SINK_MAX */
#define SINK_STEP 0.05F
#define SINK_MAX (-2.0F)
/* where a beetle doesn't sink, FALL_DAMPING scales its spd.y each step. a
 * spd.y within FALL_STOP of 0 then becomes 0 */
#define FALL_DAMPING 0.95F
#define FALL_STOP 0.1F

#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.2F

/* the index in lights of the light a beetle's model is compiled and drawn
 * under */
#define BEETLE_LIGHT 8
/* a warp beetle's greatest turn in a step, as it appears or vanishes */
#define WARP_SPIN ((Float)EIGHTH_TURN)
/* how far WARP_SCALE, WARP_COLOR's alpha and its other channels move in a
 * step */
#define WARP_SCALE_STEP 0.1F
#define WARP_ALPHA_STEP 0.05F
#define WARP_TINT_STEP 0.1F
/* WARP_COLOR's alpha while a warp beetle can't be seen */
#define WARP_UNSEEN_ALPHA (-1.0F)
/* the alpha at or below which a vanishing warp beetle narrows */
#define WARP_NARROW_ALPHA (-0.5F)
/* a bumper's lamps are lit while this bit of its work's timer is set. they are
 * dark and lit by turns, 16 steps each */
#define LAMPS_LIT 0x10

/* a beetle's range is scl.z + RANGE. sight_distance2 and near_distance2 hold
 * the range squared. a moving beetle strays no farther than the range from
 * home */
#define RANGE 60.0F
/* a beetle's turn toward a player or home in a step */
#define TURN_SPEED 0x180
/* the least INTERVAL a gunner takes */
#define INTERVAL_MIN 60
/* a gunner's aim, in Angle units. a shot starts AIM_DIP below level, along
 * the gunner's heading. it then turns toward the nearest player, by at most
 * AIM_PITCH in pitch and AIM_YAW in heading */
#define AIM_DIP 0xc00
#define AIM_PITCH 0x1800
#define AIM_YAW 0x800
/* a gunner fires on the steps its fire action's frame lies from FIRE_FRAME
 * up to FIRE_FRAME_END */
#define FIRE_FRAME 1.0F
#define FIRE_FRAME_END 1.2F
/* how far the electric kinds, the gunner and the bomber bob above and below
 * home. their scl.y holds INTERVAL or BULLET_SPEED in place of the bob's
 * reach */
#define BOB_HEIGHT 2.0F
/* a moving beetle's bottom where no floor lies below its start */
#define FLOORLESS_BOTTOM (-20.0F)

/* the points score_add_enemy adds for a defeated beetle: SCORE, or
 * SCORE_KNOCK where player_touch_attacks returns TRUE */
#define SCORE 100
#define SCORE_KNOCK 200
/* the scale a defeated beetle's ball grows to */
#define BALL_SCALE 0.15F

/* the radius of the ball beetle_draw tests against the view */
#define DRAW_RADIUS 40.0F
/* the radius of an electric beetle's collision record, of a bumper's, and
 * of any other kind's */
#define ELECTRIC_RADIUS 15.0F
#define BUMPER_RADIUS 18.0F
#define BEETLE_RADIUS 9.0F
/* how far from a discharging beetle its prominences start */
#define PROMINENCE_RADIUS 15.0F

/* points in a beetle's own frame: a gunner's muzzle, and the point a bomber
 * drops its bombs from */
#define MUZZLE_X 7.5F
#define MUZZLE_Y (-9.2F)
#define BOMB_DROP_Y (-6.0F)
/* the light a beetle shows at its front while a player is in range. a
 * bumper shows it at BUMPER_FRONT_LIGHT_X and BUMPER_FRONT_LIGHT_Y */
#define FRONT_LIGHT_X 10.0F
#define FRONT_LIGHT_Y 4.6F
#define BUMPER_FRONT_LIGHT_X 11.0F
#define BUMPER_FRONT_LIGHT_Y 2.5F
/* a bumper's two lamps, below it on either side */
#define LAMP_X (-3.35F)
#define LAMP_Y (-8.32F)
#define LAMP_Z 6.78F
/* the lamps' and the front light's kinds, as enemy_light_create takes them */
#define LAMP_LIGHT 0
#define FRONT_LIGHT 1
/* a bomber's motion draws nothing until game_state.steps passes
 * BOMBER_HIDDEN_STEPS */
#define BOMBER_HIDDEN_STEPS 2

#define BUMP_SOUND 0x1000
#define HOVER_SOUND 0x400d
#define DISCHARGE_SOUND 0x400e
#define WARP_SOUND 0x400f
/* the hover and discharge sounds' volumes, and their durations in sound
 * updates. as a warp beetle appears or vanishes, its hover sound's volume is
 * HOVER_VOLUME plus SOUND_LOUDEST times WARP_COLOR's alpha */
#define HOVER_VOLUME 10
#define HOVER_SOUND_DURATION 164
#define DISCHARGE_VOLUME 15
#define DISCHARGE_SOUND_DURATION 60
#define VIBRATION_POWER 4
#define VIBRATION_FREQ 15

/* beetle_shapes holds four collision shapes. the first is a beetle's body, and
 * the second an electric beetle's discharge. the last two are a bumper's body
 * and the wider shape above it. each kind's record takes a run of them. an
 * electric beetle's takes the first ELECTRIC_SHAPES. a bumper's takes
 * BUMPER_SHAPES from BUMPER_FIRST. any other kind's takes the body alone */
#define ELECTRIC_SHAPES 2
#define BUMPER_FIRST 2
#define BUMPER_SHAPES 2
#define BODY_SHAPES 1
/* the indices in a record: an electric beetle's discharge, and the bumper's
 * shape whose touch throws a player */
#define DISCHARGE_SHAPE 1
#define THROW_SHAPE 1

static void beetle_draw(struct task *task);
static void beetle_destroy(struct task *task);

/* each texture list's holder: a task that keeps the list loaded, or null
 * while the list isn't. beetle_textures and gunner_textures also hold their
 * kind's model in extra, as njCnkDirectObjectCompile compiled it. an extra
 * is null where the heap had no room */
static struct task *beetle_textures = NULL;
static struct task *gunner_textures = NULL;
static struct task *bomber_textures = NULL;
static struct task *bumper_textures = NULL;
static struct task *electric_textures = NULL;

static void beetle_textures_release(struct task *task)
{
    njReleaseTexture(&beetle_texlist);
    beetle_textures = NULL;
}

static void gunner_textures_release(struct task *task)
{
    njReleaseTexture(&gunner_texlist);
    gunner_textures = NULL;
}

static void bomber_textures_release(struct task *task)
{
    njReleaseTexture(&bomber_texlist);
    bomber_textures = NULL;
}

static void bumper_textures_release(struct task *task)
{
    njReleaseTexture(&bumper_texlist);
    bumper_textures = NULL;
}

static void electric_textures_release(struct task *task)
{
    njReleaseTexture(&electric_texlist);
    electric_textures = NULL;
}

static void textures_step(struct task *task)
{
}

static void beetle_init(struct task *task, struct task_work *work)
{
    /* @hack spelling: the local steps, where game_state.steps read at each
     * use would do */
    Uint32 steps = game_state.steps;
    struct enemy_work *enemy;
    struct angles ang;
    /* @hack spelling: each sine kept in y, where njSin's result used in place
     * would do */
    Float y;
    /* @hack spelling: the local bearing, where ang.y + QUARTER_TURN at each
     * use would do */
    Angle bearing;
    /* @hack spelling: the local size, where njCnkDirectObjectCompileSize's
     * result passed in place would do */
    Uint32 size;
    /* @hack spelling: the local compiled, where extra passed in place would
     * do */
    NJS_DIRECT_HEAD *compiled;

    enemy = enemy_work_alloc_grouped(task);
    work->submode = njRoundOff(work->scl.x);
    if (work->submode >= BEETLE_KINDS || work->submode < 0) {
        work->submode = BEETLE_FIXED;
    }
    if (work->submode == BEETLE_ELECTRIC ||
        work->submode == BEETLE_MOVING_ELECTRIC) {
        collision_init(task, beetle_shapes, ELECTRIC_SHAPES, COLLISION_ENEMY);
        work->collision->shapes[DISCHARGE_SHAPE].flags |= SHAPE_OFF;
        collision_set_radius(task, ELECTRIC_RADIUS);
    } else if (work->submode == BEETLE_BUMPER) {
        collision_init(task, &beetle_shapes[BUMPER_FIRST], BUMPER_SHAPES,
                       COLLISION_ENEMY);
        collision_set_radius(task, BUMPER_RADIUS);
    } else {
        collision_init(task, beetle_shapes, BODY_SHAPES, COLLISION_ENEMY);
        collision_set_radius(task, BEETLE_RADIUS);
    }
    if (work->submode == BEETLE_ELECTRIC ||
        work->submode == BEETLE_MOVING_ELECTRIC) {
        INTERVAL(enemy) = work->scl.y;
        /* a cycle is INTERVAL + 1 steps of hovering, then DISCHARGE_STEPS + 1
         * of discharge */
        work->timer = ((Uint8)work->ang.x + steps) %
                      (INTERVAL(enemy) + DISCHARGE_STEPS + 2);
        work->scl.y = BOB_HEIGHT;
    } else if (work->submode == BEETLE_BUMPER) {
        BUMP_SPEED(enemy) = (Uint8)work->ang.x;
    } else if (work->submode == BEETLE_GUNNER) {
        SHOTS(enemy) = work->ang.x & SHOT_BITS;
        if (work->ang.x & PASS_BITS) {
            work->flags |= BEETLE_BULLETS_PASS;
        }
        INTERVAL(enemy) = (Uint8)work->ang.z;
        if (INTERVAL(enemy) < INTERVAL_MIN) {
            INTERVAL(enemy) = INTERVAL_MIN;
        }
        work->ang.z &= HIGH_BYTE;
        BULLET_SPEED(enemy) = work->scl.y;
        work->scl.y = BOB_HEIGHT;
        enemy_bullet_load();
        enemy_set_offset(work, enemy);
    } else if (work->submode == BEETLE_BOMBER) {
        BOMB_KIND(enemy) = work->ang.x;
        if (BOMB_KIND(enemy) >= BOMB_KINDS || BOMB_KIND(enemy) < 0) {
            BOMB_KIND(enemy) = 0;
        }
        CIRCLE_START(enemy) = work->ang.y & HIGH_BYTE;
        INTERVAL(enemy) = work->scl.y;
        work->scl.y = BOB_HEIGHT;
        enemy_bomb_load();
    } else if (work->submode == BEETLE_WARP) {
        WARP_SCALE(enemy) = 0.0F;
        work->ang.z &= ~ASKS_DIRECT_DRAW;
    }
    /* @hack spelling: the mask written out, where &= would do */
    work->ang.x = work->ang.x & HIGH_BYTE;
    enemy_motion_step(&enemy->motion);
    enemy_light_load_texture();
    enemy->sight_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->sight_angle = QUARTER_TURN;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = SPEED;
    enemy->turn_speed = TURN_SPEED;
    /* a moving beetle's bottom reaches down to the floor below its start.
     * beetle_move then keeps the beetle at about that height above the
     * floor */
    if (work->submode == BEETLE_MOVING ||
        work->submode == BEETLE_MOVING_ELECTRIC) {
        y = surface_probe_floor(work->pos.x, work->pos.y, work->pos.z, &ang);
        if (y == NO_FLOOR) {
            enemy->bottom = FLOORLESS_BOTTOM;
        } else {
            enemy->bottom = y - work->pos.y;
        }
    }
    BOB_ANGLE(enemy) = (steps * work->ang.z) + work->ang.x;
    enemy_set_home(work, enemy);
    switch (work->submode) {
    case BEETLE_FIXED:
    case BEETLE_ELECTRIC:
    case BEETLE_GUNNER:
    case BEETLE_BUMPER:
        y = njSin((steps * work->ang.z) + work->ang.x);
        work->pos.y = (y * work->scl.y) + enemy->home.y;
        break;
    case BEETLE_WARP:
        y = njSin(work->ang.x);
        work->pos.y = (y * work->scl.y) + enemy->home.y;
        break;
    case BEETLE_BOMBER:
        work->ang.y = (work->ang.z * steps) + CIRCLE_START(enemy);
        bearing = work->ang.y + QUARTER_TURN;
        work->pos.x = enemy->home.x + (work->scl.z * njCos(bearing));
        work->pos.z = enemy->home.z - (work->scl.z * njSin(bearing));
        /* fall through */
    case BEETLE_MOVING:
    case BEETLE_MOVING_ELECTRIC:
        y = njSin(steps * work->ang.z);
        work->pos.y = (y * work->scl.y) + enemy->home.y;
        break;
    }
    enemy->motion.table = beetle_actions;
    if (work->submode == BEETLE_WARP) {
        work->mode = MODE_HIDDEN;
        enemy->motion.object = beetle_object;
        enemy->motion.next = BEETLE_ACTION_HOVER;
        work->collision->shapes->flags |= SHAPE_OFF;
    } else if (work->submode == BEETLE_GUNNER) {
        work->mode = MODE_GUNNER;
        enemy->motion.object = gunner_object;
        enemy->motion.next = BEETLE_ACTION_GUNNER;
    } else if (work->submode == BEETLE_BOMBER) {
        work->mode = MODE_CIRCLE;
        enemy->motion.object = bomber_object;
        enemy->motion.next = BEETLE_ACTION_BOMBER;
    } else if (work->submode == BEETLE_BUMPER) {
        work->mode = MODE_HOVER;
        enemy->motion.object = bumper_object;
        enemy->motion.next = BEETLE_ACTION_BUMPER;
    } else if (work->submode == BEETLE_ELECTRIC) {
        if (work->timer > INTERVAL(enemy)) {
            work->collision->shapes[DISCHARGE_SHAPE].flags &= ~SHAPE_OFF;
            work->mode = MODE_DISCHARGE;
            work->timer -= INTERVAL(enemy) + 1;
        } else {
            work->mode = MODE_HOVER;
        }
        enemy->motion.object = beetle_object;
        enemy->motion.next = BEETLE_ACTION_HOVER;
    } else {
        work->mode = MODE_HOVER;
        enemy->motion.object = beetle_object;
        enemy->motion.next = BEETLE_ACTION_HOVER;
    }
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = beetle_draw;
    task->destructor = beetle_destroy;
    if (work->submode == BEETLE_GUNNER) {
        if (gunner_textures == NULL) {
            texture_load_pvm("E_G_KUMITEX", &gunner_texlist);
            gunner_textures = task_create(0, 0, textures_step, "ManTex");
            gunner_textures->destructor = gunner_textures_release;
            njSetTexture(&gunner_texlist);
            light_apply(BEETLE_LIGHT);
            njFogEnable();
            njSetDirectClip();
            size = njCnkDirectObjectCompileSize(gunner_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((gunner_textures->extra.p = syCalloc(1, size)) != NULL) {
                compiled = gunner_textures->extra.p;
                njCnkDirectObjectCompile(gunner_object, compiled);
            }
            light_apply(light_after_draw);
        }
    } else if (work->submode == BEETLE_BOMBER) {
        if (bomber_textures == NULL) {
            texture_load_pvm("E_B_KUMITEX", &bomber_texlist);
            bomber_textures = task_create(0, 0, textures_step, "ManTex");
            bomber_textures->destructor = bomber_textures_release;
        }
    } else if (work->submode == BEETLE_BUMPER) {
        if (bumper_textures == NULL) {
            texture_load_pvm("E_S_KUMITEX", &bumper_texlist);
            bumper_textures = task_create(0, 0, textures_step, "ManTex");
            bumper_textures->destructor = bumper_textures_release;
        }
    } else {
        if ((work->submode == BEETLE_ELECTRIC ||
             work->submode == BEETLE_MOVING_ELECTRIC) &&
            electric_textures == NULL) {
            texture_load_pvm("E_E_KUMITEX", &electric_texlist);
            electric_textures = task_create(0, 0, textures_step, "ManTex");
            electric_textures->destructor = electric_textures_release;
        }
        if (beetle_textures == NULL) {
            texture_load_pvm("E_KUMITEX", &beetle_texlist);
            beetle_textures = task_create(0, 0, textures_step, "ManTex");
            beetle_textures->destructor = beetle_textures_release;
            njSetTexture(&beetle_texlist);
            light_apply(BEETLE_LIGHT);
            njFogEnable();
            njSetDirectClip();
            size = njCnkDirectObjectCompileSize(beetle_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((beetle_textures->extra.p = syCalloc(1, size)) != NULL) {
                compiled = beetle_textures->extra.p;
                njCnkDirectObjectCompile(beetle_object, compiled);
            }
            light_apply(light_after_draw);
        }
    }
    if (enemy->motion.object == beetle_object &&
        (work->ang.z & ASKS_DIRECT_DRAW) && beetle_textures != NULL &&
        beetle_textures->extra.p != NULL) {
        work->flags |= BEETLE_DIRECT_DRAW;
    }
    if (enemy->motion.object == gunner_object && gunner_textures != NULL &&
        gunner_textures->extra.p != NULL) {
        work->flags |= BEETLE_DIRECT_DRAW;
    }
}

static void beetle_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & BEETLE_AWAY) {
        enemy_turn_to_home(work, enemy);
    } else if (work->flags & BEETLE_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}

static void beetle_mark_away(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local distance, where enemy_home_distance2's
     * result compared in place would do */
    Float distance;

    if (game_state.time_stopped) {
        return;
    }
    distance = enemy_home_distance2(work, enemy);
    if (distance > enemy->near_distance2) {
        work->flags |= BEETLE_AWAY;
    } else {
        work->flags &= ~BEETLE_AWAY;
    }
}

/* a player's touch defeats a BEETLE_MOVING beetle, as a hit does. a bumper
 * takes no hit in a step where it throws a player */
static void beetle_check_hit(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    int hit;
    struct task *other;
    int player;
    int score;
    NJS_VECTOR scl;

    hit = 0;
    if (work->submode == BEETLE_MOVING && collision_next_player(task)) {
        hit = 1;
    }
    if (work->submode == BEETLE_BUMPER &&
        (other = collision_next_player_by_shape(task, THROW_SHAPE)) != NULL) {
        player = player_slot_of(other);
        player_bounce(player, 0.0F, BUMP_SPEED(enemy), 0.0F);
        enemy->motion.next = BEETLE_ACTION_BUMPER_BUMP;
        sound_play(BUMP_SOUND, NULL, 0, 0);
        vibration_start(player, VIBRATION_POWER, VIBRATION_FREQ,
                        VIBRATION_UNTIMED);
        /* @hack spelling: hit compared with 1, where hit alone would do */
    } else if (enemy_take_hit(work, enemy) || hit == 1) {
        work->mode = MODE_DEFEATED;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        if (work->submode == BEETLE_GUNNER) {
            enemy_break(work, gunner_pieces, &gunner_texlist);
        } else if (work->submode == BEETLE_BOMBER) {
            enemy_break(work, bomber_pieces, &bomber_texlist);
        } else if (work->submode == BEETLE_BUMPER) {
            enemy_break(work, bumper_pieces, &bumper_texlist);
        } else {
            enemy_break(work, beetle_pieces, &beetle_texlist);
        }
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
        if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
            /* @hack spelling: player set to shot_by, where shot_by passed at
             * each use would do */
            player = enemy->shot_by;
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            if (work->submode == BEETLE_GUNNER) {
                enemy_ball_create(player, gunner_object, &gunner_texlist, &scl,
                                  NULL);
            } else if (work->submode == BEETLE_BOMBER) {
                enemy_ball_create(player, bomber_object, &bomber_texlist, &scl,
                                  NULL);
            } else if (work->submode == BEETLE_BUMPER) {
                enemy_ball_create(player, bumper_object, &bumper_texlist, &scl,
                                  NULL);
            } else {
                enemy_ball_create(player, beetle_object, &beetle_texlist, &scl,
                                  NULL);
            }
        }
    }
}

static void beetle_mark_player_near(struct task_work *work,
                                    struct enemy_work *enemy)
{
    if (work->submode == BEETLE_GUNNER) {
        if (enemy_find_player_near_offset(work, enemy)) {
            work->flags |= BEETLE_ALERT;
        } else {
            work->flags &= ~BEETLE_ALERT;
        }
        /* @hack spelling: the gunner's arm ended by a return, where an else
         * for the other kinds would do */
        return;
    }
    if (enemy_find_player_near(work, enemy)) {
        work->flags |= BEETLE_ALERT;
    } else {
        work->flags &= ~BEETLE_ALERT;
    }
}

/* spd.x lies along ang.y, and spd.z across it. the beetle sinks while its
 * bottom lies more than twice scl.y, its bob's whole swing, above the floor */
static void beetle_move(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local steps, where game_state.steps read at each
     * use would do */
    Uint32 steps;
    /* @hack spelling: the local rise, where the difference of the sines used
     * in place would do */
    Float rise;

    if (game_state.time_stopped) {
        return;
    }
    enemy->spd.x += ACCEL(enemy).x;
    enemy->spd.z += ACCEL(enemy).z;
    if (enemy->spd.x > SPEED_MAX) {
        enemy->spd.x = SPEED_MAX;
    } else if (enemy->spd.x < SPEED_MIN) {
        enemy->spd.x = SPEED_MIN;
    }
    steps = game_state.steps;
    rise = njSin(steps * work->ang.z) - njSin((steps - 1) * work->ang.z);
    work->pos.y += rise * work->scl.y;
    work->pos.x += (enemy->spd.x * njCos(work->ang.y)) +
                   (enemy->spd.z * njSin(work->ang.y));
    work->pos.y += enemy->spd.y;
    work->pos.z += (-enemy->spd.x * njSin(work->ang.y)) +
                   (enemy->spd.z * njCos(work->ang.y));
    enemy_meet_stage(work, enemy);
    if (enemy->probe.pos.y + enemy->bottom - (work->scl.y * 2.0F) >
        enemy->probe.sides[SURFACE_NEG_Y].pos) {
        enemy->spd.y -= SINK_STEP;
        if (enemy->spd.y < SINK_MAX) {
            enemy->spd.y = SINK_MAX;
        }
    } else {
        enemy->spd.y *= FALL_DAMPING;
        if (njAbs(enemy->spd.y) < FALL_STOP) {
            enemy->spd.y = 0.0F;
        }
    }
    enemy_keep_near_home(work, enemy, work->scl.z + RANGE);
}

static void beetle_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local y, where njSin's result used in place would
     * do */
    Float y;

    y = njSin((game_state.steps * work->ang.z) + work->ang.x);
    work->pos.y = (y * work->scl.y) + enemy->home.y;
    BOB_ANGLE(enemy) += work->ang.z;
}

static void warp_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local y, where njSin's result used in place, ahead
     * of the count's step, would do */
    Float y;

    y = njSin((BOB_STEPS(enemy) * work->ang.z) + work->ang.x);
    BOB_STEPS(enemy)++;
    work->pos.y = (y * work->scl.y) + enemy->home.y;
    BOB_ANGLE(enemy) += work->ang.z;
}

static void beetle_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* turn spd, a shot's speed along x, to aim from pos at the player nearest
 * the beetle. the aim is the center of that player's first collision shape,
 * within AIM_PITCH and AIM_YAW */
static void gunner_aim(struct task_work *work, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    /* @hack spelling: the locals ax and ay, where angle_approach's results
     * passed in place would do */
    Angle ax;
    Angle ay;
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: the local dy, where the difference used in place would
     * do */
    Float dy;
    Angle pitch;
    Angle yaw;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    dy = target.y - pos->y;
    if (distance != 0.0F) {
        pitch = asinf(dy / distance) * ANGLE_PER_RADIAN;
        yaw = atan2f(pos->z - target.z, target.x - pos->x) * ANGLE_PER_RADIAN;
    } else {
        yaw = work->ang.y;
        pitch = 0;
    }
    ax = angle_approach(-AIM_DIP, pitch, AIM_PITCH);
    ay = angle_approach(work->ang.y, yaw, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, ay);
    njRotateZ(NULL, ax);
    njCalcVector(NULL, spd, spd);
    njPopMatrixEx();
}

/* in a gunner's work, the timer counts the steps a player has stayed in
 * range. the tick counts the shots of the burst */
static void beetle_hover(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    beetle_mark_away(work, enemy);
    beetle_mark_player_near(work, enemy);
    beetle_turn(work, enemy);
    if (work->submode == BEETLE_MOVING ||
        work->submode == BEETLE_MOVING_ELECTRIC ||
        work->submode == BEETLE_BOMBER) {
        beetle_move(work, enemy);
    } else if (work->submode == BEETLE_WARP) {
        warp_bob(work, enemy);
    } else {
        beetle_bob(work, enemy);
    }
    beetle_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->submode == BEETLE_ELECTRIC ||
        work->submode == BEETLE_MOVING_ELECTRIC) {
        if (work->timer > INTERVAL(enemy)) {
            work->mode = MODE_DISCHARGE;
            work->timer = 0;
            work->collision->shapes[DISCHARGE_SHAPE].flags &= ~SHAPE_OFF;
        }
    } else if (work->submode == BEETLE_GUNNER) {
        if (SHOTS(enemy) > 0) {
            if (!(work->flags & BEETLE_ALERT)) {
                work->timer = 0;
                work->tick = 0;
            } else if (work->timer > INTERVAL(enemy)) {
                if (enemy->motion.action != BEETLE_ACTION_GUNNER_FIRE) {
                    enemy->motion.next = BEETLE_ACTION_GUNNER_FIRE;
                    return;
                }
                if (enemy->motion.frame >= FIRE_FRAME &&
                    enemy->motion.frame < FIRE_FRAME_END) {
                    pos.x = MUZZLE_X;
                    pos.y = MUZZLE_Y;
                    pos.z = 0.0F;
                    njPushMatrix(&_nj_unit_matrix_);
                    njTranslateEx(&work->pos);
                    njRotateY(NULL, work->ang.y);
                    njCalcPoint(NULL, &pos, &pos);
                    njPopMatrixEx();
                    spd.x = BULLET_SPEED(enemy);
                    spd.y = 0.0F;
                    spd.z = 0.0F;
                    gunner_aim(work, &pos, &spd);
                    enemy_bullet_fire(
                        task, !(work->flags & BEETLE_BULLETS_PASS), &spd, &pos);
                    work->tick++;
                }
                if (work->tick >= SHOTS(enemy)) {
                    work->timer = 0;
                    work->tick = 0;
                }
            }
        }
    }
}

/* a discharge never ends where INTERVAL is 0 */
static void beetle_discharge(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    beetle_bob(work, enemy);
    beetle_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
        DISCHARGE_TEXLIST(enemy)++;
        if (DISCHARGE_TEXLIST(enemy) >= BEETLE_DISCHARGE_FRAMES) {
            DISCHARGE_TEXLIST(enemy) = 0;
        }
        DISCHARGE_FRAME(enemy)++;
        if (work->timer & 1) {
            prominence_create(task, &work->pos, PROMINENCE_RADIUS);
        }
        sound_start_at_for(DISCHARGE_SOUND, work, 1, DISCHARGE_VOLUME,
                           DISCHARGE_SOUND_DURATION, &work->pos);
    }
    /* @hack spelling: one test inside the other, where && would do */
    if (INTERVAL(enemy) != 0) {
        if (work->timer > DISCHARGE_STEPS) {
            work->mode = MODE_HOVER;
            work->timer = 0;
            work->collision->shapes[DISCHARGE_SHAPE].flags |= SHAPE_OFF;
        }
    }
}

static void bomber_circle(struct task *task, struct task_work *work,
                          struct enemy_work *enemy)
{
    NJS_POINT3 pos;

    beetle_bob(work, enemy);
    if (!game_state.time_stopped) {
        work->pos.x =
            enemy->home.x + (work->scl.z * njCos(work->ang.y + QUARTER_TURN));
        work->pos.z =
            enemy->home.z - (work->scl.z * njSin(work->ang.y + QUARTER_TURN));
        work->ang.y = (work->ang.z * game_state.steps) + CIRCLE_START(enemy);
    }
    beetle_check_hit(task, work, enemy);
    if (work->mode == MODE_DEFEATED) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > INTERVAL(enemy)) {
        pos.x = 0.0F;
        pos.y = BOMB_DROP_Y;
        pos.z = 0.0F;
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        njCalcPoint(NULL, &pos, &pos);
        njPopMatrixEx();
        enemy_bomb_create(NULL, BOMB_KIND(enemy), BOMB_DROPPED, &pos);
        work->timer = 0;
    }
}

static void warp_wait(struct task_work *work, struct enemy_work *enemy)
{
    beetle_mark_player_near(work, enemy);
    beetle_mark_away(work, enemy);
    if (work->flags & BEETLE_ALERT) {
        work->mode = MODE_APPEAR;
        WARP_COLOR(enemy).a = WARP_UNSEEN_ALPHA;
        WARP_COLOR(enemy).r = 1.0F;
        WARP_COLOR(enemy).g = 1.0F;
        WARP_COLOR(enemy).b = 1.0F;
        /* @hack spelling: the mask written out, where &= would do */
        work->flags = work->flags & ~BEETLE_ALERT;
        sound_play_at(WARP_SOUND, NULL, 0, SOUND_LOUDEST, &work->pos);
    }
    beetle_turn(work, enemy);
}

static void warp_appear(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the turn added negated, where -= would do */
    work->ang.y += -WARP_COLOR(enemy).a * WARP_SPIN;
    WARP_SCALE(enemy) =
        float_approach(WARP_SCALE(enemy), 1.0F, WARP_SCALE_STEP);
    enemy->shadow_size = WARP_SCALE(enemy) * SHADOW_SIZE;
    if (WARP_COLOR(enemy).a < 0.0F) {
        WARP_COLOR(enemy).a =
            float_approach(WARP_COLOR(enemy).a, 0.0F, WARP_ALPHA_STEP);
    } else if (WARP_COLOR(enemy).r > 0.0F) {
        WARP_COLOR(enemy).r =
            float_approach(WARP_COLOR(enemy).r, 0.0F, WARP_TINT_STEP);
        /* @hack spelling: one assignment to g and b, where a statement for
         * each would do */
        WARP_COLOR(enemy).b = WARP_COLOR(enemy).g = WARP_COLOR(enemy).r;
    } else {
        work->mode = MODE_HOVER;
        work->timer = 0;
        work->collision->shapes->flags &= ~SHAPE_OFF;
    }
}

static void warp_vanish(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the turn subtracted negated, where += would do */
    work->ang.y -= -WARP_COLOR(enemy).a * WARP_SPIN;
    if (WARP_COLOR(enemy).a <= WARP_NARROW_ALPHA) {
        WARP_SCALE(enemy) =
            float_approach(WARP_SCALE(enemy), 0.0F, WARP_SCALE_STEP);
        enemy->shadow_size = WARP_SCALE(enemy) * SHADOW_SIZE;
    }
    if (WARP_COLOR(enemy).r < 1.0F) {
        WARP_COLOR(enemy).r =
            float_approach(WARP_COLOR(enemy).r, 1.0F, WARP_TINT_STEP);
        /* @hack spelling: one assignment to g and b, where a statement for
         * each would do */
        WARP_COLOR(enemy).b = WARP_COLOR(enemy).g = WARP_COLOR(enemy).r;
    } else if (WARP_COLOR(enemy).a > WARP_UNSEEN_ALPHA) {
        WARP_COLOR(enemy).a = float_approach(
            WARP_COLOR(enemy).a, WARP_UNSEEN_ALPHA, WARP_ALPHA_STEP);
    } else {
        work->mode = MODE_DEFEATED;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

/* set home to pos, and raise home's y by the bob's offset at this step.
 * nothing calls beetle_rehome. it isn't static, since SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void beetle_rehome(struct task_work *work, struct enemy_work *enemy)
{
    int kind;
    /* @hack spelling: the local sine, where njSin's result used in place
     * would do */
    Float sine;
    Float scale;

    kind = njRoundOff(work->scl.x);
    sine = njSin((game_state.steps * work->ang.z) + work->ang.x);
    if (kind == BEETLE_ELECTRIC || kind == BEETLE_MOVING_ELECTRIC ||
        kind == BEETLE_GUNNER || kind == BEETLE_BOMBER) {
        scale = BOB_HEIGHT;
    } else {
        scale = work->scl.y;
    }
    enemy->home.x = work->pos.x;
    enemy->home.y = (scale * sine) + work->pos.y;
    enemy->home.z = work->pos.z;
    if (!game_state.time_stopped) {
        BOB_ANGLE(enemy) += work->ang.z;
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

static void beetle_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    int kind;
    int bomb;
    NJS_DIRECT_HEAD *compiled;
    struct angles ang;
    NJS_VECTOR scl;
    NJS_POINT3 pos;
    Float scl_z;

    if (sphere_out_of_view(&work->pos, DRAW_RADIUS)) {
        return;
    }
    light_apply(BEETLE_LIGHT);
    if (work->submode == BEETLE_GUNNER) {
        njSetTexture(&gunner_texlist);
    } else if (work->submode == BEETLE_BOMBER) {
        njSetTexture(&bomber_texlist);
    } else if (work->submode == BEETLE_BUMPER) {
        njSetTexture(&bumper_texlist);
    } else {
        njSetTexture(&beetle_texlist);
    }
    if (work->mode != MODE_HIDDEN) {
        if (work->mode == MODE_APPEAR || work->mode == MODE_VANISH) {
            constant_material_set(WARP_COLOR(enemy).a, WARP_COLOR(enemy).r,
                                  WARP_COLOR(enemy).g, WARP_COLOR(enemy).b);
            beetle_canopy_polygons[BEETLE_CANOPY_LIGHT_STRIP] |= NJD_FST_UA;
            beetle_canopy_polygons[BEETLE_CANOPY_SHELL_STRIP] |= NJD_FST_UA;
            beetle_lower_fin_polygons[BEETLE_LOWER_FIN_STRIP] |= NJD_FST_UA;
            beetle_rotor_polygons[BEETLE_ROTOR_STRIP] |= NJD_FST_UA;
            beetle_body_polygons[BEETLE_BODY_BASE_STRIP] |= NJD_FST_UA;
            beetle_body_polygons[BEETLE_BODY_SHELL_STRIP] |= NJD_FST_UA;
            beetle_rotor_disc_polygons[BEETLE_ROTOR_DISC_STRIP] |= NJD_FST_UA;
        }
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        if (work->submode == BEETLE_WARP) {
            njScale(NULL, WARP_SCALE(enemy), 1.0F, WARP_SCALE(enemy));
        }
        if (work->submode == BEETLE_BOMBER) {
            /* the loop spins while bomber_draw_ready[0] is 0. it holds 1, and
             * nothing writes it */
            while (*bomber_draw_ready == 0) {
            }
        }
        if (work->flags & BEETLE_DIRECT_DRAW) {
            /* @hack spelling: a switch on submode, its last case ending in a
             * break, where an if and else would do */
            switch (work->submode) {
            case BEETLE_GUNNER:
                compiled = gunner_textures->extra.p;
                break;
            default:
                compiled = beetle_textures->extra.p;
                break;
            }
            njSetDirectClip();
            njDirectDrawObject(compiled);
        } else if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            /* @hack spelling: the draw written in both arms, where one under
             * submode != BEETLE_BOMBER || game_state.steps >
             * BOMBER_HIDDEN_STEPS would do */
            if (work->submode == BEETLE_BOMBER) {
                if (game_state.steps > BOMBER_HIDDEN_STEPS) {
                    njCnkSimpleDrawMotion(
                        enemy->motion.object,
                        enemy->motion.table[enemy->motion.action].motion,
                        enemy->motion.frame);
                }
            } else {
                njCnkSimpleDrawMotion(
                    enemy->motion.object,
                    enemy->motion.table[enemy->motion.action].motion,
                    enemy->motion.frame);
            }
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        if (work->mode == MODE_DISCHARGE) {
            njFogDisable();
            njPushMatrixEx();
            texture_anim_apply(&discharge_model, &discharge_anim,
                               DISCHARGE_FRAME(enemy));
            njSetTexture(&discharge_texlists[DISCHARGE_TEXLIST(enemy)]);
            njTranslateEx(&work->pos);
            njCnkEasyDrawObject(&discharge_object);
            njPopMatrixEx();
            njFogEnable();
        }
        if (njFraction(work->scl.x) == 0.0F && enemy->shadow_size > 0.0F) {
            enemy_draw_shadow(work, enemy);
        }
        if (work->mode == MODE_APPEAR || work->mode == MODE_VANISH) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            beetle_canopy_polygons[BEETLE_CANOPY_LIGHT_STRIP] &= ~NJD_FST_UA;
            beetle_canopy_polygons[BEETLE_CANOPY_SHELL_STRIP] &= ~NJD_FST_UA;
            beetle_lower_fin_polygons[BEETLE_LOWER_FIN_STRIP] &= ~NJD_FST_UA;
            beetle_rotor_polygons[BEETLE_ROTOR_STRIP] &= ~NJD_FST_UA;
            beetle_body_polygons[BEETLE_BODY_BASE_STRIP] &= ~NJD_FST_UA;
            beetle_body_polygons[BEETLE_BODY_SHELL_STRIP] &= ~NJD_FST_UA;
            beetle_rotor_disc_polygons[BEETLE_ROTOR_DISC_STRIP] &= ~NJD_FST_UA;
        }
        /* @hack redundant: two stores of 1.0F that no path reaches */
        if (0) {
            work->scl.y = 1.0F;
            work->scl.z = 1.0F;
        }
    } else {
        /* @hack redundant: a readout that no path reaches, which leaves its
         * cases' code and its strings in the object */
        if (0) {
            /* @hack redundant: a push, turn and pop of the matrix, which leave
             * no code */
            njPushMatrixEx();
            njRotateY(NULL, work->ang.y + QUARTER_TURN);
            njPopMatrixEx();
            kind = njRoundOff(work->scl.x);
            if (kind >= BEETLE_KINDS || kind < 0) {
                kind = BEETLE_FIXED;
            }
            /* @hack redundant: an empty test of kind and two prints of its
             * title, which leave no code */
            if (kind) {
            }
            njPrintC(NJM_LOCATION(17, 15), beetle_titles[kind]);
            njPrintC(NJM_LOCATION(17, 21), beetle_titles[kind]);
            switch (kind) {
            case BEETLE_FIXED:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE");
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED/");
                if (work->ang.z & ASKS_DIRECT_DRAW) {
                    njPrintC(NJM_LOCATION(26, 18), "DIRECT DRAW");
                } else {
                    njPrintC(NJM_LOCATION(26, 18), "CHANK DRAW");
                }
                njPrintC(NJM_LOCATION(17, 20), "-> Y WIDTH");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case BEETLE_MOVING:
            case BEETLE_WARP:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE");
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED");
                njPrintC(NJM_LOCATION(17, 20), "-> Y WIDTH");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case BEETLE_ELECTRIC:
            case BEETLE_MOVING_ELECTRIC:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE & DELAY");
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED/");
                njPrintC(NJM_LOCATION(26, 18), (work->ang.z & ASKS_DIRECT_DRAW)
                                                   ? "DIRECT DRAW"
                                                   : "CHANK DRAW");
                njPrintC(NJM_LOCATION(17, 20), "-> INTERVAL(FRAME)");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case BEETLE_GUNNER:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE&");
                njPrintC(NJM_LOCATION(26, 16),
                         (work->ang.x & PASS_BITS) ? "CHP/RAP." : "   /RAP.");
                njPrintD(NJM_LOCATION(34, 16), work->ang.x & SHOT_BITS, 2);
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED & INT. ");
                njPrintD(NJM_LOCATION(33, 18), (Uint8)work->ang.z, 3);
                njPrintC(NJM_LOCATION(37, 18), " FRAMES");
                njPrintC(NJM_LOCATION(17, 20), "-> BULLET SPEED");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE/SCOPE");
                break;
            case BEETLE_BUMPER:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE & BUMP ");
                njPrintD(NJM_LOCATION(34, 16), (Uint8)work->ang.x, 3);
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED");
                njPrintC(NJM_LOCATION(17, 20), "-> Y WIDTH");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            case BEETLE_BOMBER:
                njPrintC(NJM_LOCATION(17, 16), "-> PHASE & BOMB ");
                bomb = (Uint8)work->ang.x;
                if (bomb >= BOMB_KINDS || bomb < 0) {
                    bomb = 0;
                }
                njPrintC(NJM_LOCATION(34, 16), beetle_bomb_names[bomb]);
                njPrintC(NJM_LOCATION(17, 18), "-> SPEED");
                njPrintC(NJM_LOCATION(17, 20), "-> INTERVAL(FRAME)");
                njPrintC(NJM_LOCATION(17, 21), "-> RANGE");
                break;
            }
            njPrintC(NJM_LOCATION(17, 17), "-> ANG Y ");
            njPrintH(NJM_LOCATION(26, 17), work->ang.y & HIGH_BYTE, 4);
            njPrintC(NJM_LOCATION(30, 17), " EME ");
            njPrintD(NJM_LOCATION(35, 17), (Uint8)work->ang.y, 3);
            njPrintC(NJM_LOCATION(17, 19), "-> TYPE(0-7)SHADOW ");
            if (njFraction(work->scl.x) == 0.0F) {
                njPrintC(NJM_LOCATION(36, 19), "ON");
            } else {
                njPrintC(NJM_LOCATION(36, 19), "OFF");
            }
            if (kind == BEETLE_GUNNER) {
                /* enemy_set_offset rounds scl.z toward 0. the readout keeps
                 * scl.z first, and puts it back after */
                scl_z = work->scl.z;
                enemy_set_offset(work, enemy);
                /* @hack spelling: one assignment to the three, where a
                 * statement for each would do */
                ang.x = ang.y = ang.z = 0;
                /* @hack spelling: one assignment to scl.z and scl.x, where a
                 * statement for each would do */
                scl.z = scl.x = njRoundOff(work->scl.z) + RANGE;
                scl.y = 1.0F;
                pos = work->pos;
                njAddVector(&pos, &enemy->offset);
                debug_draw_model_unlit(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
                work->scl.z = scl_z;
            } else {
                /* @hack spelling: one assignment to the three, where a
                 * statement for each would do */
                ang.x = ang.y = ang.z = 0;
                scl.x = work->scl.z + RANGE;
                scl.z = scl.x;
                scl.y = 1.0F;
                debug_draw_model_unlit(&work->pos, &ang, &scl,
                                       DEBUG_MODEL_CYLINDER);
            }
        }
    }
    light_apply(light_after_draw);
}

void beetle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_POINT3 pos;

    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        beetle_init(task, work);
        return;
    case MODE_HOVER:
        beetle_hover(task, work, enemy);
        break;
    case MODE_DEFEATED:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        return;
    case MODE_HIDDEN:
        warp_wait(work, enemy);
        break;
    case MODE_APPEAR:
        warp_appear(work, enemy);
        break;
    case MODE_VANISH:
        warp_vanish(work, enemy);
        if (work->mode != MODE_VANISH) {
            task->draw = NULL;
        }
        break;
    case MODE_DISCHARGE:
        beetle_discharge(task, work, enemy);
        break;
    case MODE_CIRCLE:
        bomber_circle(task, work, enemy);
        break;
    /* @hack spelling: MODE_GUNNER's case written apart, last, where its label
     * beside MODE_HOVER's would do */
    case MODE_GUNNER:
        beetle_hover(task, work, enemy);
        break;
    }
    /* @hack spelling: the empty arm and the else, where one arm under
     * !game_state.time_stopped would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        switch (work->mode) {
        case MODE_HOVER:
        case MODE_DISCHARGE:
        case MODE_CIRCLE:
        case MODE_GUNNER:
            sound_start_at_for(HOVER_SOUND, work, 1, HOVER_VOLUME,
                               HOVER_SOUND_DURATION, &work->pos);
            break;
        case MODE_APPEAR:
        case MODE_VANISH:
            sound_start_at_for(HOVER_SOUND, work, 1,
                               (SOUND_LOUDEST * WARP_COLOR(enemy).a) +
                                   HOVER_VOLUME,
                               HOVER_SOUND_DURATION, &work->pos);
            break;
        }
        enemy_motion_step(&enemy->motion);
        if (work->submode == BEETLE_BUMPER && (work->timer & LAMPS_LIT)) {
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            pos.x = LAMP_X;
            pos.y = LAMP_Y;
            pos.z = -LAMP_Z;
            njCalcPoint(NULL, &pos, &pos);
            enemy_light_create(task, LAMP_LIGHT, &pos);
            pos.x = LAMP_X;
            pos.y = LAMP_Y;
            pos.z = LAMP_Z;
            njCalcPoint(NULL, &pos, &pos);
            enemy_light_create(task, LAMP_LIGHT, &pos);
            njPopMatrixEx();
        }
        if (work->flags & BEETLE_ALERT) {
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            if (work->submode == BEETLE_BUMPER) {
                pos.x = BUMPER_FRONT_LIGHT_X;
                pos.y = BUMPER_FRONT_LIGHT_Y;
                pos.z = 0.0F;
            } else {
                pos.x = FRONT_LIGHT_X;
                pos.y = FRONT_LIGHT_Y;
                pos.z = 0.0F;
            }
            njCalcPoint(NULL, &pos, &pos);
            enemy_light_create(task, FRONT_LIGHT, &pos);
            njPopMatrixEx();
        }
    }
    if (work->mode != MODE_DEFEATED) {
        collision_register(task);
    }
    /* @hack redundant: a call of beetle_init that no path reaches, under a
     * float test */
    if (0.0F) {
        switch (work->mode) {
        case MODE_INIT:
            beetle_init(task, work);
            break;
        }
    }
}
