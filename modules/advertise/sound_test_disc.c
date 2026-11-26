/* the sound test panel's texture list, and the model of the disc it turns */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/sound_test_layout.h"

#include "advertise/sound_test_disc/text.inc"

static NJS_TEXNAME names[18] = {
#include "advertise/sound_test_disc/names.inc"
};
NJS_TEXLIST menu_sound_test_texlist = {
#include "advertise/sound_test_disc/menu_sound_test_texlist.inc"
};

static Sint16 disc_polygons[1083] = {
#include "advertise/sound_test_disc/disc_polygons.inc"
};

static Sint32 disc_vertices[1539] = {
#include "advertise/sound_test_disc/disc_vertices.inc"
};

static NJS_CNK_MODEL disc_model = {
#include "advertise/sound_test_disc/disc_model.inc"
};

NJS_CNK_OBJECT menu_sound_test_disc_object = {
#include "advertise/sound_test_disc/menu_sound_test_disc_object.inc"
};
