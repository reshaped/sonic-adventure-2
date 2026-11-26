/* the parts that ride the head of BOSS_GOLEM.PRS's Egg Golem. they are three
 * platforms, each with a target, and a model with a glow at the weak spot. the
 * head's platforms take platforms.c's modes and draw. the source also holds the
 * flash's drops and the rubble the golem's limbs throw. golem_create creates
 * the parts, and no object list names any of this.
 *
 * BOSS_GOLEM_E.PRS has no head parts, and boss_golem_e/weak_points.c holds its
 * rubble. that rubble holds seven pieces to a task where this one holds ten,
 * and draws without a fade */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_golem/attachment_art.h"
#include "boss_golem/golem.h"
#include "boss_golem/head_parts.h"
#include "boss_golem/platform_collision.h"
#include "boss_golem/platforms.h"
#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "shared/black_smoke.h"
#include "shared/blast.h"
#include "shared/golem.h"
#include "shared/golem_collision.h"
#include "shared/golem_util.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/math.h"
#include "system/matrix.h"
#include "system/task.h"
#include "system/util.h"

/* matrix_set reads a matrix in pairs of floats, which needs the matrix on an
 * eight-byte boundary */
#ifdef NONMATCHING
/* on the Dreamcast, this side's NJS_MATRIX type gives the boundary, as
 * nj_render_types.h says */
#else
#pragma aligndata8(golem_head_bases)
#endif
struct task *golem_head_tasks[GOLEM_HEAD_PARTS];
struct task *golem_head_targets[GOLEM_HEAD_PLATFORMS];
static NJS_MATRIX golem_head_bases[GOLEM_HEAD_PARTS];

#define PART_WORLD_FLAGS (WORLD_TURNED | WORLD_MOVING | WORLD_SOLID)
/* a range within which no player lies, for an object golem_head_parts_place
 * takes out of the world */
#define OUT_OF_WORLD (-1000.0F)

/* the point of the head's node where the head's glow and the smoke sit */
#define GLOW_X (-0.019805F)
#define GLOW_Y 102.32736F
#define GLOW_Z 10.183713F
#define SMOKE_RISE 0.1F
#define TILT_SWING 4096.0F
/* the glow's texture moves on a unit of texture_anim_apply's time each
 * 1 << GLOW_TIME_SHIFT steps */
#define GLOW_TIME_SHIFT 1

/* the steps a target stays down after a hit. the hit's sound plays for a key
 * of its own, an address 10 bytes into the platform's task */
#define TARGET_DOWN_STEPS 180
#define TARGET_SOUND 0x8015
#define TARGET_SOUND_KEY(platform) ((char *)(platform) + 10)

/* the flash's sound, as golem.c plays it */
#define FLASH_SOUND 8
#define DROP_GROWTH_KEEP 0.95F
#define DROP_ATTACK_LEVEL 2
#define TARGET_SHAPES 1
#define DROP_SHAPES 1

#define RUBBLE_PIECES 10
/* the bounds of a throw's random factors. the spins, the distances and the
 * puffs' speeds take one from SPREAD_LOW to SPREAD_LOW + SPREAD. a piece's
 * speed and its puff's scale take one from WIDE_LOW to WIDE_LOW + WIDE_SPREAD.
 * a piece's size takes one from SIZE_LOW to SIZE_LOW + SIZE_SPREAD. each spin
 * turns either way at even odds */
#define SPREAD_LOW 0.9F
#define SPREAD 0.2F
#define WIDE_LOW 0.8F
#define WIDE_SPREAD 0.4F
#define SIZE_LOW 0.75F
#define SIZE_SPREAD 0.5F
#define EVEN_ODDS 0.5F
/* the share of its size a piece loses over its life */
#define LIFE_SHRINK 0.5F

/* a drop of the flash, in its task's extra. the drop draws the flash's model
 * twice, the second a quarter turn about y from the first. draw i takes size_x
 * across and size_y up, each times scale[i] */
struct flash_drop {
    enum golem_flash_drop_kind kind;
    Float grow_x; /* what size_x gains a step */
    Float grow_y;
    Float size_x;
    Float size_y;
    NJS_VECTOR scale[2];
    Sint32 steps;
};
#define FLASH_DROP(task) ((struct flash_drop *)(task)->extra.p)

/* a piece of rubble, among RUBBLE_PIECES in its task's extra. it flies until
 * its life ends or it falls below y 0 */
struct rubble {
    NJS_CNK_MODEL *model;
    NJS_TEXLIST *texlist;
    Bool live;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;
    NJS_VECTOR spd;
    struct angles spin; /* what ang turns by in a step */
    Sint32 steps;
    Sint32 life; /* the steps the piece flies */
    /* the share of its life it stays opaque, which the integer truncates to
     * 0 */
    Sint32 fade_share;
    Bool smokes; /* TRUE where the piece leaves puffs of smoke */
};
/* @hack type: extra read as a void * through its address, where
 * (task)->extra.p would do */
#define RUBBLE(task) ((struct rubble *)*(void **)&(task)->extra)
/* the rubble's task counts its live pieces in its unk_3c */
#define LIVE_PIECES(task) ((task)->unk_3c.l)

static void head_model_destroy(struct task *task);
static void head_model_draw(struct task *task);
static void head_model_step(struct task *task);
static void head_target_step(struct task *task);
static void head_target_destroy(struct task *task);
static void flash_drop_destroy(struct task *task);
static void flash_drop_step(struct task *task);
static void flash_drop_draw(struct task *task);
static void rubble_destroy(struct task *task);
static void rubble_step(struct task *task);
static void rubble_draw(struct task *task);

/* clang-format off */
/* @hack type: the shape declared as an array of one, where the shape alone
 * would do */
static const struct collision_shape target_shape[TARGET_SHAPES] = {
    {0, SHAPE_SPHERE, 0, SHAPE_HURTS_PLAYERS, SHAPE_UNK_2000 | SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 10.0F, 0.0F, 0.0F, 0, {0, 0, 0}},
};
/* a hurting drop's shape. the drop's step scales its radius and size_y by the
 * drop's sizes */
static const struct collision_shape drop_shape[DROP_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(DROP_ATTACK_LEVEL), SHAPE_HURTABLE,
     {0.0F, 0.0F, 0.0F}, 12.5F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static NJS_OBJECT *collision_models[GOLEM_HEAD_PARTS] = {
#include "boss_golem/head_parts/collision_models.inc"
};
/* each part's range, which picks its object's size class and is the range
 * golem_head_parts_place gives golem_sensor_place */
static Float part_ranges[GOLEM_HEAD_PARTS] = {210.0F, 210.0F, 210.0F, 210.0F};
static Float target_radius = 7.5F;
/* each platform's home. B slides in to in_z_b, and A and C to 0 */
static Float home_x_a = -10.0F;
static Float home_x_b = 0.0F;
static Float home_x_c = 5.0F;
static Float home_z_a = -23.0F;
static Float home_z_b = -24.0F;
static Float home_z_c = -12.5F;
static NJS_CNK_OBJECT *platform_models[GOLEM_HEAD_PLATFORMS] = {
#include "boss_golem/head_parts/platform_models.inc"
};
static NJS_TEXLIST *platform_texlists[GOLEM_HEAD_PLATFORMS] = {
#include "boss_golem/head_parts/platform_texlists.inc"
};
/* the alpha of the glow at the weak spot */
static Float glow_alpha = 0.2F;
static Float return_rate = 0.005F;
static Float retract_rate = -0.0005F;
static Float retract_base = 0.004F;
static Float in_z_b = -5.0F;
/* the volumes of FLASH_SOUND for each kind of drop, and of TARGET_SOUND */
static Sint32 faint_drop_volume = 0;
static Sint32 drop_volume = 125;
static Sint32 target_volume = 75;
/* a piece of rubble's speed loses rubble_gravity a step to gravity. it then
 * keeps rubble_drag of its speed and rubble_spin_keep of its spin */
static Float rubble_gravity = 0.08F;
static Float rubble_drag = 0.9775F;
static Float rubble_spin_keep = 0.95F;
static Float dat_8c609c40 = 12.5F;
static Float dat_8c609c44 = 7.5F;
static Float dat_8c609c48 = -5.0F;
/* while the golem dies, the head model draws in a constant gray of
 * defeat_shade. every defeat_smoke_steps it gives off defeat_smoke_count puffs
 * of the size defeat_smoke_scale. they start on a ring of radius
 * defeat_smoke_radius about its glow */
static Float defeat_shade = 0.5F;
static Float defeat_smoke_radius = 10.0F;
static Float defeat_smoke_scale = 10.0F;
static Sint32 defeat_smoke_count = 5;
static Uint32 defeat_smoke_steps = 20;
/* a drop starts drop_height above the head sensor's place, at the sizes
 * drop_start_x and drop_start_y. its first step grows them by drop_grow_x and
 * drop_grow_y. each step keeps DROP_GROWTH_KEEP of the growth, which stops
 * after drop_grow_steps. the drop falls drop_fall a step, and ends once its
 * steps pass drop_life. for a faint drop, each faint_drop_ value takes the
 * place of the drop_ value of the same name */
static Float drop_grow_x = 0.55F;
static Float drop_grow_y = 0.2F;
static Float faint_drop_grow_x = 0.5F;
static Float faint_drop_grow_y = 0.15F;
static Float drop_fall = 3.5F;
static Float faint_drop_fall = 5.0F;
static Sint32 drop_grow_steps = 45;
static Sint32 drop_life = 60;
static Sint32 faint_drop_grow_steps = 40;
static Sint32 faint_drop_life = 50;
static Float drop_start_x = 1.0F;
static Float drop_start_y = 1.0F;
static Float drop_height = 100.0F;
/* each scale is drop_scale_low plus a random share of drop_scale_spread, and
 * the second takes drop_inner_scale of that */
static Float drop_inner_scale = 0.9F;
static Float drop_scale_spread = 0.1F;
static Float drop_scale_low = 0.95F;
static Float faint_drop_alpha = 0.2F;
/* @hack type: the spin unsigned, where a Sint32 would do */
static Uint32 drop_spin = 0x400;
/* a drop fades over the last drop_fade_steps of drop_life */
static Sint32 drop_fade_steps = 20;
/* a piece starts rubble_dist from the sensor's place. it flies at rubble_speed,
 * plus throw_share of the sensor's move. it spins rubble_spin a step about each
 * axis, at the size rubble_scale. it flies rubble_life steps, and stays opaque
 * for rubble_fade of them. the distance, the speed, the spins and the size
 * each take a random factor */
static Float rubble_dist = 10.0F;
static Float rubble_speed = 0.1F;
static Sint32 rubble_spin = 0x400;
static Sint32 rubble_life = 150;
static Float rubble_scale = 2.0F;
static Float rubble_fade = 0.333F;
/* each piece starts a puff of smoke at puff_speed_share of its speed. the puff
 * starts puff_dist_share of the piece's offset out from the sensor. it lies
 * puff_dist from there, at the size puff_scale */
static Float puff_dist_share = 2.5F;
static Float puff_speed_share = 1.0F;
static Float puff_dist = 20.0F;
static Float puff_scale = 15.0F;
/* a sensor that moves throw_move or less in a step throws nothing. one that
 * moves more throws with the chance throw_chance */
static Float throw_move = 2.0F;
static Float throw_share = 0.2F;
static Float throw_chance = 0.0333F;
/* a throw has from 1 to throw_pieces - 1 pieces. a piece smokes with the chance
 * smoke_chance */
static Sint32 throw_pieces = 2;
static Float smoke_chance = 1.0F;
/* each step a smoking piece starts a puff with the chance puff_chance */
static Float puff_chance = 0.1F;
static Float flight_puff_scale = 10.0F;
/* no throw starts while more pieces than rubble_limit fly */
static Sint32 rubble_limit = 32;

#ifdef NONMATCHING
/* this side returns whether it made every part, so that golem_create can
 * destroy an unfinished golem */
Bool golem_head_parts_create(struct task *golem_task)
#else
void golem_head_parts_create(struct task *golem_task)
#endif
{
    struct task *parts[GOLEM_HEAD_PARTS];
    struct task *targets[GOLEM_HEAD_PLATFORMS];
    /* @hack type: the shape declared as an array of one, where the shape alone
     * would do */
    struct collision_shape shapes[TARGET_SHAPES];
    /* @hack redundant: twelve bytes of the frame that no code touches */
    NJS_POINT3 unused;
    NJS_OBJECT *object;
    int i;
    Sint32 flags;

    for (i = 0; i < GOLEM_HEAD_PARTS; i++) {
        object = world_pool_alloc();
        /* @bug memory: writes through a null object where world_pool_alloc has
         * none left to lend. the destructors test a part's object, the
         * platforms' at 0x8c50346e and the head model's at 0x8c50458c, but the
         * copy of the model writes through it at 0x8c503c36 */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it returns FALSE where the
         * pool has no object */
        if (object == NULL) {
            return FALSE;
        }
#endif
        if (i < GOLEM_HEAD_PLATFORMS) {
            parts[i] = task_create_child(TASK_WORK | TASK_FORCE,
                                         golem_platform_step, golem_task);
#ifdef NONMATCHING
            /* this side gives the object back and returns FALSE where
             * task_create_child fails. the matching side writes through the
             * null */
            if (parts[i] == NULL) {
                world_pool_free(object);
                return FALSE;
            }
#endif
            parts[i]->destructor = golem_platform_destroy;
            parts[i]->draw = golem_platform_draw;
            parts[i]->extra.p = syMalloc(sizeof(struct golem_platform));
#ifdef NONMATCHING
            /* this side gives the object back and returns FALSE where the heap
             * has no platform record. the matching side writes through the
             * null */
            if (parts[i]->extra.p == NULL) {
                world_pool_free(object);
                return FALSE;
            }
#endif
            GOLEM_PLATFORM(parts[i])->retract_rate = retract_rate;
            GOLEM_PLATFORM(parts[i])->retract_base = retract_base;
            GOLEM_PLATFORM(parts[i])->return_rate = return_rate;
            GOLEM_PLATFORM(parts[i])->glows = TRUE;
            GOLEM_PLATFORM(parts[i])->target_down = FALSE;
            GOLEM_PLATFORM(parts[i])->target_down_steps = 0;
            GOLEM_PLATFORM(parts[i])->index = i;
            GOLEM_PLATFORM(parts[i])->sensor = GOLEM_SENSOR_HEAD;
            GOLEM_PLATFORM(parts[i])->object = platform_models[i];
            GOLEM_PLATFORM(parts[i])->texlist = platform_texlists[i];
            golem_platform_set_mode(parts[i], GOLEM_PLATFORM_MODE_WAIT);
        } else {
            parts[i] = task_create_child(TASK_WORK | TASK_FORCE,
                                         head_model_step, golem_task);
#ifdef NONMATCHING
            /* this side gives the object back and returns FALSE where
             * task_create_child fails. the matching side writes through the
             * null */
            if (parts[i] == NULL) {
                world_pool_free(object);
                return FALSE;
            }
#endif
            parts[i]->destructor = head_model_destroy;
            parts[i]->draw = head_model_draw;
        }
        golem_head_tasks[i] = parts[i];
        /* @hack spelling: the task read again from golem_head_tasks, where
         * parts[i] would do */
        golem_head_tasks[i]->motion.p = object;
        *object = *collision_models[i];
        object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
        object->scl[0] = object->scl[1] = object->scl[2] = 1.0F;
        object->ang[0] = object->ang[1] = object->ang[2] = 0;
        object->pos[0] = object->pos[1] = object->pos[2] = 0.0F;
        flags = PART_WORLD_FLAGS;
        flags |= golem_size_class_flags(part_ranges[i]);
        world_add(flags, parts[i], object);
    }
    for (i = 0; i < GOLEM_HEAD_PLATFORMS; i++) {
        targets[i] = task_create_child(TASK_WORK, head_target_step, parts[i]);
#ifdef NONMATCHING
        /* this side returns FALSE where task_create_child fails. the matching
         * side writes through the null */
        if (targets[i] == NULL) {
            return FALSE;
        }
#endif
        targets[i]->destructor = head_target_destroy;
        shapes[0] = target_shape[0];
        golem_platform_target_pos(platform_models[i],
                                  &GOLEM_PLATFORM(parts[i])->target_pos);
        shapes[0].radius = target_radius;
        /* @hack redundant: the enemy work stored in motion, where
         * enemy_work_alloc_grouped has stored it already */
        targets[i]->motion.p = enemy_work_alloc_grouped(targets[i]);
#ifdef NONMATCHING
        /* this side returns FALSE where enemy_work_alloc_grouped gives null,
         * which the matching side goes on with */
        if (targets[i]->motion.p == NULL) {
            return FALSE;
        }
#endif
        collision_init(targets[i], shapes, TARGET_SHAPES, COLLISION_ITEM);
#ifdef NONMATCHING
        /* this side returns FALSE where collision_init gives the target no
         * record */
        if (targets[i]->work->collision == NULL) {
            return FALSE;
        }
#endif
        golem_head_targets[i] = targets[i];
    }
    parts[0]->work->id = GOLEM_PLATFORM(parts[0])->id =
        WORK_ID_GOLEM_HEAD_PLATFORM_A;
    parts[1]->work->id = GOLEM_PLATFORM(parts[1])->id =
        WORK_ID_GOLEM_HEAD_PLATFORM_B;
    parts[2]->work->id = GOLEM_PLATFORM(parts[2])->id =
        WORK_ID_GOLEM_HEAD_PLATFORM_C;
    GOLEM_PLATFORM(parts[0])->home_z = GOLEM_PLATFORM(parts[0])->z = home_z_a;
    GOLEM_PLATFORM(parts[1])->home_z = GOLEM_PLATFORM(parts[1])->z = home_z_b;
    GOLEM_PLATFORM(parts[2])->home_z = GOLEM_PLATFORM(parts[2])->z = home_z_c;
    GOLEM_PLATFORM(parts[0])->home_x = GOLEM_PLATFORM(parts[0])->x = home_x_a;
    GOLEM_PLATFORM(parts[1])->home_x = GOLEM_PLATFORM(parts[1])->x = home_x_b;
    GOLEM_PLATFORM(parts[2])->home_x = GOLEM_PLATFORM(parts[2])->x = home_x_c;
    GOLEM_PLATFORM(parts[1])->in_z = in_z_b;
    GOLEM_PLATFORM(parts[0])->in_z = GOLEM_PLATFORM(parts[2])->in_z = 0.0F;
#ifdef NONMATCHING
    /* this side returns that it made every part */
    return TRUE;
#endif
}

static void head_model_destroy(struct task *task)
{
    if (task->motion.p) {
        world_remove(task, task->motion.p);
        world_pool_free(task->motion.p);
    }
    task->motion.p = NULL;
}

static void head_model_draw(struct task *task)
{
    texture_anim_apply(golem_flash_object->model, &golem_flash_anim,
                       game_state.steps >> GLOW_TIME_SHIFT);
    draw_control_set(DRAW_CONTROL_SHADOW);
    njPushMatrixEx();
    matrix_multiply_before(&golem_sensor_matrices[GOLEM_SENSOR_HEAD]);
    njSetTexture(&golem_head_model_texlist);
    if (golem->action == GOLEM_ACTION_DIE) {
        draw_control_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(NJD_CONTROL_3D_CNK_BLEND_MODE |
                         NJD_CONTROL_3D_CONSTANT_MATERIAL);
        constant_material_set(1.0F, defeat_shade, defeat_shade, defeat_shade);
        njCnkEasyDrawModel(golem_head_model_object->model);
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_control_restore();
    } else {
        njCnkEasyDrawModel(golem_head_model_object->model);
    }
    njPushMatrixEx();
    njTranslate(NULL, GLOW_X, GLOW_Y, GLOW_Z);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
    constant_material_set(glow_alpha, 1.0F, 1.0F, 1.0F);
    njSetConstantAttr(~NJD_FST_IL, 0);
    njSetTexture(&golem_flash_texlist);
    njCnkEasyDrawModel(golem_flash_object->model);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
    njPopMatrixEx();
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void head_model_step(struct task *task)
{
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    if (golem->action == GOLEM_ACTION_DIE &&
        game_state.steps % defeat_smoke_steps == 0) {
        njPushMatrixEx();
        matrix_set(&golem_sensor_matrices[GOLEM_SENSOR_HEAD]);
        njTranslate(NULL, GLOW_X, GLOW_Y, GLOW_Z);
        njGetTranslation(NULL, &pos);
        njPopMatrixEx();
        spd.x = spd.z = 0.0F;
        spd.y = SMOKE_RISE;
        black_smoke_rising_burst(&pos, &spd, defeat_smoke_radius,
                                 defeat_smoke_scale, defeat_smoke_count);
    }
}

/* a target rides its point on its platform, and collides. a hit takes it down:
 * it neither moves nor collides, and it loses its COLLISION_TARGET mark. its
 * platform doesn't glow while it is down */
static void head_target_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *platform = task->parent;
    struct collision_work *collision = work->collision;
    NJS_POINT3 pos;

    if (!GOLEM_PLATFORM(platform)->target_down) {
        if (enemy_take_hit(work, task->motion.p)) {
            work->flags &= ~WORK_HURT;
            collision->flags &= ~COLLISION_TOUCHED;
            GOLEM_PLATFORM(platform)->target_down = TRUE;
            GOLEM_PLATFORM(platform)->target_down_steps = 0;
            /* @hack spelling: the record read again through work, where
             * collision would do */
            work->collision->flags &= ~COLLISION_TARGET;
            sound_play(TARGET_SOUND, TARGET_SOUND_KEY(platform), 0, 0);
            sound_set_volume(TARGET_SOUND_KEY(platform), SOUND_ANY_ID,
                             target_volume);
            blast_create(BLAST_SMALL_RAISED, &work->pos);
        }
        njPushMatrixEx();
        matrix_set(&golem_head_matrices[GOLEM_PLATFORM(platform)->index]);
        njTranslateEx(&GOLEM_PLATFORM(platform)->target_pos);
        njGetTranslation(NULL, &pos);
        current_matrix_angles_yxz(&work->ang.x, &work->ang.y, &work->ang.z);
        njPopMatrixEx();
        work->pos = pos;
        collision_register(task);
    } else {
        work->flags &= ~WORK_HURT;
        if (++GOLEM_PLATFORM(platform)->target_down_steps >=
            TARGET_DOWN_STEPS) {
            GOLEM_PLATFORM(platform)->target_down = FALSE;
            work->collision->flags |= COLLISION_TARGET;
        }
    }
}

static void head_target_destroy(struct task *task)
{
    if (task->motion.p) {
        enemy_destroy(task);
    }
}

void golem_head_parts_place(void)
{
    struct player *player = player_tasks[0]->extra.p;
    /* @bug logic: only the first platform to search finds a target. target
     * starts at the list's head once, at 0x8c5049c2, and each search leaves it
     * at the list's end, at 0x8c504a38. the later platforms' searches find
     * nothing, and their objects never leave the world */
    struct near_target *target = near_targets;
    struct task_work *nearest;
    Float nearest_distance;
    Float range;
    int i;

    for (i = 0; i < GOLEM_HEAD_PARTS; i++) {
        njPushMatrixEx();
        if (i < GOLEM_HEAD_PLATFORMS) {
            njTranslate(NULL, GOLEM_PLATFORM(golem_head_tasks[i])->x, 0.0F,
                        GOLEM_PLATFORM(golem_head_tasks[i])->z);
        }
        range = part_ranges[i];
        if (i < GOLEM_HEAD_PLATFORMS &&
            player_work[0]->mode == PLAYER_MODE_SONIC_HOMING &&
            target->work != NULL && player->by_character.sonic.homing == 0) {
            nearest = target->work;
            nearest_distance = target->distance;
            for (; target->work != NULL; target++) {
                if (target->distance < nearest_distance) {
                    nearest_distance = target->distance;
                    nearest = target->work;
                }
            }
            if (nearest->collision->owner == golem_head_tasks[i]->child) {
                range = OUT_OF_WORLD;
            }
        }
        golem_sensor_place(golem_head_tasks[i], golem_head_tasks[i]->motion.p,
                           &golem_head_matrices[i], range,
                           &golem_head_bases[i]);
        njPopMatrixEx();
    }
}

struct task *golem_flash_drop_create(struct task *parent, int kind)
{
    struct task *task;

    task = task_create_child(TASK_WORK, flash_drop_step, parent);
    task->draw = flash_drop_draw;
    task->destructor = flash_drop_destroy;
    task->extra.p = syMalloc(sizeof(struct flash_drop));
    FLASH_DROP(task)->kind = kind;
    FLASH_DROP(task)->steps = 0;
    FLASH_DROP(task)->size_x = drop_start_x;
    FLASH_DROP(task)->size_y = drop_start_y;
    njPushMatrixEx();
    matrix_set(&golem_sensor_matrices[GOLEM_SENSOR_HEAD]);
    njTranslate(NULL, 0.0F, drop_height, 0.0F);
    njGetTranslation(NULL, &task->work->pos);
    njPopMatrixEx();
    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        collision_init(task, drop_shape, DROP_SHAPES, COLLISION_OBJECT);
    }
    /* @hack spelling: the kind tested twice, where one test would do */
    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        FLASH_DROP(task)->grow_x = drop_grow_x;
        FLASH_DROP(task)->grow_y = drop_grow_y;
    } else {
        FLASH_DROP(task)->grow_x = faint_drop_grow_x;
        FLASH_DROP(task)->grow_y = faint_drop_grow_y;
    }
    FLASH_DROP(task)->scale[0].x =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[0].y =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[0].z =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[1].x =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    FLASH_DROP(task)->scale[1].y =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    FLASH_DROP(task)->scale[1].z =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    sound_play(FLASH_SOUND, task, 0, 0);
    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        sound_set_volume(task, SOUND_ANY_ID, drop_volume);
    } else {
        sound_set_volume(task, SOUND_ANY_ID, faint_drop_volume);
    }
    return task;
}

static void flash_drop_destroy(struct task *task)
{
    sound_stop_key(task);
    if (task->extra.p != NULL) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}

static void flash_drop_step(struct task *task)
{
    struct task_work *work = task->work;
    Float fall;
    Sint32 grow_steps;
    Sint32 life;

    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        fall = drop_fall;
    } else {
        fall = faint_drop_fall;
    }
    /* @hack spelling: the kind tested twice, where one test would do */
    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        grow_steps = drop_grow_steps;
        life = drop_life;
    } else {
        grow_steps = faint_drop_grow_steps;
        life = faint_drop_life;
    }
    work->pos.y -= fall;
    if (FLASH_DROP(task)->steps < grow_steps) {
        FLASH_DROP(task)->size_x += FLASH_DROP(task)->grow_x;
        FLASH_DROP(task)->size_y += FLASH_DROP(task)->grow_y;
        FLASH_DROP(task)->grow_x *= DROP_GROWTH_KEEP;
        FLASH_DROP(task)->grow_y *= DROP_GROWTH_KEEP;
        if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
            work->collision->shapes->radius =
                drop_shape[0].radius * FLASH_DROP(task)->size_x;
            work->collision->shapes->size_y =
                drop_shape[0].size_y * FLASH_DROP(task)->size_y;
            collision_fit_radius(task);
        }
    }
    FLASH_DROP(task)->scale[0].x =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[0].y =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[0].z =
        (rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low;
    FLASH_DROP(task)->scale[1].x =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    FLASH_DROP(task)->scale[1].y =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    FLASH_DROP(task)->scale[1].z =
        ((rand() * drop_scale_spread * RAND_SCALE) + drop_scale_low) *
        drop_inner_scale;
    if (++FLASH_DROP(task)->steps > life) {
        task_kill(task);
    } else if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_HURTS) {
        collision_register(task);
    }
}

static void flash_drop_draw(struct task *task)
{
    Float alpha;
    struct task_work *work = task->work;
    NJS_VECTOR size;

    /* @bug logic: a faint drop ends at about half its alpha. the fade takes
     * drop_life for either kind, at 0x8c504f2a, where the step takes
     * faint_drop_life for a faint drop, at 0x8c504da2 */
    if (FLASH_DROP(task)->steps >= drop_life - drop_fade_steps) {
        alpha = (Float)(drop_life - FLASH_DROP(task)->steps) /
                (Float)drop_fade_steps;
    } else {
        alpha = 1.0F;
    }
    if (FLASH_DROP(task)->kind == GOLEM_FLASH_DROP_FAINT) {
        alpha *= faint_drop_alpha;
    }
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    /* @hack type: game_state's steps read as game_state_steps, where
     * game_state.steps would do */
    njRotateY(NULL, drop_spin * game_state_steps);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(DRAW_CONTROL_CONSTANT | NJD_CONTROL_3D_CNK_BLEND_MODE);
    njSetCnkBlendMode(NJD_FBS_SA | NJD_FBD_ONE);
    constant_material_set(alpha, 1.0F, 1.0F, 1.0F);
    njSetConstantAttr(~NJD_FST_IL, 0);
    size.x = size.z = FLASH_DROP(task)->size_x;
    size.y = FLASH_DROP(task)->size_y;
    size.x *= FLASH_DROP(task)->scale[0].x;
    size.y *= FLASH_DROP(task)->scale[0].y;
    size.z *= FLASH_DROP(task)->scale[0].z;
    njPushMatrixEx();
    njSetTexture(&golem_flash_texlist);
    golem_model_draw_scaled_v(golem_flash_object->model, &size);
    njPopMatrixEx();
    size.x = size.z = FLASH_DROP(task)->size_x;
    size.y = FLASH_DROP(task)->size_y;
    size.x *= FLASH_DROP(task)->scale[1].x;
    size.y *= FLASH_DROP(task)->scale[1].y;
    size.z *= FLASH_DROP(task)->scale[1].z;
    njRotateY(NULL, QUARTER_TURN);
    golem_model_draw_scaled_v(golem_flash_object->model, &size);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrixEx();
}

void golem_rubble_throw(struct task *golem_task, int sensor)
{
    struct task *task;
    struct rubble *cursor;
    struct rubble *piece;
    Sint32 count;
    Sint32 i;
    struct angles ang;
    Float dist;
    Float scale;
    Float sine;
    NJS_VECTOR offset;
    NJS_VECTOR spd;
    NJS_VECTOR move;
    NJS_POINT3 base;
    NJS_POINT3 last;
    NJS_POINT3 place;
    NJS_POINT3 pos;
    NJS_VECTOR out;

    count = (Sint32)((rand() * (Float)(throw_pieces - 1) * RAND_SCALE) + 1.0F);
    njPushMatrixEx();
    matrix_set(&golem_sensor_bases[sensor]);
    njGetTranslation(NULL, &last);
    matrix_set(&golem_sensor_matrices[sensor]);
    njGetTranslation(NULL, &place);
    njPopMatrixEx();
    move.x = place.x - last.x;
    move.y = place.y - last.y;
    move.z = place.z - last.z;
    /* @bug uninitialized: reads the golem's rubble_count as the heap left it,
     * since golem_create never sets it. golem_e_create sets its twin's to 0, at
     * 0x8c500924 of BOSS_GOLEM_E. this read is at 0x8c5051c0 */
    /* @hack spelling: the second test as the first's else, where a test of its
     * own would do */
    if (golem->rubble_count > rubble_limit) {
        return;
    } else if (njScalor2(&move) <= throw_move * throw_move) {
        return;
    }
    if (rand() * RAND_SCALE >= throw_chance) {
        return;
    }
    /* @hack spelling: the assignments inside the tests, where statements of
     * their own would do */
    if (!(task = task_create_child(TASK_WORK, rubble_step, golem_task))) {
        return;
    }
    if (!(task->extra.p = syCalloc(RUBBLE_PIECES, sizeof(struct rubble)))) {
        task_kill(task);
        return;
    }
    task->draw = rubble_draw;
    task->destructor = rubble_destroy;
    njPushMatrixEx();
    matrix_set(&golem_sensor_matrices[sensor]);
    njGetTranslation(NULL, &base);
    /* @hack spelling: the count stepped as i = i + 1, where i++ would do */
    /* @hack spelling: piece, a copy of cursor, where cursor would do */
    for (i = 0, cursor = task->extra.p; i < count; i = i + 1, cursor++) {
        piece = cursor;
        /* @hack spelling: the sine set ahead of the angle, where its value in
         * place would do */
        /* @hack redundant: rand() times 1, where (Float)rand() would do */
        sine = njSin((Angle)((rand() * 1.0F) - QUARTER_TURN));
        ang.y = (Angle)((sine * QUARTER_TURN) +
                        camera_views[camera_current_index]->ang.y) ^
                HALF_TURN;
        /* @bug arithmetic: every piece leaves at the same tilt. the tilt's
         * sine takes rand() * RAND_SCALE as an Angle, which truncates to 0 at
         * 0x8c505362. the sine of 0 is 0 */
        ang.x = (Angle)((njSin((Angle)(rand() * RAND_SCALE)) * TILT_SWING) -
                        QUARTER_TURN);
        njPushMatrixEx();
        njRotateY(NULL, ang.y);
        njRotateX(NULL, ang.x);
        njPushMatrixEx();
        out.x = out.y = 0.0F;
        out.z = -(rubble_dist * RANDOM_FROM(SPREAD_LOW, SPREAD));
        njCalcVector(NULL, &out, &offset);
        njPopMatrixEx();
        piece->pos.x = base.x + offset.x;
        piece->pos.y = base.y + offset.y;
        piece->pos.z = base.z + offset.z;
        njPushMatrixEx();
        out.x = out.y = 0.0F;
        out.z = -(rubble_speed * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD));
        njCalcVector(NULL, &out, &piece->spd);
        piece->spd.x += move.x * throw_share;
        piece->spd.y += move.y * throw_share;
        piece->spd.z += move.z * throw_share;
        njPopMatrixEx();
        piece->spin.x = (Angle)(rubble_spin * RANDOM_FROM(SPREAD_LOW, SPREAD));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.x = -piece->spin.x;
        }
        piece->spin.y = (Angle)(rubble_spin * RANDOM_FROM(SPREAD_LOW, SPREAD));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.y = -piece->spin.y;
        }
        piece->spin.z = (Angle)(rubble_spin * RANDOM_FROM(SPREAD_LOW, SPREAD));
        if (rand() * RAND_SCALE < EVEN_ODDS) {
            piece->spin.z = -piece->spin.z;
        }
        piece->scl.x = rubble_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->scl.y = rubble_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->scl.z = rubble_scale * RANDOM_FROM(SIZE_LOW, SIZE_SPREAD);
        piece->steps = 0;
        piece->life = rubble_life;
        piece->live = TRUE;
        piece->smokes = !(rand() * RAND_SCALE > smoke_chance);
        /* @bug arithmetic: each piece fades over its whole life, where
         * rubble_fade would keep it opaque for its first third. fade_share is
         * an integer, so the store at 0x8c505582 truncates rubble_fade to 0 */
        piece->fade_share = (Sint32)rubble_fade;
        piece->model = &golem_rubble_model;
        piece->texlist = &golem_rubble_texlist;
        njPopMatrixEx();
        pos.x = base.x +
                (offset.x * puff_dist_share * RANDOM_FROM(SPREAD_LOW, SPREAD));
        pos.y = base.y +
                (offset.y * puff_dist_share * RANDOM_FROM(SPREAD_LOW, SPREAD));
        pos.z = base.z +
                (offset.z * puff_dist_share * RANDOM_FROM(SPREAD_LOW, SPREAD));
        spd.x =
            piece->spd.x * puff_speed_share * RANDOM_FROM(SPREAD_LOW, SPREAD);
        spd.y =
            piece->spd.y * puff_speed_share * RANDOM_FROM(SPREAD_LOW, SPREAD);
        spd.z =
            piece->spd.z * puff_speed_share * RANDOM_FROM(SPREAD_LOW, SPREAD);
        dist = puff_dist * RANDOM_FROM(SPREAD_LOW, SPREAD);
        scale = puff_scale * RANDOM_FROM(WIDE_LOW, WIDE_SPREAD);
        golem_dust_smoke_behind(&pos, &spd, dist, scale);
    }
    /* @hack redundant: a block no path reaches, which reads task and stores
     * 0.75 */
    if (0) {
        if (task) {
        }
        puff_dist_share = 0.75F;
    }
    /* @hack redundant: a block no path reaches, which pushes and pops the
     * matrix */
    if (0) {
        njPushMatrixEx();
        njPushMatrixEx();
        njPopMatrixEx();
    }
    LIVE_PIECES(task) = count;
    /* @hack spelling: the count read back from the task, where count would
     * do */
    golem->rubble_count += LIVE_PIECES(task);
    njPopMatrixEx();
}

static void rubble_destroy(struct task *task)
{
    syFree(task->extra.p);
    task->extra.p = NULL;
}

static void rubble_step(struct task *task)
{
    struct rubble *piece;
    Sint32 i;

    for (i = 0; i < RUBBLE_PIECES; i++) {
        piece = &RUBBLE(task)[i];
        if (piece->live) {
            piece->pos.x += piece->spd.x;
            piece->pos.y += piece->spd.y;
            piece->pos.z += piece->spd.z;
            piece->spd.y -= rubble_gravity;
            piece->spd.x *= rubble_drag;
            piece->spd.y *= rubble_drag;
            piece->spd.z *= rubble_drag;
            piece->ang.x += piece->spin.x;
            piece->ang.y += piece->spin.y;
            piece->ang.z += piece->spin.z;
            piece->spin.x = (Angle)(piece->spin.x * rubble_spin_keep);
            piece->spin.y = (Angle)(piece->spin.y * rubble_spin_keep);
            piece->spin.z = (Angle)(piece->spin.z * rubble_spin_keep);
            if (piece->smokes && rand() * RAND_SCALE < puff_chance) {
                /* @bug logic: a flying piece's puff takes a scale from 9 to
                 * 9.2, where flight_puff_scale, 10, times the random factor
                 * would give 9 to 11. the random share of SPREAD is added
                 * after the product at 0x8c5058de, not within it */
                golem_dust_smoke_behind(&piece->pos, &piece->spd, 0.0F,
                                        (flight_puff_scale * SPREAD_LOW) +
                                            (rand() * (SPREAD * RAND_SCALE)));
            }
            if (++piece->steps >= piece->life || piece->pos.y < 0.0F) {
                piece->live = FALSE;
                LIVE_PIECES(task)--;
                golem->rubble_count--;
            }
        }
    }
    if (LIVE_PIECES(task) <= 0) {
        golem->rubble_count -= LIVE_PIECES(task);
        task_kill(task);
    }
}

static void rubble_draw(struct task *task)
{
    struct rubble *piece;
    Sint32 i;
    Float age;
    Float size;
    Float light;
    Float alpha;
    NJS_VECTOR scl;

    njSetTexture(RUBBLE(task)->texlist);
    for (i = 0; i < RUBBLE_PIECES; i++) {
        piece = &RUBBLE(task)[i];
        if (piece->live) {
            age = (Float)piece->steps / (Float)piece->life;
            size = 1.0F - (age * LIFE_SHRINK);
            light = 1.0F - age;
            scl.x = piece->scl.x * size;
            scl.y = piece->scl.y * size;
            scl.z = piece->scl.z * size;
            if (age < (Float)piece->fade_share) {
                alpha = 1.0F;
            } else {
                alpha = light / (1.0F - (Float)piece->fade_share);
            }
            njPushMatrixEx();
            njTranslateEx(&piece->pos);
            njRotateXYZ(NULL, piece->ang.x, piece->ang.y, piece->ang.z);
            draw_control_save();
            draw_attr_save();
            draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
            draw_control_set(DRAW_CONTROL_CONSTANT);
            njSetConstantAttr(0xff, NJD_FST_UA | NJD_FST_IS | NJD_FST_IL);
            constant_material_set(alpha, light, light, light);
            golem_model_draw_scaled_v(piece->model, &scl);
            constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
            draw_attr_restore();
            draw_control_restore();
            njPopMatrixEx();
        }
    }
}
