#include <ninja.h>
#include <sg_xpt.h>

#include "chao/body.h"
#include "chao/life.h"
#include "chao/parts.h"
#include "chao/parts_art.h"
#include "chao/work.h"
#include "object/animal.h"
#include "system/task.h"

#define SLOT_NODES 2
#define NO_NODE (-1)
#define NODE(work, index) ((struct chao_node *)(work)->shape.nodes[(index)])

/* each slot's two body nodes, NO_NODE for none. part_meshes gives the index,
 * in a kind's part models, of the mesh each node shows. a mesh beside NO_NODE
 * is never read */
/* clang-format off */
static Sint32 part_nodes[CHAO_PART_SLOTS][SLOT_NODES] = {
    {3, 10},
    {24, 26},
    {29, NO_NODE},
    {30, 31},
    {6, 13},
    {8, NO_NODE},
    {37, 39},
    {28, NO_NODE},
};
static Sint32 part_meshes[CHAO_PART_SLOTS][SLOT_NODES] = {
    {0, 1},
    {2, 3},
    {4, -1},
    {5, 6},
    {7, 8},
    {9, -1},
    {10, 11},
    {12, -1},
};
/* clang-format on */

Bool chao_child_has_animal_part(int kind, int slot)
{
    if (chao_child_part_models[kind][part_meshes[slot][0]]) {
        return TRUE;
    }
    return FALSE;
}

Bool chao_adult_has_animal_part(int kind, int slot)
{
    if (chao_adult_part_models[kind][part_meshes[slot][0]]) {
        return TRUE;
    }
    return FALSE;
}

static void clear_part_tree(struct chao_node *node)
{
    if (node) {
        node->animal_part = NULL;
        if (node->object.child) {
            clear_part_tree((struct chao_node *)node->object.child);
        }
        if (node->object.sibling) {
            clear_part_tree((struct chao_node *)node->object.sibling);
        }
    }
}

void chao_clear_animal_parts(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    int i;
    for (i = 0; i < CHAO_PART_SLOTS; i++) {
        work->profile.parameters.animal_parts[i] = CHAO_NO_PART;
    }
    clear_part_tree(work->shape.root);
}

int chao_animal_part(struct task *chao, int slot)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    return work->profile.parameters.animal_parts[slot];
}

void chao_set_animal_part(struct task *chao, int slot, int kind)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    int i;
    int node;
    int mesh;

    if (kind >= ANIMAL_KINDS) {
        kind = CHAO_NO_PART;
    }
    if (kind < 0) {
        work->profile.parameters.animal_parts[slot] = CHAO_NO_PART;
    } else {
        work->profile.parameters.animal_parts[slot] = kind;
    }
    for (i = 0; i < SLOT_NODES; i++) {
        node = part_nodes[slot][i];
        mesh = part_meshes[slot][i];
        if (node != NO_NODE) {
            if (kind >= 0) {
                if (work->profile.parameters.type == CHAO_CHILD) {
                    NODE(work, node)->animal_part =
                        chao_child_part_models[kind][mesh];
                } else {
                    NODE(work, node)->animal_part =
                        chao_adult_part_models[kind][mesh];
                }
            } else {
                NODE(work, node)->animal_part = NULL;
            }
        }
    }
}

void chao_remove_animal_part(struct task *chao, int slot)
{
    chao_set_animal_part(chao, slot, CHAO_NO_PART);
}

void chao_set_headgear(struct task *chao, int hat)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    parameters->hat = hat;
}

void chao_remove_headgear(struct task *chao)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    parameters->hat = 0;
}

/* copies of chao_set_headgear and chao_remove_headgear, which nothing calls */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void chao_set_hat(struct task *chao, int hat)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    parameters->hat = hat;
}

/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void chao_remove_hat(struct task *chao)
{
    struct chao_parameters *parameters =
        &((struct chao_work *)chao->work)->profile.parameters;
    parameters->hat = 0;
}

void chao_equip_node(struct task *chao, int node_index, NJS_CNK_OBJECT *object,
                     NJS_TEXLIST *textures)
{
    struct chao_work *work = (struct chao_work *)chao->work;

    if (NODE(work, node_index)->prop.object != object) {
        NODE(work, node_index)->prop.scale = 0.0F;
        NODE(work, node_index)->prop.object = object;
    }
    NODE(work, node_index)->prop.textures = textures;
    NODE(work, node_index)->prop.fade_in = TRUE;
    NODE(work, node_index)->placed = FALSE;
}

void chao_equip_node_at(struct task *chao, int node_index,
                        const struct chao_placement *placement)
{
    struct chao_work *work = (struct chao_work *)chao->work;

    if (NODE(work, node_index)->prop.object != placement->object) {
        NODE(work, node_index)->prop.scale = 0.0F;
        /* @hack redundant: a store of the object that the one below repeats */
        NODE(work, node_index)->prop.object = placement->object;
    }
    NODE(work, node_index)->prop.object = placement->object;
    NODE(work, node_index)->prop.textures = placement->textures;
    NODE(work, node_index)->prop_position = placement->position;
    NODE(work, node_index)->prop_angles = placement->angles;
    NODE(work, node_index)->prop.fade_in = TRUE;
    NODE(work, node_index)->placed = TRUE;
}

void chao_equip_hand_4(struct task *chao, NJS_CNK_OBJECT *object,
                       NJS_TEXLIST *textures)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    work->shape.prop_4.object = object;
    work->shape.prop_4.textures = textures;
    work->shape.prop_4.scale = 0.0F;
    work->shape.prop_4.target_scale = 1.0F;
}

void chao_equip_hand_11(struct task *chao, NJS_CNK_OBJECT *object,
                        NJS_TEXLIST *textures)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    work->shape.prop_11.object = object;
    work->shape.prop_11.textures = textures;
    work->shape.prop_11.scale = 0.0F;
    work->shape.prop_11.target_scale = 1.0F;
}

void chao_fade_node_prop(struct chao_node *node)
{
    node->prop.fade_in = FALSE;
}

static void fade_prop_tree(struct chao_node *node)
{
    if (node) {
        chao_fade_node_prop(node);
        if (node->object.child) {
            fade_prop_tree((struct chao_node *)node->object.child);
        }
        if (node->object.sibling) {
            fade_prop_tree((struct chao_node *)node->object.sibling);
        }
    }
}

void chao_fade_props(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    fade_prop_tree(work->shape.root);
    work->shape.prop_4.target_scale = 0.0F;
    work->shape.prop_11.target_scale = 0.0F;
}

void chao_drop_node_prop(struct chao_node *node)
{
    node->prop.object = NULL;
    node->prop.textures = NULL;
    node->prop.scale = 0.0F;
    node->prop.fade_in = FALSE;
}

static void drop_prop_tree(struct chao_node *node)
{
    if (node) {
        chao_drop_node_prop(node);
        if (node->object.child) {
            drop_prop_tree((struct chao_node *)node->object.child);
        }
        if (node->object.sibling) {
            drop_prop_tree((struct chao_node *)node->object.sibling);
        }
    }
}

void chao_drop_node_props(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    drop_prop_tree(work->shape.root);
}

void chao_restore_animal_parts(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    int i;

    for (i = 0; i < CHAO_PART_SLOTS; i++) {
        chao_set_animal_part(chao, i, work->profile.parameters.animal_parts[i]);
    }
    chao_drop_node_props(chao);
}

void chao_record_eaten_minimal(struct task *chao, int kind)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    work->profile.parameters.eaten_minimals |= 1 << kind;
}

void chao_forget_minimals(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    work->profile.parameters.eaten_minimals = 0;
}

Bool chao_has_eaten_minimal(struct task *chao, int kind)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    if (work->profile.parameters.eaten_minimals & (1 << kind)) {
        return TRUE;
    }
    return FALSE;
}
