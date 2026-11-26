#ifndef SHARED_CAPSULE_BOMB_ART_BUILD_STATE_H
#define SHARED_CAPSULE_BOMB_ART_BUILD_STATE_H

#include <sg_xpt.h>

#include "shared/common_object.h"

/* the index of each model's build in capsule_bomb_builds */
enum capsule_bomb_build_index {
    BUILD_CAPSULE,
    BUILD_TOP_CAP,
    BUILD_BOTTOM_CAP,
    BUILD_UPPER_HALF,
    BUILD_LOWER_HALF,
    BUILD_SHARD_A,
    BUILD_SHARD_B,
    BUILDS
};

/* TRUE once one call to capsule_bomb_build has compiled all seven builds */
static Bool capsule_bomb_built = {
#include "shared/capsule_bomb_art/build_state/capsule_bomb_built.inc"
};
static struct model_build capsule_bomb_builds[BUILDS] = {
#include "shared/capsule_bomb_art/build_state/capsule_bomb_builds.inc"
};
/* what the damage's radius gains a step */
static Float damage_growth = {
#include "shared/capsule_bomb_art/build_state/damage_growth.inc"
};
/* the quakes' powers, at the blast and after it */
static Float shake_power = {
#include "shared/capsule_bomb_art/build_state/shake_power.inc"
};
static Float aftershock_power = {
#include "shared/capsule_bomb_art/build_state/aftershock_power.inc"
};

#endif
