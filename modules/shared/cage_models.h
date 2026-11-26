/* what the cages of shared/cage.c and stg22/cage.c share. each compiles its
 * own CAGE_BUILDS models: the cage's, then its pieces'. once every build has
 * compiled, a cage throws its pieces as the compiled models. otherwise it
 * throws the models themselves */
#ifndef SHARED_CAGE_MODELS_H
#define SHARED_CAGE_MODELS_H
#include <ninja.h>
#include <sg_xpt.h>
#include "shared/cage.h"
#include "shared/object_builds.h"

/* a kind of piece a cage breaks into, and how many of it */
struct cage_piece {
    NJS_TEXLIST *texlist;
    NJS_CNK_MODEL *model;
    Sint32 count;
};

#define CAGE_PIECES 3
#define CAGE_BUILDS (1 + CAGE_PIECES)
#define CAGE_SHAPES 1
#endif
