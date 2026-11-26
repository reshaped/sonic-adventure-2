#include <ninja.h>
#include <sg_xpt.h>
#include "chaostgrace/props.h"

const Sint32 race_neutral_prop_count = RACE_NEUTRAL_PROPS;
const Sint32 race_hero_prop_count = RACE_HERO_PROPS;
const Sint32 race_dark_prop_count = RACE_DARK_PROPS;

NJS_TEXANIM race_prop_glow_cells[2] = {
#include "chaostgrace/scenery_art/race_prop_glow_cells.inc"
};

/* the labels of an object editor's fields, which no code reads. each array
 * rounds its string up to a multiple of 4 bytes */
struct scenery_editor_labels {
    char model[28], yaw[20], angle_z[12], scale[28];
};
static const struct scenery_editor_labels scenery_editor_labels = {
#include "chaostgrace/scenery_art/scenery_editor_labels.inc"
};
