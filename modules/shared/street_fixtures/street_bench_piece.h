/* Street bench piece in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STREET_BENCH_PIECE_H
#define SHARED_STREET_FIXTURES_STREET_BENCH_PIECE_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_bench_piece_textures[1] = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_textures.inc"
};

static NJS_TEXLIST street_bench_piece_texlist = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_texlist.inc"
};

static Sint16 street_bench_piece_polygons[103] = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_polygons.inc"
};

static Sint32 street_bench_piece_vertices[71] = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_vertices.inc"
};

static NJS_CNK_MODEL street_bench_piece_model = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_model.inc"
};

static NJS_CNK_OBJECT street_bench_piece_object = {
#include "shared/street_fixtures/street_bench_piece/street_bench_piece_object.inc"
};

#endif
