/* the meshes each animal gives a Chao's parts, for a child and for an adult,
 * by ANIMAL_ kind. a null mesh leaves its node's own. chao_medal_models
 * holds each medal's model by number. medal 0 is none, and draws nothing */
#ifndef CHAO_PARTS_ART_H
#define CHAO_PARTS_ART_H

#include <ninja.h>

#include "object/animal.h"

extern NJS_CNK_OBJECT *chao_medal_models[32];
extern NJS_CNK_OBJECT **chao_adult_part_models[ANIMAL_KINDS];
extern NJS_CNK_OBJECT **chao_child_part_models[ANIMAL_KINDS];

#endif
