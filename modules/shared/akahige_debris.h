/* Akahige carries three or six bombs, with its own models for each kind. */
#ifndef SHARED_AKAHIGE_DEBRIS_H
#define SHARED_AKAHIGE_DEBRIS_H

#include <ninja.h>

#include "shared/hornet_art.h"

static NJS_CNK_MODEL *hornet_3_pieces[5] = {
#include "shared/akahige_debris/hornet_3_pieces.inc"
};
static NJS_CNK_MODEL *hornet_6_pieces[5] = {
#include "shared/akahige_debris/hornet_6_pieces.inc"
};

#endif
