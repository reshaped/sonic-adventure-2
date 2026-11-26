#include "stg11/missile_textures/text.inc"
#include <ninja.h>

#include "stg11/lock_on_missile.h"

/* the set editor's labels for the missile's placement, which no code reads.
 * each label's size starts what follows it on a 4-byte boundary */
static const char angle_label[12] = "<- ANGLE Y";
static const char id_label[8] = "<- ID";
static const char aim_x_label[12] = "<- AIMPOS X";
static const char aim_y_label[12] = "<- AIMPOS Y";
static const char aim_z_label[12] = "<- AIMPOS Z";

static NJS_TEXNAME missile_names[16] = {
#include "stg11/missile_textures/missile_names.inc"
};
NJS_TEXLIST lock_on_missile_texlist = {
#include "stg11/missile_textures/lock_on_missile_texlist.inc"
};
