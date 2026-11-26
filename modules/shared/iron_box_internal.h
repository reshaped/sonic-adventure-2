/* Declarations shared by both iron box actors. */
#ifndef SHARED_IRON_BOX_INTERNAL_H
#define SHARED_IRON_BOX_INTERNAL_H

#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "hud/hud.h"
#include "object/animal.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/debris.h"
#include "shared/dust.h"
#include "shared/iron_box.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

/* the box is 2 * HALF_SIZE on a side, and its pos is the center of its base */
#define HALF_SIZE 10.0F

/* a box's modes, in its work's submode. a MODE_BROKEN box's pieces have yet
 * to fly. a MODE_GONE box's pieces have flown */
enum iron_box_mode { MODE_WHOLE, MODE_BROKEN, MODE_GONE };

/* a broken box's timer counts its steps from 0 at the break. the boxes above it
 * drop on the step the timer is DROP_STEP. its pieces fly once the timer
 * passes PIECES_STEP */
#define DROP_STEP 1
#define PIECES_STEP 2

/* the height of the floor under the box, kept in task->extra in place of a
 * pointer. task_destroy frees a non-null extra, so the destructor clears extra
 * first */
/* @hack type: extra read as a Float through its address, where (task)->extra.f
 * would do */
#define FLOOR(task) (*(Float *)&(task)->extra)
/* FLOOR(task) is NO_FLOOR of collision/surface.h until the box finds a floor.
 * a whole box looks for one every FLOOR_STEPS steps, a power of 2, from
 * FLOOR_LOOK_HEIGHT above its pos */
#define FLOOR_STEPS 32
#define FLOOR_LOOK_HEIGHT 5.0F

/* a stacked box's work holds its fall in scl, not a scale. FALL_LEFT is the
 * height left to fall, which stack_drop raises. FALL_SPEED is the box's speed
 * in y, negative while it falls */
#define FALL_LEFT(work) ((work)->scl.y)
#define FALL_SPEED(work) ((work)->scl.z)
#define FALL_DRAG 0.99F
#define FALL_GRAVITY 0.08F
/* a box that lands within DUST_HEIGHT of its floor raises puffs along its
 * base's edges */
#define DUST_HEIGHT 5.0F
#define LAND_PUFF_SCALE 8.0F
#define LAND_PUFF_DENSITY 1.2F
#define LAND_PUFF_SPEED 0.5F

#define BREAK_SOUND 0x1011
#define BREAK_PRIORITY 1
#define BREAK_SOUND_DURATION 80
#define BREAK_POINTS 20
#define BREAK_PUFFS 14
#define BREAK_PUFF_RADIUS 7.0F
#define BREAK_PUFF_RADIUS_RANGE 3.0F
#define BREAK_PUFF_SPEED 0.5F
#define BREAK_PUFF_SCALE 2.5F
#define BREAK_PUFF_SCALE_RANGE 4.0F

/* whether player 0's upgrades put SHAPE_HURT_NO_PUSH in the box's shape */
#define NO_PUSH(upgrades)                                                      \
    (((upgrades) & (UPGRADE_BIT(UPGRADE_KNUCKLES_GROVES) |                     \
                    UPGRADE_BIT(UPGRADE_ROUGE_BOOTS))) ||                      \
     (((upgrades) & UPGRADE_BIT(UPGRADE_SONIC_RUBBERUNIT)) &&                  \
      ((upgrades) & (UPGRADE_BIT(UPGRADE_SONIC_FLAMERING) |                    \
                     UPGRADE_BIT(UPGRADE_SHADOW_FLAMERING)))))

/* the work's tick holds the low byte of ang.z, the box's animal byte of
 * object/animal.h. its ANIMAL_CHANCE_BITS hold the chance, in percent, that
 * the box releases its animal. a chance of ANIMAL_CERTAIN or more is
 * certain */
#define ANIMAL(work) ((work)->tick)
#define ANIMAL_CERTAIN 10
/* the odds of the animal are its chance, in percent, times TENTH twice */
#define TENTH 0.1F

/* a bit of ang.x: the box's pieces fall through its floor */
#define PIECES_FALL_THROUGH 0x1
/* a piece flies at a speed from PIECE_SPEED up to PIECE_SPEED +
 * PIECE_SPEED_RANGE */
#define PIECE_SPEED 0.7F
#define PIECE_SPEED_RANGE 1.2F

/* a kind of piece a box breaks into, and how many of it */
struct iron_box_piece {
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    Sint32 count;
};

#define IRON_BOX_PIECES 3
/* iron_box_builds holds the box's model at BOX_BUILD, then from PIECE_BUILD
 * each of iron_box_pieces' models, in that array's order */
#define BOX_BUILD 0
#define PIECE_BUILD 1
#define IRON_BOX_BUILDS (PIECE_BUILD + IRON_BOX_PIECES)
#define IRON_BOX_SHAPES 1

#endif
