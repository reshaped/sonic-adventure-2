/* the kinds of object that the set files of Eggman's route through Cannon's
 * Core name */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg35/objects/text.inc"

/* The generated SET roster names these actor constructors. */
#include "shared/core_roster.h" /* IWYU pragma: keep -- names the bodies take */
#include "shared/beetle.h"
#include "shared/big_the_cat.h"
#include "shared/capsule_bomb.h"
#include "shared/core_block.h"
#include "shared/core_laser.h"
#include "shared/core_shutter.h"
#include "shared/core_step.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_chaos.h"
#include "shared/enemy_emi.h"
#include "shared/enemy_kyoko.h"
#include "shared/global_light_switch.h"
#include "shared/goal_door.h"
#include "shared/gold_beetle.h"
#include "shared/hornet.h"
#include "shared/iron_ball.h"
#include "shared/last_step.h"
#include "shared/lens_flare.h"
#include "shared/light_switch.h"
#include "shared/path_enemy.h"
#include "shared/shouko.h"
#include "shared/time_switch.h"
#include "shared/weight.h"

struct object_kind core_eggman_kinds[82] = {
#include "stg35/objects/core_eggman_kinds.inc"
};
