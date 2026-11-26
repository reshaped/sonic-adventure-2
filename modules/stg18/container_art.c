#include "stg18/container_art.h"

#include "stg18/container_art/text.inc"

/* the SET editor's labels for the container's placement, which no code reads.
 * each label's size starts what follows it on a 4-byte boundary */
static const char container_edit_crash_through[24] = "<- CRASHMODE(THROUGH)";
static const char container_edit_crash_reflect[24] = "<- CRASHMODE(REFRECT)";
static const char container_edit_yaw[12] = "<- ANGLE Y";
static const char container_edit_animal[28] = "<- MINIMAL AP(  ) LEV( )";
static const char container_edit_variant[16] = "<- BOX(      )";

const struct container_texture_filenames lagoon_container_texture_names = {
#include "stg18/container_art/lagoon_container_texture_names.inc"
};
const struct container_variant_labels lagoon_container_variant_names = {
    TEXT_1,
    TEXT_2,
};
