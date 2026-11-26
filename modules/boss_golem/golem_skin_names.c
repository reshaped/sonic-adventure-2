/* BOSS_GOLEM.PRS's texture filenames, from the disc. its C lies between
 * object_textures.c and flash_art.c */
#include "shared/golem_statue/materials.h"

const char golem_skin_names[GOLEM_TEXTURE_GL_BODY07 + 1]
                           [GOLEM_BODY_NAME_LENGTH] = {
#include "boss_golem/golem_skin_names/golem_skin_names.inc"
};
