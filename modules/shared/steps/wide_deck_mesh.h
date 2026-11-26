/* Render streams shared by Eternal Engine and Cannon's Core. */
#ifndef SHARED_STEPS_WIDE_DECK_MESH_H
#define SHARED_STEPS_WIDE_DECK_MESH_H

static Sint16 deck_polygons[1047] = {
#include "shared/steps/wide_deck_mesh/deck_polygons.inc"
};

static Sint32 deck_vertices[967] = {
#include "shared/steps/wide_deck_mesh/deck_vertices.inc"
};

#endif
