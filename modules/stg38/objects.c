/* the kinds of object that the set files of Knuckles's route through Cannon's
 * Core name */
#include "object/set.h"
#include "shared/core_objects.h"
#include "stg38/objects/text.inc"

/* The generated SET roster names these actor constructors. */
#include "shared/core_roster.h" /* IWYU pragma: keep -- names the bodies take */
#include "shared/beetle.h"
#include "shared/big_the_cat.h"
#include "shared/core_block.h"
#include "shared/core_goal.h"
#include "shared/core_laser.h"
#include "shared/core_step.h"
#include "shared/enemy_ai.h"
#include "shared/enemy_chaos.h"
#include "shared/enemy_emi.h"
#include "shared/enemy_kyoko.h"
#include "shared/global_light_switch.h"
#include "shared/gold_beetle.h"
#include "shared/hornet.h"
#include "shared/iron_ball.h"
#include "shared/light_switch.h"
#include "shared/sg_item_box.h"
#include "shared/time_switch.h"
#include "shared/water_bubbles.h"
#include "shared/weight.h"
#include "stg38/current.h"

struct object_kind core_knuckles_kinds[84] = {
#include "stg38/objects/core_knuckles_kinds.inc"
};
