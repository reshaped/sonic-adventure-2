/* the effects and effect textures of Sonic and Shadow, and of Amy and Metal
 * Sonic in their place */
#ifndef EFFECT_SONIC_EFFECT_H
#define EFFECT_SONIC_EFFECT_H
#include <ninja.h>
#include <sg_xpt.h>

/* every effect here but a jet draws with an effect texture list. a player
 * whose character is PLAYER_SONIC takes S_EFFTEX's, and any other
 * SH_EFFTEX's. each call loads its file into its list. a task named ManTex
 * in level 0 holds the list, and its destructor releases it. a call does
 * nothing while the list's task lives. AMY_EFFTEX loads into S_EFFTEX's list,
 * under S_EFFTEX's task. The port leaves the slot empty when the archive
 * or its holder is unavailable, so a later load can retry. */
void s_efftex_load(void);
void sh_efftex_load(void);
void amy_efftex_load(void);

/* each call from here to spin_charge_light_create makes an effect of the
 * player in slot player. the effect is a task named exec in level 3. nothing
 * is made where the heap can't give the task or its record. the slot must
 * hold a player as the call is made, unchecked */

/* a ball: the character's ball model, drawn without fog in its first or second
 * ball texture, at random. Sonic's model and textures serve a PLAYER_SONIC, and
 * Shadow's any other. a ball shrinks a little each step, and ends once its
 * scale is 0 or less.
 *
 * a jitter ball and a follow ball lie a little off the player's point, in a
 * random direction. each moves with the point. the point is unk_218 of a mech's
 * record, and unk_230 of any other's. a jitter ball turns as the player does,
 * and its draws read the slot's work and record, unchecked. a follow ball turns
 * as the player did a step before.
 *
 * the other balls stay where the center of the player's first collision shape
 * lay as they started. a large ball starts the largest. a brief ball stays
 * whole for a step. it shrinks at each of the next three, and the third ends
 * it. a follow ball, a shrink ball and a brief ball draw only while the slot
 * has a work */
void ball_jitter_create(int player);
void ball_follow_create(int player);
void ball_shrink_create(int player);
void ball_brief_create(int player);
void ball_large_create(int player);

/* a dash: the character's streak behind the player. it runs from the player's
 * center back to where the center lay as the dash started. the center is that
 * of the player's first collision shape. the steps it has taken pick its
 * texture, s_dash01 to s_dash23 for a PLAYER_SONIC. the dash ends after 20
 * steps. it draws a second set, and ends after 19, where
 * by_character.sonic.dash_steps lacks SONIC_DASH_STEP(1) or
 * SONIC_DASH_STEP(2). it ends at once where the Shinobi heap can't give its
 * copy of the streak. it ends at its first step where the center has moved 40
 * or more. that step reads the slot's task, and each draw the slot's record,
 * unchecked */
void dash_create(int player);

/* a spark of the flame ring's: a 32 by 32 sprite of fire_ss02 to fire_ss08. it
 * shows each texture for two steps. the spark starts within 1.5 of the record's
 * unk_230 along each axis. its scale starts from 0.12 to 0.2. it swells by 1.5%
 * a step. it drifts against the player's motion at 0.3. each part of the drift
 * then gains 0.05 to 0.1, and the drift slows by 5% a step. the spark ends once
 * its timer, which starts from 0 to 3, reaches 14. the motion's spd must not be
 * the zero vector, and each draw reads the slot's record, unchecked */
void flame_spark_create(int player);

/* a light of the sparkle upgrade's, about a player that charges a spin
 * dash. its head circles the player's middle, and lays a trail of 32 pieces,
 * two a step. the middle lies halfway from the player's pos to the center of
 * its first collision shape. the light starts 25 up to 40 from it, on a
 * circle that shrinks. once the trail is laid, it fades, and the light ends.
 * while it lays the trail, its ball of radius 2 attacks items and enemies at
 * level 1. a light laying its trail ends once the slot has no work. the slot's
 * work must have a collision record, unchecked. the light's own work has the id
 * WORK_ID_PLAYER_PROXY, so player_slot_of gives the player's slot */
void sparkle_light_create(int player);
/* a light of a light attack's charge: sparkle_light_create's light without the
 * ball. its work's id is 0. it starts 3 up to 4.5 from the player's middle, on
 * a circle that grows. the middle rises against world_gravity. a Shadow's turns
 * the other way */
void spin_charge_light_create(int player);

/* the Chaos Control of the player in slot player: the character's rings. they
 * draw from frame 16 of the player's animation on. they lie at the record's
 * unk_224 for an Amy, unk_1f4 for a Shadow and unk_1e8 for any other. at
 * each whole frame it inverts the screen at half brightness. a character
 * other than PLAYER_SONIC and PLAYER_SHADOW draws nothing. the effect is a
 * task named SonicEffectChaosControlEffectExecutor in level 0, made only
 * where the heap gives it. it ends at its 62nd step, or once the player has
 * missed. the slot must hold a player while it lasts, unchecked */
void chaos_control_create(int player);
/* a flash of the Chaos Control at pos: the character's rings, and a glow at
 * each whole frame. the glow faces the first camera, at a random scale. the
 * flash is a task named exec in level 3. it is made only where the heap gives
 * it and its record. it ends at its 62nd step, or once the player in slot
 * player has missed. for a null pos, the flash's mode is left as the heap gave
 * it. a mode of 0 follows the record's unk_230, and any other draws at a place
 * nothing set. a character other than PLAYER_SONIC and PLAYER_SHADOW draws
 * nothing. the slot must hold a player while the flash lasts, unchecked */
void chaos_control_flash_create(int player, NJS_POINT3 *pos);

/* metal_jet_create's kinds. the flame's life starts at 1, and each step takes
 * its fade. JET_SLOW fades by 0.19 a step, and is 0.04 up to 0.06 in size.
 * JET_FAST fades by 0.14, at 0.07 up to 0.094. JET_LASTING, which no call
 * makes, and any other kind fade slowest, by 0.1, at 0.05 up to 0.07.
 * JET_SCALED fades by 0.25, at the call's scale times 0.07 up to 0.085. Metal
 * Sonic's run makes JET_FAST where the stick's strength is above 0.9, and
 * JET_SLOW otherwise. his step makes JET_SCALED */
#define JET_SLOW 0
#define JET_FAST 1
#define JET_LASTING 2
#define JET_SCALED 3

/* make a flame of the jet of Metal Sonic in slot player. the flame is the
 * slot's task's child. where the slot holds no task, it is a task named
 * MSJetExecutor in level 3. the flame is a quad that faces the camera, added to
 * what lies behind it. it shows one of textures 8 to 11 of the record's
 * by_character.sonic.texlist, at random. it lies at the record's unk_230 plus
 * an offset, which each step lays along unk_2cc and lengthens. its color
 * darkens and its alpha falls as its life fades. it keeps its offset and its
 * life while game_state.time_stopped has bit player set. each draw still takes
 * it along with unk_230. the step that finds its life at 0.05 or below destroys
 * it. a step that finds game_state.winner_posed at 1 kills it. a flame without
 * a parent also ends once placement_try_leave says it has left range. it is
 * made only where the heap gives it. it reads through two nulls where the slot
 * holds no task: the slot's task and its own parent */
void metal_jet_create(int player, int kind, Float scale);

/* draw the flame ring's fire at the player in slot player, turned as the
 * player is, without fog. game_state's steps animate its textures. it
 * leaves the source blend at NJD_COLOR_BLENDING_SRCALPHA and the
 * destination's at NJD_COLOR_BLENDING_INVSRCALPHA. the slot must hold a
 * player, unchecked. matrix, which Sonic's draw passes, isn't read */
void flame_ring_draw(int player, NJS_MATRIX *matrix);

#endif
