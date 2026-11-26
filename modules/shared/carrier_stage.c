/* Weapons Bed's stage. once model_builds_compile has compiled its sky, the sky
 * draws by njDirectDrawObject. the other carrier stages draw theirs by
 * njCnkSimpleDrawObject */
#include "shared/carrier_stage.h"

#include <sg_xpt.h>

#include "shared/common_object.h"

#include "draw/light.h"

#include "shared/carrier_background_names.h" /* IWYU pragma: keep -- the data it defines here */

static Bool sky_compiled = FALSE;
static struct model_build sky_builds[1] = {
#include "shared/carrier_stage/sky_builds.inc"
};

#include "shared/carrier_background_groups.h" /* IWYU pragma: keep -- the data it defines here */
#include "shared/carrier_water.h" /* IWYU pragma: keep -- the data it defines here */

static struct light deck_lights[2] = {
#include "shared/carrier_stage/deck_lights.inc"
};

#include "shared/carrier_stage_task.h" /* IWYU pragma: keep -- the code it compiles here */
