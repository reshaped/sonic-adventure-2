#ifndef SHARED_BLOCK_PARTS_H
#define SHARED_BLOCK_PARTS_H

#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camset.h"
#include "camera/zones.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "collision/world.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/core_block.h"
#include "shared/core_scenery_art.h"
#include "shared/stage_util.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

struct block_anchor {
    NJS_POINT3 origin;
    struct camera_zone *zone;
};
#define ANCHOR(task) ((struct block_anchor *)(task)->extra.p)
#define RIDERS(task) ((struct task_force *)(task)->force.p)
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define FLOOR_Y(task) (*(Float *)&(task)->unk_3c)

#define HEADING(task) ((task)->work->ang.y & 0xff00)
#define MOVING_STEPS(task) (((task)->work->ang.x & 0xff00) >> 6)
#define WAIT_STEPS(task) (((task)->work->ang.x & 0xff) * 4)
#define BLOCK_SCALE(task) ((task)->work->scl.x + 1.0F)
#define REVERSED(task) ((int)njAbs((task)->work->scl.y) % 2 == 1)
#define SHADOW_KIND(task) (((task)->work->ang.z & 15) % 3)
#define HAS_CAMERA(task) ((((task)->work->ang.z & 0xf0) >> 4) & 1)
#define HIDE_FAR_PORTAL(task) ((((task)->work->ang.z & 0xf00) >> 8) & 1)
#define HIDE_NEAR_PORTAL(task) ((((task)->work->ang.z & 0xf000) >> 12) & 1)
#define BLOCK_PUSH PUSH_SOLID(PUSH_ALL_BUT_PICKUPS)
#define BLOCK_GUARD SHAPE_GUARD_LEVEL(3)
#define PORTAL_SHAPES 6
#define PORTAL_FRAMES 13

static void core_block_destroy(struct task *task);
static void core_block_step(struct task *task);
static void core_block_draw(struct task *task);
static void core_block_contact(struct task *task);
static void core_block_contact_destroy(struct task *task);
static int core_block_flash_period(struct task *task);
static void core_block_flash(struct task *task, NJS_CNK_MODEL *model,
                             int start);
static Float core_block_offset(struct task *task);
#ifdef VERTICAL_BLOCK
static void core_block_portal_glow(NJS_CNK_MODEL *model);
#else
static void core_block_portal_glow(void);
#endif
static int core_block_phase(struct task *task);

#endif
