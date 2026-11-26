/* DAI in Sand Ocean's and Hidden Base's object lists: a stack of breakable
 * tiers */
#ifndef STG21_STACKED_PLATFORM_H
#define STG21_STACKED_PLATFORM_H
#include <ninja.h>

struct task;

/* the stack's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *stacked_platform_texlists[];

/* the object list's step for DAI. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the stack's step, draw and
 * destructor, and a child task for each tier. where syCalloc has no room for
 * the list of tiers, the init returns with no tiers. the stack's step then
 * reads through null. where task_create_child can't make a tier, the init
 * destroys task.
 *
 * task needs a work, whose pos places the bottom tier and whose ang.y turns
 * the stack. each later step tests the range as the first did. the whole part
 * of |scl.z| modulo 8, plus 1, counts the tiers. the whole part of |scl.x|
 * modulo 2 picks one of the stage's two models for the tiers.
 *
 * a tier breaks down to its base once both its targets are hurt. the tiers
 * above it then fall onto it. the placement's state keeps the broken tiers,
 * two bits a tier from the bottom. its byte holds only the first four tiers.
 * tiers five to eight come back broken exactly when tier four was */
void stacked_platform_init(struct task *task);
/* tint the seven tier_fragment_ models, the pieces a breaking tier throws.
 * the call multiplies their vertex colors by a tint of the stack's own. each
 * call multiplies them again, so it must run once after each load of the
 * models */
void stacked_platform_tint_parts(void);
extern NJS_CNK_MODEL tier_fragment_0;
extern NJS_CNK_MODEL tier_fragment_1;
extern NJS_CNK_MODEL tier_fragment_2;
extern NJS_CNK_MODEL tier_fragment_3;
extern NJS_CNK_MODEL tier_fragment_4;
extern NJS_CNK_MODEL tier_fragment_5;
extern NJS_CNK_MODEL tier_fragment_6;
extern NJS_CNK_MODEL stack_shadow;
extern NJS_TEXLIST stack_shadow_texlist;
extern NJS_CNK_MODEL tier_bright_base;
extern NJS_TEXLIST tier_bright_base_texlist;
extern NJS_CNK_MODEL tier_bright_shell;
extern NJS_TEXLIST tier_bright_shell_texlist;
extern NJS_CNK_MODEL tier_dark_base;
extern NJS_TEXLIST tier_dark_base_texlist;
extern NJS_CNK_MODEL tier_dark_shell;
extern NJS_TEXLIST tier_dark_shell_texlist;
extern NJS_TEXLIST tier_fragment_0_texlist;
extern NJS_TEXLIST tier_fragment_1_texlist;
extern NJS_TEXLIST tier_fragment_2_texlist;
extern NJS_TEXLIST tier_fragment_3_texlist;
extern NJS_TEXLIST tier_fragment_4_texlist;
extern NJS_TEXLIST tier_fragment_5_texlist;
extern NJS_TEXLIST tier_fragment_6_texlist;

#endif
