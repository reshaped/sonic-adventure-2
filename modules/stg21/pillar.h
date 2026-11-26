/* PILLAR in Sand Ocean's object list: a pillar that falls once the dynamite
 * holding it up is gone */
#ifndef STG21_PILLAR_H
#define STG21_PILLAR_H
#include <ninja.h>

struct task;

/* the pillar's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *ocean_pillar_texlists[];
/* the object list's step for PILLAR. it does nothing where task leaves at its
 * placement's range. where task_create_child can't give it the shaft, a child
 * task, it kills task. otherwise it gives task the pillar's step, draw and
 * destructor. it puts the pillar's collision object in the world as
 * WORLD_SOLID, in an object world_pool_alloc lends. where the pool has none
 * to lend, the init writes through null. each later step reads through null
 * too.
 *
 * task needs a work, whose pos places the pillar and whose ang.y turns it.
 * each later step tests the range as the first did. the low byte of ang.z is
 * the id of the DYNAMITE charges that hold the pillar up. once
 * ocean_charge_count counts none of them, a blast goes off and the shaft
 * falls. the shaft hurts the players it touches as it lands. the placement
 * keeps the fall, so a pillar made again lies fallen.
 *
 * while game_state.unk_09 is set, the step counts no charges. the pillar then
 * falls without a blast where any of ang.x's low four bits is set. its
 * placement doesn't keep that fall */
void ocean_pillar_init(struct task *task);
extern NJS_CNK_MODEL pillar_broken_base;
extern NJS_TEXLIST pillar_broken_base_texlist;
extern NJS_CNK_MODEL pillar_rubble_0;
extern NJS_CNK_MODEL pillar_rubble_1;
extern NJS_TEXLIST pillar_rubble_texlist;
extern NJS_CNK_MODEL pillar_shaft;
extern NJS_TEXLIST pillar_shaft_texlist;
extern NJS_CNK_MODEL pillar_stone_0;
extern NJS_CNK_MODEL pillar_stone_1;
extern NJS_CNK_MODEL pillar_stone_2;
extern NJS_TEXLIST pillar_stone_texlist;
extern NJS_CNK_MODEL pillar_whole_base;
extern NJS_TEXLIST pillar_whole_base_texlist;
/* @hack type: the two collisions and the motion declared as arrays of one,
 * where each alone would do */
extern NJS_OBJECT pillar_fallen_collision[1];
extern NJS_MOTION pillar_motion[1];
extern NJS_OBJECT pillar_upright_collision[1];

#endif
