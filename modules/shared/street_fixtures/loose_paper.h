/* Loose paper in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_LOOSE_PAPER_H
#define SHARED_STREET_FIXTURES_LOOSE_PAPER_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME loose_paper_textures[1] = {
#include "shared/street_fixtures/loose_paper/loose_paper_textures.inc"
};

static NJS_TEXLIST loose_paper_texlist = {
#include "shared/street_fixtures/loose_paper/loose_paper_texlist.inc"
};

static Sint16 loose_paper_polygons[29] = {
#include "shared/street_fixtures/loose_paper/loose_paper_polygons.inc"
};

static Sint32 loose_paper_vertices[23] = {
#include "shared/street_fixtures/loose_paper/loose_paper_vertices.inc"
};

static NJS_CNK_MODEL loose_paper_model = {
#include "shared/street_fixtures/loose_paper/loose_paper_model.inc"
};

static NJS_CNK_OBJECT loose_paper_object = {
#include "shared/street_fixtures/loose_paper/loose_paper_object.inc"
};

#endif
