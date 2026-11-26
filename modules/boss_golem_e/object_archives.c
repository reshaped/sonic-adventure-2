/* the PVMs that texture_load_shared loads for the fight, and its objects'
 * texture lists. each list takes its textures from the PVMs by name */
#include "boss_golem_e/object_archives/text.inc"
#include <ninja.h>

#include "shared/common_pvm_textures.h"
#include "boss_golem_e/attachment_art.h"
#include "boss_golem_e/golem.h"
#include "shared/golem_archive_group.h"
#include "shared/golem_archives.h"
#include "system/texture.h"

NJS_TEXLIST **golem_texture_groups[24] = {
#include "boss_golem_e/object_archives/golem_texture_groups.inc"
};

struct texture_pvm golem_texture_pvms[6] = {
#include "boss_golem_e/object_archives/golem_texture_pvms.inc"
};
