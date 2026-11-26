/* the gold beetle, E GOLD in a stage's object list. the beetle hides until a
 * player comes in range. it then appears, hovers a while and vanishes */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/light.h"
#include "draw/lines.h"
#include "enemy/enemy.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/enemy_light.h"
#include "shared/gold_beetle.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#include "shared/gold_beetle/text.inc"

/* the beetle's modes, in its work's mode */
enum gold_beetle_mode {
    MODE_INIT,
    MODE_HOVER,
    MODE_HIDDEN,
    MODE_APPEAR,
    MODE_VANISH,
    MODE_GONE,
    MODE_DEAD
};

/* the bit of the work's flags that marks a player in range */
#define GOLD_BEETLE_ALERT 0x1000

/* an angle's high byte */
#define HIGH_BYTE 0xff00

/* what the beetle keeps in the fields of its enemy_work that enemy.h leaves
 * to each kind of enemy */
/* the hover ends once the work's timer passes HOVER_LIMIT */
#define HOVER_LIMIT(enemy) ((enemy)->unk_048)
/* the beetle's scale in x and z, which grows from 0 to 1 as it appears */
#define WARP_SCALE(enemy) ((enemy)->unk_098)
/* the steps the beetle has hovered, which its bob counts */
#define BOB_STEPS(enemy) ((enemy)->unk_1cc)
/* the bob's angle, which the beetle keeps and nothing reads */
#define BOB_ANGLE(enemy) ((enemy)->unk_1c4)
/* the constant material the beetle draws under as it appears or vanishes */
#define WARP_COLOR(enemy) ((enemy)->unk_0ac)

#define HOVER_LIMIT_SCALE 10
/* the beetle's one action, its index in gold_beetle_actions */
#define ACTION_HOVER 0

/* the beetle's range is scl.z + RANGE. sight_distance2 and near_distance2 hold
 * the range squared */
#define RANGE 60.0F
/* the spd.x the init gives the beetle, which never moves */
#define SPEED 0.4F
/* the beetle's turn toward a player in a step */
#define TURN_SPEED 0x180
#define SHADOW_SIZE 12.0F
#define SHADOW_RATIO 1.2F
/* the index in lights of the light the beetle draws under */
#define GOLD_BEETLE_LIGHT 8
/* the beetle's greatest turn in a step, as it appears or vanishes */
#define WARP_SPIN ((Float)EIGHTH_TURN)
/* how far WARP_SCALE, WARP_COLOR's alpha and its other channels move in a
 * step */
#define WARP_SCALE_STEP 0.1F
#define WARP_ALPHA_STEP 0.05F
#define WARP_TINT_STEP 0.1F
/* WARP_COLOR's alpha while the beetle can't be seen */
#define WARP_UNSEEN_ALPHA (-1.0F)
/* the alpha at or below which a vanishing beetle narrows */
#define WARP_NARROW_ALPHA (-0.5F)

/* the points score_add_enemy adds for a defeated beetle: SCORE, or
 * SCORE_KNOCK where player_touch_attacks returns TRUE */
#define SCORE 1000
#define SCORE_KNOCK 2000
/* the scale a defeated beetle's ball grows to */
#define BALL_SCALE 0.15F

/* the point at the beetle's front where it shows a light while a player is
 * in range */
#define FRONT_LIGHT_X 10.0F
#define FRONT_LIGHT_Y 4.6F
/* the front light's kind, as enemy_light_create takes it */
#define FRONT_LIGHT 1

#define HOVER_SOUND 0x400d
#define WARP_SOUND 0x400f
#define HOVER_PRIORITY 1
#define WARP_PRIORITY 0
/* the hover sound's volume, and its duration in sound updates. as the beetle
 * appears or vanishes, the volume is HOVER_VOLUME plus SOUND_LOUDEST times
 * WARP_COLOR's alpha */
#define HOVER_VOLUME 10
#define HOVER_SOUND_DURATION 5

#define GOLD_BEETLE_TEXTURES 3
static NJS_TEXNAME gold_beetle_texnames[GOLD_BEETLE_TEXTURES] = {
#include "shared/gold_beetle/gold_beetle_texnames.inc"
};
static NJS_TEXLIST gold_beetle_texlist = {
#include "shared/gold_beetle/gold_beetle_texlist.inc"
};
/* the beetle's model tree. its root, gold_beetle_object, is node 0 and has no
 * model. the root's children are nodes 1 to 5 in turn. a node's number is its
 * index in gold_beetle_motion's data */
#define NODES 6
#define NODE_5_POINTS 42
/* the heads of the strip chunks without NJD_FST_UA, each a halfword index
 * into its node's polygon list. NODE_5_STRIP_3 is node 5's third strip chunk.
 * while the beetle appears or vanishes, its draw sets NJD_FST_UA in each
 * head. every strip of the beetle then has it */
#define NODE_5_STRIP_3 46
#define NODE_5_STRIP_4 138
#define NODE_4_STRIP_1 10
#define NODE_3_STRIP_1 10
#define NODE_2_STRIP_1 10
#define NODE_2_STRIP_3 264
#define NODE_1_STRIP_1 8
/* the draw indexes the five polygon lists, so each is a flat array of
 * halfwords. each row holds a chunk. a lone 0 after a strip chunk's last strip
 * pads the chunk to whole words */
/* clang-format off */
static Sint16 gold_beetle_node_5_polygons[411] = {
#include "shared/gold_beetle/gold_beetle_node_5_polygons.inc"
};
/* clang-format on */
static Sint32 gold_beetle_node_5_vertices[255] = {
#include "shared/gold_beetle/gold_beetle_node_5_vertices.inc"
};
static NJS_CNK_MODEL gold_beetle_node_5_model = {
#include "shared/gold_beetle/gold_beetle_node_5_model.inc"
};
static NJS_CNK_OBJECT gold_beetle_node_5_object = {
#include "shared/gold_beetle/gold_beetle_node_5_object.inc"
};
#define NODE_4_POINTS 16
/* clang-format off */
static Sint16 gold_beetle_node_4_polygons[183] = {
#include "shared/gold_beetle/gold_beetle_node_4_polygons.inc"
};
/* clang-format on */
static Sint32 gold_beetle_node_4_vertices[99] = {
#include "shared/gold_beetle/gold_beetle_node_4_vertices.inc"
};
static NJS_CNK_MODEL gold_beetle_node_4_model = {
#include "shared/gold_beetle/gold_beetle_node_4_model.inc"
};
static NJS_CNK_OBJECT gold_beetle_node_4_object = {
#include "shared/gold_beetle/gold_beetle_node_4_object.inc"
};
#define NODE_3_POINTS 24
/* clang-format off */
static Sint16 gold_beetle_node_3_polygons[275] = {
#include "shared/gold_beetle/gold_beetle_node_3_polygons.inc"
};
/* clang-format on */
static Sint32 gold_beetle_node_3_vertices[147] = {
#include "shared/gold_beetle/gold_beetle_node_3_vertices.inc"
};
static NJS_CNK_MODEL gold_beetle_node_3_model = {
#include "shared/gold_beetle/gold_beetle_node_3_model.inc"
};
static NJS_CNK_OBJECT gold_beetle_node_3_object = {
#include "shared/gold_beetle/gold_beetle_node_3_object.inc"
};
#define NODE_2_POINTS 49
/* clang-format off */
static Sint16 gold_beetle_node_2_polygons[535] = {
#include "shared/gold_beetle/gold_beetle_node_2_polygons.inc"
};
/* clang-format on */
static Sint32 gold_beetle_node_2_vertices[297] = {
#include "shared/gold_beetle/gold_beetle_node_2_vertices.inc"
};
static NJS_CNK_MODEL gold_beetle_node_2_model = {
#include "shared/gold_beetle/gold_beetle_node_2_model.inc"
};
static NJS_CNK_OBJECT gold_beetle_node_2_object = {
#include "shared/gold_beetle/gold_beetle_node_2_object.inc"
};
#define NODE_1_POINTS 9
/* clang-format off */
static Sint16 gold_beetle_node_1_polygons[93] = {
#include "shared/gold_beetle/gold_beetle_node_1_polygons.inc"
};
/* clang-format on */
static Sint32 gold_beetle_node_1_vertices[57] = {
#include "shared/gold_beetle/gold_beetle_node_1_vertices.inc"
};
static NJS_CNK_MODEL gold_beetle_node_1_model = {
#include "shared/gold_beetle/gold_beetle_node_1_model.inc"
};
static NJS_CNK_OBJECT gold_beetle_node_1_object = {
#include "shared/gold_beetle/gold_beetle_node_1_object.inc"
};
static NJS_CNK_OBJECT gold_beetle_object = {
#include "shared/gold_beetle/gold_beetle_object.inc"
};
/* the beetle's one motion. it spins node 1 about y and raises and lowers
 * node 5. each of the two has a key at every frame */
#define MOTION_FRAMES 23
static NJS_MKEY_A gold_beetle_node_1_ang_keys[MOTION_FRAMES] = {
#include "shared/gold_beetle/gold_beetle_node_1_ang_keys.inc"
};
static NJS_MKEY_F gold_beetle_node_5_pos_keys[MOTION_FRAMES] = {
#include "shared/gold_beetle/gold_beetle_node_5_pos_keys.inc"
};
static NJS_MDATA2 gold_beetle_motion_data[NODES] = {
#include "shared/gold_beetle/gold_beetle_motion_data.inc"
};
static NJS_MOTION gold_beetle_motion = {
#include "shared/gold_beetle/gold_beetle_motion.inc"
};

#define GOLD_BEETLE_SHAPES 1
/* clang-format off */
static struct collision_shape gold_beetle_shapes[GOLD_BEETLE_SHAPES] = {
#include "shared/gold_beetle/gold_beetle_shapes.inc"
};
/* clang-format on */
static NJS_CNK_MODEL *gold_beetle_pieces[6] = {
#include "shared/gold_beetle/gold_beetle_pieces.inc"
};
static struct enemy_action gold_beetle_actions[1] = {
#include "shared/gold_beetle/gold_beetle_actions.inc"
};

static void gold_beetle_draw(struct task *task);
static void gold_beetle_destroy(struct task *task);

/* the task that keeps the beetle's texture list loaded, named ManTex, or null
 * while none does. it releases the list as it is destroyed */
static struct task *gold_beetle_textures = NULL;

static void gold_beetle_textures_release(struct task *task)
{
    njReleaseTexture(&gold_beetle_texlist);
    gold_beetle_textures = NULL;
}

static void gold_beetle_textures_step(struct task *task)
{
}

static void gold_beetle_init(struct task *task, struct task_work *work)
{
    /* @hack spelling: the local steps, where game_state.steps read at its
     * use would do */
    Uint32 steps = game_state.steps;
    struct enemy_work *enemy;

    enemy = enemy_work_alloc(task);
    work->submode = 0;
    collision_init(task, gold_beetle_shapes, GOLD_BEETLE_SHAPES,
                   COLLISION_ENEMY);
    HOVER_LIMIT(enemy) = (Uint8)work->ang.x * HOVER_LIMIT_SCALE;
    work->ang.z &= HIGH_BYTE;
    WARP_SCALE(enemy) = 0.0F;
    /* @hack spelling: the mask written out, where &= would do */
    work->ang.x = work->ang.x & HIGH_BYTE;
    enemy_motion_step(&enemy->motion);
    enemy_light_load_texture();
    enemy->sight_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->sight_angle = QUARTER_TURN;
    enemy->near_distance2 = (work->scl.z + RANGE) * (work->scl.z + RANGE);
    enemy->spd.x = SPEED;
    enemy->turn_speed = TURN_SPEED;
    BOB_ANGLE(enemy) = (steps * work->ang.z) + work->ang.x;
    enemy_set_home(work, enemy);
    enemy->motion.table = gold_beetle_actions;
    work->mode = MODE_HIDDEN;
    enemy->motion.object = &gold_beetle_object;
    enemy->motion.next = ACTION_HOVER;
    work->collision->shapes->flags |= SHAPE_OFF;
    enemy_motion_step(&enemy->motion);
    enemy->shadow_size = SHADOW_SIZE;
    enemy->shadow_ratio = SHADOW_RATIO;
    /* @hack spelling: the two bits set by two statements, where one |= would
     * do */
    enemy->flags |= ENEMY_LEAVES_CHAOS_DRIVE;
    enemy->flags |= ENEMY_BURST;
    task->draw = gold_beetle_draw;
    task->destructor = gold_beetle_destroy;
    if (gold_beetle_textures == NULL) {
        texture_load_pvm(GOLD_BEETLE_TEXTURE_ARCHIVE, &gold_beetle_texlist);
        gold_beetle_textures = task_create(0, 0, gold_beetle_textures_step,
                                           GOLD_BEETLE_TEXTURE_TASK_NAME);
        gold_beetle_textures->destructor = gold_beetle_textures_release;
    }
}

static void gold_beetle_turn(struct task_work *work, struct enemy_work *enemy)
{
    int player;

    if (game_state.time_stopped) {
        return;
    }
    if (work->flags & GOLD_BEETLE_ALERT) {
        player = player_nearest(&work->pos);
        if (player == 0 || player == 1) {
            enemy_turn_to_player(work, enemy, player);
        }
    }
}

static void gold_beetle_check_hit(struct task *task, struct task_work *work,
                                  struct enemy_work *enemy)
{
    NJS_VECTOR scl;
    int score;
    int player;

    if (enemy_take_hit(work, enemy)) {
        work->mode = MODE_GONE;
        work->collision->shapes->flags |= SHAPE_OFF;
        work->timer = 0;
        enemy_set_home(work, enemy);
        enemy_break(work, gold_beetle_pieces, &gold_beetle_texlist);
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
            /* @hack spelling: one assignment to scl's x, y and z, where a
             * statement for each would do */
            scl.x = scl.y = scl.z = BALL_SCALE;
            enemy_ball_create(player, &gold_beetle_object, &gold_beetle_texlist,
                              &scl, NULL);
        }
    }
}

static void gold_beetle_mark_player_found(struct task_work *work,
                                          struct enemy_work *enemy)
{
    if (enemy_find_player(work, enemy)) {
        work->flags |= GOLD_BEETLE_ALERT;
    } else {
        work->flags &= ~GOLD_BEETLE_ALERT;
    }
}

static void gold_beetle_bob(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local y, where njSin's result used in place, ahead
     * of the count's step, would do */
    Float y;

    y = njSin((BOB_STEPS(enemy) * work->ang.z) + work->ang.x);
    BOB_STEPS(enemy)++;
    work->pos.y = (y * work->scl.y) + enemy->home.y;
    BOB_ANGLE(enemy) += work->ang.z;
}

static void gold_beetle_destroy(struct task *task)
{
    enemy_destroy(task);
}

static void gold_beetle_hover(struct task *task, struct task_work *work,
                              struct enemy_work *enemy)
{
    gold_beetle_mark_player_found(work, enemy);
    gold_beetle_turn(work, enemy);
    gold_beetle_bob(work, enemy);
    gold_beetle_check_hit(task, work, enemy);
    if (work->mode == MODE_GONE) {
        return;
    }
    if (!game_state.time_stopped) {
        work->timer++;
    }
    if (work->timer > HOVER_LIMIT(enemy)) {
        work->mode = MODE_VANISH;
        work->timer = 0;
        work->flags &= ~GOLD_BEETLE_ALERT;
    }
}

static void gold_beetle_wait(struct task_work *work, struct enemy_work *enemy)
{
    gold_beetle_mark_player_found(work, enemy);
    /* @hack redundant: a test of work that no path reaches */
    if (0) {
        if (work) {
        }
    }
    if (work->flags & GOLD_BEETLE_ALERT) {
        work->mode = MODE_APPEAR;
        WARP_COLOR(enemy).a = WARP_UNSEEN_ALPHA;
        WARP_COLOR(enemy).r = 1.0F;
        WARP_COLOR(enemy).g = 1.0F;
        WARP_COLOR(enemy).b = 1.0F;
        /* @hack spelling: the mask written out, where &= would do */
        work->flags = work->flags & ~GOLD_BEETLE_ALERT;
        enemy->shadow_size = 0.0F;
        sound_play_at(WARP_SOUND, NULL, WARP_PRIORITY, SOUND_LOUDEST,
                      &work->pos);
    }
    gold_beetle_turn(work, enemy);
}

static void gold_beetle_appear(struct task_work *work, struct enemy_work *enemy)
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

static void gold_beetle_vanish(struct task_work *work, struct enemy_work *enemy)
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
        work->mode = MODE_GONE;
        work->collision->shapes->flags |= SHAPE_OFF;
    }
}

/* set home to pos, and raise home's y by the bob's offset at
 * game_state.steps. the image holds this function, though nothing calls it.
 * it is global, though no header declares it. SHC leaves out a static
 * function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void gold_beetle_rehome(struct task_work *work, struct enemy_work *enemy)
{
    /* @hack spelling: the local sine, where njSin's result used in place
     * would do */
    Float sine;
    /* @hack spelling: the local scale, where scl.y read in place would do */
    Float scale;

    /* @hack redundant: a call of njRoundOff whose result nothing reads */
    njRoundOff(work->scl.x);
    sine = njSin((game_state.steps * work->ang.z) + work->ang.x);
    scale = work->scl.y;
    enemy->home.x = work->pos.x;
    enemy->home.y = (scale * sine) + work->pos.y;
    enemy->home.z = work->pos.z;
    if (!game_state.time_stopped) {
        BOB_ANGLE(enemy) += work->ang.z;
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

static void gold_beetle_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    struct angles ang;
    NJS_VECTOR scl;

    light_apply(GOLD_BEETLE_LIGHT);
    njSetTexture(&gold_beetle_texlist);
    if (work->mode != MODE_HIDDEN) {
        if (work->mode == MODE_APPEAR || work->mode == MODE_VANISH) {
            constant_material_set(WARP_COLOR(enemy).a, WARP_COLOR(enemy).r,
                                  WARP_COLOR(enemy).g, WARP_COLOR(enemy).b);
            gold_beetle_node_5_polygons[NODE_5_STRIP_3] |= NJD_FST_UA;
            gold_beetle_node_5_polygons[NODE_5_STRIP_4] |= NJD_FST_UA;
            gold_beetle_node_4_polygons[NODE_4_STRIP_1] |= NJD_FST_UA;
            gold_beetle_node_3_polygons[NODE_3_STRIP_1] |= NJD_FST_UA;
            gold_beetle_node_2_polygons[NODE_2_STRIP_1] |= NJD_FST_UA;
            gold_beetle_node_2_polygons[NODE_2_STRIP_3] |= NJD_FST_UA;
            gold_beetle_node_1_polygons[NODE_1_STRIP_1] |= NJD_FST_UA;
        }
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y + QUARTER_TURN);
        njScale(NULL, WARP_SCALE(enemy), 1.0F, WARP_SCALE(enemy));
        if (enemy->motion.mode != ENEMY_MOTION_BLENDING) {
            njCnkSimpleDrawMotion(
                enemy->motion.object,
                enemy->motion.table[enemy->motion.action].motion,
                enemy->motion.frame);
        } else {
            njCnkSimpleDrawMotionLink(enemy->motion.object, &enemy->motion.link,
                                      enemy->motion.frame);
        }
        njPopMatrixEx();
        if (njFraction(work->scl.x) == 0.0F && enemy->shadow_size > 0.0F) {
            enemy_draw_shadow(work, enemy);
        }
        if (work->mode == MODE_APPEAR || work->mode == MODE_VANISH) {
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            gold_beetle_node_5_polygons[NODE_5_STRIP_3] &= ~NJD_FST_UA;
            gold_beetle_node_5_polygons[NODE_5_STRIP_4] &= ~NJD_FST_UA;
            gold_beetle_node_4_polygons[NODE_4_STRIP_1] &= ~NJD_FST_UA;
            gold_beetle_node_3_polygons[NODE_3_STRIP_1] &= ~NJD_FST_UA;
            gold_beetle_node_2_polygons[NODE_2_STRIP_1] &= ~NJD_FST_UA;
            gold_beetle_node_2_polygons[NODE_2_STRIP_3] &= ~NJD_FST_UA;
            gold_beetle_node_1_polygons[NODE_1_STRIP_1] &= ~NJD_FST_UA;
        }
    }
    /* @hack redundant: a block no path reaches, kept for its strings, which
     * SHC still places in this source's constants. its call takes the
     * addresses of ang and scl, which gives the function the image's stack
     * frame */
    if (0) {
        (void)GOLD_BEETLE_DEBUG_PHASE;
        (void)GOLD_BEETLE_DEBUG_EXISTENCE;
        (void)GOLD_BEETLE_DEBUG_FRAMES;
        (void)GOLD_BEETLE_DEBUG_SPEED;
        (void)GOLD_BEETLE_DEBUG_VERTICAL_RANGE;
        (void)GOLD_BEETLE_DEBUG_DETECTION_RANGE;
        (void)GOLD_BEETLE_DEBUG_YAW;
        (void)GOLD_BEETLE_DEBUG_ID;
        (void)GOLD_BEETLE_DEBUG_SHADOW_KIND;
        (void)GOLD_BEETLE_DEBUG_ON;
        (void)GOLD_BEETLE_DEBUG_OFF;
        debug_draw_model_unlit(&work->pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
    }
    light_apply(light_after_draw);
}

void gold_beetle_step(struct task *task)
{
    struct task_work *work = task->work;
    struct enemy_work *enemy = task->motion.p;
    NJS_POINT3 pos;

    if (work->mode != MODE_INIT && placement_try_leave(task)) {
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        gold_beetle_init(task, work);
        return;
    case MODE_HOVER:
        gold_beetle_hover(task, work, enemy);
        break;
    case MODE_GONE:
        work->mode = MODE_DEAD;
        /* fall through */
    case MODE_DEAD:
        placement_kill(task);
        return;
    case MODE_HIDDEN:
        gold_beetle_wait(work, enemy);
        break;
    case MODE_APPEAR:
        gold_beetle_appear(work, enemy);
        break;
    case MODE_VANISH:
        gold_beetle_vanish(work, enemy);
        if (work->mode != MODE_VANISH) {
            task->draw = NULL;
        }
        break;
    }
    if (!game_state.time_stopped) {
        enemy_save_pos(work, enemy);
        switch (work->mode) {
        case MODE_HOVER:
            sound_start_at_for(HOVER_SOUND, work, HOVER_PRIORITY, HOVER_VOLUME,
                               HOVER_SOUND_DURATION, &work->pos);
            break;
        case MODE_APPEAR:
        case MODE_VANISH:
            sound_start_at_for(HOVER_SOUND, work, HOVER_PRIORITY,
                               (SOUND_LOUDEST * WARP_COLOR(enemy).a) +
                                   HOVER_VOLUME,
                               HOVER_SOUND_DURATION, &work->pos);
            break;
        }
        enemy_motion_step(&enemy->motion);
        if (work->flags & GOLD_BEETLE_ALERT) {
            njPushMatrix(&_nj_unit_matrix_);
            njTranslateEx(&work->pos);
            njRotateY(NULL, work->ang.y);
            pos.x = FRONT_LIGHT_X;
            pos.y = FRONT_LIGHT_Y;
            pos.z = 0.0F;
            njCalcPoint(NULL, &pos, &pos);
            enemy_light_create(task, FRONT_LIGHT, &pos);
            njPopMatrixEx();
        }
    }
    if (work->mode != MODE_GONE) {
        collision_register(task);
    }
    /* @hack redundant: a call of gold_beetle_init in a switch no path
     * reaches. SHC still writes the call, at the end of the step's code */
    if (0) {
        switch (work->mode) {
        case MODE_INIT:
            gold_beetle_init(task, work);
            break;
        }
    }
}

/* the set editor's names for the beetle's actions, in gold_beetle_actions'
 * order */
/* The original SET readout retains these action names. */
static const char *gold_beetle_action_names[1] = {
#include "shared/gold_beetle/gold_beetle_action_names.inc"
};
