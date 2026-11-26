/* the laser barrier of Cannon's Core. it sets its contact sound's volume to
 * contact_volume */
#include <ninja.h>
#include "collision/collision.h"
#include "shared/core_laser.h"
#include "shared/core_laser/text.inc"
#include "shared/core_laser_names.h"

NJS_TEXLIST core_laser_texlist = {
#include "shared/core_laser/core_laser_texlist.inc"
};

#include "shared/core_laser_art.h" /* IWYU pragma: keep -- the data it defines here */

NJS_TEXLIST *core_laser_texlists[2] = {
#include "shared/core_laser/core_laser_texlists.inc"
};

static struct collision_shape contact_shape[1] = {
#include "shared/core_laser/contact_shape.inc"
};
static struct collision_shape barrier_shape[1] = {
#include "shared/core_laser/barrier_shape.inc"
};

static int contact_volume = {
#include "shared/core_laser/contact_volume.inc"
};

#include "shared/core_laser_impl.h" /* IWYU pragma: keep -- the code it compiles here */
