/* BOSS_GOLEM_E.PRS's texture filenames, from the disc. its C lies between
 * object_textures.c and weak_point_art.c */
#include "shared/golem_statue/materials.h"

const char golem_skin_names[GOLEM_TEXTURE_GL_BODY07 + 1]
                           [GOLEM_BODY_NAME_LENGTH] = {
#include "boss_golem_e/golem_skin_names/golem_skin_names.inc"
};
