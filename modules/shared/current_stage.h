/* the stage's side of the currents in side_current.h and rising_current.h.
 * LAGOON_CURRENT takes Dry Lagoon's calls and settings, and otherwise those of
 * Knuckles's route in Cannon's Core */
#ifndef SHARED_CURRENT_STAGE_H
#define SHARED_CURRENT_STAGE_H

#ifdef LAGOON_CURRENT
#include "stg18/current.h"
#include "stg18/lagoon_util.h"
#include "stg18/specks.h"

#define CURRENT_INIT lagoon_current_init
#define CURRENT_POLAR lagoon_polar_xz
#define CURRENT_RUMBLE lagoon_vib_slight_create
#define STREAM_SOUND 6
#define CAN_PUSH (player_work[0]->mode != PLAYER_MODE_ROUGE_RIDE_SWIMMER)
#define SOUND_ACTIVE TRUE
#define UPDRAFT_INIT lagoon_updraft_init
#define ENTRY_POWER 3
#define ENTRY_FREQUENCY 20
#define LOUDEST 25
#define TAPERED_PULSE_FRAME 50.0F
#define UPDRAFT_SOUND_ON(task) ((task)->work->ang.x & 1)
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define PLAYER_HEIGHT_MARK(task) (((Float *)&(task)->unk_3c)[0])
#else
#include "shared/lagoon_specks.h"
#include "shared/stage_util.h"
#include "stg38/current.h"
#include "system/game_state.h"

#define CURRENT_INIT core_current_init
#define CURRENT_POLAR polar_xz
#define CURRENT_RUMBLE vib_slight_create
#define STREAM_SOUND 5
#define CAN_PUSH (!game_state.time_stopped)
#define SOUND_ACTIVE CAN_PUSH
#define UPDRAFT_INIT core_updraft_init
#define ENTRY_POWER 6
#define ENTRY_FREQUENCY 10
#define LOUDEST 20
#define TAPERED_PULSE_FRAME 0.0F
/* @hack type: unk_3c read as a Float through its address, where
 * (task)->unk_3c.f would do */
#define PLAYER_HEIGHT_MARK(task) (*(Float *)&(task)->unk_3c)
#define UPDRAFT_SOUND_ON(task)                                                 \
    (!game_state.time_stopped && ((task)->work->ang.x & 1))
#endif

#endif
