/* BLOCK and BLOCK2, the blocks of Cannon's Core that slide from one portal
 * into another. BLOCK slides along its heading, and BLOCK2 up or down */
#ifndef SHARED_CORE_BLOCK_H
#define SHARED_CORE_BLOCK_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST core_block_body_texlist;
extern NJS_TEXLIST core_block_shadow_texlist;
extern NJS_TEXLIST core_block_portal_texlist;
/* BLOCK's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *core_block_texlists[];
extern NJS_TEXLIST core_lift_lower_texlist;
extern NJS_TEXLIST core_lift_upper_texlist;
/* BLOCK2's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *core_lift_texlists[];

extern NJS_CNK_MODEL core_block_body;
extern NJS_CNK_MODEL core_block_flash_early;
extern NJS_CNK_MODEL core_block_flash_late;
extern NJS_CNK_MODEL core_block_shadow;
extern NJS_CNK_MODEL core_block_glow[1];
extern NJS_CNK_MODEL core_block_portal;
extern NJS_CNK_MODEL core_block_modifier;
extern NJS_OBJECT core_block_land_node;
extern NJS_CNK_MODEL core_lift_lower_portal;
extern NJS_CNK_MODEL core_lift_upper_portal;
extern NJS_CNK_MODEL core_lift_lower_glow;
extern NJS_CNK_MODEL core_lift_upper_glow;

/* the object list's steps for BLOCK and BLOCK2. each keeps the placement's
 * pos, the middle between the two portals, in task's extra. it moves pos to
 * the block's place in its cycle, and does no more where
 * placement_try_leave(task) returns TRUE. otherwise it gives task the
 * block's step, draw and destructor. task needs a work, a force and an
 * extra. task's motion takes a copy of core_block_land_node from the world's
 * pool, unchecked. the copy is a moving solid in the world's collision.
 *
 * scl.z is the distance between the portals. the near portal lies along the
 * turned +z, or below for BLOCK2. scl.x + 1 scales the block and its
 * portals, and ang.y's high byte turns them. the block slides from the near
 * portal into the far one in 4 times ang.x's high byte steps. it then waits
 * inside a portal for 4 times the low byte. an odd scl.y turns the slide the
 * other way. ang.y's low byte puts the block that many 256ths of a cycle
 * ahead of game_state.steps.
 *
 * ang.z's low nibble, mod 3, chooses BLOCK's shadow. 0 is none, 1 the shadow
 * model on the floor below, and 2 a modifier volume. BLOCK2 draws a modifier
 * volume where ang.z's bit 0 is set. bit 4 adds a camera zone about the
 * block. bit 8 leaves the far portal undrawn. bit 12 leaves the near portal
 * undrawn, and both portals without their boxes. a portal's boxes are solid,
 * and one of them hurts a player inside it */
void core_block_init(struct task *task);
void core_lift_init(struct task *task);

#endif
