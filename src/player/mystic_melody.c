/* the flute's vertices carry colors, whose alphas flute_draw sets to fade
 * it. the two notes' vertices carry normals instead. all three models share
 * one material: opaque gray, added to what lies behind by alpha */
#include <ninja.h>
#include <sg_xpt.h>

#include "player/mystic_melody.h"

static Sint16 flute_polygons[27] = {
#include "player/mystic_melody/flute_polygons.inc"
};

static Sint32 flute_vertices[27] = {
#include "player/mystic_melody/flute_vertices.inc"
};

static NJS_CNK_MODEL mystic_flute_model = {
#include "player/mystic_melody/mystic_flute_model.inc"
};

NJS_CNK_OBJECT mystic_flute_object = {
#include "player/mystic_melody/mystic_flute_object.inc"
};

static Sint16 note_even_polygons[41] = {
#include "player/mystic_melody/note_even_polygons.inc"
};

static Sint32 note_even_vertices[147] = {
#include "player/mystic_melody/note_even_vertices.inc"
};

NJS_CNK_MODEL mystic_note_even_model = {
#include "player/mystic_melody/mystic_note_even_model.inc"
};

static NJS_CNK_OBJECT mystic_note_even_object = {
#include "player/mystic_melody/mystic_note_even_object.inc"
};

static Sint16 note_odd_polygons[41] = {
#include "player/mystic_melody/note_odd_polygons.inc"
};

static Sint32 note_odd_vertices[147] = {
#include "player/mystic_melody/note_odd_vertices.inc"
};

NJS_CNK_MODEL mystic_note_odd_model = {
#include "player/mystic_melody/mystic_note_odd_model.inc"
};

static NJS_CNK_OBJECT mystic_note_odd_object = {
#include "player/mystic_melody/mystic_note_odd_object.inc"
};

static NJS_MKEY_F mystic_flute_position[64] = {
#include "player/mystic_melody/mystic_flute_position.inc"
};

static NJS_MKEY_A mystic_flute_rotation[64] = {
#include "player/mystic_melody/mystic_flute_rotation.inc"
};

static NJS_MDATA2 mystic_flute_channels = {
#include "player/mystic_melody/mystic_flute_channels.inc"
};

NJS_MOTION mystic_flute_motion = {
#include "player/mystic_melody/mystic_flute_motion.inc"
};

static NJS_MKEY_F eggman_mech_flute_position[64] = {
#include "player/mystic_melody/eggman_mech_flute_position.inc"
};

static NJS_MKEY_A eggman_mech_flute_rotation[64] = {
#include "player/mystic_melody/eggman_mech_flute_rotation.inc"
};

static NJS_MDATA2 eggman_mech_flute_channels = {
#include "player/mystic_melody/eggman_mech_flute_channels.inc"
};

NJS_MOTION eggman_mech_flute_motion = {
#include "player/mystic_melody/eggman_mech_flute_motion.inc"
};

static NJS_MKEY_F tails_mech_flute_position[52] = {
#include "player/mystic_melody/tails_mech_flute_position.inc"
};

static NJS_MKEY_A tails_mech_flute_rotation[64] = {
#include "player/mystic_melody/tails_mech_flute_rotation.inc"
};

static NJS_MDATA2 tails_mech_flute_channels = {
#include "player/mystic_melody/tails_mech_flute_channels.inc"
};

NJS_MOTION tails_mech_flute_motion = {
#include "player/mystic_melody/tails_mech_flute_motion.inc"
};
