/* the animal parts and the props on a Chao's body */
#ifndef CHAO_PARTS_H
#define CHAO_PARTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;
struct chao_node;
/* a prop, and its offset and turn from its node */
struct chao_placement {
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *textures;
    NJS_VECTOR position;
    struct angles angles;
};

/* fade out every prop: each node's, and the ones at hands 4 and 11 */
void chao_fade_props(struct task *chao);
/* put a prop on a node, where the node is. a prop new to the node grows from
 * nothing */
void chao_equip_node(struct task *chao, int node_index, NJS_CNK_OBJECT *object,
                     NJS_TEXLIST *textures);
/* put a prop at hand 11, or with chao_equip_hand_4 at hand 4. it grows from
 * nothing */
void chao_equip_hand_11(struct task *chao, NJS_CNK_OBJECT *object,
                        NJS_TEXLIST *textures);
void chao_equip_hand_4(struct task *chao, NJS_CNK_OBJECT *object,
                       NJS_TEXLIST *textures);
/* empty every animal part slot, and take every node's part off */
void chao_clear_animal_parts(struct task *chao);
/* put each slot's part back on the body, and drop every node's prop */
void chao_restore_animal_parts(struct task *chao);

/* whether a kind has a part for a slot among the child's or the adult's
 * part models */
Bool chao_child_has_animal_part(int kind, int slot);
Bool chao_adult_has_animal_part(int kind, int slot);
/* a slot shows its kind's part on up to two body nodes. a child takes the
 * part from the child's models, and any other type from the adult's.
 * setting a kind outside the animal kinds empties the slot */
int chao_animal_part(struct task *chao, int slot);
void chao_set_animal_part(struct task *chao, int slot, int kind);
void chao_remove_animal_part(struct task *chao, int slot);
/* the hat drawn on the Chao's head, or 0 for none */
void chao_set_headgear(struct task *chao, int hat);
void chao_remove_headgear(struct task *chao);
/* chao_equip_node, at placement's offset and turn from the node */
void chao_equip_node_at(struct task *chao, int node_index,
                        const struct chao_placement *placement);
/* fade a node's prop out, or drop it at once. chao_drop_node_props drops
 * every node's */
void chao_fade_node_prop(struct chao_node *node);
void chao_drop_node_prop(struct chao_node *node);
void chao_drop_node_props(struct task *chao);
/* the animal kinds a Chao has eaten, a bit each */
void chao_record_eaten_minimal(struct task *chao, int kind);
void chao_forget_minimals(struct task *chao);
Bool chao_has_eaten_minimal(struct task *chao, int kind);

#endif
