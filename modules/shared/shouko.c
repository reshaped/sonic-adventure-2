/* the shouko, E SHOUKO in a stage's object list: an enemy that flies a path of
 * its own. it is a gunner or a bomber. E SW SHOUKO, its switch, wakes it.
 *
 * the functions here push at most one matrix at a time on Ninja's matrix
 * stack. the stack must have room for it, which no function here checks */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "effect/burst.h"
#include "effect/powder.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/blast.h"
#include "shared/enemy_jet.h"
#include "shared/enemy_light.h"
#include "shared/enemy_route.h"
#include "shared/shouko.h"
#include "sound/sound.h"
#include "stage/path.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "vmu/vibration.h"

#include "shared/shouko/text.inc"

/* a shouko's modes, in its work's mode */
enum shouko_mode {
    MODE_INIT,
    MODE_WAIT, /* unseen, until enemy_switch_bits has its switch's bit */
    MODE_RIDE, /* it rides its path */
    MODE_FLY_ON, /* a bomber flying straight on from the path's last segment */
    MODE_BRAKE, /* a gunner slowing to a stop from the path's last segment */
    MODE_APPROACH, /* a gunner on its way to the target */
    MODE_HOVER, /* a gunner at the target */
    MODE_HIT, /* a hit broke it */
    MODE_GONE /* the step ends it through placement_kill */
};

/* the fields of ang.x: the kind, the path's index in shouko_paths, and
 * DROP_POINT. DROP_POINT is the point of the path where a bomber's drops
 * start */
#define KIND_BITS 0xf
#define PATH_INDEX_BITS 0xf0
#define PATH_INDEX_SHIFT 4
#define DROP_POINT_BITS 0xff00
#define DROP_POINT_SHIFT 8
#define SHOUKO_KIND(work) ((work)->ang.x & KIND_BITS)
#define PATH_CHOICE(work)                                                      \
    (((work)->ang.x & PATH_INDEX_BITS) >> PATH_INDEX_SHIFT)
#define DROP_POINT(work) (((work)->ang.x & DROP_POINT_BITS) >> DROP_POINT_SHIFT)
/* the number of the switch a shouko waits on, or of a switch itself.
 * SWITCH_NUMBER_BITS hold it in a shouko's group and in a switch's ang.y */
#define SWITCH_NUMBER(work) ((work)->tick)
#define SWITCH_NUMBER_BITS 0xf

/* what a shouko keeps in the fields of its enemy_work that enemy.h leaves to
 * each kind */
/* a gunner's ang.y as it reached the target */
#define HOVER_HEADING(enemy) ((enemy)->unk_018)
#define PATH_TURN(enemy) ((enemy)->unk_01c) /* the path's turn about y */
/* the index of the shouko's path in shouko_paths */
#define PATH_INDEX(enemy) ((enemy)->unk_046)
/* the drop a bomber reaches next, or DROPS once it has passed the third */
#define NEXT_DROP(enemy) ((enemy)->unk_047)
/* the steps a gunner has counted toward its next shot */
#define RELOAD(enemy) ((enemy)->unk_048)
#define DISTANCE(enemy) ((enemy)->unk_094) /* how far along the path it is */
/* the ang.z the shouko was set with, which spaces its bombs */
#define SPREAD(enemy) ((enemy)->unk_098)
/* the distance along the path where a bomber's drops start */
#define DROP_START(enemy) ((enemy)->unk_09c)
/* the phase of a hovering gunner's bob */
#define BOB_PHASE(enemy) ((enemy)->unk_1c4)

/* the light the shouko and its bombs draw in */
#define SHOUKO_LIGHT 8
/* the distance a shouko covers along its path in a step */
#define RIDE_SPEED 25.0F
/* a shouko's turn_speed, shadow_size and shadow_ratio */
#define TURN_SPEED 0x400
#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.2F
/* the index of a path's last segment. its count points bound count - 1
 * segments */
#define LAST_SEGMENT(path) ((path)->count - 2)
/* a braking gunner's speed falls by BRAKE_STEP a step. once it is below
 * BRAKE_END, the gunner stops */
#define BRAKE_STEP 0.2F
#define BRAKE_END 0.3F
/* a gunner flies to the target at APPROACH_SPEED a step. once the target lies
 * within a step, the gunner moves onto it */
#define APPROACH_SPEED 1.0F
/* a hovering gunner bobs BOB_HEIGHT above and below the target. its phase
 * turns by BOB_TURN a step */
#define BOB_HEIGHT 5.0F
#define BOB_TURN 0x200
/* the farthest a hovering gunner turns either way from HOVER_HEADING */
#define HOVER_TURN ((int)(15 * ANGLE_PER_DEGREE))
/* a gunner's shots lie at least RELOAD_STEPS + 2 steps apart */
#define RELOAD_STEPS 8
/* work->timer counts a gunner's steps. FIRE_BIT is set in it for 64 steps of
 * each 128. a gunner fires only at those steps */
#define FIRE_BIT 0x40

/* a bomber's drops, in their order along the path */
enum bomb_drop { FIRST_DROP, SECOND_DROP, THIRD_DROP, DROPS };
/* how far past DROP_START a bomber reaches each drop */
#define FIRST_DROP_AT 0.0F
#define SECOND_DROP_AT 140.0F
#define THIRD_DROP_AT 280.0F
/* the first and third drops' bombs miss the target by SPREAD_SCALE times
 * SPREAD. each misses across x and z, along the line from where it drops */
#define SPREAD_SCALE 0.75F

/* the points a hit scores: SCORE_KNOCK where player_touch_attacks returns
 * TRUE, else SCORE */
#define SCORE 500
#define SCORE_KNOCK 1000
/* the scale of the ball a hit makes of a shouko under ENEMY_IN_MAGIC_HAND */
#define BALL_SCALE 0.026F

/* shouko_aim turns a bullet at most AIM_PITCH from level, and at most AIM_YAW
 * from the shouko's heading */
#define AIM_PITCH 0x1800
#define AIM_YAW 0x800
/* each gun's place in the shouko's own frame, whose x points ahead. one gun
 * lies at GUN_Z, and the other at -GUN_Z */
#define GUN_X 40.0F
#define GUN_Y 6.0F
#define GUN_Z 10.0F
#define BULLET_SPEED 5.0F
/* where a bomb drops from, in the shouko's own frame */
#define HATCH_X 4.0F
/* each engine's place in the shouko's own frame, at ENGINE_Z and -ENGINE_Z.
 * a jet goes JET_SLOWER slower than the shouko */
#define ENGINE_X (-80.0F)
#define ENGINE_Y 4.0F
#define ENGINE_Z 12.0F
#define JET_SLOWER 1.2F

/* the sounds of the shouko's engines. SOUND_FLY plays during the ride, and
 * as a bomber flies on after it. SOUND_HOVER plays as a gunner brakes, flies
 * to the target and hovers. each plays at ENGINE_PRIORITY and SOUND_LOUDEST */
#define SOUND_FLY 0x4020
#define SOUND_HOVER 0x4021
#define ENGINE_PRIORITY 1

#define SOUND_BULLET_HIT 0x4022
#define SOUND_BURST 0x4019
#define BURST_VOLUME 70

/* a bullet's modes, in its work's mode */
enum bullet_mode { BULLET_FLY, BULLET_HIT };
/* the squared range placement_try_leave_by takes for a bullet */
#define BULLET_RANGE2 (1000.0F * 1000.0F)
/* a bullet registers its collision record once its timer, which counts its
 * steps, passes BULLET_ARMED_STEPS */
#define BULLET_ARMED_STEPS 4

/* a bomb's modes, in its work's mode */
enum bomb_mode {
    BOMB_FALL = 0,
    BOMB_LANDED = 1,
    BOMB_BURST = 2,
    BOMB_DONE = 4 /* the burst is over, and the step kills the bomb */
};
/* a bomb falls at BOMB_SPEED a step toward the point it lands on. once it
 * lies within LAND_DISTANCE of the point, it lands there */
#define BOMB_SPEED 7.0F
#define LAND_DISTANCE 7.0F
/* a bomb's turn_speed, shadow_size and shadow_ratio */
#define BOMB_TURN_SPEED 0xe0
#define BOMB_SHADOW_SIZE 2.0F
#define BOMB_SHADOW_RATIO 1.0F
/* the radius of a bomb's collision record */
#define RECORD_RADIUS 40.0F
/* a hit bursts a falling bomb once its timer passes BOMB_ARMED_STEPS */
#define BOMB_ARMED_STEPS 20
/* a burst grows RADIUS from BURST_START to BURST_END, by BURST_STEP a
 * step. the burst's sphere, bomb_shapes[BURST_SHAPE], grows with it. LINGER
 * then falls from 0 by LINGER_STEP a step, and the burst ends once it is below
 * LINGER_END. each step of the burst turns the bomb by BURST_SPIN about y */
#define RADIUS(work) ((work)->scl.x)
#define LINGER(work) ((work)->scl.y)
#define BURST_START 5.0F
#define BURST_END 25.0F
#define BURST_STEP 1.0F
#define LINGER_STEP 0.05F
#define LINGER_END (-1.0F)
#define BURST_SPIN 0x100
/* a landed bomb draws in red while its timer is BOMB_FLASH_STEPS or less */
#define BOMB_FLASH_STEPS 2
/* the scale of the burst's puff */
#define PUFF_SCALE 15.0F

/* the task levels of the textures' holder, and of a bullet and of a bomb
 * without a parent */
#define HOLDER_LEVEL 0
#define BULLET_LEVEL 3
#define BOMB_LEVEL 3

/* the textures, which shouko_init loads from the archive E_SHOTEX */
#define SHOUKO_TEXTURES 4
static NJS_TEXNAME shouko_texname[SHOUKO_TEXTURES] = {
#include "shared/shouko/shouko_texname.inc"
};
static NJS_TEXLIST shouko_texlist = {
#include "shared/shouko/shouko_texlist.inc"
};
/* the models, in this order: the shouko's, the bomb's and the shouko's
 * shadow. the shouko's is shouko_object and its six children, shouko_node_1
 * to shouko_node_6. the shadow, shouko_shadow_object, is a modifier volume */
static Sint16 shouko_node_6_polygons[507] = {
#include "shared/shouko/shouko_node_6_polygons.inc"
};
static Sint32 shouko_node_6_vertices[429] = {
#include "shared/shouko/shouko_node_6_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_6_model = {
#include "shared/shouko/shouko_node_6_model.inc"
};
static NJS_CNK_OBJECT shouko_node_6_object = {
#include "shared/shouko/shouko_node_6_object.inc"
};
static Sint16 shouko_node_5_polygons[111] = {
#include "shared/shouko/shouko_node_5_polygons.inc"
};
static Sint32 shouko_node_5_vertices[99] = {
#include "shared/shouko/shouko_node_5_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_5_model = {
#include "shared/shouko/shouko_node_5_model.inc"
};
static NJS_CNK_OBJECT shouko_node_5_object = {
#include "shared/shouko/shouko_node_5_object.inc"
};
static Sint16 shouko_node_4_polygons[111] = {
#include "shared/shouko/shouko_node_4_polygons.inc"
};
static Sint32 shouko_node_4_vertices[99] = {
#include "shared/shouko/shouko_node_4_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_4_model = {
#include "shared/shouko/shouko_node_4_model.inc"
};
static NJS_CNK_OBJECT shouko_node_4_object = {
#include "shared/shouko/shouko_node_4_object.inc"
};
static Sint16 shouko_node_3_polygons[295] = {
#include "shared/shouko/shouko_node_3_polygons.inc"
};
static Sint32 shouko_node_3_vertices[159] = {
#include "shared/shouko/shouko_node_3_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_3_model = {
#include "shared/shouko/shouko_node_3_model.inc"
};
static NJS_CNK_OBJECT shouko_node_3_object = {
#include "shared/shouko/shouko_node_3_object.inc"
};
static Sint16 shouko_node_2_polygons[165] = {
#include "shared/shouko/shouko_node_2_polygons.inc"
};
static Sint32 shouko_node_2_vertices[123] = {
#include "shared/shouko/shouko_node_2_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_2_model = {
#include "shared/shouko/shouko_node_2_model.inc"
};
static NJS_CNK_OBJECT shouko_node_2_object = {
#include "shared/shouko/shouko_node_2_object.inc"
};
static Sint16 shouko_node_1_polygons[295] = {
#include "shared/shouko/shouko_node_1_polygons.inc"
};
static Sint32 shouko_node_1_vertices[159] = {
#include "shared/shouko/shouko_node_1_vertices.inc"
};
static NJS_CNK_MODEL shouko_node_1_model = {
#include "shared/shouko/shouko_node_1_model.inc"
};
static NJS_CNK_OBJECT shouko_node_1_object = {
#include "shared/shouko/shouko_node_1_object.inc"
};
static Sint16 shouko_polygons[757] = {
#include "shared/shouko/shouko_polygons.inc"
};
static Sint32 shouko_vertices[459] = {
#include "shared/shouko/shouko_vertices.inc"
};
static NJS_CNK_MODEL shouko_model = {
#include "shared/shouko/shouko_model.inc"
};
static NJS_CNK_OBJECT shouko_object = {
#include "shared/shouko/shouko_object.inc"
};
static Sint16 bomb_polygons[149] = {
#include "shared/shouko/bomb_polygons.inc"
};
static Sint32 bomb_vertices[159] = {
#include "shared/shouko/bomb_vertices.inc"
};
static NJS_CNK_MODEL shouko_bomb_model = {
#include "shared/shouko/shouko_bomb_model.inc"
};
static NJS_CNK_OBJECT bomb_object = {
#include "shared/shouko/bomb_object.inc"
};
/* The NJD_CO_P3 chunk's size includes its padding halfword after the
 * triangles, at index 141. */
static Sint16 shouko_shadow_polygons[143] = {
#include "shared/shouko/shouko_shadow_polygons.inc"
};
static Sint32 shouko_shadow_vertices[78] = {
#include "shared/shouko/shouko_shadow_vertices.inc"
};
static NJS_CNK_MODEL shouko_shadow_model = {
#include "shared/shouko/shouko_shadow_model.inc"
};
static NJS_CNK_OBJECT shouko_shadow_object = {
#include "shared/shouko/shouko_shadow_object.inc"
};
static struct path_point shouko_path_0_points[60] = {
#include "shared/shouko/shouko_path_0_points.inc"
};
static struct path shouko_path_0 = {
#include "shared/shouko/shouko_path_0.inc"
};
static struct path_point shouko_path_1_points[16] = {
#include "shared/shouko/shouko_path_1_points.inc"
};
static struct path shouko_path_1 = {
#include "shared/shouko/shouko_path_1.inc"
};
/* Each shouko shape attacks at level 1 and hurts players. */
#define SHOUKO_SHAPES 6
static struct collision_shape shouko_shapes[SHOUKO_SHAPES] = {
#include "shared/shouko/shouko_shapes.inc"
};
/* the models the shouko breaks into, ended by a null */
static NJS_CNK_MODEL *shouko_pieces[8] = {
#include "shared/shouko/shouko_pieces.inc"
};
KEPT static struct path *shouko_paths[2] = {
#include "shared/shouko/shouko_paths.inc"
};
#define SHOUKO_PATHS ARRAY_COUNT(shouko_paths)
/* the holder, a task that keeps shouko_texlist loaded, or null while none
 * does. its destructor releases the list */
static struct task *shouko_holder = NULL;
/* the triangle shouko_draw_route draws at each point of a path */
static NJS_POLYGON_VTX route_triangle[3] = {
#include "shared/shouko/route_triangle.inc"
};
/* the kinds' titles, by shouko_kind */
static char *shouko_titles[SHOUKO_KINDS] = {
#include "shared/shouko/shouko_titles.inc"
};

/* a switch's shapes: a box and a sphere. SWITCH_SHAPE, bit 0 of its ang.x,
 * picks one, and the switch's scl sizes it */
#define SWITCH_SHAPE(work) ((work)->ang.x & 1)
#define SWITCH_BOX 0
/* the bits of a switch's ang.y and ang.z that turn a box: all but the low
 * byte */
#define SWITCH_TURN_BITS (~0xff)
#define SWITCH_SHAPES 2
/* Both shapes ignore enemies, objects and pickups and set SHAPE_UNK_2000. */
static struct collision_shape switch_shapes[SWITCH_SHAPES] = {
#include "shared/shouko/switch_shapes.inc"
};
/* the shapes' names, by SWITCH_SHAPE */
static char *switch_shape_names[SWITCH_SHAPES] = {
#include "shared/shouko/switch_shape_names.inc"
};
/* each shape's model, an index in debug_models */
static Sint8 switch_models[SWITCH_SHAPES] = {
#include "shared/shouko/switch_models.inc"
};

#define BULLET_SHAPES 1
static struct collision_shape bullet_shape = {
#include "shared/shouko/bullet_shape.inc"
};
/* a bullet draws as a line between its two ends, in opaque yellow. SHC
 * refuses braces around each color, with error 2129 */
#define BULLET_ENDS 2
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR bullet_colors[BULLET_ENDS] = {
#include "shared/shouko/bullet_colors.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
/* in the bullet's own frame, its ends lie BULLET_HALF_LENGTH ahead of its
 * pos and as far behind */
#define BULLET_HALF_LENGTH 2.0F
static NJS_POINT3 bullet_ends[BULLET_ENDS] = {
#include "shared/shouko/bullet_ends.inc"
};
/* a bomb's shapes: the bomb, and the sphere of its burst, which is off until
 * the burst */
#define BURST_SHAPE 1
#define BOMB_SHAPES 2
static struct collision_shape bomb_shapes[BOMB_SHAPES] = {
#include "shared/shouko/bomb_shapes.inc"
};

static void shouko_destroy(struct task *task);
static void shouko_draw(struct task *task);
static void shouko_move(struct task *task, struct task_work *work,
                        struct enemy_work *enemy);
static void shouko_draw_route(struct task *task);
static void shouko_switch_step(struct task *task);
static void shouko_switch_draw(struct task *task);
static void shouko_switch_destroy(struct task *task);
static void shouko_bullet_fire(struct task *parent, NJS_VECTOR *dir,
                               NJS_POINT3 *pos);
static void shouko_bomb_drop(struct task *parent, NJS_POINT3 *pos,
                             NJS_POINT3 *target, Float spread,
                             enum bomb_drop drop);

static void shouko_holder_destroy(struct task *task)
{
    njReleaseTexture(&shouko_texlist);
    shouko_holder = NULL;
}

static void shouko_holder_step(struct task *task)
{
}

static void shouko_init(struct task *task, struct task_work *work)
{
    struct enemy_work *enemy;
    struct enemy_route *route;
    unsigned int point;

    enemy = enemy_work_alloc_grouped(task);
    work->submode = SHOUKO_KIND(work);
    if (work->submode >= SHOUKO_KINDS || work->submode < 0) {
        work->submode = SHOUKO_GUNNER;
    }
    /* @hack spelling: one assignment to both, where a statement for each
     * would do */
    task->extra.p = route = syCalloc(1, sizeof(struct enemy_route));
    PATH_INDEX(enemy) = PATH_CHOICE(work);
    if ((Uint32)PATH_INDEX(enemy) >= SHOUKO_PATHS || PATH_INDEX(enemy) < 0) {
        PATH_INDEX(enemy) = 0;
    }
    route->path = shouko_paths[PATH_INDEX(enemy)];
    point = DROP_POINT(work);
    if (path_point_distance(route->path, point, &DROP_START(enemy)) == 0) {
        DROP_START(enemy) = 0.0F;
    }
    collision_init(task, shouko_shapes, SHOUKO_SHAPES, COLLISION_ENEMY);
    DISTANCE(enemy) = 0.0F;
    enemy->target = work->scl;
    SPREAD(enemy) = work->ang.z;
    SWITCH_NUMBER(work) = enemy->group & SWITCH_NUMBER_BITS;
    PATH_TURN(enemy) = work->ang.y;
    work->ang.z = 0;
    work->ang.x = 0;
    enemy->turn_speed = TURN_SPEED;
    enemy_set_home(work, enemy);
    work->mode = MODE_WAIT;
    enemy->motion.object = &shouko_object;
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = shouko_draw;
    task->destructor = shouko_destroy;
    enemy_jet_load_texture();
    enemy_light_load_texture();
    if (work->submode == SHOUKO_BOMBER_3 || work->submode == SHOUKO_BOMBER_2 ||
        work->submode == SHOUKO_BOMBER_1) {
        blast_load_texture();
    }
    if (shouko_holder == NULL) {
        texture_load_pvm(SHOUKO_TEXTURE_ARCHIVE, &shouko_texlist);
        shouko_holder = task_create(0, HOLDER_LEVEL, shouko_holder_step,
                                    SHOUKO_TEXTURE_TASK_NAME);
        shouko_holder->destructor = shouko_holder_destroy;
    }
    /* @hack redundant: a call no path reaches, whose taking of point's
     * address keeps point in the frame. SHC writes none of the call's code */
    if (0) {
        path_segment_at(route->path, 0.0F, &point);
    }
}

/* a hovering gunner turns toward player 0, within HOVER_TURN of its
 * HOVER_HEADING. any other shouko turns toward the way it flies */
static void shouko_turn(struct task_work *work, struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (work->mode == MODE_HOVER) {
        enemy_turn_to_player(work, enemy, 0);
        if (angle_distance(work->ang.y, HOVER_HEADING(enemy)) > HOVER_TURN) {
            if (angle_difference(HOVER_HEADING(enemy), work->ang.y) < 0) {
                work->ang.y = HOVER_HEADING(enemy) - HOVER_TURN;
            } else {
                work->ang.y = HOVER_HEADING(enemy) + HOVER_TURN;
            }
        }
        return;
    }
    enemy->aim_y =
        -(Angle)(atan2f(enemy->spd.z, enemy->spd.x) * ANGLE_PER_RADIAN);
    work->ang.y = angle_approach(work->ang.y, enemy->aim_y, enemy->turn_speed);
}

static void shouko_check_hit(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    NJS_VECTOR scl;
    int score;
    /* @hack spelling: player, a local for shot_by, where shot_by passed in
     * place would do */
    int player;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        enemy_break(work, shouko_pieces, &shouko_texlist);
        score = SCORE;
        if (player_touch_attacks(task)) {
            score = SCORE_KNOCK;
        }
        score_add_enemy(score);
        enemy_defeat(work, enemy);
        if (enemy->flags & ENEMY_IN_MAGIC_HAND) {
            player = enemy->shot_by;
            /* @hack spelling: one assignment to the three, where a statement
             * for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            enemy_ball_create(player, &shouko_object, &shouko_texlist, &scl,
                              NULL);
        }
    }
}

/* move the shouko a step as its mode says. the ride ends at the path's last
 * segment. a gunner then moves to the segment's first point and brakes. a
 * bomber flies on, moving each step by spd, which holds its last step along
 * the path */
static void shouko_move(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    if (game_state.time_stopped) {
        return;
    }
    if (work->mode == MODE_RIDE) {
        struct path_sample sample;
        /* @hack redundant: unused, an array nothing reads, which sizes the
         * frame */
        NJS_VECTOR unused[4];
        NJS_VECTOR dir;
        unsigned int index;
        struct enemy_route *route;
        struct path *path;

        route = task->extra.p;
        path = route->path;
        DISTANCE(enemy) += RIDE_SPEED;
        sample.distance = DISTANCE(enemy);
        if (path_segment_at(path, DISTANCE(enemy), &index) == 0 ||
            index >= (unsigned int)LAST_SEGMENT(path) ||
            path_get_sample(path, &sample) == 0) {
            if (work->submode == SHOUKO_GUNNER) {
                work->mode = MODE_BRAKE;
                path_point_pos(path, LAST_SEGMENT(path), &sample.pos);
                njPushMatrixEx();
                njUnitMatrix(NULL);
                njTranslateEx(&enemy->home);
                njRotateY(NULL, PATH_TURN(enemy));
                njCalcPoint(NULL, &sample.pos, &work->pos);
                njPopMatrixEx();
                return;
                /* @hack spelling: the bomber's lines in an else after the
                 * gunner's return, where the lines after the if would do */
            } else {
                work->mode = MODE_FLY_ON;
                njAddVector(&work->pos, &enemy->spd);
                return;
            }
        }
        /* @hack redundant: a second path_get_sample at the same distance,
         * where the sample the test took would do */
        sample.distance = DISTANCE(enemy);
        path_get_sample(path, &sample);
        /* place the sample in the world: the path's points are offsets from
         * enemy->home, turned about y by PATH_TURN(enemy) */
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslateEx(&enemy->home);
        njRotateY(NULL, PATH_TURN(enemy));
        njCalcPoint(NULL, &sample.pos, &work->pos);
        njPopMatrixEx();
        enemy->spd = work->pos;
        njSubVector(&enemy->spd, &enemy->last_pos);
        /* tilt the shouko as the path's up vector leans, then head it along
         * spd */
        dir.x = 0.0F;
        dir.y = 1.0F;
        dir.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, PATH_TURN(enemy));
        njRotateZ(NULL, sample.ang_z);
        njRotateX(NULL, sample.ang_x);
        njCalcVector(NULL, &dir, &dir);
        work->ang.x = (Angle)(asinf(dir.z) * ANGLE_PER_RADIAN);
        work->ang.z = -(Angle)(atan2f(dir.x, dir.y) * ANGLE_PER_RADIAN);
        dir = enemy->spd;
        njUnitMatrix(NULL);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njInvertMatrix(NULL);
        njCalcVector(NULL, &dir, &dir);
        work->ang.y = -(Angle)(atan2f(dir.z, dir.x) * ANGLE_PER_RADIAN);
        njPopMatrixEx();
    } else if (work->mode == MODE_FLY_ON) {
        njAddVector(&work->pos, &enemy->spd);
    } else if (work->mode == MODE_APPROACH) {
        NJS_VECTOR way;

        way = work->scl;
        njSubVector(&way, &work->pos);
        if (njScalor(&way) < APPROACH_SPEED) {
            work->pos = work->scl;
            enemy->spd.x = 0.0F;
            enemy->spd.y = 0.0F;
            enemy->spd.z = 0.0F;
            work->mode = MODE_HOVER;
            HOVER_HEADING(enemy) = work->ang.y;
            return;
            /* @hack spelling: the approach's lines in an else after the
             * arrival's return, where the lines after the if would do */
        } else {
            njUnitVector(&way);
            way.x *= APPROACH_SPEED;
            way.y *= APPROACH_SPEED;
            way.z *= APPROACH_SPEED;
            enemy->spd = way;
        }
        njAddVector(&work->pos, &enemy->spd);
    } else if (work->mode == MODE_BRAKE && work->submode == SHOUKO_GUNNER) {
        /* @hack spelling: spd, a copy of the shouko's spd worked on and
         * stored back, where the shouko's spd worked on in place would do */
        NJS_VECTOR spd;
        Float speed;

        spd = enemy->spd;
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((speed = njScalor(&spd)) < BRAKE_END) {
            enemy->spd.x = 0.0F;
            enemy->spd.y = 0.0F;
            enemy->spd.z = 0.0F;
            work->mode = MODE_APPROACH;
            return;
            /* @hack spelling: the braking's lines in an else after the
             * stop's return, where the lines after the if would do */
        } else {
            /* @hack spelling: += of the negated step, where -= would do */
            speed += -BRAKE_STEP;
            njUnitVector(&spd);
            spd.x *= speed;
            spd.y *= speed;
            spd.z *= speed;
            enemy->spd = spd;
        }
        njAddVector(&work->pos, &enemy->spd);
    }
}

static void shouko_destroy(struct task *task)
{
    enemy_destroy(task);
}

/* move point from the shouko's own frame into the world's */
static void shouko_calc_point(struct task_work *work, NJS_POINT3 *point)
{
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcPoint(NULL, point, point);
    njPopMatrixEx();
}

/* turn vector from the shouko's own frame into the world's */
static void shouko_calc_vector(struct task_work *work, NJS_VECTOR *vector)
{
    njPushMatrix(&_nj_unit_matrix_);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njCalcVector(NULL, vector, vector);
    njPopMatrixEx();
}

/* turn spd toward the first collision shape of the player nearest the
 * shouko, as seen from pos. the turn stays within AIM_PITCH and AIM_YAW. spd
 * must point along x. a shape at pos gets a level shot along the heading */
static void shouko_aim(struct task_work *work, NJS_POINT3 *pos, NJS_VECTOR *spd)
{
    NJS_POINT3 target;
    Float distance;
    /* @hack spelling: rise, a local for the target's height over pos, where
     * the difference written in place would do */
    Float rise;
    Angle ang_z;
    Angle ang_y;
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

/* fire a bullet from each gun at the player nearest the shouko */
static void shouko_fire(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    pos.x = GUN_X;
    pos.y = GUN_Y;
    pos.z = GUN_Z;
    shouko_calc_point(work, &pos);
    spd.x = BULLET_SPEED;
    spd.y = 0.0F;
    spd.z = 0.0F;
    shouko_aim(work, &pos, &spd);
    shouko_bullet_fire(task, &spd, &pos);
    pos.x = GUN_X;
    pos.y = GUN_Y;
    pos.z = -GUN_Z;
    shouko_calc_point(work, &pos);
    spd.x = BULLET_SPEED;
    spd.y = 0.0F;
    spd.z = 0.0F;
    shouko_aim(work, &pos, &spd);
    shouko_bullet_fire(task, &spd, &pos);
}

static void shouko_drop(struct task *task, struct task_work *work,
                        struct enemy_work *enemy)
{
    NJS_POINT3 pos;

    pos.x = HATCH_X;
    pos.y = 0.0F;
    pos.z = 0.0F;
    shouko_calc_point(work, &pos);
    shouko_bomb_drop(task, &pos, &enemy->target, SPREAD(enemy),
                     NEXT_DROP(enemy));
}

/* send a jet from each engine. a shouko slower than JET_SLOWER sends its
 * jets backward */
static void shouko_send_jets(struct task_work *work, NJS_VECTOR *spd)
{
    NJS_POINT3 pos;
    NJS_VECTOR jet_spd;

    if (game_state.time_stopped) {
        return;
    }
    pos.x = ENGINE_X;
    pos.y = ENGINE_Y;
    pos.z = ENGINE_Z;
    shouko_calc_point(work, &pos);
    jet_spd.x = njScalor(spd) - JET_SLOWER;
    jet_spd.y = 0.0F;
    jet_spd.z = 0.0F;
    shouko_calc_vector(work, &jet_spd);
    enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &jet_spd);
    pos.x = ENGINE_X;
    pos.y = ENGINE_Y;
    pos.z = -ENGINE_Z;
    shouko_calc_point(work, &pos);
    enemy_jet_create(NULL, ENEMY_JET_LARGE, &pos, &jet_spd);
}

/* the step of MODE_RIDE, MODE_FLY_ON, MODE_BRAKE and MODE_APPROACH. a gunner
 * fires as it approaches the target, and a bomber drops its bombs as it
 * rides */
static void shouko_ride_step(struct task *task, struct task_work *work,
                             struct enemy_work *enemy)
{
    if (work->mode == MODE_APPROACH) {
        shouko_turn(work, enemy);
    }
    shouko_move(task, work, enemy);
    shouko_check_hit(task, work, enemy);
    if (work->mode == MODE_HIT) {
        return;
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    if (work->mode == MODE_APPROACH && (work->timer & FIRE_BIT)) {
        if (RELOAD(enemy)++ > RELOAD_STEPS) {
            shouko_fire(task, work, enemy);
            RELOAD(enemy) = 0;
        }
    }
    if (work->mode != MODE_RIDE) {
        return;
    }
    /* @hack spelling: a test of each bomber kind inside the test of the one
     * before, where one test of the three would do */
    if (work->submode != SHOUKO_BOMBER_3) {
        if (work->submode != SHOUKO_BOMBER_2) {
            if (work->submode != SHOUKO_BOMBER_1) {
                return;
            }
        }
    }
    if (NEXT_DROP(enemy) == FIRST_DROP &&
        DISTANCE(enemy) >= DROP_START(enemy) + FIRST_DROP_AT) {
        if (work->submode == SHOUKO_BOMBER_3 ||
            work->submode == SHOUKO_BOMBER_2) {
            shouko_drop(task, work, enemy);
        }
        NEXT_DROP(enemy) = SECOND_DROP;
    } else if (NEXT_DROP(enemy) == SECOND_DROP &&
               DISTANCE(enemy) >= DROP_START(enemy) + SECOND_DROP_AT) {
        if (work->submode == SHOUKO_BOMBER_3 ||
            work->submode == SHOUKO_BOMBER_1) {
            shouko_drop(task, work, enemy);
        }
        NEXT_DROP(enemy) = THIRD_DROP;
    } else if (NEXT_DROP(enemy) == THIRD_DROP &&
               DISTANCE(enemy) >= DROP_START(enemy) + THIRD_DROP_AT) {
        if (work->submode == SHOUKO_BOMBER_3 ||
            work->submode == SHOUKO_BOMBER_2) {
            shouko_drop(task, work, enemy);
        }
        NEXT_DROP(enemy) = DROPS;
    }
}

static void shouko_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: bob_sin, a local for the bob phase's sine, where njSin
     * read in place, before the phase turns, would do */
    Float bob_sin;

    bob_sin = njSin(BOB_PHASE(enemy));
    BOB_PHASE(enemy) += BOB_TURN;
    work->pos.y = enemy->target.y + (BOB_HEIGHT * bob_sin);
}

/* the step of MODE_HOVER. unlike MODE_APPROACH's step, it counts RELOAD while
 * the timer lacks FIRE_BIT. so after steps without FIRE_BIT, a hovering
 * gunner fires at the first step that has FIRE_BIT */
static void shouko_hover_step(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    shouko_turn(work, enemy);
    shouko_bob(work, enemy);
    shouko_check_hit(task, work, enemy);
    if (work->mode == MODE_HIT) {
        return;
    }
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
    if (RELOAD(enemy)++ > RELOAD_STEPS && (work->timer & FIRE_BIT)) {
        shouko_fire(task, work, enemy);
        RELOAD(enemy) = 0;
    }
}

/* fun_shouko_0d40 isn't static, though nothing calls it and no header
 * declares it. SHC leaves out a static function that nothing calls, and the
 * image holds it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void fun_shouko_0d40(struct task *task)
{
    struct enemy_route *route;
    struct task_work *work;
    struct path *path;

    route = task->extra.p;
    /* @hack redundant: a loop that never runs, whose read of route keeps
     * route's load of task's extra */
    while (0) {
        work = task->work;
        path = route->path;
        work->mode = path->count;
        work->submode = path->flags;
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw the shouko's path as a triangle at each point but the last. the
 * triangles pulse and their colors cycle as route->frames counts up. only
 * code of shouko_draw that no path reaches calls it */
static void shouko_draw_route(struct task *task)
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
        /* @hack spelling: x's product written normal.x * length, where
         * length * normal.x would do, as for y and z */
        route_triangle[2].x = point->pos.x + (normal.x * length);
        route_triangle[2].y = point->pos.y + (length * normal.y);
        route_triangle[2].z = point->pos.z + (length * normal.z);
        njDrawPolygon3DExSetData(route_triangle, 3);
    }
    njDrawPolygon3DExEnd();
    njPopMatrixEx();
}

/* draw the shouko and its shadow, except in MODE_WAIT */
static void shouko_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct path *path;
    struct angles ang;
    NJS_VECTOR scl;
    Float size;
    int loc;

    light_apply(SHOUKO_LIGHT);
    if (work->mode != MODE_WAIT) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njSetTexture(&shouko_texlist);
        njCnkSimpleDrawObject(&shouko_object);
        njPopMatrixEx();
        draw_control_set(DRAW_CONTROL_SHADOW);
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njCnkModDrawObject(&shouko_shadow_object);
        njPopMatrixEx();
        draw_control_clear(DRAW_CONTROL_SHADOW);
    }
    /* @hack redundant: a block no branch reaches. SHC writes only the code of
     * the switch's cases and the call after them. the statements before the
     * switch hold strings the image places, and give the kept code its
     * registers */
    if (0) {
        size = work->scl.x;
        loc = work->ang.x;
        njPushMatrixEx();
        njTranslateEx(&enemy->target);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njRotateY(NULL, loc + QUARTER_TURN);
        njPrintD(loc, loc, (int)size + QUARTER_TURN);
        njPrintC(NJM_LOCATION(17, 16), SHOUKO_DEBUG_YAW);
        (void)SHOUKO_DEBUG_ID;
        njPrintC(loc, SHOUKO_DEBUG_PATH);
        njPrintC(loc, SHOUKO_DEBUG_KIND);
        njPrintC(loc, SHOUKO_DEBUG_KIND);
        njPrintC(NJM_LOCATION(17, 16), SHOUKO_DEBUG_SHOOTING_X);
        switch (work->submode) {
        case SHOUKO_GUNNER:
            njPrintC(NJM_LOCATION(17, 19), SHOUKO_DEBUG_SHOOTING_X);
            njPrintC(NJM_LOCATION(17, 20), SHOUKO_DEBUG_SHOOTING_Y);
            njPrintC(NJM_LOCATION(17, 21), SHOUKO_DEBUG_SHOOTING_Z);
            break;
        case SHOUKO_BOMBER_3:
        case SHOUKO_BOMBER_2:
        case SHOUKO_BOMBER_1:
            njPrintC(NJM_LOCATION(20, 15), SHOUKO_DEBUG_BOMB);
            njPrintD(NJM_LOCATION(25, 15), DROP_POINT(work), 3);
            njPrintC(NJM_LOCATION(28, 15), SHOUKO_DEBUG_BOMB_SEPARATOR);
            if (task->extra.p) {
                struct enemy_route *route = task->extra.p;

                path = route->path;
                njPrintD(NJM_LOCATION(29, 15), path->count, 3);
            }
            njPrintC(NJM_LOCATION(17, 18), SHOUKO_DEBUG_BOMBING_RADIUS);
            njPrintC(NJM_LOCATION(17, 19), SHOUKO_DEBUG_BOMBING_X);
            njPrintC(NJM_LOCATION(17, 20), SHOUKO_DEBUG_BOMBING_Y);
            njPrintC(NJM_LOCATION(17, 21), SHOUKO_DEBUG_BOMBING_Z);
            ang.x = ang.y = ang.z = 0;
            scl.z = scl.x = work->ang.z + 60.0F;
            scl.y = 1.0F;
            debug_draw_model_unlit(&work->scl, &ang, &scl,
                                   DEBUG_MODEL_CYLINDER);
            break;
        }
        shouko_draw_route(task);
    }
    light_apply(light_after_draw);
}

void shouko_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    /* @hack spelling: mode, a copy of the work's mode for the first test,
     * where the mode read in place would do */
    unsigned int mode;
    Float x;
    Float y;
    NJS_VECTOR vector;

    mode = work->mode;
    if (mode != MODE_INIT && mode != MODE_RIDE && task->child == NULL) {
        if (placement_try_leave(task)) {
            return;
        }
    }
    switch (work->mode) {
    case MODE_INIT:
        shouko_init(task, work);
        return;
    case MODE_RIDE:
        shouko_ride_step(task, work, enemy);
        shouko_send_jets(work, &enemy->spd);
        sound_play_held_at(SOUND_FLY, work, ENGINE_PRIORITY, SOUND_LOUDEST,
                           &work->pos);
        sound_pitch_by_pos(SOUND_FLY, work, NULL);
        break;
    /* @hack spelling: the case of MODE_FLY_ON written out apart, where one
     * case for both modes would do */
    case MODE_FLY_ON:
        shouko_ride_step(task, work, enemy);
        shouko_send_jets(work, &enemy->spd);
        sound_play_held_at(SOUND_FLY, work, ENGINE_PRIORITY, SOUND_LOUDEST,
                           &work->pos);
        sound_pitch_by_pos(SOUND_FLY, work, NULL);
        break;
    case MODE_APPROACH:
    case MODE_BRAKE:
        shouko_ride_step(task, work, enemy);
        shouko_send_jets(work, &enemy->spd);
        sound_play_held_at(SOUND_HOVER, work, ENGINE_PRIORITY, SOUND_LOUDEST,
                           &work->pos);
        break;
    case MODE_HOVER:
        shouko_hover_step(task, work, enemy);
        shouko_send_jets(work, &enemy->spd);
        sound_play_held_at(SOUND_HOVER, work, ENGINE_PRIORITY, SOUND_LOUDEST,
                           &work->pos);
        break;
    case MODE_HIT:
        work->mode = MODE_GONE;
        if (task->child) {
            return;
        }
        /* fall through */
    case MODE_GONE:
        /* @bug logic: kills a hit shouko whose bullets or bombs remain, and
         * they end with it. the step asks placement_try_leave only once they
         * are gone, but after a hit it waits for them one step only (the
         * tests of child at 0x1156 and 0x12cc of the object's code, and the
         * JSR to placement_kill at 0x12d2) */
        placement_kill(task);
        return;
        /* @hack redundant: a break after the return */
        break;
    case MODE_WAIT:
        if (enemy_switch_bits & (1 << SWITCH_NUMBER(work))) {
            work->mode = MODE_RIDE;
            placement_stop(task);
            shouko_move(task, work, enemy);
        }
        return;
    }
    /* @hack spelling: the empty then-arm and its else, where the test negated
     * would do */
    if (game_state.time_stopped) {
    } else {
        enemy_save_pos(work, enemy);
        /* @hack spelling: a goto to the next statement, where none would do */
        goto next;
    }
next:
    collision_register(task);
    /* @hack redundant: a block no branch reaches. SHC writes only its call to
     * shouko_init, which the image holds. its other statements size the
     * step's frame, and give the step the registers and floats it saves */
    if (0) {
        x = work->scl.x;
        y = work->scl.y;
        njScalor(&vector);
        path_segment_at(NULL, x, &mode);
        sound_play_held_at(0, work, 1, 127, &vector);
        work->scl.x = x + y;
        work->scl.y = y;
        switch (mode) {
        case 0:
            goto init;
        init:
            shouko_init(task, work);
            break;
        }
    }
}

void shouko_switch_init(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    work->submode = SWITCH_SHAPE(work);
    SWITCH_NUMBER(work) = work->ang.y & SWITCH_NUMBER_BITS;
    work->ang.x = 0;
    /* @hack spelling: the two masks written out, where &= would do */
    work->ang.y = work->ang.y & SWITCH_TURN_BITS;
    work->ang.z = work->ang.z & SWITCH_TURN_BITS;
    collision_init(task, &switch_shapes[work->submode], 1, COLLISION_OBJECT);
    work->collision->shapes->radius = work->scl.x;
    work->collision->shapes->size_y = work->scl.y;
    work->collision->shapes->size_z = work->scl.z;
    if (work->submode == SWITCH_BOX) {
        work->collision->shapes->ang.z = work->ang.z;
    }
    collision_fit_record_radius(work->collision);
    enemy_switch_bits &= ~(1 << SWITCH_NUMBER(work));
    task->step = shouko_switch_step;
    task->draw = shouko_switch_draw;
    task->destructor = shouko_switch_destroy;
}

/* draws nothing. the code behind the false tests would draw the switch's
 * shape and print its parameters */
static void shouko_switch_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: models, a local for switch_models, where
     * switch_models indexed in place would do */
    Sint8 *models = switch_models;
    int model;
    int group;
    struct angles ang;
    NJS_VECTOR scl;

    /* @hack redundant: 0 && 1.0F, a test that is always false, where an
     * empty function would do. SHC keeps the test and the code behind it */
    if (0 && 1.0F) {
        model = models[SWITCH_SHAPE(work)];
        group = work->ang.y & SWITCH_NUMBER_BITS;
        /* @hack redundant: a test of model no path reaches */
        if (0) {
            if (model) {
            }
        }
        if (model == DEBUG_MODEL_BOX) {
            ang.x = 0;
            ang.y = work->ang.y & SWITCH_TURN_BITS;
            ang.z = work->ang.z & SWITCH_TURN_BITS;
            scl = work->scl;
            scl.x *= 2.0F;
            scl.y *= 2.0F;
            scl.z *= 2.0F;
            njPushMatrixEx();
            njTranslateEx(&work->pos);
            njRotateY(NULL, ang.y);
            njRotateZ(NULL, ang.z);
            njScaleEx(&scl);
            njCnkEasyDrawObject(debug_models[model]);
            njPopMatrixEx();
            /* @hack redundant: 0 && 1.0F again, where no prints would do */
            if (0 && 1.0F) {
                njPrintC(NJM_LOCATION(17, 16), SHOUKO_DEBUG_SWITCH_BOX);
                njPrintC(NJM_LOCATION(17, 17), SHOUKO_DEBUG_SWITCH_YAW_AND_ID);
                njPrintD(NJM_LOCATION(30, 17), group, 2);
                njPrintC(NJM_LOCATION(17, 18), SHOUKO_DEBUG_SWITCH_ROLL);
                njPrintC(NJM_LOCATION(17, 19), SHOUKO_DEBUG_SWITCH_SCALE_X);
                njPrintC(NJM_LOCATION(17, 20), SHOUKO_DEBUG_SWITCH_SCALE_Y);
                njPrintC(NJM_LOCATION(17, 21), SHOUKO_DEBUG_SWITCH_SCALE_Z);
            }
        } else if (model == DEBUG_MODEL_SPHERE) {
            ang.x = ang.y = ang.z = 0;
            scl.x = scl.y = scl.z = work->scl.x;
            debug_draw_model_unlit(&work->pos, &ang, &scl, model);
            /* @hack redundant: 0 && 1.0F again, where no prints would do */
            if (0 && 1.0F) {
                njPrintC(NJM_LOCATION(17, 16), SHOUKO_DEBUG_SWITCH_SPHERE);
                njPrintC(NJM_LOCATION(17, 17), SHOUKO_DEBUG_SWITCH_ID);
                njPrintD(NJM_LOCATION(23, 17), group, 2);
                njPrintC(NJM_LOCATION(17, 19), SHOUKO_DEBUG_SWITCH_RADIUS);
            }
        }
    }
}

static void shouko_switch_destroy(struct task *task)
{
}

static void shouko_switch_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave(task)) {
        return;
    }
    if (game_state.time_stopped == 0 &&
        (work->collision->flags & COLLISION_TOUCHED) &&
        player_slot_of(work->collision->hit->owner) != -1) {
        enemy_switch_bits |= 1 << SWITCH_NUMBER(work);
        placement_kill(task);
        return;
    }
    collision_register(task);
}

/* a bullet's ends in the world, as an array. the front end lies in the bytes
 * of its work's ang. the back end lies in pos, which follows ang. scl holds
 * the bullet's velocity */
#define BULLET_POINTS(work) ((NJS_POINT3 *)&(work)->ang)

static void shouko_bullet_move(struct task_work *work)
{
    njAddVector(BULLET_POINTS(work), &work->scl);
    njAddVector(&work->pos, &work->scl);
}

static void shouko_bullet_draw(struct task *task)
{
    struct task_work *work = task->work;

    line_queue(BULLET_POINTS(work), bullet_colors, LINE_OPAQUE);
}

static void shouko_bullet_check_hit(struct task_work *work)
{
    /* @hack spelling: collision, a local for the work's record, where the
     * record read through work would do */
    struct collision_work *collision = work->collision;

    if ((work->flags & (WORK_HURT | WORK_UNK_8)) ||
        (collision->flags & COLLISION_PUSHED)) {
        work->mode = BULLET_HIT;
        work->collision->shapes->flags |= SHAPE_OFF;
        sound_play_at(SOUND_BULLET_HIT, NULL, 0, 0, &work->pos);
    }
}

static void shouko_bullet_destroy(struct task *task)
{
}

static void shouko_bullet_step(struct task *task)
{
    struct task_work *work = task->work;

    if (placement_try_leave_by(task, BULLET_RANGE2)) {
        return;
    }
    switch (work->mode) {
    /* @hack spelling: an empty default first, where none would do */
    default:
        break;
    case BULLET_FLY:
        shouko_bullet_check_hit(work);
        shouko_bullet_move(work);
        break;
    case BULLET_HIT:
        task_kill(task);
        return;
    }
    if (work->timer++ > BULLET_ARMED_STEPS) {
        collision_register(task);
    }
}

/* give a bullet its record and its two ends, which lie about pos along its
 * velocity. pos and scl must be set, and scl must not be 0, unchecked */
static void shouko_bullet_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;

    collision_init(task, &bullet_shape, BULLET_SHAPES, COLLISION_ENEMY);
    /* @hack spelling: the work read through task, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    enemy = enemy_work_alloc(task);
    collision_set_radius(task, njScalor(&work->scl) + BULLET_HALF_LENGTH);
    work->ang.x =
        (Angle)(asinf(-work->scl.y / njScalor(&work->scl)) * ANGLE_PER_RADIAN);
    work->ang.y = (Angle)(atan2f(work->scl.x, work->scl.z) * ANGLE_PER_RADIAN);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCalcPoints(NULL, bullet_ends, BULLET_POINTS(work), BULLET_ENDS);
    njPopMatrixEx();
    enemy->damage = 2.0F;
    task->destructor = shouko_bullet_destroy;
    task->draw = shouko_bullet_draw;
}

/* fire a bullet from pos along dir, as parent's child where parent isn't
 * null */
static void shouko_bullet_fire(struct task *parent, NJS_VECTOR *dir,
                               NJS_POINT3 *pos)
{
    struct task *bullet;

    if (parent != NULL) {
        bullet = task_create_child(TASK_WORK, shouko_bullet_step, parent);
    } else {
        bullet = task_create(TASK_WORK, BULLET_LEVEL, shouko_bullet_step,
                             SHOUKO_BULLET_TASK_NAME);
    }
    if (bullet != NULL) {
        bullet->work->pos = *pos;
        bullet->work->scl = *dir;
        shouko_bullet_init(bullet);
        gunfire_create(pos, dir);
    }
}

static void shouko_bomb_move(struct task_work *work, NJS_VECTOR *spd)
{
    if (game_state.time_stopped == 0) {
        /* @hack redundant: a call no path reaches. the image keeps work and
         * spd in the frame across it, and none of its code */
        if (0) {
            njPushMatrixEx();
        }
        njAddVector(&work->pos, spd);
    }
}

/* burst a bomb at pos. timed is TRUE where the bomb's tick ran out, and FALSE
 * where a hit burst it. in a one-player game, player 0's pad shakes harder
 * the nearer player 0 is to pos. player_work[0] must then exist, unchecked */
static void shouko_bomb_burst(Bool timed, NJS_POINT3 *pos)
{
    NJS_VECTOR spd;
    Float distance;
    int power;
    int freq;

    /* @hack spelling: a call in each arm, where one call with its kind
     * picked by timed would do */
    if (timed) {
        blast_create(BLAST_LARGE_RAISED, pos);
    } else {
        blast_create(BLAST_LARGE, pos);
    }
    spd.x = 0.0F;
    spd.y = 0.0F;
    spd.z = 0.0F;
    burst_create_wobbling(pos, &spd, PUFF_SCALE);
    sound_play_at(SOUND_BURST, NULL, 0, BURST_VOLUME, pos);
    if (game_state.two_players == 0) {
        distance = njDistanceP2P(pos, &player_work[0]->pos);
        if (distance > 400.0F) {
            power = 1;
            freq = 50;
        } else if (distance > 300.0F) {
            power = 2;
            freq = 50;
        } else if (distance > 200.0F) {
            power = 4;
            freq = 40;
        } else if (distance > 100.0F) {
            power = 5;
            freq = 25;
        } else {
            power = 7;
            freq = 18;
        }
        vibration_start(0, power, freq, VIBRATION_UNTIMED);
    }
}

static void shouko_bomb_draw(struct task *task)
{
    struct task_work *work = task->work;

    light_apply(SHOUKO_LIGHT);
    njSetTexture(&shouko_texlist);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkSimpleDrawObject(&bomb_object);
    njPopMatrixEx();
    light_apply(light_after_draw);
}

static void shouko_bomb_check_hit(struct task_work *work,
                                  struct enemy_work *enemy)
{
    if (work->flags & (WORK_HURT | WORK_UNK_8)) {
        RADIUS(work) = BURST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes[BURST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes->flags |= SHAPE_OFF;
        shouko_bomb_burst(FALSE, &work->pos);
        work->mode = BOMB_BURST;
    }
}

static void shouko_bomb_fall(struct task_work *work, struct enemy_work *enemy)
{
    if (work->timer > BOMB_ARMED_STEPS) {
        shouko_bomb_check_hit(work, enemy);
    }
    shouko_bomb_move(work, &enemy->spd);
    if (njDistanceP2P(&enemy->target, &work->pos) < LAND_DISTANCE) {
        work->pos = enemy->target;
        work->mode = BOMB_LANDED;
        work->timer = 0;
        work->tick = 0;
    }
}

static void shouko_bomb_draw_landed(struct task *task)
{
    struct task_work *work = task->work;

    njSetTexture(&shouko_texlist);
    draw_control_save();
    draw_attr_save();
    if (work->timer <= BOMB_FLASH_STEPS) {
        draw_control_clear(NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL |
                           NJD_CONTROL_3D_CONSTANT_ATTR);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        draw_attr_set(0, NJD_FST_IL | NJD_FST_IS);
        constant_material_set(1.0F, 2.0F, -2.0F, -2.0F);
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    njCnkSimpleDrawObject(&bomb_object);
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_control_restore();
    draw_attr_restore();
}

static void shouko_bomb_draw_nothing(struct task *task)
{
}

/* the step of BOMB_LANDED. the landing sets tick to 0, so a landed bomb
 * bursts at its first step here. nothing reads the scl.z this step lowers */
static void shouko_bomb_tick(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: state, a local for &game_state, where game_state read
     * in place would do */
    struct game_state *state;

    shouko_bomb_check_hit(work, enemy);
    state = &game_state;
    if (work->timer > work->tick) {
        work->timer = 0;
        if (work->tick > 0 && state->time_stopped == 0) {
            work->tick--;
        }
    }
    if (state->time_stopped == 0) {
        work->scl.z -= 0.003F;
    }
    if (work->scl.z < 0.0F) {
        work->scl.z = 0.0F;
    }
    /* @bug logic: bursts the bomb a second time where shouko_bomb_check_hit
     * has just burst it. this test of tick doesn't look at the mode the hit
     * set (the hit's burst at 0x1b02, the test at 0x1cda and the second
     * burst at 0x1d0e of the object's code) */
    if (work->tick < 1) {
        work->mode = BOMB_BURST;
        RADIUS(work) = BURST_START;
        LINGER(work) = 0.0F;
        work->collision->shapes[BURST_SHAPE].flags &= ~SHAPE_OFF;
        work->collision->shapes->flags |= SHAPE_OFF;
        shouko_bomb_burst(TRUE, &work->pos);
    }
}

/* grow the burst's sphere, then let it linger until the burst ends in
 * BOMB_DONE. nothing changes while game_state.time_stopped is set */
static void shouko_bomb_burst_step(struct task_work *work,
                                   struct enemy_work *enemy)
{
    struct collision_shape *sphere = &work->collision->shapes[BURST_SHAPE];

    if (game_state.time_stopped) {
        return;
    }
    if (RADIUS(work) < BURST_END) {
        RADIUS(work) += BURST_STEP;
        sphere->radius += BURST_STEP;
    } else {
        LINGER(work) -= LINGER_STEP;
        if (LINGER(work) < LINGER_END) {
            work->mode = BOMB_DONE;
        }
    }
    work->ang.y += BURST_SPIN;
}

static void shouko_bomb_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void shouko_bomb_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;

    if (task->parent == NULL && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case BOMB_FALL:
        shouko_bomb_fall(work, enemy);
        if (work->mode == BOMB_LANDED) {
            task->draw = shouko_bomb_draw_landed;
        } else if (work->mode == BOMB_BURST) {
            task->draw = shouko_bomb_draw_nothing;
        }
        break;
    case BOMB_LANDED:
        shouko_bomb_tick(work, enemy);
        if (work->mode == BOMB_BURST) {
            task->draw = shouko_bomb_draw_nothing;
        }
        break;
    case BOMB_BURST:
        shouko_bomb_burst_step(work, enemy);
        if (work->mode != BOMB_BURST) {
            task_kill(task);
            return;
        }
        break;
    }
    collision_register(task);
    if (game_state.time_stopped == 0) {
        work->timer++;
    }
}

/* give a bomb its records, and head it at BOMB_SPEED toward the point it
 * lands on. scl must hold that point, apart from pos, unchecked */
static void shouko_bomb_init(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy;
    /* @hack redundant: unused, a vector nothing reads, which sizes the
     * frame */
    NJS_VECTOR unused;

    collision_init(task, bomb_shapes, BOMB_SHAPES, COLLISION_ENEMY);
    /* @hack spelling: the work read through task, where work would do */
    task->work->collision->flags &= ~COLLISION_TARGET;
    collision_set_radius(task, RECORD_RADIUS);
    enemy = enemy_work_alloc(task);
    enemy->shadow_size = BOMB_SHADOW_SIZE;
    enemy->shadow_ratio = BOMB_SHADOW_RATIO;
    enemy->target = work->scl;
    enemy->spd = enemy->target;
    njSubVector(&enemy->spd, &work->pos);
    njUnitVector(&enemy->spd);
    enemy->spd.x *= BOMB_SPEED;
    enemy->spd.y *= BOMB_SPEED;
    enemy->spd.z *= BOMB_SPEED;
    work->ang.y =
        (Angle)(atan2f(enemy->spd.x, enemy->spd.z) * ANGLE_PER_RADIAN);
    work->ang.x = (Angle)(asinf(-enemy->spd.y / njScalor(&enemy->spd)) *
                          ANGLE_PER_RADIAN);
    work->timer = 0;
    RADIUS(work) = 1.0F;
    LINGER(work) = 1.0F;
    enemy->turn_speed = BOMB_TURN_SPEED;
    enemy->aim_y = work->ang.y;
    enemy->motion.frame = 0.0F;
    enemy->damage = 2.0F;
    work->scl.z = 1.0F;
    enemy_set_home(work, enemy);
    task->destructor = shouko_bomb_destroy;
    task->draw = shouko_bomb_draw;
}

/* drop a bomb from pos, as parent's child where parent isn't null. FIRST_DROP
 * lands it SPREAD_SCALE times spread short of target, and THIRD_DROP as far
 * past. any other drop lands it on target. for the first and the third, pos
 * must not lie straight above or below target, unchecked */
static void shouko_bomb_drop(struct task *parent, NJS_POINT3 *pos,
                             NJS_POINT3 *target, Float spread,
                             enum bomb_drop drop)
{
    struct task *bomb;
    NJS_VECTOR dir;
    NJS_POINT3 land;

    if (parent != NULL) {
        bomb = task_create_child(TASK_WORK, shouko_bomb_step, parent);
    } else {
        bomb = task_create(TASK_WORK, BOMB_LEVEL, shouko_bomb_step,
                           SHOUKO_BOMB_TASK_NAME);
    }
    if (bomb != NULL) {
        bomb->work->pos = *pos;
        dir = *target;
        land = *target;
        njSubVector(&dir, pos);
        dir.y = 0.0F;
        njUnitVector(&dir);
        if (drop == FIRST_DROP) {
            land.x -= dir.x * spread * SPREAD_SCALE;
            land.z -= dir.z * spread * SPREAD_SCALE;
        } else if (drop == THIRD_DROP) {
            land.x += dir.x * spread * SPREAD_SCALE;
            land.z += dir.z * spread * SPREAD_SCALE;
            /* @hack spelling: an empty else, where the if alone would do */
        } else {
        }
        bomb->work->scl = land;
        shouko_bomb_init(bomb);
    }
}
