/* a garden's trees: the garden's own, and those a Chao plants from seed */
#ifndef CHAO_TREE_H
#define CHAO_TREE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/task.h"

struct garden_tree_save;

#define TREE_FRUITS 3
/* a tree's kind is its seed's kind plus 1, or 0 for an empty plot. a
 * garden's own tree is TREE_KIND_PERMANENT */
#define TREE_KIND_PERMANENT 10

/* a tree's stage, which its save record keeps. a tree that withers takes
 * TREE_STAGE_EMPTY while it still stands. nothing gives a tree
 * TREE_STAGE_DEAD, and garden_plant_tree loads it as TREE_STAGE_EMPTY */
enum tree_stage {
    TREE_STAGE_EMPTY,
    TREE_STAGE_SEED,
    TREE_STAGE_SAPLING,
    TREE_STAGE_MATURE,
    TREE_STAGE_DEAD,
    TREE_STAGE_PERMANENT
};

/* growth is below 0 while the fruit waits to start, and rises to 2. growth
 * climbs to target_growth, which growth raises and watering lifts toward 1 */
struct tree_fruit {
    Float growth, target_growth;
    Sint32 shakes_left; /* the shakes before a ripe fruit drops */
    NJS_POINT3 position; /* set where a ripe fruit draws, and never read */
    Uint8 unk_18[8];
};

struct garden_tree {
    struct task_work base;
    Uint8 kind, stage;
    Uint8 unk_32[2];
    /* the blend of a planted tree's sapling model into its grown one. as
     * the tree withers, it is the blend of the grown into the dead */
    Float growth;
    Sint32 life, water;
    struct tree_fruit fruit[TREE_FRUITS];
    /* scale is a sprout's. spring_speed eases it, or a watered sapling's
     * growth, to 1 */
    Float scale, spring_speed;
    Bool thirsty;
    Bool watered; /* water reached the tree this step */
    /* the branches' sway, in degrees. each branch adds its own entry of
     * tree_branch_phases to branch_phase */
    Angle branch_phase;
    Float branch_amplitude;
    Sint32 branch_index; /* the next branch to draw */
    /* the fruit's sway, in degrees. each fruit adds its own entry of
     * tree_fruit_phases to fruit_phase */
    Angle fruit_phase;
    Float fruit_amplitude;
    Sint32 fruit_index; /* the next fruit to draw */
    /* a hard push arms a shake, and its release ends the shake. shaken marks
     * a step with either, which each ripe fruit counts as a shake */
    Bool shake_armed, shaken;
    Float push; /* how hard the tree is pushed this step */
    Angle tilt, tilt_direction;
    Angle unk_dc;
    Uint8 unk_e0[4];
    Sint32 garden; /* a garden's own tree's CHAO_AREA_, or 0 */
    struct garden_tree_save *save;
    NJS_TEXLIST *texture;
    /* model is a garden's own tree's, and crown the Dark Garden's crown, or
     * null. copy is a planted tree's own copy of its sapling model, which
     * the blends reshape, or null */
    NJS_CNK_OBJECT *model, *crown, *copy;
};

/* the trees count their fruit into one of tree_fruit_counts while the other
 * holds the last pass's count */
extern Sint32 tree_fruit_counts[2], tree_fruit_node_index;
extern Angle tree_branch_phases[5], tree_fruit_phases[4];
/* the fruit on the trees, as the last pass counted them, and the garden's
 * fruit objects */
int garden_active_fruit_count(void);
/* the trees whose stage isn't TREE_STAGE_EMPTY, the garden's own included */
int garden_tree_count(void);
/* the trees from TREE_STAGE_SEED to TREE_STAGE_MATURE */
int garden_planted_tree_count(void);
/* the empty plots, a withering or dead tree's included */
int garden_free_tree_spots(void);
/* write tree back to the record garden_plant_tree took. the record keeps a
 * sapling's growth in percent, or a grown tree's life in hundreds, in its
 * growth. it keeps each fruit's growth in percent */
void garden_pack_tree(struct task *tree);
/* start the next pass's fruit count. call it once a frame */
void garden_advance_fruit_counts(void);
/* make the tree save describes at position, 1.1 lower, and keep save for
 * garden_pack_tree. a kind of 0 or a stage of TREE_STAGE_DEAD sets save's
 * stage to TREE_STAGE_EMPTY. TREE_KIND_PERMANENT sets it to
 * TREE_STAGE_PERMANENT. a permanent tree takes the current area's model and
 * fruit */
struct task *garden_plant_tree(NJS_POINT3 *position,
                               struct garden_tree_save *save);

#endif
