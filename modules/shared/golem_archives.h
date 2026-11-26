/* the PVMs that texture_load_shared loads for the Egg Golem's two fights, and
 * its objects' texture lists. each list takes its textures from the PVMs by
 * name */
#ifndef SHARED_GOLEM_ARCHIVES_H
#define SHARED_GOLEM_ARCHIVES_H

#include <ninja.h>

#include "system/texture.h"

extern struct texture_pvm golem_texture_pvms[];
extern NJS_TEXLIST **golem_texture_groups[];

#endif
