/* the path enemies, E PATH in a stage's object list. each flies along one
 * of the stage's paths and fires as it goes.
 *
 * the functions here push at most one matrix at a time on Ninja's matrix
 * stack. the stack must have room for it, which no function here checks */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/light.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_bomb.h"
#include "shared/enemy_bullet.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_light.h"
#include "shared/enemy_route.h"
#include "shared/path_enemy.h"
#include "shared/path_enemy_art.h"
#include "stage/path.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* an enemy's modes, in its work's mode */
enum path_enemy_mode {
    MODE_INIT,
    MODE_WAIT, /* unseen, until a player comes in range */
    MODE_RIDE,
    MODE_FLY_ON, /* flying straight on from the path's last segment */
    MODE_HOVER, /* at the path's last segment, under PATH_ENEMY_STOP */
    MODE_HIT,
    MODE_GONE
};

/* ang.x holds the kind, the PATH_ENEMY_ flags and the enemy's PATH_INDEX.
 * the enemy keeps the flags in its work's tick */
#define KIND_BITS 0xf
#define PATH_FLAGS_BITS 0xff0
#define PATH_FLAGS_SHIFT 4
#define PATH_INDEX_BITS 0xf000
#define PATH_INDEX_SHIFT 12
#define PATH_ENEMY_KIND(work) ((work)->ang.x & KIND_BITS)
#define PATH_ENEMY_FLAGS(work)                                                 \
    (((work)->ang.x & PATH_FLAGS_BITS) >> PATH_FLAGS_SHIFT)
#define PATH_CHOICE(work)                                                      \
    (((work)->ang.x & PATH_INDEX_BITS) >> PATH_INDEX_SHIFT)
/* the points the enemy starts and stops firing at, in ang.z's high and low
 * bytes */
#define FIRE_START_BITS 0xff00
#define FIRE_START_SHIFT 8
#define FIRE_END_BITS 0xff
#define FIRE_START_POINT(work)                                                 \
    (((work)->ang.z & FIRE_START_BITS) >> FIRE_START_SHIFT)
#define FIRE_END_POINT(work) ((work)->ang.z & FIRE_END_BITS)
/* each of scl's fractions, without its sign, gives a setting at these
 * scales */
/* WAKE_RANGE is scl.x's times WAKE_SCALE, plus WAKE_MIN */
#define WAKE_SCALE 400.0F
#define WAKE_MIN 10.0F
/* the index of the point the enemy starts at is scl.y's times START_SCALE */
#define START_SCALE 100.0F
#define START_POINT(work) (njAbs(njFraction((work)->scl.y) * START_SCALE))
/* RELOAD_EXTRA is scl.z's times RELOAD_SCALE */
#define RELOAD_SCALE 100.0F

/* what a path enemy keeps in the fields of its enemy_work that enemy.h leaves
 * to each kind */
/* the distances along the path where the enemy starts and stops firing */
#define FIRE_START(enemy) ((enemy)->unk_00c.x)
#define FIRE_END(enemy) ((enemy)->unk_00c.y)
#define PATH_TURN(enemy) ((enemy)->unk_01c) /* the path's turn about y */
#define BOB_PHASE(enemy) ((enemy)->unk_024)
/* the index of the enemy's path in path_enemy_paths */
#define PATH_INDEX(enemy) ((enemy)->unk_046)
#define RELOAD(enemy) ((enemy)->unk_048) /* steps left before it can fire */
/* the steps the enemy adds to each wait between shots */
#define RELOAD_EXTRA(enemy) ((enemy)->unk_04e)
#define DISTANCE(enemy) ((enemy)->unk_094) /* how far along the path it is */
/* a player nearer the wake point, home plus offset, than WAKE_RANGE wakes
 * the enemy */
#define WAKE_RANGE(enemy) ((enemy)->unk_098)

/* the counts of collision shapes each family takes from path_enemy_shapes,
 * the kumis' first and the kyokos' after them */
#define KUMI_SHAPES 1
#define KYOKO_SHAPES 5
/* every kind's turn_speed, shadow_size and shadow_ratio */
#define TURN_SPEED 0x400
#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.0F

/* each family's speed along the path, a step */
#define KUMI_SPEED 2.5F
#define KYOKO_SPEED 5.0F
/* an enemy under PATH_ENEMY_STOP slows within STOP_DISTANCE of the path's
 * end. its step there is its family's speed times STOP_EASE times the
 * distance left, but no less than STOP_MIN_SPEED */
#define STOP_DISTANCE 50.0F
#define STOP_EASE 0.2F
#define STOP_MIN_SPEED 0.3F
/* an enemy whose PATH_INDEX is LEVEL_PATH never tilts with the path */
#define LEVEL_PATH 4
/* the index of a path's last segment. its count points bound count - 1
 * segments */
#define LAST_SEGMENT(path) ((path)->count - 2)

/* the entries of path_enemy_actions: each kind's own, and the gunkumi's
 * firing */
enum path_enemy_action {
    ACTION_GUNKUMI,
    ACTION_GUNKUMI_FIRE,
    ACTION_BOMBKUMI,
    ACTION_KYOKO,
    ACTION_BACKKYOKO
};
/* a gunkumi fires while its firing action's frame is at least FIRE_FRAME
 * and below FIRE_FRAME_END */
#define FIRE_FRAME 1.0F
#define FIRE_FRAME_END 1.2F
/* the steps after a shot before the next, besides the enemy's RELOAD_EXTRA */
#define GUN_RELOAD 6
#define BOMB_RELOAD 30
#define KYOKO_RELOAD 15

/* the points the enemy's shots, bombs, jets and lights start from, in its
 * own frame. that frame's x points ahead. a shot or a bomb starts at a z of
 * 0. the two jets, and each pair of lights, stand at z and -z */
#define GUNKUMI_MUZZLE_X 7.5F
#define GUNKUMI_MUZZLE_Y (-9.2F)
#define KYOKO_MUZZLE_X 15.6F
#define KYOKO_MUZZLE_Y 11.5F
#define BOMB_DROP_X (-1.2F)
#define BOMB_DROP_Y (-8.1F)
#define KYOKO_JET_X (-18.6F)
#define BACKKYOKO_JET_X 16.6F
#define JET_Y 13.2F
#define JET_Z 13.5F
#define KUMI_LIGHT_X (-3.0F)
#define KUMI_LIGHT_Y 1.9F
#define KUMI_LIGHT_Z 17.8F
#define KYOKO_LIGHT_X (-19.8F)
#define KYOKO_LIGHT_Y 23.1F
#define KYOKO_LIGHT_Z 34.0F
/* a kyoko's third light, between its pair, at KYOKO_LIGHT_Y */
#define KYOKO_CENTER_LIGHT_X (-24.8F)
/* each family's shots' speed, along x before path_enemy_aim turns it */
#define GUNKUMI_SHOT_SPEED 3.2F
#define KYOKO_SHOT_SPEED 5.5F
/* a kyoko's jets leave at its speed less JET_SLOWER */
#define JET_SLOWER 1.8F
/* an enemy's lights glow while its timer has GLOW_BIT: 16 steps on, then 16
 * off */
#define GLOW_BIT 0x10

/* each step, a hovering enemy rises by BOB_RISE times the cosine of its
 * BOB_PHASE. the phase then turns by BOB_TURN. the enemy's ang.x and ang.z
 * each turn toward 0 by at most LEVEL_TURN */
#define BOB_RISE 0.2F
#define BOB_TURN 0x400
#define LEVEL_TURN 0x100

/* path_enemy_aim turns a shot at most AIM_PITCH from level, and at most
 * AIM_YAW from the enemy's heading */
#define AIM_PITCH 0x1800
#define AIM_YAW 0x800

/* the points a hit scores: SCORE_KNOCK where player_touch_attacks returns
 * TRUE, else SCORE */
#define SCORE 500
#define SCORE_KNOCK 1000

#define PATH_ENEMY_PATHS ARRAY_COUNT(path_enemy_paths)

static void path_enemy_destroy(struct task *task);
static void path_enemy_draw(struct task *task);

/* each family's holder: a task that keeps the family's textures loaded, or
 * null while none does. FIRST_MODEL holds the gunkumi's or the kyoko's
 * compiled model. SECOND_MODEL holds the bombkumi's or the backkyoko's. a
 * model is null where the heap had no room for it */
static struct task *kumi_holder = NULL;
static struct task *kyoko_holder = NULL;
#define FIRST_MODEL(holder) ((holder)->extra.p)
#define SECOND_MODEL(holder) ((holder)->force.p)

/* in the work's flags: the holder holds the enemy's compiled model. the draw
 * then draws that model, not the motion */
#define PATH_ENEMY_COMPILED 0x1000
/* the light the enemies draw in, and the holders compile their models in */
#define PATH_ENEMY_LIGHT 8

/* the triangle path_enemy_draw_route draws at each point of a path */
static NJS_POLYGON_VTX route_triangle[3] = {
#include "shared/path_enemy/route_triangle.inc"
};

static void kumi_holder_destroy(struct task *task)
{
    njReleaseTexture(&path_kumi_texlist);
    kumi_holder = NULL;
}

static void kyoko_holder_destroy(struct task *task)
{
    njReleaseTexture(&path_kyoko_texlist);
    kyoko_holder = NULL;
}

static void holder_step(struct task *task)
{
}

static void path_enemy_set_route(struct task_work *work,
                                 struct enemy_work *enemy,
                                 struct enemy_route *route)
{
    /* @hack spelling: point, a local for each point's index, where the index
     * passed in place would do */
    unsigned int point;

    PATH_INDEX(enemy) = PATH_CHOICE(work);
    if ((Uint32)PATH_INDEX(enemy) >= PATH_ENEMY_PATHS ||
        PATH_INDEX(enemy) < 0) {
        PATH_INDEX(enemy) = 0;
    }
    route->path = path_enemy_paths[PATH_INDEX(enemy)];
    work->tick = PATH_ENEMY_FLAGS(work);
    point = FIRE_START_POINT(work);
    if (path_point_distance(route->path, point, &FIRE_START(enemy)) == 0) {
        FIRE_START(enemy) = 0.0F;
    }
    point = FIRE_END_POINT(work);
    if (path_point_distance(route->path, point, &FIRE_END(enemy)) == 0) {
        FIRE_END(enemy) = 0.0F;
    }
    point = START_POINT(work);
    if (path_point_distance(route->path, point, &DISTANCE(enemy)) == 0) {
        DISTANCE(enemy) = 0.0F;
    }
}

static void path_enemy_init(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy;
    struct enemy_route *route;
    Uint32 size;

    enemy = enemy_work_alloc(task);
    work->submode = PATH_ENEMY_KIND(work);
    if (work->submode >= PATH_ENEMY_KINDS || work->submode < 0) {
        work->submode = PATH_ENEMY_GUNKUMI;
    }
    route = syCalloc(1, sizeof(struct enemy_route));
    task->extra.p = route;
    path_enemy_set_route(work, enemy, route);
    switch (work->submode) {
    case PATH_ENEMY_GUNKUMI:
    case PATH_ENEMY_BOMBKUMI:
        collision_init(task, &path_enemy_shapes[0], KUMI_SHAPES,
                       COLLISION_ENEMY);
        break;
    case PATH_ENEMY_KYOKO:
    case PATH_ENEMY_BACKKYOKO:
        collision_init(task, &path_enemy_shapes[KUMI_SHAPES], KYOKO_SHAPES,
                       COLLISION_ENEMY);
        break;
    }
    enemy->offset = work->scl;
    WAKE_RANGE(enemy) = njAbs(njFraction(work->scl.x) * WAKE_SCALE) + WAKE_MIN;
    PATH_TURN(enemy) = work->ang.y & PATH_TURN_BITS;
    /* @hack spelling: one assignment to both angles, where a statement for
     * each, in either order, would do */
    work->ang.x = work->ang.z = 0;
    enemy->turn_speed = TURN_SPEED;
    RELOAD_EXTRA(enemy) = njAbs(njFraction(work->scl.z) * RELOAD_SCALE);
    enemy_set_home(work, enemy);
    work->mode = MODE_WAIT;
    enemy->motion.table = path_enemy_actions;
    /* @hack spelling: the shadow's size and ratio set in each case, where
     * once after the switch would do */
    switch (work->submode) {
    case PATH_ENEMY_GUNKUMI:
        enemy->motion.object = path_gunkumi_object;
        enemy->motion.next = ACTION_GUNKUMI;
        enemy->shadow_size = SHADOW_SIZE;
        enemy->shadow_ratio = SHADOW_RATIO;
        enemy_bullet_load();
        break;
    case PATH_ENEMY_BOMBKUMI:
        enemy->motion.object = path_bombkumi_object;
        enemy->motion.next = ACTION_BOMBKUMI;
        enemy->shadow_size = SHADOW_SIZE;
        enemy->shadow_ratio = SHADOW_RATIO;
        enemy_bomb_load();
        break;
    case PATH_ENEMY_KYOKO:
        enemy->motion.object = path_kyoko_object;
        enemy->motion.next = ACTION_KYOKO;
        enemy->shadow_size = SHADOW_SIZE;
        enemy->shadow_ratio = SHADOW_RATIO;
        enemy_bullet_load();
        break;
    case PATH_ENEMY_BACKKYOKO:
        enemy->motion.object = path_backkyoko_object;
        enemy->motion.next = ACTION_BACKKYOKO;
        enemy->shadow_size = SHADOW_SIZE;
        enemy->shadow_ratio = SHADOW_RATIO;
        enemy_bullet_load();
        break;
    }
    enemy_motion_step(&enemy->motion);
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = path_enemy_draw;
    task->destructor = path_enemy_destroy;
    enemy_jet_load_texture();
    enemy_light_load_texture();
    /* @hack spelling: one test inside the other, where the kind's test &&
     * kumi_holder == NULL would do */
    if (work->submode == PATH_ENEMY_GUNKUMI ||
        work->submode == PATH_ENEMY_BOMBKUMI) {
        if (kumi_holder == NULL) {
            texture_load_pvm("E_PATHKUMITEX", &path_kumi_texlist);
            kumi_holder = task_create(0, 0, holder_step, "ManTex");
            kumi_holder->destructor = kumi_holder_destroy;
            njSetTexture(&path_kumi_texlist);
            light_apply(PATH_ENEMY_LIGHT);
            njFogEnable();
            njSetDirectClip();
            size = njCnkDirectObjectCompileSize(path_gunkumi_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((FIRST_MODEL(kumi_holder) = syCalloc(1, size)) != NULL) {
                /* @hack spelling: buffer, a local for the record just
                 * allocated, where the record read in place would do */
                void *buffer = FIRST_MODEL(kumi_holder);
                njCnkDirectObjectCompile(path_gunkumi_object, buffer);
            }
            size = njCnkDirectObjectCompileSize(path_bombkumi_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((SECOND_MODEL(kumi_holder) = syCalloc(1, size)) != NULL) {
                /* @hack spelling: buffer, a local for the record just
                 * allocated, where the record read in place would do */
                void *buffer = SECOND_MODEL(kumi_holder);
                njCnkDirectObjectCompile(path_bombkumi_object, buffer);
            }
        }
    }
    /* @hack spelling: one test inside the other, where the kind's test &&
     * kyoko_holder == NULL would do */
    if (work->submode == PATH_ENEMY_KYOKO ||
        work->submode == PATH_ENEMY_BACKKYOKO) {
        if (kyoko_holder == NULL) {
            texture_load_pvm("E_PATHKYOKOTEX", &path_kyoko_texlist);
            kyoko_holder = task_create(0, 0, holder_step, "ManTex");
            kyoko_holder->destructor = kyoko_holder_destroy;
            njSetTexture(&path_kyoko_texlist);
            light_apply(PATH_ENEMY_LIGHT);
            njFogEnable();
            njSetDirectClip();
            size = njCnkDirectObjectCompileSize(path_kyoko_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((FIRST_MODEL(kyoko_holder) = syCalloc(1, size)) != NULL) {
                /* @hack spelling: buffer, a local for the record just
                 * allocated, where the record read in place would do */
                void *buffer = FIRST_MODEL(kyoko_holder);
                njCnkDirectObjectCompile(path_kyoko_object, buffer);
            }
            size = njCnkDirectObjectCompileSize(path_backkyoko_object);
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((SECOND_MODEL(kyoko_holder) = syCalloc(1, size)) != NULL) {
                /* @hack spelling: buffer, a local for the record just
                 * allocated, where the record read in place would do */
                void *buffer = SECOND_MODEL(kyoko_holder);
                njCnkDirectObjectCompile(path_backkyoko_object, buffer);
            }
        }
    }
    switch (work->submode) {
    case PATH_ENEMY_GUNKUMI:
        if (kumi_holder != NULL && FIRST_MODEL(kumi_holder) != NULL) {
            work->flags |= PATH_ENEMY_COMPILED;
        }
        break;
    case PATH_ENEMY_BOMBKUMI:
        if (kumi_holder != NULL && SECOND_MODEL(kumi_holder) != NULL) {
            work->flags |= PATH_ENEMY_COMPILED;
        }
        break;
    case PATH_ENEMY_KYOKO:
        if (kyoko_holder != NULL && FIRST_MODEL(kyoko_holder) != NULL) {
            work->flags |= PATH_ENEMY_COMPILED;
        }
        break;
    case PATH_ENEMY_BACKKYOKO:
        if (kyoko_holder != NULL && SECOND_MODEL(kyoko_holder) != NULL) {
            work->flags |= PATH_ENEMY_COMPILED;
        }
        break;
    }
}

static void path_enemy_check_hit(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    int score;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        switch (work->submode) {
        case PATH_ENEMY_GUNKUMI:
            enemy_break(work, path_gunkumi_pieces, &path_kumi_texlist);
            break;
        case PATH_ENEMY_BOMBKUMI:
            enemy_break(work, path_bombkumi_pieces, &path_kumi_texlist);
            break;
        case PATH_ENEMY_KYOKO:
            enemy_break(work, path_kyoko_pieces, &path_kyoko_texlist);
            break;
        case PATH_ENEMY_BACKKYOKO:
            enemy_break(work, path_backkyoko_pieces, &path_kyoko_texlist);
            break;
        }
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
    }
}

/* move the enemy a step along its path, and face it as its kind faces. at
 * the path's last segment the enemy leaves the path for MODE_HOVER under
 * PATH_ENEMY_STOP, and for MODE_FLY_ON otherwise. spd holds the enemy's
 * last step on the path, and a flying enemy moves by it each step */
static void path_enemy_move(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    struct enemy_route *route;
    struct path *path;
    Float speed;
    unsigned int index;
    struct path_sample sample;
    /* @hack redundant: unused, an array nothing reads, which sizes the
     * frame */
    NJS_VECTOR unused[4];
    NJS_VECTOR dir;
    int player;
    /* @hack spelling: add, a local for njAddVector, where njAddVector called
     * in place would do */
    void (*add)(NJS_VECTOR *vd, NJS_VECTOR *vs);

    if (game_state.time_stopped) {
        return;
    }
    add = njAddVector;
    if (work->mode == MODE_RIDE) {
        route = task->extra.p;
        path = route->path;
#ifdef NONMATCHING
        /* the switch below sets speed: it has a case for each kind the
         * submode can hold. GCC can't see that the submode holds no other */
        speed = KUMI_SPEED;
#endif
        switch (work->submode) {
        case PATH_ENEMY_GUNKUMI:
        case PATH_ENEMY_BOMBKUMI:
            speed = KUMI_SPEED;
            break;
        case PATH_ENEMY_KYOKO:
        case PATH_ENEMY_BACKKYOKO:
            speed = KYOKO_SPEED;
            break;
        }
        if (work->tick & PATH_ENEMY_STOP) {
            if (DISTANCE(enemy) >= path->length - STOP_DISTANCE) {
                speed = speed * (path->length - DISTANCE(enemy)) * STOP_EASE;
                if (speed < STOP_MIN_SPEED) {
                    speed = STOP_MIN_SPEED;
                }
            }
        }
        DISTANCE(enemy) += speed;
        sample.distance = DISTANCE(enemy);
        /* @hack spelling: the empty then-arm and its else, where the test
         * negated would do */
        if (path_segment_at(path, DISTANCE(enemy), &index) != 0 &&
            index < (unsigned int)LAST_SEGMENT(path) &&
            path_get_sample(path, &sample) != 0) {
        } else {
            work->mode =
                (work->tick & PATH_ENEMY_STOP) ? MODE_HOVER : MODE_FLY_ON;
            add(&work->pos, &enemy->spd);
            return;
        }
        if (work->tick & PATH_ENEMY_MIRROR) {
            sample.pos.x = -sample.pos.x;
        }
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&enemy->home);
        njRotateY(NULL, PATH_TURN(enemy));
        njCalcPoint(NULL, &sample.pos, &work->pos);
        njPopMatrixEx();
        enemy->spd = work->pos;
        njSubVector(&enemy->spd, &enemy->last_pos);
        dir.x = 0.0F;
        dir.y = 1.0F;
        dir.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, PATH_TURN(enemy));
        njRotateZ(NULL, sample.ang_z);
        njRotateX(NULL, sample.ang_x);
        njCalcVector(NULL, &dir, &dir);
        if (PATH_INDEX(enemy) == LEVEL_PATH ||
            work->submode == PATH_ENEMY_BOMBKUMI) {
            work->ang.x = 0;
            work->ang.z = 0;
        } else {
            work->ang.x = (Angle)(asinf(dir.z) * ANGLE_PER_RADIAN);
            work->ang.z = -(Angle)(atan2f(dir.x, dir.y) * ANGLE_PER_RADIAN);
        }
        if (work->submode == PATH_ENEMY_BOMBKUMI ||
            work->submode == PATH_ENEMY_KYOKO) {
            dir = enemy->spd;
        } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
            dir.x = -enemy->spd.x;
            dir.y = -enemy->spd.y;
            dir.z = -enemy->spd.z;
            /* @hack redundant: the test of player_nearest's result, which is
             * never -1, where the then-arm alone would do */
        } else if ((player = player_nearest(&work->pos)) != -1) {
            dir = player_work[player]->pos;
            njSubVector(&dir, &work->pos);
        } else {
            dir = enemy->spd;
        }
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njInvertMatrix(NULL);
        njCalcVector(NULL, &dir, &dir);
        work->ang.y = -(Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN);
        njPopMatrixEx();
    } else if (work->mode == MODE_FLY_ON) {
        add(&work->pos, &enemy->spd);
    }
}

static void path_enemy_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* move point from the enemy's own frame into the world's */
static void path_enemy_calc_point(struct task_work *work, NJS_POINT3 *point)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, point, point);
    njPopMatrixEx();
}

/* turn vector from the enemy's own frame into the world's */
static void path_enemy_calc_vector(struct task_work *work, NJS_VECTOR *vector)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vector, vector);
    njPopMatrixEx();
}

/* turn spd toward the first collision shape of the player nearest the
 * enemy, as seen from pos. spd must point along x. the turn stays within
 * AIM_PITCH of level and AIM_YAW of the enemy's heading. a shape at pos
 * gets a level shot along the heading */
static void path_enemy_aim(struct task_work *work, NJS_POINT3 *pos,
                           NJS_VECTOR *spd)
{
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_y;
    Angle ang_z;
    Angle turn_z;
    Angle turn_y;

    target = player_work[player_nearest(&work->pos)]->collision->shapes->center;
    distance = njDistanceP2P(pos, &target);
    rise = target.y - pos->y;
    if (distance != 0.0F) {
        ang_z = (Angle)(asinf(rise / distance) * ANGLE_PER_RADIAN);
        ang_y = (Angle)(atan2f(pos->z - target.z, target.x - pos->x) *
                        ANGLE_PER_RADIAN);
    } else {
        ang_y = work->ang.y;
        ang_z = 0;
    }
    turn_z = angle_approach(0, ang_z, AIM_PITCH);
    turn_y = angle_approach(work->ang.y, ang_y, AIM_YAW);
    njPushMatrix(&_nj_unit_matrix_);
    njRotateY(NULL, turn_y);
    njRotateZ(NULL, turn_z);
    njCalcPoint(NULL, spd, spd);
    njPopMatrixEx();
}

static void path_enemy_fire_bullet(struct task *task, struct task_work *work,
                                   struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    /* @hack spelling: the kyoko's and the backkyoko's values in branches of
     * their own, where one test of both would do */
    if (work->submode == PATH_ENEMY_GUNKUMI) {
        pos.x = GUNKUMI_MUZZLE_X;
        pos.y = GUNKUMI_MUZZLE_Y;
        pos.z = 0.0F;
    } else if (work->submode == PATH_ENEMY_KYOKO) {
        pos.x = KYOKO_MUZZLE_X;
        pos.y = KYOKO_MUZZLE_Y;
        pos.z = 0.0F;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        pos.x = KYOKO_MUZZLE_X;
        pos.y = KYOKO_MUZZLE_Y;
        pos.z = 0.0F;
    }
    path_enemy_calc_point(work, &pos);
    /* @hack spelling: the kyoko's and the backkyoko's speeds in branches of
     * their own, where one test of both would do */
    if (work->submode == PATH_ENEMY_GUNKUMI) {
        spd.x = GUNKUMI_SHOT_SPEED;
    } else if (work->submode == PATH_ENEMY_KYOKO) {
        spd.x = KYOKO_SHOT_SPEED;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        spd.x = KYOKO_SHOT_SPEED;
    }
    spd.y = 0.0F;
    spd.z = 0.0F;
    path_enemy_aim(work, &pos, &spd);
    /* @hack spelling: the kind as the flag's test, whose 1 is
     * ENEMY_BULLET_STOPS and whose 0 is ENEMY_BULLET_PASSES, where the flag
     * choosing between the two names would do */
    enemy_bullet_fire(task, (work->tick & PATH_ENEMY_BULLETS_PASS) == 0, &spd,
                      &pos);
}

static void path_enemy_drop_bomb(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    NJS_POINT3 pos;

    pos.x = BOMB_DROP_X;
    pos.y = BOMB_DROP_Y;
    pos.z = 0.0F;
    path_enemy_calc_point(work, &pos);
    enemy_bomb_create(task, BOMB_RED_ONE, BOMB_DROPPED, &pos);
}

/* send a jet from each of a kyoko's two engines. a kyoko's jets go the way
 * it faces, and a backkyoko's the other way */
static void kyoko_send_jets(struct task_work *work, struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (game_state.time_stopped) {
        return;
    }
    if (work->submode == PATH_ENEMY_KYOKO) {
        pos.x = KYOKO_JET_X;
        pos.y = JET_Y;
        pos.z = JET_Z;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        pos.x = BACKKYOKO_JET_X;
        pos.y = JET_Y;
        pos.z = JET_Z;
    }
    path_enemy_calc_point(work, &pos);
    if (work->submode == PATH_ENEMY_KYOKO) {
        spd.x = njScalor(&enemy->spd) - JET_SLOWER;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        spd.x = -njScalor(&enemy->spd) + JET_SLOWER;
    }
    spd.y = 0.0F;
    spd.z = 0.0F;
    path_enemy_calc_vector(work, &spd);
    enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &spd);
    if (work->submode == PATH_ENEMY_KYOKO) {
        pos.x = KYOKO_JET_X;
        pos.y = JET_Y;
        pos.z = -JET_Z;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        pos.x = BACKKYOKO_JET_X;
        pos.y = JET_Y;
        pos.z = -JET_Z;
    }
    path_enemy_calc_point(work, &pos);
    enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &spd);
}

static void path_enemy_glow(struct task *task, struct task_work *work,
                            struct enemy_work *enemy)
{
    NJS_POINT3 pos;

    /* @hack spelling: the kyoko's and the backkyoko's values in branches of
     * their own, where one test of both would do */
    if (work->submode == PATH_ENEMY_GUNKUMI ||
        work->submode == PATH_ENEMY_BOMBKUMI) {
        pos.x = KUMI_LIGHT_X;
        pos.y = KUMI_LIGHT_Y;
        pos.z = KUMI_LIGHT_Z;
    } else if (work->submode == PATH_ENEMY_KYOKO) {
        pos.x = KYOKO_LIGHT_X;
        pos.y = KYOKO_LIGHT_Y;
        pos.z = KYOKO_LIGHT_Z;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        pos.x = KYOKO_LIGHT_X;
        pos.y = KYOKO_LIGHT_Y;
        pos.z = KYOKO_LIGHT_Z;
    }
    path_enemy_calc_point(work, &pos);
    enemy_light_create(task, 0, &pos);
    /* @hack spelling: the kyoko's and the backkyoko's other values in
     * branches of their own, where one test of both would do */
    if (work->submode == PATH_ENEMY_GUNKUMI ||
        work->submode == PATH_ENEMY_BOMBKUMI) {
        pos.x = KUMI_LIGHT_X;
        pos.y = KUMI_LIGHT_Y;
        pos.z = -KUMI_LIGHT_Z;
    } else if (work->submode == PATH_ENEMY_KYOKO) {
        pos.x = KYOKO_LIGHT_X;
        pos.y = KYOKO_LIGHT_Y;
        pos.z = -KYOKO_LIGHT_Z;
    } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
        pos.x = KYOKO_LIGHT_X;
        pos.y = KYOKO_LIGHT_Y;
        pos.z = -KYOKO_LIGHT_Z;
    }
    path_enemy_calc_point(work, &pos);
    enemy_light_create(task, 0, &pos);
    if (work->submode == PATH_ENEMY_KYOKO ||
        work->submode == PATH_ENEMY_BACKKYOKO) {
        /* @hack spelling: the two kinds' values in branches of their own,
         * where the values set without a test would do */
        if (work->submode == PATH_ENEMY_KYOKO) {
            pos.x = KYOKO_CENTER_LIGHT_X;
            pos.y = KYOKO_LIGHT_Y;
            pos.z = 0.0F;
        } else if (work->submode == PATH_ENEMY_BACKKYOKO) {
            pos.x = KYOKO_CENTER_LIGHT_X;
            pos.y = KYOKO_LIGHT_Y;
            pos.z = 0.0F;
        }
        path_enemy_calc_point(work, &pos);
        enemy_light_create(task, 0, &pos);
    }
}

/* the step of MODE_RIDE and MODE_FLY_ON. the enemy fires only while
 * DISTANCE lies from FIRE_START to FIRE_END. DISTANCE stops changing once
 * the enemy flies on */
static void path_enemy_ride_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    path_enemy_move(task, work, enemy);
    path_enemy_check_hit(task, work, enemy);
    if (work->mode != MODE_HIT) {
        if (game_state.time_stopped == 0) {
            work->timer++;
        }
        if (work->submode == PATH_ENEMY_KYOKO ||
            work->submode == PATH_ENEMY_BACKKYOKO) {
            kyoko_send_jets(work, enemy);
        }
        if (work->timer & GLOW_BIT) {
            path_enemy_glow(task, work, enemy);
        }
        if (DISTANCE(enemy) >= FIRE_START(enemy) &&
            DISTANCE(enemy) <= FIRE_END(enemy)) {
            if (RELOAD(enemy) <= 0) {
                if (work->submode == PATH_ENEMY_GUNKUMI) {
                    if (enemy->motion.action != ACTION_GUNKUMI_FIRE) {
                        enemy->motion.next = ACTION_GUNKUMI_FIRE;
                    } else if (enemy->motion.frame >= FIRE_FRAME &&
                               enemy->motion.frame < FIRE_FRAME_END) {
                        path_enemy_fire_bullet(task, work, enemy);
                        RELOAD(enemy) = RELOAD_EXTRA(enemy) + GUN_RELOAD;
                    }
                } else if (work->submode == PATH_ENEMY_BOMBKUMI) {
                    path_enemy_drop_bomb(task, work, enemy);
                    RELOAD(enemy) = RELOAD_EXTRA(enemy) + BOMB_RELOAD;
                } else if (work->submode == PATH_ENEMY_KYOKO ||
                           work->submode == PATH_ENEMY_BACKKYOKO) {
                    path_enemy_fire_bullet(task, work, enemy);
                    RELOAD(enemy) = RELOAD_EXTRA(enemy) + KYOKO_RELOAD;
                }
            } else {
                RELOAD(enemy)--;
            }
        }
    }
}

static void path_enemy_hover(struct task_work *work, struct enemy_work *enemy)
{
    int player;
    NJS_VECTOR dir;
    /* @hack spelling: bob_cos, a local for the bob phase's cosine, where njCos
     * read in place, before the phase turns, would do */
    Float bob_cos;

    bob_cos = njCos(BOB_PHASE(enemy));
    BOB_PHASE(enemy) += BOB_TURN;
    work->pos.y += bob_cos * BOB_RISE;
    player = player_nearest(&work->pos);
    work->ang.x = angle_approach(work->ang.x, 0, LEVEL_TURN);
    work->ang.z = angle_approach(work->ang.z, 0, LEVEL_TURN);
    /* @hack redundant: the test of player_nearest's result, which is never
     * -1, where the then-arm alone would do */
    if (player != -1) {
        dir = player_work[player]->pos;
        njSubVector(&dir, &work->pos);
    } else {
        dir = enemy->spd;
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njInvertMatrix(NULL);
    njCalcVector(NULL, &dir, &dir);
    work->ang.y = -(Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN);
    njPopMatrixEx();
}

/* the step of MODE_HOVER. a hovering enemy fires whatever its DISTANCE */
static void path_enemy_hover_step(struct task *task, struct task_work *work,
                                  struct enemy_work *enemy)
{
    path_enemy_hover(work, enemy);
    path_enemy_check_hit(task, work, enemy);
    if (work->mode != MODE_HIT) {
        if (game_state.time_stopped == 0) {
            work->timer++;
        }
        if (work->submode == PATH_ENEMY_KYOKO ||
            work->submode == PATH_ENEMY_BACKKYOKO) {
            kyoko_send_jets(work, enemy);
        }
        if (work->timer & GLOW_BIT) {
            path_enemy_glow(task, work, enemy);
        }
        if (RELOAD(enemy) <= 0) {
            if (work->submode == PATH_ENEMY_GUNKUMI) {
                if (enemy->motion.action != ACTION_GUNKUMI_FIRE) {
                    enemy->motion.next = ACTION_GUNKUMI_FIRE;
                } else if (enemy->motion.frame >= FIRE_FRAME &&
                           enemy->motion.frame < FIRE_FRAME_END) {
                    path_enemy_fire_bullet(task, work, enemy);
                    RELOAD(enemy) = RELOAD_EXTRA(enemy) + GUN_RELOAD;
                }
            } else if (work->submode == PATH_ENEMY_BOMBKUMI) {
                path_enemy_drop_bomb(task, work, enemy);
                RELOAD(enemy) = RELOAD_EXTRA(enemy) + BOMB_RELOAD;
            } else if (work->submode == PATH_ENEMY_KYOKO ||
                       work->submode == PATH_ENEMY_BACKKYOKO) {
                path_enemy_fire_bullet(task, work, enemy);
                RELOAD(enemy) = RELOAD_EXTRA(enemy) + KYOKO_RELOAD;
            }
        } else {
            RELOAD(enemy)--;
        }
    }
}

static void path_enemy_wait_step(struct task *task, struct task_work *work,
                                 struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    struct task_work *player;
    int i;

    pos = enemy->home;
    njAddVector(&pos, &enemy->offset);
    /* @bug logic: moves the enemy twice where both players are in range on
     * one step, since the loop goes on after a wake (BSR path_enemy_move at
     * 0x1308 of the object's code) */
    for (i = 0; i < GAME_PLAYERS; i++) {
        player = player_work[i];
        if (player != NULL &&
            WAKE_RANGE(enemy) > njDistanceP2P(&player->pos, &pos)) {
            work->mode = MODE_RIDE;
            placement_stop(task);
            path_enemy_move(task, work, enemy);
        }
    }
}

/* NOLINTBEGIN(misc-use-internal-linkage) */
/* @hack type: external linkage, where static would do */
void fun_path_enemy_1324(struct task *task, struct task_work *work,
                         struct enemy_work *enemy)
{
    struct enemy_route *route = task->extra.p;

    /* @hack redundant: a return, and behind it three calls that never run.
     * SHC writes none of their code, but sizes the function's stack frame
     * for them */
    if (1) {
        return;
    }
    path_enemy_set_route(work, enemy, route);
    path_enemy_hover(work, enemy);
    path_enemy_hover(work, enemy);
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw the enemy's path as a triangle at each point but the last. the
 * triangles pulse and their colors cycle as route->frames counts up. only
 * code of path_enemy_draw that no path reaches calls it */
static void path_enemy_draw_route(struct task *task)
{
    struct enemy_route *route = task->extra.p;
    struct task_work *work = task->work;
    struct path *path;
    struct path_point *point;
    Uint16 phase;
    Uint16 rest;
    Uint16 i;
    Uint32 path_color;
    Uint32 tip_color;
    NJS_VECTOR normal;
    Float length;

    route->frames++;
    phase = (Uint8)route->frames;
    rest = BLEND_SCALE - phase;
    path_color = BLEND_COLOR(0xff, 0xff, 0xff, 0x00, 0x00, 0xff, phase, rest);
    tip_color = BLEND_COLOR(0x00, 0xff, 0x00, 0xff, 0x00, 0xff, phase, rest);
    route_triangle[0].col = path_color;
    route_triangle[1].col = path_color;
    route_triangle[2].col = tip_color;
    path = route->path;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y & PATH_TURN_BITS);
    njDrawPolygon3DExStart(0);
    for (i = 0, point = path->points; i < path->count - 1; i++, point++) {
        normal.x = -njSin(point->slope_z) * njCos(point->slope_x);
        normal.y = njCos(point->slope_z) * njCos(point->slope_x);
        normal.z = njSin(point->slope_x);
        length =
            TIP_LENGTH +
            (TIP_GROWTH * (Float)((route->frames & TIP_FRAMES) >> TIP_SHIFT));
        route_triangle[0].x = point->pos.x;
        route_triangle[0].y = point->pos.y;
        route_triangle[0].z = point->pos.z;
        route_triangle[1].x = point[1].pos.x;
        route_triangle[1].y = point[1].pos.y;
        route_triangle[1].z = point[1].pos.z;
        route_triangle[2].x = point->pos.x + (length * normal.x);
        route_triangle[2].y = point->pos.y + (length * normal.y);
        route_triangle[2].z = point->pos.z + (length * normal.z);
        if (work->tick & PATH_ENEMY_MIRROR) {
            route_triangle[0].x = -route_triangle[0].x;
            route_triangle[1].x = -route_triangle[1].x;
            route_triangle[2].x = -route_triangle[2].x;
        }
        njDrawPolygon3DExSetData(route_triangle, 3);
    }
    njDrawPolygon3DExEnd();
    njPopMatrixEx();
}

/* draw the body, and under PATH_ENEMY_SHADOW the shadow. an enemy in
 * MODE_WAIT draws neither */
static void path_enemy_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_DIRECT_HEAD *model;
    NJS_POINT3 point_a;
    NJS_POINT3 point_b;
    NJS_POINT3 point_c;
    Float length;
    Float size;
    NJS_MATRIX *matrix;

    light_apply(PATH_ENEMY_LIGHT);
    /* @hack redundant: a block that never runs and leaves no code, but
     * changes the code SHC writes after it. NULL in place of the block's 0
     * changes that code too */
    if (0) {
        work->ang.y += QUARTER_TURN;
        work->ang.y += QUARTER_TURN;
        work->ang.y += QUARTER_TURN;
        matrix = 0;
    }
    if (work->mode != MODE_WAIT) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        /* @hack spelling: the kyoko and the backkyoko in cases of their own,
         * where one case for both would do */
        switch (work->submode) {
        case PATH_ENEMY_GUNKUMI:
        case PATH_ENEMY_BOMBKUMI:
            njSetTexture(&path_kumi_texlist);
            break;
        case PATH_ENEMY_KYOKO:
            njSetTexture(&path_kyoko_texlist);
            break;
        case PATH_ENEMY_BACKKYOKO:
            njSetTexture(&path_kyoko_texlist);
            break;
        }
        if (work->flags & PATH_ENEMY_COMPILED) {
            njSetDirectClip();
#ifdef NONMATCHING
            /* the switch below sets model: it has a case for each kind the
             * submode can hold. GCC can't see that the submode holds no
             * other */
            model = FIRST_MODEL(kumi_holder);
#endif
            switch (work->submode) {
            case PATH_ENEMY_GUNKUMI:
                model = FIRST_MODEL(kumi_holder);
                break;
            case PATH_ENEMY_BOMBKUMI:
                model = SECOND_MODEL(kumi_holder);
                break;
            case PATH_ENEMY_KYOKO:
                model = FIRST_MODEL(kyoko_holder);
                break;
            case PATH_ENEMY_BACKKYOKO:
                model = SECOND_MODEL(kyoko_holder);
                break;
            }
            njDirectDrawObject(model);
        } else if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            njCnkSimpleDrawMotion(
                enemy->motion.object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        if (work->tick & PATH_ENEMY_SHADOW) {
            switch (work->submode) {
            case PATH_ENEMY_GUNKUMI:
            case PATH_ENEMY_BOMBKUMI:
                enemy_draw_shadow(work, enemy);
                break;
            case PATH_ENEMY_KYOKO:
            case PATH_ENEMY_BACKKYOKO:
                draw_control_set(DRAW_CONTROL_SHADOW);
                njPushMatrixEx();
                njTranslateEx(&work->pos);
                njRotateY(NULL, work->ang.y + QUARTER_TURN);
                njCnkModDrawObject(&path_kyoko_shadow_object);
                njPopMatrixEx();
                draw_control_clear(DRAW_CONTROL_SHADOW);
                break;
            }
        }
        /* @hack redundant: a block that never runs. the image holds each
         * case's code, but not the switch's test. each case there calls an
         * address where no function starts, and njCnkSimpleDrawObject stands
         * in for that call */
        if (0) {
            switch (work->submode) {
            case PATH_ENEMY_GUNKUMI:
                njSetTexture(&path_kumi_texlist);
                njCnkSimpleDrawObject(path_gunkumi_object);
                break;
            case PATH_ENEMY_BOMBKUMI:
                njSetTexture(&path_kumi_texlist);
                njCnkSimpleDrawObject(path_bombkumi_object);
                break;
            case PATH_ENEMY_KYOKO:
                njSetTexture(&path_kyoko_texlist);
                njCnkSimpleDrawObject(path_kyoko_object);
                break;
            case PATH_ENEMY_BACKKYOKO:
                njSetTexture(&path_kyoko_texlist);
                njCnkSimpleDrawObject(path_backkyoko_object);
                break;
            }
            njPopMatrixEx();
        }
        /* @hack spelling: an empty else, where the if alone would do */
    } else {
    }
    light_apply(light_after_draw);
    /* @hack redundant: a return, and behind it code that never runs. the
     * image holds that code's strings and stack frame, but none of its
     * instructions */
    if (1) {
        return;
    }
    (void)"-> ANG Y ";
    (void)" ID:";
    (void)"-> Path";
    (void)"/R/";
    (void)"Ng";
    (void)"   ";
    (void)"Sd";
    (void)"Sp";
    (void)"/Type";
    (void)"-> ST(";
    (void)")ED(";
    (void)")/";
    (void)"-> SCOPE X/R";
    (void)"-> SCOPE Y/START PNT";
    (void)"-> SCOPE Z/INTERVAL";
    size = work->scl.x;
    njPopMatrixEx();
    njSetTexture(&path_kumi_texlist);
    njSetTexture(&path_kumi_texlist);
    njSetTexture(&path_kumi_texlist);
    njSetTexture(&path_kumi_texlist);
    njSetTexture(&path_kumi_texlist);
    njCalcPoint(matrix, &point_a, &point_b);
    njUnitMatrix(NULL);
    njCalcPoint(matrix, &point_a, &point_b);
    njCalcVector(NULL, &point_b, &point_c);
    njUnitMatrix(NULL);
    njCalcPoint(matrix, &point_a, &point_b);
    njCalcVector(NULL, &point_b, &point_c);
    njCalcPoint(matrix, &point_c, &point_a);
    njCalcPoint(matrix, &point_c, &point_a);
    njCalcPoint(matrix, &point_c, &point_a);
    njCalcPoint(NULL, &point_c, &point_a);
    path_point_distance(((struct enemy_route *)task->extra.p)->path, 0,
                        &length);
    path_point_distance(((struct enemy_route *)task->extra.p)->path, 0,
                        &length);
    path_enemy_draw_route(task);
    enemy_draw_shadow(work, enemy);
    point_b.x = size;
}

void path_enemy_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_VECTOR point;
    Float x;
    Float y;

    if (work->mode != MODE_INIT && work->mode != MODE_RIDE &&
        placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        /* @hack spelling: the init's call after the switch, reached by a
         * goto, where the call and a return in its case would do */
        goto init;
    case MODE_RIDE:
    case MODE_FLY_ON:
        path_enemy_ride_step(task, work, enemy);
        break;
    case MODE_HOVER:
        path_enemy_hover_step(task, work, enemy);
        break;
    case MODE_HIT:
        work->mode = MODE_GONE;
        /* fall through */
    case MODE_GONE:
        placement_kill(task);
        /* @hack spelling: a goto to the end, where a return would do */
        goto end;
    case MODE_WAIT:
        path_enemy_wait_step(task, work, enemy);
        return;
    }
    /* @hack spelling: the empty then-arm and its else, where the test
     * negated would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        enemy_motion_step(&enemy->motion);
    }
    if (work->mode != MODE_HIT) {
        collision_register(task);
    }
    /* @hack spelling: a goto to the end, where a return would do */
    goto end;
init:
    path_enemy_init(task, work);
    return;
end:
    /* @hack redundant: a return, and behind it code that never runs. the
     * image holds that code's stack frame, but none of its instructions */
    if (1) {
        return;
    }
    x = work->scl.x;
    y = work->scl.y;
    njCalcPoint(NULL, &point, &point);
    work->pos.x = x + y;
}
