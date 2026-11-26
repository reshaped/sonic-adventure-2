/* Street bench in Mission Street. */
#ifndef SHARED_STREET_FIXTURES_STREET_BENCH_H
#define SHARED_STREET_FIXTURES_STREET_BENCH_H

#include <ninja.h>
#include <sg_xpt.h>

static NJS_TEXNAME street_bench_textures[4] = {
#include "shared/street_fixtures/street_bench/street_bench_textures.inc"
};

static NJS_TEXLIST street_bench_texlist = {
#include "shared/street_fixtures/street_bench/street_bench_texlist.inc"
};

static Sint16 street_bench_polygons[319] = {
#include "shared/street_fixtures/street_bench/street_bench_polygons.inc"
};

static Sint32 street_bench_vertices[315] = {
#include "shared/street_fixtures/street_bench/street_bench_vertices.inc"
};

static NJS_CNK_MODEL street_bench_mesh = {
#include "shared/street_fixtures/street_bench/street_bench_mesh.inc"
};

static NJS_CNK_OBJECT street_bench_object = {
#include "shared/street_fixtures/street_bench/street_bench_object.inc"
};

#endif
