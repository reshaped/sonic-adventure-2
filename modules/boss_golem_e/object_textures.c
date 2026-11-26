/* the list the fight's own PVM fills. texture_load_shared gives the golem's
 * parts their textures from it by name */
#include <ninja.h>

#include "boss_golem_e/attachment_art.h"
#include "shared/golem_material/names.h"
#include "shared/golem_statue/materials.h"

const struct golem_effect_names golem_effect_materials = {
#include "boss_golem_e/object_textures/golem_effect_materials.inc"
};

static NJS_TEXNAME object_textures[46] = {
#include "boss_golem_e/object_textures/object_textures.inc"
};

NJS_TEXLIST golem_e_object_texlist = {
    object_textures, sizeof(object_textures) / sizeof(object_textures[0])};
