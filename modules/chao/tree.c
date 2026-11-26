/* a garden's trees from seed to dead wood, their fruit, and their save
 * records */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "chao/egg.h"
#include "chao/fruit.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/model.h"
#include "chao/movement.h"
#include "chao/orchard_art.h"
#include "chao/planting.h"
#include "chao/toy_art.h"
#include "chao/tree.h"
#include "chao/tuning.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "draw/lines.h"
#include "object/fall.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define TREE_WORK(tree) ((struct garden_tree *)(tree)->work)

enum tree_mode {
    TREE_EMPTY,
    TREE_SEED,
    TREE_WAIT_FOR_SPROUT,
    TREE_SPROUT,
    TREE_SAPLING,
    TREE_WAIT_FOR_GROWTH,
    TREE_RAPID_GROWTH,
    TREE_BEARING,
    TREE_WITHER,
    TREE_DEAD,
    TREE_SHRINK /* scale counts down to 0, and no draw reads it in this mode */
};

/* tree_shapes, by index. a grown tree has BULK_SHAPE, TRUNK_SHAPE,
 * SHADE_SHAPE and TOP_SHAPE, and a sapling has STEM_SHAPE. pickups and
 * animals near a tree test for SHADE_SHAPE's id, GARDEN_SHADE_HIT */
enum tree_shape {
    BULK_SHAPE, /* resists pushes, and pushes nothing */
    TRUNK_SHAPE,
    SHADE_SHAPE,
    PLOT_SHAPE, /* an empty plot's shape, of id GARDEN_EMPTY_PLOT_HIT */
    BASE_SHAPE, /* where a GARDEN_WATER_HIT field waters the tree */
    /* on while the tree is thirsty, of id GARDEN_THIRST_HIT. a Chao it
     * touches may go to water the tree */
    THIRST_SHAPE,
    STEM_SHAPE,
    TOP_SHAPE,
    STEM_TOP_SHAPE,
    TREE_SHAPES
};

/* garden_tree_models' three models of a planted tree */
enum { TREE_MODEL_SAPLING, TREE_MODEL_GROWN, TREE_MODEL_DEAD };

#define FULL_WATER 10000
#define THIRST_LEVEL 6000 /* a tree with less water is thirsty */
/* above WET a sapling grows faster, and a grown tree's life runs down
 * slowest. at DRY or below it runs down fastest */
#define WET 5000
#define DRY 2000
#define SEED_WATER 100 /* a seed's gain each tree_seed_interval */
#define TREE_LIFE 6000
#define WAIT_STEPS 180
#define SPRING_STEPS 200
#define DEAD_STEPS 18000

/* a fruit's growth runs up to FRUIT_GROWN. above FRUIT_RIPE, each shake
 * counts toward the fruit's drop. above FRUIT_FALLING, the fruit can also
 * drop by itself while the garden holds fewer than FRUIT_OBJECT_LIMIT fruit
 * objects */
#define FRUIT_RIPE 0.9F
#define FRUIT_FALLING 1.9F
#define FRUIT_GROWN 2.0F
#define FRUIT_OBJECT_LIMIT 4

/* a push past SHAKE_PUSH arms a shake, and one below SHAKE_RELEASE ends it */
#define SHAKE_PUSH 0.95F
#define SHAKE_RELEASE 0.1F
#define TILT_PER_PUSH (FULL_TURN / 60.0F)
/* a sapling's growth a step while its water is above WET, and while not */
#define WET_GROWTH 0.00005F
#define DRY_GROWTH 0.00002F
/* the fruit's and the branches' sway. each step, a push adds GAIN of itself
 * to the amplitude, up to MAX degrees. the amplitude then keeps KEEP of
 * itself, and the phase turns by STEP */
#define FRUIT_SWAY_GAIN 0.9F
#define FRUIT_SWAY_MAX 14.0F
#define FRUIT_SWAY_KEEP 0.98F
#define FRUIT_SWAY_STEP 0x720
#define BRANCH_SWAY_GAIN 0.3F
#define BRANCH_SWAY_MAX 6.0F
#define BRANCH_SWAY_KEEP 0.986F
#define BRANCH_SWAY_STEP 0x600
/* a grown fruit's scale */
#define FRUIT_SCALE 1.5F

#define GROW_SOUND 0x1014
#define SHAKE_SOUND 0x1008
#define DROP_SOUND 0x1009

/* the shakes a ripe fruit holds on through: 11 to 24 where RAND_MAX is
 * 32767 */
/* @hack type: the count cast to Uint16, where its int would do */
#define SHAKES_TO_DROP() ((Uint16)((int)(rand() * 4.2724606e-4F) + 11))

static const struct collision_shape tree_shapes[TREE_SHAPES] = {
#include "chao/tree/tree_shapes.inc"
};
static const NJS_POINT3 tree_initial_drop_position = {
#include "chao/tree/tree_initial_drop_position.inc"
};
/* the index of tree_fruit_counts that holds the last pass's count */
static Sint32 last_slot;

#define SHAPES_OFF(tree, i)                                                    \
    for ((i) = TREE_SHAPES; (i) != 0;) {                                       \
        (i)--;                                                                 \
        collision_shape_off((tree), (i));                                      \
    }
#define TREE_MODE(work, mode_value)                                            \
    {                                                                          \
        (work)->base.mode = (mode_value);                                      \
        (work)->base.submode = 0;                                              \
        (work)->base.timer = 0;                                                \
        (work)->base.tick = 0;                                                 \
    }
#define BLEND_MODELS(work, first, second)                                      \
    garden_blend_models(garden_tree_models[(work)->kind - 1][(first)],         \
                        garden_tree_models[(work)->kind - 1][(second)],        \
                        (work)->copy, (work)->growth)
/* ease value to 1 as a damped spring, whose speed is speed */
#define SPRING_TO_ONE(value, speed)                                            \
    {                                                                          \
        (speed) += (1.0F - (value)) * 0.15F;                                   \
        (value) += (speed);                                                    \
        (speed) *= 0.85F;                                                      \
    }
/* a new fruit's growth, from -0.3 to 0 */
#define FRUIT_START() (-(rand() * (0.3F * RAND_SCALE)))
#define FRUIT_HEADING() ((int)(rand() * 6.0349703e-5F))

#ifdef NONMATCHING
/* this side's fruit drop, which returns the fruit or null. so garden_tree_step
 * keeps a fruit that couldn't drop */
static struct task *drop_tree_fruit(struct garden_tree *work,
                                    NJS_POINT3 *position, NJS_VECTOR *velocity)
{
    int kind;
    if (work->stage != TREE_STAGE_PERMANENT) {
        kind = work->kind - 1;
    } else {
        switch (work->garden) {
        case CHAO_AREA_NEUTRAL:
            kind = GARDEN_FRUIT_NEUTRAL;
            break;
        case CHAO_AREA_HERO:
            kind = GARDEN_FRUIT_HERO_GARDEN;
            break;
        case CHAO_AREA_DARK:
            kind = GARDEN_FRUIT_DARK_GARDEN;
            break;
        default:
            return NULL;
        }
    }
    return garden_fruit_create(kind, position, FRUIT_HEADING(), velocity);
}
#endif

static void tree_shapes_off(struct task *tree)
{
    int i;
    SHAPES_OFF(tree, i);
}

void garden_advance_fruit_counts(void)
{
    if (last_slot) {
        last_slot = 0;
        tree_fruit_counts[1] = 0;
    } else {
        last_slot = 1;
        tree_fruit_counts[0] = 0;
    }
}
static void count_fruit(void)
{
    if (last_slot) {
        tree_fruit_counts[0]++;
    } else {
        tree_fruit_counts[1]++;
    }
}
int garden_active_fruit_count(void)
{
    int total =
        tree_fruit_counts[last_slot] + garden_object_count(GARDEN_OBJECT_FRUIT);
    return total;
}
static void garden_tree_clone_model(struct garden_tree *work)
{
    work->copy = garden_clone_object(
        garden_tree_models[work->kind - 1][TREE_MODEL_SAPLING]);
}
void fun_0c5251ae(void) /* NOLINT(misc-use-internal-linkage) */
{
}
static void garden_tree_free_model(struct garden_tree *work)
{
    if (work->copy) {
        garden_free_object(work->copy);
        work->copy = NULL;
    }
}
static void start_bearing(struct task *tree)
{
    struct garden_tree *work = TREE_WORK(tree);
    work->growth = 1.0F;
    work->stage = TREE_STAGE_MATURE;
    work->fruit[0].growth = FRUIT_START();
    work->fruit[1].growth = FRUIT_START();
    work->fruit[2].growth = FRUIT_START();
    work->fruit[0].shakes_left = SHAKES_TO_DROP();
    work->fruit[1].shakes_left = SHAKES_TO_DROP();
    work->fruit[2].shakes_left = SHAKES_TO_DROP();
    tree_shapes_off(tree);
    collision_shape_on(tree, BULK_SHAPE);
    collision_shape_on(tree, TRUNK_SHAPE);
    collision_shape_on(tree, SHADE_SHAPE);
    collision_shape_on(tree, TOP_SHAPE);
    fall_set_kind(tree->work, tree->motion.p, FALL_KIND_SHAKE);
    TREE_MODE(work, TREE_BEARING);
    work->thirsty = FALSE;
    work->life = TREE_LIFE;
}

static void find_fruit_node(NJS_POINT3 *position, struct garden_tree *work,
                            NJS_CNK_OBJECT *node, int index)
{
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)node->pos);
    njRotateXYZ(NULL, node->ang[0], node->ang[1], node->ang[2]);
    if (!node->model) {
        njRotateX(NULL,
                  (Angle)((work->fruit_amplitude *
                           njSin(tree_fruit_phases[tree_fruit_node_index] +
                                 work->fruit_phase)) *
                          ANGLE_PER_DEGREE));
        if (tree_fruit_node_index == index) {
            njGetTranslation(NULL, position);
            position->y -= 1.5F;
        }
        tree_fruit_node_index++;
    }
    if (node->child) {
        /* @bug memory: a node with a child searches its sibling instead, at
         * 0x0c52536a. the search misses the child's nodes, and reads through
         * null where the node has no sibling */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: a node with a child searches
         * the child */
        find_fruit_node(position, work, node->child, index);
#else
        find_fruit_node(position, work, node->sibling, index);
#endif
    }
    njPopMatrixEx();
    if (node->sibling) {
        find_fruit_node(position, work, node->sibling, index);
    }
}

static void fruit_drop_position(NJS_POINT3 *position, struct garden_tree *work,
                                int index)
{
    NJS_CNK_OBJECT *node;
    tree_fruit_node_index = 0;
#ifdef NONMATCHING
    /* this side repairs the uninitialized bug tagged below: *position starts at
     * the tree's place */
    *position = work->base.pos;
#endif
    if (work->garden != CHAO_AREA_HERO) {
        njPushMatrix(&_nj_unit_matrix_);
        njTranslateEx(&work->base.pos);
        njRotateY(NULL, work->tilt_direction);
        njRotateX(NULL, work->tilt);
        njRotateY(NULL, work->base.ang.y - work->tilt_direction);
        if (work->copy) {
            node = work->copy;
        } else if (work->model) {
            node = work->model;
        } else {
            /* @bug logic: a tree with no model returns at 0x0c52544c with its
             * matrix still pushed */
            /* @bug uninitialized: a tree with no model returns at 0x0c52544c
             * with *position unwritten. garden_tree_step, which calls this at
             * 0x0c525d0c, then makes the fruit there, by garden_fruit_create
             * at 0x0c525d7e or 0x0c525e00 */
#ifdef NONMATCHING
            /* this side repairs the logic bug tagged above: it pops the matrix
             * before the return */
            njPopMatrixEx();
#endif
            return;
        }
        njPushMatrixEx();
        njTranslateEx((NJS_VECTOR *)node->pos);
        njRotateXYZ(NULL, node->ang[0], node->ang[1], node->ang[2]);
        if (node->child) {
            find_fruit_node(position, work, node->child, index);
        }
        njPopMatrixEx();
        if (node->sibling) {
            find_fruit_node(position, work, node->sibling, index);
        }
        njPopMatrixEx();
    } else {
        position->x = work->base.pos.x;
        position->y = work->base.pos.y + 29.0F;
        position->z = work->base.pos.z;
    }
}

static void garden_tree_step(struct task *tree)
{
    struct garden_tree *work = TREE_WORK(tree);
    struct tree_fruit *fruit;
    struct task_motion *motion;
    NJS_VECTOR initial;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    Float push;
    Float length;
    int i;
    Bool drop;
    switch (work->base.mode) {
    case TREE_EMPTY:
        if (garden_take_command(tree) == PLANT_SEED) {
            work->kind = garden_take_state(tree) + 1;
            collision_shape_off(tree, PLOT_SHAPE);
            collision_shape_on(tree, BASE_SHAPE);
            collision_shape_on(tree, THIRST_SHAPE);
            work->water = work->thirsty = 0;
            TREE_MODE(work, TREE_SEED);
            work->stage = TREE_STAGE_SEED;
        }
        break;
    case TREE_SEED:
        if (garden_hit_kind(tree, BASE_SHAPE, GARDEN_WATER_HIT)) {
            work->water = FULL_WATER;
        }
        if (work->base.timer++ >= chao_tuning.tree_seed_interval) {
            work->base.timer = 0;
            work->water += SEED_WATER;
        }
        if (work->water >= FULL_WATER) {
            work->water = THIRST_LEVEL;
            collision_shape_off(tree, BASE_SHAPE);
            work->base.mode = TREE_WAIT_FOR_SPROUT;
            work->base.submode = 0;
            work->stage = TREE_STAGE_SAPLING;
        }
        break;
    case TREE_WAIT_FOR_SPROUT:
        if (work->base.timer++ > WAIT_STEPS) {
            garden_tree_clone_model(work);
            collision_shape_on(tree, STEM_SHAPE);
            work->scale = 0.01F;
            work->spring_speed = 0;
            TREE_MODE(work, TREE_SPROUT);
        }
        break;
    case TREE_SPROUT:
        SPRING_TO_ONE(work->scale, work->spring_speed);
        if (work->base.timer++ > SPRING_STEPS) {
            work->scale = 1.0F;
            work->water = FULL_WATER;
            work->thirsty = FALSE;
            TREE_MODE(work, TREE_SAPLING);
        }
        break;
    case TREE_SAPLING:
        if (garden_hit_kind(tree, BASE_SHAPE, GARDEN_WATER_HIT)) {
            work->water = FULL_WATER;
            collision_shape_off(tree, BASE_SHAPE);
            collision_shape_off(tree, THIRST_SHAPE);
            TREE_MODE(work, TREE_WAIT_FOR_GROWTH);
        }
        if (work->base.timer++ > chao_tuning.tree_water_interval) {
            work->water -= chao_tuning.tree_water_use;
            work->base.timer = 0;
        }
        if (work->water > WET) {
            work->growth += WET_GROWTH;
        } else {
            work->growth += DRY_GROWTH;
            if (work->water < 0) {
                work->water = 0;
            }
        }
        if (work->water < THIRST_LEVEL && work->thirsty == 0) {
            work->thirsty = TRUE;
            collision_shape_on(tree, BASE_SHAPE);
            collision_shape_on(tree, THIRST_SHAPE);
        }
        BLEND_MODELS(work, TREE_MODEL_SAPLING, TREE_MODEL_GROWN);
        if (work->growth >= 1.0F) {
            start_bearing(tree);
        }
        break;
    case TREE_WAIT_FOR_GROWTH:
        if (work->base.timer++ > WAIT_STEPS) {
            work->spring_speed = 0;
            TREE_MODE(work, TREE_RAPID_GROWTH);
            sound_play_at(GROW_SOUND, NULL, 0, 0, &tree->work->pos);
        }
        break;
    case TREE_RAPID_GROWTH:
        SPRING_TO_ONE(work->growth, work->spring_speed);
        BLEND_MODELS(work, TREE_MODEL_SAPLING, TREE_MODEL_GROWN);
        if (work->base.timer++ > SPRING_STEPS) {
            start_bearing(tree);
        }
        break;
    case TREE_BEARING:
        work->watered = FALSE;
        if (garden_hit_kind(tree, BASE_SHAPE, GARDEN_WATER_HIT)) {
            work->water = FULL_WATER;
            collision_shape_off(tree, BASE_SHAPE);
            collision_shape_off(tree, THIRST_SHAPE);
            work->thirsty = FALSE;
            work->watered = TRUE;
        }
        if (work->base.timer++ > chao_tuning.tree_life_interval) {
            if (work->water > WET) {
                work->life--;
            } else if (work->water > DRY) {
                work->life -= 2;
            } else {
                work->life -= 3;
            }
            work->water -= chao_tuning.tree_water_use;
            if (work->water < 0) {
                work->water = 0;
            }
            if (work->stage == TREE_STAGE_MATURE && work->life <= 0) {
                work->stage = TREE_STAGE_EMPTY;
                TREE_MODE(work, TREE_WITHER);
                work->life = 0;
            }
            if (work->water < THIRST_LEVEL && work->thirsty == 0) {
                work->thirsty = TRUE;
                collision_shape_on(tree, BASE_SHAPE);
                collision_shape_on(tree, THIRST_SHAPE);
            }
            work->base.timer = 0;
        }
        motion = tree->motion.p;
        length = njScalor(&motion->acc);
        work->push = length;
        if (length < 0.01F) {
            work->unk_dc = angle_approach(work->unk_dc, 0, 0x400);
        }
        /* @hack redundant: a second load of tree->motion, which motion
         * already holds */
        motion = tree->motion.p;
        push = work->push;
        work->tilt = (Angle)(push * TILT_PER_PUSH);
        work->tilt_direction =
            QUARTER_TURN -
            (Angle)(atan2f(motion->acc.z, motion->acc.x) * ANGLE_PER_RADIAN);
        motion->acc.x = 0;
        motion->acc.y = 0;
        motion->acc.z = 0;
        work->shaken = FALSE;
        if (push < SHAKE_RELEASE) {
            if (work->shake_armed) {
                sound_play_at_for(SHAKE_SOUND, tree, 1, -15, 60,
                                  &work->base.pos);
                work->shake_armed = FALSE;
                work->shaken = TRUE;
            }
        }
        if (push > SHAKE_PUSH && work->shake_armed == 0) {
            work->shake_armed = TRUE;
            work->shaken = TRUE;
        }
        for (i = 0; i < TREE_FRUITS; i++) {
            fruit = &work->fruit[i];
            if (fruit->growth >= 0) {
                fruit->growth += chao_tuning.tree_fruit_growth;
                if (fruit->growth > FRUIT_GROWN) {
                    fruit->growth = FRUIT_GROWN;
                }
                if (fruit->target_growth < fruit->growth) {
                    fruit->target_growth = fruit->growth;
                }
                /* @hack spelling: the watered and target_growth tests nested,
                 * where one test joined by && would do */
                if (work->watered) {
                    if (fruit->target_growth < 1.0F) {
                        fruit->target_growth += 0.6F;
                        if (fruit->target_growth > 1.0F) {
                            fruit->target_growth = 1.0F;
                        }
                    }
                }
                if (fruit->growth < fruit->target_growth) {
                    fruit->growth += 0.08F;
                    if (fruit->growth > FRUIT_GROWN) {
                        fruit->growth = FRUIT_GROWN;
                    }
                }
                count_fruit();
            } else if (garden_active_fruit_count() <
                       chao_tuning.tree_fruit_limit) {
                /* @hack spelling: work->fruit[i], where fruit would do */
                work->fruit[i].growth += chao_tuning.tree_fruit_growth;
            }
            if (fruit->growth > FRUIT_RIPE) {
                drop = FALSE;
                if (work->shaken) {
                    fruit->shakes_left--;
                    if (fruit->shakes_left <= 0) {
                        drop = TRUE;
                    }
                }
                if (fruit->growth > FRUIT_FALLING &&
                    garden_object_count(GARDEN_OBJECT_FRUIT) <
                        FRUIT_OBJECT_LIMIT) {
                    if (work->base.timer % 10 == 1 && RANDOM_UNIT() < 0.002F) {
                        drop = TRUE;
                    }
                }
                if (drop) {
#ifdef NONMATCHING
                    /* the fruit as it was, which this side puts back where the
                     * drop fails */
                    struct tree_fruit ripe = *fruit;
#endif
                    /* @hack redundant: a copy of tree_initial_drop_position
                     * that nothing reads */
                    initial = tree_initial_drop_position;
                    (void)initial;
                    fruit->shakes_left = SHAKES_TO_DROP();
                    fruit->target_growth = fruit->growth = FRUIT_START();
                    velocity.x = (njSin(work->tilt_direction) * 0.03F) +
                                 ((0.5F - RANDOM_UNIT()) * 0.01F);
                    velocity.y = 0;
                    velocity.z = (njCos(work->tilt_direction) * 0.03F) +
                                 ((0.5F - RANDOM_UNIT()) * 0.01F);
                    if (RANDOM_UNIT() < 0.5F) {
                        velocity.x *= -1.0F;
                        velocity.z *= -1.0F;
                    }
                    fruit_drop_position(&position, work, i);
#ifdef NONMATCHING
                    /* this side keeps the fruit on the tree where
                     * drop_tree_fruit can't make it. the matching side drops it
                     * whether or not the fruit is made */
                    if (!drop_tree_fruit(work, &position, &velocity)) {
                        *fruit = ripe;
                        continue;
                    }
#else
                    if (work->stage == TREE_STAGE_PERMANENT) {
                        switch (work->garden) {
                        case CHAO_AREA_NEUTRAL:
                            garden_fruit_create(GARDEN_FRUIT_NEUTRAL, &position,
                                                FRUIT_HEADING(), &velocity);
                            break;
                        case CHAO_AREA_HERO:
                            garden_fruit_create(GARDEN_FRUIT_HERO_GARDEN,
                                                &position, FRUIT_HEADING(),
                                                &velocity);
                            break;
                        case CHAO_AREA_DARK:
                            garden_fruit_create(GARDEN_FRUIT_DARK_GARDEN,
                                                &position, FRUIT_HEADING(),
                                                &velocity);
                            break;
                        }
                    } else {
                        garden_fruit_create(work->kind - 1, &position,
                                            FRUIT_HEADING(), &velocity);
                    }
#endif
                    sound_play_at(DROP_SOUND, NULL, 0, 0, &tree->work->pos);
                }
            }
        }
        work->fruit_amplitude += push * FRUIT_SWAY_GAIN;
        if (work->fruit_amplitude > FRUIT_SWAY_MAX) {
            work->fruit_amplitude = FRUIT_SWAY_MAX;
        }
        work->fruit_phase += FRUIT_SWAY_STEP;
        work->fruit_amplitude *= FRUIT_SWAY_KEEP;
        work->branch_amplitude += push * BRANCH_SWAY_GAIN;
        if (work->branch_amplitude > BRANCH_SWAY_MAX) {
            work->branch_amplitude = BRANCH_SWAY_MAX;
        }
        work->branch_phase += BRANCH_SWAY_STEP;
        work->branch_amplitude *= BRANCH_SWAY_KEEP;
        break;
    case TREE_WITHER:
        /* @bug logic: a tree withers in one step. a grown tree's growth is
         * already 1, as start_bearing stores it at 0x0c525216 and
         * garden_plant_tree at 0x0c526b3c. the add at 0x0c525eb6 takes it
         * past 1 at the wither's first step, and the test at 0x0c525eba ends
         * the blend to the dead model there */
        work->growth += 0.01F;
        if (work->growth > 1.0F) {
            work->growth = 1.0F;
            TREE_MODE(work, TREE_DEAD);
        }
        BLEND_MODELS(work, TREE_MODEL_GROWN, TREE_MODEL_DEAD);
        break;
    case TREE_DEAD:
        if (work->base.timer++ > DEAD_STEPS) {
            TREE_MODE(work, TREE_SHRINK);
            work->scale = 1.0F;
        }
        break;
    case TREE_SHRINK:
        work->scale -= 0.01F;
        if (work->scale < 0) {
            work->scale = 0;
            SHAPES_OFF(tree, i);
            collision_shape_on(tree, PLOT_SHAPE);
            work->base.submode = work->base.mode = TREE_EMPTY;
            work->base.timer = 0;
            work->base.tick = 0;
            garden_tree_free_model(work);
        }
        break;
    }
    collision_register(tree);
}

static void draw_tree_node(struct garden_tree *work, NJS_CNK_OBJECT *node)
{
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)node->pos);
    njRotateXYZ(NULL, node->ang[0], node->ang[1], node->ang[2]);
    if (node->model) {
        njRotateX(NULL, (Angle)(work->branch_amplitude *
                                njSin(work->branch_phase +
                                      tree_branch_phases[work->branch_index]) *
                                ANGLE_PER_DEGREE));
        njCnkEasyDrawModel(node->model);
        work->branch_index++;
    } else {
        struct tree_fruit *fruit = &work->fruit[work->fruit_index];
        NJS_VECTOR offset = {0, -1.8F, 0};
        Float growth;
        Float scale;
        growth = fruit->growth;
        if (growth > 1.0F) {
            growth = 1.0F;
        }
        scale = growth * FRUIT_SCALE;
        if (scale > 0) {
            if (fruit->growth > FRUIT_RIPE) {
                njPushMatrixEx();
                njGetTranslation(NULL, &fruit->position);
                njPopMatrixEx();
                njRotateX(NULL,
                          (Angle)(work->fruit_amplitude *
                                  njSin(work->fruit_phase +
                                        tree_fruit_phases[work->fruit_index]) *
                                  ANGLE_PER_DEGREE));
            } else {
                njRotateX(NULL,
                          (Angle)(0.2F * work->fruit_amplitude *
                                  njSin(work->fruit_phase +
                                        tree_fruit_phases[work->fruit_index]) *
                                  ANGLE_PER_DEGREE));
            }
            njScale(NULL, scale, scale, scale);
            njTranslateEx(&offset);
            switch (work->garden) {
            case CHAO_AREA_NEUTRAL:
                njCnkEasyDrawModel(
                    garden_fruit_models[GARDEN_FRUIT_NEUTRAL]->model);
                break;
            case CHAO_AREA_HERO:
                njCnkEasyDrawModel(
                    garden_fruit_models[GARDEN_FRUIT_HERO_GARDEN]->model);
                break;
            case CHAO_AREA_DARK:
                njCnkEasyDrawModel(
                    garden_fruit_models[GARDEN_FRUIT_DARK_GARDEN]->model);
                break;
            default:
                if (work->kind == 1) {
                    njRotateX(NULL, HALF_TURN);
                }
                njCnkEasyDrawModel(garden_fruit_models[work->kind - 1]->model);
                break;
            }
        }
        work->fruit_index++;
    }
    if (node->child) {
        draw_tree_node(work, node->child);
    }
    njPopMatrixEx();
    if (node->sibling) {
        draw_tree_node(work, node->sibling);
    }
}

static void draw_bearing_tree(struct garden_tree *work, NJS_CNK_OBJECT *root)
{
    int i;
    Angle *small_phase;
    Angle *ripe_phase;
    Angle yaw = 0;

    /* @hack spelling: yaw, still 0, as the start of branch_index, fruit_index
     * and i, where 0 would do */
    work->branch_index = yaw;
    work->fruit_index = yaw;
    njPushMatrixEx();
    njTranslateV(NULL, (NJS_VECTOR *)root->pos);
    njRotateXYZ(NULL, root->ang[0], root->ang[1], root->ang[2]);
    njCnkEasyDrawModel(root->model);
    if (root->child) {
        draw_tree_node(work, root->child);
    }
    njPopMatrixEx();
    if (root->sibling) {
        draw_tree_node(work, root->sibling);
    }
    if (work->garden == CHAO_AREA_HERO) {
        njPushMatrixEx();
        njTranslate(NULL, 0, 28.0F, 0);
        i = yaw;
        small_phase = tree_fruit_phases;
        ripe_phase = tree_fruit_phases;
        do {
            struct tree_fruit *fruit = &work->fruit[i];
            Float growth = fruit->growth;
            NJS_VECTOR offset = {0, -1.8F, 0};
            Float scale;
            growth = growth > 1.0F ? 1.0F : growth;
            scale = growth * FRUIT_SCALE;
            njPushMatrixEx();
            njRotateY(NULL, yaw);
            njRotateX(NULL, (Angle)(30.0F * ANGLE_PER_DEGREE));
            njTranslate(NULL, 0, 0.5F, 0);
            if (scale > 0) {
                if (fruit->growth > FRUIT_RIPE) {
#ifdef NONMATCHING
#else
                    /* @hack redundant: a test of small_phase that no path
                     * reaches, in the branch that reads ripe_phase. the two
                     * cursors always point at the same entry, where
                     * tree_fruit_phases[i] would do for both */
                    if (0) {
                        if (small_phase) {
                        }
                    }
#endif
                    njRotateX(NULL,
                              (Angle)(work->fruit_amplitude *
                                      njSin(work->fruit_phase + *ripe_phase) *
                                      ANGLE_PER_DEGREE));
                } else {
                    njRotateX(NULL,
                              (Angle)(0.2F * work->fruit_amplitude *
                                      njSin(work->fruit_phase + *small_phase) *
                                      ANGLE_PER_DEGREE));
                }
                njScale(NULL, scale, scale, scale);
                njTranslateEx(&offset);
                njCnkEasyDrawModel(
                    garden_fruit_models[GARDEN_FRUIT_HERO_GARDEN]->model);
            }
            njPopMatrixEx();
            i++;
            small_phase = small_phase + 1;
            ripe_phase = ripe_phase + 1;
            yaw += FULL_TURN / TREE_FRUITS;
        } while (i < TREE_FRUITS);
        njPopMatrixEx();
    }
}

#define TREE_SHADOW_ORIGIN(work, position)                                     \
    {                                                                          \
        njPushMatrix(&_nj_unit_matrix_);                                       \
        njTranslate(NULL, (work)->base.pos.x, (work)->base.pos.y,              \
                    (work)->base.pos.z);                                       \
        njRotateY(NULL, (work)->tilt_direction);                               \
        njRotateX(NULL, (work)->tilt);                                         \
        njRotateY(NULL, -(work)->tilt_direction);                              \
        njRotateY(NULL, (work)->base.ang.y);                                   \
        njTranslate(NULL, 0, 20.0F, 0);                                        \
        njGetTranslation(NULL, &(position));                                   \
        njPopMatrixEx();                                                       \
    }
static void draw_trunk_shadow(struct garden_tree *work)
{
    NJS_VECTOR position;
    TREE_SHADOW_ORIGIN(work, position);
    /* @bug logic: the trunk's shadow draws about the current matrix's
     * origin, not at position. TREE_SHADOW_ORIGIN puts position 20 up the
     * tilted trunk, as for draw_branch_shadows, by njGetTranslation at
     * 0x0c526466. nothing moves there: the push at 0x0c526472 runs on to the
     * scale at 0x0c526484 */
    njPushMatrixEx();
    njScale(NULL, 3.0F, 2.0F, 3.0F);
    njTranslate(NULL, 0, 0, 0.85F);
    njCnkModDrawModel(&garden_shadow_model);
    njPopMatrixEx();
}
/* a branch's shadow, which sway and angle stretch as the branch sways */
#define TREE_BRANCH_SHADOW(width, length, sway, angle, phase)                  \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njScale(NULL, (width), 2.0F,                                           \
                (length) + (0.15F * (sway) * njSin((angle) + (phase))));       \
        njTranslate(NULL, 0, 0, 0.85F);                                        \
        njCnkModDrawModel(&garden_shadow_model);                               \
        njPopMatrixEx();                                                       \
    }
static void draw_branch_shadows(struct garden_tree *work)
{
    NJS_VECTOR position;
    Angle angle;
    Float sway;
    Angle *phase;
    TREE_SHADOW_ORIGIN(work, position);
    njPushMatrixEx();
    njTranslateV(NULL, &position);
    njRotateY(NULL, work->base.ang.y + EIGHTH_TURN);
    phase = tree_branch_phases;
    angle = work->branch_phase;
    sway = work->branch_amplitude;
    TREE_BRANCH_SHADOW(3.0F, 7.7F, sway, angle, *phase++);
    njRotateY(NULL, 0x3800);
    TREE_BRANCH_SHADOW(3.0F, 7.3F, sway, angle, *phase++);
    njRotateY(NULL, 0x5000);
    TREE_BRANCH_SHADOW(4.0F, 7.8F, sway, angle, *phase++);
    njRotateY(NULL, 0x4b00);
    TREE_BRANCH_SHADOW(4.0F, 7.1F, sway, angle, *phase);
    njPopMatrixEx();
}
#define TILT_TREE(work)                                                        \
    {                                                                          \
        njRotateY(NULL, (work)->tilt_direction);                               \
        njRotateX(NULL, (work)->tilt);                                         \
        njRotateY(NULL, (work)->base.ang.y - (work)->tilt_direction);          \
    }
static void garden_tree_draw(struct task *tree)
{
    struct garden_tree *work = TREE_WORK(tree);
    int kind = work->kind - 1;
    njSetTexture(work->texture);
    njPushMatrixEx();
    njTranslateEx(&work->base.pos);
    /* @hack redundant: a test of work->crown with an empty body */
#ifdef NONMATCHING
#else
    if (work->crown) {
    }
#endif
    switch (work->base.mode) {
    case TREE_EMPTY:
        break;
    case TREE_SEED:
    case TREE_WAIT_FOR_SPROUT:
        njRotateY(NULL, work->base.ang.y);
        njScale(NULL, 1.5F, 1.5F, 1.5F);
        njCnkEasyDrawModel(garden_seed_models[kind]->model);
        break;
    case TREE_SPROUT:
        if (work->copy) {
            Float scale = work->scale;
            njRotateY(NULL, work->base.ang.y);
            njScale(NULL, scale, scale, scale);
            njCnkEasyDrawObject(work->copy);
        }
        break;
    case TREE_BEARING:
        if (work->copy) {
            TILT_TREE(work);
            draw_bearing_tree(work, work->copy);
        } else if (work->model) {
            TILT_TREE(work);
            draw_bearing_tree(work, work->model);
        }
        break;
    case TREE_DEAD:
        /* @bug logic: a dead tree's translucency never applies. the
         * constant_material_set at 0x0c5268ba clears the material the one at
         * 0x0c5268ae sets. the restores at 0x0c5268c0 and 0x0c5268c6 undo
         * the draw state. both come before the tree draws */
        draw_control_save();
        draw_attr_save();
        draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
        draw_attr_set(0, NJD_FST_UA | NJD_FST_DB);
        draw_attr_clear(0, NJD_FST_IL | NJD_FST_IS | NJD_FST_IA | NJD_FST_FL |
                               NJD_FST_ENV);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        constant_material_set(work->scale - 1.0F, 0, 0, 0);
        constant_material_set(0, 0, 0, 0);
        draw_attr_restore();
        draw_control_restore();
        /* fall through */
    default:
        if (work->copy) {
            TILT_TREE(work);
            njCnkEasyDrawObject(work->copy);
        }
        break;
    }
    njPopMatrixEx();
    if (work->stage == TREE_STAGE_PERMANENT) {
        if (!work->crown) {
            draw_branch_shadows(work);
        }
    } else if (work->base.mode > TREE_BEARING) {
        draw_trunk_shadow(work);
    }
}
static void garden_tree_destroy(struct task *tree)
{
    garden_tree_free_model(TREE_WORK(tree));
    garden_unregister_object(tree);
}

#ifdef NONMATCHING
/* this side repairs the bug tagged in the matching side. a byte from -128 to
 * -56 unpacks as 128 to 200 percent. a pack past 127 stores that byte */
static Float unpack_fruit_growth(Sint8 saved)
{
    int percent = saved <= -56 ? (Uint8)saved : saved;
    return percent * 0.01F;
}

static Sint8 pack_fruit_growth(Float growth)
{
    int percent = (int)(growth * 100.0F);
    return (Sint8)(percent > 127 ? percent - 256 : percent);
}
#define UNPACK_FRUIT_GROWTH(saved) unpack_fruit_growth(saved)
#define PACK_FRUIT_GROWTH(growth) pack_fruit_growth(growth)
#else
/* @bug arithmetic: a fruit grown past 1.27 loads as one yet to start. its
 * record keeps growth in percent in a Sint8, which a percent past 127
 * overflows. garden_plant_tree reads the byte back negative, by the
 * sign-extending load at 0x0c526b50 */
#define UNPACK_FRUIT_GROWTH(saved) ((saved) * 0.01F)
#define PACK_FRUIT_GROWTH(growth) ((growth) * 100.0F)
#endif

struct task *garden_plant_tree(NJS_POINT3 *position,
                               struct garden_tree_save *save)
{
    struct task *tree;
    struct garden_tree *work;
    struct task_work *base;
    int i;
    tree = task_create(0, 2, garden_tree_step, "ALO_GrowTreeExecutor");
    work = syCalloc(1, sizeof(*work));
    tree->work = &work->base;
    base = tree->work;
    tree->motion.p = syCalloc(1, sizeof(struct task_motion));
    tree->draw = garden_tree_draw;
    tree->destructor = garden_tree_destroy;
    collision_init(tree, tree_shapes, TREE_SHAPES, COLLISION_OBJECT_MUTUAL);
    tree_shapes_off(tree);
    work->kind = save->kind;
    if (!save->kind) {
        save->stage = TREE_STAGE_EMPTY;
    }
    if (save->kind == TREE_KIND_PERMANENT) {
        save->stage = TREE_STAGE_PERMANENT;
    }
    if (save->stage == TREE_STAGE_DEAD) {
        save->stage = TREE_STAGE_EMPTY;
    }
    work->stage = save->stage;
    work->water = save->water * 100;
    work->garden = 0;
    switch (save->stage) {
    case TREE_STAGE_EMPTY:
        collision_shape_on(tree, PLOT_SHAPE);
        base->mode = TREE_EMPTY;
        base->submode = 0;
        break;
    case TREE_STAGE_SEED:
        collision_shape_on(tree, BASE_SHAPE);
        base->mode = TREE_SEED;
        base->submode = 0;
        break;
    case TREE_STAGE_SAPLING:
        collision_shape_on(tree, STEM_SHAPE);
        collision_shape_on(tree, STEM_TOP_SHAPE);
        work->growth = save->growth * 0.01F;
        base->mode = TREE_SAPLING;
        base->submode = 0;
        break;
    case TREE_STAGE_MATURE:
        collision_shape_on(tree, BULK_SHAPE);
        collision_shape_on(tree, TRUNK_SHAPE);
        collision_shape_on(tree, SHADE_SHAPE);
        collision_shape_on(tree, TOP_SHAPE);
        work->growth = 1.0F;
        work->life = save->growth * 100;
        for (i = 0; i < TREE_FRUITS; i++) {
            work->fruit[i].growth = UNPACK_FRUIT_GROWTH(save->fruit[i]);
            work->fruit[i].shakes_left = SHAKES_TO_DROP();
        }
        fall_set_kind(tree->work, tree->motion.p, FALL_KIND_SHAKE);
        base->mode = TREE_BEARING;
        base->submode = 0;
        break;
    case TREE_STAGE_PERMANENT:
        collision_shape_on(tree, BULK_SHAPE);
        collision_shape_on(tree, TRUNK_SHAPE);
        collision_shape_on(tree, SHADE_SHAPE);
        collision_shape_on(tree, TOP_SHAPE);
        for (i = 0; i < TREE_FRUITS; i++) {
            work->fruit[i].growth = UNPACK_FRUIT_GROWTH(save->fruit[i]);
            work->fruit[i].shakes_left = SHAKES_TO_DROP();
        }
        work->garden = chao_current_area();
        work->water = FULL_WATER;
        fall_set_kind(tree->work, tree->motion.p, FALL_KIND_SHAKE);
        base->mode = TREE_BEARING;
        base->submode = 0;
        break;
    }
    work->base.pos = *position;
    work->base.pos.y -= 1.1F;
    work->base.ang.y = save->angle << 8;
    work->texture = &egg_texlist;
    if (save->stage >= TREE_STAGE_SAPLING) {
        if (save->stage == TREE_STAGE_PERMANENT) {
            switch (work->garden) {
            case CHAO_AREA_NEUTRAL:
                work->texture = &neutral_garden_texlist;
                work->model = &garden_tree_neutral;
                work->crown = NULL;
                break;
            case CHAO_AREA_HERO:
                work->texture = &hero_garden_texlist;
                work->model = &garden_tree_hero;
                work->crown = NULL;
                break;
            case CHAO_AREA_DARK:
                work->texture = &dark_garden_texlist;
                work->model = &garden_tree_dark;
                work->crown = &garden_tree_dark_crown;
                break;
            }
        } else {
            work->texture = &egg_texlist;
            garden_tree_clone_model(work);
            BLEND_MODELS(work, TREE_MODEL_SAPLING, TREE_MODEL_GROWN);
        }
    }
    work->save = save;
    garden_register_object(GARDEN_OBJECT_GARDEN_TREE, tree, 0);
    return tree;
}

/* set count to the trees whose work makes predicate hold, through index
 * and tree */
#define COUNT_TREES(count, index, tree, work, predicate)                       \
    (index) = garden_object_count(GARDEN_OBJECT_GARDEN_TREE);                  \
    (count) = 0;                                                               \
    while (index) {                                                            \
        (index)--;                                                             \
        (tree) = garden_active_object(GARDEN_OBJECT_GARDEN_TREE, (index));     \
        (work) = TREE_WORK(tree);                                              \
        if (predicate) {                                                       \
            ++(count);                                                         \
        }                                                                      \
    }
int garden_tree_count(void)
{
    int index;
    int count;
    struct task *tree;
    struct garden_tree *work;
    COUNT_TREES(count, index, tree, work, work->stage);
    return count;
}
int garden_planted_tree_count(void)
{
    int index;
    int count;
    struct task *tree;
    struct garden_tree *work;
    COUNT_TREES(count, index, tree, work,
                work->stage != TREE_STAGE_EMPTY &&
                    work->stage != TREE_STAGE_PERMANENT);
    return count;
}
int garden_free_tree_spots(void)
{
    int index;
    int count;
    struct task *tree;
    struct garden_tree *work;
    COUNT_TREES(count, index, tree, work, work->stage == TREE_STAGE_EMPTY);
    return count;
}

void garden_pack_tree(struct task *tree)
{
    struct garden_tree *work = TREE_WORK(tree);
    struct garden_tree_save *save = work->save;
    save->kind = work->kind;
    switch (work->stage) {
    case TREE_STAGE_EMPTY:
    case TREE_STAGE_SEED:
        break;
    case TREE_STAGE_SAPLING:
        save->growth = work->growth * 100.0F;
        break;
    case TREE_STAGE_MATURE:
        save->growth = work->life / 100;
        break;
    case TREE_STAGE_DEAD:
        save->stage = TREE_STAGE_EMPTY;
        break;
    case TREE_STAGE_PERMANENT:
        break;
    }
    save->stage = work->stage;
    /* @bug logic: the record takes water's low byte at 0x0c526f68, but
     * garden_plant_tree reads the byte as hundreds. full water, 10000, loads
     * back as 1600 */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: the record takes water in
     * hundreds */
    save->water = work->water / 100;
#else
    save->water = work->water;
#endif
    save->angle = (work->base.ang.y & 0xff00) >> 8;
    save->fruit[0] = PACK_FRUIT_GROWTH(work->fruit[0].growth);
    save->fruit[1] = PACK_FRUIT_GROWTH(work->fruit[1].growth);
    save->fruit[2] = PACK_FRUIT_GROWTH(work->fruit[2].growth);
}
