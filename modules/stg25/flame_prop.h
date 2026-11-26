/* what Death Chamber's flame props share: the fire pot, the fire mouth and the
 * two torches. each draws a flame over its model, in one of
 * stg25/chamber_color.h's colors. Egg Quarters compiles them and its snake pot
 * with EGG_QUARTERS defined, which picks the flame's sound and shape */
#ifndef STG25_FLAME_PROP_H
#define STG25_FLAME_PROP_H
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "player/player.h"
#include "sound/sound.h"
#include "system/task.h"

/* the flame shows the one of its FLAME_FRAMES textures that its task's unk_3c
 * holds. the init picks a frame at random. each step takes the next, back to 0
 * after the last */
#define FLAME_FRAMES 16
#define FLAME_FRAME(task) ((task)->unk_3c.l)
/* FLAME_TID indexes the halfword of the flame model's polygon list that holds
 * its texture's id. it is the second halfword of the list's first NJD_CT_TID
 * chunk. a draw puts the frame there, and keeps the halfword's NJD_FFM_MASK
 * bits. FLAME_TID_BITS are the others */
#define FLAME_TID 7
#define FLAME_TID_BITS 0x3fff

/* the flame adds its colors to what lies behind it, by ADDITIVE_BLEND.
 * ALPHA_BLEND is the blend Ninja's draws take otherwise, which the draw puts
 * back */
#define ADDITIVE_BLEND (NJD_FBS_ONE | NJD_FBD_ONE)
#define ALPHA_BLEND (NJD_FBS_SA | NJD_FBD_ISA)

/* the flame's sound, an id in the stage's sound list. a play of it lasts
 * FLAME_SOUND_STEPS + 1 updates, as sound_play_at_for says */
#ifdef EGG_QUARTERS
#define FLAME_SOUND 0
#else
#define FLAME_SOUND 5
#endif
#define FLAME_SOUND_PRIORITY 1
#define FLAME_SOUND_STEPS 15
/* play the flame's sound at origin while player 0 lies within range of it.
 * the other player's distance doesn't count, even in split screen */
#define PLAY_NEAR_FLAME(task, origin, range, volume)                           \
    do {                                                                       \
        if (njDistanceP2P(&player_work[0]->pos, (origin)) < (range)) {         \
            sound_play_at_for(FLAME_SOUND, task, FLAME_SOUND_PRIORITY, volume, \
                              FLAME_SOUND_STEPS, origin);                      \
        }                                                                      \
    } while (0)

/* the flame's shape hurts players. it pushes no shape of a player, an enemy, an
 * object or a pickup. no shape of a player, an enemy or an object pushes it */
#define FLAME_PUSH PUSH_FIRM(PUSH_ALL_BUT_PICKUPS)
#define FLAME_LEVELS (SHAPE_HURTS_PLAYERS | SHAPE_ATTACK_LEVEL(2))
#ifdef EGG_QUARTERS
#define FLAME_FORM SHAPE_CAPSULE
#define FLAME_SHAPE_FLAGS SHAPE_HURTABLE
#else
#define FLAME_FORM SHAPE_CYLINDER
#define FLAME_SHAPE_FLAGS (SHAPE_HURTABLE | SHAPE_IGNORE_UNK_9)
#endif
/* a prop's body pushes the shapes of every group, and none of theirs pushes
 * it */
#define BODY_PUSH (PUSH_ALL | PUSH_FIRM(PUSH_ALL))

#endif
