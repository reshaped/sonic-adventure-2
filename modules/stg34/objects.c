/* the kinds of object that the set files of Sonic's route through Cannon's Core
 * name */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg34/objects/text.inc"

/* The generated SET roster names these actor constructors. */
#include "shared/core_roster.h" /* IWYU pragma: keep -- names the bodies take */
#include "shared/big_the_cat.h"
#include "shared/core_block.h"
#include "shared/core_laser.h"
#include "shared/core_shutter.h"
#include "shared/core_step.h"
#include "shared/enemy_1000.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_chaos.h"
#include "shared/ghost.h"
#include "shared/global_light_switch.h"
#include "shared/gold_beetle.h"
#include "shared/hornet.h"
#include "shared/iron_ball.h"
#include "shared/lens_flare.h"
#include "shared/light_switch.h"
#include "shared/monkey.h"
#include "shared/no_input.h"
#include "shared/path_enemy.h"
#include "shared/switch_ring_group.h"
#include "shared/time_switch.h"
#include "shared/weight.h"
#include "stg34/electric_pillar.h"

struct object_kind core_sonic_kinds[83] = {
#include "stg34/objects/core_sonic_kinds.inc"
};
